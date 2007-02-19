#include "ADCL.h"
#include "ADCL_internal.h"
#include "ADCL_fprototypes.h"


#pragma weak adcl_fnctgrp_create_  = adcl_fnctgrp_create
#pragma weak adcl_fnctgrp_create__ = adcl_fnctgrp_create
#pragma weak ADCL_FNCTGRP_CREATE   = adcl_fnctgrp_create

#pragma weak adcl_fnctgrp_free_  = adcl_fnctgrp_free
#pragma weak adcl_fnctgrp_free__ = adcl_fnctgrp_free
#pragma weak ADCL_FNCTGRP_FREE   = adcl_fnctgrp_free

#pragma weak adcl_fnctgrp_register_fnct_   = adcl_fnctgrp_register_fnct
#pragma weak adcl_fnctgrp_register_fnct__  = adcl_fnctgrp_register_fnct
#pragma weak ADCL_FNCTGRP_REGISTER_FNCT    = adcl_fnctgrp_register_fnct

#pragma weak adcl_fnctgrp_register_fnct_and_attrset_   = adcl_fnctgrp_register_fnct_and_attrset
#pragma weak adcl_fnctgrp_register_fnct_and_attrset__  = adcl_fnctgrp_register_fnct_and_attrset
#pragma weak ADCL_FNCTGRP_REGISTER_FNCT_AND_ATTRSET    = adcl_fnctgrp_register_fnct_and_attrset

void adcl_fnctgrp_create ( int* maxnum, int *fctgrp, int *ierr )
{
    ADCL_fnctgrp_t *cfnctgrp;

    if ( ( NULL == maxnum ) ||
	 ( NULL == fctgrp ) ) {
	*ierr = ADCL_INVALID_ARG;
	return;
    }

    *ierr = ADCL_fnctgrp_create ( *maxnum, &cfnctgrp );
    if ( *ierr == ADCL_SUCCESS ) {
	*fctgrp = cfnctgrp->f_findex;
    }
    return;
}

void adcl_fnctgrp_free ( int *fctgrp, int *ierr )
{
    ADCL_fnctgrp_t *cfnctgrp;

    if (  NULL == fctgrp )  {
	*ierr = ADCL_INVALID_ARG;
	return;
    }

    cfnctgrp = (ADCL_fnctgrp_t *)  ADCL_array_get_ptr_by_pos (ADCL_fnctgrp_farray, *fctgrp );
    *ierr = ADCL_fnctgrp_free ( &cfnctgrp );
    return;
}

void adcl_fnctgrp_register_fnct ( int *fctgrp, int *cnt, 
				 void *fct, int *ierr )
{
    ADCL_fnctgrp_t *cfnctgrp;

    if (  ( NULL == fctgrp )  ||
	  ( NULL == cnt )     ||
	  ( NULL == fct )   )   {
	*ierr = ADCL_INVALID_ARG;
	return;
    }

    cfnctgrp = (ADCL_fnctgrp_t *) ADCL_array_get_ptr_by_pos (ADCL_fnctgrp_farray, *fctgrp );
    *ierr = ADCL_fnctgrp_register_fnct ( cfnctgrp, *cnt, fct );
    return;
}

void adcl_fnctgrp_register_fnct_and_attrset ( int *fctgrp, int *cnt, 
					     void *fct, int *attrset, 
					     int *array_of_attrvalues, 
					     int *ierr )
{
    ADCL_fnctgrp_t *cfnctgrp;
    ADCL_attrset_t *cattrset;

    if (  ( NULL == fctgrp )  ||
	  ( NULL == cnt )     ||
	  ( NULL == fct )     ||
	  ( NULL == attrset ) ||
	  ( NULL == array_of_attrvalues )){
	*ierr = ADCL_INVALID_ARG;
	return;
    }

    cfnctgrp = (ADCL_fnctgrp_t *) ADCL_array_get_ptr_by_pos ( ADCL_fnctgrp_farray, *fctgrp );
    cattrset = (ADCL_attrset_t *) ADCL_array_get_ptr_by_pos ( ADCL_attrset_farray, *attrset );
    *ierr = ADCL_fnctgrp_register_fnct_and_attrset ( cfnctgrp, *cnt, fct, cattrset, 
						     array_of_attrvalues );
    return;
}


