#include "ADCL.h"
#include "ADCL_internal.h"

int ADCL_Request_create ( ADCL_Vector vec, MPI_Comm cart_comm, 
			  ADCL_Request *req )
{
    int topo_type;
    
    /* Right now we can only handle cartesian topologies! */
    MPI_Topo_test ( cart_comm, &topo_type );
    if ( MPI_UNDEFINED==topo_type || MPI_GRAPH==topo_type ) {
	return ADCL_INVALID_COMM;
    }
    
    return ADCL_request_create (vec, cart_comm, req, MPI_ORDER_C );
}

int ADCL_Request_free ( ADCL_Request *req )
{

    if ( NULL == req ) {
	return ADCL_INVALID_REQUEST;
    }

    return ADCL_request_free ( req );
}

int ADCL_Request_start ( ADCL_Request req ) 
{
    int ret=ADCL_SUCCESS;
    int db;

    /* Check validity of the request  */

    ret = ADCL_request_init ( req, &db );
    if ( ADCL_SUCCESS != ret ) {
	return ret;
    }
    
    if ( db ) {
	ret = ADCL_request_wait ( req );
    }
    return ret;
}

int ADCL_Request_init ( ADCL_Request req ) 
{
    int ret=ADCL_SUCCESS;
    int db;

    /* Check validity of the request  */

    ret = ADCL_request_init ( req, &db );
    return ret;
}


int ADCL_Request_wait ( ADCL_Request req ) 
{
    int ret=ADCL_SUCCESS;

    /* Check validity of the request  */

    ret = ADCL_request_wait ( req );
    return ret;
}

int ADCL_Request_start_overlap ( ADCL_Request req, ADCL_work_fctn_ptr* midfctn,
				 ADCL_work_fctn_ptr *endfctn, 
				 ADCL_work_fctn_ptr *totalfctn,
				 void *arg1, void* arg2, void *arg3 )

{
    int ret=ADCL_SUCCESS;
    int db;

    /* Check validity of the request  */

    ret = ADCL_request_init ( req, &db );
    if ( ADCL_SUCCESS != ret ) {
	return ret;
    }
    
    if ( db ) {
	if ( ADCL_NULL_FCTN_PTR != midfctn ) {
	    midfctn ( arg1, arg2, arg3 );
	}
	ret = ADCL_request_wait ( req );
	if ( ADCL_NULL_FCTN_PTR != endfctn ) {
	    endfctn ( arg1, arg2, arg3 );
	}
    }
    else {
	if ( ADCL_NULL_FCTN_PTR != endfctn ) {
	    totalfctn(arg1, arg2, arg3 );
	}
    }
    
    return ret;
}
