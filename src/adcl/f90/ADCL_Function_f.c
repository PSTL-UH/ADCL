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


void adcl_function_create ( void *iptr, int *ierr )
{
    return;
}

void adcl_fnctset_create ( int* maxnum, int *array_of_fncts, int *fctset, char *name, int *ierr )
{
    ADCL_fnctset_t *cfnctset;

    if ( ( NULL == maxnum ) ||
	 ( NULL == fctset ) ) {
	*ierr = ADCL_INVALID_ARG;
	return;
    }

    *ierr = ADCL_fnctset_create ( *maxnum, NULL, &cfnctset );
    if ( *ierr == ADCL_SUCCESS ) {
	*fctset = cfnctset->f_findex;
    }
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
    *ierr = ADCL_fnctset_free ( &cfnctset );
    return;
}



