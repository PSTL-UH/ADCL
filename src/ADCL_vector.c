#include "ADCL.h"


static int ADCL_local_id_counter=0;

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_vector_allocate ( int ndims, int *dims, int nc, int hwidth, 
			   MPI_Datatype dat, ADCL_vector *vec )
{
    int i;
    int numints, numaddr, numdats, combiner;
    ADCL_vector_t *tvec;

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
    if ( MPI_DATATYPE_NULL == dat ) {
	return ADCL_INVALID_DAT;
    }
    /* Datatype has to be a basic datatype */
    MPI_Type_get_envelope ( dat, &numints, &numaddr, &numdats, &combiner );
    if ( MPI_COMBINER_NAMED != combiner ) {
	return ADCL_INVALID_DAT;
    }
    if ( NULL == vec ) {
	return ADCL_INVALID_ARG;
    }

    /* Allocate a new vector object */
    tvec = (ADCL_vector_t *) calloc ( 1, sizeof (ADCL_vector_t) );
    if ( NULL == tvec ) {
	return ADCL_NO_MEMORY;
    }
    
    /* Set the according elements of the structure */
    tvec->v_id     = ADCL_local_id_counter++; 
    tvec->v_rfcnt  = 1;
    tvec->v_ndims  = ndims;
    tvec->v_nc     = nc;
    tvec->v_hwidth = hwidth;
    tvec->v_dims   = (int *) malloc ( ndims * sizeof(int));
    if ( NULL == tvec->v_dims ) {
	free ( tvec );
	return ADCL_NO_MEMORY;
    }
    for (i=0; i<ndims; i++ ) {
	tvec->v_dims[i] = dims[i];
    }

    tvec->v_alloc = TRUE;
    tvec->v_dat   = dat;

    /* TODO: add the function call to allocate the according data array */
    if ( dat == MPI_DOUBLE ) {
	if ( ndims == 3 ) {
	    double ****lvec;
	    int ldims[4];

	    ldims[0] = dims[0];
	    ldims[1] = dims[1];
	    ldims[2] = dims[2];
	    ldims[3] = nc;
	    
	    ADCL_allocate_4D_double_matrix ( &lvec, ldims);
	    tvec->v_data = &(lvec[0][0][0][0]);
	}
	else {
	    printf("dimension not supported right now!\n");
	}
    }
    else {
	printf("Datatype not supported right now!\n");
    }
	    

    *vec = tvec;
    return ADCL_SUCCESS;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_vector_free  ( ADCL_vector *vec )
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

    tvec->v_rfcnt--;
    if ( tvec->v_rfcnt == 0 ) {
	if ( NULL != tvec->v_dims ) {
	    free ( tvec->v_dims);
	}
	/* TODO: free the data pointer */
	
	free ( tvec );
    }
    else if ( tvec->v_rfcnt < 0 ) {
	return ADCL_ERROR_INTERNAL;
    }

    *vec = ADCL_VECTOR_NULL;
    return ADCL_SUCCESS;
}


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_vector_register ( int ndims, int *dims, int nc, int hwidth, 
			   MPI_Datatype dat, void *data, 
			   ADCL_vector *vec )
{
    int i;
    int numints, numaddr, numdats, combiner;
    ADCL_vector_t *tvec;

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
    if ( MPI_DATATYPE_NULL == dat ) {
	return ADCL_INVALID_DAT;
    }
    /* Datatype has to be a basic datatype */
    MPI_Type_get_envelope ( dat, &numints, &numaddr, &numdats, &combiner );
    if ( MPI_COMBINER_NAMED == combiner ) {
	return ADCL_INVALID_DAT;
    }
    if ( NULL == data ) {
	return ADCL_INVALID_DATA;
    }
    if ( NULL == vec ) {
	return ADCL_INVALID_ARG;
    }

    /* Allocate a new vector object */
    tvec = (ADCL_vector_t *) malloc (sizeof(ADCL_vector_t) );
    if ( NULL == tvec ) {
	return ADCL_NO_MEMORY;
    }
    
    /* Set the according elements of the structure */
    tvec->v_id     = ADCL_local_id_counter++; 
    tvec->v_rfcnt  = 1;
    tvec->v_alloc  = FALSE;
    tvec->v_ndims  = ndims;
    tvec->v_nc     = nc;
    tvec->v_hwidth = hwidth;
    tvec->v_dims   = (int *) malloc ( ndims * sizeof(int));
    if ( NULL == tvec->v_dims ) {
	free ( tvec );
	return ADCL_NO_MEMORY;
    }
    for (i=0; i<ndims; i++ ) {
	tvec->v_dims[i] = dims[i];
    }
    tvec->v_data = data;
    tvec->v_dat  = dat;

    /* Finally, set the returning handle correctly */
    *vec = tvec;
    return ADCL_SUCCESS;
}


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_vector_deregister  ( ADCL_vector *vec )
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

    tvec->v_rfcnt--;
    if ( tvec->v_rfcnt == 0 ) {
	if ( NULL != tvec->v_dims ) {
	    free ( tvec->v_dims );
	}
	free ( tvec );
    } 
    else if ( tvec->v_rfcnt < 0 ) {
	return ADCL_ERROR_INTERNAL;
    }
    
    *vec = ADCL_VECTOR_NULL;
    return ADCL_SUCCESS;
}



