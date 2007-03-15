#include "ADCL.h"
#include "ADCL_internal.h"

int ADCL_Request_create ( ADCL_Vector vec, ADCL_Topology topo,
			  ADCL_Request *req )
{
    int i, ret;
    ADCL_vector_t **svecs, **rvecs;

    if ( vec->v_id < 0 ) {
	return ADCL_INVALID_VECTOR;
    }
    if ( topo->t_id < 0 ) {
	return ADCL_INVALID_TOPOLOGY;
    }
    svecs = ( ADCL_vector_t **) malloc ( 4 * topo->t_ndims * sizeof(ADCL_vector_t *));
    if ( NULL == svecs ) {
	return ADCL_NO_MEMORY;
    }
    rvecs = &svecs[2*topo->t_ndims];

    for ( i=0; i<2*topo->t_ndims; i++ ) {
	svecs[i] = vec;
	rvecs[i] = vec;
    }


    ret = ADCL_request_create_generic (svecs, rvecs, topo, req, MPI_ORDER_C );
    free ( svecs ) ;
    return ret;
}


int ADCL_Request_create_generic ( ADCL_Vector *array_of_send_vectors, 
				  ADCL_Vector *array_of_recv_vectors, 
				  ADCL_Topology topo, ADCL_Request *req )
{
    int i;
    
    if ( topo->t_id < 0 ) {
	return ADCL_INVALID_TOPOLOGY;
    }
    if ( NULL == array_of_send_vectors ||
	 NULL == array_of_recv_vectors ) {
	return ADCL_INVALID_ARG;
    }
    for ( i=0; i< 2*topo->t_ndims; i++ ) {
	if ( array_of_send_vectors[i]->v_id < 0 ||
	     array_of_recv_vectors[i]->v_id < 0 ) {
	    return ADCL_INVALID_VECTOR;
	}
    }

    return ADCL_request_create_generic ( array_of_send_vectors, 
					 array_of_recv_vectors, 
					 topo, req, MPI_ORDER_C );
}

int ADCL_Request_create_fnctset ( ADCL_Topology topo, ADCL_Fnctset fnctset, 
				  ADCL_Request *req )
{
    
    if ( topo->t_id < 0 ) {
	return ADCL_INVALID_TOPOLOGY;
    }
    if ( fnctset->fs_id < 0 ) {
	return ADCL_INVALID_FNCTSET;
    }
    if ( NULL == req ) {
	return ADCL_INVALID_ARG;
    }

    return ADCL_request_create_fnctset (topo, fnctset, req);
}


int ADCL_Request_free ( ADCL_Request *req )
{
    ADCL_request_t *preq = *req;

    if ( NULL == req ) {
	return ADCL_INVALID_REQUEST;
    }
    if ( preq->r_id < 0 ) {
	return ADCL_INVALID_REQUEST;
    }
    if ( preq->r_comm_state != ADCL_COMM_AVAIL ) {
	return ADCL_INVALID_REQUEST;
    }

    return ADCL_request_free ( req );
}

int ADCL_Request_start ( ADCL_Request req ) 
{
    int ret=ADCL_SUCCESS;
    int db;
    TIME_TYPE t1, t2;

    /* Check validity of the request  */

    t1 = TIME;
    ret = ADCL_request_init ( req, &db );
    if ( ADCL_SUCCESS != ret ) {
	return ret;
    }
    
    if ( db ) {
	ret = ADCL_request_wait ( req );
    }
    t2 = TIME;
    ADCL_request_update ( req, t1, t2 );

    return ret;
}

int ADCL_Request_init ( ADCL_Request req ) 
{
    TIME_TYPE t1, t2;
    int ret=ADCL_SUCCESS;
    int db;

    /* Check validity of the request  */
    t1 = TIME;
    ret = ADCL_request_init ( req, &db );
    t2 = TIME;
    req->r_time = t2-t1;

    return ret;
}


int ADCL_Request_wait ( ADCL_Request req ) 
{
    int ret=ADCL_SUCCESS;
    TIME_TYPE t1, t2;

    /* Check validity of the request  */
    t1 = TIME;
    ret = ADCL_request_wait ( req );
    t2 = TIME;
    ADCL_request_update ( req, t1, (t2+req->r_time));

    return ret;
}

int ADCL_Request_start_overlap ( ADCL_Request req, ADCL_work_fnct_ptr* midfctn,
				 ADCL_work_fnct_ptr *endfctn, 
				 ADCL_work_fnct_ptr *totalfctn )

{
    TIME_TYPE t1, t2;
    int ret=ADCL_SUCCESS;
    int db;

    /* Check validity of the request  */

    t1 = TIME;
    ret = ADCL_request_init ( req, &db );
    if ( ADCL_SUCCESS != ret ) {
	return ret;
    }
    
    if ( db ) {
	if ( ADCL_NULL_FNCT_PTR != midfctn ) {
	    midfctn ( req );
	}
	ret = ADCL_request_wait ( req );
	if ( ADCL_NULL_FNCT_PTR != endfctn ) {
	    endfctn ( req );
	}
    }
    else {
	if ( ADCL_NULL_FNCT_PTR != endfctn ) {
	    totalfctn (req );
	}
    }
    t2 = TIME;
    ADCL_request_update ( req, t1, t2 );
    
    return ret;
}

int ADCL_Request_get_comm ( ADCL_Request req, MPI_Comm *comm, int *rank, int *size ) 
{
    if ( req->r_id < 0 ) {
	return ADCL_INVALID_REQUEST;
    }
    if ( NULL == comm || NULL == rank || NULL == size ) {
	return ADCL_INVALID_ARG;
    }

    return ADCL_request_get_comm ( req, comm, rank, size );
}
