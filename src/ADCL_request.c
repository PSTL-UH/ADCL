#include "ADCL.h"

static int ADCL_local_id_counter=0;

int ADCL_request_create ( ADCL_vector vec, MPI_Comm cart_comm, 
			  ADCL_request *req )
{
    int i, ret = ADCL_SUCCESS;
    int topo_type;
    ADCL_request_t *newreq;
    ADCL_vector_t *pvec = vec;
    int cartdim;
    
    /* Right now we can only handle cartesian topologies! */
    MPI_Topo_test ( cart_comm, &topo_type );
    if ( MPI_UNDEFINED == topo_type  ||
	 MPI_GRAPH == topo_type ) {
	return ADCL_INVALID_COMM;
    }
    
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

    /* Determine how many neighboring processes I have  and who they are. 
       Needed later for the neighborhood communication */ 
    MPI_Cartdim_get ( cart_comm, &cartdim );
    newreq->r_nneighbors = 2*cartdim;
    newreq->r_neighbors = (int *) malloc ( 2*cartdim *sizeof(int));
    if ( NULL == newreq->r_neighbors ) {
	ret = ADCL_NO_MEMORY;
	goto exit;
    }
    for ( i=0; i< cartdim; i++ ) {
	MPI_Cart_shift ( cart_comm, i, 1, &(newreq->r_neighbors[2*i]), 
			 &(newreq->r_neighbors[2*i+1]) );
    }
    
    /* Generate the derived datatypes describing which parts 
       of this vector are going to be sent/received from which 
       process */
    ret = ADCL_subarray_c ( cartdim, vec->v_dims, vec->v_hwidth, 
			    newreq->r_neighbors, &(newreq->r_sdats), 
			    &(newreq->r_rdats ) );
    if ( ret != ADCL_SUCCESS ) {
	goto exit;
    }
    
    /* Allocate the request arrays, 2 for each neighboring process */
    newreq->r_sreqs = (MPI_Request *)malloc(newreq->r_nneighbors * sizeof(MPI_Request));
    newreq->r_rreqs = (MPI_Request *)malloc(newreq->r_nneighbors * sizeof(MPI_Request));
    if ( NULL == newreq->r_rreqs|| NULL == newreq->r_sreqs ) {
	ret = ADCL_NO_MEMORY;
	goto exit;
    }

    /* Initialize temporary data arrays for Pack/Unpack operations */
    newreq->r_psize = 0;    /* for now! */
    if ( newreq->r_psize > 0 ) {
	newreq->r_rbuf = (char *) malloc ( newreq->r_psize ); 
	newreq->r_sbuf = (char *) malloc ( newreq->r_psize ); 
	if ( NULL == newreq->r_rbuf || NULL == newreq->r_sbuf ) {
	    ret = ADCL_NO_MEMORY;
	    goto exit;
	}
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
	if ( NULL != newreq->r_rdats ) {
	    for ( i=0; i<newreq->r_nneighbors; i++ ){
		if ( MPI_DATATYPE_NULL != newreq->r_rdats[i]) {
		    MPI_Type_free ( &(newreq->r_rdats[i]));
		}
	    }
	    free ( newreq->r_rdats );
	}
	if ( NULL != newreq->r_sdats ) {
	    for ( i=0; i<newreq->r_nneighbors; i++ ){
		if ( MPI_DATATYPE_NULL != newreq->r_sdats[i]) {
		    MPI_Type_free ( &(newreq->r_sdats[i]));
		}
	    }
	    free ( newreq->r_sdats );
	}
	if ( MPI_WIN_NULL != newreq->r_win ) {
	    MPI_Win_free ( &(newreq->r_win) );
	}
	if ( NULL != newreq->r_sbuf ) {
	    free ( newreq->r_sbuf);
	}
	if ( NULL != newreq->r_rbuf ) {
	    free ( newreq->r_rbuf);
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

int ADCL_request_free ( ADCL_request *req )
{
    ADCL_request_t *preq;
    int i;

    /* check whether the request is a valid object
       and is allowed to be free right now (= no communciation
       started ) */
    if ( NULL == req ) {
	return ADCL_INVALID_REQUEST;
    }

    preq = *req;
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
	if ( NULL != preq->r_rdats ) {
	    for ( i=0; i<preq->r_nneighbors; i++ ){
		if ( MPI_DATATYPE_NULL != preq->r_rdats[i]) {
		    MPI_Type_free ( &(preq->r_rdats[i]));
		}
	    }
	    free ( preq->r_rdats );
	}
	if ( NULL != preq->r_sdats ) {
	    for ( i=0; i<preq->r_nneighbors; i++ ){
		if ( MPI_DATATYPE_NULL != preq->r_sdats[i]) {
		    MPI_Type_free ( &(preq->r_sdats[i]));
		}
	    }
	    free ( preq->r_sdats );
	}
    if ( MPI_WIN_NULL != preq->r_win ) {
	MPI_Win_free ( &(preq->r_win) );
    }
    if ( NULL != preq->r_sbuf ) {
	free ( preq->r_sbuf);
    }
    if ( NULL != preq->r_rbuf ) {
	free ( preq->r_rbuf);
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

int ADCL_3D_comm_single_block ( ADCL_request req )
{
    TIME_TYPE t1, t2;
    int tmp;
    ADCL_request_t *tmpreq = req;
    ADCL_method_t *tmethod;
 
    if ( tmpreq->r_comm_state == ADCL_COMM_ACTIVE ) {
	printf("Illegal to initiate a new communication before finishing"
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
	    printf("%s: Unknown object status for tmpreq %d, status %d \n", 
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

int ADCL_3D_comm_dual_block_init ( ADCL_request req )
{
    TIME_TYPE t1, t2;
    ADCL_request_t *tmpreq = req;
    ADCL_method_t *tmethod;
    int tmp;
    
    if ( tmpreq->r_comm_state != ADCL_COMM_AVAIL ) {
	printf("Illegal to initiate a new communication before finishing the last"
		    " one\n");
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
	    printf("%s: Unknown object status for tmpreq %d, status %d \n", 
		   __FILE__, tmpreq->r_id, tmpreq->rd_state );
	    break;
    }

    return ADCL_SUCCESS;
}

int ADCL_3D_comm_dual_block_wait ( ADCL_request req )
{
    TIME_TYPE t1, t2;
    ADCL_request_t *tmpreq = req;
    ADCL_method_t *tmethod;
    int tmp;
    
    if ( tmpreq->r_comm_state != ADCL_COMM_ACTIVE ) {
	printf("Have to initiate a communication before calling wait\n");
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
	    printf("%s: Unknown object status for tmpreq %d, status %d \n", 
		   __FILE__, tmpreq->r_id, tmpreq->rd_state );
	    break;
    }

    return ADCL_SUCCESS;
}

