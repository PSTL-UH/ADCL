#include "ADCL_internal.h"
#include <stdarg.h>

int ADCL_printf ( const char* format, ... ) 
{
    va_list ap;

    va_start ( ap, format );
    vprintf( format, ap );
    va_end (ap);

    return ADCL_SUCCESS;
}
