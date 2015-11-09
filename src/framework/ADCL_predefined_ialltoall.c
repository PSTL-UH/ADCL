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
//#include "ADCL_config.h"
//#include "ADCL_internal.h"

const int ADCL_attr_ialltoall_type_bc=0;
const int ADCL_attr_ialltoall_type_nbc=1;

const int ADCL_attr_ialltoall_alg_native=100;
const int ADCL_attr_ialltoall_alg_linear=101;
const int ADCL_attr_ialltoall_alg_linear_sync=102;
const int ADCL_attr_ialltoall_alg_ladd_block2=103;
const int ADCL_attr_ialltoall_alg_ladd_block4=104;
const int ADCL_attr_ialltoall_alg_ladd_block8=105;
const int ADCL_attr_ialltoall_alg_pairwise=106;
const int ADCL_attr_ialltoall_alg_pairwise_excl=107;
const int ADCL_attr_ialltoall_alg_diss=108;

const int ADCL_attr_ialltoall_depth_0=1000;
const int ADCL_attr_ialltoall_depth_1=1001;
const int ADCL_attr_ialltoall_depth_2=1002;
const int ADCL_attr_ialltoall_depth_3=1003;
const int ADCL_attr_ialltoall_depth_4=1004;
const int ADCL_attr_ialltoall_depth_5=1005;


ADCL_attribute_t *ADCL_ialltoall_attrs[ADCL_ATTR_IALLTOALL_TOTAL_NUM];
ADCL_attrset_t *ADCL_ialltoall_attrset;
ADCL_function_t *ADCL_ialltoall_functions[ADCL_METHOD_IALLTOALL_TOTAL_NUM];
ADCL_fnctset_t *ADCL_ialltoall_fnctset;


