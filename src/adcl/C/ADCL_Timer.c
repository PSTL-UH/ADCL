/*
 * Copyright (c) 2009           High Performance Computing Center Stuttgart. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include "ADCL.h"
#include "ADCL_internal.h"

/**********************************************************************/
int ADCL_Timer_create ( int nreq, ADCL_Request *reqs, ADCL_Timer *timer )
/**********************************************************************/
{
    int i, ret; 
//    MPI_Group refgroup, testgroup; 
    MPI_Comm refcomm, testcomm; 

    int relation;
  
    /* are requests undefined or their emethods already associated with other timers? */
    for (i=0; i<nreq; i++){
      if ( NULL == reqs[i] || ADCL_REQUEST_NULL == reqs[i] ) 
         return ADCL_INVALID_REQUEST;

      if ( reqs[i]->r_emethod->em_assoc_with_timer > -1 ) 
         return ADCL_INVALID_EMETHOD; 
    }  

    /* see if their are mismatches in topology */ 
    refcomm = reqs[0]->r_emethod->em_topo->t_comm; 
    for (i=1; i<nreq; i++){
        MPI_Comm_compare ( refcomm, reqs[i]->r_emethod->em_topo->t_comm, &relation );
        if ( ( relation != MPI_IDENT) && (relation != MPI_CONGRUENT) ) {
            return ADCL_INVALID_TOPOLOGY;
        }
    } 

//    MPI_Comm_group(req[0]->r_topo->t_comm, &refgroup);
//    for (i=1; i<nreq; i++){
//       MPI_Comm_group(req[i]->r_topo->t_comm, &testgroup);  
//       MPI_Group_compare(refgroup, testgroup, &relation);
//       if ( MPI_UNEQUAL == relation ) return ADCL_INVALID_TOPOLOGY;
//    } 
      
    ret = ADCL_timer_create ( nreq, reqs, timer );

    return ret;
}


/**********************************************************************/
int ADCL_Timer_free ( ADCL_Timer *timer )
/**********************************************************************/
{
    ADCL_timer_t *ptimer = *timer;
    int ret; 

    if ( NULL == timer ) {
        return ADCL_INVALID_TIMER;
    }
    if ( 0 > ptimer->t_nreqs ) {
        ADCL_printf("This is not good: there are still requests associated with this timer\n");
        return ADCL_INVALID_TIMER;
    }

    ret = ADCL_timer_free ( timer );

    timer = ADCL_TIMER_NULL;

    return ret; 
}

/**********************************************************************/
int ADCL_Timer_start ( ADCL_Timer timer )
/**********************************************************************/
{
    int ret; 

    if ( NULL == timer ) {
        return ADCL_INVALID_TIMER;
    }

    ret = ADCL_timer_start ( timer );

    return ret;

}

/**********************************************************************/
int ADCL_Timer_stop ( ADCL_Timer timer )
/**********************************************************************/
{

    int ret; 

    if ( NULL == timer ) {
        return ADCL_INVALID_TIMER;
    }

    ret = ADCL_timer_stop ( timer );

    return ret;

}

