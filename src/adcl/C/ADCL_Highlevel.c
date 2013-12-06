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

#ifdef ADCL_LIBNBC

int ADCL_Ibcast_init ( void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm, ADCL_Request* req)
{

  int size, ret;

  ADCL_Topology *topo;
  ADCL_Vmap *vmap;
  ADCL_Vector *vec;

  topo = (ADCL_Topology *) calloc ( 1, sizeof (ADCL_Topology) );
  vmap = (ADCL_Vmap *) calloc ( 1, sizeof (ADCL_Vmap) );
  vec = (ADCL_Vector *) calloc ( 1, sizeof (ADCL_Vector) );

  MPI_Comm_size (comm, &size);

  if ( count == 0 ) {
      return ADCL_SUCCESS;
  }

  if ( NULL == buffer ) {
      return ADCL_INVALID_ARG;
  }

  if ( root < 0 || root >= size ) {
      return ADCL_INVALID_ARG;
  }

  ret = ADCL_Topology_create ( comm, topo );
  if ( ADCL_SUCCESS != ret) return ret;

  ret = ADCL_Vmap_all_allocate( vmap );
  if ( ADCL_SUCCESS != ret) return ret;

  ret = ADCL_Vector_register_generic ( 1, &count, 0, *vmap, datatype, buffer, vec );
  if ( ADCL_SUCCESS != ret) return ret;

  ret = ADCL_Request_create_generic_rooted ( *vec, ADCL_VECTOR_NULL, *topo, ADCL_FNCTSET_IBCAST, root, req );
  if ( ADCL_SUCCESS != ret) return ret;

  (*req)->r_Highlevel.topo = topo;
  (*req)->r_Highlevel.svmap = vmap;
  (*req)->r_Highlevel.rvmap = NULL;
  (*req)->r_Highlevel.svec = vec;
  (*req)->r_Highlevel.rvec = NULL;

  (*req)->r_highlevel = 1;

  return ADCL_SUCCESS;

}

int ADCL_Ialltoall_init ( void *sbuffer, int scount, MPI_Datatype sdatatype, void *rbuffer, int rcount, MPI_Datatype rdatatype, MPI_Comm comm, ADCL_Request* req)
{

  int size, ret;

  ADCL_Topology *topo;
  ADCL_Vmap *svmap,*rvmap;
  ADCL_Vector *svec,*rvec;

  topo = (ADCL_Topology *) calloc ( 1, sizeof (ADCL_Topology) );
  svmap = (ADCL_Vmap *) calloc ( 1, sizeof (ADCL_Vmap) );
  rvmap = (ADCL_Vmap *) calloc ( 1, sizeof (ADCL_Vmap) );
  svec = (ADCL_Vector *) calloc ( 1, sizeof (ADCL_Vector) );
  rvec = (ADCL_Vector *) calloc ( 1, sizeof (ADCL_Vector) );

  MPI_Comm_size (comm, &size);

  if ( scount == 0 ) {
      return ADCL_SUCCESS;
  }

  if ( NULL == sbuffer || NULL == rbuffer ) {
      return ADCL_INVALID_ARG;
  }

  ret = ADCL_Topology_create ( comm, topo );
  if ( ADCL_SUCCESS != ret) return ret;

  if(sbuffer == MPI_IN_PLACE){
    ret = ADCL_Vmap_inplace_allocate( svmap );
    if ( ADCL_SUCCESS != ret) return ret;
  }else{
    ret = ADCL_Vmap_alltoall_allocate( scount, scount, svmap );
    if ( ADCL_SUCCESS != ret) return ret;
  }

  ret = ADCL_Vmap_alltoall_allocate( rcount, rcount, rvmap );
  if ( ADCL_SUCCESS != ret) return ret;

  // Calculating the required buffer sizes
  scount = scount*size;
  rcount = rcount*size;

  if(sbuffer == MPI_IN_PLACE){
    ret = ADCL_Vector_allocate_generic ( 0, NULL, 0, *svmap, MPI_DATATYPE_NULL, NULL, svec );
    if ( ADCL_SUCCESS != ret) return ret;
  }else{
    ret = ADCL_Vector_register_generic ( 1, &scount, 0, *svmap, sdatatype, sbuffer, svec );
    if ( ADCL_SUCCESS != ret) return ret;
  }

  ret = ADCL_Vector_register_generic ( 1, &rcount, 0, *rvmap, rdatatype, rbuffer, rvec );
  if ( ADCL_SUCCESS != ret) return ret;

  ret = ADCL_Request_create_generic ( *svec, *rvec, *topo, ADCL_FNCTSET_IALLTOALL, req );
  if ( ADCL_SUCCESS != ret) return ret;

  (*req)->r_Highlevel.topo = topo;
  (*req)->r_Highlevel.svmap = svmap;
  (*req)->r_Highlevel.svec = svec;
  (*req)->r_Highlevel.rvmap = rvmap;
  (*req)->r_Highlevel.rvec = rvec;

  (*req)->r_highlevel = 1;

  return ADCL_SUCCESS;

}

