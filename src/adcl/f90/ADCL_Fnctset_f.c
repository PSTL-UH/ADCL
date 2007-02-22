#include "ADCL.h"
#include "ADCL_internal.h"
#include "ADCL_fprototypes.h"


#pragma weak adcl_fnctset_create_  = adcl_fnctset_create
#pragma weak adcl_fnctset_create__ = adcl_fnctset_create
#pragma weak ADCL_FNCTSET_CREATE   = adcl_fnctset_create

#pragma weak adcl_fnctset_free_  = adcl_fnctset_free
#pragma weak adcl_fnctset_free__ = adcl_fnctset_free
#pragma weak ADCL_FNCTSET_FREE   = adcl_fnctset_free

#pragma weak adcl_fnctset_register_fnct_   = adcl_fnctset_register_fnct
#pragma weak adcl_fnctset_register_fnct__  = adcl_fnctset_register_fnct
#pragma weak ADCL_FNCTSET_REGISTER_FNCT    = adcl_fnctset_register_fnct

#pragma weak adcl_fnctset_register_fnct_and_attrset_   = adcl_fnctset_register_fnct_and_attrset
#pragma weak adcl_fnctset_register_fnct_and_attrset__  = adcl_fnctset_register_fnct_and_attrset
#pragma weak ADCL_FNCTSET_REGISTER_FNCT_AND_ATTRSET    = adcl_fnctset_register_fnct_and_attrset

void adcl_fnctset_create ( int* maxnum, int *fctset, int *ierr )
{
    ADCL_fnctset_t *cfnctset;

    if ( ( NULL == maxnum ) ||
	 ( NULL == fctset ) ) {
	*ierr = ADCL_INVALID_ARG;
	return;
    }

    *ierr = ADCL_fnctset_create ( *maxnum, &cfnctset );
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

void adcl_fnctset_register_fnct ( int *fctset, int *cnt, 
				 void *fct, int *ierr )
{
    ADCL_fnctset_t *cfnctset;

    if (  ( NULL == fctset )  ||
	  ( NULL == cnt )     ||
	  ( NULL == fct )   )   {
	*ierr = ADCL_INVALID_ARG;
	return;
    }

    cfnctset = (ADCL_fnctset_t *) ADCL_array_get_ptr_by_pos (ADCL_fnctset_farray, *fctset );
    *ierr = ADCL_fnctset_register_fnct ( cfnctset, *cnt, fct );
    return;
}

void adcl_fnctset_register_fnct_and_attrset ( int *fctset, int *cnt, 
					     void *fct, int *attrset, 
					     int *array_of_attrvalues, 
					     int *ierr )
{
    ADCL_fnctset_t *cfnctset;
    ADCL_attrset_t *cattrset;

    if (  ( NULL == fctset )  ||
	  ( NULL == cnt )     ||
	  ( NULL == fct )     ||
	  ( NULL == attrset ) ||
	  ( NULL == array_of_attrvalues )){
	*ierr = ADCL_INVALID_ARG;
	return;
    }

    cfnctset = (ADCL_fnctset_t *) ADCL_array_get_ptr_by_pos ( ADCL_fnctset_farray, *fctset );
    cattrset = (ADCL_attrset_t *) ADCL_array_get_ptr_by_pos ( ADCL_attrset_farray, *attrset );
    *ierr = ADCL_fnctset_register_fnct_and_attrset ( cfnctset, *cnt, fct, cattrset, 
						     array_of_attrvalues );
    return;
}


