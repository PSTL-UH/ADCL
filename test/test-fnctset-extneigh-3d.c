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

/* Dimensions of the data matrix per process */
#define DIM0  4
#define DIM1  5
#define DIM2  6

static void dump_vector_3D ( double ***data, int rank, int *dim);
static void dump_vector_4D ( double ****data, int rank, int *dim, int nc);

static void set_data_3D ( double ***data, int rank, MPI_Comm cart_comm, int *dim, int hwidth );
static void set_data_4D ( double ****data, int rank, MPI_Comm cart_comm, int *dim, int hwidth, int nc);

static void check_data_3D ( double ***data, int rank, MPI_Comm cart_comm, int *dim, int hwidth, int *neighbors ); 
static void check_data_4D ( double ****data, int rank, MPI_Comm cart_comm, int *dim, int hwidth, int nc, int *neighbors ); 

static int calc_entry3D ( int control_x, int control_y, int control_z,  double ***data, int rank, 
        MPI_Comm cart_comm, int *dim, int hwidth, int *neighbors);
static int calc_entry4D ( int control_x, int control_y, int control_z,  double ****data, int rank, 
        MPI_Comm cart_comm, int *dim, int hwidth, int *neighbors, int nc);


int main ( int argc, char ** argv ) 
{
    /* General variables */
    int hwidth, rank, size, err;

    /* Definition of the 2-D vector */
    int dims[3], neighbors[6];
    double ***data, ****data2;
    ADCL_Vector vec;

    /* Variables for the process topology information */
    int cdims[]={0,0,0};
    int periods[]={0,0,0};
    ADCL_Vmap vmap;
    MPI_Comm cart_comm;
    ADCL_Topology topo;
    ADCL_Request request;

    /* Initiate the MPI environment */
    MPI_Init ( &argc, &argv );
    MPI_Comm_rank ( MPI_COMM_WORLD, &rank );
    MPI_Comm_size ( MPI_COMM_WORLD, &size );

    /* Describe the neighborhood relations */
    MPI_Dims_create ( size, 3, cdims );
    MPI_Cart_create ( MPI_COMM_WORLD, 3, cdims, periods, 0, &cart_comm);
    MPI_Cart_shift ( cart_comm, 0, 1, &(neighbors[0]), &(neighbors[1]));
    MPI_Cart_shift ( cart_comm, 1, 1, &(neighbors[2]), &(neighbors[3]));
    MPI_Cart_shift ( cart_comm, 2, 1, &(neighbors[4]), &(neighbors[5]));


    /* Initiate the ADCL library and register a topology object with ADCL */
    ADCL_Init ();
    ADCL_Topology_create_extended ( cart_comm, &topo );

    /**********************************************************************/
    /* Test 1: hwidth=1, nc=0 */
    hwidth=1;
    dims[0] = DIM0 + 2*hwidth;
    dims[1] = DIM1 + 2*hwidth;
    dims[2] = DIM2 + 2*hwidth;
    err = ADCL_Vmap_halo_allocate ( hwidth, &vmap );
    if ( ADCL_SUCCESS != err) goto exit;
    err = ADCL_Vector_allocate_generic ( 3,  dims, 0, vmap, MPI_DOUBLE, &data, &vec );
    if ( ADCL_SUCCESS != err) goto exit;
    ADCL_Request_create ( vec, topo, ADCL_FNCTSET_NEIGHBORHOOD, &request );
    set_data_3D ( data, rank, cart_comm, dims, hwidth );
#ifdef VERBOSE3D
    dump_vector_3D ( data, rank, dims );
#endif

    ADCL_Request_start ( request );
    check_data_3D ( data, rank, cart_comm, dims, hwidth, neighbors );

    ADCL_Request_free ( &request );
    ADCL_Vector_free ( &vec );
    ADCL_Vmap_free ( &vmap ); 

    /**********************************************************************/
    /* Test 2: hwidth=2, nc=0 */
    hwidth=2;
    dims[0] = DIM0 + 2*hwidth;
    dims[1] = DIM1 + 2*hwidth;
    dims[2] = DIM2 + 2*hwidth;
    err = ADCL_Vmap_halo_allocate ( hwidth, &vmap );
    if ( ADCL_SUCCESS != err) goto exit;
    err = ADCL_Vector_allocate_generic ( 3,  dims, 0, vmap, MPI_DOUBLE, &data, &vec );
    if ( ADCL_SUCCESS != err) goto exit;
    ADCL_Request_create ( vec, topo, ADCL_FNCTSET_NEIGHBORHOOD, &request );

    set_data_3D ( data, rank, cart_comm, dims, hwidth );
#ifdef VERBOSE3D2
    dump_vector_3D ( data, rank, dims );
#endif

    ADCL_Request_start ( request );
    check_data_3D ( data, rank, cart_comm, dims, hwidth, neighbors );

    ADCL_Request_free ( &request );
    ADCL_Vector_free ( &vec );
    ADCL_Vmap_free ( &vmap ); 

    /**********************************************************************/
    /* Test 3: hwidth=1, nc=1 */
    hwidth=1;
    dims[0] = DIM0 + 2*hwidth;
    dims[1] = DIM1 + 2*hwidth;
    dims[2] = DIM2 + 2*hwidth;
    err = ADCL_Vmap_halo_allocate ( hwidth, &vmap );
    if ( ADCL_SUCCESS != err) goto exit;
    err = ADCL_Vector_allocate_generic ( 3,  dims, 1, vmap, MPI_DOUBLE, &data2, &vec );
    if ( ADCL_SUCCESS != err) goto exit;
    ADCL_Request_create ( vec, topo, ADCL_FNCTSET_NEIGHBORHOOD, &request );

    set_data_4D ( data2, rank, cart_comm, dims, hwidth, 1 );
#ifdef VERBOSE4D
    dump_vector_4D ( data2, rank, dims, 1 );
#endif

    ADCL_Request_start ( request );
    check_data_4D ( data2, rank, cart_comm, dims, hwidth, 1, neighbors);

    ADCL_Request_free ( &request );
    ADCL_Vector_free ( &vec );
    ADCL_Vmap_free ( &vmap ); 

    /**********************************************************************/
    /* Test 4: hwidth=2, nc=1 */
    hwidth=2;
    dims[0] = DIM0 + 2*hwidth;
    dims[1] = DIM1 + 2*hwidth;
    dims[2] = DIM2 + 2*hwidth;
    err = ADCL_Vmap_halo_allocate ( hwidth, &vmap );
    if ( ADCL_SUCCESS != err) goto exit;
    err = ADCL_Vector_allocate_generic ( 3,  dims, 1, vmap, MPI_DOUBLE, &data2, &vec );
    if ( ADCL_SUCCESS != err) goto exit;
    ADCL_Request_create ( vec, topo, ADCL_FNCTSET_NEIGHBORHOOD, &request );

    set_data_4D ( data2, rank, cart_comm, dims, hwidth, 1);
#ifdef VERBOSE4D
    dump_vector_4D ( data2, rank, dims, 1 );
#endif

    ADCL_Request_start ( request );
    check_data_4D ( data2, rank, cart_comm, dims, hwidth, 1, neighbors );

    ADCL_Request_free ( &request );
    ADCL_Vector_free ( &vec );
    ADCL_Vmap_free ( &vmap ); 

    /**********************************************************************/
    /* Test 5: hwidth=2, nc=2 */
    hwidth=2;
    dims[0] = DIM0 + 2*hwidth;
    dims[1] = DIM1 + 2*hwidth;
    dims[2] = DIM2 + 2*hwidth;
    err = ADCL_Vmap_halo_allocate ( hwidth, &vmap );
    if ( ADCL_SUCCESS != err) goto exit;
    err = ADCL_Vector_allocate_generic ( 3,  dims, 2, vmap, MPI_DOUBLE, &data2, &vec );
    if ( ADCL_SUCCESS != err) goto exit;
    ADCL_Request_create ( vec, topo, ADCL_FNCTSET_NEIGHBORHOOD, &request );

    set_data_4D ( data2, rank, cart_comm, dims, hwidth, 2 );
#ifdef VERBOSE4D
    dump_vector_4D ( data2, rank, dims, 2 );
#endif

    ADCL_Request_start ( request );
    check_data_4D ( data2, rank, cart_comm, dims, hwidth, 2, neighbors);

    ADCL_Request_free ( &request );
    ADCL_Vector_free ( &vec );
    ADCL_Vmap_free ( &vmap ); 

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

static void check_data_3D ( double ***data, int rank, MPI_Comm cart_comm, int *dim, int hwidth, int *neighbors)
{
    int control_x, control_y, control_z, lres=1, gres;

    // check for each of the 27 possible locations
    for(control_x=-1; control_x<=1; control_x++){
        for(control_y=-1; control_y<=1; control_y++){
            for(control_z=-1; control_z<=1; control_z++){
#ifdef INCCORNER
                if((control_x * control_y * control_z) != 0){
                    // corner
                    lres = calc_entry3D ( control_x, control_y, control_z, data, rank, cart_comm, dim, hwidth, neighbors);
                }
#endif
                if((control_x * control_y * control_z) == 0){
                    // edge, face or inside
                    lres = calc_entry3D ( control_x, control_y, control_z, data, rank, cart_comm, dim, hwidth, neighbors);
                }
            }
        }
    }

    MPI_Allreduce ( &lres, &gres, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD );

    if ( gres == 1 ) {
        if ( rank == 0 ) {
            printf("3-D C testsuite: hwidth = %d, nc = 0 passed\n", hwidth );
        }
    }
    else {
        if ( rank == 0 ) {
            printf("3-D C testsuite: hwidth = %d, nc = 0 failed\n", hwidth);
        }
        dump_vector_3D ( data, rank, dim );
    }
    return;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

static void check_data_4D ( double ****data, int rank, MPI_Comm cart_comm, int *dim, int hwidth, int nc, int *neighbors) 
{

    int control_x, control_y, control_z, lres=1, gres;

    for(control_x=-1; control_x<=1; control_x++){
        for(control_y=-1; control_y<=1; control_y++){
            for(control_z=-1; control_z<=1; control_z++){
#ifdef INCCORNER			
                if((control_x * control_y * control_z) != 0){
                    lres = calc_entry4D ( control_x, control_y, control_z, data, rank, cart_comm, dim, hwidth, neighbors, nc);
                }
#endif
                if((control_x * control_y * control_z) == 0){
                    lres = calc_entry4D ( control_x, control_y, control_z, data, rank, cart_comm, dim, hwidth, neighbors, nc);
                }
            }
        }
    }

    MPI_Allreduce ( &lres, &gres, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD );
    if ( gres == 1 ) {
        if ( rank == 0 ) {
            printf("4-D C testsuite: hwidth = %d, nc = 0 passed\n", 
                    hwidth );
        }
    }
    else {
        if ( rank == 0 ) {
            printf("4-D C testsuite: hwidth = %d, nc = 0 failed\n",
                    hwidth);
        }
        dump_vector_4D ( data, rank, dim , nc);
    }


    return;

}
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
static void set_data_3D ( double ***data, int rank, MPI_Comm cart_comm, int *dim, int hwidth ) 
{
    int i, j, k;
    int coords[3];
    int dims[3]; //size of each dimension
    int period[3];

    //Get the coordinate of the process
    //MPI_Cart_coords(cart_comm, rank, 2, coords);
    MPI_Cart_get(cart_comm, 3, dims, period, coords);

    for (i=0; i<dim[0]; i++ ) {
        for (j=0; j<dim[1]; j++ ){
            for (k=0; k<hwidth; k++ ){
                data[i][j][k] = -1;
            }
            for (k=dim[2]-hwidth; k<dim[2]; k++ ) {
                data[i][j][k] = -1;
            }
        }
    }

    for (i=0; i<dim[0]; i++ ) {
        for (k=0; k<dim[2]; k++ ){
            for (j=0; j<hwidth; j++ ){
                data[i][j][k] = -1;
            }
            for (j=dim[1]-hwidth; j<dim[1]; j++ ) {
                data[i][j][k] = -1;
            }
        }
    }

    for (j=0; j<dim[1]; j++ ) {
        for (k=0; k<dim[2]; k++ ){
            for (i=0; i<hwidth; i++ ){
                data[i][j][k] = -1;
            }
            for (i=dim[0]-hwidth; i<dim[0]; i++ ) {
                data[i][j][k] = -1;
            }
        }
    }

    for ( i=hwidth; i<dim[0]-hwidth; i++) {
        for (j=hwidth; j<dim[1]-hwidth; j++ ){
            for (k=hwidth; k<dim[2]-hwidth; k++ ){
                //printf("rank = %d, coords[0] = %d, coords[1] = %d, coords[2] = %d \n", rank, coords[0], coords[1], coords[2]);
                data[i][j][k] = (coords[2] * (dim[2]-hwidth*2) + (k-hwidth) ) 
                    + (dims[2] * (dim[2] - hwidth*2)) * ((dim[1] - hwidth*2) * coords[1] + (j-hwidth)) 
                    + (dims[2] * (dim[2] - hwidth*2)) * (dims[1] * (dim[1] - hwidth*2)) * ((dim[0] - hwidth*2) * coords[0]+(i-hwidth)) ;
            }  
        }
    }


    return;
}
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
static void set_data_4D ( double ****data, int rank,  MPI_Comm cart_comm, int *dim, int hwidth, int nc ) 
{
    int i, j, k, l;
    int coords[3];
    int dims[3]; //size of each dimension
    int period[3];

    //Get the coordinate of the process
    //MPI_Cart_coords(cart_comm, rank, 2, coords);
    MPI_Cart_get(cart_comm, 3, dims, period, coords);

    for (i=0; i<dim[0]; i++ ) {
        for (j=0; j<dim[1]; j++ ){
            for (k=0; k<hwidth; k++ ){
                for (l=0; l<nc; l++ ) {
                    data[i][j][k][l] = -1;
                }
            }
            for (k=dim[2]-hwidth; k<dim[2]; k++ ) {
                for (l=0; l<nc; l++ ) {
                    data[i][j][k][l] = -1;
                }
            }
        }
    }

    for (i=0; i<dim[0]; i++ ) {
        for (k=0; k<dim[2]; k++ ){
            for (j=0; j<hwidth; j++ ){
                for (l=0; l<nc; l++ ) {
                    data[i][j][k][l] = -1;
                }
            }
            for (j=dim[1]-hwidth; j<dim[1]; j++ ) {
                for (l=0; l<nc; l++ ) {
                    data[i][j][k][l] = -1;
                }
            }
        }
    }

    for (j=0; j<dim[1]; j++ ) {
        for (k=0; k<dim[2]; k++ ){
            for (i=0; i<hwidth; i++ ){
                for (l=0; l<nc; l++ ) {
                    data[i][j][k][l] = -1;
                }
            }
            for (i=dim[0]-hwidth; i<dim[0]; i++ ) {
                for (l=0; l<nc; l++ ) {
                    data[i][j][k][l] = -1;
                }
            }
        }
    }


    for ( i=hwidth; i<dim[0]-hwidth; i++) {
        for (j=hwidth; j<dim[1]-hwidth; j++ ){
            for (k=hwidth; k<dim[2]-hwidth; k++ ){
                for (l=0; l<nc; l++ ) {
                    //printf("rank = %d, coords[0] = %d, coords[1] = %d, coords[2] = %d \n", rank, coords[0], coords[1], coords[2]);
                    data[i][j][k][l] = (coords[2] * (dim[2]-hwidth*2) + (k-hwidth) ) 
                        + (dims[2] * (dim[2] - hwidth*2)) * ((dim[1] - hwidth*2) * coords[1] + (j-hwidth)) 
                        + (dims[2] * (dim[2] - hwidth*2)) * (dims[1] * (dim[1] - hwidth*2)) * ((dim[0] - hwidth*2) * coords[0]+(i-hwidth)) ;
                }          
            }  
        }
    }



    return;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
static void dump_vector_3D ( double ***data, int rank, int *dim)
{
    int i, j, k;

    for (i=0; i<dim[0]; i++) {
        for ( j=0; j<dim[1]; j++ ) {
            printf("Rank %d : dim[0]=%d dim[1]=%d ", rank, i, j);
            for ( k=0; k<dim[2]; k++ ) {
                printf("%lf ", data[i][j][k]);
            }
            printf ("\n");
        }
    }
    return;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
static void dump_vector_4D ( double ****data, int rank, int *dim, int nc)
{
    int i, j, k, l;

    for (i=0; i<dim[0]; i++) {
        for ( j=0; j<dim[1]; j++ ) {
            printf("Rank %d dim[0]=%d dim[1]=%d: ", rank, i, j);
            for ( k=0; k<dim[2]; k++ ) {
                for ( l=0; l<nc; l++ ) {
                    printf("%lf ", data[i][j][k][l]);
                }
            }
            printf ("\n");
        }
    }

    return;
}


static int calc_entry3D ( int control_x, int control_y, int control_z,  double ***data, int rank, MPI_Comm cart_comm, int *dim, int hwidth, 
        int *neighbors)
{
    int i, j, k;
    int lres=1;
    double should_be;
    int coords[3], n_coords[3], c_coords[3];
    int dims[3]; //size of each dimension
    int period[3];
    int compensate_i, compensate_j, compensate_k; 
    int compensate[3]; //are used to calculate compensate_i,j,k
    int	loopstart[3], loopend[3];
    int neighbor_cond[3];

    MPI_Cart_get(cart_comm, 3, dims, period, coords); 
    neighbor_cond[0] = neighbor_cond[1] = neighbor_cond[2] = 0;

    switch(control_x){
        case 0:
            loopstart[0] = hwidth;
            loopend[0] = dim[0] - hwidth;
            compensate[0] = 0;
            c_coords[0] = coords[0];
            neighbor_cond[0] = 1;
            break;

        case 1:
            loopstart[0] = dim[0] - hwidth *2;
            loopend[0] = dim[0]-hwidth ;
            compensate[0] = - (dim[0] - hwidth*2) ;
            if(neighbors[0]!=MPI_PROC_NULL){
                MPI_Cart_coords (cart_comm, neighbors[0], 3, n_coords);
                c_coords[0] = n_coords[0];
                neighbor_cond[0] = 1;
            }
            break;

        case -1:
            loopstart[0] = hwidth;
            loopend[0] = hwidth * 2;	
            compensate[0] = (dim[0] - hwidth*2) ;
            if(neighbors[1]!=MPI_PROC_NULL){
                MPI_Cart_coords (cart_comm, neighbors[1], 3, n_coords);
                c_coords[0] = n_coords[0];
                neighbor_cond[0] = 1;
            }
    }

    switch(control_y){
        case 0:		
            loopstart[1] = hwidth;
            loopend[1] = dim[1] - hwidth;
            compensate[1] = 0;
            c_coords[1] = coords[1];
            neighbor_cond[1] = 1;
            break;

        case 1:		
            loopstart[1] = dim[1] - hwidth *2;
            loopend[1] = dim[1]-hwidth ; 
            compensate[1] = - (dim[1] - hwidth*2) ;
            if(neighbors[2]!=MPI_PROC_NULL){
                MPI_Cart_coords (cart_comm, neighbors[2], 3, n_coords);
                c_coords[1] = n_coords[1];
                neighbor_cond[1] = 1;
            }
            break;

        case -1:	
            loopstart[1] = hwidth;
            loopend[1] = hwidth * 2;		
            compensate[1] = (dim[1] - hwidth*2) ;
            if(neighbors[3]!=MPI_PROC_NULL){
                MPI_Cart_coords (cart_comm, neighbors[3], 3, n_coords);
                c_coords[1] = n_coords[1];
                neighbor_cond[1] = 1;
            }
    }

    switch(control_z){
        case 0:		
            loopstart[2] = hwidth;
            loopend[2] = dim[2] - hwidth;
            compensate[2] = 0;
            c_coords[2] = coords[2];
            neighbor_cond[2] = 1;
            break;

        case 1:		
            loopstart[2] = dim[2] - hwidth *2;
            loopend[2] = dim[2]-hwidth ; 
            compensate[2] = - (dim[2] - hwidth*2) ;
            if(neighbors[4]!=MPI_PROC_NULL){
                MPI_Cart_coords (cart_comm, neighbors[4], 3, n_coords);
                c_coords[2] = n_coords[2];
                neighbor_cond[2] = 1;
            }
            break;

        case -1:	
            loopstart[2] = hwidth;
            loopend[2] = hwidth * 2;	 	
            compensate[2] = (dim[2] - hwidth*2) ;
            if(neighbors[5]!=MPI_PROC_NULL){
                MPI_Cart_coords (cart_comm, neighbors[5], 3, n_coords);
                c_coords[2] = n_coords[2];
                neighbor_cond[2] = 1;
            }
    }

    for ( i = loopstart[0]; i < loopend[0]; i++){
        for ( j= loopstart[1]; j < loopend[1]; j++){
            for (k = loopstart[2]; k < loopend[2]; k++){
                if(neighbor_cond[0] && neighbor_cond[1] && neighbor_cond[2]){				
                    should_be = (c_coords[2] * (dim[2]-hwidth*2) + (k-hwidth) ) 
                        + (dims[2] * (dim[2] - hwidth*2)) * ((dim[1] - hwidth*2) * c_coords[1] + (j-hwidth)) 
                        + (dims[2] * (dim[2] - hwidth*2)) * (dims[1] * (dim[1] - hwidth*2)) * ((dim[0] - hwidth*2) * c_coords[0]+(i-hwidth)) ;
                }
                else{
                    should_be = -1;
                }

                compensate_i = i + compensate[0];
                compensate_j = j + compensate[1];
                compensate_k = k + compensate[2];

                if ( data[compensate_i][compensate_j][compensate_k] != should_be ){
                    lres = 0;
                    printf("3D shouldbe = %f data= %f control_x= %d control_y= %d control_z= %d rank = %d\n", 
                            should_be, data[compensate_i][compensate_j][compensate_k], control_x, control_y, control_z, rank);
                }
            }
        }
    }
    return lres;
}

static int calc_entry4D ( int control_x, int control_y, int control_z,  double ****data, int rank, MPI_Comm cart_comm, int *dim, int hwidth, 
        int *neighbors, int nc)
{
    int i, j, k, l;
    int lres=1;
    double should_be;
    int coords[3], n_coords[3], c_coords[3];
    int dims[3]; //size of each dimension
    int period[3];
    int compensate_i, compensate_j, compensate_k; 
    int compensate[3];
    int	loopstart[3], loopend[3];
    int neighbor_cond[3];

    MPI_Cart_get(cart_comm, 3, dims, period, coords); 
    neighbor_cond[0] = neighbor_cond[1] = neighbor_cond[2] = 0;

    switch(control_x){
        case 0:		
            loopstart[0] = hwidth;
            loopend[0] = dim[0] - hwidth;
            compensate[0] = 0;
            c_coords[0] = coords[0];
            neighbor_cond[0] = 1;
            break;

        case 1:		
            loopstart[0] = dim[0] - hwidth *2;
            loopend[0] = dim[0]-hwidth ;
            compensate[0] = - (dim[0] - hwidth*2) ;
            if(neighbors[0]!=MPI_PROC_NULL){
                MPI_Cart_coords (cart_comm, neighbors[0], 3, n_coords);
                c_coords[0] = n_coords[0];
                neighbor_cond[0] = 1;
            }
            break;

        case -1:	
            loopstart[0] = hwidth;
            loopend[0] = hwidth * 2;	
            compensate[0] = (dim[0] - hwidth*2) ;
            if(neighbors[1]!=MPI_PROC_NULL){
                MPI_Cart_coords (cart_comm, neighbors[1], 3, n_coords);
                c_coords[0] = n_coords[0];
                neighbor_cond[0] = 1;
            }
    }

    switch(control_y){
        case 0:		
            loopstart[1] = hwidth;
            loopend[1] = dim[1] - hwidth;
            compensate[1] = 0;
            c_coords[1] = coords[1];
            neighbor_cond[1] = 1;
            break;

        case 1:		
            loopstart[1] = dim[1] - hwidth *2;
            loopend[1] = dim[1]-hwidth ; 
            compensate[1] = - (dim[1] - hwidth*2) ;
            if(neighbors[2]!=MPI_PROC_NULL){
                MPI_Cart_coords (cart_comm, neighbors[2], 3, n_coords);
                c_coords[1] = n_coords[1];
                neighbor_cond[1] = 1;
            }
            break;

        case -1:	
            loopstart[1] = hwidth;
            loopend[1] = hwidth * 2;		
            compensate[1] = (dim[1] - hwidth*2) ;
            if(neighbors[3]!=MPI_PROC_NULL){
                MPI_Cart_coords (cart_comm, neighbors[3], 3, n_coords);
                c_coords[1] = n_coords[1];
                neighbor_cond[1] = 1;
            }
    }

    switch(control_z){
        case 0:		
            loopstart[2] = hwidth;
            loopend[2] = dim[2] - hwidth;
            compensate[2] = 0;
            c_coords[2] = coords[2];
            neighbor_cond[2] = 1;
            break;

        case 1:		
            loopstart[2] = dim[2] - hwidth *2;
            loopend[2] = dim[2]-hwidth ; 
            compensate[2] = - (dim[2] - hwidth*2) ;
            if(neighbors[4]!=MPI_PROC_NULL){
                MPI_Cart_coords (cart_comm, neighbors[4], 3, n_coords);
                c_coords[2] = n_coords[2];
                neighbor_cond[2] = 1;
            }
            break;

        case -1:	
            loopstart[2] = hwidth;
            loopend[2] = hwidth * 2;	 	
            compensate[2] = (dim[2] - hwidth*2) ;
            if(neighbors[5]!=MPI_PROC_NULL){
                MPI_Cart_coords (cart_comm, neighbors[5], 3, n_coords);
                c_coords[2] = n_coords[2];
                neighbor_cond[2] = 1;
            }
    }

    for ( i = loopstart[0]; i < loopend[0]; i++){
        for ( j= loopstart[1]; j < loopend[1]; j++){
            for (k = loopstart[2]; k < loopend[2]; k++){
                for (l = 0; l < nc; l++){
                    if(neighbor_cond[0] && neighbor_cond[1] && neighbor_cond[2]){				
                        should_be = (c_coords[2] * (dim[2]-hwidth*2) + (k-hwidth) ) 
                            + (dims[2] * (dim[2] - hwidth*2)) * ((dim[1] - hwidth*2) * c_coords[1] + (j-hwidth)) 
                            + (dims[2] * (dim[2] - hwidth*2)) * (dims[1] * (dim[1] - hwidth*2)) * ((dim[0] - hwidth*2) * c_coords[0]+(i-hwidth)) ;
                    }
                    else{
                        should_be = -1;
                    }

                    compensate_i = i + compensate[0];
                    compensate_j = j + compensate[1];
                    compensate_k = k + compensate[2];

                    if ( data[compensate_i][compensate_j][compensate_k][l] != should_be ){
                        lres = 0;
                        printf("4D shouldbe = %f data= %f control_x= %d control_y= %d control_z= %d rank = %d nc= %d\n", 
                                should_be, data[compensate_i][compensate_j][compensate_k][l], control_x, control_y, control_z, rank, nc);
                    }
                }
            }
        }
    }
    return lres;
}


