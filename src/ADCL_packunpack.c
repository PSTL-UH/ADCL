#include "ADCL.h"


int ADCL_packunpack_init ( int num, int neighbors[], MPI_Comm comm, 
			   char ***sendbuf, MPI_Datatype *sdats, 
			   char ***recvbuf, MPI_Datatype *rdats ) 
{
    int i, ssize, rsize;
    char **sbuf=*sendbuf;
    char **rbuf=*recvbuf;

    rbuf = (char **) calloc (1, num * sizeof(char*)); 
    sbuf = (char **) calloc (1, num * sizeof(char*)); 
    if ( NULL == rbuf || NULL == sbuf ) {
	return ADCL_NO_MEMORY;
    }
    
    for ( i = 0; i < num; i++ ) {
	if ( MPI_PROC_NULL != neighbors[i] ) {
	    MPI_Pack_size ( 1, rdats[i], comm, &rsize );
	    MPI_Pack_size ( 1, sdats[i], comm, &ssize );
	    rbuf[i] = (char *) malloc ( rsize );
	    sbuf[i] = (char *) malloc ( ssize );
	    if ( NULL == rbuf[i] || NULL == sbuf[i] ) {
		return ADCL_NO_MEMORY;
	    }
	}
    }

    *sendbuf = sbuf;
    *recvbuf = rbuf;

    return ADCL_SUCCESS;
}

void ADCL_packunpack_free ( int num, char ***sendbuf, char ***recvbuf ) 
{
    int i;
    char **sbuf = *sendbuf;
    char **rbuf = *recvbuf;

    if ( NULL != rbuf ) {
	for ( i = 0; i < num; i++ ) {
	    if ( NULL != rbuf[i] ) {
		free ( rbuf[i] );
	    }
	}
	free ( rbuf );
    }

    if ( NULL != sbuf ) {
	for ( i = 0; i < num; i++ ) {
	    if ( NULL != sbuf[i] ) {
		free ( sbuf[i] );
	    }
	}
	free ( sbuf );
    }

    *sendbuf=NULL;
    *recvbuf=NULL;
    return;
}
