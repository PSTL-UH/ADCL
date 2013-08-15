/*
 * Copyright (c) 2006-2013      University of Houston. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include <math.h>
#include "ADCL_internal.h"
#include "ADCL_config.h"

ADCL_attribute_t *ADCL_allreduce_attrs[ADCL_ATTR_ALLREDUCE_TOTAL_NUM];
ADCL_attrset_t *ADCL_allreduce_attrset;
ADCL_function_t *ADCL_allreduce_functions[ADCL_METHOD_ALLREDUCE_TOTAL_NUM];
ADCL_fnctset_t *ADCL_allreduce_fnctset;

int ADCL_predefined_allreduce ( void )
{
    int m_allreduce_attr[1];
    int count = 0;

    ADCL_function_create_async ( ADCL_allreduce_native, NULL,
                                 ADCL_ATTRSET_NULL,
                                 m_allreduce_attr, "Allreduce_native",
                                 & ADCL_allreduce_functions[count]);
    count++;

    ADCL_function_create_async ( ADCL_allreduce_linear, NULL,
                                 ADCL_ATTRSET_NULL,
                                 m_allreduce_attr, "Allreduce_linear",
                                 & ADCL_allreduce_functions[count]);
    count++;

    ADCL_function_create_async ( ADCL_allreduce_nonoverlapping, NULL,
                                 ADCL_ATTRSET_NULL,
                                 m_allreduce_attr, "Allreduce_nonoverlapping",
                                 & ADCL_allreduce_functions[count]);
    count++;

    ADCL_function_create_async ( ADCL_allreduce_recursivedoubling, NULL,
                                 ADCL_ATTRSET_NULL,
                                 m_allreduce_attr, "Allreduce_recursive_doubling",
                                 & ADCL_allreduce_functions[count]);
    count++;

    ADCL_function_create_async ( ADCL_allreduce_ring, NULL,
                                 ADCL_ATTRSET_NULL,
                                 m_allreduce_attr, "Allreduce_ring",
                                 & ADCL_allreduce_functions[count]);
    count++;

    ADCL_fnctset_create ( ADCL_METHOD_ALLREDUCE_TOTAL_NUM,
                          ADCL_allreduce_functions,
                          "AllReduce",
                          &ADCL_allreduce_fnctset );


    if ( count != ADCL_METHOD_ALLREDUCE_TOTAL_NUM) {
        ADCL_printf("Total Number wrong\n");
        return ADCL_ERROR_INTERNAL;
    }


    return ADCL_SUCCESS;
}
