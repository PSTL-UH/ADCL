
#include <stdio.h>
#include "mpi.h"
#include "ADCL.h"
#include "nbc.h"

int main (int argc, char *argv[])
{
  int i, cnt, dim, err;
  int rank, size;
  int cdims=0;
  int periods=0;
  
  ADCL_Topology topo;
  ADCL_Vector svec, rvec;
  ADCL_Vmap svmap, rvmap;
  ADCL_Request request;
  int root = 0;
  cnt = 1;
  dim = 1;
  MPI_Comm cart_comm;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);
  MPI_Comm_size (MPI_COMM_WORLD, &size);
  
  int *sdata, *rdata;
  
  ADCL_Init ();
  
  // Creating the ADCL Topology  
  MPI_Dims_create ( size, 1, &cdims );
  MPI_Cart_create ( MPI_COMM_WORLD, 1, &cdims, &periods, 0, &cart_comm);
  err = ADCL_Topology_create ( cart_comm, &topo );
  if ( ADCL_SUCCESS != err) goto exit;
  
  
  // Creating the ADCL Vector
  err = ADCL_Vmap_all_allocate( &svmap ); 
  if ( ADCL_SUCCESS != err) goto exit;   
  err = ADCL_Vmap_all_allocate( &rvmap ); 
  if ( ADCL_SUCCESS != err) goto exit;   

  err = ADCL_Vector_allocate_generic ( 1, &dim, 0, svmap, MPI_INT, &sdata, &svec );
  if ( ADCL_SUCCESS != err) goto exit;   
  err = ADCL_Vector_allocate_generic ( 1, &dim, 0, rvmap, MPI_INT, &rdata, &rvec );
  if ( ADCL_SUCCESS != err) goto exit;   
  
  // Creating the ADCL Request
  err = ADCL_Request_create_generic_rooted ( svec, rvec, topo, ADCL_FNCTSET_IBCAST, root, &request );
  if ( ADCL_SUCCESS != err) goto exit;   
  
  for ( i = 0; i< 150; i++ ) {  
    if(rank == root){
      *sdata = 123+i;
    }else{
      *sdata = 10;
    }

    // Request start (ibcast)
    err = ADCL_Request_start( request );



//    err = ADCL_Request_init( request );
    if ( ADCL_SUCCESS != err) goto exit;


    // do something incredibly important

  //  err = ADCL_Request_wait ( request );
    if ( ADCL_SUCCESS != err) goto exit;   
    
    if ( *sdata != 123+i ) {
      printf("Process: %d, buff: %d\n",rank,*sdata);
    }
  }
  
  exit:
    if ( ADCL_SUCCESS != err) { printf("ADCL error nr. %d\n", err); } 

    if ( ADCL_REQUEST_NULL != request) ADCL_Request_free ( &request );
    if ( ADCL_VECTOR_NULL  != svec)    ADCL_Vector_free ( &svec );
    if ( ADCL_VECTOR_NULL  != rvec)    ADCL_Vector_free ( &rvec );
    if ( ADCL_VMAP_NULL    != svmap)   ADCL_Vmap_free (&svmap);
    if ( ADCL_VMAP_NULL    != rvmap)   ADCL_Vmap_free (&rvmap);
	
    if ( ADCL_TOPOLOGY_NULL != topo)   ADCL_Topology_free ( &topo );
    MPI_Comm_free ( &cart_comm );
    ADCL_Finalize ();
    MPI_Finalize ();

   return 0;
}
