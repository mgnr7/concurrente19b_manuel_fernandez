#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

typedef struct
{
	long double a;		/*punto inferior*/
	long double b;		/*punto superior*/
	int n;			/*cantidad de trapecios*/
	long double result;	/*resultado*/
	int thread_count;
	pthread_mutex_t mutex;
}shared_data_t;


typedef struct
{
	long double bottom;			/*punto inferior del intervalo en el que va a calcular el thread*/
	long double top;				/*punto superior del intervalo en el que va a calcular el thread*/
	int cant_trapecios;		/*cantidad de trapecios para el thread*/
	long double thread_result;	/*resultado del thread*/
	shared_data_t* shared_data;
}private_data_t;


int analyze_arguments(int argc, char*argv[], shared_data_t* shared_data);
int create_threads(shared_data_t* shared_data);
void* trapezoidal_area(void* data);	/*Calcula la integral de la funcion "f" en el intervalo [a, b] con "n" trapecios*/
long double imagen(long double x);							/*Devuelve la imagen f(x)*/


int main (int argc, char*argv[])
{
	shared_data_t* shared_data = (shared_data_t*) calloc(1, sizeof(shared_data_t));
	if(shared_data == NULL)
		return (void)fprintf(stderr, "error: could not allocate memory\n"), 2;

	int error = analyze_arguments(argc, argv, shared_data);
	if(error == 0)
	{
		
		pthread_mutex_init(&shared_data->mutex,NULL);
		struct timespec start_time;	
		clock_gettime(CLOCK_MONOTONIC, &start_time);	
							
		int error = create_threads(shared_data);
		if(error == 0)
		{
			printf("Total Area: %0.Lf \n", shared_data->result);
			struct timespec end_time;
			clock_gettime(CLOCK_MONOTONIC, &end_time);	
			double elapsed_seconds = end_time.tv_sec - start_time.tv_sec + 1e-9 *(end_time.tv_nsec - start_time.tv_nsec);
			printf("Calculations time %.9lfs\n", elapsed_seconds);	
		}	
		
	}
	pthread_mutex_destroy(&shared_data->mutex);
	free(shared_data);
	return error;
}

int analyze_arguments(int argc, char*argv[], shared_data_t* shared_data)
{
	if(argc != 5)
	{
		fprintf(stderr, "\n	usage: a, b, n, thread_count \n");	
		return 1;
	}
	if(sscanf(argv[1],"%Lf", &shared_data->a) != 1)
	{
		fprintf(stderr, "\n	invalid a value: %s\n", argv[1]);	
		return 2;
	}	
	if(sscanf(argv[2],"%Lf", &shared_data->b) != 1 || shared_data->a > shared_data->b)
	{	
		fprintf(stderr, "invalid b value: %s\n", argv[2]);
		return 2;
	}
	if(sscanf(argv[3],"%d", &shared_data->n) != 1 || shared_data->n == 0)
	{
		fprintf(stderr, "invalid n value: %s\n", argv[3]);
		return 3;
	}
	if(sscanf(argv[4],"%d", &shared_data->thread_count) != 1)
	{
		fprintf(stderr, "invalid n value: %s\n", argv[4]);
		return 4;
	}
	if(shared_data->thread_count == 0)
	{
		shared_data->thread_count = sysconf(_SC_NPROCESSORS_ONLN);
	}
	
	return EXIT_SUCCESS;
}

int create_threads(shared_data_t* shared_data)
{
	int number_of_threads = shared_data->thread_count;
	pthread_t* threads = (pthread_t*) malloc(number_of_threads * sizeof(pthread_t)); 
	
	if(threads == NULL)
		return (void)fprintf(stderr, "error: could not allocate memory for %d threads \n", number_of_threads), 1;
	
	private_data_t* private_data = (private_data_t*) calloc(number_of_threads, sizeof(private_data_t));	
	
	if(private_data == NULL)
		return (void)fprintf(stderr, "error: could not allocate memory for %d threads \n", number_of_threads), 3;
		
	/*hay que dividir el intervalo entre los threads*/
	
	for(int index = 0; index < number_of_threads ; ++index)							
	{
		private_data[index].bottom = shared_data->a + (index * ((shared_data->b - shared_data->a) / number_of_threads));
		private_data[index].top = shared_data->a + ((index + 1) * ((shared_data->b - shared_data->a)/ number_of_threads));
		private_data[index].cant_trapecios = (shared_data->n / shared_data->thread_count);
		private_data[index].shared_data = shared_data;
		pthread_create(&threads[index], NULL, trapezoidal_area, &private_data[index]);
	}

	for(int index = 0; index < number_of_threads ; ++index)								
		pthread_join(threads[index], NULL); 														
	
	
	free(private_data);				
	free(threads);	
	return 0;
}

void* trapezoidal_area(void* data)
{
	 private_data_t* private_data = (private_data_t*)data;
	 shared_data_t* shared_data = private_data->shared_data;
	 long double a = private_data->bottom;
	 long double b = private_data->top;
	 int n = private_data->cant_trapecios;
	 long double trapezoid_width;
	 trapezoid_width = (b - a) / n;
	 
	 long double thr_result;
	 thr_result = imagen(a);
	 thr_result += imagen(b);
	 
	 for(long double index = 1; index < n; index++)
	 {
		 thr_result = thr_result + imagen(a + index * trapezoid_width);
	 } 
	 
	 pthread_mutex_lock(&shared_data->mutex);
	 private_data->shared_data->result += (trapezoid_width / 2) * 2*thr_result;
	 pthread_mutex_unlock(&shared_data->mutex);
	 
	 return NULL;
}

long double imagen(long double x)
{
	/*Evaluar x en la parabola y retornar el valor obtenido*/
	 return (100*(x*x)) +(5*x)+ 125;
}
