/*
 * Copyright (c) 2006-2013      University of Houston. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include "ADCL.h"
#include "ADCL_internal.h"

extern ADCL_array_t *ADCL_emethod_array;

int ADCL_Init (void )
{
    int ret;

    ret = ADCL_printf_init();

    /* Initialize the request, topology and vector fortran pointer arrays */
    ADCL_array_init ( &(ADCL_vector_farray),    "ADCL_Vector",    ADCL_MAX_ARRAYSIZE );
    ADCL_array_init ( &(ADCL_emethod_array),    "ADCL_emethod",   ADCL_MAX_ARRAYSIZE );
    ADCL_array_init ( &(ADCL_hist_array),       "ADCL_hist",      ADCL_MAX_ARRAYSIZE );
    ADCL_array_init ( &(ADCL_request_farray),   "ADCL_Request",   ADCL_MAX_ARRAYSIZE );
    ADCL_array_init ( &(ADCL_topology_farray),  "ADCL_Topology",  ADCL_MAX_ARRAYSIZE );
    ADCL_array_init ( &(ADCL_attribute_farray), "ADCL_Attribute", ADCL_MAX_ARRAYSIZE );
    ADCL_array_init ( &(ADCL_attrset_farray),   "ADCL_Attrset",   ADCL_MAX_ARRAYSIZE );
    ADCL_array_init ( &(ADCL_function_farray),  "ADCL_Function",  ADCL_MAX_ARRAYSIZE );
    ADCL_array_init ( &(ADCL_fnctset_farray),   "ADCL_Fnctgrp",   ADCL_MAX_ARRAYSIZE );
    ADCL_array_init ( &(ADCL_vmap_farray),      "ADCL_vmap",      ADCL_MAX_ARRAYSIZE );
    ADCL_array_init ( &(ADCL_timer_farray),     "ADCL_timer",     ADCL_MAX_ARRAYSIZE );

    ret = ADCL_predefined_init ();
    ret = ADCL_readenv();
#ifdef ADCL_DISPLAY
    ret = ADCL_display_init();
#endif
    return ret;
}

int ADCL_Finalize ( void )
{
    int ret;
#ifdef ADCL_DISPLAY
    ADCL_display_finalize();
#endif
    /* free the stored hist objects */
    ADCL_hist_free ( );
    /* free the predefined function set  */
    ADCL_predefined_finalize ();
    /* free the request and vector fortran pointer arrays */
    ret = ADCL_array_free ( &(ADCL_vector_farray));
    ret = ADCL_array_free ( &(ADCL_emethod_array));
    ret = ADCL_array_free ( &(ADCL_hist_array));
    ret = ADCL_array_free ( &(ADCL_request_farray));
    ret = ADCL_array_free ( &(ADCL_topology_farray));
    ret = ADCL_array_free ( &(ADCL_attribute_farray));
    ret = ADCL_array_free ( &(ADCL_attrset_farray));
    ret = ADCL_array_free ( &(ADCL_function_farray));
    ret = ADCL_array_free ( &(ADCL_fnctset_farray));
    ret = ADCL_array_free ( &(ADCL_vmap_farray));
    ret = ADCL_array_free ( &(ADCL_timer_farray));

    ADCL_printf_finalize ();
    return ret;
}