#endif

int ADCL_Reduce_init ( void *sbuffer, void *rbuffer, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm, ADCL_Request* req)
{

  int size, ret;

  ADCL_Topology *topo;
  ADCL_Vmap *svmap,*rvmap;
  ADCL_Vector *svec,*rvec;

  topo = (ADCL_Topology *) calloc ( 1, sizeof (ADCL_Topology) );
  svmap = (ADCL_Vmap *) calloc ( 1, sizeof (ADCL_Vmap) );
  rvmap = (ADCL_Vmap *) calloc ( 1, sizeof (ADCL_Vmap) );
  svec = (ADCL_Vector *) calloc ( 1, sizeof (ADCL_Vector) );
  rvec = (ADCL_Vector *) calloc ( 1, sizeof (ADCL_Vector) );

  MPI_Comm_size (comm, &size);

  if ( count == 0 ) {
      return ADCL_SUCCESS;
  }

  if ( NULL == sbuffer || NULL == rbuffer ) {
      return ADCL_INVALID_ARG;
  }

  if ( root < 0 || root >= size ) {
      return ADCL_INVALID_ARG;
  }

  ret = ADCL_Topology_create ( comm, topo );
  if ( ADCL_SUCCESS != ret) return ret;

  if(sbuffer == MPI_IN_PLACE){
    ret = ADCL_Vmap_inplace_allocate( svmap );
    if ( ADCL_SUCCESS != ret) return ret;
  }else{
    ret = ADCL_Vmap_reduce_allocate( op, svmap );
    if ( ADCL_SUCCESS != ret) return ret;
  }

  ret = ADCL_Vmap_reduce_allocate( op, rvmap );
  if ( ADCL_SUCCESS != ret) return ret;

  if(sbuffer == MPI_IN_PLACE){
    ret = ADCL_Vector_allocate_generic ( 0, NULL, 0, *svmap, MPI_DATATYPE_NULL, NULL, svec );
    if ( ADCL_SUCCESS != ret) return ret;
  }else{
    ret = ADCL_Vector_register_generic ( 1, &count, 0, *svmap, datatype, sbuffer, svec );
    if ( ADCL_SUCCESS != ret) return ret;
  }

  ret = ADCL_Vector_register_generic ( 1, &count, 0, *rvmap, datatype, rbuffer, rvec );
  if ( ADCL_SUCCESS != ret) return ret;

  ret = ADCL_Request_create_generic_rooted ( *svec, *rvec, *topo, ADCL_FNCTSET_REDUCE, root, req );
  if ( ADCL_SUCCESS != ret) return ret;

  (*req)->r_Highlevel.topo = topo;
  (*req)->r_Highlevel.svmap = svmap;
  (*req)->r_Highlevel.svec = svec;
  (*req)->r_Highlevel.rvmap = rvmap;
  (*req)->r_Highlevel.rvec = rvec;

  (*req)->r_highlevel = 1;

  return ADCL_SUCCESS;

}

