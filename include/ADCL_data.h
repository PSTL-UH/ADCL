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

#ifdef ADCL_DUMMY_MPI
#include "ADCL_dummy_mpi.h"
#else
#include "mpi.h"
#endif

struct ADCL_data_s{
    int                     d_id; /* id of the object */
    int                 d_findex; /* index of this object in the fortran array */
    int                 d_refcnt; /* reference counter of this object */
    /* Network Topology information */
    int                     d_np; /* Number of processors */
    /* Logical Topology information */
    int                 d_tndims; /* Topology number of dimensions */
    int              *d_tperiods; /* periodicity for each cartesian dimension */
    /* Vector information */
    int                 d_vndims; /* Vector number of dimensions */
    int                 *d_vdims; /* Vector extent of each the dimensions */
    int                     d_nc; /* Extent of each data point  */
   /* Vector map information */
    int                d_vectype; /* Vector type */
    int                 d_hwidth; /* Halo cells width */
    int*                 d_rcnts; /* receive counts for AllGatherV */ 
    int*                 d_displ; /* displacements for AllGatherV */
    MPI_Op                  d_op; /* MPI operator for AllReduce */
    int                d_inplace; /* MPI_IN_PLACE */
    /* Attribute information */
    int               d_asmaxnum; /* Number of attributes in the attribute set */
    int              *d_attrvals; /* Values of the winning attributes */
    /* Function set and winner function */
    char               *d_fsname; /* Function set name */
    char               *d_wfname; /* Winner function name */
    int                  d_fsnum; /* Number of available functions in the function set */
    /* Performance Data */
    double               *d_perf; /* Array of performance data of size = number of functions */
};
typedef struct ADCL_data_s ADCL_data_t;

extern ADCL_array_t *ADCL_data_array;

int  ADCL_data_create ( ADCL_emethod_t *e );
void ADCL_data_free   ( void );
int  ADCL_data_find   ( ADCL_emethod_t *e, ADCL_data_t **data );
void ADCL_data_read_from_file ( void );

#endif /* __ADCL_DATA_H__ */
