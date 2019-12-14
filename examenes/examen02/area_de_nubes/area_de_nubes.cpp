#include <mpi.h>

int main(int argc, char* argv[])
{
	double a = 0.0;
	double b = 0.0;
	if(argc == 3)
	{
		a = argv[2];
		b = argv[3];
	}
	else
	{
		std::cout << "error in the amount of parameters" << std::endl;
	}
	
	int rows = read_integer();
	int cols = read_integer();
	char** image = calloc(rows, sizeof(char*));
	for(int index = 0; index < rows; ++index)
	{
		image[index] = calloc(cols, sizeof(char));
		if(image[index] == NULL)
		{
			for(int j = index; j >= 0; --j)
			{
				free(image[j]);
			}
			return 1;
		}
	}
	
	read_real(image);
	MPI_Init(&argc, &argv);
	int my_rank = -1;
	int process_count = -1;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	
	MPI_Bcast(&image, 1, MPI_CHAR, 0, MPI_COMM_WORLD); /*Pasar como vectores, no como matriz*/
	MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&a, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&b, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	
	int cloud_count = 0;
	int my_start = calc_start(rows, my_rank, process_count);
	int my_finish = calc_finish(rows, my_rank, process_count);
	
	int** mirror = calloc(my_finish - my_start, sizeof(int*));
	for(int index = 0; index < my_finish - my_start; ++index)
	{
		mirror[index] = calloc(my_finish - my_start, sizeof(int));
		if(mirror[index] == NULL)
		{
			for(int j = index; j >= 0; --j)
			{
				free(mirror[j]);
			}
			return 2;
		}
	}
	
	for(int r = my_start; r < my_finish; ++r)
	{
		for(int c = 0; c <= cols; ++c)
		{
			if(mirror[r - my_start] == 0)
			{
				if(image[r][c] >= a && image[r][c] <= b)
				{
					++cloud_count;
					expand_cloud(image[r][c]);
				}
				else
				{
					mirro[r][c] = -1;
				}
			}
		}
	}
	
	if(my_rank != 0)
	{
		for(int r = 0; r < my_finish - my_start; ++r)
		{
			MPI_Send(&mirror[r], 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		}
	}
	else
	{		
		for(int i = 1; i<= process_count; ++i)
		{
			for(int j = 0; j < calc_start(rows, i, process_count) - calc_finish(rows, i, process_count); ++j)
			{
				MPI_Recv(mirror[], 1, MPI_INT, i,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				total_mirror[i] = mirror[];
			}
		}
	}
	
	MPI_Finalize();
	int total_clouds = 0;
	for(int r = 0; r < rows; ++r)
	{
		for(int c = 0; c < cols; ++c)
		{
			if(total_mirror[r][c] != -1)
			{
				++total_clouds;
				cloud_areas[total_clouds] = 0;
				expand_total_clouds(r,c);
			}
		}
	}
	
	print_clouds(cloud_areas[]);
}

void expand_cloud(int r, int c)
{
	if(!exists(r,c))
	{
		return;
	}
	if(mirror[r][c] != 0)
	{
		return;
	}
	if(image[r][c] >= a && image[r][c] <= b)
	{
		expand_cloud(r - 1, c);
		expand_cloud(r, c - 1);
		expand_cloud(r + 1, c);
		expand_cloud(r, c + 1);
	}
	else
	{
		mirror[r][c] = -1
	}	
}


bool exists(int r,int c, int my_start, int my_finish)
{
	return r >= my_start && r <= my_finish && c >= 0 && c <= cols;
}


void expand_total_clouds(int r, int c)
{
	if(!exists(r,c,rows,cols))
	{
		return;
	}
	if(total_mirror[r][c] = -1)
	{
		return;
	}
	total_mirror[r][c] = -1;
	expand_total_clouds(r-1, c);
	expand_total_clouds(r, c-1);
	expand_total_clouds(r+1, c);
	expand_total_clouds(r, c+1);
}

void print_clouds(int* cloud_areas[])
{
	for(int index = 1; index <= cloud_count, ++index)
	{
		std::cout << index << ": " << cloud_areas[index] << std::endl;
	}
}

calc_start(int rows, int rank, int process_count)
{
	return rank * (rows / process_count);
}

calc_finish(int rows, int rank, int process_count)
{
	return calc_start(rows, rank+1, process_count);
}






