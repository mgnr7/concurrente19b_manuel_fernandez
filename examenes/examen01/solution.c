#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

typedef struct
{
	char** tablero;
	int tamano;
	int cantidad_threads;
	pthread_mutex_t std_mutex;
	size_t errores;
	pthread_barrier_t barrera;
} shared_data_t;

typedef struct
{
	char* valores_leidos;
	shared_data_t* shared_data;
	int indice;
} private_data_t;


int read_terrain(shared_data_t* shared_data);
int create_threads(shared_data_t* shared_data);
void* run(void* data);
int leer_filas(private_data_t* private_data, shared_data_t* shared_data);
int leer_columnas(private_data_t* private_data, shared_data_t* shared_data);
int leer_bloque(private_data_t* private_data, shared_data_t* shared_data);
int agregado(int incio, int fin, char valor, private_data_t* private_data);
int no_es_numero(char* valor);
int min(int a, int b);

int main(int argc, char* argv[])
{
	shared_data_t* shared_data = (shared_data_t*) calloc(1, sizeof(shared_data_t));
	if(shared_data == NULL)
		fprintf(stderr, "no se pudo reservar datos compartidos");
		
	shared_data->cantidad_threads = sysconf(_SC_NPROCESSORS_ONLN);		/*cantidad de threads = cantidad de CPUs disponibles*/
	shared_data->errores = 0;
	
	pthread_mutex_init(&shared_data->std_mutex, NULL);
	pthread_barrier_init(&shared_data->barrera, NULL , shared_data->cantidad_threads);
	
	int error = read_terrain(shared_data);
	if(!error)
	{
		error = create_threads(shared_data);
		if(!error)
		{
			if(shared_data->errores == 0)
				fprintf(stderr, "VALIDO");
		}
	}
	pthread_barrier_destroy(&shared_data->barrera);
	pthread_mutex_destroy(&shared_data->std_mutex);
	free(shared_data);
	return 0;
}	//fin de main

int read_terrain(shared_data_t* shared_data)
{
		if ( scanf("%d", &shared_data->tamano) != 1 )
		return 1;
		
		getchar();


		shared_data->tablero = calloc((shared_data->tamano * shared_data->tamano), sizeof(char*));
		
		if(shared_data->tablero == NULL)
		{
			free(shared_data->tablero);
			return 1;
		}
		
		for(int r = 0; r < (shared_data->tamano * shared_data->tamano); r++)
		{
			shared_data->tablero[r] = (char*)calloc((shared_data->tamano * shared_data->tamano), sizeof(char));
			if(shared_data->tablero[r] == NULL)
			{
				for(int j = r; j >= 0; j--)
				{
					free(shared_data->tablero[j]);
				}
				free(shared_data->tablero);
				return 2;
			}
			
			for(int c = 0; c < (shared_data->tamano * shared_data->tamano); c++)
			{
				if ( scanf("%d", shared_data->tablero[r][c]) != 1 ) 
				{
					printf("e %d, %d \n", r , c);
					getchar();
				}
			}
			
		}
		
		return 0;
	
}	//fin de read_terrain

int create_threads(shared_data_t* shared_data)
{
	pthread_t* threads = (pthread_t*)malloc(shared_data->cantidad_threads * sizeof(pthread_t));
	if(threads == NULL)
	{
		fprintf(stderr, "no se pudo reservar memoria para los threads");
		return 1;
	}
		

	private_data_t* private_data = (private_data_t*) calloc(shared_data->cantidad_threads, sizeof(private_data_t));
	if(private_data == NULL)
	 {
		fprintf(stderr, "no se pudo reservar memoria para los datos privados");
		free(threads);
		return 2;
	 }
	 
		
	/*Se reserva memoria en private_data para almacernar el arreglo para almacenar los valores leidos en cada fila, columna o bloque*/
	for(int index = 0; index < shared_data->cantidad_threads; ++index)
	{	
		private_data[index].valores_leidos = (char*) calloc(shared_data->tamano * shared_data->tamano, sizeof(char));			
		if(private_data[index].valores_leidos == NULL)
		{
			for(int j = index; j >= 0; j--)
			{
				free(private_data[j].valores_leidos);
			}
			free(private_data);
			return 1;
		}
		
		private_data[index].shared_data = shared_data;
	}
	
	
	for(int index = 0; index < shared_data->cantidad_threads; ++index)
	{
		private_data[index].indice = index;
		pthread_create(&threads[index], NULL, run, &private_data[index]);
	}
	
	for(int index = 0; index < shared_data->cantidad_threads; ++index)
	{
		pthread_join(threads[index], NULL);
	}
	
	free(private_data);
	free(threads);
	return 0;
}

