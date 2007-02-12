#include "ADCL_internal.h"

static int ADCL_local_id_counter=0;
static ADCL_method_t*  ADCL_request_get_method ( ADCL_request_t *req, int mode);

ADCL_array_t *ADCL_request_farray;


#define CHECK_COMM_STATE(state1,state2) if (state1!=state2) return ADCL_SUCCESS

 
int ADCL_request_create ( ADCL_vector_t *vec, ADCL_topology_t *topo, 
			  ADCL_request_t **req, int order )
{
    int ret = ADCL_SUCCESS;
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

    /* 
    ** Set the topology information
    */ 
    newreq->r_comm       = topo->t_comm;    /* we might have to duplicate it! */
    MPI_Comm_rank ( topo->t_comm, &(newreq->r_rank) );
    
    newreq->r_nneighbors = 2 * topo->t_ndims;
    newreq->r_neighbors = (int *) malloc ( newreq->r_nneighbors * sizeof(int));
    newreq->r_coords = (int *) malloc ( topo->t_ndims * sizeof(int));
    if ( NULL == newreq->r_neighbors || NULL == newreq->r_coords ) {
	ret = ADCL_NO_MEMORY;
	goto exit;
    }
    
    memcpy ( newreq->r_neighbors, topo->t_neighbors, 
	     sizeof(int)*newreq->r_nneighbors );
    memcpy ( newreq->r_coords, topo->t_coords, sizeof(int) * topo->t_ndims );

#ifdef MPI_WIN 
    MPI_Type_size (MPI_DOUBLE, &tsize);
    if ( pvec->v_nc > 0 ) {
      tcount = pvec->v_nc;
    }
    for (i=0; i <pvec->v_ndims; i++){
        tcount *= pvec->v_dims[i];
    }
    MPI_Win_create ( newreq->r_vec->v_data, tcount , tsize, MPI_INFO_NULL, 
		     topo->t_comm, &(newreq->r_win));
    MPI_Comm_group ( topo->t_comm, &(newreq->r_group) );   
#else
    newreq->r_win        = MPI_WIN_NULL; /* TBD: unused for right now! */
    newreq->r_group      = MPI_GROUP_NULL; /* TBD: unused for right now! */
#endif

    /* 
    ** Generate the derived datatypes describing which parts of this
    ** vector are going to be sent/received from which process
    */
    if ( vec->v_ndims == 1 || ( vec->v_ndims == 2 && vec->v_nc > 0 )) {
	ret = ADCL_indexed_1D_init ( vec->v_dims[0],
				     vec->v_hwidth,
				     vec->v_nc, 
				     vec->v_dat,
				     order, 
				     &(newreq->r_sdats), 
				     &(newreq->r_rdats) );
    }
    else if ( (vec->v_ndims == 2 && vec->v_nc == 0) ||
	      (vec->v_ndims == 3 && vec->v_nc > 0 )) {
	ret = ADCL_indexed_2D_init ( vec->v_dims,
				     vec->v_hwidth,
				     vec->v_nc, 
				     order, 
				     vec->v_dat,
				     &(newreq->r_sdats), 
				     &(newreq->r_rdats) );
    }
    else if ( MPI_ORDER_C == order && 
	      ((vec->v_ndims == 3 && vec->v_nc == 0) ||
	       (vec->v_ndims == 4 && vec->v_nc > 0 ))){
	ret = ADCL_indexed_3D_init ( vec->v_dims,
				     vec->v_hwidth,
				     vec->v_nc, 
				     order, 
				     vec->v_dat,
				     &(newreq->r_sdats), 
				     &(newreq->r_rdats) );
    }   
    else {
	ret = ADCL_subarray_init ( newreq->r_nneighbors/2, 
				   vec->v_ndims, 
				   vec->v_dims, 
				   vec->v_hwidth, 
				   vec->v_nc,
				   order, 
				   vec->v_dat,
				   &(newreq->r_sdats), 
				   &(newreq->r_rdats ) );
    }
    if ( ret != ADCL_SUCCESS ) {
	goto exit;
    }
    
    /* Allocate the request arrays, 2 for each neighboring process */
    newreq->r_sreqs=(MPI_Request *)malloc(newreq->r_nneighbors*
					  sizeof(MPI_Request));
    newreq->r_rreqs=(MPI_Request *)malloc(newreq->r_nneighbors*
					  sizeof(MPI_Request));
    if ( NULL == newreq->r_rreqs|| NULL == newreq->r_sreqs ) {
	ret = ADCL_NO_MEMORY;
	goto exit;
    }

    /* Initialize temporary data arrays for Pack/Unpack operations */
    ret = ADCL_packunpack_init ( newreq->r_nneighbors, 
				 newreq->r_neighbors,
				 topo->t_comm, 
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
    newreq->r_cmethod  = NULL;
    newreq->r_ermethod = ADCL_emethod_init ( newreq->r_comm, 
					     newreq->r_nneighbors,
					     newreq->r_neighbors,
					     pvec->v_ndims, 
					     pvec->v_dims, 
					     pvec->v_nc,
					     pvec->v_hwidth );

 exit:
    if ( ret != ADCL_SUCCESS ) {
	if ( NULL != newreq->r_sreqs ) {
	    free ( newreq->r_sreqs );
	}
	if ( NULL != newreq->r_rreqs ) {
	    free ( newreq->r_rreqs );
	}
	ADCL_subarray_free ( newreq->r_nneighbors, &(newreq->r_sdats), 
			     &(newreq->r_rdats) );
	ADCL_packunpack_free ( newreq->r_nneighbors, &(newreq->r_rbuf),
			       &(newreq->r_sbuf), &(newreq->r_spsize), 
			       &(newreq->r_rpsize) );

	if ( NULL != newreq->r_neighbors ) {
	    free ( newreq->r_neighbors );
	}
	if ( MPI_WIN_NULL != newreq->r_win ) {
	    MPI_Win_free ( &(newreq->r_win) );
	}
	if ( NULL != newreq->r_ermethod ) {
	    ADCL_emethod_free ( newreq->r_ermethod );
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
    ADCL_array_remove_element ( ADCL_request_farray, preq->r_findex);
    ADCL_subarray_free ( preq->r_nneighbors, &(preq->r_sdats), 
			 &(preq->r_rdats) );
    ADCL_packunpack_free ( preq->r_nneighbors, &(preq->r_rbuf),
			   &(preq->r_sbuf), &(preq->r_spsize), 
			   &(preq->r_rpsize) );

    ADCL_emethod_free ( preq->r_ermethod );
#ifdef MPI_WIN
    MPI_Win_free ( &(preq->r_win));
    MPI_Group_free (&(preq->r_group));
#endif
        
    if ( NULL != preq->r_neighbors ) {
	free ( preq->r_neighbors );
    }
    if ( MPI_WIN_NULL != preq->r_win ) {
	MPI_Win_free ( &(preq->r_win) );
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

    CHECK_COMM_STATE ( req->r_comm_state, ADCL_COMM_AVAIL);
	
    req->r_cmethod = ADCL_request_get_method ( req, ADCL_COMM_AVAIL);
    req->r_cmethod->m_ifunc ( req );

    *db = req->r_cmethod->m_db;
    if ( req->r_cmethod->m_db ) {
	req->r_comm_state = ADCL_COMM_ACTIVE;
    }

    return ADCL_SUCCESS;
}
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_request_wait ( ADCL_request_t *req )
{

    CHECK_COMM_STATE (req->r_comm_state, ADCL_COMM_ACTIVE);
	
    if ( NULL != req->r_cmethod->m_wfunc ) {
	req->r_cmethod->m_wfunc ( req );
    }
    
    req->r_comm_state = ADCL_COMM_AVAIL;
    return ADCL_SUCCESS;
}


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_request_update ( ADCL_request_t *req, 
			  TIME_TYPE t1, TIME_TYPE t2 )
{
    
    if ( (t1 == -1 ) && ( t2 == -1 ) ) {
	return ADCL_SUCCESS;
    }

    ADCL_printf("%d: request %d method %d (%s) %8.4f \n", 
		req->r_rank, req->r_id, req->r_cmethod->m_id, 
		req->r_cmethod->m_name, t2>t1 ? (t2-t1):(1000000-t1+t2));
    switch ( req->r_ermethod->er_state ) {
	case ADCL_STATE_TESTING: 	    
	    ADCL_emethods_update (req->r_ermethod, req->r_erlast, 
				  req->r_erflag, t1, t2);
	    break;
	case ADCL_STATE_REGULAR:
	    req->r_ermethod->er_state = ADCL_emethod_monitor (req->r_ermethod, 
							      req->r_ermethod->er_last,
							      t2, t1 );	    
	    break;
	case ADCL_STATE_DECISION:
	default:
	    ADCL_printf("%s: Unknown object status for req %d, status %d \n", 
			__FILE__, req->r_id, req->r_ermethod->er_state );
	    break;
    }
    

    return ADCL_SUCCESS;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
static ADCL_method_t*  ADCL_request_get_method ( ADCL_request_t *req, 
						 int mode ) 
{
    int tmp, flag, rank;
    ADCL_method_t *tmethod=NULL;

    switch ( req->r_ermethod->er_state ) {
	case ADCL_STATE_TESTING: 
	    tmp = ADCL_emethods_get_next ( req->r_ermethod, mode, &flag );
	    if ( ADCL_EVAL_DONE == tmp ) {
		req->r_ermethod->er_state = ADCL_STATE_DECISION;
	    }
	    else if ( (ADCL_ERROR_INTERNAL == tmp)||( 0 > tmp )) {
		return NULL;
	    }
	    else {
		req->r_erlast = tmp;
		req->r_erflag = flag;
		tmethod = ADCL_emethod_get_method (req->r_ermethod, tmp );
		break;
	    }
	    /* no break; statement here on purpose! */
	case ADCL_STATE_DECISION:
	    MPI_Comm_rank ( req->r_comm, &rank );
#if 0
	    ADCL_printf("#%d: Initiating decision procedure for req %d\n", 
			rank, req->r_id);
#endif
	    tmp = ADCL_emethods_get_winner ( req->r_ermethod, req->r_comm);
	    req->r_ermethod->er_last    = tmp;
	    req->r_ermethod->er_wmethod = ADCL_emethod_get_method(req->r_ermethod, tmp);
	    ADCL_printf("#%d:  req %d winner is %d %s\n", 
			rank, req->r_id, req->r_ermethod->er_wmethod->m_id, 
			req->r_ermethod->er_wmethod->m_name);
	    
	    req->r_ermethod->er_state = ADCL_STATE_REGULAR;
	    /* no break; statement here on purpose! */
	case ADCL_STATE_REGULAR:
	    tmethod = req->r_ermethod->er_wmethod;
	    break;
	default:
	    ADCL_printf("#%s: Unknown object status for req %d, status %d\n", 
			__FILE__, req->r_id, req->r_ermethod->er_state );
	    break;
    }

    return tmethod;
}


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
