#include "ADCL.h"
#include "ADCL_internal.h"

int ADCL_Function_create ( ADCL_work_fnct_ptr *iptr, ADCL_Attrset attrset,
			   int *array_of_values, char *name, 
			   ADCL_Function *fnct );

{
    return ADCL_function_create_async ( iptr, NULL, attrset, array_of_values, 
					name, fnct);
}

int ADCL_Function_create_async ( ADCL_work_fnct_ptr *iptr, ADCL_work_fnct_ptr *wptr, 
				 ADCL_Attrset attrset, int *array_of_values, 
				 char *name, ADCL_Function *fnct )
{
    return ADCL_function_create_async ( iptr, wptr, attrset, array_of_values, 
					name, fnct );
}

int ADCL_Function_free ( ADCL_Function *fnct )
{

    return ADCL_function_free (fnct);
}

int ADCL_Fnctset_create ( int maxnum, ADCL_Function *fncts, 
			  char *name, ADCL_Fnctset *fctset )
{
    if ( 0 >= maxnum ) {
	return ADCL_INVALID_ARG;
    }
    if ( NULL == fctset ) {
	return ADCL_INVALID_ARG;
    }

    return ADCL_fnctset_create ( maxnum, name, fctset );
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



