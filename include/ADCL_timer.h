/*
 * Copyright (c) 2006-2007      University of Houston. All rights reserved.
 * Copyright (c) 2007           Cisco, Inc. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#ifndef __ADCL_TIMER_H__
#define __ADCL_TIMER_H__

#ifdef ADCL_DUMMY_MPI
#include "ADCL_dummy_mpi.h"
#else
#include "mpi.h"
#endif

#include "ADCL_request.h"

/* this is a meta object */
struct ADCL_timer_s{
    int                        t_id; /* unique identifier */
    int                    t_findex; /* index for the fortran interface */

    /* elements for the timer object */
    int                 t_nemethods; /* #emethods associated with a timer, 
                                        also the #functions in a meta-function */
    int                     t_nreqs; /* #request associated with a timer, t_nreqs >= t_nemethods */
    ADCL_request_t         **t_reqs; /* ordered array of requests associated with this timer, 
                                        starting with all requests of the emethod with the smallest id 
                                        up to the one with the largest id */ 
    int              *t_offset_reqs; /* array of length (t_nemethods+1) with offsets for t_reqs,
                                        t_offset_reqs[iemethod] gives the first request of iemethod,
                                        t_offset_reqs[iemethod+1]-t_offset_reqs[iemethod] the number of 
                                          requests belonging to iemethod */

    /* Elements required for the selection logic */
    ADCL_function_t     *t_function;
    ADCL_emethod_t       *t_emethod; 
    int                    t_erlast; /* last method used */
    int                    t_erflag; /* flag to be passed to the state machine */
    //TIME_TYPE              t_time; /* temporary buffer to store the exeuction
    //                                  time for dual-block operations */
    TIME_TYPE                  t_t1; /* start time */ 

    int               t_barrier_cnt; /* counter for calls to timer_start and timer_stop resp. since 
                                        MPI_barrier is only called at certain intervals during testing phase */    
};
typedef struct ADCL_timer_s ADCL_timer_t;

extern ADCL_array_t * ADCL_timer_farray;

int ADCL_timer_create ( int nreq, ADCL_request_t **reqs, ADCL_timer_t **timer );
int ADCL_timer_free ( ADCL_timer_t **timer );
int ADCL_timer_start ( ADCL_timer_t *timer );
int ADCL_timer_stop ( ADCL_timer_t *timer );
int ADCL_timer_update( ADCL_timer_t *timer, TIME_TYPE t1, TIME_TYPE t2 );

#endif /* __ADCL_TIMER_H__ */
