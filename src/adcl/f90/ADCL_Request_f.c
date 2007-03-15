#include "ADCL.h"
#include "ADCL_internal.h"
#include "ADCL_fprototypes.h"


#pragma weak adcl_request_create_  = adcl_request_create
#pragma weak adcl_request_create__ = adcl_request_create
#pragma weak ADCL_REQUEST_CREATE   = adcl_request_create

#pragma weak adcl_request_create_fnctset_   = adcl_request_create_fnctset
#pragma weak adcl_request_create_fnctset__  = adcl_request_create_fnctset
#pragma weak ADCL_REQUEST_CREATE_FNCTSET    = adcl_request_create_fnctset

#pragma weak adcl_request_create_generic_   = adcl_request_create_generic
#pragma weak adcl_request_create_generic__  = adcl_request_create_generic
#pragma weak ADCL_REQUEST_CREATE_GENERIC    = adcl_request_create_generic

#pragma weak adcl_request_free_  = adcl_request_free
#pragma weak adcl_request_free__ = adcl_request_free
#pragma weak ADCL_REQUEST_FREE   = adcl_request_free

#pragma weak adcl_request_start_  = adcl_request_start
#pragma weak adcl_request_start__ = adcl_request_start
#pragma weak ADCL_REQUEST_START   = adcl_request_start

#pragma weak adcl_request_init_  = adcl_request_init
#pragma weak adcl_request_init__ = adcl_request_init
#pragma weak ADCL_REQUEST_INIT   = adcl_request_init

#pragma weak adcl_request_wait_  = adcl_request_wait
#pragma weak adcl_request_wait__ = adcl_request_wait
#pragma weak ADCL_REQUEST_WAIT   = adcl_request_wait

#pragma weak adcl_request_start_overlap_  = adcl_request_start_overlap
#pragma weak adcl_request_start_overlap__ = adcl_request_start_overlap
#pragma weak ADCL_REQUEST_START_OVERLAP   = adcl_request_start_overlap

#pragma weak adcl_request_get_comm_  = adcl_request_get_comm
#pragma weak adcl_request_get_comm__ = adcl_request_get_comm
#pragma weak ADCL_REQUEST_GET_COMM   = adcl_request_get_comm


void adcl_request_create ( int *vec, int *topo, int *req, int *ierror ) 
{
    ADCL_vector_t *cvec, **svecs, **rvecs;
    ADCL_request_t *creq;
    ADCL_topology_t *ctopo;
    int i;
    
    if ( ( NULL == vec )   || 
	 ( NULL == topo )  ||
	 ( NULL == req  )  ){
	*ierror = ADCL_INVALID_ARG;
	return;
    }
    
    ctopo = (ADCL_topology_t *) ADCL_array_get_ptr_by_pos ( ADCL_topology_farray,
							    *topo );
    if ( NULL == ctopo ) {
	*ierror = ADCL_INVALID_TOPOLOGY;
	return;
    }

    cvec = (ADCL_vector_t *) ADCL_array_get_ptr_by_pos ( ADCL_vector_farray, 
							 *vec );
    if ( NULL == cvec ) {
	*ierror = ADCL_INVALID_VECTOR;
	return;
    }
    svecs = ( ADCL_vector_t **) malloc ( 4 * ctopo->t_ndims * sizeof(ADCL_vector_t *));
    if ( NULL == svecs ) {
	*ierror = ADCL_NO_MEMORY;
	return;
    }
    rvecs = &svecs[2*ctopo->t_ndims];

    for ( i=0; i<2*ctopo->t_ndims; i++ ) {
	svecs[i] = cvec;
	rvecs[i] = cvec;
    }

    *ierror = ADCL_request_create_generic ( svecs, rvecs, ctopo, &creq, MPI_ORDER_FORTRAN );
    if ( *ierror == ADCL_SUCCESS ) {
	*req = creq->r_findex;
    }
    free ( svecs );

    return;
}

