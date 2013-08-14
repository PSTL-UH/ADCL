/*
 * Copyright (c) 2006-2013      University of Houston. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include "ADCL.h"
#include "ADCL_internal.h"
#include "ADCL_fprototypes.h"

/* Note: Vector_allocate, vector_free and vector_get_data_ptr are
   intentionally not offered in the fortran interface. */

#ifndef _SX
#pragma weak adcl_vector_register_generic_  = adcl_vector_register_generic
#pragma weak adcl_vector_register_generic__ = adcl_vector_register_generic
#pragma weak ADCL_VECTOR_REGISTER_GENERIC   = adcl_vector_register_generic

#pragma weak adcl_vector_deregister_  = adcl_vector_deregister
#pragma weak adcl_vector_deregister__ = adcl_vector_deregister
#pragma weak ADCL_VECTOR_DEREGISTER   = adcl_vector_deregister

#endif


#ifdef _SX
void adcl_vector_register_generic_ ( int *ndims, int *dims, int *nc, int *vmap,
                           int *dat, void *data, int *vec, int *ierror )
#else
void adcl_vector_register_generic  ( int *ndims, int *dims, int *nc, int *vmap,
                           int *dat, void *data, int *vec, int* ierror )
#endif
{
    int i;
    ADCL_vmap_t *cvmap;
    ADCL_vector_t *cvec;
    MPI_Datatype cdat;

    /* Verification of the input parameters */
    /* check first for vmap */
    if ( NULL == vmap ) {
       *ierror = ADCL_INVALID_VMAP;
       return;
    }
    cvmap = (ADCL_vmap_t *) ADCL_array_get_ptr_by_pos ( ADCL_vmap_farray, *vmap );
    if ( ADCL_VECTOR_INPLACE == cvmap->m_vectype ) {
       /* how do I verify that the user set data == MPI_IN_PLACE? */
       data = MPI_IN_PLACE;
       goto allocate;
    }

    /* if it is not ADCL_VECTOR_INPLACE, check rest of input parameters */
    if ( ( NULL == ndims ) ||
         ( NULL == dims )  ||
         ( NULL == nc   )  ||
         ( NULL == dat )   ||
         ( NULL == data )  ||
         ( NULL == vec )   ) {
        *ierror = ADCL_INVALID_ARG;
        return;
    }

    /* Verification of the input parameters */
    if ( 0 > *ndims ) {
        *ierror = ADCL_INVALID_NDIMS;
        return;
    }
    for ( i=0; i<*ndims; i++ ) {
        if ( 0 > dims[i] ) {
            *ierror = ADCL_INVALID_DIMS;
            return;
        }
    }
    if ( 0 > *nc ) {
        *ierror = ADCL_INVALID_NC;
        return;
    }

    cdat = MPI_Type_f2c (*dat);
    if ( cdat == MPI_DATATYPE_NULL ) {
        *ierror = ADCL_INVALID_TYPE;
        return;
    }

    if ( cdat == MPI_DOUBLE_PRECISION ) {
        cdat = MPI_DOUBLE;
    }
    else if ( cdat == MPI_REAL ) {
        cdat = MPI_FLOAT;
    }
    else if ( cdat == MPI_INTEGER ) {
        cdat = MPI_INT;
    }

allocate:
    *ierror = ADCL_vector_register_generic ( *ndims, dims, *nc, cvmap, cdat, data, &cvec );
    if ( *ierror == ADCL_SUCCESS ) {
        *vec = cvec->v_findex;
    }


}


#ifdef _SX
void adcl_vector_deregister_( int *vec, int *ierror )
#else
void adcl_vector_deregister ( int *vec, int *ierror )
#endif
{
    ADCL_vector_t *cvec;

    cvec = (ADCL_vector_t *) ADCL_array_get_ptr_by_pos ( ADCL_vector_farray,
                                                         *vec );
    *ierror = ADCL_vector_deregister ( &cvec );

    *vec = ADCL_FVECTOR_NULL;
    return;
}

