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
	
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 		
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);  
	
	char hostname[MPI_MAX_PROCESSOR_NAME];	
	int hostname_length = -1;
	MPI_Get_processor_name(hostname, &hostname_length);
	
	char message[MESSAGE_CAPACITY];
	sprintf(message, "Winner!!, rank: %d", my_rank);
	
	int hot_potato_original = 0;
	int hot_potato = 0;
	int losers = 0;
	int out_of_game = 0;
	int game_on = 1;
	
	if ( argc >= 2 )
	{
		hot_potato_original = atoi(argv[1]);
		hot_potato = hot_potato_original;
		
		if(hot_potato < 1)
		{
			std::cout << "Hot_Potato Invalid value"<< std::endl;
			return 0;
		}
		MPI_Bcast(&losers, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(&hot_potato_original, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(&game_on, 1, MPI_INT, 0, MPI_COMM_WORLD);
	}
	else{
		std::cout << "Hot_Potato initial value required" << std::endl;
		MPI_Finalize();
		return 0;
	}

	

while(game_on)
{
	MPI_Send(&hot_potato, 1, MPI_INT, (my_rank + 1) % process_count, 0, MPI_COMM_WORLD);
	MPI_Send(&game_on, 1, MPI_INT, (my_rank + 1) % process_count, 0, MPI_COMM_WORLD);
	MPI_Send(&losers, 1, MPI_INT, (my_rank + 1) % process_count, 0, MPI_COMM_WORLD);	
	
	if(my_rank != 0)
	{
		MPI_Recv(&hot_potato, 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&game_on, 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&losers, 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		if(!out_of_game)
		{
			if(losers == process_count - 1)
			{
				game_on = 0;
				std::cout << "Winner! rank: " << my_rank << std::endl;
			}
			else
			{
				--hot_potato;
				if(hot_potato == 0)
				{
					out_of_game = 1;
					++losers;
					hot_potato = hot_potato_original;
				}
			}
		}
	}

	if(my_rank == 0)
	{
		MPI_Recv(&hot_potato, 1, MPI_INT, process_count - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&game_on, 1, MPI_INT, process_count - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&losers, 1, MPI_INT, process_count - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if(!out_of_game)
		{
			if(losers == process_count - 1)
			{
				game_on = 0;
				std::cout << "Winner! rank: " << my_rank << std::endl;
			}
			else
			{
				--hot_potato;
				if(hot_potato == 0)
				{
					out_of_game = 1;
					++losers;
					hot_potato = hot_potato_original;
				}
			}
		}
	}
}

	MPI_Finalize();
}
