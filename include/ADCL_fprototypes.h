/*
 * Copyright (c) 2006-2012      University of Houston. All rights reserved.
 * Copyright (c) 2007           Cisco, Inc. All rights reserved.
 * Copyright (c) 2009           HLRS. All rights reserved.
 * 
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#ifndef __ADCL_FPROTOTYPES_H__
#define __ADCL_FPROTOTYPES_H__

#include "ADCL.h"
#include "ADCL_internal.h"


#define ADCL_FVECTOR_NULL     -1
#define ADCL_FVECTSET_NULL    -2
#define ADCL_FREQUEST_NULL    -3
#define ADCL_FTOPOLOGY_NULL   -4
#define ADCL_FATTRIBUTE_NULL  -5
#define ADCL_FATTRSET_NULL    -6
#define ADCL_FFUNCTION_NULL   -7
#define ADCL_FFNCTSET_NULL    -8
#define ADCL_FNULL_FNCT_PTR   -9
#define ADCL_FVMAP_NULL      -10 
#define ADCL_FTIMER_NULL     -11 

/* ADCL environment functions */
void adcl_init   ( int *ierror );
void adcl_init_  ( int *ierror );
void adcl_init__ ( int *ierror );
void ADCL_INIT   ( int *ierror );

void adcl_finalize   ( int *ierror );
void adcl_finalize_  ( int *ierror );
void adcl_finalize__ ( int *ierror );
void ADCL_FINALIZE   ( int *ierror );


/* ADCL attribute and attribute set functions */
void adcl_attribute_create   ( int* maxnvalues, int *array_of_values,
                               int *attr, int *ierr );
void adcl_attribute_create_  ( int* maxnvalues, int *array_of_values,
                               int *attr, int *ierr );
void adcl_attribute_create__ ( int* maxnvalues, int *array_of_values,
                               int *attr, int *ierr );
void ADCL_ATTRIBUTE_CREATE   ( int* maxnvalues, int *array_of_values,
                               int *attr, int *ierr );

void adcl_attribute_free    ( int *attr, int *ierr );
void adcl_attribute_free_   ( int *attr, int *ierr );
void adcl_attribute_free__  ( int *attr, int *ierr );
void ADCL_ATTRIBUTE_FREE    ( int *attr, int *ierr );

void adcl_attrset_create   ( int* maxnum, int *array_of_attributes,
                             int *attrset, int *ierr );
void adcl_attrset_create_  ( int* maxnum, int *array_of_attributes,
                             int *attrset, int *ierr );
void adcl_attrset_create__ ( int* maxnum, int *array_of_attributes,
                             int *attrset, int *ierr );
void ADCL_ATTRSET_CREATE   ( int* maxnum, int *array_of_attributes,
                             int *attrset, int *ierr );

void adcl_attrset_free   ( int *attrset, int *ierr );
void adcl_attrset_free_  ( int *attrset, int *ierr );
void adcl_attrset_free__ ( int *attrset, int *ierr );
void ADCL_ATTRSET_FREE   ( int *attrset, int *ierr );


/* ADCL Function and functionsets */
void adcl_function_create   ( void *iptr, int *attrset, int *array_of_attrvals, char *name,
                              int *fnct, int *ierr, int *name_len );
void adcl_function_create_  ( void *iptr, int *attrset, int *array_of_attrvals, char *name,
                              int *fnct, int *ierr, int *name_len );
void adcl_function_create__ ( void *iptr, int *attrset, int *array_of_attrvals, char *name,
                              int *fnct, int *ierr, int *name_len );
void ADCL_FUNCTION_CREATE   ( void *iptr, int *attrset, int *array_of_attrvals, char *name,
                              int *fnct, int *ierr, int *name_len );


void adcl_function_create_async   ( void *iptr, void *wptr, int *attrset,
                                    int *array_of_attrvals, char *name,
                                    int *fnct, int *ierr, int *name_len );
void adcl_function_create_async_  ( void *iptr, void *wptr, int *attrset,
                                    int *array_of_attrvals, char *name,
                                    int *fnct, int *ierr, int *name_len );
void adcl_function_create_async__ ( void *iptr, void *wptr, int *attrset,
                                    int *array_of_attrvals, char *name,
                                    int *fnct, int *ierr, int *name_len );
