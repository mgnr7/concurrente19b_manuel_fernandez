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
	
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 		
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);  
	
	char hostname[MPI_MAX_PROCESSOR_NAME];	
	int hostname_length = -1;
	MPI_Get_processor_name(hostname, &hostname_length);
	
	int hot_potato_original = 0;
	int hot_potato = 0;
	int intial_rank = 0;
	int losers = 0;
	int out_of_game = 0;
	int game_on = 1;
	
	if ( argc >= 3 )
	{
		hot_potato_original = atoi(argv[1]);
		hot_potato = atoi(argv[1]);
		
		if(hot_potato < 1)
		{
			std::cout << "Hot_Potato Invalid value"<< std::endl;
			return 0;
		}
		
		--hot_potato;
		
		if(hot_potato == 0)
		{
			std::cout << "The potato exploded, rank: " << my_rank << std::endl;
			hot_potato = hot_potato_original;
			out_of_game = 1;
			++losers;
			MPI_Send(&hot_potato, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
			MPI_Bcast(&losers, 1, MPI_INT, 0, MPI_COMM_WORLD);
			MPI_Bcast(&hot_potato_original, 1, MPI_INT, 0, MPI_COMM_WORLD);
			MPI_Bcast(&game_on, 1, MPI_INT, 0, MPI_COMM_WORLD);
		}
		else
		{
			MPI_Send(&hot_potato, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
			MPI_Bcast(&losers, 1, MPI_INT, 0, MPI_COMM_WORLD);
			MPI_Bcast(&hot_potato_original, 1, MPI_INT, 0, MPI_COMM_WORLD);
			MPI_Bcast(&game_on, 1, MPI_INT, 0, MPI_COMM_WORLD);
		}
		
		
	}
	else{
		std::cout << "Hot_Potato initial value and initial process required" << std::endl;
		MPI_Finalize();
		return 0;
	}


	
	/*Identificar cuando un proceso salio del juego: losers*/
	/*Verificar si hot_potato es negativa, si es negativa proceso sale del juego y lo unico que hace es pasar la papa al proceso siguiente sin decrementarla*/
	/*Cuando la papa explota, hay que darle el valor original y enviarla al siguiente proceso*/
	/*decrementar hot_potato, si es 0 explota, si no enviarla al siguiente proceso*/
	/*Cuando un proceso detecta que es el unico en el juego imprime que es el ganador*/
	
	while(game_on)
	{
		
		if(my_rank != 0)
		{
			MPI_Recv(&hot_potato, 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&game_on, 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&losers, 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		else
		{
			MPI_Recv(&hot_potato, 1, MPI_INT, process_count, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&game_on, 1, MPI_INT, process_count, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&losers, 1, MPI_INT, process_count, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}	
			
		if(!out_of_game) /*El proceso sigue en juego*/
		{
			if(losers == process_count -1) /*El proceso gano*/
			{
				std::cout << "WINNER!!, rank: " << my_rank << std::endl;	
				game_on = 0;
				
				if(my_rank < process_count - 1)
				{
					MPI_Send(&hot_potato, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
					MPI_Send(&losers, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
					MPI_Send(&game_on, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
				}
				else
				{
					MPI_Send(&hot_potato, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
					MPI_Send(&losers, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
					MPI_Send(&game_on, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
				}
				
			}
			
			else /*Aun no hay ganador*/
			{
				--hot_potato;
				
				if(hot_potato == 0)
				{
					std::cout << "The potato exploded, rank: " << my_rank << std::endl;	
					out_of_game = 1;
					++losers;
					hot_potato = hot_potato_original;
				}
				
				if(my_rank < process_count - 1)
				{
					MPI_Send(&hot_potato, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
					MPI_Send(&losers, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
				}
				else
				{
					MPI_Send(&hot_potato, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
					MPI_Send(&losers, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
				}

			}

		}
		else/*El proceso ya no esta en juego*/
		{
			if(my_rank < process_count - 1)
			{
				MPI_Send(&hot_potato, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
				MPI_Send(&losers, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
			}
			else
			{
				MPI_Send(&hot_potato, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
				MPI_Send(&losers, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
			}
		}
	
	}

	MPI_Finalize();
}
