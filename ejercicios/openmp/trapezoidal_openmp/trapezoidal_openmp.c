#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <omp.h>

/*EJECUTAR CON OPENMP POR LA CANTIDAD ESPECIFICADA DE TRABAJADORES*/
double trapezoidal_area(double a, double b, int n);	/*Calcula la integral de la funcion "f" en el intervalo [a, b] con "n" trapecios*/
double imagen(double x);							/*Devuelve la imagen f(x)*/

int main (int argc, char*argv[])	/*RECIBE CANTIDAD DE TRABAJADORES*/
{

	double a;		/*punto inferior*/
	double b;		/*punto superior*/
	int n;			/*cantidad de trapecios*/
	long double result;	/*resultado*/
	int w;			/*trabajadores*/
	
	w = omp_get_max_threads();
	result = 0;
	
	if(argc < 4)
	{
		fprintf(stderr, "\n	usage: a, b, n, w \n");	
		return 1;
	}

	if(sscanf(argv[1],"%lf", &a) != 1)
	{
		fprintf(stderr, "\n	invalid a value: %s\n", argv[1]);	
		return 2;
	}
		
	if(sscanf(argv[2],"%lf", &b) != 1 || a > b)
	{	
		fprintf(stderr, "invalid b value: %s\n", argv[2]);
		return 2;
	}
	if(sscanf(argv[3],"%d", &n) != 1 || n == 0)
	{
		fprintf(stderr, "invalid n value: %s\n", argv[3]);
		return 3;
	}
	
	if(argc == 5)
	{
		sscanf(argv[4],"%d", &w);
	}

	struct timespec start_time;	
	clock_gettime(CLOCK_MONOTONIC, &start_time);						
	
		
	#pragma omp parallel for num_threads(w) default(none) shared(a, b, n, w, result)
		for(int index = 0; index < w; ++index)
		{
			long double local_result;
			double local_a;
			double local_b;
			int local_n;
			local_a = a + (index * ((b - a) / w));
			local_b = a + ((index + 1) * ((b - a) / w));
			local_n = n / w;
			local_result = trapezoidal_area(local_a, local_b, local_n);
			#pragma omp critical(result)
			result += local_result;
		}
		
			
	
			
	printf("Total Area: %0.Lf \n", result);
	struct timespec end_time;
	clock_gettime(CLOCK_MONOTONIC, &end_time);	
	double elapsed_seconds = end_time.tv_sec - start_time.tv_sec + 1e-9 *(end_time.tv_nsec - start_time.tv_nsec);
	printf("Calculations time %.9lfs\n", elapsed_seconds);
	
	return 0;
}

double trapezoidal_area(double a, double b, int n)
{
	 double trapezoid_width;
	 trapezoid_width = (b - a) / n;
	 
	 double result;
	 result = imagen(a);
	 result += imagen(b);
	 
	 for(double index = 1; index < n; ++index)
	 {
		 result = result +  imagen(a + index * trapezoid_width);
	 } 
	 
	 
	 return (trapezoid_width / 2) * 2*result;
}

double imagen(double x)
{
	/*Evaluar x en la parabola y retornar el valor obtenido*/
	 return x*x;
}