void ADCL_FUNCTION_CREATE_ASYNC   ( void *iptr, void *wptr, int *attrset,
                                    int *array_of_attrvals, char *name,
                                    int *fnct, int *ierr, int *name_len );


void adcl_function_free   ( int *fnct, int *ierr  );
void adcl_function_free_  ( int *fnct, int *ierr  );
void adcl_function_free__ ( int *fnct, int *ierr  );
void ADCL_FUNCTION_FREE   ( int *fnct, int *ierr  );

void adcl_fnctset_create   ( int* maxnum, int *array_of_fncts, char *name,
                             int *fctset, int *ierr, int *name_len  );
void adcl_fnctset_create_  ( int* maxnum, int *array_of_fncts, char *name,
                             int *fctset, int *ierr, int *name_len  );
void adcl_fnctset_create__ ( int* maxnum, int *array_of_fncts, char *name,
                             int *fctset, int *ierr, int *name_len  );
void ADCL_FNCTSET_CREATE   ( int* maxnum, int *array_of_fncts, char *name,
                             int *fctset, int *ierr, int *name_len );

void adcl_fnctset_create_single   ( void *init_fnct, void *wait_fnct,
                                    int *attrset, char *name,
                                    int *without_attr_vals,
                                    int *num_without_attr_vals,
                                    int *fnctset, int *ierr, int *name_len );
void adcl_fnctset_create_single_  ( void *init_fnct, void *wait_fnct,
                                    int *attrset, char *name,
                                    int *without_attr_vals,
                                    int *num_without_attr_vals,
                                    int *fnctset, int *ierr, int *name_len );
void adcl_fnctset_create_single__ ( void *init_fnct, void *wait_fnct,
                                    int *attrset, char *name,
                                    int *without_attr_vals,
                                    int *num_without_attr_vals,
                                    int *fnctset, int *ierr, int *name_len );
void ADCL_FNCTSET_CREATE_SINGLE   ( void *init_fnct, void *wait_fnct,
                                    int *attrset, char *name,
                                    int *without_attr_vals,
                                    int *num_without_attr_vals,
                                    int *fnctset, int *ierr, int *name_len );

void adcl_fnctset_free   ( int *fctset, int *ierr );
void adcl_fnctset_free_  ( int *fctset, int *ierr );
void adcl_fnctset_free__ ( int *fctset, int *ierr );
void ADCL_FNCTSET_FREE   ( int *fctset, int *ierr );

/* ADCL vmap functions */
void adcl_vmap_inplace_allocate   ( int *vmap, int* ierror );
void adcl_vmap_inplace_allocate_  ( int *vmap, int* ierror );
void adcl_vmap_inplace_allocate__ ( int *vmap, int* ierror );
void ADCL_VMAP_INPLACE_ALLOCATE   ( int *vmap, int* ierror );

void adcl_vmap_halo_allocate   ( int* hwidth, int *vmap, int* ierror );
void adcl_vmap_halo_allocate_  ( int* hwidth, int *vmap, int* ierror );
void adcl_vmap_halo_allocate__ ( int* hwidth, int *vmap, int* ierror );
void ADCL_VMAP_HALO_ALLOCATE   ( int* hwidth, int *vmap, int* ierror );

void adcl_vmap_list_allocate   ( int *size, int *rcnts, 
    int *displ, int *vmap, int *ierror );
void adcl_vmap_list_allocate_  ( int *size, int *rcnts, 
        int *displ, int *vmap, int *ierror );
void adcl_vmap_list_allocate__ ( int *size, int *rcnts, 
    int *displ, int *vmap, int *ierror );
void ADCL_VMAP_LIST_ALLOCATE   ( int *size, int *rcnts, 
    int *displ, int *vmap, int *ierror );

void adcl_vmap_allreduce_allocate   ( int *op, int *vmap, int *ierror );
void adcl_vmap_allreduce_allocate_  ( int *op, int *vmap, int *ierror );
void adcl_vmap_allreduce_allocate__ ( int *op, int *vmap, int *ierror );
void ADCL_VMAP_ALLREDUCE_ALLOCATE   ( int *op, int *vmap, int *ierror );

