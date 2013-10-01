
#include <stdio.h>
#include "mpi.h"
#include "ADCL.h"
#include "nbc.h"
#include "latency.h"
#include "latency_internal.h"
#include "eddhr.h"

int main (int argc, char *argv[])
{
  int i, dim_cnt, dim, err, j, k, maxit,total_progress_calls=0;
  int rank, size;
  int cdims=0;
  int periods=0;
  
  ADCL_Topology topo;
  ADCL_Vector svec, rvec;
  ADCL_Vmap svmap, rvmap;
  ADCL_Request request;
  ADCL_Timer timer;

  int root = 0;
  dim_cnt = 1;
  dim = 1024;
  MPI_Comm cart_comm;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);
  MPI_Comm_size (MPI_COMM_WORLD, &size);
  
  char *sdata;//char  *rdata;
  
  ADCL_Init ();
  
  // Creating the ADCL Topology  
  MPI_Dims_create ( size, 1, &cdims );
  MPI_Cart_create ( MPI_COMM_WORLD, 1, &cdims, &periods, 0, &cart_comm);
  err = ADCL_Topology_create ( cart_comm, &topo );
  if ( ADCL_SUCCESS != err) goto exit;
  
  
  // Creating the ADCL Vector
  err = ADCL_Vmap_all_allocate( &svmap ); 
  if ( ADCL_SUCCESS != err) goto exit;   
  // err = ADCL_Vmap_all_allocate( &rvmap ); 
  // if ( ADCL_SUCCESS != err) goto exit;   

  err = ADCL_Vector_allocate_generic ( dim_cnt, &dim, 0, svmap, MPI_BYTE, &sdata, &svec );
  if ( ADCL_SUCCESS != err) goto exit;   
  // err = ADCL_Vector_allocate_generic ( dim_cnt, &dim, 0, rvmap, MPI_BYTE, &rdata, &rvec );
  // if ( ADCL_SUCCESS != err) goto exit;   
  
  // Creating the ADCL Request
  err = ADCL_Request_create_generic_rooted ( svec, ADCL_VECTOR_NULL, topo, ADCL_FNCTSET_IBCAST, root, &request );
  if ( ADCL_SUCCESS != err) goto exit;

  // define timer object
  ADCL_Timer_create ( 1, &request, &timer );

  maxit=350;
  for ( i = 0; i<maxit ; i++ ) {  

    ADCL_Timer_start( timer );

    // set buffer values
    for(k=0;k<dim;k++){
      if(rank == root){
	sdata[k] = 'b';
      }else{
	sdata[k] = 'a';
      }
    }
    // request start instead of request init + request progress
    // err = ADCL_Request_start( request );

    // request init
    err = ADCL_Request_init( request );
    if ( ADCL_SUCCESS != err) goto exit;


    // do something incredibly important

    // lat_calc_exec ( 1228000 );

    // call progress function
    j=0;
    while(1){
      err = ADCL_Request_progress ( request );
      if ( ADCL_CONTINUE == err) {
	j++;
	total_progress_calls++;
	continue;
      }
      break;
    }
    //if(i==100) printf("Process: %d, n. progress calls: %d\n",rank, j);

    // extra call to wait
    err = ADCL_Request_wait ( request );

    if ( ADCL_SUCCESS != err) goto exit;   

    // check if bcast value is correct
    for(k=0;k<dim;k++){
      if ( sdata[k] != 'b' ) {
	printf("Wrong value, Process: %d, buff: %c\n",rank,sdata[k]);
      }
    }

    ADCL_Timer_stop( timer );

  }

  printf("Process: %d, rate of progress calls: %d\n",rank,total_progress_calls/maxit);
  
  exit:
    if ( ADCL_SUCCESS != err) { printf("ADCL error nr. %d\n", err); } 

    if ( ADCL_TIMER_NULL != timer)     ADCL_Timer_free   ( &timer );
    if ( ADCL_REQUEST_NULL != request) ADCL_Request_free ( &request );
    if ( ADCL_VECTOR_NULL  != svec)    ADCL_Vector_free ( &svec );
    //    if ( ADCL_VECTOR_NULL  != rvec)    ADCL_Vector_free ( &rvec );
    if ( ADCL_VMAP_NULL    != svmap)   ADCL_Vmap_free (&svmap);
    //    if ( ADCL_VMAP_NULL    != rvmap)   ADCL_Vmap_free (&rvmap);
	
    if ( ADCL_TOPOLOGY_NULL != topo)   ADCL_Topology_free ( &topo );
    MPI_Comm_free ( &cart_comm );
    ADCL_Finalize ();
    MPI_Finalize ();

   return 0;
}