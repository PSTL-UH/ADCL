
#include "nbc.h"

#include "mpi.h"
#include "ADCL.h"
#include "ADCL_internal.h"

#include <math.h>

/* log(2) */
#define LOG2 0.69314718055994530941

/* true/false */
#define true 1
#define false 0

int ADCL_ibcast(ADCL_request_t *req, int alg, int segsize, int fanout);
static int bcast_sched_binomial(int rank, int p, int root, NBC_Schedule *schedule, void *buffer, int count, MPI_Datatype datatype, int fragsize, int size);
static int bcast_sched_generic(int rank, int p, int root, NBC_Schedule *schedule, void *buffer, int count, MPI_Datatype datatype, int fragsize, int size, int fanout);

void ADCL_ibcast_binomial( ADCL_request_t *req )
{
  int segsize = req->r_function->f_attrvals[2]*1024;
  ADCL_ibcast(req, ADCL_IBCAST_BINOMIAL, segsize, 0);

  /* All done */
  return;
}

void ADCL_ibcast_generic( ADCL_request_t *req )
{
  int segsize = req->r_function->f_attrvals[2]*1024;
  int fanout = req->r_function->f_attrvals[1];
  ADCL_ibcast(req, ADCL_IBCAST_GENERIC, segsize, fanout);

  /* All done */
  return;
}

void ADCL_ibcast_wait( ADCL_request_t *req )
{
  NBC_Wait(&(req->r_handle), MPI_STATUS_IGNORE);
}

#ifdef HAVE_SYS_WEAK_ALIAS_PRAGMA
#pragma weak NBC_Ibcast=PNBC_Ibcast
#define NBC_Ibcast PNBC_Ibcast
#endif

int ADCL_ibcast(ADCL_request_t *req, int alg, int segsize, int fanout) {
  ADCL_topology_t *topo = req->r_emethod->em_topo;

  int root = req->r_emethod->em_root;
  MPI_Comm comm = topo->t_comm;

  NBC_Handle *handle = &(req->r_handle);
      
  /* careful, pointers to sbuf are modified! */
  void *buffer = req->r_svecs[0]->v_data;

  /* use receive vector */
  MPI_Datatype datatype = req->r_rdats[0]; 
  int count = req->r_rvecs[0]->v_dims[0];
   
  int rank, p, res, size;
  NBC_Schedule *schedule;

  // enum { NBC_IBCAST_LINEAR, NBC_IBCAST_BINOMIAL, NBC_IBCAST_CHAIN } alg;
  
  res = NBC_Init_handle(handle, comm);
  if(res != NBC_OK) { printf("Error in NBC_Init_handle(%i)\n", res); return res; }
  res = MPI_Comm_rank(comm, &rank);
  if (MPI_SUCCESS != res) { printf("MPI Error in MPI_Comm_rank() (%i)\n", res); return res; }
  res = MPI_Comm_size(comm, &p);
  if (MPI_SUCCESS != res) { printf("MPI Error in MPI_Comm_rank() (%i)\n", res); return res; }
  res = MPI_Type_size(datatype, &size);
  if (MPI_SUCCESS != res) { printf("MPI Error in MPI_Type_size() (%i)\n", res); return res; }
  
  handle->tmpbuf=NULL;

#ifdef ADCL_CACHE_SCHEDULE
  /* Check if alg or fanout changed */
  if( req->r_Ibcast_args.schedule == NULL || alg != req->r_Ibcast_args.alg || segsize != req->r_Ibcast_args.segsize || fanout != req->r_Ibcast_args.fanout) {
#endif
    schedule = (NBC_Schedule*)malloc(sizeof(NBC_Schedule));
    
    res = NBC_Sched_create(schedule);
    if(res != NBC_OK) { printf("Error in NBC_Sched_create, res = %i\n", res); return res; }

    switch(alg) {
    case ADCL_IBCAST_BINOMIAL:
      res = bcast_sched_binomial(rank, p, root, schedule, buffer, count, datatype, segsize, size);
      break;
    case ADCL_IBCAST_GENERIC:
      res = bcast_sched_generic(rank, p, root, schedule, buffer, count, datatype, segsize, size, fanout);
      break;
    }

    if (NBC_OK != res) { printf("Error in Schedule creation() (%i)\n", res); return res; }
    
    res = NBC_Sched_commit(schedule);
    if (NBC_OK != res) { printf("Error in NBC_Sched_commit() (%i)\n", res); return res; }
#ifdef ADCL_CACHE_SCHEDULE
    /* save schedule to list */
    req->r_Ibcast_args.alg = alg;
    req->r_Ibcast_args.segsize = segsize;
    req->r_Ibcast_args.fanout = fanout;
    req->r_Ibcast_args.schedule = schedule;
  }else{
    schedule = req->r_Ibcast_args.schedule;
  }
#endif
  
  res = NBC_Start(handle, schedule);
  if (NBC_OK != res) { printf("Error in NBC_Start() (%i)\n", res); return res; }
  
  return NBC_OK;
}

/* better binomial bcast 
 * working principle:
 * - each node gets a virtual rank vrank
 * - the 'root' node get vrank 0 
 * - node 0 gets the vrank of the 'root'
 * - all other ranks stay identical (they do not matter)
 *
 * Algorithm:
 * - each node with vrank > 2^r and vrank < 2^r+1 receives from node
 *   vrank - 2^r (vrank=1 receives from 0, vrank 0 receives never)
 * - each node sends each round r to node vrank + 2^r
 * - a node stops to send if 2^r > commsize  
 */
