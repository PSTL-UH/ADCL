/*
 * Copyright (c) 2006-2007      University of Houston. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include "ADCL.h"
#include "ADCL_internal.h"

int ADCL_Vector_allocate ( int ndims, int *dims, int nc, int hwidth, 
			   MPI_Datatype dat, void *data, ADCL_Vector *vec )
{
    int i, err;
    int numints, numaddr, numdats, combiner;

    /* Verification of the input parameters */
    if ( 0 > ndims ) {
	return ADCL_INVALID_NDIMS;
    }
    if ( NULL == dims ) {
	return ADCL_INVALID_DIMS;
    }
    for ( i=0; i<ndims; i++ ) {
	if ( 0 > dims[i] ) {
	    return ADCL_INVALID_DIMS;
	}
    }
    if ( 0 > hwidth ) {
	return ADCL_INVALID_HWIDTH;
    }
    if ( 0 > nc ) {
	return ADCL_INVALID_NC;
    }
    if ( MPI_DATATYPE_NULL == dat ) {
	return ADCL_INVALID_DAT;
    }

    if ( NULL == data ) {
	return ADCL_INVALID_ARG;
    }

    /* Datatype has to be a basic datatype */
    MPI_Type_get_envelope ( dat, &numints, &numaddr, &numdats, &combiner );
    if ( MPI_COMBINER_NAMED != combiner ) {
	return ADCL_INVALID_DAT;
    }
    if ( NULL == vec ) {
	return ADCL_INVALID_ARG;
    }

    /* Call the backend function */
    err= ADCL_vector_allocate ( ndims, dims, nc, hwidth, dat, vec );
    *((void **) data ) = ADCL_vector_get_data_ptr ( *vec );
    return err;
}

int ADCL_Vector_free  ( ADCL_Vector *vec )
{
    ADCL_vector_t *tvec;
    
    /* Verification of input parameters */
    if ( NULL == vec ) {
	return ADCL_INVALID_ARG;
    }
    tvec = *vec;
    if ( TRUE != tvec->v_alloc ) {
	/* this object has been registered with ADCL_vector_register and
	   should therefore use ADCL_vector_deregister instead of the current
	   routine! */
	return ADCL_INVALID_DATA;
    }

    return ADCL_vector_free ( vec );
}

int ADCL_Vector_register ( int ndims, int *dims, int nc, int hwidth, 
			   MPI_Datatype dat, void *data, 
			   ADCL_Vector *vec )
{
    int i;
    int numints, numaddr, numdats, combiner;

    /* Verification of the input parameters */
    if ( 0 > ndims ) {
	return ADCL_INVALID_NDIMS;
    }
    if ( NULL == dims ) {
	return ADCL_INVALID_DIMS;
    }
    for ( i=0; i<ndims; i++ ) {
	if ( 0 > dims[i] ) {
	    return ADCL_INVALID_DIMS;
	}
    }
    if ( 0 > hwidth ) {
	return ADCL_INVALID_HWIDTH;
    }
    if ( 0 > nc ) {
	return ADCL_INVALID_NC;
    }
    if ( MPI_DATATYPE_NULL == dat ) {
	return ADCL_INVALID_DAT;
    }

    /* Datatype has to be a basic datatype */
    MPI_Type_get_envelope ( dat, &numints, &numaddr, &numdats, &combiner );
    if ( MPI_COMBINER_NAMED != combiner ) {
	return ADCL_INVALID_DAT;
    }
    if ( NULL == data ) {
	return ADCL_INVALID_DATA;
    }
    if ( NULL == vec ) {
	return ADCL_INVALID_ARG;
    }

    return ADCL_vector_register ( ndims, dims, nc, hwidth, dat, data, vec );
}

int ADCL_Vector_deregister  ( ADCL_Vector *vec )
{
    ADCL_vector_t *tvec;
    
    /* Verification of input parameters */
    if ( NULL == vec ) {
	return ADCL_INVALID_ARG;
    }
    tvec = *vec;
    if ( FALSE != tvec->v_alloc ) {
	/* this object has been allocated with ADCL_vector_allocate and
	   should therefore use ADCL_vector_free instead of the current
	   routine! */
	return ADCL_INVALID_DATA;
    }

    return ADCL_vector_deregister ( vec );
}
