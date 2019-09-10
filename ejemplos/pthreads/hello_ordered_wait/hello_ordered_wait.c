#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

typedef struct
{
  size_t thread_count;
  size_t next_thread;
} shared_data_t;														/*Compartido entre los threads, puede ser modificado al mismo tiempo*/

typedef struct 
{
	size_t thread_num;
	shared_data_t* shared_data;
} private_data_t;

int create_threads(shared_data_t* shared_data);
void* run(void* data);

int main (int argc, char*argv[])
{
	shared_data_t* shared_data = (shared_data_t*) calloc(1, sizeof(shared_data_t));
	if(shared_data == NULL)
		return (void)fprintf(stderr, "error: could not allocate memory\n"), 2;
	
	shared_data->thread_count = sysconf(_SC_NPROCESSORS_ONLN);
	shared_data->next_thread = 0;
	if(argc >=2)
		shared_data->thread_count = strtoull(argv[1], NULL, 10);
	
	struct timespec start_time;	
	clock_gettime(CLOCK_MONOTONIC, &start_time);						/*Mide el tiempo y lo guarda en el registro*/	
	
	int error = create_threads(shared_data);
	if(error)
		return error;
		
	struct timespec end_time;
	clock_gettime(CLOCK_MONOTONIC, &end_time);
	
	double elapsed_seconds = end_time.tv_sec - start_time.tv_sec + 1e-9 *(end_time.tv_nsec - start_time.tv_nsec);
	printf("Hellow execution time %.9lfs\n", elapsed_seconds);
	
	return 0;
}

/*Si el thread principal se acaba, se acaba el programa*/

int create_threads(shared_data_t* shared_data)
{
    pthread_t* threads = (pthread_t*) malloc(shared_data->thread_count * sizeof(pthread_t)); 	/*malloc retorna un puntero void y reserva (thread_count * sizeof(pthread_t)) espacio en memoria*/
	
	if(threads == NULL)
		return (void)fprintf(stderr, "error: could not allocate memory for %zu threads \n", shared_data->thread_count), 1;
	
	private_data_t* private_data = (private_data_t*) calloc(shared_data->thread_count, sizeof(private_data_t));	/*Calloc reserva memoria y rellena con ceros*/
	
	if(private_data == NULL)
		return (void)fprintf(stderr, "error: could not allocate memory for %zu threads \n", shared_data->thread_count), 3;
		
	
	for(size_t index = 0; index < shared_data->thread_count ; ++index)							/*Primera fase: Creacion de threads*/
	{
		private_data[index].thread_num = index;
		private_data[index].shared_data = shared_data;
		pthread_create(&threads[index], NULL, run, &private_data[index]);
	}
	
	
	for(size_t index = 0; index < shared_data->thread_count ; ++index)							/*Segunda fase: Espera por cada uno de los threads*/	
		pthread_join(threads[index], NULL); 														
	
	printf("Hellow World from main thread\n");
	
	free(private_data);				/*Se elimina en el orden inverso en el que se crea la memoria dinamica*/
	free(shared_data);
	free(threads);	
	return 0;
}

void* run(void* data)
{
	private_data_t* private_data = (private_data_t*)data;
	shared_data_t* shared_data = private_data->shared_data;
	
	size_t thread_num = (*private_data).thread_num;
	size_t thread_count = shared_data->thread_count; 
	while(shared_data->next_thread < thread_num)
			;	/*busy wait*/
			
	printf("Hellow World from secondary thread %zu of %zu\n", thread_num, thread_count); 
	shared_data->next_thread++;
	
	return NULL;
}
