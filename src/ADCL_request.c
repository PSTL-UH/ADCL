#include "ADCL_internal.h"

static int ADCL_local_id_counter=0;
static int ADCL_request_set_topoinfo ( ADCL_request_t *newreq, 
				       MPI_Comm cart_comm );

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

    /* Initialize data structures required for single-block operations */
    newreq->rs_state        = ADCL_STATE_TESTING;
    newreq->rs_num_emethods = ADCL_get_num_singleblock_methods ();
    newreq->rs_last_emethod = ADCL_UNDEFINED;

    newreq->rs_emethods = (ADCL_emethod_t*) calloc ( 1, newreq->rs_num_emethods*
						     sizeof(ADCL_emethod_t));
    if ( NULL == newreq->rs_emethods ) {
	ret = ADCL_NO_MEMORY;
	goto exit;
    }
    for ( i=0; i<newreq->rs_num_emethods; i++ ) {
	newreq->rs_emethods[i].em_id     = ADCL_emethod_get_next_id ();
	newreq->rs_emethods[i].em_method = ADCL_get_singleblock_method(i);
	newreq->rs_emethods[i].em_min    = 999999;
    }
    newreq->rs_wmethod = NULL;

    /* Initialize data structures required for dual-block operations */
    newreq->rd_state        = ADCL_STATE_TESTING;
    newreq->rd_num_emethods = ADCL_get_num_dualblock_methods ();
    newreq->rd_last_emethod = ADCL_UNDEFINED;

    newreq->rd_emethods = (ADCL_emethod_t *) calloc ( 1, newreq->rd_num_emethods*
						      sizeof(ADCL_emethod_t));
    if ( NULL == newreq->rd_emethods ) {
	ret = ADCL_NO_MEMORY;
	goto exit;
    }
    for ( i=0; i<newreq->rd_num_emethods; i++ ) {
	newreq->rd_emethods[i].em_id = ADCL_emethod_get_next_id ();
	newreq->rd_emethods[i].em_method = ADCL_get_dualblock_method(i);
	newreq->rd_emethods[i].em_min    = 999999;
    }
    newreq->rd_wmethod = NULL;

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
	if ( NULL != newreq->rs_emethods ) {
	    free ( newreq->rs_emethods );
	}
	if ( NULL != newreq->rd_emethods ) {
	    free ( newreq->rd_emethods );
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

    if ( preq->r_id > ADCL_local_id_counter || 
	 preq->r_id < 0 ) {
	return ADCL_INVALID_REQUEST;
    }
    if ( preq->r_comm_state != ADCL_COMM_AVAIL ) {
	return ADCL_INVALID_REQUEST;
    }

    /* it seems to be valid, so free all allocated resources */
    if ( NULL != preq->r_sreqs ) {
	free ( preq->r_sreqs );
    }
    if ( NULL != preq->r_rreqs ) {
	free ( preq->r_rreqs );
    }
    if ( NULL != preq->r_neighbors ) {
	free ( preq->r_neighbors );
    }
    ADCL_subarray_free ( preq->r_nneighbors, &(preq->r_sdats), 
			 &(preq->r_rdats) );
    ADCL_packunpack_free ( preq->r_nneighbors, &(preq->r_rbuf),
			   &(preq->r_sbuf), &(preq->r_spsize), 
			   &(preq->r_rpsize) );

    if ( MPI_WIN_NULL != preq->r_win ) {
	MPI_Win_free ( &(preq->r_win) );
    }
    if ( NULL != preq->rs_emethods ) {
	free ( preq->rs_emethods );
    }
    if ( NULL != preq->rd_emethods ) {
	free ( preq->rd_emethods );
    }
    if ( NULL != preq ) {
	free ( preq );
    }
    
    *req = ADCL_REQUEST_NULL;
    return ADCL_SUCCESS;
}

