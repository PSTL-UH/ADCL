#include "ADCL.h"
#include "ADCL_internal.h"

int ADCL_Fnctgrp_create ( int maxnum, ADCL_Fnctgrp *fctgrp )
{
    if ( 0 >= maxnum ) {
	return ADCL_INVALID_ARG;
    }
    if ( NULL == fctgrp ) {
	return ADCL_INVALID_ARG;
    }

    return ADCL_fnctgrp_create ( maxnum, fctgrp );
}

int ADCL_Fnctgrp_free ( ADCL_Fnctgrp *fctgrp )
{
    if ( NULL == fctgrp ) {
	return ADCL_INVALID_ARG;
    }

    if ( (*fctgrp)->f_id < 0 ) {
	return ADCL_INVALID_FNCTGRP;
    }

    return ADCL_fnctgrp_free ( fctgrp );
}



int ADCL_Fnctgrp_register_fnct ( ADCL_Fnctgrp fctgrp, int cnt, 
				 ADCL_work_fnct_ptr *fct )
{

    if ( fctgrp->f_id < 0 ) {
	return ADCL_INVALID_FNCTGRP;
    }
    if ( 0 > cnt ) {
	return ADCL_INVALID_ARG;
    }
    if ( NULL == fct ) {
	return ADCL_INVALID_ARG;
    }
    
    return ADCL_fnctgrp_register_fnct ( fctgrp, cnt, fct );
}


int ADCL_Fnctgrp_register_fnct_and_attrset ( ADCL_Fnctgrp fctgrp, int cnt, 
					     ADCL_work_fnct_ptr *fct, 
					     ADCL_Attrset attrset, 
					     int *array_of_attrvalues )
{
    if ( fctgrp->f_id < 0 ) {
	return ADCL_INVALID_FNCTGRP;
    }
    if ( 0 > cnt ) {
	return ADCL_INVALID_ARG;
    }
    if ( NULL == fct ) {
	return ADCL_INVALID_ARG;
    }
    if ( attrset->as_id < 0 ) {
	return ADCL_INVALID_ATTRSET;
    }
    if ( NULL == array_of_attrvalues ) {
	return ADCL_INVALID_ARG;
    }

    return ADCL_fnctgrp_register_fnct_and_attrset ( fctgrp, cnt, fct, attrset, 
						    array_of_attrvalues );
}
