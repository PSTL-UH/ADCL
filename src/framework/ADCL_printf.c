/*
 * Copyright (c) 2006-2007      University of Houston. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include "ADCL_internal.h"
#include <stdarg.h>

#define MAXLINE 2048

#ifdef ADCL_FILE_PER_PROC
static FILE *fd=NULL;
static char buffer[MAXLINE][128];
static int bufcnt=0;
#endif
int ADCL_printf_silence=0;



int ADCL_printf_init ( void )
{
#ifdef ADCL_FILE_PER_PROC
    int rank;
    char filename[32];

    rank = 0;
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
    int i;

    if ( bufcnt > 0 ) {
    for (i=0; i< bufcnt; i++ ) {
        fprintf(fd, "%s", buffer[i] );
    }
    }

    fclose ( fd );
#endif
    return ADCL_SUCCESS;
}


int ADCL_printf ( const char* format, ... )
{
    va_list ap;

    if ( ADCL_printf_silence == 0 ) {
    va_start ( ap, format );
#ifdef ADCL_FILE_PER_PROC
/*  vfprintf(fd, format, ap ); */

    vsprintf(buffer[bufcnt], format, ap);
    bufcnt++;
    if ( bufcnt == MAXLINE ) {
        /*
         * dump everything to the file and reset
         * the counter
        */
        int i;

        for (i=0; i< MAXLINE; i++ ) {
        fprintf(fd, "%s", buffer[i] );
        }
        bufcnt = 0;
    }
#else
    vprintf( format, ap );
#endif
    va_end (ap);
    }

    return ADCL_SUCCESS;
}

int ADCL_printf_winner ( const char* format,int rank, const char* objname, int id, int fnctset_id, const char* fnctset_name, int function_id, const char* function_name )
{
  int ret;

    if ( ADCL_printf_silence == 0 ) {
      ret = ADCL_printf( format, rank, objname, id, fnctset_id, fnctset_name, function_id, function_name );
      return ret;
    // Only process with rank 0 writes the winner function set/function
    }else if(ADCL_printf_silence == 2 && rank ==0){
      printf( format, rank, objname, id, fnctset_id, fnctset_name, function_id, function_name );
    }
 
    return ADCL_SUCCESS;
}
