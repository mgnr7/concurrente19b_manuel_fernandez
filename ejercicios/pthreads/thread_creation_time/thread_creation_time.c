#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* run(void* );

int main (int argc, char*argv[])
{
	size_t pruebas_count;
	pruebas_count = 1;
	
	if(argc >=2)
		pruebas_count = strtoull(argv[1], NULL, 10);
	
	pthread_t* threads = (pthread_t*) malloc(pruebas_count * sizeof(pthread_t)); 	/*malloc retorna un puntero void y reserva (thread_count * sizeof(pthread_t)) espacio en memoria*/
	
	if(threads == NULL)
		return (void)fprintf(stderr, "error: could not allocate memory for %zu threads \n", pruebas_count), 1;
	
	double min_time;
	double act_time;
	size_t min_prueba;
	
	min_time = 50.0;
	
	for(size_t index = 0; index < pruebas_count ; ++index)							/*Primera fase: Creacion de threads*/
	{
		struct timespec start_time;	
		clock_gettime(CLOCK_MONOTONIC, &start_time);
		
		pthread_create(&threads[index], NULL, run, NULL);
		pthread_join(threads[index], NULL); 
		
		struct timespec end_time;
		clock_gettime(CLOCK_MONOTONIC, &end_time);
		
		act_time = end_time.tv_sec - start_time.tv_sec + 1e-9 *(end_time.tv_nsec - start_time.tv_nsec);
		
		if(act_time < min_time){
			min_time = act_time;
			min_prueba = index;
		}
	}
	
	printf("Minimum thread creation and destruction time was %.9lfs among %zu trials\n", min_time, min_prueba);												
	
	free(threads);	
	return 0;
}

void* run(void* arg)
{
	return NULL;
}
