#include "ADCL_internal.h"

#define AAO_SB_TAG 11111


/* Neighborhood communication initiating all operations at-once */
int ADCL_CHANGE_SB_AAO ( ADCL_request_t *req )
{
    int i, nneighs=req->r_nneighbors;

    PREPARE_COMMUNICATION(req);
    
    for ( i=0; i<nneighs; i++ ) {
	if ( MPI_PROC_NULL != req->r_neighbors[i] ) {
	    RECV_START(req, i, AAO_SB_TAG);
	    SEND_START(req, i, AAO_SB_TAG);
	}
	else {
	    req->r_sreqs[i] = MPI_REQUEST_NULL;
	    req->r_rreqs[i] = MPI_REQUEST_NULL;
	}
    }

    SEND_WAITALL(req);
    RECV_WAITALL(req);

    STOP_COMMUNICATION(req);
        
    return ADCL_SUCCESS;
}

