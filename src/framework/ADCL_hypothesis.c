#include "ADCL_internal.h"

int ADCL_hypothesis_compare2methods_attr ( ADCL_emethod_req_t *ermethods, 
				       					   int *attr_list, int pos1, int pos2 )
{
    int num_diff=0;
    int last_diff=-1;
    
    ADCL_emethod_t *emeth1, *emeth2;
    ADCL_emethod_t tmpmeth[2];

    emeth1 = ermethods->er_emethods[pos1];
    emeth2 = ermethods->er_emethods[pos2];

    /* determine in how many attributes the two methods differ */
   
    for ( i=0; i < ADCL_ATTR_TOTAL_NUM; i++ ) {
	if ( emeth1->em_method.m_attr[i] != emeth2->em_method.m_attr[i] ) {
	    num_diff++;
	    attr_list[i]=1;
	}
    }
    return num_diff;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/* Shrink the emethods list based on an attribute and a required value; */
int ADCL_hypothesis_shrinklist_byattr ( ADCL_emethod_req_t *ermethod, 
                                        int attribute, int required_value )
{
    int i, count=0;
    ADCL_method_t *tmeth=NULL;
    int org_count=ermethod->er_num_emethods;
    ADCL_emethod_t *emethods=ermethod->er_emethods;

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

    return ADCL_SUCCESS;
}

int ADCL_hypothesis_compare2methods_perf(ADCL_emethod_req_t *ermethods, 
				         				 int pos1, int pos2 )
{
    ADCL_emethod_t *emeth1, *emeth2;
    int winner, realwinner=-1;
    ADCL_emethod_t tmpmeth[2];

    emeth1 = ermethods->er_emethods[pos1];
    emeth2 = ermethods->er_emethods[pos2];

    /* Only do the comparison, if really just a single attribute is different! */
    ADCL_statistics_filter_timings ( emeth1, 1, ermethods->er_comm);
    ADCL_statistics_filter_timings ( emeth2, 1, ermethods->er_comm);
    
    ADCL_statistics_determine_votes ( emeth1, 1, ermethods->er_comm );
    ADCL_statistics_determine_votes ( emeth2, 1, ermethods->er_comm );

    tmpmeth[0].em_lpts = emeth1.em_lpts;
    tmpmeth[1].em_lpts = emeth2.em_lpts;
    winner = ADCL_statistics_global_max (tmpmeth, 2, ermethods->er_comm);

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

