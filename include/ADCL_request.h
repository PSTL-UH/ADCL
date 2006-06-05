#ifndef __ADCL_REQUEST_H__
#define __ADCL_REQUEST_H__

#include "mpi.h"

#include "ADCL_vector.h"
#include "ADCL_method.h"
#include "ADCL_emethod.h"

#define ADCL_COMM_ACTIVE 1024
#define ADCL_COMM_AVAIL  1025

/* The most general ADCL object used to start and identify a communication 'entity' */
struct ADCL_request_s{
    int                        r_id; /* unique identifier */
    int                    r_findex; /* index for the fortran interface */
    int                r_comm_state; /* communication state of the object  */
    ADCL_vector_t            *r_vec; /* ptr to the vector describing data items */
    MPI_Comm                 r_comm; /* Communicator used for communication */
    MPI_Win                   r_win; /* window used for one-sided operations */
    int                r_nneighbors; /* numbers of neighboring processes = dim of 
					v_dats, v_sreqs, v_rreqs. nneighbor should
				        be 2*the number of dimensions of the 
				        cartesian structure */
    int                *r_neighbors; /* array of neighboring processes. */
    int                   *r_coords; /* coordinate of this proc in the proc-topology */
    int                   *r_spsize; /* size of each individual temporary sbuf used 
			     	        for pack/unpack */
    int                   *r_rpsize; /* size of each individual temporary rbuf used 
					for pack/unpack */
    MPI_Datatype           *r_sdats; /* array of MPI datatypes used for sending */
    MPI_Datatype           *r_rdats; /* array of MPI datatypes used for receiving */
    MPI_Request            *r_sreqs; /* array of send requests used for nb ops */
    MPI_Request            *r_rreqs; /* array of recv requests used for nb ops */
    char                   **r_rbuf; /* temp recv buffer used for pack/unpack */
    char                   **r_sbuf; /* temp send buffer used for pack/unpack */    

    int                     r_erlast; /* last method used */
    int                     r_erflag; /* flag to be passed to the state machine */
    ADCL_emethod_req_t   *r_ermethod; /* list of emethods being evaluated */
    ADCL_method_t         *r_wmethod; /* winner method used after the testing */
};
typedef struct ADCL_request_s ADCL_request_t;

extern ADCL_array_t * ADCL_request_farray;

int ADCL_request_create ( ADCL_vector_t *vec, MPI_Comm comm, 
			  ADCL_request_t **req, int order );
int ADCL_request_free ( ADCL_request_t **req );
int ADCL_request_init ( ADCL_request_t *req, int *db );
int ADCL_request_wait ( ADCL_request_t *req );

#endif /* __ADCL_REQUEST_H__ */

