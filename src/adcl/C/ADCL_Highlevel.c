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

  if ( count == 0 ) {
      return ADCL_SUCCESS;
  }

  if ( NULL == buffer ) {
      return ADCL_INVALID_ARG;
  }

  MPI_Comm_size (comm, &size);

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
  (*req)->r_Highlevel.svec = vec;

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

  if ( scount == 0 && rcount == 0 ) {
      return ADCL_SUCCESS;
  }

  if ( NULL == sbuffer || NULL == rbuffer ) {
      return ADCL_INVALID_ARG;
  }

  MPI_Comm_size (comm, &size);

  ret = ADCL_Topology_create ( comm, topo );
  if ( ADCL_SUCCESS != ret) return ret;

  ret = ADCL_Vmap_alltoall_allocate( scount, scount, svmap );
  if ( ADCL_SUCCESS != ret) return ret;

  ret = ADCL_Vmap_alltoall_allocate( rcount, rcount, rvmap );
  if ( ADCL_SUCCESS != ret) return ret;

  scount = scount*size;
  rcount = rcount*size;

  ret = ADCL_Vector_register_generic ( 1, &scount, 0, *svmap, sdatatype, sbuffer, svec );
  if ( ADCL_SUCCESS != ret) return ret;

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
