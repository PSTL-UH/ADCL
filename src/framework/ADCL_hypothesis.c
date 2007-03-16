#include "ADCL_internal.h"

int ADCL_hypo_req_confidence=2;

static int next_attr_combination ( int cnt, ADCL_attrset_t *attrset, 
				   ADCL_attribute_t  **attr_list, int *attr_val_list );
static int get_num_methods_and_blocks ( ADCL_emethod_t *e, int *nummethods, 
					int *numblocks);

extern ADCL_attribute_t *ADCL_neighborhood_attrs[ADCL_ATTR_TOTAL_NUM];


int ADCL_hypothesis_shrinklist_byattr ( ADCL_emethod_t *e,
                                        int attr_pos, int required_value )
{
    int i, count=0;
    ADCL_fnctset_t *fnctset=&(e->em_fnctset);
    int org_count=fnctset->fs_maxnum;
    ADCL_attrset_t *attrset=fnctset->fs_attrset;
    ADCL_function_t *tfunc=NULL;


    /* This following loop is only for debugging purposes and should be removed
       from later production runs */
    for ( i=0; i < org_count; i++ ) {
	if ( fnctset->fs_fptrs[i]->f_attrvals[attr_pos] != required_value ) {
	    ADCL_printf("#Removing function %d for attr %d required %d is %d\n",
			fnctset->fs_fptrs[i]->f_id, attrset->as_attrs[attr_pos]->a_id, 
			required_value, fnctset->fs_fptrs[i]->f_attrvals[attr_pos]);
	}
    }

    for ( i=0; i < org_count; i++ ) {
        tfunc = fnctset->fs_fptrs[i];
	if ( tfunc->f_attrvals[attr_pos] == required_value ) {
            if ( count != i ) {
		/* move this emethod from pos i to pos count */
		fnctset->fs_fptrs[count] = fnctset->fs_fptrs[i];
		
		/* do the same with the statisticst objects */
		e->em_stats[count] = e->em_stats[i];
	    }
	    count++;
	}
    }
    fnctset->fs_maxnum = count;
    ADCL_printf("#Fnctset has been shrinked from %d to %d entries\n", 
		org_count, count );


    /* Finally, adjust the attribute list and the attrset to the new values */
    attrset->as_attrs[attr_pos]->a_maxnvalues =  1;
    attrset->as_attrs[attr_pos]->a_values[0]  =  required_value;
	
    attrset->as_attrs_baseval[attr_pos] = required_value;
    attrset->as_attrs_maxval[attr_pos]  = required_value;
    attrset->as_attrs_numval[attr_pos]  = 1;
        
    return ADCL_SUCCESS;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_hypothesis_set ( ADCL_emethod_t *e, int attrpos, int attrval ) 
{
    ADCL_hypothesis_t *hypo = &(e->em_hypo);
  
  if ( hypo->h_attr_hypothesis[attrpos] == ADCL_ATTR_NOT_SET ) {
    hypo->h_attr_hypothesis[attrpos] = attrval;
    hypo->h_attr_confidence[attrpos] = 1;
    ADCL_printf("#Hypothesis for attr %d set to %d, confidence"
		" %d\n", attrpos, attrval, hypo->h_attr_confidence[attrpos]);
  } 
  else if ( attrval == hypo->h_attr_hypothesis[attrpos] ) {
    hypo->h_attr_confidence[attrpos]++;
    ADCL_printf("#Hypothesis for attr %d is %d, confidence "
		"incr to %d\n", attrpos, attrval, hypo->h_attr_confidence[attrpos]);
  }
  else {
    hypo->h_attr_confidence[attrpos]--;
    ADCL_printf("#Hypothesis for attr %d is %d, confidence "
		"decr to %d\n", attrpos, hypo->h_attr_hypothesis[attrpos], 
		hypo->h_attr_confidence[attrpos]);
    if ( hypo->h_attr_confidence[attrpos] == 0 ) {
      /* we don't have a performance hypthesis for this attribute anymore */
	hypo->h_attr_hypothesis[attrpos] = ADCL_ATTR_NOT_SET;
    }
  }
  
  return ADCL_SUCCESS;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/* General description what this algorithm is doing.
 * Goal: this algorithm is supposed to evaluate the performance 
 *       of different emethods based on the attributes. The main
 *       difference to v1 are that the evaluation is not done
 *       every time an emethod has been finished with testing,
 *       but every time all available values for a given attribute
 *       has been evaluated. Example: attr[0] has three possible
 *       values (a,b,c). Three methods with attr[0...n]={a,...},
 *       {b,...},{c,...} with ... being an arbitrary but constant
 *       list of all other attributes. A comparison of all
 *       three methods will lead to a performance hypothesis
 *       for attr[0] and increase/decrease the confidence value
 *       by one. (This approach avoids the unhandled case in 
 *       v1 and is more consistent. It also reduces the number
 *       of additional collective operations).
 *
 *       The second goal is to 'postpone' the evaluation until a 
 *       given number of attributes have been evaluated. Thus, if
 *       if num_active_attr is 2, with current_active_attr[0]=attr[0],
 *       and current_active_attr[1]=attr[1], all methods providing 
 *       all possible combination of attr[0] and attr[1] has to be finished.
 *       This coalloscing of attributes further reduces the communication,
 *       reduces potentially the number of emethods having to be evaluated
 *       but delays the shrinking of the emethods list.
 * 
 * Implementation:
 */


int ADCL_hypothesis_eval_v2 ( ADCL_emethod_t *e )
{
    int numblocks=0, nummethods=0;
    int cnt=0, blencnt=0, bcnt=0;
    int *winners=NULL, *blength=NULL; 
    int i, tval, loop, ret = ADCL_SUCCESS;
    int *attrval_list=NULL;
    int *attr_pos=NULL;
    int pos;

    ADCL_statistics_t  **tmp_stats=NULL;
    ADCL_function_t  **tmp_fncts=NULL;
    ADCL_attribute_t **tmp_active_attr_list=NULL;
    ADCL_hypothesis_t *hypo=&(e->em_hypo);
    ADCL_attrset_t *attrset=e->em_fnctset.fs_attrset;


    /* 
    ** Please note: the attrval_list is always in the same order 
    ** as the attributes in the attribute set. This is in contrary
    ** to the tmp_active_attr_list
    */
    attrval_list         = (int *) malloc ( attrset->as_maxnum * sizeof(int)); 
    tmp_active_attr_list = (ADCL_attribute_t **) malloc ( attrset->as_maxnum * 
							  sizeof(ADCL_attribute_t *));
    if ( NULL == attrval_list || NULL == tmp_active_attr_list ) {
	return ADCL_NO_MEMORY;
    }
    
    /* 
     * Determine how many blocks we will generate and how many individual
     * methods will be in the final list. 
     */
    get_num_methods_and_blocks ( e, &nummethods, &numblocks);
    
    tmp_stats = (ADCL_statistics_t **) malloc (nummethods * sizeof(ADCL_statistics_t*)); 
    tmp_fncts = (ADCL_function_t **)   malloc (nummethods * sizeof(ADCL_function_t*)); 
    blength = (int *) calloc ( 1, 3 * numblocks * sizeof(int));
    if ( NULL==tmp_stats || NULL== tmp_fncts || NULL == blength ){
	free ( attrval_list );
	free ( tmp_active_attr_list );
	return ADCL_NO_MEMORY;
    }
    winners  = &(blength[numblocks]);
    attr_pos = &(blength[2*numblocks]);
    
    
    /* 
     * main loop over the number of attributes currently investigated. 
     * Within this loop we generate the list of emethods, which 
     * we will pass to the evaluation function. 
     */
    for ( loop=0; loop<hypo->h_num_active_attrs; loop++ ){
	
	/* 
	 * The attribute currently investigated has to be on the top 
	 * of the list for the next_attr_combination function.  Thus
	 * generate for every 'loop' loop iterations a new list of
	 * attributes moving the according element to the top of the list 
	 */
	
	for ( i=0; i< hypo->h_num_active_attrs; i++ ) {
	    pos = (i+loop)%hypo->h_num_active_attrs;
	    tmp_active_attr_list[i] = hypo->h_active_attr_list[pos];
	}
	
	
	/* Generate the first combination of attributes. The first
	   one is really independent of the list of attributes used. 
	*/
	for ( i = 0; i < attrset->as_maxnum; i++ ) {
	    attrval_list[i] = attrset->as_attrs_baseval[i];
	}
	
	pos = ADCL_attrset_get_pos ( attrset, hypo->h_active_attr_list[loop]);
	attr_pos[bcnt] = pos;
	
	ret = ADCL_SUCCESS;
	while ( ret != ADCL_EVAL_DONE ) {
	    ADCL_emethod_get_stats_by_attrs ( e, attrval_list, 
					      &tmp_stats[cnt], 
					      &tmp_fncts[cnt]); 
	    cnt++;
	    blencnt++;
	    
	    ret = next_attr_combination(hypo->h_num_active_attrs, 
					attrset,
					tmp_active_attr_list, 
					attrval_list);
	    if ( ret != ADCL_SUCCESS ) {
		blength[bcnt++] = blencnt;
		blencnt         = 0;
		if ( ret != ADCL_EVAL_DONE ) {
		    attr_pos[bcnt]  = pos;
		}
	    }
	}
    }
    
    
    /* Determine now the winners across all provided measurement lists */
    ADCL_statistics_filter_timings  ( tmp_stats, nummethods, e->em_topo->t_rank);
    ADCL_statistics_global_max ( tmp_stats, nummethods, e->em_topo->t_comm, 
				 numblocks, blength, winners, e->em_topo->t_rank);
    
    /* Set the according performance hypothesis */
    for (i=0; i< numblocks; i++ ) {
	tval = ADCL_function_get_attrval ( tmp_fncts[winners[i]], attr_pos[i]);
	ADCL_hypothesis_set ( e, attr_pos[i], tval );
    }
    
    /* Now the shrink the emethods list if we reached a threshold */
    for ( i=0; i< numblocks; i++ ){
	if ( hypo->h_attr_confidence[attr_pos[i]] >= ADCL_hypo_req_confidence ) {
	    ADCL_hypothesis_shrinklist_byattr( e, attr_pos[i], 
					       hypo->h_attr_hypothesis[attr_pos[i]]);
	    hypo->h_attr_confidence[attr_pos[i]]=0;
	    
	    /* TBD: we will have to adapt here the number of attributes for pattrs[i]
	     *      as well as the attr_base value!!! This has to  be done on a 
	     *      per emethods_req basis 
	     */
	}
    }
    
    /* Switch to the next list of attributes to be evaluated */
    hypo->h_num_active_attrs  = 1;
    hypo->h_active_attr_list[0] = e->em_fnctset.fs_attrset->as_attrs[2];
    /* hypo->h_num_required_meas = e->em_fnctset.fs_attrset->as_attrs_numval[2]+1;*/ 
    hypo->h_num_required_meas = 1000;    /* don't do that right now */

    free ( tmp_active_attr_list );
    free ( tmp_stats );
    free ( tmp_fncts );
    free ( blength );

return ADCL_SUCCESS;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

/* cnt:           number of currently handled attributes 
 *                Please note: cnt does not have to be equal to attrset->as_maxnum;
 * attrset:       attribute set which we are working with.
 * attr_list:     list of attributes currently being handled. An array of length cnt.
 *                Please note: the order of the attributes here might not be equal
 *                to the order of the attributes in attrset or in the hypo->h_active_attr_list;
 * attr_val_list: array of dimension ADCL_ATTR_TOTAL_NUM containg the last used 
 *                combination of attributes 
 */

static int next_attr_combination ( int cnt, ADCL_attrset_t *attrset, 
				   ADCL_attribute_t  **attr_list, int *attr_val_list )
{
    int i, ret=ADCL_SUCCESS;
    int thisval, thispos;
    ADCL_attribute_t *thisattr;

    for ( i = 0; i < cnt; i++ ) {
	thisattr = attr_list[i];
	thispos = ADCL_attrset_get_pos ( attrset, thisattr );
	thisval = attr_val_list[thispos];
	
	if ( thisval < attrset->as_attrs_maxval[thispos] ) {
	    attr_val_list[thispos] = ADCL_attribute_get_nextval (thisattr, thisval);
	    return ret;
	}
	else if ( thisval == attrset->as_attrs_maxval[thispos] ){
	    attr_val_list[thispos] = attrset->as_attrs_baseval[thispos];
	    ret = ADCL_ATTR_NEW_BLOCK;
	}
	else {
	    /* BUg, should not happen */
	}
    }
    
    return ADCL_EVAL_DONE;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
static int get_num_methods_and_blocks ( ADCL_emethod_t *e, int *nummethods, 
					int *numblocks)
{
    int tmpnblocks, i, j;
    int nm, nb=0;
    ADCL_hypothesis_t *hypo=&(e->em_hypo);

    for ( nm=hypo->h_num_active_attrs, i=0; i<hypo->h_num_active_attrs; i++) {
	for ( tmpnblocks=1, j=0; j < hypo->h_num_active_attrs; j++ ) {
	    if ( j == i ) {
		continue;
	    }
	    tmpnblocks *= hypo->h_active_attr_list[j]->a_maxnvalues;
	}
	nb += tmpnblocks;
	nm *= hypo->h_active_attr_list[i]->a_maxnvalues;
    }

    *nummethods = nm;
    *numblocks  = nb;
    
    return ADCL_SUCCESS;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
#ifdef V3
/* 
** This routines takes the statistical data of functions which have been already
** tested, but which did not yet go through the evaluation proces. 
*/
int ADCL_hypothesis_eval_meas_series ( ADCL_emethod_t *e, int nummethods )
{
    ADCL_hypothesis_t *hypo=&(e->em_hypo);
    ADCL_statistics_t **tmp_stats;
    int i;

    tmp_stats = (ADCL_statistics_t **) malloc (nummethods * sizeof(ADCL_statistics_t*)); 
    if ( NULL==tmp_stats ) {
	return ADCL_NO_MEMORY;
    }

    for ( i=0; i<nummethods; i++ ) {
      tmp_stats[i] = e->em_stats[hypo->h_noneval[i]];
    }

    /* Determine now the winners across all provided measurement lists */
    ADCL_statistics_filter_timings  ( tmp_stats, nummethods, e->em_topo->t_rank);
    ADCL_statistics_global_max_v3 ( tmp_stats, nummethods, e->em_topo->t_comm, 
				    e->em_topo->t_rank );

    /* Clear the list of not evaluated functions on the performance hypothesis
       object */
    for ( i=0; i < e->em_fnctset->fs_maxnum; i++ ) {
      hypo->h_noneval[i] = -1;
    }

    return ADCL_SUCCESS;
}
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
int ADCL_hypothesis_eval_one_attr ( ADCL_emethod_t *e,  ) 
{

  return ADCL_SUCCESS;
}
#endif
