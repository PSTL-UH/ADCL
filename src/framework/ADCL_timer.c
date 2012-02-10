/*
 * Copyright (c) 2006-2007      University of Houston. All rights reserved.
 * Copyright (c) 2008           HLRS. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ADCL.h"
#include "ADCL_internal.h"
#include "ADCL_request.h"
#include "ADCL_emethod.h" 

ADCL_array_t* ADCL_timer_farray=NULL;
int ADCL_timer_steps_between_barriers=1;
static int ADCL_local_timer_counter=0;

extern ADCL_array_t *ADCL_emethod_array;
extern int ADCL_emethod_numtests;
extern int ADCL_use_barrier;

/* helper functions */
int ADCL_get_next_comb( int n, int *ub, int *comb); /* to find next combinations of functions 
   for metafunction */

int * ADCL_timer_get_assoc_reqs ( ADCL_timer_t *timer, int nreq_in, ADCL_request_t** reqs_in,  
    int nemethods, ADCL_emethod_t **emethods ); 
    /* saves all requests that are associated with a timer in an ordered array */

int* ADCL_timer_name_metafnct( ADCL_timer_t *timer, int nemethods, ADCL_emethod_t **emethods,
     int* comb, char tfnctname[]); /* constructs name for meta function */ 

/***************************************************************************************************/
int ADCL_timer_create ( int nreq_in, ADCL_request_t **reqs_in, ADCL_timer_t **timer )
/* takes a list of requests and creates a new timer object with it; the list is enlarged if not    */ 
/* all requests associated with an emethod have been added to the timer object                     */
/***************************************************************************************************/
{
    int i, j, id;
    int done;
    int aoffset, foffset;     /* offsets for attributes and functions, resp. */ 
    int nems_tot;             /* number of emethods in array ADCL_emethod_array */

    ADCL_timer_t     *ttimer; /* temporary timer object for creation */
    ADCL_attrset_t *tattrset; /* temporary attribute set */
    int        *tmp_attrvals; /* temporary copy of attribute values */ 
    ADCL_emethod_t        *e; /* temporary emethod object */ 
    ADCL_function_t    *fnct; /* temporary function object */ 
    char tfnct_name[100];  

    int nmetaattrs;      /* #attributes of meta function (sum of attributes of all functionssets 
                            from emethods associated with the timer object */ 
    int nmetafncts;      /* all possible combinations (product) of functions of all functionssets 
                            from emethods associated with the timer object */ 
 
    ADCL_attribute_t **metaattrlist; /* joint attributes for new meta functionset */
    ADCL_attrset_t     *metaattrset; /* joint attribute set for new meta functionset */
    ADCL_function_t       *metafnct; /* function with joint attribute set */
    ADCL_function_t  **metafnctlist; /* list of all functions with joint attributes */ 
    ADCL_fnctset_t     *metafnctset; /* basically all functions of functions sets from emethods 
                                        associated with the timer object, but based on new joint 
                                        attribute set */
    int                      *comb; /* array with current combination/selection of functions */
    int                        *ub; /* array of upper bound for comb, upper bound is #functions in 
                                       functionset, lower bound is assumed to be 0 */
    ADCL_function_t     **fnctlist; /* list of all functions with joint attributes */ 
    int                  nemethods; /* #emethods that are associated to the timer */
    ADCL_emethod_t      **emethods; /* list of those emethods */

    /* Allocate a new timer object */
    ttimer = (ADCL_timer_t *) calloc ( 1, sizeof (ADCL_timer_t) );
    if ( NULL == ttimer ) {
        return ADCL_NO_MEMORY;
    }

    /* Set the according elements of the structure */
    ttimer->t_id     = ADCL_local_timer_counter++;
    ADCL_array_get_next_free_pos ( ADCL_timer_farray, &(ttimer->t_findex) );
    ADCL_array_set_element ( ADCL_timer_farray,
                 ttimer->t_findex,
                 ttimer->t_id,
                 ttimer );
    ttimer->t_barrier_cnt = 1;
 
    /* mark emethods */
    for ( i=0; i<nreq_in; i++) {
       reqs_in[i]->r_emethod->em_assoc_with_timer = ttimer->t_id; 
    }

    /* get number of emethods associated with timer and compute number of meta functions
       as well as number of meta attributes */
    nemethods  = 0;
    nmetafncts = 1;
    nmetaattrs = 0;

    nems_tot = ADCL_array_get_number_elements ( ADCL_emethod_array ); 
    for ( id=0; id<nems_tot; id++ ) {
        e = ( ADCL_emethod_t * ) ADCL_array_get_ptr_by_id ( ADCL_emethod_array, id );
        if (e->em_assoc_with_timer != ttimer->t_id ) 
           continue; 
        nemethods++;
        nmetafncts *= e->em_fnctset.fs_maxnum;
        if ( ADCL_ATTRSET_NULL != e->em_fnctset.fs_attrset ) {
           nmetaattrs += e->em_fnctset.fs_attrset->as_maxnum;
        }
    }

    /* save emethods associated with the timer in a list */
    emethods = (ADCL_emethod_t **) malloc ( nemethods * sizeof(ADCL_emethod_t *) ); 
    nemethods  = 0;
    for ( i=0; i<nems_tot ; i++ ) {
        e = ( ADCL_emethod_t * ) ADCL_array_get_ptr_by_pos ( ADCL_emethod_array, i );
        if (e->em_assoc_with_timer == ttimer->t_id ) {
            emethods[nemethods] = e;
            nemethods++; 

            /* "disable" emethods, no barriers */
            e->em_state = ADCL_STATE_REGULAR;
        }
    }
    ttimer->t_nemethods = nemethods;

    /* book keeping: figure out which requests belong to an emethod and save those in order in a list,
       initializes also timer-t_offset_reqs[] */
    ADCL_timer_get_assoc_reqs ( ttimer, nreq_in, reqs_in, nemethods, emethods ); 

    /* create joint attribute list from all relevant function sets */
    if ( 0 == nmetaattrs ) { /* happens at the moment e.g. for Allreduce functionset */
       metaattrset = ADCL_ATTRSET_NULL;
    }
    else {
       metaattrlist = (ADCL_attribute_t **) malloc( nmetaattrs * sizeof(ADCL_attribute_t *) );
       aoffset = 0; 
       for ( i=0; i<nemethods; i++ ) {
          if ( ADCL_ATTRSET_NULL != emethods[i]->em_fnctset.fs_attrset ) {
             /* copy all attributes of attribute_sets into a joint list 
                metaattrlist = (attr_of_fnctset1, attr_of_fnctset2, ..., attr_of_fnctsetn) */
             tattrset = emethods[i]->em_fnctset.fs_attrset; 
             for ( j=0; j<tattrset->as_maxnum; j++) {
                metaattrlist[aoffset+j] =  tattrset->as_attrs[j];
             }
             aoffset += tattrset->as_maxnum;
          }
       }
       ADCL_attrset_create ( nmetaattrs, metaattrlist, &(metaattrset) );
    }

    /* create new joint fnctset by "copying" all relevant functions, but with metaattrset and right attr_values */
    metafnctlist = (ADCL_function_t **) malloc ( nmetafncts * sizeof(ADCL_function_t *) );
    tmp_attrvals = (int*) malloc ( nmetaattrs * sizeof(int) );
    comb     = (int *) calloc ( nemethods, sizeof(int) );
    ub       = (int *) malloc ( nemethods * sizeof(int) ); 
    for ( i=0; i<nemethods; i++ ) 
        ub[i] = emethods[i]->em_fnctset.fs_maxnum-1; 
   
    done = 0; 
    foffset = 0;
    while (!done) {
       /* - set attribute list: take the combination and copy from each functionset the values of 
            the attributes for the specified function to the array   
          - save the list of function combinations to the meta function */
       aoffset = 0;
       fnctlist = (ADCL_function_t **) malloc ( nemethods * sizeof(ADCL_function_t *) );
       for ( i=0; i<nemethods; i++ ) {
           fnctlist[i] = emethods[i]->em_fnctset.fs_fptrs[comb[i]]; /* same idx for emethod and function pointer OK */ 

           /* copy its attrvals to tmp_attrvals at the right position */
           if ( ADCL_ATTRSET_NULL != emethods[i]->em_fnctset.fs_attrset ) {
              memcpy (&(tmp_attrvals[aoffset]), fnctlist[i]->f_attrvals, fnctlist[i]->f_attrset->as_maxnum * sizeof(int) ); 
              aoffset += emethods[i]->em_fnctset.fs_attrset->as_maxnum;
           }
       } 
   
       /* give the child a name: meta_t1:r1_r2_func1_r3_func4 */
       ADCL_timer_name_metafnct(ttimer, nemethods, emethods, comb, tfnct_name); 

       /* make a copy of fnct with new attrset and new fattrvals_tmp and save to metafnctlist */
       ADCL_function_create_async ( ADCL_FUNCTION_NULL, ADCL_FUNCTION_NULL, metaattrset, tmp_attrvals, 
           tfnct_name, &(metafnct) );
       metafnct->f_fnctlist  = fnctlist;
       metafnctlist[foffset] = metafnct;
       
       /* get next combination of functions */
       done = ADCL_get_next_comb( nemethods, ub, comb); 

       foffset++;
    } 
    ADCL_fnctset_create ( nmetafncts, metafnctlist, "timer_pseudofnctset", &(metafnctset) );

    /* create a new emethod (any of the topologies will do) */
    ttimer->t_emethod = ADCL_emethod_init( reqs_in[0]->r_emethod->em_topo, ADCL_VECTOR_NULL, 
					   metafnctset, ADCL_NO_ROOT ); 

    /* set requests to first fnct, user might start request before starting the timer */
    fnct = metafnctlist[0]; 
    for ( i=0; i<ttimer->t_nemethods; i++) { 
        for ( j=ttimer->t_offset_reqs[i]; j<ttimer->t_offset_reqs[i+1]; j++) {
            ttimer->t_reqs[j]->r_function = fnct->f_fnctlist[i]; 
        } 
    }

    *timer = ttimer;

    return ADCL_SUCCESS;
}