void* run (void* data)
{
	private_data_t* private_data = (private_data_t*) data;
	shared_data_t* shared_data = private_data->shared_data;
	leer_filas(private_data, shared_data);
	/*barrera*/
	pthread_barrier_wait(&shared_data->barrera);
	
	leer_columnas(private_data, shared_data);
	/*barrera*/
	pthread_barrier_wait(&shared_data->barrera);
	
	leer_bloque(private_data, shared_data);
	
	return NULL;
}

int leer_filas(private_data_t* private_data, shared_data_t* shared_data)
{
	int n = shared_data->tamano * shared_data->tamano;
	for(int fila = private_data->indice; fila < n; fila += shared_data->cantidad_threads)
	{
		for(int columna = 0; columna < n; columna++)
		{
			private_data->valores_leidos[columna] = shared_data->tablero[fila][columna];
			if(private_data->valores_leidos[columna] != 0)
			{
				if(agregado(0, columna, private_data->valores_leidos[columna], private_data))
				{
					pthread_mutex_lock(&shared_data->std_mutex);
					shared_data->errores += 1;
					printf("f %d , %d\n" , fila, columna);
					pthread_mutex_unlock(&shared_data->std_mutex);
				}
			}
		}
		
		private_data->valores_leidos = (char*)realloc(private_data->valores_leidos, 0);
		private_data->valores_leidos = (char*)realloc(private_data->valores_leidos, n);
	}
	
	return 0;
}

int leer_columnas(private_data_t* private_data, shared_data_t* shared_data)
{
	int n = shared_data->tamano * shared_data->tamano;
	for(int columna = private_data->indice; columna < n; columna += shared_data->cantidad_threads)
	{
		for(int fila = 0; fila < n; fila++)
		{
			private_data->valores_leidos[fila] = shared_data->tablero[fila][columna];
			if(private_data->valores_leidos[fila] != 0 )
				{
					if(agregado(0, fila, private_data->valores_leidos[fila], private_data))
					{
						pthread_mutex_lock(&shared_data->std_mutex);
						shared_data->errores += 1;
						printf("C %d , %d\n" , fila, columna);
						pthread_mutex_unlock(&shared_data->std_mutex);
					}
				}
			
		}
		
		private_data->valores_leidos = (char*)realloc(private_data->valores_leidos, 0);
		private_data->valores_leidos = (char*)realloc(private_data->valores_leidos, n);
	}
	
	return 0;
}

int leer_bloque(private_data_t* private_data, shared_data_t* shared_data)
{
	int m;
	m = shared_data->tamano * shared_data->tamano;
	int n; 
	n = shared_data->tamano;
	int contador;
	contador = 0;	
	int start;
	int end;
		
	start = private_data->indice * abs(m/shared_data->cantidad_threads) + min(private_data->indice, m % shared_data->cantidad_threads);
	end = (private_data->indice + 1) * abs(m/shared_data->cantidad_threads) + min((private_data->indice + 1), m % shared_data->cantidad_threads);
		
	int col_count;
	col_count = 0;
			
	for(int itr = start; itr < end; itr++)
	{
		for(int fila = start ; fila < end ; fila++)
		{
			for(int columna = n * col_count; columna < (n * col_count) + n; columna++)
			{
				private_data->valores_leidos[contador] = shared_data->tablero[fila][columna];
				contador ++;
				if(private_data->valores_leidos[contador -1] != 0 )
				{
					if(agregado(0, contador-1, private_data->valores_leidos[contador-1], private_data))
					{
						pthread_mutex_lock(&shared_data->std_mutex);
						shared_data->errores += 1;
						printf("B %d , %d\n" , fila, columna);
						pthread_mutex_unlock(&shared_data->std_mutex);
					}
				}
			}
		}

		private_data->valores_leidos = (char*)realloc(private_data->valores_leidos, 0);
		private_data->valores_leidos = (char*)realloc(private_data->valores_leidos, n);
						
		contador = 0;
		col_count++;
	}
			
	return 0;
}


int agregado(int inicio, int fin, char valor, private_data_t* private_data)
{
	for(int index = inicio; index < fin; index++)
	{
		if(private_data->valores_leidos[index] == valor)
			return 1;
	}
	return 0;
}

int min(int a, int b)
{
	if(a < b)
	{
		return a;
	}
		
	else{
		return b;
	}
}