void adcl_request_create_fnctset ( int * topo, int *fnctset, int *req, int *ierror )
{
    ADCL_topology_t *ctopo;
    ADCL_request_t * creq;
    ADCL_fnctset_t *cfnctset;

    if ( ( NULL == fnctset )   || 
	 ( NULL == topo    )  ||
	 ( NULL == req     )  ){
	*ierror = ADCL_INVALID_ARG;
	return;
    }
    
    ctopo = (ADCL_topology_t *) ADCL_array_get_ptr_by_pos ( ADCL_topology_farray,
							    *topo );
    if ( NULL == ctopo ) {
	*ierror = ADCL_INVALID_TOPOLOGY;
	return;
    }
    cfnctset = (ADCL_fnctset_t *) ADCL_array_get_ptr_by_pos ( ADCL_fnctset_farray,
							      *fnctset );
    if ( NULL == cfnctset ) {
	*ierror = ADCL_INVALID_FNCTSET;
	return;
    }

    *ierror = ADCL_request_create_fnctset ( ctopo, cfnctset, &creq );
    if ( ADCL_SUCCESS == *ierror ) {
	*req = creq->r_findex;
    }

    return;
}

void adcl_request_create_generic ( int *array_of_svecs, int *array_of_rvecs, int *topo, 
				   int *req, int *ierror )
{
    return;
}

void adcl_request_get_comm ( int *req, int *comm, int *rank, int *size, int *ierror )
{
    ADCL_request_t *creq;
    MPI_Comm ccomm;

    if ( NULL == req  ||
	 NULL == comm ||
	 NULL == rank ||
	 NULL == size ) {
	*ierror = ADCL_INVALID_ARG;
	return;
    }

    creq = (ADCL_request_t *) ADCL_array_get_ptr_by_pos (ADCL_request_farray, 
							 *req );
    if ( NULL == creq ) {
	*ierror = ADCL_INVALID_REQUEST;
	return;
    }
    *ierror = ADCL_request_get_comm ( creq, &ccomm, rank, size );
    if ( ADCL_SUCCESS == *ierror ) {
	*comm = MPI_Comm_c2f(ccomm);
    }

    return;
}

void adcl_request_free ( int *req, int *ierror )
{
    ADCL_request_t *creq;

    creq = (ADCL_request_t *) ADCL_array_get_ptr_by_pos (ADCL_request_farray, 
							 *req );
    if ( NULL == creq ) {
	*ierror = ADCL_INVALID_REQUEST;
	return;
    }
    *ierror = ADCL_Request_free ( &creq );
    return;
}

void adcl_request_start ( int *req, int *ierror )
{
    ADCL_request_t *creq;

    creq = (ADCL_request_t *) ADCL_array_get_ptr_by_pos (ADCL_request_farray, 
							 *req );
    if ( NULL == creq ) {
	*ierror = ADCL_INVALID_REQUEST;
	return;
    }
    *ierror = ADCL_Request_start ( creq );
    return;
}

void adcl_request_init ( int *req, int *ierror )
{
    ADCL_request_t *creq;

    creq = (ADCL_request_t *) ADCL_array_get_ptr_by_pos (ADCL_request_farray, 
							 *req );
    if ( NULL == creq ) {
	*ierror = ADCL_INVALID_REQUEST;
	return;
    }
    *ierror = ADCL_Request_init ( creq );
    return;
}

void adcl_request_wait ( int *req, int *ierror )
{
    ADCL_request_t *creq;

    creq = (ADCL_request_t *) ADCL_array_get_ptr_by_pos (ADCL_request_farray, 
							 *req );
    if ( NULL == creq ) {
	*ierror = ADCL_INVALID_REQUEST;
	return;
    }
    *ierror = ADCL_Request_wait ( creq );
    return;
}

void adcl_request_start_overlap ( int *req, ADCL_work_fnct_ptr *mid,
				  ADCL_work_fnct_ptr *end, 
				  ADCL_work_fnct_ptr *total, 
				  int *ierror )
{
    ADCL_request_t *creq;

    creq = (ADCL_request_t *) ADCL_array_get_ptr_by_pos (ADCL_request_farray, 
							 *req );

    if ( NULL == creq ) {
	*ierror = ADCL_INVALID_REQUEST;
	return;
    }
    *ierror = ADCL_Request_start_overlap ( creq, mid, end, total );
    return;
}
