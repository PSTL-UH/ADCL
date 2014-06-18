/*
 * Copyright (c) 2006-2007      University of Houston. All rights reserved.
 * Copyright (c) 2007           Cisco, Inc. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include "ADCL.h"
#include "ADCL_internal.h"
#include "ADCL_fprototypes.h"

#ifdef ADCL_LIBNBC

#ifndef _SX
#pragma weak adcl_ibcast_init_  = adcl_ibcast_init
#pragma weak adcl_ibcast_init__ = adcl_ibcast_init
#pragma weak ADCL_IBCAST_INIT   = adcl_ibcast_init

#pragma weak adcl_ialltoall_init_  = adcl_ialltoall_init
#pragma weak adcl_ialltoall_init__ = adcl_ialltoall_init
#pragma weak ADCL_IALLTOALL_INIT   = adcl_ialltoall_init

#pragma weak adcl_iallgather_init_  = adcl_iallgather_init
#pragma weak adcl_iallgather_init__ = adcl_iallgather_init
#pragma weak ADCL_IALLGATHER_INIT   = adcl_iallgather_init
#endif

#ifdef _SX
void adcl_ibcast_init_ ( void* buffer, int* count, int* datatype, int* root, int* comm, int* req, int* ierror)
#else
void adcl_ibcast_init  ( void* buffer, int* count, int* datatype, int* root, int* comm, int* req, int* ierror)
#endif
{

  MPI_Datatype cdatatype;
  MPI_Comm ccomm;
  ADCL_Request *creq;

  if ( ( NULL == buffer )     ||
       ( NULL == count )    ||
       ( NULL == datatype ) ||
       ( NULL == root ) ||
       ( NULL == comm ) ||
       ( NULL == req ) ){
    *ierror = ADCL_INVALID_ARG;
    return;
  }

  ccomm = MPI_Comm_f2c (*comm);
  if ( ccomm == MPI_COMM_NULL ) {
    *ierror = ADCL_INVALID_COMM;
    return;
  }

  int size;
  MPI_Comm_size (ccomm, &size);

  if ( *count == 0 ) {
    *ierror = ADCL_SUCCESS;
    return;
  }

  if ( *root < 0 || *root >= size ) {
    *ierror = ADCL_INVALID_ARG;
    return;
  }

  cdatatype = MPI_Type_f2c (*datatype);
  if ( cdatatype == MPI_DATATYPE_NULL ) {
    *ierror = ADCL_INVALID_TYPE;
    return;
  }

  creq = (ADCL_Request *) calloc ( 1, sizeof (ADCL_Request) );

  *ierror = ADCL_Ibcast_init ( buffer, *count, cdatatype, *root, ccomm, creq);

  *req = (*creq)->r_findex;

  return ;
}

#ifdef _SX
void adcl_ialltoall_init_ ( void* sbuffer, int* scount, int* sdatatype, void* rbuffer, int* rcount, int* rdatatype, int* comm, int* req, int* ierror)
#else
void adcl_ialltoall_init  ( void* sbuffer, int* scount, int* sdatatype, void* rbuffer, int* rcount, int* rdatatype, int* comm, int* req, int* ierror)
#endif
{

  MPI_Datatype csdatatype, crdatatype;
  MPI_Comm ccomm;
  ADCL_Request *creq;

  if ( ( NULL == sbuffer )     ||
       ( NULL == scount )    ||
       ( NULL == sdatatype ) ||
       ( NULL == rbuffer )     ||
       ( NULL == rcount )    ||
       ( NULL == rdatatype ) ||
       ( NULL == comm ) ||
       ( NULL == req ) ){
    *ierror = ADCL_INVALID_ARG;
    return;
  }

  if ( *scount == 0 ) {
    *ierror = ADCL_SUCCESS;
    return;
  }

  csdatatype = MPI_Type_f2c (*sdatatype);
  if ( csdatatype == MPI_DATATYPE_NULL ) {
    *ierror = ADCL_INVALID_TYPE;
    return;
  }

  crdatatype = MPI_Type_f2c (*rdatatype);
  if ( crdatatype == MPI_DATATYPE_NULL ) {
    *ierror = ADCL_INVALID_TYPE;
    return;
  }

  ccomm = MPI_Comm_f2c (*comm);
  if ( ccomm == MPI_COMM_NULL ) {
    *ierror = ADCL_INVALID_COMM;
    return;
  }

  creq = (ADCL_Request *) calloc ( 1, sizeof (ADCL_Request) );

  *ierror = ADCL_Ialltoall_init ( sbuffer, *scount, csdatatype, rbuffer, *rcount, crdatatype, ccomm, creq);

  *req = (*creq)->r_findex;

  return ;
}

#ifdef _SX
void adcl_iallgather_init_ ( void* sbuffer, int* scount, int* sdatatype, void* rbuffer, int* rcount, int* rdatatype, int* comm, int* req, int* ierror)
#else
void adcl_iallgather_init  ( void* sbuffer, int* scount, int* sdatatype, void* rbuffer, int* rcount, int* rdatatype, int* comm, int* req, int* ierror)
#endif
{

  MPI_Datatype csdatatype, crdatatype;
  MPI_Comm ccomm;
  ADCL_Request *creq;

  if ( ( NULL == sbuffer )     ||
       ( NULL == scount )    ||
       ( NULL == sdatatype ) ||
       ( NULL == rbuffer )     ||
       ( NULL == rcount )    ||
       ( NULL == rdatatype ) ||
       ( NULL == comm ) ||
       ( NULL == req ) ){
    *ierror = ADCL_INVALID_ARG;
    return;
  }

  if ( *scount == 0 ) {
    *ierror = ADCL_SUCCESS;
    return;
  }

  csdatatype = MPI_Type_f2c (*sdatatype);
  if ( csdatatype == MPI_DATATYPE_NULL ) {
    *ierror = ADCL_INVALID_TYPE;
    return;
  }

  crdatatype = MPI_Type_f2c (*rdatatype);
  if ( crdatatype == MPI_DATATYPE_NULL ) {
    *ierror = ADCL_INVALID_TYPE;
    return;
  }

  ccomm = MPI_Comm_f2c (*comm);
  if ( ccomm == MPI_COMM_NULL ) {
    *ierror = ADCL_INVALID_COMM;
    return;
  }

  creq = (ADCL_Request *) calloc ( 1, sizeof (ADCL_Request) );

  *ierror = ADCL_Iallgather_init ( sbuffer, *scount, csdatatype, rbuffer, *rcount, crdatatype, ccomm, creq);

  *req = (*creq)->r_findex;

  return ;
}
#endif

#ifndef _SX
#pragma weak adcl_reduce_init_  = adcl_reduce_init
#pragma weak adcl_reduce_init__ = adcl_reduce_init
#pragma weak ADCL_REDUCE_INIT   = adcl_reduce_init

#pragma weak adcl_allreduce_init_  = adcl_allreduce_init
#pragma weak adcl_allreduce_init__ = adcl_allreduce_init
#pragma weak ADCL_ALLREDUCE_INIT   = adcl_allreduce_init

#pragma weak adcl_alltoall_init_  = adcl_alltoall_init
#pragma weak adcl_alltoall_init__ = adcl_alltoall_init
#pragma weak ADCL_ALLTOALL_INIT   = adcl_alltoall_init

#pragma weak adcl_alltoallv_init_  = adcl_alltoallv_init
#pragma weak adcl_alltoallv_init__ = adcl_alltoallv_init
#pragma weak ADCL_ALLTOALLv_INIT   = adcl_alltoallv_init

#pragma weak adcl_allgatherv_init_  = adcl_allgatherv_init
#pragma weak adcl_allgatherv_init__ = adcl_allgatherv_init
#pragma weak ADCL_ALLGATHERV_INIT   = adcl_allgatherv_init
#endif

#ifdef _SX
void adcl_reduce_init_ ( void *sbuffer, void *rbuffer, int* count, int* datatype, int* op, int* root, int* comm, int* req, int* ierror)
#else
void adcl_reduce_init  ( void *sbuffer, void *rbuffer, int* count, int* datatype, int* op, int* root, int* comm, int* req, int* ierror)
#endif
{

  MPI_Datatype cdatatype;
  MPI_Comm ccomm;
  MPI_Op cop;
  ADCL_Request *creq;

  if ( ( NULL == sbuffer )     ||
       ( NULL == rbuffer )     ||
       ( NULL == count )    ||
       ( NULL == datatype ) ||
       ( NULL == op )     ||
       ( NULL == root ) ||
       ( NULL == comm ) ||
       ( NULL == req ) ){
    *ierror = ADCL_INVALID_ARG;
    return;
  }

  ccomm = MPI_Comm_f2c (*comm);
  if ( ccomm == MPI_COMM_NULL ) {
    *ierror = ADCL_INVALID_COMM;
    return;
  }

  int size;
  MPI_Comm_size (ccomm, &size);

  if ( *count == 0 ) {
    *ierror = ADCL_SUCCESS;
    return;
  }

  if ( *root < 0 || *root >= size ) {
    *ierror = ADCL_INVALID_ARG;
    return;
  }

  cdatatype = MPI_Type_f2c (*datatype);
  if ( cdatatype == MPI_DATATYPE_NULL ) {
    *ierror = ADCL_INVALID_TYPE;
    return;
  }

  cop = MPI_Op_f2c (*op);
  if ( cop == MPI_OP_NULL ) {
    *ierror = ADCL_INVALID_OP;
    return;
  }

  creq = (ADCL_Request *) calloc ( 1, sizeof (ADCL_Request) );

  *ierror = ADCL_Reduce_init ( sbuffer, rbuffer, *count, cdatatype, cop, *root, ccomm, creq);

  *req = (*creq)->r_findex;

  return ;
}

#ifdef _SX
void adcl_allreduce_init_ ( void *sbuffer, void *rbuffer, int* count, int* datatype, int* op, int* comm, int* req, int* ierror)
#else
void adcl_allreduce_init  ( void *sbuffer, void *rbuffer, int* count, int* datatype, int* op, int* comm, int* req, int* ierror)
#endif
{

  MPI_Datatype cdatatype;
  MPI_Comm ccomm;
  MPI_Op cop;
  ADCL_Request *creq;

  if ( ( NULL == sbuffer )     ||
       ( NULL == rbuffer )     ||
       ( NULL == count )    ||
       ( NULL == datatype ) ||
       ( NULL == op )     ||
       ( NULL == comm ) ||
       ( NULL == req ) ){
    *ierror = ADCL_INVALID_ARG;
    return;
  }

  if ( *count == 0 ) {
    *ierror = ADCL_SUCCESS;
    return;
  }

  cdatatype = MPI_Type_f2c (*datatype);
  if ( cdatatype == MPI_DATATYPE_NULL ) {
    *ierror = ADCL_INVALID_TYPE;
    return;
  }

  cop = MPI_Op_f2c (*op);
  if ( cop == MPI_OP_NULL ) {
    *ierror = ADCL_INVALID_OP;
    return;
  }

  ccomm = MPI_Comm_f2c (*comm);
  if ( ccomm == MPI_COMM_NULL ) {
    *ierror = ADCL_INVALID_COMM;
    return;
  }

  creq = (ADCL_Request *) calloc ( 1, sizeof (ADCL_Request) );

  *ierror = ADCL_Allreduce_init ( sbuffer, rbuffer, *count, cdatatype, cop, ccomm, creq);

  *req = (*creq)->r_findex;

  return ;
}


#ifdef _SX
void adcl_alltoall_init_ ( void *sbuffer, int* scount, int* sdatatype, void* rbuffer, int* rcount, int* rdatatype, int* comm, int* req, int* ierror)
#else
void adcl_alltoall_init  ( void *sbuffer, int* scount, int* sdatatype, void* rbuffer, int* rcount, int* rdatatype, int* comm, int* req, int* ierror)
#endif
{

  MPI_Datatype csdatatype, crdatatype;
  MPI_Comm ccomm;
  ADCL_Request *creq;

  if ( ( NULL == sbuffer )     ||
       ( NULL == scount )    ||
       ( NULL == sdatatype ) ||
       ( NULL == rbuffer )     ||
       ( NULL == rcount )    ||
       ( NULL == rdatatype ) ||
       ( NULL == comm ) ||
       ( NULL == req ) ){
    *ierror = ADCL_INVALID_ARG;
    return;
  }

  if ( *scount == 0 ) {
    *ierror = ADCL_SUCCESS;
    return;
  }

  csdatatype = MPI_Type_f2c (*sdatatype);
  if ( csdatatype == MPI_DATATYPE_NULL ) {
    *ierror = ADCL_INVALID_TYPE;
    return;
  }

  crdatatype = MPI_Type_f2c (*rdatatype);
  if ( crdatatype == MPI_DATATYPE_NULL ) {
    *ierror = ADCL_INVALID_TYPE;
    return;
  }

  ccomm = MPI_Comm_f2c (*comm);
  if ( ccomm == MPI_COMM_NULL ) {
    *ierror = ADCL_INVALID_COMM;
    return;
  }

  creq = (ADCL_Request *) calloc ( 1, sizeof (ADCL_Request) );

  *ierror = ADCL_Alltoall_init ( sbuffer, *scount, csdatatype, rbuffer, *rcount, crdatatype, ccomm, creq);

  *req = (*creq)->r_findex;

  return ;
}

#ifdef _SX
void adcl_alltoallv_init_ ( void *sbuffer, int* scounts, int* sdispls, int* sdatatype, void *rbuffer, int* rcounts, int* rdispls, int* rdatatype, int* comm, int* req, int* ierror)
#else
void adcl_alltoallv_init  ( void *sbuffer, int* scounts, int* sdispls, int* sdatatype, void *rbuffer, int* rcounts, int* rdispls, int* rdatatype, int* comm, int* req, int* ierror)
#endif
{

  MPI_Datatype csdatatype, crdatatype;
  MPI_Comm ccomm;
  ADCL_Request *creq;

  if ( ( NULL == sbuffer )     ||
       ( NULL == scounts )    ||
       ( NULL == sdispls )    ||
       ( NULL == sdatatype ) ||
       ( NULL == rbuffer )     ||
       ( NULL == rcounts )    ||
       ( NULL == rdispls )    ||
       ( NULL == rdatatype ) ||
       ( NULL == comm ) ||
       ( NULL == req ) ){
    *ierror = ADCL_INVALID_ARG;
    return;
  }

  ccomm = MPI_Comm_f2c (*comm);
  if ( ccomm == MPI_COMM_NULL ) {
    *ierror = ADCL_INVALID_COMM;
    return;
  }

  int size, scount=0, i;
  MPI_Comm_size (ccomm, &size);

  // Calculating the total number of sent elements
  for(i = 0; i < size; i++){
    scount += scounts[i];
  }

  if ( scount == 0 ) {
    *ierror = ADCL_SUCCESS;
    return;
  }

  csdatatype = MPI_Type_f2c (*sdatatype);
  if ( csdatatype == MPI_DATATYPE_NULL ) {
    *ierror = ADCL_INVALID_TYPE;
    return;
  }

  crdatatype = MPI_Type_f2c (*rdatatype);
  if ( crdatatype == MPI_DATATYPE_NULL ) {
    *ierror = ADCL_INVALID_TYPE;
    return;
  }


  creq = (ADCL_Request *) calloc ( 1, sizeof (ADCL_Request) );

  *ierror = ADCL_Alltoallv_init ( sbuffer, scounts, sdispls, csdatatype, rbuffer, rcounts, rdispls, crdatatype, ccomm, creq);

  *req = (*creq)->r_findex;

  return ;
}

#ifdef _SX
void adcl_allgatherv_init_ ( void *sbuffer, int* scount, int* sdatatype, void *rbuffer, int* rcounts, int *displs, int* rdatatype, int* comm, int* req, int* ierror)
#else
void adcl_allgatherv_init  ( void *sbuffer, int* scount, int* sdatatype, void *rbuffer, int* rcounts, int *displs, int* rdatatype, int* comm, int* req, int* ierror)
#endif
{

  MPI_Datatype csdatatype, crdatatype;
  MPI_Comm ccomm;
  ADCL_Request *creq;

  if ( ( NULL == sbuffer )     ||
       ( NULL == scount )    ||
       ( NULL == sdatatype ) ||
       ( NULL == rbuffer )     ||
       ( NULL == rcounts )    ||
       ( NULL == displs )    ||
       ( NULL == rdatatype ) ||
       ( NULL == comm ) ||
       ( NULL == req ) ){
    *ierror = ADCL_INVALID_ARG;
    return;
  }

  if ( *scount == 0 ) {
    *ierror = ADCL_SUCCESS;
    return;
  }

  csdatatype = MPI_Type_f2c (*sdatatype);
  if ( csdatatype == MPI_DATATYPE_NULL ) {
    *ierror = ADCL_INVALID_TYPE;
    return;
  }

  crdatatype = MPI_Type_f2c (*rdatatype);
  if ( crdatatype == MPI_DATATYPE_NULL ) {
    *ierror = ADCL_INVALID_TYPE;
    return;
  }

  ccomm = MPI_Comm_f2c (*comm);
  if ( ccomm == MPI_COMM_NULL ) {
    *ierror = ADCL_INVALID_COMM;
    return;
  }

  creq = (ADCL_Request *) calloc ( 1, sizeof (ADCL_Request) );

  *ierror = ADCL_Allgatherv_init ( sbuffer, *scount, csdatatype, rbuffer, rcounts, displs, crdatatype, ccomm, creq);

  *req = (*creq)->r_findex;

  return ;
}
