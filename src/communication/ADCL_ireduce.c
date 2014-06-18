
#include "nbc.h"

#include "mpi.h"
#include "ADCL.h"
#include "ADCL_internal.h"

#include <math.h>

//////////////////////// NBC required definitions ////////////////////////

#ifndef MPI_IN_PLACE
#define MPI_IN_PLACE (void*)1
#endif

static __inline__ int NBC_Copy(void *src, int srccount, MPI_Datatype srctype, void *tgt, int tgtcount, MPI_Datatype tgttype, MPI_Comm comm);
static __inline__ int NBC_Type_intrinsic(MPI_Datatype type);

/* log(2) */
#define LOG2 0.69314718055994530941

/* true/false */
#define true 1
#define false 0

#define NBC_IN_PLACE(sendbuf, recvbuf, inplace) \
  { \
  inplace = 0; \
  if(recvbuf == sendbuf) { \
  inplace = 1; \
  } else \
    if(sendbuf == MPI_IN_PLACE) { \
  sendbuf = recvbuf; \
  inplace = 1; \
    } else \
      if(recvbuf == MPI_IN_PLACE) { \
  recvbuf = sendbuf; \
  inplace = 1; \
      } \
  }

static __inline__ int NBC_Copy(void *src, int srccount, MPI_Datatype srctype, void *tgt, int tgtcount, MPI_Datatype tgttype, MPI_Comm comm) {
  int size, pos, res;
  MPI_Aint ext;
  void *packbuf;

  if((srctype == tgttype) && NBC_Type_intrinsic(srctype)) {
    /* if we have the same types and they are contiguous (intrinsic                                                                                                              \
                                                                                                                                                                                    
     * types are contiguous), we can just use a single memcpy */
    res = MPI_Type_extent(srctype, &ext);
    if (MPI_SUCCESS != res) { printf("MPI Error in MPI_Type_extent() (%i)\n", res); return res; }
    memcpy(tgt, src, srccount*ext);
  } else {
    /* we have to pack and unpack */
    res = MPI_Pack_size(srccount, srctype, comm, &size);
    if (MPI_SUCCESS != res) { printf("MPI Error in MPI_Pack_size() (%i)\n", res); return res; }
    packbuf = malloc(size);
    if (NULL == packbuf) { printf("Error in malloc()\n"); return res; }
    pos=0;
    res = MPI_Pack(src, srccount, srctype, packbuf, size, &pos, comm);
    if (MPI_SUCCESS != res) { printf("MPI Error in MPI_Pack() (%i)\n", res); return res; }
    pos=0;
    res = MPI_Unpack(packbuf, size, &pos, tgt, tgtcount, tgttype, comm);
    if (MPI_SUCCESS != res) { printf("MPI Error in MPI_Unpack() (%i)\n", res); return res; }
    free(packbuf);
  }

  return NBC_OK;
}

static __inline__ int NBC_Type_intrinsic(MPI_Datatype type) {

  if( ( type == MPI_INT ) ||
      ( type == MPI_LONG ) ||
      ( type == MPI_SHORT ) ||
      ( type == MPI_UNSIGNED ) ||
      ( type == MPI_UNSIGNED_SHORT ) ||
      ( type == MPI_UNSIGNED_LONG ) ||
      ( type == MPI_FLOAT ) ||
      ( type == MPI_DOUBLE ) ||
      ( type == MPI_LONG_DOUBLE ) ||
      ( type == MPI_BYTE ) ||
      ( type == MPI_FLOAT_INT) ||
      ( type == MPI_DOUBLE_INT) ||
      ( type == MPI_LONG_INT) ||
      ( type == MPI_2INT) ||
      ( type == MPI_SHORT_INT) ||
      ( type == MPI_LONG_DOUBLE_INT))
    return 1;
  else
    return 0;
}

////////////////////////////////////////////////////////////////////////////////

int ADCL_ireduce(ADCL_request_t *req, int alg);
static __inline__ int red_sched_binomial(int rank, int p, int root, void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, void *redbuf, NBC_Schedule *schedule, NBC_Handle *handle);
static __inline__ int red_sched_chain(int rank, int p, int root, void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int ext, int size, NBC_Schedule *schedule, NBC_Handle *handle, int fragsize);


void ADCL_ireduce_binomial( ADCL_request_t *req )
{
  ADCL_ireduce(req, ADCL_IREDUCE_BINOMIAL);

  /* All done */
  return;
}

void ADCL_ireduce_chain( ADCL_request_t *req )
{
  ADCL_ireduce(req, ADCL_IREDUCE_CHAIN);

  /* All done */
  return;
}

void ADCL_ireduce_wait( ADCL_request_t *req )
{
  NBC_Wait(&(req->r_handle), MPI_STATUS_IGNORE);
}

#ifdef HAVE_SYS_WEAK_ALIAS_PRAGMA
#pragma weak NBC_Ireduce=PNBC_Ireduce
#define NBC_Ireduce PNBC_Ireduce
#endif

