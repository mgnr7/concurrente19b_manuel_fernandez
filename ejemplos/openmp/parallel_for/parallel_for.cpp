#include <iostream>
#include <omp.h>

int main (int argc, char*argv[])
{
	int thread_count = omp_get_max_threads();
	if(argc >= 2)
	{
		thread_count = atoi(argv[1]);
	}
	
	int iteration_count = thread_count;
	if(argc >= 3)
	{
		iteration_count = atoi(argv[2]);
	}
																			/*default(none): no haga supuestos*/
	#pragma omp parallel for num_threads(thread_count) \
		default(none) shared(iteration_count, std::cout)		/*ciclo por contador [si no se sabe la cantidada de iteraciones de ante mano, produce un error, no puede tener break, continue o return], mapeo por bloques por defecto*/
		for(int iteration = 0; iteration < iteration_count; ++iteration)		/*cada thread ejecuta el for*/
		{
			#pragma omp critical(cout)
			std::cout << omp_get_thread_num() << " / " << omp_get_num_threads() 
			<< ": iteration " << iteration << " / " << iteration_count << std::endl;
		}
	return 0;
}


