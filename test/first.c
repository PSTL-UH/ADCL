#include <stdio.h>

#include "ADCL.h"
#include "mpi.h"

#define NIT 100

int main ( int argc, char ** argv ) 
{
    int i, rank, size;
    int dims[3]={66,34,34};
    int cdims[]={0,0,0};
    int periods[]={0,0,0};
    ADCL_Vector vec;
    ADCL_Request request;
    
    MPI_Comm cart_comm;

    MPI_Init ( &argc, &argv );
    MPI_Comm_rank ( MPI_COMM_WORLD, &rank );
    MPI_Comm_size ( MPI_COMM_WORLD, &size );

    ADCL_Init ();
    ADCL_Vector_allocate ( 3,  dims, 1, 1, MPI_DOUBLE, &vec );

    MPI_Dims_create ( size, 3, cdims );
    MPI_Cart_create ( MPI_COMM_WORLD, 3, cdims, periods, 0, &cart_comm);

    ADCL_Request_create ( vec, cart_comm, &request );

    for ( i=0; i<NIT; i++ ) {
	ADCL_Request_start( request );
    }

    ADCL_Request_free ( &request );
    ADCL_Vector_free ( &vec );
    MPI_Comm_free ( &cart_comm );
    
    ADCL_Finalize ();
    MPI_Finalize ();
    return 0;
}
