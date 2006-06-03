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
#pragma weak Adcl_REQUEST_START   = adcl_request_start

#pragma weak adcl_request_init_  = adcl_request_init
#pragma weak adcl_request_init__ = adcl_request_init
#pragma weak Adcl_REQUEST_INIT   = adcl_request_init

#pragma weak adcl_request_wait_  = adcl_request_wait
#pragma weak adcl_request_wait__ = adcl_request_wait
#pragma weak Adcl_REQUEST_WAIT   = adcl_request_wait

#pragma weak adcl_request_start_overlap_  = adcl_request_start_overlap
#pragma weak adcl_request_start_overlap__ = adcl_request_start_overlap
#pragma weak Adcl_REQUEST_START_OVERLAP   = adcl_request_start_overlap

void adcl_request_create ( int *vec, int *comm, int *req, int *ierror ) 
{
    ADCL_vector_t *cvec;
    ADCL_request_t *creq;
    MPI_Comm ccomm;
    int topo_type;
    
    if ( ( NULL == vec )   || 
	 ( NULL == comm )  ||
	 ( NULL == req  )  ){
	*ierror = ADCL_INVALID_ARG;
	return;
    }

    ccomm = MPI_Comm_f2c (*comm);
    if ( cdat == MPI_COMM_NULL ) {
	*ierror = ADCL_INVALID_COMM;
	return;
    }
    MPI_Topo_test ( ccomm, &topo_type );
    if ( MPI_UNDEFINED==topo_type || MPI_GRAPH==topo_type ) {
	return ADCL_INVALID_COMM;
    }

    cvec = (ADCL_vector_t *) ADCL_array_get_ptr_by_pos ( ADCL_vector_farray, 
							 *vec );

    *ierror = ADCL_request_create ( cvec, ccomm, &creq, MPI_ORDER_FORTRAN );
    if ( *ierror == ADCL_SUCCESS ) {
	*req = creq->r_findex;
    }

    return;
}

void adcl_request_free ( int *vec, int *ierror )
{
    ADCL_vector_t *cvec;

    cvec = (ADCL_vector_t *) ADCL_array_get_ptr_by_pos ( ADCL_vector_farray, 
							 *vec );
    *ierror = ADCL_vector_deregister ( &cvec );

    return;
}