int ADCL_Allreduce_init ( void *sbuffer, void *rbuffer, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, ADCL_Request* req)
{

  int ret;

  ADCL_Topology *topo;
  ADCL_Vmap *svmap,*rvmap;
  ADCL_Vector *svec,*rvec;

  topo = (ADCL_Topology *) calloc ( 1, sizeof (ADCL_Topology) );
  svmap = (ADCL_Vmap *) calloc ( 1, sizeof (ADCL_Vmap) );
  rvmap = (ADCL_Vmap *) calloc ( 1, sizeof (ADCL_Vmap) );
  svec = (ADCL_Vector *) calloc ( 1, sizeof (ADCL_Vector) );
  rvec = (ADCL_Vector *) calloc ( 1, sizeof (ADCL_Vector) );

  if ( count == 0 ) {
      return ADCL_SUCCESS;
  }

  if ( NULL == sbuffer || NULL == rbuffer ) {
      return ADCL_INVALID_ARG;
  }

  ret = ADCL_Topology_create ( comm, topo );
  if ( ADCL_SUCCESS != ret) return ret;

  if(sbuffer == MPI_IN_PLACE){
    ret = ADCL_Vmap_inplace_allocate( svmap );
    if ( ADCL_SUCCESS != ret) return ret;
  }else{
    ret = ADCL_Vmap_allreduce_allocate( op, svmap );
    if ( ADCL_SUCCESS != ret) return ret;
  }

    ret = ADCL_Vmap_allreduce_allocate( op, rvmap );
    if ( ADCL_SUCCESS != ret) return ret;
  
  if(sbuffer == MPI_IN_PLACE){
    ret = ADCL_Vector_allocate_generic ( 0, NULL, 0, *svmap, MPI_DATATYPE_NULL, NULL, svec );
    if ( ADCL_SUCCESS != ret) return ret;
  }else{
    ret = ADCL_Vector_register_generic ( 1, &count, 0, *svmap, datatype, sbuffer, svec );
    if ( ADCL_SUCCESS != ret) return ret;
  }

  ret = ADCL_Vector_register_generic ( 1, &count, 0, *rvmap, datatype, rbuffer, rvec );
  if ( ADCL_SUCCESS != ret) return ret;

  ret = ADCL_Request_create_generic ( *svec, *rvec, *topo, ADCL_FNCTSET_ALLREDUCE, req );
  if ( ADCL_SUCCESS != ret) return ret;

  (*req)->r_Highlevel.topo = topo;
  (*req)->r_Highlevel.svmap = svmap;
  (*req)->r_Highlevel.svec = svec;
  (*req)->r_Highlevel.rvmap = rvmap;
  (*req)->r_Highlevel.rvec = rvec;

  (*req)->r_highlevel = 1;

  return ADCL_SUCCESS;

}

