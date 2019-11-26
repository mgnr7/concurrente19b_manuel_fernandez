#include <iomanip>
#include <iostream>
#include <mpi.h>
#include <cstdlib>
#include <time.h>


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
	
	srand(my_rank + time(nullptr) + clock());
	int my_lucky_number = rand() % 100;

	int global_min = -1;
	int global_max = -1;
	double average = -1.0;
	int global_sum = -1;

	char ready = 'Y';
	if(my_rank != 0)
		MPI_Recv(&ready, /*capacity*/ 1, MPI_CHAR, /*source*/ my_rank - 1, /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	
	std::cout << "Process " << my_rank << ": my lucky number is " << std::setw(2) << std::setfill('0') 
	<< my_lucky_number << std::endl;


	MPI_Reduce(&my_lucky_number, &global_min, /*int count*/ 1, MPI_INT, MPI_MIN, /*int root*/ 0, MPI_COMM_WORLD);
	MPI_Reduce(&my_lucky_number, &global_max, /*int count*/ 1, MPI_INT, MPI_MAX, /*int root*/ 0, MPI_COMM_WORLD);
	MPI_Reduce(&my_lucky_number, &global_sum, /*int count*/ 1, MPI_INT, MPI_SUM, /*int root*/ 0, MPI_COMM_WORLD);
	
	average = global_sum / process_count ;
	if(my_rank == 0)
	{
		std::cout << "Process " << my_rank << ": all minimum: " << std::setw(2) << global_min << std::endl;
		std::cout << "Process " << my_rank << ": all maximum: " << std::setw(2) << global_max << std::endl;
		std::cout << "Process " << my_rank << ": average : "<< std::fixed << std::setprecision(2) << average << std::endl;	
	}

	if(my_rank < process_count - 1)
	{
		MPI_Send(&ready, 1, MPI_CHAR, my_rank + 1, 0, MPI_COMM_WORLD);
	}
	
	MPI_Finalize();
}
