#include "ADCL_internal.h"

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
    ADCL_emethod_t tmpmeth[2];
   
    emeth1 = &(ermethods->er_emethods[pos1]);
    emeth2 = &(ermethods->er_emethods[pos2]);

    ADCL_statistics_filter_timings ( emeth1, 1, ermethods->er_comm);
    ADCL_statistics_filter_timings ( emeth2, 1, ermethods->er_comm);
    
    ADCL_statistics_determine_votes ( emeth1, 1, ermethods->er_comm );
    ADCL_statistics_determine_votes ( emeth2, 1, ermethods->er_comm );

    tmpmeth[0].em_lpts = emeth1->em_lpts;
    tmpmeth[1].em_lpts = emeth2->em_lpts;
    ADCL_statistics_global_max (tmpmeth, 2, ermethods->er_comm, 1, &blength, &winner);

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
