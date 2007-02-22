#include "ADCL.h"
#include "ADCL_internal.h"

int ADCL_Fnctset_create ( int maxnum, ADCL_Fnctset *fctset )
{
    if ( 0 >= maxnum ) {
	return ADCL_INVALID_ARG;
    }
    if ( NULL == fctset ) {
	return ADCL_INVALID_ARG;
    }

    return ADCL_fnctset_create ( maxnum, fctset );
}

int ADCL_Fnctset_free ( ADCL_Fnctset *fctset )
{
    if ( NULL == fctset ) {
	return ADCL_INVALID_ARG;
    }

    if ( (*fctset)->f_id < 0 ) {
	return ADCL_INVALID_FNCTSET;
    }

    return ADCL_fnctset_free ( fctset );
}



int ADCL_Fnctset_register_fnct ( ADCL_Fnctset fctset, int cnt, 
				 ADCL_work_fnct_ptr *fct )
{

    if ( fctset->f_id < 0 ) {
	return ADCL_INVALID_FNCTSET;
    }
    if ( 0 > cnt ) {
	return ADCL_INVALID_ARG;
    }
    if ( NULL == fct ) {
	return ADCL_INVALID_ARG;
    }
    
    return ADCL_fnctset_register_fnct ( fctset, cnt, fct );
}


int ADCL_Fnctset_register_fnct_and_attrset ( ADCL_Fnctset fctset, int cnt, 
					     ADCL_work_fnct_ptr *fct, 
					     ADCL_Attrset attrset, 
					     int *array_of_attrvalues )
{
    if ( fctset->f_id < 0 ) {
	return ADCL_INVALID_FNCTSET;
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

    return ADCL_fnctset_register_fnct_and_attrset ( fctset, cnt, fct, attrset, 
						    array_of_attrvalues );
}
