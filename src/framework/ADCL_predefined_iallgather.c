/*
 * Copyright (c) 2007-2009      HLRS. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include <math.h>
#include "ADCL_iallgather.h"
#include "ADCL_config.h"

const int ADCL_attr_iallgather_alg_linear=1000; 

ADCL_attribute_t *ADCL_iallgather_attrs[ADCL_ATTR_IALLGATHER_TOTAL_NUM];
ADCL_attrset_t *ADCL_iallgather_attrset;
ADCL_function_t *ADCL_iallgather_functions[ADCL_METHOD_IALLGATHER_TOTAL_NUM];
ADCL_fnctset_t *ADCL_iallgather_fnctset;


int ADCL_predefined_iallgather ( void )
{
    int count;
    int m_iallgather_attrs[ADCL_ATTR_IALLGATHER_TOTAL_NUM];
    int ADCL_attr_iallgather_alg[ADCL_ATTR_IALLGATHER_ALG_MAX];

    char * ADCL_attr_iallgather_alg_names[ADCL_ATTR_IALLGATHER_ALG_MAX] = 
      {"Iallgather_linear"};

    ADCL_attr_iallgather_alg[0] = ADCL_attr_iallgather_alg_linear;


    ADCL_attribute_create ( ADCL_ATTR_IALLGATHER_ALG_MAX, ADCL_attr_iallgather_alg, 
			    ADCL_attr_iallgather_alg_names, "iallgather_alg", 
			    &ADCL_iallgather_attrs[0] );
    
    ADCL_attrset_create ( ADCL_ATTR_IALLGATHER_TOTAL_NUM, ADCL_iallgather_attrs, &ADCL_iallgather_attrset );

    count=0;

    m_iallgather_attrs[0] = ADCL_attr_iallgather_alg_linear;
    ADCL_function_create_async ( ADCL_iallgather_linear, ADCL_iallgather_wait, ADCL_iallgather_attrset,
        			 m_iallgather_attrs, "Iallgather_linear", 
        			 &ADCL_iallgather_functions[count]);
    count++;


    if ( count != ADCL_METHOD_IALLGATHER_TOTAL_NUM ) {
	ADCL_printf ("Iallgather: Total Number wrong\n");
	return ADCL_ERROR_INTERNAL;
    }

    ADCL_fnctset_create ( ADCL_METHOD_IALLGATHER_TOTAL_NUM, 
			  ADCL_iallgather_functions, 
			  "Iallgather",
			  &ADCL_iallgather_fnctset );

    return ADCL_SUCCESS;
}