int ADCL_ireduce(ADCL_request_t *req, int alg) {

  ADCL_topology_t *topo = req->r_emethod->em_topo;

  int root = req->r_emethod->em_root;
  MPI_Comm comm = topo->t_comm;

  NBC_Handle *handle = &(req->r_handle);
      
  void *sendbuf = req->r_svecs[0]->v_data;
  void *recvbuf = req->r_rvecs[0]->v_data;

  MPI_Datatype datatype = req->r_sdats[0];

  int count = req->r_svecs[0]->v_map->m_rcnt;
  MPI_Op op = req->r_svecs[0]->v_map->m_op;
  
  char *redbuf=NULL, inplace;

  int rank, p, res, segsize, size;
  NBC_Schedule *schedule;
  MPI_Aint ext;

  NBC_IN_PLACE(sendbuf, recvbuf, inplace);

  res = NBC_Init_handle(handle, comm);
  if(res != NBC_OK) { printf("Error in NBC_Init_handle(%i)\n", res); return res; }
  res = MPI_Comm_rank(comm, &rank);
  if (MPI_SUCCESS != res) { printf("MPI Error in MPI_Comm_rank() (%i)\n", res); return res; }
  res = MPI_Comm_size(comm, &p);
  if (MPI_SUCCESS != res) { printf("MPI Error in MPI_Comm_size() (%i)\n", res); return res; }
  res = MPI_Type_extent(datatype, &ext);
  if (MPI_SUCCESS != res) { printf("MPI Error in MPI_Type_extent() (%i)\n", res); return res; }
  res = MPI_Type_size(datatype, &size);
  if (MPI_SUCCESS != res) { printf("MPI Error in MPI_Type_size() (%i)\n", res); return res; }

  /* only one node -> copy data */
  if((p == 1) && !inplace) {
    res = NBC_Copy(sendbuf, count, datatype, recvbuf, count, datatype, comm);
    if (NBC_OK != res) { printf("Error in NBC_Copy() (%i)\n", res); return res; }
  }

#ifdef ADCL_CACHE_SCHEDULE
  /* Check if alg or fanout changed */
  if( req->r_Ireduce_args->schedule == NULL || alg != req->r_Ireduce_args->alg) {
#endif

    schedule = (NBC_Schedule*)malloc(sizeof(NBC_Schedule));
    if (NULL == schedule) { printf("Error in malloc()\n"); return res; }

    res = NBC_Sched_create(schedule);
    if(NBC_OK != res) { printf("Error in NBC_Sched_create, (%i)\n", res); return res; }

    switch(alg) {
    case ADCL_IREDUCE_BINOMIAL:
      if(rank == root) {
	/* root reduces in receivebuffer */
	handle->tmpbuf = malloc(ext*count);
      } else {
	/* recvbuf may not be valid on non-root nodes */
	handle->tmpbuf = malloc(ext*count*2);
	redbuf = ((char*)handle->tmpbuf)+(ext*count);
      }
      if (NULL == handle->tmpbuf) { printf("Error in malloc() (%i)\n", res); return res; }
      res = red_sched_binomial(rank, p, root, sendbuf, recvbuf, count, datatype, op, redbuf, schedule, handle);
      break;
    case ADCL_IREDUCE_CHAIN:
      handle->tmpbuf = malloc(ext*count);
      segsize = 16384/2;
      if (NULL == handle->tmpbuf) { printf("Error in malloc() (%i)\n", res); return res; }
      res = red_sched_chain(rank, p, root, sendbuf, recvbuf, count, datatype, op, ext, size, schedule, handle, segsize);
      break;
    }

    res = NBC_Sched_commit(schedule);
    if (NBC_OK != res) { printf("Error in NBC_Sched_commit() (%i)\n", res); return res; }

#ifdef ADCL_CACHE_SCHEDULE
    /* save schedule to list */
    req->r_Ireduce_args->alg = alg;
    req->r_Ireduce_args->schedule = schedule;
  }else{
    schedule = req->r_Ireduce_args->schedule;
  }
#endif
  
  res = NBC_Start(handle, schedule);
  if (NBC_OK != res) { printf("Error in NBC_Start() (%i)\n", res); return res; }

  return NBC_OK;
}


#define RANK2VRANK(rank, vrank, root) \
  { \
  vrank = rank; \
  if (rank == 0) vrank = root; \
  if (rank == root) vrank = 0; \
  }
#define VRANK2RANK(rank, vrank, root) \
  { \
  rank = vrank; \
  if (vrank == 0) rank = root; \
  if (vrank == root) rank = 0; \
  }