void adcl_vmap_reduce_allocate   ( int *op, int *vmap, int *ierror );
void adcl_vmap_reduce_allocate_  ( int *op, int *vmap, int *ierror );
void adcl_vmap_reduce_allocate__ ( int *op, int *vmap, int *ierror );
void ADCL_VMAP_REDUCE_ALLOCATE   ( int *op, int *vmap, int *ierror );

void adcl_vmap_alltoall_allocate   ( int *scnt, int *rcnt, int *vmap, int *ierror ); 
void adcl_vmap_alltoall_allocate_  ( int *scnt, int *rcnt, int *vmap, int *ierror ); 
void adcl_vmap_alltoall_allocate__ ( int *scnt, int *rcnt, int *vmap, int *ierror ); 
void ADCL_VMAP_ALLTOALL_ALLOCATE   ( int *scnt, int *rcnt, int *vmap, int *ierror ); 

void adcl_vmap_all_allocate   ( int *vmap, int* ierror );
void adcl_vmap_all_allocate_  ( int *vmap, int* ierror );
void adcl_vmap_all_allocate__ ( int *vmap, int* ierror );
void ADCL_VMAP_ALL_ALLOCATE   ( int *vmap, int* ierror );

void adcl_vmap_free   ( int *vmap, int *ierror );
void adcl_vmap_free_  ( int *vmap, int *ierror );
void adcl_vmap_free__ ( int *vmap, int *ierror );
void ADCL_VMAP_FREE   ( int *vmap, int *ierror );


/* ADCL vector functions */

void adcl_vector_register   ( int *ndims, int *dims, int *nc, int *vectype,
                              int *hwidth,int *dat, void *data, int *vec, int *ierror);
void adcl_vector_register_  ( int *ndims, int *dims, int *nc, int *vectype,
                              int *hwidth,int *dat, void *data, int *vec, int *ierror);
void adcl_vector_register__ ( int *ndims, int *dims, int *nc, int *vectype,
                              int *hwidth,int *dat, void *data, int *vec, int *ierror);
void ADCL_VECTOR_REGISTER   ( int *ndims, int *dims, int *nc, int *vectype,
                              int *hwidth,int *dat, void *data, int *vec, int *ierror);

void adcl_vector_register_generic   ( int *ndims, int *dims, int *nc, int *vmap,
                                      int *dat, void *data, int *vec, int* ierror );
void adcl_vector_register_generic_  ( int *ndims, int *dims, int *nc, int *vmap,
                                      int *dat, void *data, int *vec, int* ierror );
void adcl_vector_register_generic__ ( int *ndims, int *dims, int *nc, int *vmap,
                                      int *dat, void *data, int *vec, int* ierror );
void ADCL_VECTOR_REGISTER_GENERIC   ( int *ndims, int *dims, int *nc, int *vmap,
                                      int *dat, void *data, int *vec, int* ierror );

void adcl_vector_deregister   ( int *vec, int *ierror );
void adcl_vector_deregister_  ( int *vec, int *ierror );
void adcl_vector_deregister__ ( int *vec, int *ierror );
void ADCL_VECTOR_DEREGISTER   ( int *vec, int *ierror );

void adcl_vectset_create   ( int *maxnum, int *svecs, int *rvecs,
                             int *vectset, int *ierror );
void adcl_vectset_create_  ( int *maxnum, int *svecs, int *rvecs,
                             int *vectset, int *ierror );
void adcl_vectset_create__ ( int *maxnum, int *svecs, int *rvecs, 
                             int *vectset, int *ierror );
void ADCL_VECTSET_CREATE   ( int *maxnum, int *svecs, int *rvecs, 
                             int *vectset, int *ierror );

void adcl_vectset_free   ( int *vectset, int *ierror );
void adcl_vectset_free_  ( int *vectset, int *ierror );
void adcl_vectset_free__ ( int *vectset, int *ierror );
void ADCL_VECTSET_FREE   ( int *vectset, int *ierror );

/* ADCL Process topology functions */

void adcl_topology_create_generic   ( int *ndims, int *nneigh, int *lneighb, int *rneighb, int* flip,
                                      int *coords, int *dir, int *comm, int *topo, int *ierror );
void adcl_topology_create_generic_  ( int *ndims, int *nneigh, int *lneighb, int *rneighb, int* flip,
                                      int *coords, int *dir, int *comm, int *topo, int *ierror );
