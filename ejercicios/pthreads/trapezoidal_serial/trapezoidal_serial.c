#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

long double trapezoidal_area(double a, double b, int n);	/*Calcula la integral de la funcion "f" en el intervalo [a, b] con "n" trapecios*/
long double imagen(double x);							/*Devuelve la imagen f(x)*/

int main (int argc, char*argv[])
{

	long double a;		/*punto inferior*/
	long double b;		/*punto superior*/
	int n;			/*cantidad de trapecios*/
	long double result;	/*resultado*/
	
	if(argc != 4)
	{
		fprintf(stderr, "\n	usage: a, b, n \n");	
		return 1;
	}

	if(sscanf(argv[1],"%Lf", &a) != 1)
	{
		fprintf(stderr, "\n	invalid a value: %s\n", argv[1]);	
		return 2;
	}
		
	if(sscanf(argv[2],"%Lf", &b) != 1 || a > b)
	{	
		fprintf(stderr, "invalid b value: %s\n", argv[2]);
		return 2;
	}
	if(sscanf(argv[3],"%d", &n) != 1 || n == 0)
	{
		fprintf(stderr, "invalid n value: %s\n", argv[3]);
		return 3;
	}

		struct timespec start_time;	
		clock_gettime(CLOCK_MONOTONIC, &start_time);						
		
		result = trapezoidal_area(a, b, n);		/*Invocacion a trapezoidal_Area*/
		printf("Total Area: %0.Lf \n", result);
		struct timespec end_time;
		clock_gettime(CLOCK_MONOTONIC, &end_time);	
		double elapsed_seconds = end_time.tv_sec - start_time.tv_sec + 1e-9 *(end_time.tv_nsec - start_time.tv_nsec);
		printf("Calculations time %.9lfs\n", elapsed_seconds);
	
	return 0;
}

long double trapezoidal_area(double a, double b, int n)
{
	/*Determinar el ancho de cada trapecio:
	 * (b - a) / n
	 * 
	 *Hay que ejecutar n sumas:
	 * -Primero calcular ancho/2
	 * -Despues calcular f(a) y f(b):
	 * 	result = f(a) + f(b)
	 * -Despues ejecutar las sumas 2*f(a + ancho_trapecio) + ... + 2*f(b - ancho_trapecio)
	 * Estas sumas se hacen dentro de un ciclo for que se incrementa cada ancho_trapecio, mientras sea menor a b inciando en a+ancho_trapecio
	 * for(index = ancho_trapecio; index < b; index + ancho_trapecio)
	 * -Finalemente al resultado de las sumas hay que multiplicarlo por (ancho_trapecio / 2)
	 * */	 
	 long double trapezoid_width;
	 trapezoid_width = (b - a) / n;
	 
	 long double result;
	 result = imagen(a);
	 result += imagen(b);
	 
	 for(double index = 1; index < n; index++)
	 {
		 result = result +  imagen(a+index * trapezoid_width);
	 } 
	 
	 
	 return (trapezoid_width / 2) * 2*result;
}

long double imagen(double x)
{
	/*Evaluar x en la parabola y retornar el valor obtenido*/
	 long double y;
	 y = (100*(x*x)) +(5*x)+ 125;
	 return y;
}
