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
	
	int hot_potato = 0;
	
	if ( argc >= 2 )
	{
		hot_potato = atoi(argv[1]);
		
		--hot_potato;
		
		if(hot_potato == 0)
		{
			--hot_potato;
			std::cout << "The potato exploded, rank: " << my_rank << std::endl;
			MPI_Send(&hot_potato, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
			
			MPI_Finalize();
			return 0;
		}
		else
		{
			MPI_Send(&hot_potato, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);	
		}
		
		
	}
	else{
		std::cout << "positive integer required" << std::endl;
		MPI_Finalize();
		return 0;
	}


	
	/*Identificar cuando un proceso salio del juego*/
	/*Verificar si hot_potato es negativa, si es negativa proceso sale del juego y lo unico que hace es pasar la papa al proceso siguiente sin decrementarla*/
	/*Cuando la papa explota, hay que darle el valor original y enviarla al siguiente proceso*/
	/*decrementar hot_potato, si es 0 explota, si no enviarla al siguiente proceso*/
	/*Cuando un proceso detecta que es el unico en el juego imprime que es el ganador*/
	
	if(my_rank != 0)
	{
		MPI_Recv(&hot_potato, 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	else
	{
		MPI_Recv(&hot_potato, 1, MPI_INT, process_count, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	
	if(hot_potato < 0)
	{
		MPI_Finalize();
		return 0;
	}
	
	--hot_potato;
	
	if(hot_potato == 0)
	{
		--hot_potato;
		std::cout << "The potato exploded, rank: " << my_rank << std::endl;	
	}
	
	if(my_rank < process_count - 1)
	{
		MPI_Send(&hot_potato, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
	}
	else
	{
		MPI_Send(&hot_potato, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}

	MPI_Finalize();
}
