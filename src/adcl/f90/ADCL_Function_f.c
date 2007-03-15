#include "ADCL.h"
#include "ADCL_internal.h"
#include "ADCL_fprototypes.h"

#pragma weak adcl_function_create_   = adcl_function_create
#pragma weak adcl_function_create__  = adcl_function_create
#pragma weak ADCL_FUNCTION_CREATE    = adcl_function_create

#pragma weak adcl_function_create_async_  = adcl_function_create_async
#pragma weak adcl_function_create_async__ = adcl_function_create_async
#pragma weak ADCL_FUNCTION_CREATE_ASYNC   = adcl_function_create_async

#pragma weak adcl_function_free_   = adcl_function_free
#pragma weak adcl_function_free__  = adcl_function_free
#pragma weak ADCL_FUNCTION_FREE    = adcl_function_free


#pragma weak adcl_fnctset_create_  = adcl_fnctset_create
#pragma weak adcl_fnctset_create__ = adcl_fnctset_create
#pragma weak ADCL_FNCTSET_CREATE   = adcl_fnctset_create

#pragma weak adcl_fnctset_free_  = adcl_fnctset_free
#pragma weak adcl_fnctset_free__ = adcl_fnctset_free
#pragma weak ADCL_FNCTSET_FREE   = adcl_fnctset_free


void adcl_function_create ( void *iptr, int *attrset, int *array_of_attrvals, char *name, 
			    int *fnct, int *ierr )
{
    ADCL_attrset_t *cattrset;
    ADCL_function_t *cfunction;

    if ( ( NULL == iptr )    ||
	 ( NULL == attrset ) ||
	 ( NULL == array_of_attrvals ) ||
	 ( NULL == fnct ) ) {
	*ierr = ADCL_INVALID_ARG;
	return;
    }
    
    if ( ADCL_FATTRSET_NULL == *attrset ) {
	*ierr = ADCL_Function_create ( iptr, ADCL_ATTRSET_NULL, NULL, NULL, &cfunction );
    }
    else {
	cattrset = ( ADCL_attrset_t *) ADCL_array_get_ptr_by_pos ( ADCL_attrset_farray, *attrset );
	if ( NULL == cattrset ) {
	    *ierr = ADCL_INVALID_ATTRSET;
	    return;
	}
	*ierr = ADCL_Function_create ( iptr, cattrset, array_of_attrvals, NULL, &cfunction );
    }
    if ( ADCL_SUCCESS == *ierr ) {
	*fnct = cfunction->f_findex;
    }

    return;
}

void adcl_function_create_async ( void *iptr, void *wptr, int *attrset, int *array_of_attrvals, 
				  char *name, int *fnct, int *ierr )
{
    ADCL_attrset_t *cattrset;
    ADCL_function_t *cfunction;

    if ( ( NULL == iptr )    ||
	 ( NULL == wptr )    ||
	 ( NULL == attrset ) ||
	 ( NULL == array_of_attrvals ) ||
	 ( NULL == fnct ) ) {
	*ierr = ADCL_INVALID_ARG;
	return;
    }

    if ( ADCL_FATTRSET_NULL == *attrset ) {
	*ierr = ADCL_Function_create_async ( iptr, wptr, ADCL_ATTRSET_NULL, NULL, NULL, &cfunction );
    }
    else {
	cattrset = ( ADCL_attrset_t *) ADCL_array_get_ptr_by_pos ( ADCL_attrset_farray, *attrset );
	if ( NULL == cattrset ) {
	    *ierr = ADCL_INVALID_ATTRSET;
	    return;
	}
    }
    *ierr = ADCL_Function_create_async ( iptr, wptr, cattrset, array_of_attrvals, NULL, &cfunction );
    if ( ADCL_SUCCESS == *ierr ) {
	*fnct = cfunction->f_findex;
    }

    return;
}

void adcl_function_free ( int *fnct, int *ierr  )
{
    ADCL_function_t *cfunction;

    if ( NULL == fnct ) {
        *ierr = ADCL_INVALID_ARG;
	return;
    }

    cfunction = (ADCL_function_t *) ADCL_array_get_ptr_by_pos ( ADCL_function_farray, *fnct );
    if ( NULL == cfunction ) {
	*ierr = ADCL_INVALID_FUNCTION;
	return;
    }
    *ierr = ADCL_Function_free ( &cfunction );

    fnct = ADCL_FUNCTION_NULL;
    return;
}

void adcl_fnctset_create ( int* maxnum, int *array_of_fncts, char *name, int *fctset, int *ierr )
{
    ADCL_fnctset_t *cfnctset;
    ADCL_function_t **cfncts;
    int i;

    if ( ( NULL == maxnum )        ||
	 ( NULL == array_of_fncts) ||
	 ( NULL == fctset ) ) {
	*ierr = ADCL_INVALID_ARG;
	return;
    }
    
    cfncts = ( ADCL_function_t **) malloc ( *maxnum  * sizeof  ( ADCL_function_t *));
    if ( NULL == cfncts ) {
	*ierr = ADCL_NO_MEMORY;
	return;
    }
    else {
	for ( i=0; i< *maxnum; i++) {
	    cfncts[i] = (ADCL_function_t *) ADCL_array_get_ptr_by_pos ( ADCL_function_farray, 
									array_of_fncts[i] );
	    if ( NULL == cfncts[i] ) {
		*ierr = ADCL_INVALID_FUNCTION;
		return;
	    }
	}
    }

    *ierr = ADCL_fnctset_create ( *maxnum, cfncts, NULL, &cfnctset );
    if ( *ierr == ADCL_SUCCESS ) {
	*fctset = cfnctset->fs_findex;
    }

    free ( cfncts );
    return;
}

void adcl_fnctset_free ( int *fctset, int *ierr )
{
    ADCL_fnctset_t *cfnctset;

    if (  NULL == fctset )  {
	*ierr = ADCL_INVALID_ARG;
	return;
    }

    cfnctset = (ADCL_fnctset_t *)  ADCL_array_get_ptr_by_pos (ADCL_fnctset_farray, *fctset );
    if ( NULL == cfnctset ) {
	*ierr = ADCL_INVALID_FNCTSET;
	return;
    }

    *ierr = ADCL_fnctset_free ( &cfnctset );
    return;
}