static __inline__ int red_sched_binomial(int rank, int p, int root, void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, void *redbuf, NBC_Schedule *schedule, NBC_Handle *handle) {
  int firstred, vrank, vpeer, peer, res, maxr, r;

  RANK2VRANK(rank, vrank, root);
  maxr = (int)ceil((log(p)/LOG2));
  firstred = 1;
  for(r=1; r<=maxr; r++) {
    if((vrank % (1<<r)) == 0) {
      /* we have to receive this round */
      vpeer = vrank + (1<<(r-1));
      VRANK2RANK(peer, vpeer, root)
	if(peer<p) {
	  res = NBC_Sched_recv(0, true, count, datatype, peer, schedule);
	  if (NBC_OK != res) { free(handle->tmpbuf); printf("Error in NBC_Sched_recv() (%i)\n", res); return res; }
	  /* we have to wait until we have the data */
	  res = NBC_Sched_barrier(schedule);
	  if (NBC_OK != res) { free(handle->tmpbuf); printf("Error in NBC_Sched_barrier() (%i)\n", res); return res; }
	  /* perform the reduce in my local buffer */
	  if(firstred) {
	    if(rank == root) {
	      /* root is the only one who reduces in the receivebuffer                                                                                                                
	       * take data from sendbuf in first round - save copy */
	      res = NBC_Sched_op(recvbuf, false, sendbuf, false, 0, true, count, datatype, op, schedule);
	    } else {
	      /* all others may not have a receive buffer                                                                                                                             
	       * take data from sendbuf in first round - save copy */
	      res = NBC_Sched_op((char *)redbuf-(unsigned long)handle->tmpbuf, true, sendbuf, false, 0, true, count, datatype, op, schedule);
	    }
	    firstred = 0;
	  } else {
	    if(rank == root) {
	      /* root is the only one who reduces in the receivebuffer */
	      res = NBC_Sched_op(recvbuf, false, recvbuf, false, 0, true, count, datatype, op, schedule);
	    } else {
	      /* all others may not have a receive buffer */
	      res = NBC_Sched_op((char *)redbuf-(unsigned long)handle->tmpbuf, true, (char *)redbuf-(unsigned long)handle->tmpbuf, true, 0, true, count, datatype, op, schedule);
	    }
	  }
	  if (NBC_OK != res) { free(handle->tmpbuf); printf("Error in NBC_Sched_op() (%i)\n", res); return res; }
	  /* this cannot be done until handle->tmpbuf is unused :-( */
	  res = NBC_Sched_barrier(schedule);
	  if (NBC_OK != res) { free(handle->tmpbuf); printf("Error in NBC_Sched_barrier() (%i)\n", res); return res; }
	}
    } else {
      /* we have to send this round */
      vpeer = vrank - (1<<(r-1));
      VRANK2RANK(peer, vpeer, root)
	if(firstred) {
	  /* we did not reduce anything */
	  res = NBC_Sched_send(sendbuf, false, count, datatype, peer, schedule);
	} else {
	  /* we have to use the redbuf the root (which works in receivebuf) is never sending .. */
	  res = NBC_Sched_send((char *)redbuf-(unsigned long)handle->tmpbuf, true, count, datatype, peer, schedule);
	}
      if (NBC_OK != res) { free(handle->tmpbuf); printf("Error in NBC_Sched_send() (%i)\n", res); return res; }
      /* leave the game */
      break;
    }
  }

  return NBC_OK;
}

static __inline__ int red_sched_chain(int rank, int p, int root, void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int ext, int size, NBC_Schedule *schedule, NBC_Handle *handle, int fragsize) {
  int res, vrank, rpeer, speer, numfrag, fragnum, fragcount, thiscount;
  long offset;

  RANK2VRANK(rank, vrank, root);
  VRANK2RANK(rpeer, vrank+1, root);
  VRANK2RANK(speer, vrank-1, root);

  if(count == 0) return NBC_OK;

  numfrag = count*size/fragsize;
  if((count*size)%fragsize != 0) numfrag++;
  fragcount = count/numfrag;
  /*printf("numfrag: %i, count: %i, size: %i, fragcount: %i\n", numfrag, count, size, fragcount);*/

  for(fragnum = 0; fragnum < numfrag; fragnum++) {
    offset = fragnum*fragcount*ext;
    thiscount = fragcount;
    if(fragnum == numfrag-1) {
      /* last fragment may not be full */
      thiscount = count-fragcount*fragnum;
    }

    /* last node does not recv */
    if(vrank != p-1) {
      res = NBC_Sched_recv((char*)offset, true, thiscount, datatype, rpeer, schedule);
      if (NBC_OK != res) { printf("Error in NBC_Sched_recv() (%i)\n", res); return res; }
      res = NBC_Sched_barrier(schedule);
      /* root reduces into receivebuf */
      if(vrank == 0) {
        res = NBC_Sched_op((char*)recvbuf+offset, false, (char*)sendbuf+offset, false, (char*)offset, true, thiscount, datatype, op, schedule);
      } else {
        res = NBC_Sched_op((char*)offset, true, (char*)sendbuf+offset, false, (char*)offset, true, thiscount, datatype, op, schedule);
      }
      res = NBC_Sched_barrier(schedule);
    }

    /* root does not send */
    if(vrank != 0) {
      /* rank p-1 has to send out of sendbuffer :) */
      if(vrank == p-1) {
        res = NBC_Sched_send((char*)sendbuf+offset, false, thiscount, datatype, speer, schedule);
      } else {
        res = NBC_Sched_send((char*)offset, true, thiscount, datatype, speer, schedule);
      }
      if (NBC_OK != res) { printf("Error in NBC_Sched_send() (%i)\n", res); return res; }
      /* this barrier here seems awkward but isn't!!!! */
      res = NBC_Sched_barrier(schedule);
    }
  }
  return NBC_OK;
}
