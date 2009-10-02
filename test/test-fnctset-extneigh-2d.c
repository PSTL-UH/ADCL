/*
 * Copyright (c) 2006-2007      University of Houston. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include <stdio.h>

#include "ADCL.h"
#include "mpi.h"

//#define VERBOSE2D

/* Dimensions of the data matrix per process */
#define DIM0  3
#define DIM1  4

extern void dump_vector_2D_mpi ( double **data, int *dim, MPI_Comm cart_comm );
extern void dump_vector_3D_mpi ( double ***data, int *dim, int nc, MPI_Comm cart_comm );
static void set_data_2D ( double **data, int rank, int *dim, int hwidth, MPI_Comm cart_comm );
static void set_data_3D ( double ***data, int rank, int *dim, int hwidth, int nc, MPI_Comm cart_comm );
static void check_data_2D ( double **data, int rank, int *dim, 
			    int hwidth, int *neighbors, MPI_Comm cart_comm ); 
static void check_data_3D ( double ***data, int rank, int *dim, 
			    int hwidth, int nc, int *neighbors, MPI_Comm cart_comm ); 

static int calc_entry ( int i, int j, int cond, int *dim, int *dims, int *n_coords, int hwidth );
static void check_entry2D ( double **data, int i, int j, double should_be, char* error, int* lres ); 
static void check_entry3D ( double ***data, int i, int j, int k, double should_be, char* error, int* lres ); 


