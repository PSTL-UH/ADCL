#include "ADCL_internal.h"
#include "ADCL_papi.h"

int ADCL_Papi_create ( ADCL_Papi *papi )
{
    return ADCL_papi_create (papi);
}

int ADCL_Papi_enter ( ADCL_Papi papi ) 
{
    return ADCL_papi_enter (papi);
}

int ADCL_Papi_leave ( ADCL_Papi papi ) 
{
    return ADCL_papi_leave (papi);
}

int ADCL_Papi_free ( ADCL_Papi *papi)
{
    return ADCL_papi_free (papi);
}

int ADCL_Papi_print ( ADCL_Papi papi)
{
    return ADCL_papi_print (papi);
}