/***************************************************************************************************/
int ADCL_timer_free  ( ADCL_timer_t **timer )
/***************************************************************************************************/
{
    int i;
    ADCL_timer_t *ttimer=*timer;
    ADCL_emethod_t *em;

    /* unmark emethods */
    for ( i=0; i<ttimer->t_nreqs; i++) {
       em = ttimer->t_reqs[i]->r_emethod;
       em->em_assoc_with_timer = -1; 
       /* "enable" emethods */
       em->em_state = ADCL_STATE_TESTING;
    }
 
    /* free arrays */
    if ( NULL              != ttimer->t_reqs ) free ( ttimer->t_reqs ); 
    if ( ADCL_EMETHOD_NULL != ttimer->t_emethod )  free ( ttimer->t_emethod );  

    ADCL_array_remove_element ( ADCL_timer_farray, ttimer->t_findex);
    free ( ttimer );
    
    *timer = ADCL_TIMER_NULL;
    
    return ADCL_SUCCESS;
}


/***************************************************************************************************/
int ADCL_timer_start ( ADCL_timer_t *timer )
/***************************************************************************************************/
{
    int i, j;
    ADCL_function_t *fnct;
    MPI_Comm comm =  timer->t_emethod->em_topo->t_comm;

    //CHECK_COMM_STATE ( req->r_comm_state, ADCL_COMM_AVAIL );

    if (( timer->t_emethod->em_state == ADCL_STATE_TESTING && 1 == timer->t_barrier_cnt ) ||
          timer->t_emethod->em_state == ADCL_STATE_REGULAR ) { 
       timer->t_function = ADCL_emethod_get_function_by_state( timer->t_emethod, &(timer->t_erlast), 
           &(timer->t_erflag), "timer", timer->t_id, ADCL_COMM_AVAIL);

       /* set requests to which fnct belongs */
       fnct = timer->t_function; 
       for ( i=0; i<timer->t_nemethods; i++) { 
           for ( j=timer->t_offset_reqs[i]; j<timer->t_offset_reqs[i+1]; j++) {
               timer->t_reqs[j]->r_function = fnct->f_fnctlist[i]; 
           } 
       }

#ifdef PERF_DETAILS
       start_time = TIME;
#endif /* PERF_DETAILS */
#ifdef ADCL_USE_BARRIER
       if ( 1 == ADCL_use_barrier && timer->t_emethod->em_state == ADCL_STATE_TESTING ) {
            //  already fulfilled: && 1 == timer->t_barrier_cnt ) {
           MPI_Barrier ( comm );
       }
#endif /* ADCL_USE_BARRIER */
#ifdef PERF_DETAILS
       end_time = TIME;
       elapsed_time += (end_time - start_time);
#endif /* PERF_DETAILS */
       /* Get starting point in time after barrier */
       timer->t_t1 = TIME;
    }

    return ADCL_SUCCESS;
}

