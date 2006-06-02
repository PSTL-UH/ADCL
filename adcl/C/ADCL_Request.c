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