int ADCL_Alltoall_init ( void *sbuffer, int scount, MPI_Datatype sdatatype, void *rbuffer, int rcount, MPI_Datatype rdatatype, MPI_Comm comm, ADCL_Request* req)
{

  int size, ret;

  ADCL_Topology *topo;
  ADCL_Vmap *svmap,*rvmap;
  ADCL_Vector *svec,*rvec;

  topo = (ADCL_Topology *) calloc ( 1, sizeof (ADCL_Topology) );
  svmap = (ADCL_Vmap *) calloc ( 1, sizeof (ADCL_Vmap) );
  rvmap = (ADCL_Vmap *) calloc ( 1, sizeof (ADCL_Vmap) );
  svec = (ADCL_Vector *) calloc ( 1, sizeof (ADCL_Vector) );
  rvec = (ADCL_Vector *) calloc ( 1, sizeof (ADCL_Vector) );

  MPI_Comm_size (comm, &size);

  if ( scount == 0 ) {
      return ADCL_SUCCESS;
  }

  if ( NULL == sbuffer || NULL == rbuffer ) {
      return ADCL_INVALID_ARG;
  }

  ret = ADCL_Topology_create ( comm, topo );
  if ( ADCL_SUCCESS != ret) return ret;

  if(sbuffer == MPI_IN_PLACE){
    ret = ADCL_Vmap_inplace_allocate( svmap );
    if ( ADCL_SUCCESS != ret) return ret;
  }else{
    ret = ADCL_Vmap_alltoall_allocate( scount, scount, svmap );
    if ( ADCL_SUCCESS != ret) return ret;
  }

  ret = ADCL_Vmap_alltoall_allocate( rcount, rcount, rvmap );
  if ( ADCL_SUCCESS != ret) return ret;

  // Calculating the required buffer sizes
  scount = scount*size;
  rcount = rcount*size;

  if(sbuffer == MPI_IN_PLACE){
    ret = ADCL_Vector_allocate_generic ( 0, NULL, 0, *svmap, MPI_DATATYPE_NULL, NULL, svec );
    if ( ADCL_SUCCESS != ret) return ret;
  }else{
    ret = ADCL_Vector_register_generic ( 1, &scount, 0, *svmap, sdatatype, sbuffer, svec );
    if ( ADCL_SUCCESS != ret) return ret;
  }

  ret = ADCL_Vector_register_generic ( 1, &rcount, 0, *rvmap, rdatatype, rbuffer, rvec );
  if ( ADCL_SUCCESS != ret) return ret;

  ret = ADCL_Request_create_generic ( *svec, *rvec, *topo, ADCL_FNCTSET_ALLTOALL, req );
  if ( ADCL_SUCCESS != ret) return ret;

  (*req)->r_Highlevel.topo = topo;
  (*req)->r_Highlevel.svmap = svmap;
  (*req)->r_Highlevel.svec = svec;
  (*req)->r_Highlevel.rvmap = rvmap;
  (*req)->r_Highlevel.rvec = rvec;

  (*req)->r_highlevel = 1;

  return ADCL_SUCCESS;

}

int ADCL_Alltoallv_init ( void *sbuffer, int* scounts, int *sdispls, MPI_Datatype sdatatype, void *rbuffer, int* rcounts, int *rdispls, MPI_Datatype rdatatype, MPI_Comm comm, ADCL_Request* req)
{

  int size, ret, i, scount=0, rcount;

  ADCL_Topology *topo;
  ADCL_Vmap *svmap,*rvmap;
  ADCL_Vector *svec,*rvec;

  topo = (ADCL_Topology *) calloc ( 1, sizeof (ADCL_Topology) );
  svmap = (ADCL_Vmap *) calloc ( 1, sizeof (ADCL_Vmap) );
  rvmap = (ADCL_Vmap *) calloc ( 1, sizeof (ADCL_Vmap) );
  svec = (ADCL_Vector *) calloc ( 1, sizeof (ADCL_Vector) );
  rvec = (ADCL_Vector *) calloc ( 1, sizeof (ADCL_Vector) );

  MPI_Comm_size (comm, &size);

  // Calculating the total number of sent elements
  for(i = 0; i < size; i++){
    scount += scounts[i];
  }

  if ( scount == 0 ) {
      return ADCL_SUCCESS;
  }

  if ( NULL == sbuffer || NULL == rbuffer ) {
      return ADCL_INVALID_ARG;
  }
  

  ret = ADCL_Topology_create ( comm, topo );
  if ( ADCL_SUCCESS != ret) return ret;

  if(sbuffer == MPI_IN_PLACE){
    ret = ADCL_Vmap_inplace_allocate( svmap );
    if ( ADCL_SUCCESS != ret) return ret;
  }else{
    ret = ADCL_Vmap_list_allocate( size, scounts, sdispls, svmap );
    if ( ADCL_SUCCESS != ret) return ret;
  }

  ret = ADCL_Vmap_list_allocate( size, rcounts, rdispls, rvmap );
  if ( ADCL_SUCCESS != ret) return ret;

  // Calculating the required buffer sizes
  scount = sdispls[size-1] + scounts[size-1];
  rcount = rdispls[size-1] + rcounts[size-1];

  if(sbuffer == MPI_IN_PLACE){
    ret = ADCL_Vector_allocate_generic ( 0, NULL, 0, *svmap, MPI_DATATYPE_NULL, NULL, svec );
    if ( ADCL_SUCCESS != ret) return ret;
  }else{
    ret = ADCL_Vector_register_generic ( 1, &scount, 0, *svmap, sdatatype, sbuffer, svec );
    if ( ADCL_SUCCESS != ret) return ret;
  }

  ret = ADCL_Vector_register_generic ( 1, &rcount, 0, *rvmap, rdatatype, rbuffer, rvec );
  if ( ADCL_SUCCESS != ret) return ret;

  ret = ADCL_Request_create_generic ( *svec, *rvec, *topo, ADCL_FNCTSET_ALLTOALLV, req );
  if ( ADCL_SUCCESS != ret) return ret;

  (*req)->r_Highlevel.topo = topo;
  (*req)->r_Highlevel.svmap = svmap;
  (*req)->r_Highlevel.svec = svec;
  (*req)->r_Highlevel.rvmap = rvmap;
  (*req)->r_Highlevel.rvec = rvec;

  (*req)->r_highlevel = 1;

  return ADCL_SUCCESS;

}