/***************************************************************************************************/
int ADCL_timer_stop ( ADCL_timer_t *timer )
/***************************************************************************************************/
{
    TIME_TYPE t2;
    MPI_Comm comm =  timer->t_emethod->em_topo->t_comm;

#ifdef PERF_DETAILS
    TIME_TYPE start_time, end_time;
    static TIME_TYPE elapsed_time = 0;
#endif /* PERF_DETAILS */

    if (( timer->t_emethod->em_state == ADCL_STATE_TESTING && ADCL_timer_steps_between_barriers == timer->t_barrier_cnt ) || 
          timer->t_emethod->em_state == ADCL_STATE_REGULAR ) {
#ifdef PERF_DETAILS
        start_time = TIME;
#endif /* PERF_DETAILS */
#ifdef ADCL_USE_BARRIER
        if ( 1 == ADCL_use_barrier && timer->t_emethod->em_state == ADCL_STATE_TESTING ) {
            MPI_Barrier ( comm );
            timer->t_barrier_cnt = 1;
        }
#endif /* ADCL_USE_BARRIER */

#ifdef PERF_DETAILS
        end_time = TIME;
        elapsed_time += (end_time - start_time);
#endif
        /* Get ending point in time after barrier */
        t2 = TIME;

#ifndef ADCL_USERLEVEL_TIMINGS
        /* Update the request with the timings */
        ADCL_timer_update ( timer, timer->t_t1, t2 );
#endif /* ADCL_USERLEVEL_TIMINGS */
    }
    else if ( timer->t_emethod->em_state == ADCL_STATE_TESTING && ADCL_timer_steps_between_barriers != timer->t_barrier_cnt ) {
        timer->t_barrier_cnt++;
    }

    return ADCL_SUCCESS;
}


