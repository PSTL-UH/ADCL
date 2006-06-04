#include "ADCL_internal.h"


int ADCL_array_init (ADCL_array_t **arr, const char name[64], int size )
{
    ADCL_array_t *tarr;

    tarr = (ADCL_array_t *) malloc ( sizeof(ADCL_array_t) );
    if ( NULL == tarr ) {
	return ADCL_NO_MEMORY;
    }

    strncpy ( tarr->name, name, 64 );
    tarr->last = 0;
    tarr->size = size;

    tarr->array=(ADCL_array_elem_t*)calloc(1,size*sizeof(ADCL_array_elem_t));
    if ( NULL == tarr->array ) {
	free ( tarr );
	return ADCL_NO_MEMORY;
    }
    
    *arr = tarr;
    return ADCL_SUCCESS;
}

/* 
** Right now, we are not checking whether the element array is 
** really free
*/
int ADCL_array_free (ADCL_array_t **arr )
{
    ADCL_array_t *tarr=*arr;

    if ( NULL != tarr) {
	if ( NULL != tarr->array ) {
	    free ( tarr->array );
	}
	free ( tarr );
    }
    *arr = NULL;
    return ADCL_SUCCESS;
}

int ADCL_array_get_next_free_pos  ( ADCL_array_t *arr, int *pos)
{
    ADCL_array_elem_t *oldarray=NULL;
    int oldsize;
    int i;
    int thispos=-1;

    if ( arr->last < (arr->size -1) ) {
	for (i=0; i< arr->size; i++ ) {
	    if ( arr->array[i].in_use == FALSE ) {
		thispos = i;
		break;
	    }
	}
    }
    else {
	oldarray = arr->array;
	oldsize  = arr->size;

	arr->size *= 2;
	arr->array = (ADCL_array_elem_t*) calloc ( 1, arr->size * 
						   sizeof(ADCL_array_elem_t));
	if ( NULL == arr->array ) {
	    *pos = -1;
	    return ADCL_NO_MEMORY;
	}

	memcpy ( arr->array, oldarray, oldsize * sizeof(ADCL_array_elem_t));	
	free (oldarray);
	thispos = oldsize;
    }
    
    
    *pos = thispos;
    return ADCL_SUCCESS;
}

void * ADCL_array_get_ptr_by_pos ( ADCL_array_t *arr, int pos )
{
    if ( pos < arr->size ) {
	return arr->array[pos].ptr;
    }

    return NULL;
}

void * ADCL_array_get_ptr_by_id  ( ADCL_array_t *arr, int id  )
{
    int i;

    for (i=0; i<arr->size; i++ ) {
	if ( arr->array[i].id == id ) {
	    return arr->array[i].ptr;
	}
    }

    return NULL;
}

int ADCL_array_get_size ( ADCL_array_t *arr ) 
{
    return arr->size;
}

int ADCL_array_get_last ( ADCL_array_t *arr ) 
{
    return arr->last;
}

int ADCL_array_set_element (ADCL_array_t *arr, int pos, int id, void *ptr)
{
    int ret = ADCL_ERROR_INTERNAL;

    if ( pos < arr->size ) {
	arr->array[pos].id  = id;
	arr->array[pos].ptr = ptr;
	ret = ADCL_SUCCESS;
    }

    if ( pos > arr->last ) {
	arr->last = thispos;
    }
    
    return ret;
}    

int ADCL_array_remove_element ( ADCL_array_t *arr, int pos )
{
    if ( arr->array[pos].in_use = TRUE ) {
	if ( NULL != arr->array[pos].ptr ) {
	    free ( arr->array[pos].ptr ) ;
	}
	arr->array[pos].ptr    = NULL;
	arr->array[pos].id     = MPI_UNDEFINED;
	arr->array[pos].in_use = FALSE;
    }
	    
    return ADCL_SUCCESS;
}
