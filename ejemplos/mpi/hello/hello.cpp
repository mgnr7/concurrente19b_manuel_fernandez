#include <iostream>
#include <mpi.h>

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);
	int my_rank = -1;
	int process_count = -1;
	
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);	
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	
	char hostname[MPI_MAX_PROCESSOR_NAME];
	int hostname_length = -1;
	MPI_Get_processor_name(hostname, hostname_length);
	
	
	
	std::cout << "hello from main thread "<< my_rank << " of "<< process_count << " on " << hostname << "\n"<< std::endl;
	
	MPI_Finalize();
}