/***************************************************************************************************/
int ADCL_timer_update( ADCL_timer_t *timer, TIME_TYPE t1, TIME_TYPE t2 ) 
/* similar to ADCL_request_update                                     */
/***************************************************************************************************/
{
    if ( (t1 == -1 ) && ( t2 == -1 ) ) {
        return ADCL_SUCCESS;
    }
/* to investigate: t_function */
    ADCL_printf("%d: timer %d method %d (%s) %8.4f \n",
        timer->t_emethod->em_topo->t_rank, timer->t_id, timer->t_function->f_id,
        timer->t_function->f_name, t2>t1 ? (t2-t1):(1000000-t1+t2)); 
    switch ( timer->t_emethod->em_state ) {
    case ADCL_STATE_TESTING:
        ADCL_emethods_update (timer->t_emethod, timer->t_erlast,
                  timer->t_erflag, t1, t2);
        break;
    case ADCL_STATE_REGULAR:
        timer->t_emethod->em_state = ADCL_emethod_monitor (timer->t_emethod,
                                 timer->t_emethod->em_last,
                                 t2, t1 );
        break;
    case ADCL_STATE_DECISION:
    default:
        ADCL_printf("%s: Unknown object status for timer %d, status %d \n",
            __FILE__, timer->t_id, timer->t_emethod->em_state );
        break;
    }

    return ADCL_SUCCESS;
}

/***************************************************************************************************/
/***************************************************************************************************/
/***                                     HELPER FUNCTIONS                                        ***/
/***************************************************************************************************/
/***************************************************************************************************/


/***************************************************************************************************/
int ADCL_get_next_comb( int n, int *ub, int *comb){
/***************************************************************************************************/
/* gets the next combination of an array comb with upper bounds ub 
   e.g. comb[3, 2, 1], ub[4, 2, 2] -> comb[4, 2, 1]
        comb[4, 2, 1], ub[4, 2, 2] -> comb[1, 1, 2]
   n       (IN)    - dimension of the array 
   ub[n]   (IN)    - upper bounds of array 
   comb[n] (INOUT) - array with current / updated combination, lower boundary is 0 

   return values:  1, done, comb[i] == ub[i], i=1,n
                   0, got next comb in array */
/***************************************************************************************************/
    int i, j; 

    /* find the next possible idx to increment */
    for ( i=0; i<n; i++) { 
       if ( comb[i] < ub[i] ) 
          break;
    }
 
    if ( i == n && comb[n-1] == ub[n-1] )  /* this was the last pos, we are done */
       return 1; 
          
    /* OK, increment the value with that index and set all values with smaller indices to 0 */
    comb[i]++;
    for ( j=0; j<i; j++ ) 
       comb[j] = 0;

    return 0; 
}
     
