#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <mpi.h>

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);
	
	int my_rank = -1;
	int process_count = -1;
	
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); /*my_rank: identificador para cada proceso dentro el mundo*/
	MPI_Comm_size(MPI_COMM_WORLD, &process_count); /*tamano del mundo o cantidad de procesos*/
	
	char hostname[MPI_MAX_PROCESSOR_NAME];	
	int hostname_length = -1;
	MPI_Get_processor_name(hostname, &hostname_length);
	
	if ( argc >= 2 )
	{
		int hot_potato = atoi(argv[1]);
	}
	else{
		std::cout << "positive integer required" << std::endl;
	}
	
	/*decrementar hot_potato, si es 0 explota, si no enviarla al siguiente proceso*/
	
	
/*
		if ( my_rank == 0 )
		{
			std::cin >> global_start >> global_finish;
			for ( int destination = 1; destination < process_count; ++destination )
			{
				MPI_Send(&global_start, 1, MPI_INT, destination, 0, MPI_COMM_WORLD);
				MPI_Send(&global_finish, 1, MPI_INT, destination, 0, MPI_COMM_WORLD);
			}
		}
		else
		{
			MPI_Recv(&global_start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&global_finish, 1, MPI_INT,  0,  0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}
	*/	
	MPI_Finalize();
}