int ADCL_Allgatherv_init ( void *sbuffer, int scount, MPI_Datatype sdatatype, void *rbuffer, int* rcounts, int *displs, MPI_Datatype rdatatype, MPI_Comm comm, ADCL_Request* req)
{

  int size, ret, rcount;

  ADCL_Topology *topo;
  ADCL_Vmap *svmap,*rvmap;
  ADCL_Vector *svec,*rvec;

  topo = (ADCL_Topology *) calloc ( 1, sizeof (ADCL_Topology) );
  svmap = (ADCL_Vmap *) calloc ( 1, sizeof (ADCL_Vmap) );
  rvmap = (ADCL_Vmap *) calloc ( 1, sizeof (ADCL_Vmap) );
  svec = (ADCL_Vector *) calloc ( 1, sizeof (ADCL_Vector) );
  rvec = (ADCL_Vector *) calloc ( 1, sizeof (ADCL_Vector) );

  MPI_Comm_size (comm, &size);

  if ( scount == 0 ) {
      return ADCL_SUCCESS;
  }

  if ( NULL == sbuffer || NULL == rbuffer ) {
      return ADCL_INVALID_ARG;
  }
  

  ret = ADCL_Topology_create ( comm, topo );
  if ( ADCL_SUCCESS != ret) return ret;

  if(sbuffer == MPI_IN_PLACE){
    ret = ADCL_Vmap_inplace_allocate( svmap );
    if ( ADCL_SUCCESS != ret) return ret;
  }else{
    ret = ADCL_Vmap_all_allocate( svmap );
    if ( ADCL_SUCCESS != ret) return ret;
  }

  ret = ADCL_Vmap_list_allocate( size, rcounts, displs, rvmap );
  if ( ADCL_SUCCESS != ret) return ret;

  // Calculating the required buffer size for receive
  rcount = displs[size-1] + rcounts[size-1];

  if(sbuffer == MPI_IN_PLACE){
    ret = ADCL_Vector_allocate_generic ( 0, NULL, 0, *svmap, MPI_DATATYPE_NULL, NULL, svec );
    if ( ADCL_SUCCESS != ret) return ret;
  }else{
    ret = ADCL_Vector_register_generic ( 1, &scount, 0, *svmap, sdatatype, sbuffer, svec );
    if ( ADCL_SUCCESS != ret) return ret;
  }
  
  ret = ADCL_Vector_register_generic ( 1, &rcount, 0, *rvmap, rdatatype, rbuffer, rvec );
  if ( ADCL_SUCCESS != ret) return ret;

  ret = ADCL_Request_create_generic ( *svec, *rvec, *topo, ADCL_FNCTSET_ALLGATHERV, req );
  if ( ADCL_SUCCESS != ret) return ret;

  (*req)->r_Highlevel.topo = topo;
  (*req)->r_Highlevel.svmap = svmap;
  (*req)->r_Highlevel.svec = svec;
  (*req)->r_Highlevel.rvmap = rvmap;
  (*req)->r_Highlevel.rvec = rvec;

  (*req)->r_highlevel = 1;

  return ADCL_SUCCESS;

}
