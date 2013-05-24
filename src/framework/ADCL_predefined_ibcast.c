#include <math.h>
#include "ADCL_ibcast.h"
#include "ADCL_config.h"

ADCL_function_t *ADCL_ibcast_functions[ADCL_METHOD_IBCAST_TOTAL_NUM];
ADCL_fnctset_t *ADCL_ibcast_fnctset;

int ADCL_predefined_ibcast( void )
{
    int count=0;

/* ******************************************************************** */
/* IBCAST - Fortran function set 3                                   	*/
/* ******************************************************************** */

    count = 0;

    ADCL_function_create_async ( ADCL_ibcast_linear, ADCL_ibcast_wait,
                                 ADCL_ATTRSET_NULL,
                                 NULL, "Ibcast_linear",
                                 & ADCL_ibcast_functions[count]);
    count++;

    ADCL_function_create_async ( ADCL_ibcast_binomial, ADCL_ibcast_wait,
                                 ADCL_ATTRSET_NULL,
                                 NULL, "Ibcast_binomial",
                                 & ADCL_ibcast_functions[count]);
    count++;
	
	ADCL_function_create_async ( ADCL_ibcast_chain, ADCL_ibcast_wait,
                                 ADCL_ATTRSET_NULL,
                                 NULL, "Ibcast_chain",
                                 & ADCL_ibcast_functions[count]);
    count++;



    ADCL_fnctset_create ( ADCL_METHOD_IBCAST_TOTAL_NUM,
                          ADCL_ibcast_functions,
                          "Ibcast",
                          &ADCL_ibcast_fnctset );


    if ( count != ADCL_METHOD_IBCAST_TOTAL_NUM) {
        ADCL_printf("Total Number wrong\n");
        return ADCL_ERROR_INTERNAL;
    }

    return ADCL_SUCCESS;
}
