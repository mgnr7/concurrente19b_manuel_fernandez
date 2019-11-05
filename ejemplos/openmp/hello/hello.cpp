#include <iostream>

int main ()
{
	#pragma omp parallel
	{
		#pragma omp critical(cout)
		std::cout << "Hellow World from secondary thread" << std::endl;
		
	}
	return 0;
}


