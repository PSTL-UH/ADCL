#include "ADCL.h"
#include "ADCL_internal.h"

int ADCL_Init (void )
{
    int ret;

    ret = ADCL_printf_init();
    ret = ADCL_method_init();

    ret = ADCL_readenv();
    ret = ADCL_emethod_req_init ();

    /* Initialize the request, topology and vector fortran pointer arrays */
    ADCL_array_init ( &(ADCL_vector_farray),    "ADCL_Vector",   32 );
    ADCL_array_init ( &(ADCL_request_farray),   "ADCL_Request",  32 );
    ADCL_array_init ( &(ADCL_topology_farray),  "ADCL_Topology", 32 );
    ADCL_array_init ( &(ADCL_attribute_farray), "ADCL_Attribute",32 );
    ADCL_array_init ( &(ADCL_attrset_farray),   "ADCL_Attrset",  32 );
    return ret;
}

int ADCL_Finalize ( void ) 
{
    int ret;
    

    ret = ADCL_method_finalize();
    ret = ADCL_emethod_req_finalize();

    /* Initialize the request and vector fortran pointer arrays */
    ADCL_array_free ( &(ADCL_vector_farray));
    ADCL_array_free ( &(ADCL_request_farray));
    ADCL_array_free ( &(ADCL_topology_farray));
    ADCL_array_free ( &(ADCL_attribute_farray));
    ADCL_array_free ( &(ADCL_attrset_farray));

    ADCL_printf_finalize ();
    return ret;
}