/***************************************************************************************************/
int * ADCL_timer_get_assoc_reqs ( ADCL_timer_t *timer, int nreq_in, ADCL_request_t** reqs_in,  
    int nemethods, ADCL_emethod_t **emethods ) { 
/***************************************************************************************************/
/*  figure out which requests share an emethodx , so the function of these request can 
    be set at the same time; save those request in a ordered list
    since several requests can share an emethod, the requests of the user my be incomplete 
    timer               (INOUT) - timer object, values for t_nreqs, t_reqs and t_offset_reqs are set 
    nreq_in             (IN)    - #requests (as specified by the user)
    reqs_in             (IN)    - array of (unordered) requests (as specified by the user)
    nemethods           (IN)    - size of array emethods 
    emethods[nemethods] (IN)    - array of emethods associated with the timer
*/
/***************************************************************************************************/
    int i, k, id, found, rank;
    ADCL_emethod_t *e; 
    ADCL_request_t *treq, **treqs;  /* temporary request and array of requests */
    int offset_reqs, nreqs_tot;

    nreqs_tot = ADCL_array_get_number_elements ( ADCL_request_farray ); 
    treqs     = (ADCL_request_t **) malloc( nreqs_tot * sizeof (ADCL_request_t*)); /* this array is 
       too large, but it avoids code doubling */
    timer->t_offset_reqs = (int *) malloc ( (nemethods+1) * sizeof(int) );

    offset_reqs = 0;
    timer->t_offset_reqs[0]=0;
    for ( i=0; i<nemethods; i++ ) {
       e = emethods[i];

       /*  find requests that have this emethod */
       for ( id=0; id<nreqs_tot; id++) {
          /* get request */ 
          treq = ADCL_array_get_ptr_by_id ( ADCL_request_farray, id );
 
          if ( treq->r_emethod->em_id == e->em_id ){ 
             treqs[offset_reqs] = treq; 
             offset_reqs++;
          }
       }
       timer->t_offset_reqs[i+1] = offset_reqs;
    }

    if ( nreq_in != offset_reqs ) {
       /* we have a slight problem: the user forgot to associate a request with the timer; this request shares an emethod with 
          another request which is associated with the timer */ 
       rank = reqs_in[0]->r_emethod->em_topo->t_rank;
       for ( i=0; i<offset_reqs; i++ ) {
          found = 0;
          for ( k=0; k<nreq_in; k++) {
              if ( treqs[i] == reqs_in[k] ) {
                  found = 1;
                  break;
              }
          }
          if ( 0 == found && 0 == rank ) {
             printf ("WARNING: request %d has been automatically added to the timer object due to a shared emethod object\n", 
                treqs[i]->r_id); 
          }
       }
    }

    /* copy resulting array to timer structure */
    timer->t_nreqs       = offset_reqs;
    timer->t_reqs        = (ADCL_request_t **) malloc( timer->t_nreqs * sizeof (ADCL_request_t*));
    for ( i=0; i<timer->t_nreqs; i++ ) {
        timer->t_reqs[i] = treqs[i];
    }

    return ADCL_SUCCESS;
}

/***************************************************************************************************/
int* ADCL_timer_name_metafnct( ADCL_timer_t* timer, int nemethods, ADCL_emethod_t **emethods,
     int *comb, char tfnct_name[]){ 
/***************************************************************************************************/
    int i, k;
    char itoabuf[5];
    ADCL_function_t         *tfnct; /* temporary function */


    strcpy(tfnct_name,"meta_t");
    sprintf(itoabuf, "%d", timer->t_id);
    strcat(tfnct_name, itoabuf);
    for ( i=0; i<nemethods; i++ ) {
        tfnct = emethods[i]->em_fnctset.fs_fptrs[comb[i]]; /* same idx for emethod and function pointer OK */ 
        for ( k=timer->t_offset_reqs[i]; k<timer->t_offset_reqs[i+1]; k++) { 
           strcat(tfnct_name,"_r");
           sprintf(itoabuf, "%d", timer->t_reqs[k]->r_id);
           strcat(tfnct_name, itoabuf);
        }
        strcat(tfnct_name, "_");   
        strcat(tfnct_name, tfnct->f_name);
    }

    return ADCL_SUCCESS;
}
/***************************************************************************************************/
     

