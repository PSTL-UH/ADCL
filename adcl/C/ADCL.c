#include "ADCL.h"
#include "ADCL_internal.h"

int ADCL_Init (void )
{
    int ret;

    ret = ADCL_method_init();
    return ret;
}

int ADCL_Finalize ( void ) 
{
    int ret;
    
    ret = ADCL_method_finalize();
    return ret;
}
