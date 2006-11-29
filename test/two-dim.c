#include <stdio.h>

#include "ADCL.h"
#include "mpi.h"

/* Dimensions of the data matrix per process */
#define DIM0  8
#define DIM1  16

static void dump_vector_2D ( double *data, int rank, int dim);
static void dump_vector_3D ( double **data, int rank, int dim, int nc);
static void set_data_2D ( double *data, int rank, int dim, int hwidth );
static void set_data_3D ( double **data, int rank, int dim, int hwidth, int nc);
static void check_data_2D ( double *data, int rank, int size, int dim, int hwidth ); 
static void check_data_3D ( double **data, int rank, int size, int dim, int hwidth, int nc ); 


int main ( int argc, char ** argv ) 
{
    /* General variables */
    int i, rank, size;
    
    /* Definition of the 2-D vector */
    int dims[2];
    double **data, ***data2;
    ADCL_Vector vec;

    /* Variables for the process topology information */
    int cdims[]={0,0};
    int periods[]={0,0};
    MPI_Comm cart_comm;
    ADCL_Topology topo;
    ADCL_Request request;

    /* Initiate the MPI environment */
    MPI_Init ( &argc, &argv );
    MPI_Comm_rank ( MPI_COMM_WORLD, &rank );
    MPI_Comm_size ( MPI_COMM_WORLD, &size );

    /* Describe the neighborhood relations */
    MPI_Dims_create ( size, 2, cdims );
    MPI_Cart_create ( MPI_COMM_WORLD, 2, cdims, periods, 0, &cart_comm);

    /* Initiate the ADCL library and register a topology object with ADCL */
    ADCL_Init ();
    ADCL_Topology_create ( cart_comm, &topo );

    /**********************************************************************/
    /* Test 1: hwidth=1, nc=0 */
    hwidth=1;
    dims[0] = DIM0 + 2*hwidth;
    dims[1] = DIM1 + 2*hwidth;
    ADCL_Vector_allocate ( 2,  &dims, 0, hwidth, MPI_DOUBLE, &data, &vec );
    ADCL_Request_create ( vec, topo, &request );
    
    set_data_2D ( data, rank, dims, hwidth );
#ifdef VERBOSE
    dump_vector_2D ( data, rank, dims );
#endif

    ADCL_Request_start ( request );
    check_data_2D ( data, rank, dims, hwidth );

    ADCL_Request_free ( &request );
    ADCL_Vector_free ( &vec );

    /**********************************************************************/
    /* Test 2: hwidth=2, nc=0 */
    hwidth=1;
    dims[0] = DIM0 + 2*hwidth;
    dims[1] = DIM1 + 2*hwidth;
    ADCL_Vector_allocate ( 2,  &dims, 0, hwidth, MPI_DOUBLE, &data, &vec );
    ADCL_Request_create ( vec, topo, &request );
    
    set_data_2D ( data, rank, dims, hwidth );
#ifdef VERBOSE
    dump_vector_2D ( data, rank, dims );
#endif

    ADCL_Request_start ( request );
    check_data_2D ( data, rank, dims, hwidth );

    ADCL_Request_free ( &request );
    ADCL_Vector_free ( &vec );

    /**********************************************************************/
    /* Test 3: hwidth=1, nc=1 */
    hwidth=1;
    dims[0] = DIM0 + 2*hwidth;
    dims[1] = DIM1 + 2*hwidth;
    ADCL_Vector_allocate ( 2,  &dims, 1, hwidth, MPI_DOUBLE, &data, &vec );
    ADCL_Request_create ( vec, topo, &request );
    
    set_data_3D ( data, rank, dims, hwidth, 1 );
#ifdef VERBOSE
    dump_vector_3D ( data, rank, dims, 1 );
#endif

    ADCL_Request_start ( request );
    check_data_3D ( data, rank, dims, hwidth, 1);

    ADCL_Request_free ( &request );
    ADCL_Vector_free ( &vec );

    /**********************************************************************/
    /* Test 4: hwidth=2, nc=1 */
    hwidth=2;
    dims[0] = DIM0 + 2*hwidth;
    dims[1] = DIM1 + 2*hwidth;
    ADCL_Vector_allocate ( 2,  &dims, 1, hwidth, MPI_DOUBLE, &data, &vec );
    ADCL_Request_create ( vec, topo, &request );
    
    set_data_3D ( data, rank, dims, hwidth, 1 );
#ifdef VERBOSE
    dump_vector_3D ( data, rank, dims, 1 );
#endif

    ADCL_Request_start ( request );
    check_data_3D ( data, rank, dims, hwidth, 1 );

    ADCL_Request_free ( &request );
    ADCL_Vector_free ( &vec );

    /**********************************************************************/
    /* Test 3: hwidth=2, nc=2 */
    hwidth=2;
    dims[0] = DIM0 + 2*hwidth;
    dims[1] = DIM1 + 2*hwidth;
    ADCL_Vector_allocate ( 2,  &dims, 2, hwidth, MPI_DOUBLE, &data, &vec );
    ADCL_Request_create ( vec, topo, &request );
    
    set_data_3D ( data, rank, dims, hwidth, 2 );
#ifdef VERBOSE
    dump_vector_3D ( data, rank, dims, 2 );
#endif

    ADCL_Request_start ( request );
    check_data_3D ( data, rank, dims, hwidth, 2 );

    ADCL_Request_free ( &request );
    ADCL_Vector_free ( &vec );

    /**********************************************************************/
    ADCL_Topology_free ( &topo );
    MPI_Comm_free ( &cart_comm );
    
    ADCL_Finalize ();
    MPI_Finalize ();
    return 0;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
static void check_data_1D ( double *data, int rank, int size, int dim, int hwidth ) 
{
    int i, lres=1, gres;

    if ( rank == 0 ) {
	for (i=0; i< hwidth; i++ ) {
	    if ( data[i] != -1 ) {
		lres = 0;
	    }
	}
    }
    else {
	for (i=0; i< hwidth; i++ ) {
	    if ( data[i] != rank-1 ) {
		lres = 0;
	    }
	}
    }

    for ( i=hwidth; i<(DIM0+hwidth); i++) {
	if ( data[i] != rank) {
	    lres = 0;
	}
    }

    if ( rank == size -1 ) {
	for ( i=DIM0+hwidth; i<DIM0+2*hwidth; i++) {
	    if ( data[i] != -1 ) {
		lres = 0;
	    }
	}
    }
    else {
	for ( i=DIM0+hwidth; i<DIM0+2*hwidth; i++) {
	    if ( data[i] != rank+1 ) {
		lres = 0;
	    }
	}
    }

    MPI_Allreduce ( &lres, &gres, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD );
    if ( gres == 1 ) {
	if ( rank == 0 ) {
	    printf("1-D testsuite: hwidth=%d, nc=0 result is correct\n", hwidth);
	}
    }
    else {
	if ( rank == 0 ) {
	    printf("1-D testsuite: hwidth=%d, nc=0 result is wrong\n", hwidth);
	}
	dump_vector_1D ( data, rank, dim);
    }

    return;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
static void check_data_2D ( double **data, int rank, int size, int dim, int hwidth, int nc )
{
    int i, j, lres=1, gres;

    if ( rank == 0 )  {
	for (i=0; i< hwidth; i++ ) {
	    for (j=0; j<nc; j++ ){
		if ( data[i][j] != -1) {
		    lres = 0;
		}
	    }
	}
    }
    else {
	for (i=0; i< hwidth; i++ ) {
	    for (j=0; j<nc; j++ ){
		if ( data[i][j] != rank-1) {
		    lres = 0;
		}
	    }
	}
    }

    for ( i=hwidth; i<(DIM0+hwidth); i++) {
	for (j=0; j<nc; j++ ){
	    if ( data[i][j] != rank) {
		lres = 0;
	    }
	}
    }

    
    if ( rank == size -1 ) {
	for ( i=DIM0+hwidth; i<DIM0+2*hwidth; i++) {
	    for (j=0; j<nc; j++ ){
		if ( data[i][j] != -1 ) {
		    lres = 0;
		}
	    }
	}
    }
    else {
	for ( i=DIM0+hwidth; i<DIM0+2*hwidth; i++) {
	    for (j=0; j<nc; j++ ){
		if ( data[i][j] != rank+1 ) {
		    lres = 0;
		}
	    }
	}
    }

    MPI_Allreduce ( &lres, &gres, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD );
    if ( gres == 1 ) {
	if ( rank == 0 ) {
	    printf("1-D testsuite: hwidth=%d, nc=%d result is correct\n", 
		   hwidth, nc);
	}
    }
    else {
	if ( rank == 0 ) {
	    printf("1-D testsuite: hwidth=%d, nc=%d result is wrong\n",
		   hwidth, nc);
	}
	dump_vector_2D ( data, rank, dim, 1);
    }


    return;
}
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
static void set_data_1D ( double *data, int rank, int dim, int hwidth ) 
{
    int i;

    for (i=0; i< hwidth; i++ ) {
	data[i] = -1;
    }
    for ( i=hwidth; i<(DIM0+hwidth); i++) {
	data[i] = rank;
    }
    for ( i=DIM0+hwidth; i<DIM0+2*hwidth; i++) {
	data[i] = -1;
    }

    return;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
static void set_data_2D ( double **data, int rank, int dim, int hwidth, int nc ) 
{
    int i, j;

    for (i=0; i< hwidth; i++ ) {
	for (j=0; j<nc; j++ ){
	    data[i][j] = -1;
	}
    }
    for ( i=hwidth; i<(DIM0+hwidth); i++) {
	for (j=0; j<nc; j++ ){
	    data[i][j] = rank;
	}
    }
    for ( i=DIM0+hwidth; i<DIM0+2*hwidth; i++) {
	for (j=0; j<nc; j++ ){
	    data[i][j] = -1;
	}
    }

    return;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
static void dump_vector_2D ( double *data, int rank, int *dim)
{
    int i, j;
    
    for (i=0; i<dim[0]; i++) {
	printf("%d : ", rank);
	for ( j=0; j<dim[1]; j++ ) {
	    printf("%lf ", data[i][j]);
	}
	printf ("\n");
    }

    return;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
static void dump_vector_3D ( double **data, int rank, int dim, int nc)
{
    int i, j;
    
    printf("%d : ", rank);
    for (i=0; i<dim; i++) {
	for (j=0; j<nc; j++) {
	    printf("%lf ", data[i][j]);
	}
    }
    printf ("\n");

    return;
}
