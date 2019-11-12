/*
main:
	shared mutex := semaphore(1)
	shared rest_count := read_integer()
	shared rest_capacity[rest_count] := 0[rest_count]
	shared count[rest_count] := 0[rest_count]
	shared rest_queue[rest_count] := 0[rest_count]
	
	for local index := 0 to rest_count do
		rest_capacity[index] := read_integer()
		rest_queue[index] = semaphore(rest_capacity[index])
		count[index] := rest_capacity[index]

	local id := 0
	while true do
		case read_char() of
		'P' : create_thread(patient(id++))
		'I' : create_thread(impatient(id++))
		EOF : return
		

patient(id):
	wait(mutex)
	--count[id mod rest_count]
	signal(mutex)
	wait(rest_queue[id mod rest_count])
	eat()
	signal(rest_queue[id mod rest_count])
	wait(mutex)
	++count
	signal(mutex)
	
impatient(id):

	local hungry := 1
	if count[id mod rest_count] > 0 do
		wait(mutex)
		--count[id mod rest_count]
		signal(mutex)
		wait(rest_queue[id mod rest_count])
		eat()
		signal(rest_queue[id mod rest_count])
		wait(mutex)
		++count[id mod rest_count]
		signal(mutex)
		hungry := 0
	
	else:
		walk()
		local minimum := 0
		for local index := 0 to rest_count do
			if count[index] > 0
				wait(mutex)
				-- count[index]
				signal(mutex)
				wait(rest_queue[index])
				eat()
				signal(rest_queue[index])
				wait(mutex)
				++count[index]
				signal(mutex)
				index := rest_count
				hungry := 0
				
			else:
				if count[minimum] > count[index] do
					minimum := index
					walk()
		if hungry > 0 do
		wait(mutex)
		--count[minimum]			
		signal(mutex)
		wait(rest_queue[minimum])
		eat()
		signal(rest_queue[minimum])		
		wait(mutex)
		++count[index]
		signal(mutex)
*/					

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

typedef struct
{
	pthread_mutex_t mutex;		
	size_t rest_count;			/*amount of restaurants*/
	size_t* rest_capacity;		/*array of the capacity of each restaurant*/
	size_t* count;				/*array with the capacity avaliable for each restaurant*/
	sem_t* rest_queue;			/*array of semaphores, one semaphore for every restaurant*/
}shared_data_t;

typedef struct
{
	shared_data_t* shared_data;
	size_t id;
}private_data_t;


void* patient(void* data);
void* impatient(void* data);

int main(int argc, char* argv[])
{
	shared_data_t* shared_data = (shared_data_t*) calloc(1, sizeof(shared_data_t));
	if(shared_data == NULL)
		return (void)fprintf(stderr, "error: could not allocate memory\n"), 1;
	
	shared_data->rest_count = getchar() - 48;	
		
	shared_data->rest_capacity = (size_t*)calloc(shared_data->rest_count, sizeof(size_t));
	if(shared_data->rest_capacity == NULL)
		return (void)fprintf(stderr, "error: could not allocate memory\n"), 3;
	
	shared_data->count = (size_t*)calloc(shared_data->rest_count, sizeof(size_t));
	if(shared_data->count == NULL)
		return (void)fprintf(stderr, "error: could not allocate memory\n"), 4;
	
	shared_data->rest_queue = (sem_t*)calloc(shared_data->rest_count, sizeof(sem_t));
	if(shared_data->rest_queue == NULL)
		return (void)fprintf(stderr, "error: could not allocate memory\n"), 5;	
		
	pthread_mutex_init(&shared_data->mutex,/*attr*/NULL);
	
	                  
	/*
		for local index := 0 to rest_count do
		rest_capacity[index] := read_integer()
		rest_queue[index] = semaphore(rest_capacity[index])
		count[index] := rest_capacity[index]
	 */
	
	for(size_t index = 0; index < shared_data->rest_count; index++)
	{
		getchar();
		shared_data->rest_capacity[index] = getchar()-48;
		sem_init(&shared_data->rest_queue[index], 0, shared_data->rest_capacity[index]);
		shared_data->count[index] = shared_data->rest_capacity[index];
	}
	
	/*
	 local id := 0
		while true do
			case read_char() of
			'P' : create_thread(patient(id++))
			'I' : create_thread(impatient(id++))
			EOF : return 
	 */
	size_t initial_size;
	initial_size = 10;
	private_data_t* private_data = (private_data_t*) calloc(initial_size, sizeof(private_data_t));
		if(private_data == NULL)
		{
			return (void)fprintf(stderr, "error: could not allocate memory\n"), 6;
		}
		
	size_t current_size;
	current_size = initial_size;	
	
	size_t id;
	id = 0;
	size_t bool;
	bool = 1;
	
	while(bool)
	{
		pthread_t thread;
		private_data[id].id = id;
		
		private_data[id].shared_data = shared_data;
		
		char c;
		getchar();
		c = getchar();
		
		switch (c){
			case 'P':
				--current_size;
				pthread_create(&thread, NULL, patient, &private_data[id]);
				break;
			case 'I':
				--current_size;
				pthread_create(&thread, NULL, impatient, &private_data[id]);
				break;
			/*case EOF:
				bool = 0;
				break;
			*/
			default:
				printf("leaving the simulation\n");
				bool = 0;
				break;
		}
		
		id++;
		
		
		if(current_size == 0)
		{
			private_data = (private_data_t*) realloc(private_data, initial_size + 10);
			initial_size = initial_size + 10;
			current_size = initial_size;
		}
		
		
		
	}
	
	//Hacer Join antes de borrar memoria
	
	free(private_data);
	pthread_mutex_destroy(&shared_data->mutex);
	free(shared_data->rest_queue);
	free(shared_data->count);
	free(shared_data->rest_capacity);
	free(shared_data);
	return 0;
}


