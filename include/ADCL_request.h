#ifndef __ADCL_REQUEST_H__
#define __ADCL_REQUEST_H__

#include "mpi.h"

#include "ADCL_vector.h"
#include "ADCL_function.h"
#include "ADCL_emethod.h"
#include "ADCL_topology.h"

#define ADCL_COMM_ACTIVE 1024
#define ADCL_COMM_AVAIL  1025

/* The most general ADCL object used to start and identify a communication 'entity' */
struct ADCL_request_s{
    int                        r_id; /* unique identifier */
    int                    r_findex; /* index for the fortran interface */
    int                r_comm_state; /* communication state of the object  */

    ADCL_vector_t            *r_vec; /* ptr to the vector describing data items */
    ADCL_emethod_t       *r_emethod; /* ptr to the emethod describing everything */ 
    ADCL_function_t     *r_function; /* ADCL function currently being used. This pointer
					will also contain the 'winner' function once 
					the evaluation part is over. */


    /* Elements used for the communication */
    int                   *r_spsize; /* size of each individual temporary sbuf used 
			     	        for pack/unpack */
    int                   *r_rpsize; /* size of each individual temporary rbuf used 
					for pack/unpack */
    MPI_Win                   r_win; /* window used for one-sided operations */
    MPI_Group               r_group; /* Group used for some window operations */
    MPI_Datatype           *r_sdats; /* array of MPI datatypes used for sending */
    MPI_Datatype           *r_rdats; /* array of MPI datatypes used for receiving */
    MPI_Request            *r_sreqs; /* array of send requests used for nb ops */
    MPI_Request            *r_rreqs; /* array of recv requests used for nb ops */
    char                   **r_rbuf; /* temp recv buffer used for pack/unpack */
    char                   **r_sbuf; /* temp send buffer used for pack/unpack */    

    /* Elements required for the selection logic */
    int                     r_erlast; /* last method used */
    int                     r_erflag; /* flag to be passed to the state machine */
    TIME_TYPE                 r_time; /* temporary buffer to store the exeuction 
					 time for dual-block operations */
};
typedef struct ADCL_request_s ADCL_request_t;

extern ADCL_array_t * ADCL_request_farray;

int ADCL_request_create ( ADCL_vector_t *vec, ADCL_topology_t *topo, 
			  ADCL_request_t **req, int order );
int ADCL_request_free ( ADCL_request_t **req );
int ADCL_request_init ( ADCL_request_t *req, int *db );
int ADCL_request_wait ( ADCL_request_t *req );

int ADCL_request_update ( ADCL_request_t *req, 
			  TIME_TYPE t1, TIME_TYPE t2 );

#endif /* __ADCL_REQUEST_H__ */

