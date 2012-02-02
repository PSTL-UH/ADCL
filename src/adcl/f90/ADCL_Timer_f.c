/*
 * Copyright (c) 2009           HLRS. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include "ADCL.h"
#include "ADCL_internal.h"
#include "ADCL_fprototypes.h"

#ifndef _SX
#pragma weak adcl_timer_create_  = adcl_timer_create
#pragma weak adcl_timer_create__ = adcl_timer_create
#pragma weak ADCL_TIMER_CREATE   = adcl_timer_create

#pragma weak adcl_timer_free_  = adcl_timer_free
#pragma weak adcl_timer_free__ = adcl_timer_free
#pragma weak ADCL_TIMER_FREE   = adcl_timer_free

#pragma weak adcl_timer_start_  = adcl_timer_start
#pragma weak adcl_timer_start__ = adcl_timer_start
#pragma weak ADCL_TIMER_START   = adcl_timer_start

#pragma weak adcl_timer_stop_  = adcl_timer_stop
#pragma weak adcl_timer_stop__ = adcl_timer_stop
#pragma weak ADCL_TIMER_STOP   = adcl_timer_stop
#endif

/**********************************************************************/
#ifdef _SX
void adcl_timer_create_ ( int* nreq, int *reqs, int* timer, int* ierror )
#else
void adcl_timer_create  ( int* nreq, int *reqs, int* timer, int* ierror )
#endif
/**********************************************************************/
{
    ADCL_timer_t *ttimer=NULL;
    ADCL_request_t **creqs = NULL;
    MPI_Comm refcomm;
    int relation, i;

    if ( 0    >= *nreq ||
         NULL == reqs  ) {
       *ierror = ADCL_INVALID_ARG;
       return;
    }

    /* are requests undefined or their emethods already associated with other timers? */
    creqs = (ADCL_request_t **) malloc ( *nreq * sizeof(ADCL_request_t *) );
    for (i=0; i<*nreq; i++){
         if ( 0 > reqs[i] || ADCL_FREQUEST_NULL == reqs[i] ) {
            *ierror = ADCL_INVALID_REQUEST;
            goto exit;
         }

         creqs[i] = (ADCL_request_t *) ADCL_array_get_ptr_by_pos ( ADCL_request_farray, reqs[i] );

         if ( creqs[i]->r_emethod->em_assoc_with_timer > -1 ) { 
            *ierror = ADCL_INVALID_EMETHOD; 
            goto exit;
         }
    } 
   
    /* see if their are mismatches in topology */ 
    refcomm = creqs[0]->r_emethod->em_topo->t_comm; 
    for (i=1; i<*nreq; i++){
        MPI_Comm_compare ( refcomm, creqs[i]->r_emethod->em_topo->t_comm, &relation );
        if ( ( relation != MPI_IDENT) && (relation != MPI_CONGRUENT) ) {
            *ierror = ADCL_INVALID_TOPOLOGY;
            goto exit;
        }
    } 

    *ierror = ADCL_timer_create ( *nreq, creqs, &ttimer );

    if ( ADCL_SUCCESS  == *ierror ) {
        *timer = ttimer->t_findex;
    }

exit:
    if ( NULL != creqs ) free (creqs);
    return;

}

/**********************************************************************/
#ifdef _SX
void adcl_timer_free_ ( int *timer, int *ierror )
#else
void adcl_timer_free  ( int *timer, int *ierror )
#endif
/**********************************************************************/
{
    ADCL_timer_t *ctimer=NULL;

    if ( NULL == timer ) {
       *ierror = ADCL_INVALID_ARG;
       return;
    }

    ctimer = (ADCL_timer_t *) ADCL_array_get_ptr_by_pos ( ADCL_timer_farray,
                                                              *timer );

    *ierror = ADCL_timer_free ( &ctimer );

    *timer = ADCL_FTIMER_NULL;

    return;
}

/**********************************************************************/
#ifdef _SX
void adcl_timer_start_ ( int *timer, int *ierror )
#else
void adcl_timer_start  ( int *timer, int *ierror )
#endif
/**********************************************************************/
{
    ADCL_timer_t *ctimer=NULL;

    if ( NULL == timer ) {
       *ierror = ADCL_INVALID_ARG;
       return;
    }

    ctimer = (ADCL_timer_t *) ADCL_array_get_ptr_by_pos ( ADCL_timer_farray,
                                                              *timer );

    *ierror = ADCL_timer_start ( ctimer );

    return;
}

/**********************************************************************/
#ifdef _SX
void adcl_timer_stop_ ( int *timer, int *ierror )
#else
void adcl_timer_stop  ( int *timer, int *ierror )
#endif
/**********************************************************************/
{
    ADCL_timer_t *ctimer=NULL;

    if ( NULL == timer ) {
       *ierror = ADCL_INVALID_ARG;
       return;
    }

    ctimer = (ADCL_timer_t *) ADCL_array_get_ptr_by_pos ( ADCL_timer_farray,
                                                              *timer );

    *ierror = ADCL_timer_stop ( ctimer );

    return;
}

