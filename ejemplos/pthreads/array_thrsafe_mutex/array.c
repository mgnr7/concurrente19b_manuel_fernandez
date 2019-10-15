#include <assert.h>
#include <stdlib.h>
#include <pthread.h>
#include "array.h"

typedef struct array
{
	void** array_elements;
	size_t array_capacity;
	size_t array_count;
	pthread_mutex_t mutex;
} array_t;

/*private section*/
size_t array_find_first_private(array_t* array, const void* element, size_t start_pos);

array_t* array_create(size_t capacity)
{
	assert(capacity);
	array_t* array = (array_t*)calloc(1, sizeof(array_t)); 
	if(array == NULL)
	{
		return NULL;
	}
		
	array->array_capacity = capacity;
	array->array_count = 0;

	pthread_mutex_init(&array->mutex, NULL);

	array->array_elements = (void**)malloc( capacity * sizeof(void*) );
	if(array->array_elements == NULL)
	{
		free(array);
		return NULL;
	}
	return array; 	
}

void array_destroy(array_t* array)
{
	assert(array);
	
	pthread_mutex_destroy(&array->mutex);

	free(array->array_elements);
	free(array);
}

int array_increase_capacity(array_t* array) /*Condicion de carrera pero el usuario no la llama directamente*/
{
	assert(array);
	
	size_t new_capacity = 10 * array->array_capacity;
	void** new_elements = (void**)realloc( array->array_elements, new_capacity * sizeof(void*) );
	if ( new_elements == NULL )
		return -1;

	array->array_capacity = new_capacity;
	array->array_elements = new_elements;
	return 0; // Success
}

int array_decrease_capacity(array_t* array) /*Condicion de carrera pero el usuario no la llama directamente*/
{
	assert(array);
	
	size_t new_capacity = array->array_capacity / 10;
	if ( new_capacity < 10 )
		return 0;

	void** new_elements = (void**)realloc( array->array_elements, new_capacity * sizeof(void*) );
	if ( new_elements == NULL )
		return -1;

	array->array_capacity = new_capacity;
	array->array_elements = new_elements;

	return 0; // Success
}

size_t array_get_count(array_t* array)
{
	assert(array);
	pthread_mutex_lock(&array->mutex);
	size_t result = array->array_count;
	pthread_mutex_unlock(&array->mutex);
	return result;
}

void* array_get_element(array_t* array, size_t index)
{
	assert(array);
	assert( index < array_get_count(array) );
	pthread_mutex_lock(&array->mutex);
	void* result = array->array_elements[index];
	pthread_mutex_unlock(&array->mutex);
	return result;
}

int array_append(array_t* array, void* element)
{
	assert(array);
	pthread_mutex_lock(&array->mutex);
	if ( array->array_count == array->array_capacity )
		if ( array_increase_capacity(array) )
			return (void) pthread_mutex_unlock(&array->mutex), -1;
	
	assert( array->array_count < array->array_capacity);	
	array->array_elements[array->array_count++] = element;
	pthread_mutex_unlock(&array->mutex);
	return 0; // Success
}

size_t array_find_first(array_t* array, const void* element, size_t start_pos)
{
	assert(array);
	pthread_mutex_lock(&array->mutex);
	size_t result = array_find_first_private(array, element, start_pos);
	pthread_mutex_unlock(&array->mutex);
	return result;
}

size_t array_find_first_private(array_t* array, const void* element, size_t start_pos)
{
	assert(array);
	for ( size_t index = start_pos; index < array->array_count; ++index )
		if ( array->array_elements[index] == element )
			return index;

	return array_not_found;
}

int array_remove_first(array_t* array, const void* element, size_t start_pos)
{
	assert(array);
	
	pthread_mutex_lock(&array->mutex);
	size_t index = array_find_first_private(array, element, start_pos);
	if ( index == array_not_found )
		return (void)pthread_mutex_unlock(&array->mutex), -1;

	for ( --array->array_count; index < array->array_count; ++index )
		array->array_elements[index] = array->array_elements[index + 1];
	if ( array->array_count == array->array_capacity / 10 )
		array_decrease_capacity(array);

	pthread_mutex_unlock(&array->mutex);
	return 0; // Removed
}
