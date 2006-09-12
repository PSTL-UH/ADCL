#include "ADCL_internal.h"

static int next_attr_combination ( int cnt, int *attr_list, int *attr_val_list );
static int get_num_methods_and_blocks ( ADCL_emethod_req_t *er, int *nummethods, 
					int *numblocks);


int ADCL_hypothesis_c2m_attr ( ADCL_emethod_req_t *ermethods,
			       int *attr_list, int pos1, int pos2 )
{
    int i, num_diff=0;
    ADCL_emethod_t *emeth1, *emeth2;

    emeth1 = &(ermethods->er_emethods[pos1]);
    emeth2 = &(ermethods->er_emethods[pos2]);

    /* determine in how many attributes the two methods differ */
    for ( i=0; i < ADCL_ATTR_TOTAL_NUM; i++ ) {
	if ( emeth1->em_method->m_attr[i] != emeth2->em_method->m_attr[i] ) {
	    num_diff++;
	    attr_list[i]=1;
	}
    }
    return num_diff;
}


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

int ADCL_hypothesis_shrinklist_byattr ( ADCL_emethod_req_t *ermethod, 
                                        int attribute, int required_value )
{
    int i, count=0;
    ADCL_method_t *tmeth=NULL;
    int org_count=ermethod->er_num_emethods;
    ADCL_emethod_t *emethods=ermethod->er_emethods;

    /* This following loop is only for debugging purposes and should be removed
       from later production runs */
    for ( i=0; i < org_count; i++ ) {
        tmeth = emethods[i].em_method;
	if ( tmeth->m_attr[attribute] != required_value ) {
	    ADCL_printf("#Removing method %d for attr %d required %d is %d\n",
			tmeth->m_id, attribute, required_value, 
			tmeth->m_attr[attribute]);
	}
    }

    for ( i=0; i < org_count; i++ ) {
        tmeth = emethods[i].em_method;
	if ( tmeth->m_attr[attribute] == required_value ) {
            if ( count != i ) {
		/* move this emethod from pos i to pos count */
		ermethod->er_emethods[count] = ermethod->er_emethods[i];
	    }
	    count++;
	}
    }
    ermethod->er_num_emethods = count;

    ADCL_printf("#Emethod list has been shrinked from %d to %d entries\n", 
		org_count, count );

    return ADCL_SUCCESS;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_hypothesis_c2m_perf(ADCL_emethod_req_t *ermethods, 
			     int pos1, int pos2 )
{
    ADCL_emethod_t *emeth1, *emeth2;
    int winner, realwinner=-1, blength=2;
    ADCL_emethod_t tmpmeth[2], *t=NULL;
   
    emeth1 = &(ermethods->er_emethods[pos1]);
    emeth2 = &(ermethods->er_emethods[pos2]);

    ADCL_statistics_filter_timings ( &emeth1, 1, ermethods->er_comm);
    ADCL_statistics_filter_timings ( &emeth2, 1, ermethods->er_comm);
    
    ADCL_statistics_determine_votes ( &emeth1, 1, ermethods->er_comm );
    ADCL_statistics_determine_votes ( &emeth2, 1, ermethods->er_comm );

    tmpmeth[0].em_lpts = emeth1->em_lpts;
    tmpmeth[1].em_lpts = emeth2->em_lpts;
    t = tmpmeth;
    ADCL_statistics_global_max (&t, 2, ermethods->er_comm, 1, &blength, &winner);

    if ( winner == 0 ) {
        realwinner = pos1;
    }
    else if ( winner == 1 ) {
	realwinner = pos2;
    }

    return realwinner;
}
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_hypothesis_set ( ADCL_emethod_req_t *er, int attr, int attrval ) 
{
  
  if ( er->er_attr_hypothesis[attr] == ADCL_ATTR_NOT_SET ) {
    er->er_attr_hypothesis[attr] = attrval;
    er->er_attr_confidence[attr] = 1;
    ADCL_printf("Hypothesis for attr %d set to %d, confidence"
		" %d\n", attr, attrval, er->er_attr_confidence[attr]);
  } 
  else if ( attrval == er->er_attr_hypothesis[attr] ) {
    er->er_attr_confidence[attr]++;
    ADCL_printf("Hypothesis for attr %d is %d, confidence "
		"incr to %d\n", attr, attrval, er->er_attr_confidence[attr]);
  }
  else {
    er->er_attr_confidence[attr]--;
    ADCL_printf("Hypothesis for attr %d is %d, confidence "
		"decr to %d\n", attr, er->er_attr_hypothesis[attr], 
		er->er_attr_confidence[attr]);
    if ( er->er_attr_confidence[attr] == 0 ) {
      /* we don't have a performance hypthesis for this attribute anymore */
      er->er_attr_hypothesis[attr] = ADCL_ATTR_NOT_SET;
    }
  }

  return ADCL_SUCCESS;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_hypothesis_eval_v1 ( ADCL_emethod_req_t *er )
{
    int k, jj;
    int num_diff = 0;
    int fmethod, smethod;
    int fattr, sattr;
    int attr_list[ADCL_ATTR_TOTAL_NUM];

    for ( k=0; k< er->er_num_avail_meas-1; k++ ){
      /* check whether we only differ to method k in attribute attr[j] */
      memset ( attr_list, 0, sizeof(int)*ADCL_ATTR_TOTAL_NUM);
      num_diff = ADCL_hypothesis_c2m_attr ( er, attr_list, 
					    k, er->er_last);
      
      /*  if only one attribute is different, we can extract useful
	  data */
      if ( num_diff == 1 ) {
	for ( jj=0; attr_list[jj] == 0 && jj < ADCL_ATTR_TOTAL_NUM; jj++);
	
	  fmethod = ADCL_hypothesis_c2m_perf( er, k,er->er_last);
	  ADCL_printf("Attr: %d Comparing methods %d (%d) and %d (%d) "
		      " winner is %d\n", jj,
		      er->er_emethods[k].em_method->m_id, 
		      er->er_emethods[k].em_method->m_attr[jj],
		      er->er_emethods[er->er_last].em_method->m_id, 
		      er->er_emethods[er->er_last].em_method->m_attr[jj],
		      er->er_emethods[fmethod].em_method->m_id );

	  smethod = (fmethod==k)? er->er_last:k;		
	  sattr = er->er_emethods[smethod].em_method->m_attr[jj];
	  fattr = er->er_emethods[fmethod].em_method->m_attr[jj];
		
	  if ( er->er_attr_hypothesis[jj] == ADCL_ATTR_NOT_SET ) {
	    er->er_attr_hypothesis[jj] = fattr;
	    er->er_attr_confidence[jj] = 1;
	    ADCL_printf("Hypothesis for attr %d set to %d, confidence"
			" %d\n", jj, fattr, 
			er->er_attr_confidence[jj]);
	  } 
	  else if ( fattr == er->er_attr_hypothesis[jj] ) {
	    er->er_attr_confidence[jj]++;
	    ADCL_printf("Hypothesis for attr %d is %d, confidence "
			"incr to %d\n", jj, fattr, 
			er->er_attr_confidence[jj]);
	  }
	  else if ( sattr == er->er_attr_hypothesis[jj] ) {
	    er->er_attr_confidence[jj]--;
	    ADCL_printf("Hypothesis for attr %d is %d, confidence "
			"decr to %d\n", jj, er->er_attr_hypothesis[jj], 
			er->er_attr_confidence[jj]);
	    if ( er->er_attr_confidence[jj] == 0 ) {
	      /* we don't have a performance hypthesis 
		 for this attribute anymore */
	      er->er_attr_hypothesis[jj] = ADCL_ATTR_NOT_SET;
	    }
	  }
	  else {
	    ADCL_printf("Unhandled case at this point!\n");
	    /* What we would have to do is to compare against
	    ** the method which has the identical attributes as 
	    ** method er_last, only differing in attr[j]. The problem
	    ** is, that this approach breaks the k-loop *and* the
	    ** method might not exist. This situation can only 
	    */
	  }
	}        
    }
      
    for ( jj=0; jj< ADCL_ATTR_TOTAL_NUM; jj++ ) {
	if ( er->er_attr_confidence[jj] >= ADCL_attr_max[jj] && 
	     !er->er_attr_handled[jj] ) {
	    /* remove all methods from the emethods list which
	       have a different value for attribute attr[j] than 
	       hypothesis[j] using function (2) */
	    ADCL_hypothesis_shrinklist_byattr ( er, jj , 
						er->er_attr_hypothesis[jj] ); 
	    er->er_attr_handled[jj] = 1;
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

int ADCL_hypothesis_eval_v2 ( ADCL_emethod_req_t *er )
{
    ADCL_emethod_t  **tmp_emethods=NULL;
    int numblocks=0, nummethods=0;
    int cnt=0, blencnt=0, bcnt=0;
    int *winners=NULL, *blength=NULL, *pattrs=NULL;
    int i, tval, loop, ret = ADCL_SUCCESS;
    int attr_list[ADCL_ATTR_TOTAL_NUM], tmp_active_attr_list[ADCL_ATTR_TOTAL_NUM];

    if ( er->er_num_active_attrs == 2 && 
	 er->er_num_avail_meas == 4 ) {
        /* 
	 * Determine how many blocks we will generate and how many individual
	 * methods will be in the final list. 
	 */
	get_num_methods_and_blocks ( er, &nummethods, &numblocks);
	
	tmp_emethods = (ADCL_emethod_t **) malloc (nummethods * sizeof(ADCL_emethod_t*)); 
	blength = (int *) calloc ( 1, 3 * numblocks * sizeof(int));
	if ( NULL==tmp_emethods || NULL == blength ){
	    return ADCL_NO_MEMORY;
	}
	winners = &(blength[numblocks]);
	pattrs = &(blength[2*numblocks]);
	
	/* 
	 * main loop over the number of attributes currently investigated. 
	 * Within this loop we generate the list of emethods, which 
	 * we will pass to the evaluation function. 
	 */
	for ( loop=0; loop<er->er_num_active_attrs; loop++ ){
	    
	    /* 
	     * The attribute currently investigated has to be on the top 
	     * of the list for the next_attr_combination function.  Thus
	     * generate for every 'loop' loop iterations a new list of
	     * attributes moving the according element to the top of the list 
	     */
	    for ( i=0; i< er->er_num_active_attrs; i++ ) {
		tmp_active_attr_list[i] = er->er_active_attr_list[(i+loop)%er->er_num_active_attrs];
	    }
	    
	    /* Generate the first combination of attributes. The first
	       one is really independent of the list of attributes used. 
	    */
	    for ( i = 0; i < ADCL_ATTR_TOTAL_NUM; i++ ) {
		attr_list[i] = ADCL_attr_base[i];
	    }
	    pattrs[bcnt] = er->er_active_attr_list[loop];
	    
	    ret = ADCL_SUCCESS;
	    while ( ret != ADCL_EVAL_DONE ) {
		tmp_emethods[cnt++] = ADCL_emethod_get_by_attrs ( er, attr_list); 
		blencnt++;
		
		ret = next_attr_combination(er->er_num_active_attrs, 
					    tmp_active_attr_list, 
					    attr_list);
		if ( ret != ADCL_SUCCESS ) {
		    pattrs[bcnt]    = er->er_active_attr_list[loop];
		    blength[bcnt++] = blencnt;
		    blencnt         = 0;
		}
	    }
	}
	
	/* Determine now the winners across all provided measurement lists */
	ADCL_statistics_filter_timings  ( tmp_emethods, nummethods, er->er_comm);
	ADCL_statistics_determine_votes ( tmp_emethods, nummethods, er->er_comm);
	ADCL_statistics_global_max ( tmp_emethods, nummethods, er->er_comm, 
				     numblocks, blength, winners);
	
	/* Set the according performance hypothesis */
	for (i=0; i< numblocks; i++ ) {
	    tval = tmp_emethods[winners[i]]->em_method->m_attr[pattrs[i]];
	    ADCL_hypothesis_set ( er, pattrs[i], tval );
	}
	
	/* Now the shrink the emethods list if we reached a threshold */
	for ( i=0; i< er->er_num_active_attrs; i++ ){
	    if ( er->er_attr_confidence[pattrs[i]] >= ADCL_attr_max[pattrs[i]] ) {
		ADCL_hypothesis_shrinklist_byattr(er, pattrs[i], 
						  er->er_attr_hypothesis[pattrs[i]]);
		/* TBD: we will have to adapt here the number of attributes for pattrs[i]
		 *      as well as the attr_base value!!! This has to  be done on a 
		 *      per eemethods_req basis 
		 */
	    }
	}
	
	/* Switch to the next list of attributes to be evaluated */
	er->er_num_active_attrs=1;
	er->er_active_attr_list[0] = ADCL_ATTR_TRANSFER;
	
	free ( tmp_emethods );
	free ( blength );
    }

    return ADCL_SUCCESS;
}
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

/* cnt: number of currently handled attributes 
 * attr_list: list of attributes currently being handled. An array of length cnt 
 * attr_val_list: array of dimension ADCL_ATTR_TOTAL_NUM containg the last used 
 * combination of attributes 
 * last_attr: which entry of the attr_list has last been modified 
 */

static int next_attr_combination ( int cnt, int *attr_list, int *attr_val_list )
{
    int i, ret=ADCL_SUCCESS;
    int thisattr, thisval;

    for ( i = 0; i < cnt; i++ ) {
	thisattr = attr_list[i];
	thisval = attr_val_list[thisattr];
	
	if ( thisval < (ADCL_attr_base[thisattr] + ADCL_attr_max[thisattr]) ) {
	    attr_val_list[thisattr]++;
	    return ret;
	}
	else if ( thisval ==  (ADCL_attr_base[thisattr] + ADCL_attr_max[thisattr]) ) {
	    attr_val_list[thisattr] = ADCL_attr_base[thisattr];
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

static int get_num_methods_and_blocks ( ADCL_emethod_req_t *er, int *nummethods, 
					int *numblocks)
{
    int tmpnblocks, i, j;
    int nm, nb;

    for ( nm=er->er_num_active_attrs, i=0; i<er->er_num_active_attrs; i++) {
	for ( tmpnblocks=1, j=0; j < er->er_num_active_attrs; j++ ) {
	    if ( j == i ) {
		continue;
	    }
	    tmpnblocks *= ADCL_attr_max[er->er_active_attr_list[j]];
	}
	nb += tmpnblocks;
	nm *= ADCL_attr_max[er->er_active_attr_list[i]];
    }

    *nummethods = nm;
    *numblocks  = nb;
    return ADCL_SUCCESS;
}
