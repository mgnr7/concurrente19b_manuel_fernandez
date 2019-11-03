typedef struct
{
	char** tablero;
	int tamano;
	int cantidad_threads;
	pthread_mutex_t std_mutex;
} shared_data_t;

typedef struct
{
	char* valores_leidos;
	shared_data_t* shared_data;
	int indice;
} private_data_t;

main(int argc, char* argv[])
{
	shared_data_t* shared_data = (shared_data_t*) calloc(1, sizeof(shared_data_t));
	if(shared_data == NULL)
		fprintf(stderr, "no se pudo reservar datos compartidos");
		
	shared_data->cantidad_threads = sysconf(_SC_NPROCESSORS_ONLN);
	
	pthread_mutex_init(&shared_data->std_mutex, NULL);
	
	int error = read_terrain(shared_data, argv[], argc);
	if(error)
	{
		pthread_mutex_destroy(&shared_data->std_mutex);
		free(shared_data);
		return 0;
	}	
	
	error = create_threads();
	if(error)
	{
		pthread_mutex_destroy(&shared_data->std_mutex);
		free(shared_data);
		return 0;
	}
	
	pthread_mutex_destroy(&shared_data->std_mutex);
	free(shared_data);
	return 0;
}	//fin de main

int read_terrain(shared_data_t* shared_data, char* argv[], int argc)
{
	if(argc > 2)
	{
		shared_data->tamano = getchar();
		getchar(); //salto de linea
		shared_data->tablero = (char*) calloc((shared_data->tamano * shared_data->tamano), sizeof(char*));
		
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
			
			for(int c = 1; c <= (shared_data->tamano * shared_data->tamano); c++)
			{
				shared_data->tablero[r][c] = getchar();
				getchar(); //espacio en blanco
			}
			getchar(); //cambio de linea
		}
	}
}	//fin de read_terrain

int create_threads(shared_data_t* shared_data)
{
	pthread_t* threads = (pthread_t*)malloc(shared_data->cantidad_threads * sizeof(pthreads_t));
	if(threads == NULL)
		return (void) fprintf(stderr, "no se pudo reservar memoria para los threads");

	private_data_t* private_data = (private_data_t*) calloc(shared_data->cantidad_threads, sizeof(private_data_t));
	if(private_data == NULL)
		return (void) fprintf(stderr, "no se pudo reservar memoria para los datos privados");
	
	for(int index = 0; index < shared_data->cantidad_threads; ++index)
	{
		private_data[index]->valores_leidos = (char*) calloc(shared_data->tamano * shared_data->tamano, sizeof(char));
		if(private_data[index]->valores_leidos == NULL)
		{
			for(int j = index; j >= 0; j--)
			{
				free(private_data[j]->valores_leidos);
			}
			free(private_data);
			return 1;
		}
		
		private_data[index]->shared_data = shared_data;
	}
	
	for(int index = 0; index < shared_data->cantidad_threads; ++index)
	{
		private_data[index]->indice = index;
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
	leer_columnas(private_data, shared_data);
	leer_bloques(private_data, shared_data);
	
}

int leer_filas(private_data_t* private_data, shared_data_t* shared_data)
{
	int n = shared_data->tamano * shared_data->tamano;
	for(int fila = private_data->indice; fila < n; fila += shared_data->cantidad_threads)
	{
		for(int columna = 0; columna < n; columna++)
		{
			private_data->valores_leidos[columna] = shared_data->tablero[filas][columna];
			if(private_data->valores_leidos[columna] != 0)
			{
				if(agregado(0, columna, private_data->valores_leidos[columna]))
				{
					pthread_mutex_lock(&shared_data->std_mutex);
					printf("f %d , %d" , fila, columna);
					pthread_mutex_unlock(&shared_data->std_mutex);
				}
				if(no_es_numero(private_data->valores_leidos[columna]))
				{
					pthread_mutex_lock(&shared_data);
					fprintf("e %d, %d", fila, columna);
					pthread_mutex_unlock(&shared_data->std_mutex);
				}
			}
		}
		
		private_data->valores_leidos = (char*)reserve(0, sizeof(char));
		private_data->valores_leidos = (char*)reserve(n, sizeof(char));
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
			private_data->valores_leidos[columna] = shared_data->tablero[fila][columna];
			if(no_es_numero(valores_leidos[columna]))
			{
				pthread_mutex_lock(&shared_data);
				fprintf("e %d, %d", fila, columna);
				pthread_mutex_unlock(&shared_data->std_mutex);
			}	
			else if(private_data->valores_leidos[columna] != 0 && agregado(0, columna, private_data->valores_leidos[columna]))
			{
				pthread_mutex_lock(&shared_data->std_mutex);
				printf("C %d , %d" , fila, columna);
				pthread_mutex_unlock(&shared_data->std_mutex);
			}
			
		}
		
		private_data->valores_leidos = (char*)reserve(0, sizeof(char));
		private_data->valores_leidos = (char*)reserve(n, sizeof(char));
	}
	
	return 0;
}

int leer_bloque(private_data_t* private_data, shared_data_t* shared_data)
{
	int m = shared_data->tamano * shared_data->tamano;
	int n = shared_data->tamano;
	for(int fila = (private_data->indice * n); fila < m; fila += (shared_data->cantidad_threads*n))
	{
		
	}
}