void adcl_topology_create_generic__ ( int *ndims, int *nneigh, int *lneighb, int *rneighb, int* flip,
                                      int *coords, int *dir, int *comm, int *topo, int *ierror );
void ADCL_TOPOLOGY_CREATE_GENERIC   ( int *ndims, int *nneigh, int *lneighb, int *rneighb, int* flip,
                                      int *coords, int *dir, int *comm, int *topo, int *ierror );

void adcl_topology_create   ( int* cart_comm, int *topo, int *ierror );
void adcl_topology_create_  ( int* cart_comm, int *topo, int *ierror );
void adcl_topology_create__ ( int* cart_comm, int *topo, int *ierror );
void ADCL_TOPOLOGY_CREATE   ( int* cart_comm, int *topo, int *ierror );

void adcl_topology_create_extended   ( int* cart_comm, int *topo, int *ierror );
void adcl_topology_create_extended_  ( int* cart_comm, int *topo, int *ierror );
void adcl_topology_create_extended__ ( int* cart_comm, int *topo, int *ierror );
void ADCL_TOPOLOGY_CREATE_EXTENDED   ( int* cart_comm, int *topo, int *ierror );

void adcl_topology_get_cart_neighbors   ( int* nneigh, int* lneighbors, int* rneighbors, int* flip, int* cart_comm, int *ierror );
void adcl_topology_get_cart_neighbors_  ( int* nneigh, int* lneighbors, int* rneighbors, int* flip, int* cart_comm, int *ierror );
void adcl_topology_get_cart_neighbors__ ( int* nneigh, int* lneighbors, int* rneighbors, int* flip, int* cart_comm, int *ierror );
void ADCL_TOPOLOGY_GET_CART_NEIGHBORS   ( int* nneigh, int* lneighbors, int* rneighbors, int* flip, int* cart_comm, int *ierror );

void adcl_topology_dump   (int *topo, int *ierror );
void adcl_topology_dump_  (int *topo, int *ierror );
void adcl_topology_dump__ (int *topo, int *ierror );
void ADCL_TOPOLOGY_DUMP   (int *topo, int *ierror );

void adcl_topology_free   (int *topo, int *ierror);
void adcl_topology_free_  (int *topo, int *ierror);
void adcl_topology_free__ (int *topo, int *ierror);
void ADCL_TOPOLOGY_FREE   (int *topo, int *ierror);

/* ADCL Request functions */

void adcl_request_create   ( int *vec, int *topo, int *fnctset, int *req, int *ierror );
void adcl_request_create_  ( int *vec, int *topo, int *fnctset, int *req, int *ierror );
void adcl_request_create__ ( int *vec, int *topo, int *fnctset, int *req, int *ierror );
void ADCL_REQUEST_CREATE   ( int *vec, int *topo, int *fnctset, int *req, int *ierror );


void adcl_request_create_generic   ( int *svec, int *rvec, int *topo, int *fnctset,
                                     int *req, int *ierror );
void adcl_request_create_generic_  ( int *svec, int *rvec, int *topo, int *fnctset,
                                     int *req, int *ierror );
void adcl_request_create_generic__ ( int *svec, int *rvec, int *topo, int *fnctset,
                                     int *req, int *ierror );
void ADCL_REQUEST_CREATE_GENERIC   ( int *svec, int *rvec, int *topo, int *fnctset,
                                     int *req, int *ierror );
/*void adcl_request_create_generic   ( int *vectset, int *topo,
                                     int *fnctset, int *req, int *ierror );
void adcl_request_create_generic_  ( int *vectset, int *topo,
                                     int *fnctset, int *req, int *ierror );
void adcl_request_create_generic__ ( int *vectset, int *topo,
                                     int *fnctset, int *req, int *ierror );
void ADCL_REQUEST_CREATE_GENERIC   ( int *vectset, int *topo,
                                     int *fnctset, int *req, int *ierror );*/

void adcl_request_get_comm   ( int *req, int *comm, int *rank, int *size, int *ierr);
void adcl_request_get_comm_  ( int *req, int *comm, int *rank, int *size, int *ierr);
void adcl_request_get_comm__ ( int *req, int *comm, int *rank, int *size, int *ierr);
void ADCL_REQUEST_GET_COMM   ( int *req, int *comm, int *rank, int *size, int *ierr);

