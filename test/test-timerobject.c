/*
 * Copyright (c) 2006-2007      University of Houston. All rights reserved.
 * Copyright (c) 2009           HLRS. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include <stdio.h>
#include <stdlib.h> 

#include "ADCL.h"
#include "mpi.h"

/* Test program for timer object
   =============================
   2D neighborhood communication, two requests with two different emethods
*/

/* Dimensions of the data matrix per process */
#define DIM0  8
#define DIM1  4

void dump_vector_2D ( double **data, int rank, int *dim);
void dump_vector_2D_int ( int **data, int rank, int *dim);
void set_data_2D ( double **data, int rank, int *dim, int hwidth );
void set_data_2D_int ( int **data, int rank, int *dim, int hwidth );
void check_data_2D ( double **data, int rank, int *dim, 
			    int hwidth, int *neighbors ); 
void check_data_2D_int ( int **data, int rank, int *dim, 
			    int hwidth, int *neighbors ); 

/**********************************************************************/
int main ( int argc, char ** argv ) 
/**********************************************************************/
{
    /* General variables */
    int hwidth, rank, size, err, it, maxit;
    
    /* Definition of the 2-D vector */
    int dims[2], neighbors[4];
    double **data1;
    int **data2;
    ADCL_Vmap vmap;
    ADCL_Vector vec1, vec2;

    /* Variables for the process topology information */
    int cdims[]={0,0};
    int periods[]={0,0};
    MPI_Comm cart_comm;
    ADCL_Topology topo;
    ADCL_Request req1, req2;
    ADCL_Request *reqs;  

    /* Timer */
    ADCL_Timer timer;

    /* Initiate the MPI environment */
    MPI_Init ( &argc, &argv );
    MPI_Comm_rank ( MPI_COMM_WORLD, &rank );
    MPI_Comm_size ( MPI_COMM_WORLD, &size );
    if (size == 1) {
        printf("This is a MPI program, start with more than one processor.\n");
        return 0;
    }

    /* Describe the neighborhood relations */
    MPI_Dims_create ( size, 2, cdims );
    MPI_Cart_create ( MPI_COMM_WORLD, 2, cdims, periods, 0, &cart_comm);
    MPI_Cart_shift ( cart_comm, 0, 1, &(neighbors[0]), &(neighbors[1]));
    MPI_Cart_shift ( cart_comm, 1, 1, &(neighbors[2]), &(neighbors[3]));

    /* Initiate the ADCL library and register a topology object with ADCL */
    ADCL_Init ();
    ADCL_Topology_create ( cart_comm, &topo );

    hwidth=1;
    dims[0] = DIM0 + 2*hwidth;
    dims[1] = DIM1 + 2*hwidth;

    err = ADCL_Vmap_halo_allocate ( hwidth, &vmap );
    if ( ADCL_SUCCESS != err) goto exit;

    /* allocate vector and request no. 1  */
    err = ADCL_Vector_allocate_generic ( 2,  dims, 0, vmap, MPI_DOUBLE, &data1, &vec1 );
    if ( ADCL_SUCCESS != err) goto exit;
    ADCL_Request_create ( vec1, topo, ADCL_FNCTSET_NEIGHBORHOOD, &req1 );

    /* allocate vector and request no. 1  */
    err = ADCL_Vector_allocate_generic ( 2,  dims, 0, vmap, MPI_INT, &data2, &vec2 );
    if ( ADCL_SUCCESS != err) goto exit;
    ADCL_Request_create ( vec2, topo, ADCL_FNCTSET_NEIGHBORHOOD, &req2 );

    /* define timer object */
    reqs = (ADCL_Request *) malloc ( 2 * sizeof (ADCL_Request) );
    reqs[0] = req1; 
    reqs[1] = req2;
    ADCL_Timer_create ( 2, reqs, &timer );
    free ( reqs ); 

    maxit = 100;
    for (it=0; it<maxit; it++){
      ADCL_Timer_start( timer );
      set_data_2D ( data1, rank, dims, hwidth );
      set_data_2D_int ( data2, rank, dims, hwidth );
#ifdef VERBOSE
      dump_vector_2D ( data1, rank, dims );
      dump_vector_2D_int ( data2, rank, dims );
#endif

      ADCL_Request_start ( req1 );
      ADCL_Request_start ( req2 );

      check_data_2D ( data1, rank, dims, hwidth, neighbors );
      check_data_2D_int ( data2, rank, dims, hwidth, neighbors );

      ADCL_Timer_stop( timer );
    }

    ADCL_Timer_free   ( &timer );
    ADCL_Request_free ( &req1 );
    ADCL_Request_free ( &req2 );
    ADCL_Vector_free  ( &vec1 );
    ADCL_Vector_free  ( &vec2 );
    ADCL_Vmap_free    ( &vmap );

exit: 
    ADCL_Topology_free ( &topo );
    MPI_Comm_free ( &cart_comm );
    
    ADCL_Finalize ();
    MPI_Finalize ();
    return 0;
}









