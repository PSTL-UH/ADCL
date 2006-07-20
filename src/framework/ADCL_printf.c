#include "ADCL_internal.h"
#include <stdarg.h>


static FILE *fd=NULL;
int ADCL_printf_silence=0;

int ADCL_printf_init ( void )
{
#ifdef ADCL_FILE_PER_PROC
    int rank;
    char filename[32];

    MPI_Comm_rank ( MPI_COMM_WORLD, &rank );
    sprintf(filename, "%d.out", rank);
    fd = fopen (filename, "w");
    if ( NULL == fd ) {
	MPI_Abort ( MPI_COMM_WORLD, 1 );
    }
#endif
    return ADCL_SUCCESS;
}
int ADCL_printf_finalize ( void )
{
#ifdef ADCL_FILE_PER_PROC
    fclose ( fd );
#endif
    return ADCL_SUCCESS;
}


int ADCL_printf ( const char* format, ... ) 
{
    va_list ap;

    if ( !ADCL_printf_silence ) {
	va_start ( ap, format );
#ifdef ADCL_FILE_PER_PROC 
	vfprintf(fd, format, ap );
#else
	vprintf( format, ap );
#endif
	va_end (ap);
    }

    return ADCL_SUCCESS;
}
