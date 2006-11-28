#include <stdio.h>
#include <string.h>

#include "ADCL.h"
#include "mpi.h"

#define DIM0  8
#define HWIDTH 1

static void dump_vector ( double *data, int rank, int dim);

int main ( int argc, char ** argv ) 
{
    int i, rank, size;
    int dims=DIM0+2*HWIDTH;
    int cdims=0;
    int periods=0;
    double *data;
    
    ADCL_Vector vec;
    ADCL_Topology topo;
    ADCL_Request request;
    
    MPI_Comm cart_comm;

    MPI_Init ( &argc, &argv );
    MPI_Comm_rank ( MPI_COMM_WORLD, &rank );
    MPI_Comm_size ( MPI_COMM_WORLD, &size );

    ADCL_Init ();
    ADCL_Vector_allocate ( 1,  &dims, 0, 1, MPI_DOUBLE, &data, &vec );

    MPI_Dims_create ( size, 1, &cdims );
    MPI_Cart_create ( MPI_COMM_WORLD, 1, &cdims, &periods, 0, &cart_comm);

    ADCL_Topology_create ( cart_comm, &topo );
    ADCL_Request_create ( vec, topo, &request );

    for (i=0; i< HWIDTH; i++ ) {
	data[i] = -1;
    }
    for ( i=HWIDTH; i<(DIM0+HWIDTH); i++) {
	data[i] = rank;
    }
    for ( i=DIM0+HWIDTH; i<DIM0+2*HWIDTH; i++) {
	data[i] = -1;
    }
    
    dump_vector ( data, rank, dims);
    ADCL_Request_start( request );
    dump_vector ( data, rank, dims);

    ADCL_Request_free ( &request );
    ADCL_Vector_free ( &vec );
    ADCL_Topology_free ( &topo );

    MPI_Comm_free ( &cart_comm );
    
    ADCL_Finalize ();
    MPI_Finalize ();
    return 0;
}


static void dump_vector ( double *data, int rank, int dim)
{
    int i;
    
    printf("%d : ", rank);
    for (i=0; i<dim; i++) {
	printf("%lf ", data[i]);
    }
    printf ("\n");

    return;
}
