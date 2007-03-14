#include "ADCL.h"
#include "ADCL_internal.h"
#include "ADCL_fprototypes.h"


#pragma weak adcl_request_create_  = adcl_request_create
#pragma weak adcl_request_create__ = adcl_request_create
#pragma weak ADCL_REQUEST_CREATE   = adcl_request_create

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
    cvec = (ADCL_vector_t *) ADCL_array_get_ptr_by_pos ( ADCL_vector_farray, 
							 *vec );

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

void adcl_request_free ( int *req, int *ierror )
{
    ADCL_request_t *creq;

    creq = (ADCL_request_t *) ADCL_array_get_ptr_by_pos (ADCL_request_farray, 
							 *req );
    *ierror = ADCL_Request_free ( &creq );
    return;
}

void adcl_request_start ( int *req, int *ierror )
{
    ADCL_request_t *creq;

    creq = (ADCL_request_t *) ADCL_array_get_ptr_by_pos (ADCL_request_farray, 
							 *req );
    *ierror = ADCL_Request_start ( creq );
    return;
}

void adcl_request_init ( int *req, int *ierror )
{
    ADCL_request_t *creq;

    creq = (ADCL_request_t *) ADCL_array_get_ptr_by_pos (ADCL_request_farray, 
							 *req );
    *ierror = ADCL_Request_init ( creq );
    return;
}

void adcl_request_wait ( int *req, int *ierror )
{
    ADCL_request_t *creq;

    creq = (ADCL_request_t *) ADCL_array_get_ptr_by_pos (ADCL_request_farray, 
							 *req );
    *ierror = ADCL_Request_wait ( creq );
    return;
}

void adcl_request_start_overlap ( int *req, ADCL_work_fnct_ptr *mid,
				  ADCL_work_fnct_ptr *end, 
				  ADCL_work_fnct_ptr *total, 
				  void *arg1, void* arg2, void* arg3, 
				  int *ierror )
{
    ADCL_request_t *creq;

    creq = (ADCL_request_t *) ADCL_array_get_ptr_by_pos (ADCL_request_farray, 
							 *req );

    *ierror = ADCL_Request_start_overlap ( creq, mid, end, total, 
					   arg1, arg2, arg3 );

    return;
}