int main ( int argc, char ** argv ) 
{
    /* General variables */
    int hwidth, rank, size, err;
    
    /* Definition of the 2-D vector */
    int dims[2], neighbors[4];
    double **data, ***data2;
    ADCL_Vector vec;

    /* Variables for the process topology information */
    int cdims[]={0,0};
    int periods[]={0,0};
    ADCL_Vmap vmap;
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
    MPI_Cart_shift ( cart_comm, 0, 1, &(neighbors[0]), &(neighbors[1]));
    MPI_Cart_shift ( cart_comm, 1, 1, &(neighbors[2]), &(neighbors[3]));


    /* Initiate the ADCL library and register a topology object with ADCL */
    ADCL_Init ();
    ADCL_Topology_create_extended ( cart_comm, &topo );

    /**********************************************************************/
    /* Test 1: hwidth=1, nc=0 */
    hwidth=1;
    dims[0] = DIM0 + 2*hwidth;
    dims[1] = DIM1 + 2*hwidth;
    err = ADCL_Vmap_halo_allocate ( hwidth, &vmap );
    if ( ADCL_SUCCESS != err) goto exit;
    err = ADCL_Vector_allocate_generic ( 2,  dims, 0, vmap, MPI_DOUBLE, &data, &vec );
    if ( ADCL_SUCCESS != err) goto exit;
    ADCL_Request_create ( vec, topo, ADCL_FNCTSET_NEIGHBORHOOD, &request );
    
    set_data_2D ( data, rank, dims, hwidth, cart_comm );
#ifdef VERBOSE2D
    dump_vector_2D_mpi ( data, rank, dims, cart_comm );
#endif

    ADCL_Request_start ( request );
    check_data_2D ( data, rank, dims, hwidth, neighbors, cart_comm );

    ADCL_Request_free ( &request );
    ADCL_Vector_free  ( &vec );
    ADCL_Vmap_free    ( &vmap );

    /**********************************************************************/
    /* Test 2: hwidth=2, nc=0 */
    hwidth=2;
    dims[0] = DIM0 + 2*hwidth;
    dims[1] = DIM1 + 2*hwidth;
    err = ADCL_Vmap_halo_allocate ( hwidth, &vmap );
    if ( ADCL_SUCCESS != err) goto exit;
    err = ADCL_Vector_allocate_generic ( 2,  dims, 0, vmap, MPI_DOUBLE, &data, &vec );
    if ( ADCL_SUCCESS != err) goto exit;
    ADCL_Request_create ( vec, topo, ADCL_FNCTSET_NEIGHBORHOOD, &request );
    
    set_data_2D ( data, rank, dims, hwidth, cart_comm );
#ifdef VERBOSE2D2
    dump_vector_2D_mpi ( data, rank, dims, cart_comm );
#endif

    ADCL_Request_start ( request );
    check_data_2D ( data, rank, dims, hwidth, neighbors, cart_comm );

    ADCL_Request_free ( &request );
    ADCL_Vector_free ( &vec );
    ADCL_Vmap_free    ( &vmap );

    /**********************************************************************/
    /* Test 3: hwidth=1, nc=1 */
    hwidth=1;
    dims[0] = DIM0 + 2*hwidth;
    dims[1] = DIM1 + 2*hwidth;
    err = ADCL_Vmap_halo_allocate ( hwidth, &vmap );
    if ( ADCL_SUCCESS != err) goto exit;
    err = ADCL_Vector_allocate_generic ( 2,  dims, 1, vmap, MPI_DOUBLE, &data2, &vec );
    if ( ADCL_SUCCESS != err) goto exit;
    ADCL_Request_create ( vec, topo, ADCL_FNCTSET_NEIGHBORHOOD, &request );
    
    set_data_3D ( data2, rank, dims, hwidth, 1, cart_comm );
#ifdef VERBOSE3D
    dump_vector_3D_mpi ( data2, rank, dims, 1, cart_comm );
#endif

    ADCL_Request_start ( request );
    check_data_3D ( data2, rank, dims, hwidth, 1, neighbors, cart_comm );

    ADCL_Request_free ( &request );
    ADCL_Vector_free ( &vec );
    ADCL_Vmap_free    ( &vmap );

    /**********************************************************************/
    /* Test 4: hwidth=2, nc=1 */
    hwidth=2;
    dims[0] = DIM0 + 2*hwidth;
    dims[1] = DIM1 + 2*hwidth;
    err = ADCL_Vmap_halo_allocate ( hwidth, &vmap );
    if ( ADCL_SUCCESS != err) goto exit;
    err = ADCL_Vector_allocate_generic ( 2,  dims, 1, vmap, MPI_DOUBLE, &data2, &vec );
    if ( ADCL_SUCCESS != err) goto exit;
    ADCL_Request_create ( vec, topo, ADCL_FNCTSET_NEIGHBORHOOD, &request );
    
    set_data_3D ( data2, rank, dims, hwidth, 1, cart_comm );
#ifdef VERBOSE3D
    dump_vector_3D_mpi ( data2, rank, dims, 1, cart_comm );
#endif

    ADCL_Request_start ( request );
    check_data_3D ( data2, rank, dims, hwidth, 1, neighbors, cart_comm );

    ADCL_Request_free ( &request );
    ADCL_Vector_free ( &vec );
    ADCL_Vmap_free    ( &vmap );

    /**********************************************************************/
    /* Test 5: hwidth=2, nc=2 */
    hwidth=2;
    dims[0] = DIM0 + 2*hwidth;
    dims[1] = DIM1 + 2*hwidth;
    err = ADCL_Vmap_halo_allocate ( hwidth, &vmap );
    if ( ADCL_SUCCESS != err) goto exit;
    err = ADCL_Vector_allocate_generic ( 2,  dims, 2, vmap, MPI_DOUBLE, &data2, &vec );
    if ( ADCL_SUCCESS != err) goto exit;
    ADCL_Request_create ( vec, topo, ADCL_FNCTSET_NEIGHBORHOOD, &request );
    
    set_data_3D ( data2, rank,  dims, hwidth, 2, cart_comm );
#ifdef VERBOSE
    dump_vector_3D_mpi ( data2, rank, dims, 2, cart_comm );
#endif

    ADCL_Request_start ( request );
    check_data_3D ( data2, rank, dims, hwidth, 2, neighbors, cart_comm );

    ADCL_Request_free ( &request );
    ADCL_Vector_free ( &vec );
    ADCL_Vmap_free    ( &vmap );

    /**********************************************************************/
exit: 
    ADCL_Topology_free ( &topo );
    MPI_Comm_free ( &cart_comm );
    
    ADCL_Finalize ();
    MPI_Finalize ();
    return 0;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
static void check_data_3D ( double ***data, int rank, int *dim, 
			    int hwidth, int nc, int *neighbors, MPI_Comm cart_comm ) 
{
    int i, j, k, lres=1, gres;
    double should_be;
    int coords[2], n_coords[2], c_coords[2];
    int dims[2]; //size of each dimension
    int period[2];
			
    MPI_Cart_get(cart_comm, 2, dims, period, coords); 

    //Up HALO Cell
    if(neighbors[0] != MPI_PROC_NULL){
        MPI_Cart_coords (cart_comm, neighbors[0], 2, n_coords);
    }
    for ( j = hwidth; j < dim[1] - hwidth; j++ ) {
        for ( i= dim[0] - hwidth*2; i < dim[0]-hwidth ; i++ ) {
            for ( k=0; k<nc; k++ ) {
                should_be = calc_entry (i, j, neighbors[0] != MPI_PROC_NULL, dim, dims, n_coords, hwidth ); 
                check_entry3D ( data, i-(dim[0]-hwidth*2), j, k, should_be, "out1", &lres );
            }
        }
    }

    //Down HALO Cell 
    if(neighbors[1] != MPI_PROC_NULL){
        MPI_Cart_coords (cart_comm, neighbors[1], 2, n_coords);
    }
    for ( j = hwidth; j < dim[1] - hwidth; j++ ) {
        for ( i= hwidth; i < hwidth * 2 ; i++ ) {
            for ( k=0; k<nc; k++ ) { 
                should_be = calc_entry (i, j, neighbors[1] != MPI_PROC_NULL, dim, dims, n_coords, hwidth ); 
                check_entry3D ( data, i+(dim[0]-hwidth*2), j, k, should_be, "out1", &lres );
            }
        }
    }

    //Left HALO Cell  
    if(neighbors[2]!=MPI_PROC_NULL){
        MPI_Cart_coords (cart_comm, neighbors[2], 2, n_coords);
    }
    for ( i = hwidth; i < dim[0] - hwidth; i++ ) {
        for ( j= dim[1] - hwidth*2; j < dim[1]-hwidth ; j++ ) {
            for ( k=0; k<nc; k++ ) { 
                should_be = calc_entry (i, j, neighbors[2] != MPI_PROC_NULL, dim, dims, n_coords, hwidth ); 
                check_entry3D ( data, i, j-(dim[1]-hwidth*2), k, should_be, "out2", &lres );
            }
        }
    }

    //Right HALO Cell
                if(neighbors[3]!=MPI_PROC_NULL){
                    MPI_Cart_coords (cart_comm, neighbors[3], 2, n_coords);
    }
    for ( i = hwidth; i < dim[0] - hwidth; i++ ) {
        for ( j= hwidth; j < hwidth * 2 ; j++ ) {
            for ( k=0; k<nc; k++ ) { 
                should_be = calc_entry (i, j, neighbors[3] != MPI_PROC_NULL, dim, dims, n_coords, hwidth ); 
                check_entry3D ( data, i, j+(dim[1]-hwidth*2), k, should_be, "out2", &lres );
            }
        }
    }

    //Inside
    for ( i=hwidth; i<dim[0]-hwidth; i++) {
	for (j=hwidth; j<dim[1]-hwidth; j++ ){
            for ( k=0; k<nc; k++ ) { 
                should_be = calc_entry (i, j, 1, dim, dims, coords, hwidth ); 
                check_entry3D ( data, i, j, k, should_be, "inside", &lres );
            }
	}
    }

    //Up-Left Corner
    if((neighbors[0]!=MPI_PROC_NULL) && (neighbors[2]!=MPI_PROC_NULL)){
        MPI_Cart_coords (cart_comm, neighbors[0], 2, n_coords);
        c_coords[0] = n_coords[0];
        MPI_Cart_coords (cart_comm, neighbors[2], 2, n_coords);
        c_coords[1] = n_coords[1];
    }
    for ( i= dim[0] - hwidth*2; i < dim[0]-hwidth ; i++ ) {
        for ( j= dim[1] - hwidth*2; j < dim[1]-hwidth ; j++ ) {
            for ( k=0; k<nc; k++ ) { 
                should_be = calc_entry (i, j, (neighbors[0]!=MPI_PROC_NULL) && (neighbors[2]!=MPI_PROC_NULL), 
                        dim, dims, c_coords, hwidth ); 
                check_entry3D ( data, i-(dim[0]-hwidth*2), j-(dim[1]-hwidth*2), k, should_be, "corner1", &lres );
            }
        }
    }

    //Up-right Corner
    if ((neighbors[0]!=MPI_PROC_NULL) && (neighbors[3]!=MPI_PROC_NULL)){
        MPI_Cart_coords (cart_comm, neighbors[0], 2, n_coords);
        c_coords[0] = n_coords[0];
        MPI_Cart_coords (cart_comm, neighbors[3], 2, n_coords);
        c_coords[1] = n_coords[1];
    }
    for ( i= dim[0] - hwidth*2; i < dim[0]-hwidth ; i++ ) {
        for ( j= hwidth ; j < hwidth * 2 ; j++ ) {
            for ( k=0; k<nc; k++ ) { 
                should_be = calc_entry (i, j, (neighbors[0]!=MPI_PROC_NULL) && (neighbors[3]!=MPI_PROC_NULL), 
                        dim, dims, c_coords, hwidth ); 
                check_entry3D ( data, i-(dim[0]-hwidth*2), j+(dim[1]-hwidth*2), k, should_be, "corner2", &lres );
            }
        }
    }

    //Down-left Corner
    if((neighbors[1]!=MPI_PROC_NULL) && (neighbors[2]!=MPI_PROC_NULL)){
        MPI_Cart_coords (cart_comm, neighbors[1], 2, n_coords);
        c_coords[0] = n_coords[0];
        MPI_Cart_coords (cart_comm, neighbors[2], 2, n_coords);
        c_coords[1] = n_coords[1];
    }
    for ( i= hwidth; i < hwidth * 2; i++ ) {
        for ( j= dim[1] - hwidth*2; j < dim[1]-hwidth ; j++ ) {
            for ( k=0; k<nc; k++ ) { 
                should_be = calc_entry (i, j, (neighbors[1]!=MPI_PROC_NULL) && (neighbors[2]!=MPI_PROC_NULL), 
                        dim, dims, c_coords, hwidth ); 
                check_entry3D ( data, i+(dim[0]-hwidth*2), j-(dim[1]-hwidth*2), k, should_be, "corner3", &lres );
            }
        }
    }

    //Down-right Corner
    if((neighbors[1]!=MPI_PROC_NULL) && (neighbors[3]!=MPI_PROC_NULL)){
        MPI_Cart_coords (cart_comm, neighbors[1], 2, n_coords);
        c_coords[0] = n_coords[0];
        MPI_Cart_coords (cart_comm, neighbors[3], 2, n_coords);
        c_coords[1] = n_coords[1];
    }
    for ( i= hwidth; i < hwidth * 2; i++ ) {
        for ( j= hwidth ; j < hwidth * 2 ; j++ ) {
            for ( k=0; k<nc; k++ ) { 
                should_be = calc_entry (i, j, (neighbors[1]!=MPI_PROC_NULL) && (neighbors[3]!=MPI_PROC_NULL), 
                        dim, dims, c_coords, hwidth ); 
                check_entry3D ( data, i+(dim[0]-hwidth*2), j+(dim[1]-hwidth*2), k, should_be, "corner4", &lres );
            }
        }
    }

    MPI_Allreduce ( &lres, &gres, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD );
    if ( gres == 1 ) {
	if ( rank == 0 ) {
	    printf("3-D C testsuite: hwidth = %d, nc = %d passed\n", 
		   hwidth, nc);
	}
    }
    else {
	if ( rank == 0 ) {
	    printf("3-D C testsuite: hwidth = %d, nc = %d failed\n", 
		   hwidth, nc);
	}
	dump_vector_3D_mpi ( data, dim, nc, cart_comm);
    }

    return;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
static void check_data_2D ( double **data, int rank, int *dim, int hwidth, 
			    int *neighbors, MPI_Comm cart_comm)
{
    int i, j, lres=1, gres;
    double should_be;
    int coords[2], n_coords[2], c_coords[2];
    int dims[2]; //size of each dimension
    int period[2];
			
    MPI_Cart_get(cart_comm, 2, dims, period, coords); 

    //Up HALO Cell
    if(neighbors[0] != MPI_PROC_NULL){
        MPI_Cart_coords (cart_comm, neighbors[0], 2, n_coords);
    }
    for ( j = hwidth; j < dim[1] - hwidth; j++ ) {
        for ( i= dim[0] - hwidth*2; i < dim[0]-hwidth ; i++ ) {
        	should_be = calc_entry (i, j, neighbors[0] != MPI_PROC_NULL, dim, dims, n_coords, hwidth ); 
            check_entry2D ( data, i-(dim[0]-hwidth*2), j, should_be, "out1", &lres );
        }
    }

    //Down HALO Cell 
    if(neighbors[1] != MPI_PROC_NULL){
        MPI_Cart_coords (cart_comm, neighbors[1], 2, n_coords);
    }
    for ( j = hwidth; j < dim[1] - hwidth; j++ ) {
        for ( i= hwidth; i < hwidth * 2 ; i++ ) {
        	should_be = calc_entry (i, j, neighbors[1] != MPI_PROC_NULL, dim, dims, n_coords, hwidth ); 
            check_entry2D ( data, i+(dim[0]-hwidth*2), j, should_be, "out1", &lres );
        }
    }

    //Left HALO Cell  
    if(neighbors[2]!=MPI_PROC_NULL){
        MPI_Cart_coords (cart_comm, neighbors[2], 2, n_coords);
    }
    for ( i = hwidth; i < dim[0] - hwidth; i++ ) {
        for ( j= dim[1] - hwidth*2; j < dim[1]-hwidth ; j++ ) {
            should_be = calc_entry (i, j, neighbors[2] != MPI_PROC_NULL, dim, dims, n_coords, hwidth ); 
        	check_entry2D ( data, i, j-(dim[1]-hwidth*2), should_be, "out2", &lres );
        }
    }

    //Right HALO Cell
    if(neighbors[3]!=MPI_PROC_NULL){
    	MPI_Cart_coords (cart_comm, neighbors[3], 2, n_coords);
    }
    for ( i = hwidth; i < dim[0] - hwidth; i++ ) {
        for ( j= hwidth; j < hwidth * 2 ; j++ ) { 
        	should_be = calc_entry (i, j, neighbors[3] != MPI_PROC_NULL, dim, dims, n_coords, hwidth ); 
        	check_entry2D ( data, i, j+(dim[1]-hwidth*2), should_be, "out2", &lres );
        }
    }

    //Inside
    for ( i=hwidth; i<dim[0]-hwidth; i++) {
		for (j=hwidth; j<dim[1]-hwidth; j++ ){ 
			should_be = calc_entry (i, j, 1, dim, dims, coords, hwidth ); 
			check_entry2D ( data, i, j, should_be, "inside", &lres );
		}
    }

    //Up-Left Corner
    if((neighbors[0]!=MPI_PROC_NULL) && (neighbors[2]!=MPI_PROC_NULL)){
        MPI_Cart_coords (cart_comm, neighbors[0], 2, n_coords);
        c_coords[0] = n_coords[0];
        MPI_Cart_coords (cart_comm, neighbors[2], 2, n_coords);
        c_coords[1] = n_coords[1];
    }
    for ( i= dim[0] - hwidth*2; i < dim[0]-hwidth ; i++ ) {
        for ( j= dim[1] - hwidth*2; j < dim[1]-hwidth ; j++ ) {
                should_be = calc_entry (i, j, (neighbors[0]!=MPI_PROC_NULL) && (neighbors[2]!=MPI_PROC_NULL), 
                        dim, dims, c_coords, hwidth ); 
                check_entry2D ( data, i-(dim[0]-hwidth*2), j-(dim[1]-hwidth*2), should_be, "corner1", &lres );
        }
    }

    //Up-right Corner
    if ((neighbors[0]!=MPI_PROC_NULL) && (neighbors[3]!=MPI_PROC_NULL)){
        MPI_Cart_coords (cart_comm, neighbors[0], 2, n_coords);
        c_coords[0] = n_coords[0];
        MPI_Cart_coords (cart_comm, neighbors[3], 2, n_coords);
        c_coords[1] = n_coords[1];
    }
    for ( i= dim[0] - hwidth*2; i < dim[0]-hwidth ; i++ ) {
        for ( j= hwidth ; j < hwidth * 2 ; j++ ) {
                should_be = calc_entry (i, j, (neighbors[0]!=MPI_PROC_NULL) && (neighbors[3]!=MPI_PROC_NULL), 
                        dim, dims, c_coords, hwidth ); 
                check_entry2D ( data, i-(dim[0]-hwidth*2), j+(dim[1]-hwidth*2), should_be, "corner2", &lres );
        }
    }

    //Down-left Corner
    if((neighbors[1]!=MPI_PROC_NULL) && (neighbors[2]!=MPI_PROC_NULL)){
        MPI_Cart_coords (cart_comm, neighbors[1], 2, n_coords);
        c_coords[0] = n_coords[0];
        MPI_Cart_coords (cart_comm, neighbors[2], 2, n_coords);
        c_coords[1] = n_coords[1];
    }
    for ( i= hwidth; i < hwidth * 2; i++ ) {
        for ( j= dim[1] - hwidth*2; j < dim[1]-hwidth ; j++ ) {
        	should_be = calc_entry (i, j, (neighbors[1]!=MPI_PROC_NULL) && (neighbors[2]!=MPI_PROC_NULL), 
                        dim, dims, c_coords, hwidth ); 
            check_entry2D ( data, i+(dim[0]-hwidth*2), j-(dim[1]-hwidth*2), should_be, "corner3", &lres );
        }
    }

    //Down-right Corner
    if((neighbors[1]!=MPI_PROC_NULL) && (neighbors[3]!=MPI_PROC_NULL)){
        MPI_Cart_coords (cart_comm, neighbors[1], 2, n_coords);
        c_coords[0] = n_coords[0];
        MPI_Cart_coords (cart_comm, neighbors[3], 2, n_coords);
        c_coords[1] = n_coords[1];
    }
    for ( i= hwidth; i < hwidth * 2; i++ ) {
        for ( j= hwidth ; j < hwidth * 2 ; j++ ) {			should_be = calc_entry (i, j, (neighbors[1]!=MPI_PROC_NULL) && (neighbors[3]!=MPI_PROC_NULL), 
                        dim, dims, c_coords, hwidth ); 
        	check_entry2D ( data, i+(dim[0]-hwidth*2), j+(dim[1]-hwidth*2), should_be, "corner4", &lres );        }
    }

    MPI_Allreduce ( &lres, &gres, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD );
    if ( gres == 1 ) {
		if ( rank == 0 ) {
	    	printf("2-D C testsuite: hwidth = %d, nc = 0 passed\n", 
		   hwidth );
		}
    }
    else {
		if ( rank == 0 ) {
		    printf("2-D C testsuite: hwidth = %d, nc = 0 failed\n",
		   hwidth);
		}
	    dump_vector_2D_mpi ( data, dim, cart_comm);
		printf("%d: n[0]=%d n[1]=%d n[2]=%d n[3]=%d\n", rank, neighbors[0], 
	       neighbors[1], neighbors[2], neighbors[3] );
    }


    return;
}
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
static void set_data_2D ( double **data, int rank, int *dim, int hwidth, MPI_Comm cart_comm ) 
{
    int i, j;
    int coords[2];
    int dims[2]; //size of each dimension
    int period[2];


    //Get the coordinate of the process
    //MPI_Cart_coords(cart_comm, rank, 2, coords);
    MPI_Cart_get(cart_comm, 2, dims, period, coords);

    for (i=0; i<dim[0]; i++ ) {
		for (j=0; j<hwidth; j++ ){
	    	data[i][j] =-1;
		}
		for (j=dim[1]-hwidth; j<dim[1]; j++ ) {
	    	data[i][j] =-1;
		}
    }
    
	for ( j=0; j<dim[1]; j++ ) {
		for ( i=0; i<hwidth; i++ ) {
		    data[i][j]=-1;
		}
		for ( i=dim[0]-hwidth; i<dim[0]; i++ ) {
		    data[i][j]=-1;
		}
    }

    for ( i=hwidth; i<dim[0]-hwidth; i++) {
		for (j=hwidth; j<dim[1]-hwidth; j++ ){
	    	data[i][j] = (coords[0] * (dim[0]-hwidth*2) + (i-hwidth) ) + (dims[0] * (dim[0] - hwidth*2)) * ((dim[1] - hwidth*2) * coords[1] + (j-hwidth));

		}
    }


    return;
}
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
static void set_data_3D ( double ***data, int rank, int *dim, int hwidth, int nc, MPI_Comm cart_comm ) 
{

    int i, j, k;
    int coords[2];
    int dims[2]; //size of each dimension
    int period[2];

    MPI_Cart_get(cart_comm, 2, dims, period, coords);

    for (i=0; i<dim[0]; i++ ) {
		for (j=0; j<hwidth; j++ ){
	    	for ( k=0; k<nc; k++ ) {
				data[i][j][k] = -1;
	    	}
		}
		for (j=dim[1]-hwidth; j<dim[1]; j++ ) {
	    	for ( k=0; k<nc; k++ ) {
				data[i][j][k] = -1;
	    	}
		}
    }
    
	for ( j=0; j<dim[1]; j++ ) {
		for ( i=0; i<hwidth; i++ ) {
	    	for ( k=0; k<nc; k++ ) {
				data[i][j][k]=-1;
	    	}
		}
		for ( i=dim[0]-hwidth; i<dim[0]; i++ ) {
	    	for ( k=0; k<nc; k++ ) {
				data[i][j][k]=-1;
	    	}
		}
    }

    for ( i=hwidth; i<dim[0]-hwidth; i++) {
		for (j=hwidth; j<dim[1]-hwidth; j++ ){
		    for ( k=0; k<nc; k++ ) {
				data[i][j][k] =  ( coords[0] * (dim[0]-hwidth*2) + (i-hwidth) ) + 
    	                         ( dims[0] * (dim[0]-hwidth*2) ) * ( (dim[1] - hwidth*2) * coords[1] + (j-hwidth) );
	    	}
		}
    }


    return;
}

/**************************************************************************************************/
int calc_entry ( int i, int j, int cond, int *dim, int *dims, int *n_coords, int hwidth ) {
/**************************************************************************************************/
    int entry = -1;

    if ( cond ){
        //printf("coords = %d %d, n_coords = %d, %d \n", coords[0], coords[1], n_coords[0], n_coords[1]);
        entry = (n_coords[0] * (dim[0]-hwidth*2) + (i-hwidth) ) +
            (dims[0] * (dim[0]-hwidth*2)) * ((dim[1] - hwidth*2) * n_coords[1] + (j-hwidth));
    }

    return entry;
}

/**************************************************************************************************/
void check_entry3D ( double ***data, int i, int j, int k, double should_be, char* error, int* lres ) 
/**************************************************************************************************/
{ 
    if ( data[i][j][k] != should_be ){
        *lres = 0;
        printf("3D %s shouldbe = %lf data= %f nc= %d \n", error, should_be, data[i][j][k], k);
    }
}

/**************************************************************************************************/
void check_entry2D ( double **data, int i, int j, double should_be, char* error, int* lres ) 
/**************************************************************************************************/
{ 
    if ( data[i][j] != should_be ){
        *lres = 0;
        printf("2D %s shouldbe = %lf data= %f \n", error, should_be, data[i][j]);
    }
}


