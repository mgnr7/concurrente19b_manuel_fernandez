#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <mpi.h>

#define MESSAGE_CAPACITY 1024

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
	
	int hot_potato = 0;
	char message[MESSAGE_CAPACITY];
	sprintf(message, "The potato exploded, rank: %d", my_rank);
	
	if ( argc >= 2 )
	{
		hot_potato = atoi(argv[1]);	
	}
	else{
		std::cout << "positive integer required" << std::endl;
		MPI_Finalize();
		return 0;
	}



	while(hot_potato > 0)
	{
		--hot_potato;
		if(hot_potato == 0)
		{
			--hot_potato;
			std::cout <<" exploto rank: "<< my_rank << std::endl;	
		}
		
		MPI_Send(&hot_potato, 1, MPI_INT, (my_rank + 1) % process_count, 0, MPI_COMM_WORLD);

		if(my_rank != 0)
		{
			MPI_Recv(&hot_potato, 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		else
		{
			MPI_Recv(&hot_potato, 1, MPI_INT, process_count - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}
	
	
	MPI_Finalize();
}
