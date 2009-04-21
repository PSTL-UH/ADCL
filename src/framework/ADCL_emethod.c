/*
 * Copyright (c) 2006-2007      University of Houston. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include "ADCL_internal.h"

static int ADCL_local_id_counter = 0;
ADCL_array_t *ADCL_emethod_array = NULL;

int ADCL_emethod_selection = -1;
int ADCL_merge_requests = 1;
int ADCL_emethod_numtests = ADCL_EMETHOD_NUMTESTS;
int ADCL_emethod_use_perfhypothesis = 0; /* false */
int ADCL_emethod_learn_from_hist = 0;

#define ADCL_ATTR_TOTAL_NUM 3
extern ADCL_attribute_t *ADCL_neighborhood_attrs[ADCL_ATTR_TOTAL_NUM];

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
ADCL_emethod_t *ADCL_emethod_init (ADCL_topology_t *t, ADCL_vector_t *v,
                   ADCL_fnctset_t *f )

{
    ADCL_emethod_t *e = NULL;
    ADCL_hypothesis_t *hypo = NULL;

    int i, ret=ADCL_SUCCESS;

    if ( ADCL_merge_requests && v != ADCL_VECTOR_NULL ) {
       int j, last, found=-1;
       int result;
       ADCL_topology_t *topo;
       ADCL_vector_t *vec;
       ADCL_vmap_t *vec_map, *v_map;

       /* Check first, whether we have an entry in the ADCL_emethods_array,
          which fulfills already our requirements;
       */
       last = ADCL_array_get_last ( ADCL_emethod_array );
       for ( i=0; i<= last; i++ ) {
          e = ( ADCL_emethod_t * ) ADCL_array_get_ptr_by_pos (
          ADCL_emethod_array, i );
          topo = e->em_topo;
          vec  = e->em_vec;
          if ( ADCL_VECTOR_NULL == vec  ) {
              continue;
          }

          MPI_Comm_compare ( topo->t_comm, t->t_comm, &result );
          if ( ( result != MPI_IDENT) && (result != MPI_CONGRUENT) ) {
              continue;
          }
          vec_map = vec->v_map;
	  v_map   = v->v_map;

          if ( ( e->em_orgfnctset != f )          ||
               ( topo->t_ndims   != t->t_ndims  ) ||
               ( vec->v_ndims    != v->v_ndims  ) ||
               ( vec->v_nc       != v->v_nc     ) ||
               ( vec_map->m_vectype != v_map->m_vectype ) ) {
	       continue;
          }

          for ( j=0 ; j<vec->v_ndims; j++ ){
             if ( vec->v_dims[j] != v->v_dims[j] ) {
                goto nextemethod;
             }
	  }

          switch (vec_map->m_vectype) {
	  case ADCL_VECTOR_HALO:
             if ( vec_map->m_hwidth != v_map->m_hwidth  ) {
	        continue;
	     }

             for ( j=0; j< (2*topo->t_ndims); j++ ) {
                if ( topo->t_neighbors[j] != t->t_neighbors [j] ) {
                   goto nextemethod;
	        }
             }
	     break;
	  case ADCL_VECTOR_LIST:
             for ( j=0; j<topo->t_size; j++){
                if ((vec_map->m_rcnts[j] != v_map->m_rcnts[j]) || 
	           (vec_map->m_displ[j] != v_map->m_rcnts[j])) {
	          goto nextemethod;
	        }
             }
	     break;
	  }

          /* all tests OK */
          found = i;
          break;
nextemethod:
          continue;
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
    e->em_explored_hist = -2;
    e->em_topo        = t;
    e->em_vec         = v;
    if ( NULL != v && ADCL_VECTOR_NULL != v) {
       ADCL_vector_add_reference(v);
    }
    e->em_orgfnctset  = f;

    /*
    ** Set the algorithm for the selection logic. Set it to the default value,
    ** if we have an attribute set. However, if no attributes are assigned to
    ** functionset, we have to use the brute force algorithm
    */
    if ( f->fs_attrset != NULL && f->fs_attrset != ADCL_ATTRSET_NULL ) {
        e->em_perfhypothesis = ADCL_emethod_use_perfhypothesis;
    }
    else {
        e->em_perfhypothesis = 0;
    }

    /*
    ** Generate a duplicate of the functions which we will work with.
    ** The reason is, that the list of functions etc. might be modified
    ** during the runtime optimization. We do not want to delete the original
    ** set, since it might be use by multiple requests/emehods *and*
    ** we might need the original list again when a re-evaluation has been
    ** initiated.
    */
    ADCL_fnctset_dup ( f, &(e->em_fnctset));
    ADCL_statistics_create ( &(e->em_stats), f->fs_maxnum ); 

    /* initiate the performance hypothesis structure */

    if ( e->em_perfhypothesis ) {
        ADCL_hypothesis_init ( e );
    }

    /* for verification runs */
    if ( 0 == strcmp ( f->fs_name , "Neighborhood communication") ) {
        if ( -1 != ADCL_emethod_selection ) {
            e->em_state = ADCL_STATE_REGULAR;
            e->em_wfunction = ADCL_emethod_get_function ( e, ADCL_emethod_selection );
        }
    }

    /* History list initialization */
    e->em_hist_list =  (ADCL_hist_list_t *)calloc(1, sizeof(ADCL_hist_list_t));
    if ( NULL ==  e->em_hist_list ) {
        ret = ADCL_NO_MEMORY;
        goto exit;
    }
    /* Initialize history entries count to 0 */
    e->em_hist_cnt = 0;
 exit:
    if ( ret != ADCL_SUCCESS  ) {
        ADCL_statistics_free ( &(e->em_stats), f->fs_maxnum );

        if ( NULL != hypo->h_attr_hypothesis ) {
            free ( hypo->h_attr_hypothesis );
        }
        if ( NULL != hypo->h_attr_confidence ) {
            free ( hypo->h_attr_confidence );
        }
        if ( NULL != hypo->h_curr_attrvals ) {
            free ( hypo->h_curr_attrvals );
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
    int i;
    e->em_rfcnt--;
    if ( e->em_rfcnt == 0 ) {
        ADCL_hypothesis_t *hypo = &(e->em_hypo);

        if ( NULL != e->em_stats  ) {
            for ( i=0; i< e->em_fnctset.fs_maxnum; i++ ) {
                if ( NULL != e->em_stats[i] ) {
                    if ( NULL != e->em_stats[i]->s_time ) {
                        free ( e->em_stats[i]->s_time );
                    }
                    free ( e->em_stats[i] );
                }
            }
            free ( e->em_stats );
        }

        ADCL_vector_free(&(e->em_vec));

        if ( NULL != hypo->h_attr_hypothesis ) {
            free ( hypo->h_attr_hypothesis );
        }
        if ( NULL != hypo->h_attr_confidence ) {
            free ( hypo->h_attr_confidence );
        }
        if ( NULL != hypo->h_curr_attrvals ) {
            free ( hypo->h_curr_attrvals );
        }
        if ( NULL != e->em_hist_criteria ) {   
            if ( NULL != e->em_hist_criteria->hc_filter_criteria ){
                free( e->em_hist_criteria->hc_filter_criteria );
            }
            free(e->em_hist_criteria);
        }


/* TBD
        hist_list = e->em_hist_list;
        do {
            ADCL_hist_free(hist_list->hl_curr);
            hist_list = hist_list->hl_next;
        } while (NULL != hist_list);
*/
        ADCL_array_remove_element ( ADCL_emethod_array, e->em_findex );
        free ( e );
    }

    return;
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
int ADCL_emethod_get_function_by_attrs ( ADCL_emethod_t *em,
                     int *attrval,
                     int *pos )
{
    int i, j, found;
    ADCL_fnctset_t* fnctset;
    int ret=ADCL_NOT_FOUND;

    fnctset = &(em->em_fnctset);
    for ( i=0; i< fnctset->fs_maxnum; i++ ) {
        for ( found=1, j=0; j<fnctset->fs_attrset->as_maxnum; j++ ){
            if ( fnctset->fs_fptrs[i]->f_attrvals[j] != attrval[j] ) {
                found = 0; /* false */
                break;
            }
        }
        if ( found ) {
            *pos = i;
            ret = ADCL_SUCCESS;
            break;
        }
    }
    return ret;
}
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_emethod_get_stats_by_attrs ( ADCL_emethod_t *em, int *attrval,
                      ADCL_statistics_t **stat,
                      ADCL_function_t **func )
{
  int i, j, found=0;
  ADCL_fnctset_t* fnctset;

  *stat = NULL;
  *func = NULL;

  fnctset = &(em->em_fnctset);
  for ( i=0; i< fnctset->fs_maxnum; i++ ) {
      for ( found=1, j=0; j<fnctset->fs_attrset->as_maxnum; j++ ) {
        if ( fnctset->fs_fptrs[i]->f_attrvals[j] != attrval[j] ) {
            found = 0; /* false */
            break;
        }
      }
      if ( found ) {
        *stat = em->em_stats[i];
        *func = fnctset->fs_fptrs[i];
        break;
      }
  }

  if ( !found ) {
      return ADCL_NOT_FOUND;
  }

  return ADCL_SUCCESS;
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
int ADCL_emethods_get_winner (ADCL_emethod_t *emethod, MPI_Comm comm, int count)
{
    int winner;

    /*
    ** Filter the input data, i.e. remove outliers which
    ** would falsify the results
    */
    ADCL_statistics_filter_timings ( emethod->em_stats, count,
                                     emethod->em_topo->t_rank );

    /*
    ** Determine now how many point each method achieved globally. The
    ** method with the largest number of points will be the chosen one.
    */
    if ( 0 == emethod->em_perfhypothesis ) {
        ADCL_statistics_global_max_v3 ( emethod->em_stats, count,
                                        emethod->em_topo->t_comm,
                                        emethod->em_topo->t_rank);
    }

    emethod->em_filtering = ADCL_statistics_get_winner_v3 ( emethod->em_stats,
                                                           count, &winner );

    return winner;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_emethods_get_next ( ADCL_emethod_t *e, int *flag )
{
    int next = ADCL_EVAL_DONE;
    int last = e->em_last;
    int hist_search_res;
    ADCL_hist_t *hist;
    ADCL_function_t *func;
    TIME_TYPE start, end;

    if ( 1 == ADCL_emethod_learn_from_hist ) {
        /* Search for solution/hints in the hist stored from previous runs */
        start = TIME;
        hist_search_res = ADCL_hist_find( e, &hist );
	end = TIME;
    }
    else {
        hist_search_res = ADCL_UNEQUAL;
    }
    switch (hist_search_res) {
        case ADCL_IDENT:
            func = e->em_orgfnctset->fs_fptrs[hist->h_wfnum];
            if ( ADCL_FUNCTION_NULL != func ) {
                e->em_state = ADCL_STATE_REGULAR;
                e->em_wfunction = func;
                return ADCL_SOL_FOUND;
            }
            else {
                ADCL_printf("Function %s is not found in the function set\n", hist->h_wfname );
            }
            break;
        case ADCL_SIMILAR:
#ifdef HL_VERBOSE
                if(0 == e->em_topo->t_rank) {
                    printf("Time for prediction: %f us\n", end-start);
                }
#endif
                /* one idea is to increment the confidance number for 
                   attributes values of the winning function */
                e->em_state = ADCL_STATE_REGULAR;
                e->em_wfunction = e->em_orgfnctset->fs_fptrs[e->em_hist->h_wfnum];
                /* TO BE MONITORED for discarding in case bad results */
                return ADCL_SOL_FOUND;
            break;
        case ADCL_UNEQUAL:
        default:
            break;
    }
    if ( e->em_stats[last]->s_count < ADCL_emethod_numtests ) {
        *flag = ADCL_FLAG_PERF;
        e->em_stats[last]->s_count++;
        return last;
    }

    if ( e->em_stats[last]->s_rescount < ADCL_emethod_numtests ) {
        /*
        ** ok, some data is still outstanding. So we
        ** do not switch yet to the evaluation mode,
        ** we return the last method with the noperf flag
        ** (= performance data not relevant for evaluation)
        */
        *flag = ADCL_FLAG_NOPERF;
        return last;
    }

    ADCL_STAT_SET_TESTED ( e->em_stats[last]);
    ADCL_statistics_filter_timings ( &(e->em_stats[last]), 1,
                                     e->em_topo->t_rank );

    if ( e->em_perfhypothesis ) {
        ADCL_statistics_global_max_v3 ( &(e->em_stats[last]), 1,
                                        e->em_topo->t_comm,
                                        e->em_topo->t_rank );
        next = ADCL_hypothesis_get_next ( e );
        if ( next != ADCL_EVAL_DONE ) {
            e->em_last=next;
            e->em_stats[next]->s_count++;
        }
    }
    else {
        if ( last < ( e->em_fnctset.fs_maxnum -1 ) ) {
            next = last+1;
            e->em_last = next;
            e->em_stats[next]->s_count++;
        }
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
        stat = emethod->em_stats[pos];
        stat->s_time[stat->s_rescount] = exectime;
        stat->s_rescount++;
    }

    return;
}