int ADCL_predefined_ialltoall ( void )
{
    int count;
    int m_ialltoall_attrs[ADCL_ATTR_IALLTOALL_TOTAL_NUM];
    int ADCL_attr_ialltoall_type[ADCL_ATTR_IALLTOALL_TYPE_MAX];
    int ADCL_attr_ialltoall_alg[ADCL_ATTR_IALLTOALL_ALG_MAX];
    int ADCL_attr_ialltoall_depth[ADCL_ATTR_IALLTOALL_DEPTH_MAX];

    char * ADCL_attr_ialltoall_type_names[ADCL_ATTR_IALLTOALL_TYPE_MAX] = {"Blocking","Non-blocking"};
    char * ADCL_attr_ialltoall_alg_names[ADCL_ATTR_IALLTOALL_ALG_MAX] = {"Ialltoall_native","Ialltoall_linear","Ialltoall_linear_sync","Ialltoall_ladd_block2","Ialltoall_ladd_block4","Ialltoall_ladd_block8","Ialltoall_pairwise","Ialltoall_pairwise_excl","Ialltoall_diss"};
    char * ADCL_attr_ialltoall_depth_names[ADCL_ATTR_IALLTOALL_DEPTH_MAX] = {"0 (none)","1","2","3","4","5"};

    ADCL_attr_ialltoall_type[0] = ADCL_attr_ialltoall_type_bc;
    ADCL_attr_ialltoall_type[1] = ADCL_attr_ialltoall_type_nbc;

    ADCL_attr_ialltoall_alg[0] = ADCL_attr_ialltoall_alg_native;
    ADCL_attr_ialltoall_alg[1] = ADCL_attr_ialltoall_alg_linear;
    ADCL_attr_ialltoall_alg[2] = ADCL_attr_ialltoall_alg_linear_sync;
    ADCL_attr_ialltoall_alg[3] = ADCL_attr_ialltoall_alg_ladd_block2;
    ADCL_attr_ialltoall_alg[4] = ADCL_attr_ialltoall_alg_ladd_block4;
    ADCL_attr_ialltoall_alg[5] = ADCL_attr_ialltoall_alg_ladd_block8;
    ADCL_attr_ialltoall_alg[6] = ADCL_attr_ialltoall_alg_pairwise;
    ADCL_attr_ialltoall_alg[7] = ADCL_attr_ialltoall_alg_pairwise_excl;
    ADCL_attr_ialltoall_alg[8] = ADCL_attr_ialltoall_alg_diss;

    ADCL_attr_ialltoall_depth[0] = ADCL_attr_ialltoall_depth_0;
    ADCL_attr_ialltoall_depth[1] = ADCL_attr_ialltoall_depth_1;
    ADCL_attr_ialltoall_depth[2] = ADCL_attr_ialltoall_depth_2;
    ADCL_attr_ialltoall_depth[3] = ADCL_attr_ialltoall_depth_3;
    ADCL_attr_ialltoall_depth[4] = ADCL_attr_ialltoall_depth_4;
    ADCL_attr_ialltoall_depth[5] = ADCL_attr_ialltoall_depth_5;

    ADCL_attribute_create ( ADCL_ATTR_IALLTOALL_TYPE_MAX, ADCL_attr_ialltoall_type, 
			    ADCL_attr_ialltoall_type_names, "collective type", 
			    &ADCL_ialltoall_attrs[0] );
    ADCL_attribute_create ( ADCL_ATTR_IALLTOALL_ALG_MAX, ADCL_attr_ialltoall_alg, 
			    ADCL_attr_ialltoall_alg_names, "algorithm", 
			    &ADCL_ialltoall_attrs[1] );
    ADCL_attribute_create ( ADCL_ATTR_IALLTOALL_DEPTH_MAX, ADCL_attr_ialltoall_depth, 
			    ADCL_attr_ialltoall_depth_names, "depth of progress call", 
			    &ADCL_ialltoall_attrs[2] );
    
    ADCL_attrset_create ( ADCL_ATTR_IALLTOALL_TOTAL_NUM, ADCL_ialltoall_attrs, &ADCL_ialltoall_attrset );

    count=0;

// Native (blocking)
#if 1
    m_ialltoall_attrs[0] = ADCL_attr_ialltoall_type_bc;
    m_ialltoall_attrs[1] = ADCL_attr_ialltoall_alg_native;
    m_ialltoall_attrs[2] = ADCL_attr_ialltoall_depth_0;
    ADCL_function_create_async ( ADCL_alltoall_native, NULL, ADCL_ialltoall_attrset,
        			 m_ialltoall_attrs, "Alltoall_native", 
        			 &ADCL_ialltoall_functions[count]);
    count++;
#endif

// Non-blocking linear
#if 1
    m_ialltoall_attrs[0] = ADCL_attr_ialltoall_type_nbc;
    m_ialltoall_attrs[1] = ADCL_attr_ialltoall_alg_linear;
    m_ialltoall_attrs[2] = ADCL_attr_ialltoall_depth_0;
    ADCL_function_create_async ( ADCL_ialltoall_linear, ADCL_ialltoall_wait, ADCL_ialltoall_attrset,
        			 m_ialltoall_attrs, "Ialltoall_linear_0", 
        			 &ADCL_ialltoall_functions[count]);
    count++;

    m_ialltoall_attrs[0] = ADCL_attr_ialltoall_type_nbc;
    m_ialltoall_attrs[1] = ADCL_attr_ialltoall_alg_linear;
    m_ialltoall_attrs[2] = ADCL_attr_ialltoall_depth_1;
    ADCL_function_create_async ( ADCL_ialltoall_linear, ADCL_ialltoall_wait, ADCL_ialltoall_attrset,
        			 m_ialltoall_attrs, "Ialltoall_linear_1",
        			 &ADCL_ialltoall_functions[count]);
    count++;

    m_ialltoall_attrs[0] = ADCL_attr_ialltoall_type_nbc;
    m_ialltoall_attrs[1] = ADCL_attr_ialltoall_alg_linear;
    m_ialltoall_attrs[2] = ADCL_attr_ialltoall_depth_2;
    ADCL_function_create_async ( ADCL_ialltoall_linear, ADCL_ialltoall_wait, ADCL_ialltoall_attrset,
        			 m_ialltoall_attrs, "Ialltoall_linear_2",
        			 &ADCL_ialltoall_functions[count]);
    count++;

    m_ialltoall_attrs[0] = ADCL_attr_ialltoall_type_nbc;
    m_ialltoall_attrs[1] = ADCL_attr_ialltoall_alg_linear;
    m_ialltoall_attrs[2] = ADCL_attr_ialltoall_depth_3;
    ADCL_function_create_async ( ADCL_ialltoall_linear, ADCL_ialltoall_wait, ADCL_ialltoall_attrset,
        			 m_ialltoall_attrs, "Ialltoall_linear_3",
        			 &ADCL_ialltoall_functions[count]);
    count++;

#if MAX_PROGRESS_FOR_DEPTH == 5
    m_ialltoall_attrs[0] = ADCL_attr_ialltoall_type_nbc;
    m_ialltoall_attrs[1] = ADCL_attr_ialltoall_alg_linear;
    m_ialltoall_attrs[2] = ADCL_attr_ialltoall_depth_4;
    ADCL_function_create_async ( ADCL_ialltoall_linear, ADCL_ialltoall_wait, ADCL_ialltoall_attrset,
        			 m_ialltoall_attrs, "Ialltoall_linear_4",
        			 &ADCL_ialltoall_functions[count]);
    count++;

    m_ialltoall_attrs[0] = ADCL_attr_ialltoall_type_nbc;
    m_ialltoall_attrs[1] = ADCL_attr_ialltoall_alg_linear;
    m_ialltoall_attrs[2] = ADCL_attr_ialltoall_depth_5;
    ADCL_function_create_async ( ADCL_ialltoall_linear, ADCL_ialltoall_wait, ADCL_ialltoall_attrset,
        			 m_ialltoall_attrs, "Ialltoall_linear_5",
        			 &ADCL_ialltoall_functions[count]);
    count++;
#endif
#endif

// Blocking linear
#if 1
    m_ialltoall_attrs[0] = ADCL_attr_ialltoall_type_bc;
    m_ialltoall_attrs[1] = ADCL_attr_ialltoall_alg_linear;
    m_ialltoall_attrs[2] = ADCL_attr_ialltoall_depth_0;

    ADCL_function_create_async ( ADCL_alltoall_linear, NULL, ADCL_ialltoall_attrset,
                                 m_ialltoall_attrs, "Alltoall_linear_SR",
                                 &ADCL_ialltoall_functions[count]);
    count++;

    m_ialltoall_attrs[0] = ADCL_attr_ialltoall_type_bc;
    m_ialltoall_attrs[1] = ADCL_attr_ialltoall_alg_linear_sync;
    m_ialltoall_attrs[2] = ADCL_attr_ialltoall_depth_0;
    ADCL_function_create_async ( ADCL_alltoall_linear_sync, NULL, ADCL_ialltoall_attrset,
                                 m_ialltoall_attrs, "Alltoall_linear_sync_SR",
                                 &ADCL_ialltoall_functions[count]);
    count++;

    m_ialltoall_attrs[0] = ADCL_attr_ialltoall_type_bc;
    m_ialltoall_attrs[1] = ADCL_attr_ialltoall_alg_ladd_block2;
    m_ialltoall_attrs[2] = ADCL_attr_ialltoall_depth_0;
    ADCL_function_create_async ( ADCL_alltoall_ladd_block2, NULL, ADCL_ialltoall_attrset,
                                 m_ialltoall_attrs, "Alltoall_ladd_block2",
                                 &ADCL_ialltoall_functions[count]);
    count++;

    m_ialltoall_attrs[0] = ADCL_attr_ialltoall_type_bc;
    m_ialltoall_attrs[1] = ADCL_attr_ialltoall_alg_ladd_block4;
    m_ialltoall_attrs[2] = ADCL_attr_ialltoall_depth_0;
    ADCL_function_create_async ( ADCL_alltoall_ladd_block4, NULL, ADCL_ialltoall_attrset,
                                 m_ialltoall_attrs, "Alltoall_ladd_block4",
                                 &ADCL_ialltoall_functions[count]);
    count++;

    m_ialltoall_attrs[0] = ADCL_attr_ialltoall_type_bc;
    m_ialltoall_attrs[1] = ADCL_attr_ialltoall_alg_ladd_block8;
    m_ialltoall_attrs[2] = ADCL_attr_ialltoall_depth_0;
    ADCL_function_create_async ( ADCL_alltoall_ladd_block8, NULL, ADCL_ialltoall_attrset,
                                 m_ialltoall_attrs, "Alltoall_ladd_block8",
                                 &ADCL_ialltoall_functions[count]);
    count++;
#endif

// Non-blocking pairwise
#if 1
    m_ialltoall_attrs[0] = ADCL_attr_ialltoall_type_nbc;
    m_ialltoall_attrs[1] = ADCL_attr_ialltoall_alg_pairwise;
    m_ialltoall_attrs[2] = ADCL_attr_ialltoall_depth_0;
    ADCL_function_create_async ( ADCL_ialltoall_pairwise, ADCL_ialltoall_wait, ADCL_ialltoall_attrset,
        			 m_ialltoall_attrs, "Ialltoall_pairwise_0",
        			 &ADCL_ialltoall_functions[count]);
    count++;

    m_ialltoall_attrs[0] = ADCL_attr_ialltoall_type_nbc;
    m_ialltoall_attrs[1] = ADCL_attr_ialltoall_alg_pairwise;
    m_ialltoall_attrs[2] = ADCL_attr_ialltoall_depth_1;
    ADCL_function_create_async ( ADCL_ialltoall_pairwise, ADCL_ialltoall_wait, ADCL_ialltoall_attrset,
        			 m_ialltoall_attrs, "Ialltoall_pairwise_1",
        			 &ADCL_ialltoall_functions[count]);
    count++;

    m_ialltoall_attrs[0] = ADCL_attr_ialltoall_type_nbc;
    m_ialltoall_attrs[1] = ADCL_attr_ialltoall_alg_pairwise;
    m_ialltoall_attrs[2] = ADCL_attr_ialltoall_depth_2;
    ADCL_function_create_async ( ADCL_ialltoall_pairwise, ADCL_ialltoall_wait, ADCL_ialltoall_attrset,
        			 m_ialltoall_attrs, "Ialltoall_pairwise_2",
        			 &ADCL_ialltoall_functions[count]);
    count++;

    m_ialltoall_attrs[0] = ADCL_attr_ialltoall_type_nbc;
    m_ialltoall_attrs[1] = ADCL_attr_ialltoall_alg_pairwise;
    m_ialltoall_attrs[2] = ADCL_attr_ialltoall_depth_3;
    ADCL_function_create_async ( ADCL_ialltoall_pairwise, ADCL_ialltoall_wait, ADCL_ialltoall_attrset,
        			 m_ialltoall_attrs, "Ialltoall_pairwise_3",
        			 &ADCL_ialltoall_functions[count]);
    count++;

#if MAX_PROGRESS_FOR_DEPTH == 5
    m_ialltoall_attrs[0] = ADCL_attr_ialltoall_type_nbc;
    m_ialltoall_attrs[1] = ADCL_attr_ialltoall_alg_pairwise;
    m_ialltoall_attrs[2] = ADCL_attr_ialltoall_depth_4;
    ADCL_function_create_async ( ADCL_ialltoall_pairwise, ADCL_ialltoall_wait, ADCL_ialltoall_attrset,
        			 m_ialltoall_attrs, "Ialltoall_pairwise_4",
        			 &ADCL_ialltoall_functions[count]);
    count++;

    m_ialltoall_attrs[0] = ADCL_attr_ialltoall_type_nbc;
    m_ialltoall_attrs[1] = ADCL_attr_ialltoall_alg_pairwise;
    m_ialltoall_attrs[2] = ADCL_attr_ialltoall_depth_5;
    ADCL_function_create_async ( ADCL_ialltoall_pairwise, ADCL_ialltoall_wait, ADCL_ialltoall_attrset,
        			 m_ialltoall_attrs, "Ialltoall_pairwise_5",
        			 &ADCL_ialltoall_functions[count]);
    count++;
#endif
#endif

// Blocking pairwise
#if 1
    m_ialltoall_attrs[0] = ADCL_attr_ialltoall_type_bc;
    m_ialltoall_attrs[1] = ADCL_attr_ialltoall_alg_pairwise;
    m_ialltoall_attrs[2] = ADCL_attr_ialltoall_depth_0;
    ADCL_function_create_async ( ADCL_alltoall_pairwise, NULL, ADCL_ialltoall_attrset,
                                 m_ialltoall_attrs, "Alltoall_pairwise_SR",
                                 &ADCL_ialltoall_functions[count]);
    count++;

    m_ialltoall_attrs[0] = ADCL_attr_ialltoall_type_bc;
    m_ialltoall_attrs[1] = ADCL_attr_ialltoall_alg_pairwise_excl;
    m_ialltoall_attrs[2] = ADCL_attr_ialltoall_depth_0;
    ADCL_function_create_async ( ADCL_alltoall_pairwise_excl, NULL, ADCL_ialltoall_attrset,
                                 m_ialltoall_attrs, "Alltoall_pairwise_excl_SR",
                                 &ADCL_ialltoall_functions[count]);
    count++;
#endif

// Non-blocking dissemination
#if 1
    m_ialltoall_attrs[0] = ADCL_attr_ialltoall_type_nbc;
    m_ialltoall_attrs[1] = ADCL_attr_ialltoall_alg_diss;
    m_ialltoall_attrs[2] = ADCL_attr_ialltoall_depth_0;
    ADCL_function_create_async ( ADCL_ialltoall_diss, ADCL_ialltoall_wait, ADCL_ialltoall_attrset,
        			 m_ialltoall_attrs, "Ialltoall_diss_0",
        			 &ADCL_ialltoall_functions[count]);
    count++;

    m_ialltoall_attrs[0] = ADCL_attr_ialltoall_type_nbc;
    m_ialltoall_attrs[1] = ADCL_attr_ialltoall_alg_diss;
    m_ialltoall_attrs[2] = ADCL_attr_ialltoall_depth_1;
    ADCL_function_create_async ( ADCL_ialltoall_diss, ADCL_ialltoall_wait, ADCL_ialltoall_attrset,
        			 m_ialltoall_attrs, "Ialltoall_diss_1",
        			 &ADCL_ialltoall_functions[count]);
    count++;

    m_ialltoall_attrs[0] = ADCL_attr_ialltoall_type_nbc;
    m_ialltoall_attrs[1] = ADCL_attr_ialltoall_alg_diss;
    m_ialltoall_attrs[2] = ADCL_attr_ialltoall_depth_2;
    ADCL_function_create_async ( ADCL_ialltoall_diss, ADCL_ialltoall_wait, ADCL_ialltoall_attrset,
        			 m_ialltoall_attrs, "Ialltoall_diss_2",
        			 &ADCL_ialltoall_functions[count]);
    count++;

    m_ialltoall_attrs[0] = ADCL_attr_ialltoall_type_nbc;
    m_ialltoall_attrs[1] = ADCL_attr_ialltoall_alg_diss;
    m_ialltoall_attrs[2] = ADCL_attr_ialltoall_depth_3;
    ADCL_function_create_async ( ADCL_ialltoall_diss, ADCL_ialltoall_wait, ADCL_ialltoall_attrset,
        			 m_ialltoall_attrs, "Ialltoall_diss_3",
        			 &ADCL_ialltoall_functions[count]);
    count++;

#if MAX_PROGRESS_FOR_DEPTH == 5
    m_ialltoall_attrs[0] = ADCL_attr_ialltoall_type_nbc;
    m_ialltoall_attrs[1] = ADCL_attr_ialltoall_alg_diss;
    m_ialltoall_attrs[2] = ADCL_attr_ialltoall_depth_4;
    ADCL_function_create_async ( ADCL_ialltoall_diss, ADCL_ialltoall_wait, ADCL_ialltoall_attrset,
        			 m_ialltoall_attrs, "Ialltoall_diss_4",
        			 &ADCL_ialltoall_functions[count]);
    count++;

    m_ialltoall_attrs[0] = ADCL_attr_ialltoall_type_nbc;
    m_ialltoall_attrs[1] = ADCL_attr_ialltoall_alg_diss;
    m_ialltoall_attrs[2] = ADCL_attr_ialltoall_depth_5;
    ADCL_function_create_async ( ADCL_ialltoall_diss, ADCL_ialltoall_wait, ADCL_ialltoall_attrset,
        			 m_ialltoall_attrs, "Ialltoall_diss_5",
        			 &ADCL_ialltoall_functions[count]);
    count++;
#endif
#endif


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
