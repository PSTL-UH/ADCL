#include "ADCL.h"

#define AAO_SB_TAG 11111

#define SEND_START(req,i,tag) MPI_Isend(req->r_vec->v_data, 1,  \
  req->r_sdats[i], req->r_neighbors[i], tag, req->r_comm, &req->r_sreqs[i])

#define RECV_START(req,i,tag) MPI_Irecv(req->r_vec->v_data, 1,  \
  req->r_rdats[i], req->r_neighbors[i], tag, req->r_comm, &req->r_rreqs[i])

#define SEND_WAITALL(req) MPI_Waitall(req->r_nneighbors, req->r_sreqs, \
  MPI_STATUSES_IGNORE )

#define RECV_WAITALL(req) MPI_Waitall(req->r_nneighbors, req->r_rreqs, \
  MPI_STATUSES_IGNORE)

#define SEND_WAIT(req,i) MPI_Wait(&req->r_sreqs[i], MPI_STATUS_IGNORE)
#define RECV_WAIT(req,i) MPI_Wait(&req->r_rreqs[i], MPI_STATUS_IGNORE)

/* Neighborhood communication initiating all operations at-once */
int ADCL_change_sb_aao ( ADCL_request_t *req )
{
    int i, nneighs=req->r_nneighbors;

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

    return ADCL_SUCCESS;
}

/* Neighborhood communication based on pairwise message exchange */
int ADCL_change_sb_pairwaise ( ADCL_request_t *req )
{
    int i, dim;
    int nneighs = req->r_nneighbors;

    for ( dim=0, i=0; i<nneighs ; i+=2, dim++ ) {
	if ( req->r_coords[dim] % 2 == 0 ) { 
	    if ( MPI_PROC_NULL == req->r_neighbors[i] ) {
		RECV_START(req, i, AAO_SB_TAG);
		SEND_START(req, i, AAO_SB_TAG);
		RECV_WAIT (req,i);
		SEND_WAIT (req,i);
	    }
	    if ( MPI_PROC_NULL == req->r_neighbors[i+1] ) {
		RECV_START(req, i+1, AAO_SB_TAG);
		SEND_START(req, i+1, AAO_SB_TAG);
		RECV_WAIT (req,i);
		SEND_WAIT (req,i);
	    }
	}
	else {
	    if ( MPI_PROC_NULL == req->r_neighbors[i+1] ) {
		RECV_START(req, i+1, AAO_SB_TAG);
		SEND_START(req, i+1, AAO_SB_TAG);
		RECV_WAIT (req,i);
		SEND_WAIT (req,i);
	    }
	    if ( MPI_PROC_NULL == req->r_neighbors[i] ) {
		RECV_START(req, i, AAO_SB_TAG);
		SEND_START(req, i, AAO_SB_TAG);
		RECV_WAIT (req,i);
		SEND_WAIT (req,i);
	    }
	}
    }

    return ADCL_SUCCESS;
}
