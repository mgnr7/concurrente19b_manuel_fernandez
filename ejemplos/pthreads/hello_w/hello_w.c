#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* run(void* data)
{
	size_t thread_num = (size_t)data; 									
	printf("Hellow World from secondary thread %zu\n", thread_num); 
	return NULL;
}

int main (int argc, char*argv[])
{
	size_t thread_count = sysconf(_SC_NPROCESSORS_ONLN);
	
	if(argc >=2)
	{
		thread_count = strtoull(argv[1], NULL, 10);
	}
	
	pthread_t* threads = (pthread_t*) malloc(thread_count * sizeof(pthread_t)); 	/*malloc retorna un puntero void y reserva (thread_count * sizeof(pthread_t)) espacio en memoria*/
	
	for(size_t index = 0; index < thread_count ; ++index)							/*Primera fase: Creacion de threads*/
		pthread_create(&threads[index], NULL, run, (void*)index);
	
	printf("Hellow World from main thread\n");
	
	for(size_t index = 0; index < thread_count ; ++index)							/*Segunda fase: Espera por cada uno de los threads*/	
		pthread_join(threads[index], NULL); 														
	
	free(threads);	
	return 0;
}

/*Si el thread principal se acaba, se acaba el programa*/
