#include "ADCL_internal.h"

static int ADCL_local_id_counter=0;
static ADCL_array_t *ADCL_emethod_array=NULL;

int ADCL_emethod_selection = -1;
int ADCL_merge_requests=1;
int ADCL_emethod_numtests=ADCL_EMETHOD_NUMTESTS;
int ADCL_emethod_use_perfhypothesis=0; /* false */

#define ADCL_ATTR_TOTAL_NUM 3
extern ADCL_attribute_t *ADCL_neighborhood_attrs[ADCL_ATTR_TOTAL_NUM];

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
ADCL_emethod_t *ADCL_emethod_init (ADCL_topology_t *t, ADCL_vector_t *v, 
				   ADCL_fnctset_t *f )

{
    ADCL_emethod_t *e=NULL;    
    ADCL_hypothesis_t *hypo=NULL;
    int i, ret=ADCL_SUCCESS;

    if ( ADCL_merge_requests ) {
	int j, last, found=-1;
	int result;
	ADCL_topology_t *topo;
	ADCL_vector_t *vec;
	
	/* Check first, whether we have an entry in the ADCL_emethods_array,
	   which fulfills already our requirements; 
	*/
	last = ADCL_array_get_last ( ADCL_emethod_array );
	for ( i=0; i<= last; i++ ) {
	    e = ( ADCL_emethod_t * ) ADCL_array_get_ptr_by_pos ( 
		ADCL_emethod_array, i );
	    topo = e->em_topo;
	    vec  = e->em_vec;

	    MPI_Comm_compare ( topo->t_comm, t->t_comm, &result );
	    if ( ( result != MPI_IDENT) && (result != MPI_CONGRUENT) ) {
		continue;
	    }
	    
	    found = i;
	    if ( ( e->em_orgfnctset == f )          &&
		 ( topo->t_ndims   == t->t_ndims  ) && 
		 ( vec->v_ndims    == v->v_ndims  ) && 
		 ( vec->v_nc       == v->v_nc     ) && 
		 ( vec->v_hwidth   == v->v_hwidth ) ) {
		
		for ( j=0; j< (2*topo->t_ndims); j++ ) {
		    if ( topo->t_neighbors[i] != t->t_neighbors [i] ) {
			found = -1;
			break;
		    }
		}
		if ( found == -1 ) {
		    continue;
		}
		for ( j=0 ; j< vec->v_ndims; j++ ){
		    if ( vec->v_dims[i] != v->v_dims[i] ) {
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
	    e->em_rfcnt++;
	    return e;
	}
    }
	
    /* we did not find this configuraion yet, so we have to add it */
    e = ( ADCL_emethod_t *) calloc (1, sizeof(ADCL_emethod_t));
    if ( NULL == e ) {
	return NULL;
    }

    ADCL_array_get_next_free_pos ( ADCL_emethod_array, &e->em_findex );
    ADCL_array_set_element ( ADCL_emethod_array, e->em_findex, 
			     e->em_id, e );    

    e->em_id          = ADCL_local_id_counter++;
    e->em_rfcnt       = 1;
    e->em_state       = ADCL_STATE_TESTING;
    e->em_topo        = t;
    e->em_vec         = v;
    e->em_orgfnctset  = f;

    /* 
    ** Generate a duplicate of the functions which we will work with.
    ** The reason is, that the list of functions etc. might be modified
    ** during the runtime optimization. We do not want to delete the original
    ** set, since it might be use by multiple requests/emehods *and*
    ** we might need the original list again when a re-evaluation has been 
    ** initiated. 
    */
    ADCL_fnctset_dup ( f, &(e->em_fnctset));

    /* Allocate the according number of statitics objects */
    e->em_stats = (ADCL_statistics_t *) calloc ( 1, f->fs_maxnum*sizeof(ADCL_statistics_t));
    if ( NULL == e->em_stats ) {
	ret = ADCL_NO_MEMORY;
	goto exit;
    }
    
    /* initiate the performance hypothesis structure */
    hypo = &(e->em_hypo);
    hypo->h_num_avail_meas = 0;
    hypo->h_num_active_attrs = 0;
    hypo->h_attr_hypothesis  = (int *) calloc (1, f->fs_attrset->as_maxnum * sizeof(int));
    hypo->h_attr_confidence  = (int *) calloc (1, f->fs_attrset->as_maxnum * sizeof(int));
    hypo->h_active_attr_list = (ADCL_attribute_t **) calloc (1, f->fs_attrset->as_maxnum * sizeof(ADCL_attribute_t *));
    if ( NULL == hypo->h_attr_hypothesis ||
	 NULL == hypo->h_attr_confidence ||
	 NULL == hypo->h_active_attr_list ) {
	ret = ADCL_NO_MEMORY;
	goto exit;
    }

    /* Initialize the attribute list if we are dealing with a predefined functionset */
    if ( 0 == strcmp ( f->fs_name , "Neighborhood communication") ) {
	hypo->h_num_active_attrs = 2;
	hypo->h_active_attr_list[0] = ADCL_neighborhood_attrs[0]; /* ADCL_ATTR_MAPPING */
	hypo->h_active_attr_list[1] = ADCL_neighborhood_attrs[0]; /* ADCL_ATTR_NONCONT */
    }
    

//    if ( -1 != ADCL_emethod_selection ) {
//	er->er_state = ADCL_STATE_REGULAR;
//	er->er_wmethod = ADCL_emethod_get_method ( er, ADCL_emethod_selection );
//   }

 exit:
    if ( ret != ADCL_SUCCESS  ) {
	if ( NULL != e->em_stats  ) {
	    free ( e->em_stats );
	}
	if ( NULL != hypo->h_attr_hypothesis ) {
	    free ( hypo->h_attr_hypothesis );
	}
	if ( NULL != hypo->h_attr_confidence ) {
	    free ( hypo->h_attr_confidence );
	}
	if ( NULL != hypo->h_active_attr_list ) {
	    free ( hypo->h_active_attr_list );
	}

	ADCL_array_remove_element ( ADCL_emethod_array, e->em_findex );
	free ( e );
	e = NULL;
    }

    return e;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
void ADCL_emethod_free ( ADCL_emethod_t * e )
{

    e->em_rfcnt--;
    if ( e->em_rfcnt == 0 ) {
	ADCL_hypothesis_t *hypo = &(e->em_hypo);

	if ( NULL != e->em_stats  ) {
	    free ( e->em_stats );
	}

	if ( NULL != hypo->h_attr_hypothesis ) {
	    free ( hypo->h_attr_hypothesis );
	}
	if ( NULL != hypo->h_attr_confidence ) {
	    free ( hypo->h_attr_confidence );
	}
	if ( NULL != hypo->h_active_attr_list ) {
	    free ( hypo->h_active_attr_list );
	}

	ADCL_array_remove_element ( ADCL_emethod_array, e->em_findex );
	free ( e );
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
ADCL_function_t* ADCL_emethod_get_function ( ADCL_emethod_t *e, int pos)
{
    return e->em_fnctset.fs_fptrs[pos];
}
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
ADCL_function_t* ADCL_emethod_get_by_attrs ( ADCL_emethod_t *em, 
					     int *attrval)
{
  int i, j, found;
  ADCL_fnctset_t* fnctset;
  ADCL_function_t *result=NULL;

  fnctset = &(em->em_fnctset);
  for ( i=0; i< fnctset->fs_maxnum; i++ ) {
      for ( found=1, j=0; j<fnctset->fs_attrset->as_maxnum; j++ ){
	  if ( fnctset->fs_fptrs[i]->f_attrvals[j] != attrval[j] ) {
	      found = 0; /* false */
	      break;
	  }
      }
      if ( found ) {
	  result = fnctset->fs_fptrs[i];
	  break;
      }
  }
	 
  
  return result;
}

    
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_emethod_monitor ( ADCL_emethod_t *emethod, int pos, 
			   TIME_TYPE tstart, TIME_TYPE tend )
{
    /* to be done later */
    return ADCL_STATE_REGULAR;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_emethods_get_winner (ADCL_emethod_t *emethod, MPI_Comm comm)
{
    int winner, rank;

    MPI_Comm_rank ( emethod->em_topo->t_comm, &rank );

    /* 
    ** Filter the input data, i.e. remove outliers which 
    ** would falsify the results 
    */
    ADCL_statistics_filter_timings ( &(emethod->em_stats),
				     emethod->em_fnctset.fs_maxnum, 
				     rank );

    /* 
    ** Determine now how many point each method achieved globally. The
    ** method with the largest number of points will be the chosen one.
    */
    ADCL_statistics_global_max ( &(emethod->em_stats),
				 emethod->em_fnctset.fs_maxnum, 
				 emethod->em_topo->t_comm, 1, 
				 &(emethod->em_fnctset.fs_maxnum), 
				 &winner, rank);

    return winner;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_emethods_get_next ( ADCL_emethod_t *e, int mode, int *flag )
{
    int i, next=ADCL_EVAL_DONE;
    int last = e->em_last;
    ADCL_hypothesis_t *hypo=&(e->em_hypo);
    
    if ( e->em_stats[last].s_count < ADCL_emethod_numtests ) {
        *flag = ADCL_FLAG_PERF;
        e->em_stats[last].s_count++;
        return last;
    }
    
    ADCL_STAT_SET_TESTED (&(e->em_stats[last]));
    MPI_Barrier ( e->em_topo->t_comm );
    hypo->h_num_avail_meas++;
    if ( e->em_stats[last].s_rescount < ADCL_emethod_numtests ) {
        /* 
        ** ok, some data is still outstanding. So we 
        ** do not switch yet to the evaluation mode, 
        ** we return the last method with the noperf flag 
        ** (= performance data not relevant for evaluation)
        */
        *flag = ADCL_FLAG_NOPERF;
        return last;
    }

#if 0
    if ( ADCL_emethod_use_perfhypothesis ) {
	ADCL_hypothesis_eval_v2 ( e );
    }
    
#endif

    for ( hypo->h_num_avail_meas=0,i=0;i<e->em_fnctset.fs_maxnum;i++){
	/* increase er_num_available_measurements every time 
	   a method has the em_tested flag set to true; */
	if ( !(ADCL_STAT_IS_TESTED (&(e->em_stats[i])))  ) {
	    next = i;
	    e->em_last=next;
	    e->em_stats[next].s_count++;
	    break;
	}
	hypo->h_num_avail_meas++;
    }
    
    *flag = ADCL_FLAG_PERF;
    return next;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
void ADCL_emethods_update ( ADCL_emethod_t *emethod, int pos, int flag, 
			    TIME_TYPE tstart, TIME_TYPE tend )
{
    ADCL_statistics_t *stat;
    TIME_TYPE exectime;
    
    ADCL_STAT_TIMEDIFF ( tstart, tend, exectime );
    if ( flag == ADCL_FLAG_PERF ) {
	stat = &(emethod->em_stats[pos]);
	stat->s_time[stat->s_rescount] = exectime;
	stat->s_rescount++;
    }

    return;
}
