/*
 * Copyright (c) 2007-2009      HLRS. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include <math.h>
#include "ADCL_ireduce.h"
#include "ADCL_config.h"

const int ADCL_attr_ireduce_alg_binomial=1000;
const int ADCL_attr_ireduce_alg_chain=1001;

const int ADCL_attr_ireduce_segsize_32 = 32;
const int ADCL_attr_ireduce_segsize_64 = 64;
const int ADCL_attr_ireduce_segsize_128 = 128;
const int ADCL_attr_ireduce_segsize_null = 0;

ADCL_attribute_t *ADCL_ireduce_attrs[ADCL_ATTR_IREDUCE_TOTAL_NUM];
ADCL_attrset_t *ADCL_ireduce_attrset;
ADCL_function_t *ADCL_ireduce_functions[ADCL_METHOD_IREDUCE_TOTAL_NUM];
ADCL_fnctset_t *ADCL_ireduce_fnctset;


int ADCL_predefined_ireduce ( void )
{
    int count;
    int m_ireduce_attrs[ADCL_ATTR_IREDUCE_TOTAL_NUM];
    int ADCL_attr_ireduce_alg[ADCL_ATTR_IREDUCE_ALG_MAX];
    int ADCL_attr_ireduce_segsize[ADCL_ATTR_IREDUCE_SEGSIZE_MAX];

    char * ADCL_attr_ireduce_alg_names[ADCL_ATTR_IREDUCE_ALG_MAX] = 
      {"Ireduce_binomial", "Ireduce_chain"};
    char * ADCL_attr_ireduce_segsize_names[ADCL_ATTR_IREDUCE_SEGSIZE_MAX] = { "32","64","128" };

    ADCL_attr_ireduce_alg[0] = ADCL_attr_ireduce_alg_binomial;
    ADCL_attr_ireduce_alg[1] = ADCL_attr_ireduce_alg_chain;

    ADCL_attr_ireduce_segsize[0] = ADCL_attr_ireduce_segsize_32;
    ADCL_attr_ireduce_segsize[1] = ADCL_attr_ireduce_segsize_64;
    ADCL_attr_ireduce_segsize[2] = ADCL_attr_ireduce_segsize_128;

    ADCL_attribute_create ( ADCL_ATTR_IREDUCE_ALG_MAX, ADCL_attr_ireduce_alg, 
			    ADCL_attr_ireduce_alg_names, "ireduce_alg", 
			    &ADCL_ireduce_attrs[0] );

    ADCL_attribute_create ( ADCL_ATTR_IREDUCE_SEGSIZE_MAX, ADCL_attr_ireduce_segsize, 
			    ADCL_attr_ireduce_segsize_names, "ireduce_segsize", 
			    &ADCL_ireduce_attrs[1] );
    
    ADCL_attrset_create ( ADCL_ATTR_IREDUCE_TOTAL_NUM, ADCL_ireduce_attrs, &ADCL_ireduce_attrset );

    count=0;

    m_ireduce_attrs[0] = ADCL_attr_ireduce_alg_binomial;
    m_ireduce_attrs[1] = ADCL_attr_ireduce_segsize_null;
    ADCL_function_create_async ( ADCL_ireduce_binomial, ADCL_ireduce_wait, ADCL_ireduce_attrset,
        			 m_ireduce_attrs, "Ireduce_binomial", 
        			 &ADCL_ireduce_functions[count]);
    count++;

    m_ireduce_attrs[0] = ADCL_attr_ireduce_alg_chain;
    m_ireduce_attrs[1] = ADCL_attr_ireduce_segsize_32;
    ADCL_function_create_async ( ADCL_ireduce_chain, ADCL_ireduce_wait, ADCL_ireduce_attrset,
        			 m_ireduce_attrs, "Ireduce_chain_32",
        			 &ADCL_ireduce_functions[count]);
    count++;

    m_ireduce_attrs[0] = ADCL_attr_ireduce_alg_chain;
    m_ireduce_attrs[1] = ADCL_attr_ireduce_segsize_64;
    ADCL_function_create_async ( ADCL_ireduce_chain, ADCL_ireduce_wait, ADCL_ireduce_attrset,
        			 m_ireduce_attrs, "Ireduce_chain_64",
        			 &ADCL_ireduce_functions[count]);
    count++;


    m_ireduce_attrs[0] = ADCL_attr_ireduce_alg_chain;
    m_ireduce_attrs[1] = ADCL_attr_ireduce_segsize_128;
    ADCL_function_create_async ( ADCL_ireduce_chain, ADCL_ireduce_wait, ADCL_ireduce_attrset,
        			 m_ireduce_attrs, "Ireduce_chain_128",
        			 &ADCL_ireduce_functions[count]);
    count++;

    if ( count != ADCL_METHOD_IREDUCE_TOTAL_NUM ) {
	ADCL_printf ("Ireduce: Total Number wrong\n");
	return ADCL_ERROR_INTERNAL;
    }

    ADCL_fnctset_create ( ADCL_METHOD_IREDUCE_TOTAL_NUM, 
			  ADCL_ireduce_functions, 
			  "Ireduce",
			  &ADCL_ireduce_fnctset );

    return ADCL_SUCCESS;
}
