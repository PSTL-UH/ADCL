
#include "mpi.h"
#include "ADCL.h"
#include "ADCL_internal.h"

void ADCL_ibcast_linear( ADCL_request_t *req )
{

   ADCL_topology_t *topo = req->r_emethod->em_topo;

   int root = req->r_emethod->em_root; 
   int rank;
      
   MPI_Comm comm = topo->t_comm;
   
   /* careful, pointers to sbuf are modified! */
   void *sbuf = req->r_svecs[0]->v_data;

   /* use receive vector */
   MPI_Datatype dtype = req->r_rdats[0]; 
   int count = req->r_rvecs[0]->v_dims[0];
   MPI_Comm_rank ( comm, &rank );
   if ( rank == root ) printf("ADCL_ibcast_linear\n");   
   NBC_Ibcast(sbuf, count, dtype, root, comm, &(req->r_handle), NBC_BCAST_LINEAR);

    /* All done */
    return;
}

void ADCL_ibcast_binomial( ADCL_request_t *req )
{

   ADCL_topology_t *topo = req->r_emethod->em_topo;

   int root = req->r_emethod->em_root; 
   int rank;
   MPI_Comm comm = topo->t_comm;
   
   /* careful, pointers to sbuf are modified! */
   void *sbuf = req->r_svecs[0]->v_data;

   /* use receive vector */
   MPI_Datatype dtype = req->r_rdats[0]; 
   int count = req->r_rvecs[0]->v_dims[0];
   
   MPI_Comm_rank ( comm, &rank );
   if ( rank == root ) printf("ADCL_ibcast_binomial\n");   
   NBC_Ibcast(sbuf, count, dtype, root, comm, &(req->r_handle), NBC_BCAST_BINOMIAL);

    /* All done */
    return;
}

void ADCL_ibcast_chain( ADCL_request_t *req )
{

   ADCL_topology_t *topo = req->r_emethod->em_topo;

   int root = req->r_emethod->em_root;
   int rank;
   MPI_Comm comm = topo->t_comm;
   
   /* careful, pointers to sbuf are modified! */
   void *sbuf = req->r_svecs[0]->v_data;

   /* use receive vector */
   MPI_Datatype dtype = req->r_rdats[0]; 
   int count = req->r_rvecs[0]->v_dims[0];
   
   MPI_Comm_rank ( comm, &rank );
   if ( rank == root ) printf("ADCL_ibcast_chain\n");   
   NBC_Ibcast(sbuf, count, dtype, root, comm, &(req->r_handle), NBC_BCAST_CHAIN);

    /* All done */
    return;
}

void ADCL_ibcast_wait( ADCL_request_t *req )
{
  NBC_Wait(&(req->r_handle), MPI_STATUS_IGNORE);
}