void adcl_request_free   ( int *req, int *ierror );
void adcl_request_free_  ( int *req, int *ierror );
void adcl_request_free__ ( int *req, int *ierror );
void ADCL_REQUEST_FREE   ( int *req, int *ierror );

void adcl_request_start   ( int *req, int *ierror );
void adcl_request_start_  ( int *req, int *ierror );
void adcl_request_start__ ( int *req, int *ierror );
void ADCL_REQUEST_START   ( int *req, int *ierror );

void adcl_request_init   ( int *req, int *ierror );
void adcl_request_init_  ( int *req, int *ierror );
void adcl_request_init__ ( int *req, int *ierror );
void ADCL_REQUEST_INIT   ( int *req, int *ierror );

void adcl_request_wait   ( int *req, int *ierror );
void adcl_request_wait_  ( int *req, int *ierror );
void adcl_request_wait__ ( int *req, int *ierror );
void ADCL_REQUEST_WAIT   ( int *req, int *ierror );

void adcl_request_start_overlap   ( int *req, ADCL_work_fnct_ptr *mid,
                                    ADCL_work_fnct_ptr *end,
                                    ADCL_work_fnct_ptr *total,
                                    int *ierror );
void adcl_request_start_overlap_  ( int *req, ADCL_work_fnct_ptr *mid,
                                    ADCL_work_fnct_ptr *end,
                                    ADCL_work_fnct_ptr *total,
                                    int *ierror );
void adcl_request_start_overlap__ ( int *req, ADCL_work_fnct_ptr *mid,
                                    ADCL_work_fnct_ptr *end,
                                    ADCL_work_fnct_ptr *total,
                                    int *ierror );
void ADCL_REQUEST_START_OVERLAP   ( int *req, ADCL_work_fnct_ptr *mid,
                                    ADCL_work_fnct_ptr *end,
                                    ADCL_work_fnct_ptr *total,
                                    int *ierror );

void adcl_request_update   ( int *req, TIME_TYPE *time, int *ierror );
void adcl_request_update_  ( int *req, TIME_TYPE *time, int *ierror );
void adcl_request_update__ ( int *req, TIME_TYPE *time, int *ierror );
void ADCL_REQUEST_UPDATE   ( int *req, TIME_TYPE *time, int *ierror );

/* ADCL Timer functions */

void adcl_timer_create    ( int* nreq, int *reqs, int* timer, int* ierror );
void adcl_timer_create_   ( int* nreq, int *reqs, int* timer, int* ierror );
void adcl_timer_create__  ( int* nreq, int *reqs, int* timer, int* ierror );
void ADCL_TIMER_CREATE  ( int* nreq, int *reqs, int* timer, int* ierror );

void adcl_timer_free    ( int *timer, int *ierror );
void adcl_timer_free_   ( int *timer, int *ierror );
void adcl_timer_free__  ( int *timer, int *ierror );
void ADCL_TIMER_FREE    ( int *timer, int *ierror );

void adcl_timer_start   ( int *timer, int *ierror );
void adcl_timer_start_  ( int *timer, int *ierror );
void adcl_timer_start__ ( int *timer, int *ierror );
void ADCL_TIMER_START   ( int *timer, int *ierror );

void adcl_timer_stop    ( int *timer, int *ierror );
void adcl_timer_stop_   ( int *timer, int *ierror );
void adcl_timer_stop__  ( int *timer, int *ierror );
void ADCL_TIMER_STOP    ( int *timer, int *ierror );

#ifdef ADCL_LIBNBC

void adcl_ibcast_init   ( void* buffer, int* count, int* datatype, int* root, int* comm, int* req, int* ierror);
void adcl_ibcast_init_  ( void* buffer, int* count, int* datatype, int* root, int* comm, int* req, int* ierror);
void adcl_ibcast_init__ ( void* buffer, int* count, int* datatype, int* root, int* comm, int* req, int* ierror);
void ADCL_IBCAST_INIT   ( void* buffer, int* count, int* datatype, int* root, int* comm, int* req, int* ierror);

