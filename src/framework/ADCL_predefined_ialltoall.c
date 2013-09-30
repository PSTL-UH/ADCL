/*
 * Copyright (c) 2007-2009      HLRS. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include <math.h>
#include "ADCL_ialltoall.h"
#include "ADCL_config.h"

const int ADCL_attr_ialltoall_alg_linear=1000; 
const int ADCL_attr_ialltoall_alg_pairwise=1001;
const int ADCL_attr_ialltoall_alg_diss=1002;

ADCL_attribute_t *ADCL_ialltoall_attrs[ADCL_ATTR_IALLTOALL_TOTAL_NUM];
ADCL_attrset_t *ADCL_ialltoall_attrset;
ADCL_function_t *ADCL_ialltoall_functions[ADCL_METHOD_IALLTOALL_TOTAL_NUM];
ADCL_fnctset_t *ADCL_ialltoall_fnctset;


int ADCL_predefined_ialltoall ( void )
{
    int count;
    int m_ialltoall_attrs[1];
    int ADCL_attr_ialltoall_alg[ADCL_ATTR_IALLTOALL_ALG_MAX];

    char * ADCL_attr_ialltoall_alg_names[ADCL_ATTR_ALLTOALL_ALG_MAX] = 
        {"Ialltoall_linear",  "Ialltoall_pair", "Ialltoall_diss" };

    ADCL_attr_ialltoall_alg[0] = ADCL_attr_ialltoall_alg_linear;
    ADCL_attr_ialltoall_alg[1] = ADCL_attr_ialltoall_alg_pairwise;
    ADCL_attr_ialltoall_alg[2] = ADCL_attr_ialltoall_alg_diss;


    ADCL_attribute_create ( ADCL_ATTR_IALLTOALL_ALG_MAX, ADCL_attr_ialltoall_alg, 
			    ADCL_attr_ialltoall_alg_names, "ialltoall_alg", 
			    &ADCL_ialltoall_attrs[0] );
    
    ADCL_attrset_create ( ADCL_ATTR_IALLTOALL_TOTAL_NUM, ADCL_ialltoall_attrs, &ADCL_ialltoall_attrset );

    count=0;

    m_ialltoall_attrs[0] = ADCL_attr_ialltoall_alg_linear;
    ADCL_function_create_async ( ADCL_ialltoall_linear, ADCL_ialltoall_wait, ADCL_ialltoall_attrset,
        			 m_ialltoall_attrs, "Ialltoall_linear", 
        			 &ADCL_ialltoall_functions[count]);
    count++;

    m_ialltoall_attrs[0] = ADCL_attr_ialltoall_alg_pairwise;
    ADCL_function_create_async ( ADCL_ialltoall_pairwise, ADCL_ialltoall_wait, ADCL_ialltoall_attrset,
        			 m_ialltoall_attrs, "Ialltoall_pairwise", 
        			 &ADCL_ialltoall_functions[count]);
    count++;

    m_ialltoall_attrs[0] = ADCL_attr_ialltoall_alg_diss;
    ADCL_function_create_async ( ADCL_ialltoall_diss, ADCL_ialltoall_wait, ADCL_ialltoall_attrset,
        			 m_ialltoall_attrs, "Ialltoall_diss", 
        			 &ADCL_ialltoall_functions[count]);
    count++;

    if ( count != ADCL_METHOD_IALLTOALL_TOTAL_NUM ) {
	ADCL_printf ("Ialltoall: Total Number wrong\n");
	return ADCL_ERROR_INTERNAL;
    }

    ADCL_fnctset_create ( ADCL_METHOD_IALLTOALL_TOTAL_NUM, 
			  ADCL_ialltoall_functions, 
			  "Ialltoall",
			  &ADCL_ialltoall_fnctset );

    return ADCL_SUCCESS;
}
