#include "ADCL_internal.h"

static int ADCL_local_id_counter=0;
static int ADCL_request_set_topoinfo ( ADCL_request_t *newreq, 
				       MPI_Comm cart_comm );
static int ADCL_request_check_commstate ( ADCL_request_t *req, int mode );
static ADCL_method_t*  ADCL_request_get_method ( ADCL_request_t *req, int mode);
static int ADCL_request_update ( ADCL_request_t *req, 
				 TIME_TYPE t1, TIME_TYPE t2 );

ADCL_array_t *ADCL_request_farray;
 
int ADCL_request_create ( ADCL_vector_t *vec, MPI_Comm cart_comm, 
			  ADCL_request_t **req, int order )
{
    int i, ret = ADCL_SUCCESS;
    ADCL_request_t *newreq;
    ADCL_vector_t *pvec = vec;
    
    /* Alloacte the general ADCL_request structure */
    newreq = (ADCL_request_t *) calloc ( 1, sizeof(ADCL_request_t));
    if ( NULL == newreq ) {
	return ADCL_NO_MEMORY;
    }

    /* Fill in the according elements, start with the simple ones */
    newreq->r_id         = ADCL_local_id_counter++;
    ADCL_array_get_next_free_pos ( ADCL_request_farray, &(newreq->r_findex) );
    ADCL_array_set_element ( ADCL_request_farray, 
			     newreq->r_findex, 
			     newreq->r_id,
			     newreq );

    newreq->r_comm_state = ADCL_COMM_AVAIL;
    newreq->r_vec        = pvec;
    newreq->r_comm       = cart_comm;    /* we might have to duplicate it! */
    newreq->r_win        = MPI_WIN_NULL; /* TBD: unused for right now! */

    /* 
    ** Determine how many neighboring processes I have and who they are. 
    ** Needed later for the neighborhood communication 
    */ 
    ret = ADCL_request_set_topoinfo ( newreq, cart_comm );
    if ( ADCL_SUCCESS != ret ) {
	goto exit;
    }

    /* 
    ** Generate the derived datatypes describing which parts of this
    ** vector are going to be sent/received from which process
    */
    ret = ADCL_subarray_init ( newreq->r_nneighbors/2, 
			       newreq->r_neighbors, 
			       vec->v_ndims, 
			       vec->v_dims, 
			       vec->v_hwidth, 
			       vec->v_nc,
			       order, 
			       &(newreq->r_sdats), 
			       &(newreq->r_rdats ) );
    if ( ret != ADCL_SUCCESS ) {
	goto exit;
    }
    
    /* Allocate the request arrays, 2 for each neighboring process */
    newreq->r_sreqs=(MPI_Request *)malloc(newreq->r_nneighbors*sizeof(MPI_Request));
    newreq->r_rreqs=(MPI_Request *)malloc(newreq->r_nneighbors*sizeof(MPI_Request));
    if ( NULL == newreq->r_rreqs|| NULL == newreq->r_sreqs ) {
	ret = ADCL_NO_MEMORY;
	goto exit;
    }

    /* Initialize temporary data arrays for Pack/Unpack operations */
    ret = ADCL_packunpack_init ( newreq->r_nneighbors, 
				 newreq->r_neighbors,
				 cart_comm, 
				 &(newreq->r_sbuf), 
				 newreq->r_sdats,
				 &(newreq->r_spsize),
				 &(newreq->r_rbuf), 
				 newreq->r_rdats, 
				 &(newreq->r_rpsize) );
    if ( ADCL_SUCCESS != ret ) {
	goto exit;
    }

    /* Initialize the emethod data structures required for evaluating
       the different communication methods */
    newreq->r_state        = ADCL_STATE_TESTING;
    newreq->r_last_emethod = ADCL_UNDEFINED;
    newreq->r_wmethod      = NULL;

    newreq->r_emethods = ADCL_emethods_request_init ( cart_comm, 
						      newreq->r_nneighbors,
						      newreq->r_neighbors,
						      pvec->v_ndims, 
						      pvec->v_dims, 
						      pvec->v_nc,
						      pvec->v_hwidth, 
						      &(newreq->r_num_emethods );

 exit:
    if ( ret != ADCL_SUCCESS ) {
	if ( NULL != newreq->r_sreqs ) {
	    free ( newreq->r_sreqs );
	}
	if ( NULL != newreq->r_rreqs ) {
	    free ( newreq->r_rreqs );
	}
	if ( NULL != newreq->r_neighbors ) {
	    free ( newreq->r_neighbors );
	}
	ADCL_subarray_free ( newreq->r_nneighbors, &(newreq->r_sdats), 
			     &(newreq->r_rdats) );
	ADCL_packunpack_free ( newreq->r_nneighbors, &(newreq->r_rbuf),
			       &(newreq->r_sbuf), &(newreq->r_spsize), 
			       &(newreq->r_rpsize) );

	if ( MPI_WIN_NULL != newreq->r_win ) {
	    MPI_Win_free ( &(newreq->r_win) );
	}
	if ( NULL != newreq->r_emethods ) {
	    free ( newreq->r_emethods );
	}

	if ( NULL != newreq ) {
	    free ( newreq );
	    newreq = ADCL_REQUEST_NULL;
	}
    }

    *req = newreq;
    return ret;
}

int ADCL_request_free ( ADCL_request_t **req )
{
    ADCL_request_t *preq=*req;

    if ( NULL != preq->r_sreqs ) {
	free ( preq->r_sreqs );
    }
    if ( NULL != preq->r_rreqs ) {
	free ( preq->r_rreqs );
    }
    if ( NULL != preq->r_neighbors ) {
	free ( preq->r_neighbors );
    }
    ADCL_array_remove_element ( ADCL_request_farray, preq->r_findex);
    ADCL_subarray_free ( preq->r_nneighbors, &(preq->r_sdats), 
			 &(preq->r_rdats) );
    ADCL_packunpack_free ( preq->r_nneighbors, &(preq->r_rbuf),
			   &(preq->r_sbuf), &(preq->r_spsize), 
			   &(preq->r_rpsize) );

    if ( MPI_WIN_NULL != preq->r_win ) {
	MPI_Win_free ( &(preq->r_win) );
    }
    if ( NULL != preq->r_emethods ) {
	free ( preq->r_emethods );
    }
    if ( NULL != preq ) {
	free ( preq );
    }
    
    *req = ADCL_REQUEST_NULL;
    return ADCL_SUCCESS;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

int ADCL_request_init ( ADCL_request_t *req, int *db )
{
    int ret;
    TIME_TYPE t1, t2;
    ADCL_method_t *tmethod;

    ret = ADCL_request_check_commstate ( req, ADCL_COMM_AVAIL );
    if ( ADCL_SUCCESS != ret ) {
	return ret;
    }
	
    tmethod = ADCL_request_get_method ( req, ADCL_COMM_AVAIL );
    t1 = TIME;
    tmethod->m_ifunc ( req );
    t2 = TIME;

    ret = ADCL_request_update ( req, t1, t2 );
    *db = tmethod->m_db;
    if ( tmethod->m_db ) {
	req->r_comm_state = ADCL_COMM_ACTIVE;
    }

    return ret;
}
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

int ADCL_request_wait ( ADCL_request_t *req )
{
    int ret;
    TIME_TYPE t1, t2;
    ADCL_method_t *tmethod;

    ret = ADCL_request_check_commstate ( req, ADCL_COMM_ACTIVE );
    if ( ADCL_SUCCESS != ret ) {
	return ret;
    }
	
    tmethod = ADCL_request_get_method ( req, ADCL_COMM_ACTIVE );
    if ( NULL != tmethod->m_wfunc ) {
	t1 = TIME;
	tmethod->m_wfunc ( req );
	t2 = TIME;
	
	ret = ADCL_request_update ( req, t1, t2 );
    }
    else {
	ret = ADCL_request_update ( req, -1, -1 );
    }
    
    req->r_comm_state = ADCL_COMM_AVAIL;
    return ret;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
static int ADCL_request_check_commstate ( ADCL_request_t *req, int mode ) 
{

    /* requested mode and current communication mode 
       have to be identical */
    if ( req->r_comm_state == mode ) {
	return ADCL_SUCCESS;
    }
   
    return ADCL_USER_ERROR;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
static int ADCL_request_update ( ADCL_request_t *req, 
				 TIME_TYPE t1, TIME_TYPE t2 )
{
    int tmp;
    
    if ( (t1 == -1 ) && ( t2 == -1 ) ) {
	return ADCL_SUCCESS;
    }

    tmp = req->r_last_emethod;
    switch ( req->r_state ) {
	case ADCL_STATE_TESTING: 	    
	    ADCL_emethods_update ( req->r_num_emethods, req->r_emethods, 
				   req->r_last_emethod, t1, t2 );
	    break;
	case ADCL_STATE_REGULAR:
	    req->r_state = ADCL_emethod_monitor ( &(req->r_emethods[tmp]), 
						  t2, t1 );	    
	    break;
	case ADCL_STATE_DECISION:
	default:
	    ADCL_printf("%s: Unknown object status for req %d, status %d \n", 
			__FILE__, req->r_id, req->r_state );
	    break;
    }
    

    return ADCL_SUCCESS;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
static ADCL_method_t*  ADCL_request_get_method ( ADCL_request_t *req, int mode ) 
{
    int tmp;
    ADCL_method_t *tmethod=NULL;

    switch ( req->r_state ) {
	case ADCL_STATE_TESTING: 
	    tmp = ADCL_emethods_get_next ( req->r_num_emethods, 
					   req->r_emethods,
					   req->r_last_emethod, 
					   mode );
	    if ( ADCL_EVAL_DONE == tmp ) {
		req->r_state = ADCL_STATE_DECISION;
	    }
	    else if ( ( ADCL_ERROR_INTERNAL == tmp )   ||
		      ( 0 > tmp )                      || 
		      ( req->r_num_emethods <= tmp )) {
		return NULL;
	    }
	    else {
		req->r_last_emethod = tmp;
		tmethod = ADCL_emethod_get_method (&(req->r_emethods[tmp]));
		break;
	    }
	    /* no break; statement here on purpose! */
	case ADCL_STATE_DECISION:
	    tmp = ADCL_emethods_get_winner ( req->r_num_emethods,
					     req->r_emethods, 
					     req->r_comm );
	    req->r_wmethod = ADCL_emethod_get_method(&req->r_emethods[tmp]);
	    req->r_last_emethod = tmp;
	    req->r_state = ADCL_STATE_REGULAR;
	    /* no break; statement here on purpose! */
	case ADCL_STATE_REGULAR:
	    tmethod = req->r_wmethod;
	    break;
	default:
	    ADCL_printf("%s: Unknown object status for tmpreq %d, status %d \n", 
			__FILE__, req->r_id, req->r_state );
	    break;
    }

    return tmethod;
}


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
static int ADCL_request_set_topoinfo (ADCL_request_t *newreq, MPI_Comm ccomm )
{
    int cartdim, i, rank;

    MPI_Comm_rank ( ccomm, &rank );
    
    MPI_Cartdim_get ( ccomm, &cartdim );
    newreq->r_coords = (int *)malloc ( cartdim * sizeof(int));
    if ( NULL == newreq->r_coords ) {
	return ADCL_NO_MEMORY;
    }

    MPI_Cart_coords ( ccomm, rank, cartdim, newreq->r_coords );

    newreq->r_nneighbors = 2*cartdim;
    newreq->r_neighbors  = (int *) malloc ( 2*cartdim *sizeof(int));
    if ( NULL == newreq->r_neighbors ) {
	return ADCL_NO_MEMORY;
    }
    for ( i=0; i< cartdim; i++ ) {
	MPI_Cart_shift ( ccomm, i, 1, &(newreq->r_neighbors[2*i]), 
			 &(newreq->r_neighbors[2*i+1]) );
    }

    return ADCL_SUCCESS;
}
