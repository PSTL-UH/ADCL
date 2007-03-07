#include "ADCL_internal.h"

#define AAO_SB_TAG 11111


/* Neighborhood communication initiating all operations at-once */
void ADCL_CHANGE_SB_AAO ( ADCL_request_t *req, void *a, void *b, void *c )
{
    int i, nneighs=2*TOPO->t_ndims;

    PREPARE_COMMUNICATION(req);
    
    for ( i=0; i<nneighs; i++ ) {
	if ( MPI_PROC_NULL != TOPO->t_neighbors[i] ) {
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
        
    return;
}

