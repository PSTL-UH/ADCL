/*
 * Copyright (c) 2006-2013      University of Houston. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include "ADCL_internal.h"
#include "ADCL_config.h"

ADCL_attribute_t *ADCL_allgatherv_attrs[ADCL_ATTR_ALLGATHERV_TOTAL_NUM];
ADCL_attrset_t *ADCL_allgatherv_attrset;
ADCL_function_t *ADCL_allgatherv_functions[ADCL_METHOD_ALLGATHERV_TOTAL_NUM];
ADCL_fnctset_t *ADCL_allgatherv_fnctset;



int ADCL_predefined_allgatherv ( void )
{
    int m_allgatherv_attr[1];
    int count = 0;

/*  ADCL_attribute_create ( ADCL_ATTR_TRANSFER_MAX, ADCL_attr_transfer_allgatherv,
                            ADCL_attr_transfer_allgatherv_names , "transfer primitive",
                            &ADCL_allgatherv_attrs[1]);

    ADCL_attrset_create ( ADCL_ATTR_ALLGATHERV_TOTAL_NUM, ADCL_allgatherv_attrs,
                          &ADCL_allgatherv_attrset); */


    /* Register function aao, ddt, IsendIrecv */
    //m_allgatherv_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_allgatherv_linear;

    ADCL_function_create_async ( ADCL_allgatherv_native, NULL,
                                 ADCL_ATTRSET_NULL,
                                 m_allgatherv_attr, "Allgatherv_native",
                                 & ADCL_allgatherv_functions[count]);
    count++;

    ADCL_function_create_async ( ADCL_allgatherv_recursivedoubling, NULL,
                                 ADCL_ATTRSET_NULL,
                                 m_allgatherv_attr, "Allgatherv_recursive_doubling",
                                 & ADCL_allgatherv_functions[count]);
    count++;

    ADCL_function_create_async ( ADCL_allgatherv_linear, NULL,
                                 ADCL_ATTRSET_NULL,
                                 m_allgatherv_attr, "Allgatherv_linear",
                                 & ADCL_allgatherv_functions[count]);
    count++;

    ADCL_function_create_async ( ADCL_allgatherv_bruck, NULL,
                                 ADCL_ATTRSET_NULL,
                                 m_allgatherv_attr, "Allgatherv_bruck",
                                 & ADCL_allgatherv_functions[count]);
    count++;

    ADCL_function_create_async ( ADCL_allgatherv_neighborexchange, NULL,
                                 ADCL_ATTRSET_NULL,
                                 m_allgatherv_attr, "Allgatherv_neighbor_exchange",
                                 & ADCL_allgatherv_functions[count]);
    count++;

    ADCL_function_create_async ( ADCL_allgatherv_ring, NULL,
                                 ADCL_ATTRSET_NULL,
                                 m_allgatherv_attr, "Allgatherv_ring",
                                 & ADCL_allgatherv_functions[count]);
    count++;


    /* this algo only works for topo->t_size == 2
     * ADCL_function_create_async ( ADCL_allgatherv_two_procs, NULL,
     *                              ADCL_ATTRSET_NULL,
     *                              m_allgatherv_attr, "Allgatherv_two_procs",
     *                              & ADCL_allgatherv_functions[count]);
     * count++; */

//    m_nn_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_nn_PostStartPut;
//    /* m_nn_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
//    ADCL_function_create_async ( ADCL_change_sb_aao_post_start_put, NULL,
//                                 ADCL_neighborhood_attrset,
//                                 m_nn_attr, "PostStartPut_aao",
//                                 &ADCL_neighborhood_functions[count]);

    ADCL_fnctset_create ( ADCL_METHOD_ALLGATHERV_TOTAL_NUM,
                          ADCL_allgatherv_functions,
                          "AllGatherV",
                          &ADCL_allgatherv_fnctset );

    if ( count != ADCL_METHOD_ALLGATHERV_TOTAL_NUM) {
        ADCL_printf("Total Number wrong\n");
        return ADCL_ERROR_INTERNAL;
    }

    return ADCL_SUCCESS;
}
