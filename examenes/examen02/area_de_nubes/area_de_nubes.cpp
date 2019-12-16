#include <mpi.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

void expand_cloud(int r, int c, int my_start, int my_finish, int** mirror, char** image, double a, double b, int cols);
bool exists(int r,int c, int my_start, int my_finish, int cols);
void expand_total_clouds(int r, int c ,int rows, int cols, int** total_mirror);
void print_clouds(int* cloud_areas, int total_clouds);
int calc_start(int rows, int rank, int process_count);
int calc_finish(int rows, int rank, int process_count);
void read_real(char** image, int* rows, int* cols, std::string filename);


int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);
	
	int my_rank = -1;
	int process_count = -1;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	
	
	double a = 0.0;
	double b = 0.0;
	std::string filename = "";
	
	if(argc == 4)
	{
		filename = argv[1];
		a = atof(argv[2]);
		b = atof(argv[3]);
	}
	else
	{
		std::cout << "error in the amount of parameters" << std::endl;
	}
	
	char** image = NULL;
	int** total_mirror = NULL;
	int rows = -1;
	int cols = -1;
	if(my_rank == 0)
	{
		read_real(image, &rows, &cols, filename);	
		if(image == NULL)
		{
			return 1;
		}
		
		total_mirror = (int**)calloc(rows, sizeof(int*));
		for(int index = 0; index < rows; ++index)
		{
			total_mirror[index] = (int *)calloc(cols, sizeof(int));
			if(total_mirror[index] == NULL)
			{
				for(int j = index; j >= 0; --j)
				{
					free(total_mirror[j]);
				}
				return 1;
			}
		}
		
		
	}

	MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&a, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&b, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	if(my_rank != 0)
	{
		char** image = (char**)calloc(rows, sizeof(char*));
		for(int index = 0; index < rows; ++index)
		{
			image[index] = (char*)calloc(cols, sizeof(char));
			if(image[index] == NULL)
			{
				for(int j = index; j >= 0; --j)
				{
					free(image[j]);
				}
				return 1;
			}
		}
	}
	
	for(int i = 1; i < process_count; ++i)
	{
		for(int r = 0; r < rows; ++r)
		{
			MPI_Bcast(&image[r], cols, MPI_CHAR, 0, MPI_COMM_WORLD);
		}
	}

	int cloud_count = 0;
	int my_start = calc_start(rows, my_rank, process_count);
	int my_finish = calc_finish(rows, my_rank, process_count);
	
	int** mirror = (int**)calloc(my_finish - my_start, sizeof(int*));
	for(int index = 0; index < my_finish - my_start; ++index)
	{
		mirror[index] = (int*)calloc(my_finish - my_start, sizeof(int));
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
					expand_cloud(r,c, my_start, my_finish, mirror, image, a , b, cols);
				}
				else
				{
					mirror[r][c] = -1;
				}
			}
		}
	}
	
	
	if(my_rank != 0)
	{
		for(int r = 0; r < my_finish - my_start; ++r)
		{
			MPI_Send(mirror[r], cols, MPI_INT, 0, 0, MPI_COMM_WORLD);
		}
	}
	else
	{		
		for(int i = 1; i<= process_count; ++i)
		{
			for(int j = calc_start(rows, i, process_count); j < (calc_finish(rows, i, process_count)); ++j)
			{
				MPI_Recv(total_mirror[j], cols, MPI_INT, i,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			}
		}
	}
	
	if(my_rank == 0)
	{
		for(int r = 0; r < my_finish; ++r)
		{
			for(int c = 0; c < cols; ++c)
			{
				total_mirror[r][c] = mirror[r][c];
			}
		}
	}
	
	
	free(mirror);
	free(image);
	
	/*LIBERAR MEMORIA!!!! mirror, images*/
	
	MPI_Finalize();
	
	int total_clouds = 0;
	int* cloud_areas = (int*)calloc(100, sizeof(int));
	int cloud_areas_size = 100;

	for(int r = 0; r < rows; ++r)
	{
		for(int c = 0; c < cols; ++c)
		{
			if(total_mirror[r][c] != -1)
			{
				++total_clouds;
				if(total_clouds == cloud_areas_size)
				{
					int* temp = (int*)realloc(cloud_areas, 2 * cloud_areas_size);
					if(temp == NULL)
					{
						return 1;	
					}
					cloud_areas = temp;	
					cloud_areas_size = cloud_areas_size * 2;
				}
				cloud_areas[total_clouds] = 0;
				expand_total_clouds(r,c, rows, cols, total_mirror);
			}
		}
	}
	
	print_clouds(cloud_areas, total_clouds);
	
	free(total_mirror);
	
}

void expand_cloud(int r, int c, int my_start, int my_finish, int** mirror, char** image, double a, double b, int cols)
{
	if(!exists(r,c, my_start, my_finish, cols))
	{
		return;
	}
	if(mirror[r][c] != 0)
	{
		return;
	}
	if(image[r][c] >= a && image[r][c] <= b)
	{
		expand_cloud(r - 1, c, my_start, my_finish, mirror, image, a , b, cols);
		expand_cloud(r, c - 1, my_start, my_finish, mirror, image, a , b, cols);
		expand_cloud(r + 1, c, my_start, my_finish, mirror, image, a , b, cols);
		expand_cloud(r, c + 1, my_start, my_finish, mirror, image, a , b, cols);
	}
	else
	{
		mirror[r][c] = -1;
	}	
}


bool exists(int r,int c, int my_start, int my_finish, int cols)
{
	return r >= my_start && r <= my_finish && c >= 0 && c <= cols;
}


void expand_total_clouds(int r, int c, int rows, int cols, int** total_mirror)
{
	if(!exists(r,c,rows,cols, cols))
	{
		return;
	}
	if(total_mirror[r][c] == -1)
	{
		return;
	}
	total_mirror[r][c] = -1;
	expand_total_clouds(r-1, c, rows, cols, total_mirror);
	expand_total_clouds(r, c-1, rows, cols, total_mirror);
	expand_total_clouds(r+1, c, rows, cols, total_mirror);
	expand_total_clouds(r, c+1, rows, cols, total_mirror);
}

void print_clouds(int* cloud_areas, int cloud_count)
{
	for(int index = 1; index <= cloud_count; ++index)
	{
		std::cout << index << ": " << cloud_areas[index] << std::endl;
	}
}

int calc_start(int rows, int rank, int process_count)
{
	return rank * (rows / process_count);
}

int calc_finish(int rows, int rank, int process_count)
{
	return calc_start(rows, rank+1, process_count);
}

void read_real(char** image, int* rows, int* cols, std::string filename)
{
	std::ifstream file(filename);
	if(!file)
	{
		std::cerr << "Unable to open file. \n" ;
		return;
	}
	
	file >> *rows >> *cols;
		
	image = (char**)calloc(*rows, sizeof(char*));
		
	for(int index = 0; index < *rows; ++index)
	{
		image[index] = (char*)calloc(*cols, sizeof(char));
		if(image[index] == NULL)
		{
			for(int j = index; j >= 0; --j)
			{
				free(image[j]);
				return;
			}
		}
	}
		
	
	for(int r = 0; r < *rows; ++r)
	{
		for(int c = 0; c < *cols; ++c)
		{
			file >> image[r][c];
		}
	}
		
	file.close();
}




