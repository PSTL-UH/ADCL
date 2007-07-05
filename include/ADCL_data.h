/*
 * Copyright (c) 2006-2007      University of Houston. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#ifndef __ADCL_DATA_H__
#define __ADCL_DATA_H__

#if ADCL_DUMMY_MPI
#include "ADCL_dummy_mpi.h"
#else
#include "mpi.h"
#endif

struct ADCL_data_s{
    int                     d_id; /* id of the object */
    int                 d_findex; /* index of this object in the fortran array */
    int                 d_refcnt; /* reference counter of this object */
    /* Topology information */
    MPI_Comm              d_comm;
    int                 d_tndims;
    int             *d_neighbors; /* array of neighboring processes, dim 2*t_ndims */
    /* Vector information */
    int                 d_vndims;
    int                 *d_vdims;
    int                     d_nc;
    int                 d_hwidth;
    int                d_comtype;
    /* Function set and winner function */
    ADCL_fnctset_t    *d_fnctset; /* pointer to the function set.*/
    ADCL_function_t *d_wfunction; /* contain the 'winner' function once the
                                     evaluation of that problem is over. */
};
typedef struct ADCL_data_s ADCL_data_t;

extern ADCL_array_t *ADCL_data_array;

int  ADCL_data_create ( ADCL_emethod_t *e );
void ADCL_data_free   ( void );
int  ADCL_data_find   ( ADCL_emethod_t *e, ADCL_data_t **data );

#endif /* __ADCL_DATA_H__ */
