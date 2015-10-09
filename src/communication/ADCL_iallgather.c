
#include "nbc_internal.h"
#include "mpi.h"
#include "ADCL.h"
#include "ADCL_internal.h"

#include <math.h>

int ADCL_iallgather(ADCL_request_t *req, int alg);
static __inline__ int allgather_sched_linear(int rank, int p, MPI_Aint sndext, MPI_Aint rcvext, NBC_Schedule *schedule, void* sendbuf, int sendcount, MPI_Datatype sendtype, void* recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);

void ADCL_iallgather_linear( ADCL_request_t *req )
{
  ADCL_iallgather(req, ADCL_IALLGATHER_LINEAR);

  /* All done */
  return;
}


void ADCL_iallgather_wait( ADCL_request_t *req )
{
  NBC_Wait(&(req->r_handle), MPI_STATUS_IGNORE);
}

#ifdef HAVE_SYS_WEAK_ALIAS_PRAGMA
#pragma weak NBC_Iallgather=PNBC_Iallgather
#define NBC_Iallgather PNBC_Iallgather
#endif

int ADCL_iallgather(ADCL_request_t *req, int alg) {

  ADCL_topology_t *topo = req->r_emethod->em_topo;

  int root = req->r_emethod->em_root;
  MPI_Comm comm = topo->t_comm;

  NBC_Handle *handle = &(req->r_handle);
      
  void *sendbuf = req->r_svecs[0]->v_data;
  void *recvbuf = req->r_rvecs[0]->v_data;

  MPI_Datatype sendtype = req->r_sdats[0];
  MPI_Datatype recvtype = req->r_rdats[0];

  int sendcount = req->r_svecs[0]->v_map->m_rcnt;
  int recvcount = req->r_rvecs[0]->v_map->m_rcnt;
  
  char *rbuf, inplace;

  int rank, p, res;
  NBC_Schedule *schedule;
  MPI_Aint rcvext, sndext;

  NBC_IN_PLACE(sendbuf, recvbuf, inplace);

  res = NBC_Init_handle(handle, comm);
  if(res != NBC_OK) { printf("Error in NBC_Init_handle(%i)\n", res); return res; }
  res = MPI_Comm_rank(comm, &rank);
  if (MPI_SUCCESS != res) { printf("MPI Error in MPI_Comm_rank() (%i)\n", res); return res; }
  res = MPI_Comm_size(comm, &p);
  if (MPI_SUCCESS != res) { printf("MPI Error in MPI_Comm_size() (%i)\n", res); return res; }
  res = MPI_Type_extent(sendtype, &sndext);
  if (MPI_SUCCESS != res) { printf("MPI Error in MPI_Type_extent() (%i)\n", res); return res; }
  res = MPI_Type_extent(recvtype, &rcvext);
  if (MPI_SUCCESS != res) { printf("MPI Error in MPI_Type_extent() (%i)\n", res); return res; }

  handle->tmpbuf = NULL;

  if(!((rank == 0) && inplace)) {
    /* copy my data to receive buffer */
    rbuf = ((char *)recvbuf) + (rank*recvcount*rcvext);
    res = NBC_Copy(sendbuf, sendcount, sendtype, rbuf, recvcount, recvtype, comm);
    if (NBC_OK != res) { printf("Error in NBC_Copy() (%i)\n", res); return res; }
  }

#ifdef ADCL_CACHE_SCHEDULE
  /* Check if alg or fanout changed */
  if( req->r_Iallgather_args->schedule == NULL || alg != req->r_Iallgather_args->alg) {
#endif

    schedule = (NBC_Schedule*)malloc(sizeof(NBC_Schedule));
    if (NULL == schedule) { printf("Error in malloc()\n"); return res; }

    res = NBC_Sched_create(schedule);
    if(NBC_OK != res) { printf("Error in NBC_Sched_create, (%i)\n", res); return res; }

    switch(alg) {
    case ADCL_IALLGATHER_LINEAR:
      res = allgather_sched_linear(rank, p, sndext, rcvext, schedule, sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
      break;
    }

    if (NBC_OK != res) { printf("Error in Schedule creation() (%i)\n", res); return res; }
    
    res = NBC_Sched_commit(schedule);
    if (NBC_OK != res) { printf("Error in NBC_Sched_commit() (%i)\n", res); return res; }

#ifdef ADCL_CACHE_SCHEDULE
    /* save schedule to list */
    req->r_Iallgather_args->alg = alg;
    req->r_Iallgather_args->schedule = schedule;
  }else{
    schedule = req->r_Iallgather_args->schedule;
  }
#endif
  
  res = NBC_Start(handle, schedule);
  if (NBC_OK != res) { printf("Error in NBC_Start() (%i)\n", res); return res; }

  return NBC_OK;
}


static __inline__ int allgather_sched_linear(int rank, int p, MPI_Aint sndext, MPI_Aint rcvext, NBC_Schedule* schedule, void* sendbuf, int sendcount, MPI_Datatype sendtype, void* recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm) {

  int res, r;
  char *rbuf, *sbuf;

  res = NBC_OK;

  sbuf = ((char *)recvbuf) + (rank*recvcount*rcvext);
  /* do p-1 rounds */
  for(r=0;r<p;r++) {
    if(r != rank) {
      /* recv from rank r */
      rbuf = ((char *)recvbuf) + r*(recvcount*rcvext);
      res = NBC_Sched_recv(rbuf, false, recvcount, recvtype, r, schedule);
      if (NBC_OK != res) { printf("Error in NBC_Sched_recv() (%i)\n", res); return res; }
      /* send to rank r - not from the sendbuf to optimize MPI_IN_PLACE */
      res = NBC_Sched_send(sbuf, false, recvcount, recvtype, r, schedule);
      if (NBC_OK != res) { printf("Error in NBC_Sched_send() (%i)\n", res); return res; }
    }
  }

  return res;
}