#define RANK2VRANK(rank, vrank, root)		\
  {						\
    vrank = rank;				\
    if (rank == 0) vrank = root;		\
    if (rank == root) vrank = 0;		\
  }
#define VRANK2RANK(rank, vrank, root)		\
  {						\
    rank = vrank;				\
    if (vrank == 0) rank = root;		\
    if (vrank == root) rank = 0;		\
  }

static int bcast_sched_binomial(int rank, int p, int root, NBC_Schedule *schedule, void *buffer, int count, MPI_Datatype datatype, int fragsize, int size) {

  int maxr, vrank, rpeer, speer, r, res;
  int numfrag, fragnum, fragcount, thiscount;
  MPI_Aint ext;
  char *buf;

  res = MPI_Type_extent(datatype, &ext);
  if (MPI_SUCCESS != res) { printf("MPI Error in MPI_Type_extent() (%i)\n", res); return res; }

  if(count == 0) return NBC_OK;

  numfrag = count*size/fragsize;
  if((count*size)%fragsize != 0) numfrag++;
  fragcount = count/numfrag;
  /*if(!rank) printf("numfrag: %i, count: %i, size: %i, fragcount: %i\n", numfrag, count, size, fragcount);*/
  
  maxr = (int)ceil((log(p)/LOG2));

  RANK2VRANK(rank, vrank, root);

  for(fragnum = 0; fragnum < numfrag; fragnum++) {
    buf = (char*)buffer+fragnum*fragcount*ext;
    thiscount = fragcount;
    if(fragnum == numfrag-1) {
      /* last fragment may not be full */
      thiscount = count-fragcount*fragnum;
    }

    /* receive from the right hosts  */
    if(vrank != 0) {
      for(r=0; r<maxr; r++) {
	if((vrank >= (1<<r)) && (vrank < (1<<(r+1)))) {
	  VRANK2RANK(rpeer, vrank-(1<<r), root);
	  res = NBC_Sched_recv(buffer, false, thiscount, datatype, rpeer, schedule);
	  if (NBC_OK != res) { printf("Error in NBC_Sched_recv() (%i)\n", res); return res; }
	}
      }
      res = NBC_Sched_barrier(schedule);
      if (NBC_OK != res) { printf("Error in NBC_Sched_barrier() (%i)\n", res); return res; }
    }

    /* now send to the right hosts */
    for(r=0; r<maxr; r++) {
      if(((vrank + (1<<r) < p) && (vrank < (1<<r))) || (vrank == 0)) {
	VRANK2RANK(speer, vrank+(1<<r), root);
	res = NBC_Sched_send(buffer, false, thiscount, datatype, speer, schedule);
	if (NBC_OK != res) { printf("Error in NBC_Sched_send() (%i)\n", res); return res; }
	if(vrank == 0) res = NBC_Sched_barrier(schedule);
      }
    }
  }

  return NBC_OK;
}

static int bcast_sched_generic(int rank, int p, int root, NBC_Schedule *schedule, void *buffer, int count, MPI_Datatype datatype, int fragsize, int size, int fanout) {

  int maxr, vrank, rpeer, speer, r, res, i;
  int numfrag, fragnum, fragcount, thiscount;
  MPI_Aint ext;
  char *buf;

  RANK2VRANK(rank, vrank, root);
  VRANK2RANK(rpeer, (int)(vrank-1)/fanout, root);

  res = MPI_Type_extent(datatype, &ext);
  if (MPI_SUCCESS != res) { printf("MPI Error in MPI_Type_extent() (%i)\n", res); return res; }

  if(count == 0) return NBC_OK;

  numfrag = count*size/fragsize;
  if((count*size)%fragsize != 0) numfrag++;
  fragcount = count/numfrag;
  /*if(!rank) printf("numfrag: %i, count: %i, size: %i, fragcount: %i\n", numfrag, count, size, fragcount);*/

  maxr = (int)ceil((log(p)/LOG2));

  for(fragnum = 0; fragnum < numfrag; fragnum++) {
    buf = (char*)buffer+fragnum*fragcount*ext;
    thiscount = fragcount;
    if(fragnum == numfrag-1) {
      /* last fragment may not be full */
      thiscount = count-fragcount*fragnum;
    }

    /* receive from the right hosts  */
    if(vrank != 0) {
      res = NBC_Sched_recv(buffer, false, thiscount, datatype, rpeer, schedule);
      //printf("process %d receives from rpeer = %d\n",rank, rpeer);                                                                                                                    
      if (NBC_OK != res) { printf("Error in NBC_Sched_recv() (%i)\n", res); return res; }
      res = NBC_Sched_barrier(schedule);
      if (NBC_OK != res) { printf("Error in NBC_Sched_barrier() (%i)\n", res); return res; }
    }

    /* now send to the right hosts */
    for(i=1;i<=fanout;i++){
      VRANK2RANK(speer, (vrank*fanout+i), root);
      if(speer < p) {
	res = NBC_Sched_send(buffer, false, thiscount, datatype, speer, schedule);
	//printf("process %d sends to speer = %d\n",rank, speer);                                                                                                                       
	if (NBC_OK != res) { printf("Error in NBC_Sched_send() (%i)\n", res); return res; }
	if(vrank == 0) res = NBC_Sched_barrier(schedule);
      }
    }
  }

  return NBC_OK;
}