/*
patient(id):
	wait(mutex)
	--count[id mod rest_count]
	signal(mutex)
	wait(rest_queue[id mod rest_count])
	eat()
	signal(rest_queue[id mod rest_count])
	wait(mutex)
	++count
	signal(mutex)
*/

void* patient(void* data)
{
	private_data_t* private_data = (private_data_t*)data;
	shared_data_t* shared_data = private_data->shared_data;
	
	pthread_mutex_lock(&shared_data->mutex);
	--shared_data->count[private_data->id % shared_data->rest_count];
	pthread_mutex_unlock(&shared_data->mutex);
	
	sem_wait(&shared_data->rest_queue[private_data->id % shared_data->rest_count]);
	//eat();
	sem_post(&shared_data->rest_queue[private_data->id % shared_data->rest_count]);
	
	pthread_mutex_lock(&shared_data->mutex);
	++shared_data->count[private_data->id % shared_data->rest_count];
	pthread_mutex_unlock(&shared_data->mutex);
	
	return NULL;
}

/*
impatient(id):

	local hungry := 1
	if count[id mod rest_count] > 0 do
		wait(mutex)
		--count[id mod rest_count]
		signal(mutex)
		wait(rest_queue[id mod rest_count])
		eat()
		signal(rest_queue[id mod rest_count])
		wait(mutex)
		++count[id mod rest_count]
		signal(mutex)
		hungry := 0
	
	else:
		walk()
		local minimum := 0
		for local index := 0 to rest_count do
			if count[index] > 0
				wait(mutex)
				-- count[index]
				signal(mutex)
				wait(rest_queue[index])
				eat()
				signal(rest_queue[index])
				wait(mutex)
				++count[index]
				signal(mutex)
				index := rest_count
				hungry := 0
				
			else:
				if count[minimum] > count[index] do
					minimum := index
					walk()
	
		if hungry > 0 do
		wait(mutex)
		--count[minimum]			
		signal(mutex)
		wait(rest_queue[minimum])
		eat()
		signal(rest_queue[minimum])		
		wait(mutex)
		++count[index]
		signal(mutex)

*/

void* impatient(void* data)
{
	private_data_t* private_data = (private_data_t*)data;
	shared_data_t* shared_data = private_data->shared_data;
	
	size_t hungry;
	hungry = 1;
	
	size_t favorite = private_data->id % shared_data->rest_count;
	
	if(shared_data->count[favorite] > 0)
	{
		pthread_mutex_lock(&shared_data->mutex);
		--shared_data->count[favorite];
		pthread_mutex_unlock(&shared_data->mutex);
		
		sem_wait(&shared_data->rest_queue[favorite]);
		//eat();
		sem_post(&shared_data->rest_queue[favorite]);
		
		pthread_mutex_lock(&shared_data->mutex);
		++shared_data->count[favorite];
		pthread_mutex_unlock(&shared_data->mutex);
		
		hungry = 0;
	}
	else
	{
		//walk();
		size_t minimum = 0;
		for(size_t index = favorite + 1; index < shared_data->rest_count; ++index)
		{
			if(shared_data->count[index] > 0)
			{
				pthread_mutex_lock(&shared_data->mutex);
				--shared_data->count[index];
				pthread_mutex_unlock(&shared_data->mutex);
				
				sem_wait(&shared_data->rest_queue[index]);
				//eat();
				sem_post(&shared_data->rest_queue[index]);
				
				pthread_mutex_lock(&shared_data->mutex);
				++shared_data->count[index];
				pthread_mutex_unlock(&shared_data->mutex);
				
				hungry = 0;
			}
			else
			{
				if(shared_data->count[minimum] > shared_data->count[index])
				{
					minimum = index;
					//walk();
				}
			}
		}
		
		for(size_t index = 0; index < favorite; ++index)
		{
			if(shared_data->count[index] > 0)
			{
				pthread_mutex_lock(&shared_data->mutex);
				--shared_data->count[index];
				pthread_mutex_unlock(&shared_data->mutex);
				
				sem_wait(&shared_data->rest_queue[index]);
				//eat();
				sem_post(&shared_data->rest_queue[index]);
				
				pthread_mutex_lock(&shared_data->mutex);
				++shared_data->count[index];
				pthread_mutex_unlock(&shared_data->mutex);
				
				hungry = 0;
			}
			else
			{
				if(shared_data->count[minimum] > shared_data->count[index])
				{
					minimum = index;
					//walk();
				}
			}
		}
		
		/*if all restaurants are full, wait for the restaurant with the shorter wait*/
		
		if(hungry)
		{
			pthread_mutex_lock(&shared_data->mutex);
			--shared_data->count[minimum];
			pthread_mutex_unlock(&shared_data->mutex);
				
			sem_wait(&shared_data->rest_queue[minimum]);
			//eat();
			sem_post(&shared_data->rest_queue[minimum]);
				
			pthread_mutex_lock(&shared_data->mutex);
			++shared_data->count[minimum];
			pthread_mutex_unlock(&shared_data->mutex);
					
			hungry = 0;
		}
		
	}
	
	return NULL;
}

/*void eat(){}*/

/*void walk(){}*/
 