int ADCL_3D_comm_single_block ( ADCL_request_t *req )
{
    TIME_TYPE t1, t2;
    int tmp;
    ADCL_request_t *tmpreq = req;
    ADCL_method_t *tmethod;
 
    if ( tmpreq->r_comm_state == ADCL_COMM_ACTIVE ) {
	ADCL_printf("Illegal to initiate a new communication before finishing"
		    " the last one\n");
	return ADCL_USER_ERROR;
    }

    switch ( tmpreq->rs_state ) {
	case ADCL_STATE_TESTING: 
	    tmp = ADCL_emethods_get_next ( tmpreq->rs_num_emethods, 
					   tmpreq->rs_emethods,
					   tmpreq->rs_last_emethod );
	    if ( ADCL_EVAL_DONE == tmp ) {
		tmpreq->rs_state = ADCL_STATE_DECISION;
	    }
	    else if ( ( ADCL_ERROR_INTERNAL == tmp )   ||
		      ( 0 > tmp )                      || 
		      ( tmpreq->rs_num_emethods <= tmp )) {
		return ADCL_ERROR_INTERNAL;
	    }
	    else {
		tmpreq->rs_last_emethod = tmp;
		tmethod = ADCL_emethod_get_method (&(tmpreq->rs_emethods[tmp]));
		
		t1 = TIME;
		tmethod->m_ifunc ( tmpreq );
		t2 = TIME;

		ADCL_emethods_update(tmpreq->rs_num_emethods,tmpreq->rs_emethods,
				     tmpreq->rs_last_emethod, t1, t2 );
	    }
	    break;
	case ADCL_STATE_DECISION:
	    tmp = ADCL_emethods_get_winner ( tmpreq->rs_num_emethods,
					     tmpreq->rs_emethods, 
					     tmpreq->r_comm );
	    tmpreq->rs_wmethod=ADCL_emethod_get_method(&tmpreq->rs_emethods[tmp]);
	    tmpreq->rs_last_emethod = tmp;
	    tmpreq->rs_state  = ADCL_STATE_REGULAR;
	    /* no break; statement here on purpose! */
	case ADCL_STATE_REGULAR:
	    t1 = TIME;
	    tmpreq->rs_wmethod->m_ifunc ( tmpreq );
	    t2 = TIME;

	    tmpreq->rs_state=ADCL_emethod_monitor(&tmpreq->rs_emethods[tmpreq->rs_last_emethod], 
						      t2, t1 );	    
	    break;
	default:
	    ADCL_printf("%s: Unknown object status for tmpreq %d, status %d \n", 
			__FILE__, tmpreq->r_id, tmpreq->rs_state );
	    break;
    }

    /* No need to mofify the communication state of the object, since this is 
       a blocking operation */
    return ADCL_SUCCESS;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

int ADCL_3D_comm_dual_block_init ( ADCL_request_t *req )
{
    TIME_TYPE t1, t2;
    ADCL_request_t *tmpreq = req;
    ADCL_method_t *tmethod;
    int tmp;
    
    if ( tmpreq->r_comm_state != ADCL_COMM_AVAIL ) {
	ADCL_printf("Illegal to initiate a new communication before finishing"
		    " the last one\n");
	return ADCL_USER_ERROR;
    }

    switch ( tmpreq->rd_state ) {
	case ADCL_STATE_TESTING:
	    tmp = ADCL_emethods_get_next ( tmpreq->rd_num_emethods, 
					   tmpreq->rd_emethods,
					   tmpreq->rd_last_emethod );
	    if ( ADCL_EVAL_DONE == tmp ) {
		tmpreq->rd_state = ADCL_STATE_DECISION;
	    }
	    else if ( ( ADCL_ERROR_INTERNAL == tmp )   ||
		      ( 0 > tmp )                      || 
		      ( tmpreq->rd_num_emethods <= tmp )) {
		return ADCL_ERROR_INTERNAL;
	    }
	    else {
		tmpreq->rd_last_emethod = tmp;
		tmethod = ADCL_emethod_get_method(&(tmpreq->rd_emethods[tmp]));
		
		t1 = TIME;
		tmethod->m_ifunc ( tmpreq );
		t2 = TIME;

		ADCL_emethods_update ( tmpreq->rd_num_emethods, tmpreq->rd_emethods, 
				       tmpreq->rd_last_emethod, t1, t2 );
		tmpreq->r_comm_state = ADCL_COMM_ACTIVE;
	    }
	    break;
	case ADCL_STATE_DECISION:
	    tmp = ADCL_emethods_get_winner ( tmpreq->rd_num_emethods,
					     tmpreq->rd_emethods, 
					     tmpreq->r_comm );
	    tmpreq->rd_wmethod = ADCL_emethod_get_method ( &(tmpreq->rd_emethods[tmp]) );
	    tmpreq->rd_last_emethod = tmp;
	    tmpreq->rd_state   = ADCL_STATE_REGULAR;
	    /* no break; statement here on purpose! */
	case ADCL_STATE_REGULAR:
	    t1 = TIME;
	    tmpreq->rd_wmethod->m_ifunc ( tmpreq );
	    t2 = TIME;

	    tmpreq->rd_state = ADCL_emethod_monitor ( &(tmpreq->rd_emethods[
						      tmpreq->rd_last_emethod]), 
						      t2, t1 );	    
	    tmpreq->r_comm_state = ADCL_COMM_ACTIVE;
	    break;
	default:
	    ADCL_printf("%s: Unknown object status for tmpreq %d, status %d \n", 
			__FILE__, tmpreq->r_id, tmpreq->rd_state );
	    break;
    }

    return ADCL_SUCCESS;
}

int ADCL_3D_comm_dual_block_wait ( ADCL_request_t *req )
{
    TIME_TYPE t1, t2;
    ADCL_request_t *tmpreq = req;
    ADCL_method_t *tmethod;
    int tmp;
    
    if ( tmpreq->r_comm_state != ADCL_COMM_ACTIVE ) {
	ADCL_printf("Have to initiate a communication before calling wait\n");
	return ADCL_USER_ERROR;
    }

    switch ( tmpreq->rd_state ) {
	case ADCL_STATE_TESTING: 	    
	    tmp = tmpreq->rd_last_emethod;
	    tmethod = ADCL_emethod_get_method(&(tmpreq->rd_emethods[tmp]));
	    
	    t1 = TIME;
	    tmethod->m_wfunc ( tmpreq );
	    t2 = TIME;

	    ADCL_emethods_update ( tmpreq->rd_num_emethods, tmpreq->rd_emethods, 
				   tmpreq->rd_last_emethod, t1, t2 );
	    tmpreq->r_comm_state = ADCL_COMM_AVAIL;
	    break;
	case ADCL_STATE_REGULAR:
	    tmp = tmpreq->rd_last_emethod;
	    tmethod = ADCL_emethod_get_method(&(tmpreq->rd_emethods[tmp]));

	    t1 = TIME;
	    tmpreq->rd_wmethod->m_wfunc ( tmpreq );
	    t2 = TIME;

	    tmpreq->rd_state = ADCL_emethod_monitor ( &(tmpreq->rd_emethods[
						      tmpreq->rd_last_emethod]), 
						      t2, t1 );	    
	    tmpreq->r_comm_state = ADCL_COMM_AVAIL;
	    break;
	case ADCL_STATE_DECISION:
	default:
	    ADCL_printf("%s: Unknown object status for tmpreq %d, status %d \n", 
			__FILE__, tmpreq->r_id, tmpreq->rd_state );
	    break;
    }

    return ADCL_SUCCESS;
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
