/*
 * Copyright (c) 2007-2009      HLRS. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include <math.h>
#include "ADCL_alltoall.h"
#include "ADCL_config.h"

const int ADCL_attr_alltoall_alg_linear_sync=100;
const int ADCL_attr_alltoall_alg_ladd_block2=101;
const int ADCL_attr_alltoall_alg_ladd_block4=102;
const int ADCL_attr_alltoall_alg_ladd_block8=103;
const int ADCL_attr_alltoall_alg_pairwise_excl=104;
const int ADCL_attr_alltoall_alg_linear=105;
const int ADCL_attr_alltoall_alg_pairwise=106;

ADCL_attribute_t *ADCL_alltoall_attrs[ADCL_ATTR_ALLTOALL_TOTAL_NUM];
ADCL_attrset_t *ADCL_alltoall_attrset;
ADCL_function_t *ADCL_alltoall_functions[ADCL_METHOD_ALLTOALL_TOTAL_NUM];
ADCL_fnctset_t *ADCL_alltoall_fnctset;


int ADCL_predefined_alltoall ( void )
{
    int count;
    int m_alltoall_attrs[ADCL_ATTR_ALLTOALL_TOTAL_NUM];
    int ADCL_attr_alltoall_alg[ADCL_ATTR_ALLTOALL_ALG_MAX];
    
    char * ADCL_attr_alltoall_alg_names[ADCL_ATTR_ALLTOALL_ALG_MAX] = {"Alltoall_linear_sync","Alltoall_ladd_block2","Alltoall_ladd_block4","Alltoall_ladd_block8","Alltoall_pairwise_excl","Alltoall_linear","Alltoall_pairwise"};

    ADCL_attr_alltoall_alg[0] = ADCL_attr_alltoall_alg_linear_sync;
    ADCL_attr_alltoall_alg[1] = ADCL_attr_alltoall_alg_ladd_block2;
    ADCL_attr_alltoall_alg[2] = ADCL_attr_alltoall_alg_ladd_block4;
    ADCL_attr_alltoall_alg[3] = ADCL_attr_alltoall_alg_ladd_block8;
    ADCL_attr_alltoall_alg[4] = ADCL_attr_alltoall_alg_pairwise_excl;
    ADCL_attr_alltoall_alg[5] = ADCL_attr_alltoall_alg_linear;
    ADCL_attr_alltoall_alg[6] = ADCL_attr_alltoall_alg_pairwise;

    ADCL_attribute_create ( ADCL_ATTR_ALLTOALL_ALG_MAX, ADCL_attr_alltoall_alg, 
			    ADCL_attr_alltoall_alg_names, "alltoall_alg", 
			    &ADCL_alltoall_attrs[0] );
    
    ADCL_attrset_create ( ADCL_ATTR_ALLTOALL_TOTAL_NUM, ADCL_alltoall_attrs, &ADCL_alltoall_attrset );

    count=0;
    m_alltoall_attrs[0] = ADCL_attr_alltoall_alg_linear;
    ADCL_function_create_async ( ADCL_alltoall_linear, NULL, ADCL_alltoall_attrset,
        			 m_alltoall_attrs, "Alltoall_linear_SR", 
        			 &ADCL_alltoall_functions[count]);
    count++;

    m_alltoall_attrs[0] = ADCL_attr_alltoall_alg_linear_sync;
    ADCL_function_create_async ( ADCL_alltoall_linear_sync, NULL, ADCL_alltoall_attrset,
        			 m_alltoall_attrs, "Alltoall_linear_sync_SR", 
        			 &ADCL_alltoall_functions[count]);
    count++;

    m_alltoall_attrs[0] = ADCL_attr_alltoall_alg_ladd_block2;
    ADCL_function_create_async ( ADCL_alltoall_ladd_block2, NULL, ADCL_alltoall_attrset,
        			 m_alltoall_attrs, "Alltoall_ladd_block2", 
        			 &ADCL_alltoall_functions[count]);
    count++;

    m_alltoall_attrs[0] = ADCL_attr_alltoall_alg_ladd_block4;
    ADCL_function_create_async ( ADCL_alltoall_ladd_block4, NULL, ADCL_alltoall_attrset,
        			 m_alltoall_attrs, "Alltoall_ladd_block4", 
        			 &ADCL_alltoall_functions[count]);
    count++;

    m_alltoall_attrs[0] = ADCL_attr_alltoall_alg_ladd_block8;
    ADCL_function_create_async ( ADCL_alltoall_ladd_block8, NULL, ADCL_alltoall_attrset,
        			 m_alltoall_attrs, "Alltoall_ladd_block8", 
        			 &ADCL_alltoall_functions[count]);
    count++;

    m_alltoall_attrs[0] = ADCL_attr_alltoall_alg_pairwise;
    ADCL_function_create_async ( ADCL_alltoall_pairwise, NULL, ADCL_alltoall_attrset,
        			 m_alltoall_attrs, "Alltoall_pair_SR", 
        			 &ADCL_alltoall_functions[count]);
    count++;

    m_alltoall_attrs[0] = ADCL_attr_alltoall_alg_pairwise_excl;
    ADCL_function_create_async ( ADCL_alltoall_pairwise_excl, NULL, ADCL_alltoall_attrset,
        			 m_alltoall_attrs, "Alltoall_pair_excl_SR", 
        			 &ADCL_alltoall_functions[count]);
    count++;


    if ( count != ADCL_METHOD_ALLTOALL_TOTAL_NUM ) {
	ADCL_printf ("Alltoall: Total Number wrong\n");
	return ADCL_ERROR_INTERNAL;
    }

    ADCL_fnctset_create ( ADCL_METHOD_ALLTOALL_TOTAL_NUM, 
			  ADCL_alltoall_functions, 
			  "Alltoall",
			  &ADCL_alltoall_fnctset );

    return ADCL_SUCCESS;
}
