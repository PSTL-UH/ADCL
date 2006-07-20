#include "ADCL.h"
#include "ADCL_internal.h"

int ADCL_Init (void )
{
    int ret;

    ret = ADCL_printf_init();
    ret = ADCL_method_init();

    ret = ADCL_read_env();
    ret = ADCL_emethod_req_init ();

    /* Initialize the request and vector fortran pointer arrays */
    ADCL_array_init ( &(ADCL_vector_farray), "ADCL_Vector",  32 );
    ADCL_array_init ( &(ADCL_request_farray), "ADCL_Vector",  32 );
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

    ADCL_printf_finalize ();
    return ret;
}