void adcl_ialltoall_init   ( void* sbuffer, int* scount, int* sdatatype, void* rbuffer, int* rcount, int* rdatatype, int* comm, int* req, int* ierror);
void adcl_ialltoall_init_  ( void* sbuffer, int* scount, int* sdatatype, void* rbuffer, int* rcount, int* rdatatype, int* comm, int* req, int* ierror);
void adcl_ialltoall_init__ ( void* sbuffer, int* scount, int* sdatatype, void* rbuffer, int* rcount, int* rdatatype, int* comm, int* req, int* ierror);
void ADCL_IALLTOALL_INIT   ( void* sbuffer, int* scount, int* sdatatype, void* rbuffer, int* rcount, int* rdatatype, int* comm, int* req, int* ierror);

#endif

void adcl_reduce_init   ( void *sbuffer, void *rbuffer, int* count, int* datatype, int* op, int* root, int* comm, int* req, int* ierror);
void adcl_reduce_init_  ( void *sbuffer, void *rbuffer, int* count, int* datatype, int* op, int* root, int* comm, int* req, int* ierror);
void adcl_reduce_init__ ( void *sbuffer, void *rbuffer, int* count, int* datatype, int* op, int* root, int* comm, int* req, int* ierror);
void ADCL_REDUCE_INIT   ( void *sbuffer, void *rbuffer, int* count, int* datatype, int* op, int* root, int* comm, int* req, int* ierror);

void adcl_allreduce_init  ( void *sbuffer, void *rbuffer, int* count, int* datatype, int* op, int* comm, int* req, int* ierror);
void adcl_allreduce_init_  ( void *sbuffer, void *rbuffer, int* count, int* datatype, int* op, int* comm, int* req, int* ierror);
void adcl_allreduce_init__ ( void *sbuffer, void *rbuffer, int* count, int* datatype, int* op, int* comm, int* req, int* ierror);
void ADCL_ALLREDUCE_INIT   ( void *sbuffer, void *rbuffer, int* count, int* datatype, int* op, int* comm, int* req, int* ierror);

void adcl_alltoall_init   ( void *sbuffer, int* scount, int* sdatatype, void* rbuffer, int* rcount, int* rdatatype, int* comm, int* req, int* ierror);
void adcl_alltoall_init_  ( void *sbuffer, int* scount, int* sdatatype, void* rbuffer, int* rcount, int* rdatatype, int* comm, int* req, int* ierror);
void adcl_alltoall_init__ ( void *sbuffer, int* scount, int* sdatatype, void* rbuffer, int* rcount, int* rdatatype, int* comm, int* req, int* ierror);
void ADCL_ALLTOALL_INIT   ( void *sbuffer, int* scount, int* sdatatype, void* rbuffer, int* rcount, int* rdatatype, int* comm, int* req, int* ierror);

void adcl_alltoallv_init   ( void *sbuffer, int* scounts, int* sdispls, int* sdatatype, void *rbuffer, int* rcounts, int* rdispls, int* rdatatype, int* comm, int* req, int* ierror);
void adcl_alltoallv_init_  ( void *sbuffer, int* scounts, int* sdispls, int* sdatatype, void *rbuffer, int* rcounts, int* rdispls, int* rdatatype, int* comm, int* req, int* ierror);
void adcl_alltoallv_init__ ( void *sbuffer, int* scounts, int* sdispls, int* sdatatype, void *rbuffer, int* rcounts, int* rdispls, int* rdatatype, int* comm, int* req, int* ierror);
void ADCL_ALLTOALLV_INIT   ( void *sbuffer, int* scounts, int* sdispls, int* sdatatype, void *rbuffer, int* rcounts, int* rdispls, int* rdatatype, int* comm, int* req, int* ierror);

void adcl_allgatherv_init   ( void *sbuffer, int* scount, int* sdatatype, void *rbuffer, int* rcounts, int *displs, int* rdatatype, int* comm, int* req, int* ierror);
void adcl_allgatherv_init_  ( void *sbuffer, int* scount, int* sdatatype, void *rbuffer, int* rcounts, int *displs, int* rdatatype, int* comm, int* req, int* ierror);
void adcl_allgatherv_init__ ( void *sbuffer, int* scount, int* sdatatype, void *rbuffer, int* rcounts, int *displs, int* rdatatype, int* comm, int* req, int* ierror);
void ADCL_ALLGATHERV_INIT   ( void *sbuffer, int* scount, int* sdatatype, void *rbuffer, int* rcounts, int *displs, int* rdatatype, int* comm, int* req, int* ierror);


#endif /*  __ADCL_FPROTOTYPES_H__ */
