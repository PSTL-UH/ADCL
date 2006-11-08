#include "ADCL_internal.h"

static int ADCL_local_id_counter=0;
static ADCL_array_t *ADCL_emethod_req_array=NULL;

int ADCL_emethod_selection = -1;
int ADCL_merge_requests=1;
int ADCL_emethod_numtests=ADCL_EMETHOD_NUMTESTS;
int ADCL_emethod_use_perfhypothesis=0; /* false */

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_emethod_req_init ( void ) 
{
    ADCL_array_init ( &(ADCL_emethod_req_array), "ADCL_emethod_req", 32 );    
    return ADCL_SUCCESS;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_emethod_req_finalize ( void ) 
{
    ADCL_array_free ( &(ADCL_emethod_req_array) );    
    return ADCL_SUCCESS;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
ADCL_emethod_req_t * ADCL_emethod_init ( MPI_Comm comm, int nneighbors, 
					 int *neighbors, int vndims, 
					 int *vdims, int vnc, int vhwidth )
{
    ADCL_emethod_req_t *er;    
    int i;

    if ( ADCL_merge_requests ) {
	int j, last, found=-1;
	int result;
	
	/* Check first, whether we have an entry in the ADCL_emethods_req_array,
	   which fulfills already our requirements; 
	*/
	last = ADCL_array_get_last ( ADCL_emethod_req_array );
	for ( i=0; i<= last; i++ ) {
	    er = ( ADCL_emethod_req_t * ) ADCL_array_get_ptr_by_pos ( 
		ADCL_emethod_req_array, i );
	    
	    MPI_Comm_compare ( er->er_comm, comm, &result );
	    if ( ( result != MPI_IDENT) && (result != MPI_CONGRUENT) ) {
		continue;
	    }
	    
	    found = i;
	    if ( ( er->er_nneighbors == nneighbors ) && 
		 ( er->er_vndims     == vndims     ) && 
		 ( er->er_vnc        == vnc        ) && 
		 ( er->er_vhwidth    == vhwidth    ) ) {
		
		for ( j=0; j< er->er_nneighbors; j++ ) {
		    if ( er->er_neighbors[i] != neighbors [i] ) {
			found = -1;
			break;
		    }
		}
		if ( found == -1 ) {
		    continue;
		}
		for ( j=0 ; j< er->er_vndims; j++ ){
		    if ( er->er_vdims[i] != vdims[i] ) {
			found = -1;
			break;
		    }
		}
		if ( found != -1 ) {
		    break;
		}
	    }
	}
	
	if ( found > -1 ) {
	    er->er_rfcnt++;
	    return er;
	}
    }
	
    /* we did not find this configuraion yet, so we have to add it */
    er = ( ADCL_emethod_req_t *) calloc (1, sizeof(ADCL_emethod_req_t));
    if ( NULL == er ) {
	return NULL;
    }

    er->er_comm           = comm;
    er->er_nneighbors     = nneighbors;
    er->er_vndims         = vndims;
    er->er_rfcnt          = 1;
    er->er_state          = ADCL_STATE_TESTING;
    er->er_neighbors      = (int *) malloc ( nneighbors * sizeof(int));
    er->er_vdims          = (int *) malloc ( vndims * sizeof(int));
    
    for(i=0; i<ADCL_ATTR_TOTAL_NUM; i++){
      er->er_attr_hypothesis[i]= ADCL_ATTR_NOT_SET;
    }
    
    memset(er->er_attr_confidence, 0, ADCL_ATTR_TOTAL_NUM * sizeof(short));
    er->er_num_avail_meas = 0;

    if ( NULL == er->er_neighbors || NULL == er->er_vdims ) {
	free ( er );
	return NULL;
    }
    memcpy ( er->er_neighbors, neighbors, nneighbors * sizeof(int));
    memcpy ( er->er_vdims, vdims, vndims * sizeof(int));
    er->er_vnc     = vnc;
    er->er_vhwidth = vhwidth;

    er->er_num_emethods = ADCL_get_num_methods ();
    er->er_emethods     = (ADCL_emethod_t*) calloc ( 1, er->er_num_emethods *
						     sizeof(ADCL_emethod_t));
    if ( NULL == er->er_emethods ) {
	free ( er->er_vdims ) ;
	free ( er->er_neighbors );
	free ( er );
	return NULL;
    }

    for ( i=0; i< er->er_num_emethods; i++ ) {
	er->er_emethods[i].em_time   = (TIME_TYPE *) calloc (1, 
							     ADCL_emethod_numtests 
							     * sizeof(TIME_TYPE));
	  er->er_emethods[i].em_id     = ADCL_emethod_get_next_id ();
	  er->er_emethods[i].em_method = ADCL_get_method(i);
	  er->er_emethods[i].em_flags  = 0;
    }

    er->er_num_active_attrs=2;
    er->er_active_attr_list[0]=ADCL_ATTR_MAPPING;
    er->er_active_attr_list[1]=ADCL_ATTR_NONCONT;

    ADCL_array_get_next_free_pos ( ADCL_emethod_req_array, &er->er_pos );
    ADCL_array_set_element ( ADCL_emethod_req_array, 
			     er->er_pos, 
			     er->er_pos,
			     er );    

    if ( -1 != ADCL_emethod_selection ) {
	er->er_state = ADCL_STATE_REGULAR;
	er->er_wmethod = ADCL_emethod_get_method ( er, ADCL_emethod_selection );
    }

    return er;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
void ADCL_emethod_free ( ADCL_emethod_req_t * er )
{
    int i;

    er->er_rfcnt--;
    if ( er->er_rfcnt == 0 ) {
	for ( i=0; i< er->er_num_emethods; i++ ) {
	    if ( NULL != er->er_emethods[i].em_time ) {
		free ( er->er_emethods[i].em_time );
	    }
	}
	if ( NULL != er->er_vdims  ) {
	    free ( er->er_vdims);
	}
	if ( NULL != er->er_neighbors ) {
	    free ( er->er_neighbors );
	}
	if ( NULL != er->er_emethods ) {
	    free ( er->er_emethods );
	}
	ADCL_array_remove_element ( ADCL_emethod_req_array, er->er_pos );
	free ( er );
    }

    return;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
double ADCL_emethod_time (void) 
{ 
    struct timeval tp; 
    gettimeofday (&tp, NULL); 
    return tp.tv_usec;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_emethod_get_next_id (void)
{
    return ADCL_local_id_counter++;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
ADCL_method_t* ADCL_emethod_get_method ( ADCL_emethod_req_t *erm, int pos)
{
    return erm->er_emethods[pos].em_method;
}
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
ADCL_emethod_t* ADCL_emethod_get_by_attrs ( ADCL_emethod_req_t *erm, 
					    int *attr)
{
  int i, j, found;
  ADCL_emethod_t* tmp_emethod, *result=NULL;

  for ( i=0; i<erm->er_num_emethods; i++ ) {
    tmp_emethod = &(erm->er_emethods[i]);
    for ( found=1, j=0; j<ADCL_ATTR_TOTAL_NUM; j++ ){
      if ( tmp_emethod->em_method->m_attr[j] != attr[j] ) {
	found = 0; /* false */
	break;
      }
    }
    if ( found ) {
      result = tmp_emethod;
      break;
    }
  }
	 
  
  return result;
}

    
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_emethod_monitor ( ADCL_emethod_req_t *ermethod, int pos, 
			   TIME_TYPE tstart, TIME_TYPE tend )
{
    /* to be done later */
    return ADCL_STATE_REGULAR;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_emethods_get_winner (ADCL_emethod_req_t *ermethod, MPI_Comm comm)
{
    int i, winner, rank;
    ADCL_emethod_t **er=NULL;
    
    er = (ADCL_emethod_t **) malloc ( ermethod->er_num_emethods * 
				      sizeof(ADCL_emethod_t *));
    if ( NULL == er ) {
	return ADCL_NO_MEMORY;
    }
    for ( i=0; i<ermethod->er_num_emethods; i++ ) {
	er[i] = &(ermethod->er_emethods[i]);
    }

    MPI_Comm_rank ( ermethod->er_comm, &rank );

    /* 
    ** Filter the input data, i.e. remove outliers which 
    ** would falsify the results 
    */
    ADCL_statistics_filter_timings ( er,
				     ermethod->er_num_emethods, 
				     rank );

    /* 
    ** Weight now the performance of each method 
    */
    ADCL_statistics_determine_votes ( er,
				      ermethod->er_num_emethods, 
				      rank );

    /* 
    ** Determine now how many point each method achieved globally. The
    ** method with the largest number of points will be the chosen one.
    */
    ADCL_statistics_global_max ( er,
				 ermethod->er_num_emethods, 
				 ermethod->er_comm, 1, 
				 &(ermethod->er_num_emethods), 
				 &winner, rank);

    free ( er );
    return winner;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_emethods_get_next ( ADCL_emethod_req_t *er, int mode, int *flag )
{
    int i, next=ADCL_EVAL_DONE;
    ADCL_emethod_t *emethod;

    emethod = &(er->er_emethods[er->er_last]);
    if ( emethod->em_count < ADCL_emethod_numtests ) {
        *flag = ADCL_FLAG_PERF;
        emethod->em_count++;
        return er->er_last;
    }
    
    ADCL_EM_SET_TESTED (emethod);
    MPI_Barrier ( er->er_comm );
    er->er_num_avail_meas++;
    if ( emethod->em_rescount < ADCL_emethod_numtests ) {
        /* 
        ** ok, some data is still outstanding. So we 
        ** do not switch yet to the evaluation mode, 
        ** we return the last method with the noperf flag 
        ** (= performance data not relevant for evaluation)
        */
        *flag = ADCL_FLAG_NOPERF;
        return er->er_last;
    }

    if ( ADCL_emethod_use_perfhypothesis ) {
	ADCL_hypothesis_eval_v2 ( er );
    }
    
    for ( er->er_num_avail_meas=0,i=0;i<er->er_num_emethods;i++){
	/* increase er_num_available_measurements every time 
	   a method has the em_tested flag set to true; */
	if ( !(ADCL_EM_IS_TESTED (&(er->er_emethods[i])))  ) {
	    next = i;
	    er->er_last=next;
	    er->er_emethods[next].em_count++;
	    break;
	}
	er->er_num_avail_meas++;
    }
    
    *flag = ADCL_FLAG_PERF;
    return next;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
void ADCL_emethods_update ( ADCL_emethod_req_t *ermethods, int pos, int flag, 
			    TIME_TYPE tstart, TIME_TYPE tend )
{
    ADCL_emethod_t *emethods = ermethods->er_emethods;
    ADCL_emethod_t *tmpem;
    TIME_TYPE exectime;
    
    ADCL_EMETHOD_TIMEDIFF ( tstart, tend, exectime );
    if ( flag == ADCL_FLAG_PERF ) {
	tmpem = &emethods[pos];
	tmpem->em_time[tmpem->em_rescount] = exectime;
	tmpem->em_rescount++;
    }

    return;
}
