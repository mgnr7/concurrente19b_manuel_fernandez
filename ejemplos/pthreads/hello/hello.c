#include <stdio.h>
#include <pthread.h>

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
    void *(*start_routine)(void*), void *arg);

/*_t : type
 *void *(*start_routine)(void*): Puntero a la funcion start_routine
 *operador ( recibe a la izquierda una direccion de memoria hacia una rutina
 * operador ) recibe parametros separados por comas*/


void* run(void* data)
{
	//(void)data;
	size_t thread_num = (size_t)data; 										/*No se va a usar el parametro*/
	printf("Hellow World from secondary thread %zu\n", thread_num); 
	return NULL;
}

int main (void)
{
	pthread_t thread;  										/*Intanciar Registro*/
	pthread_create(&thread, NULL, run, (void*)1);				/*Creal el thread e inicializa el registro*/
	printf("Hellow World from main thread\n");
	pthread_join(thread, NULL);								/*Espera al thread del registro que se le pasa por parametro, retorna un codigo de error*/
	return 0;
}

/*Si e thread principal se acaba, se acaba el programa*/
