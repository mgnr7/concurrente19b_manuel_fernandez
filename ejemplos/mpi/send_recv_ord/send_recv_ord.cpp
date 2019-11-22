#include <mpi.h>
#include <iostream>
#include <cstdio>
#include <cstring>

#define MESSAGE_CAPACITY 1024

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);
	
	int my_rank = -1;
	int process_count = -1;
	
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	
	char hostname[MPI_MAX_PROCESSOR_NAME];
	int hostname_length = -1;
	MPI_Get_processor_name(hostname, &hostname_length);
	
	char message[MESSAGE_CAPACITY];
	sprintf(message, "Hello from main thread of process %d of %d on %s\n"
		, my_rank, process_count, hostname);
//	message << "Hello from main thread of process " << my_rank
//		<< " of " << process_count << " on " << hostname << "\n"; //std::endl;
	
	if ( my_rank == 0 )
	{
		std::cout << message;
		for ( int sender = 1; sender < process_count; ++sender )
		{
			MPI_Recv(message, /*capacity*/ MESSAGE_CAPACITY, MPI_CHAR, /*source*/ sender, /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			std::cout << message;
		}
	}
	else
	{
		MPI_Send(message, /*count*/ strlen(message) + 1, MPI_CHAR, /*dest*/ 0, /*tag*/ 0, MPI_COMM_WORLD);
	}
	
	
	MPI_Finalize();
}
