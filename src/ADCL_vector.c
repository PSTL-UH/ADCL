#include "ADCL.h"


static int ADCL_local_id_counter=0;
static int ADCL_vector_get_realdim ( int ndims, int *dims, int nc, 
				     int *rndims, int **rdims );

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_vector_allocate ( int ndims, int *dims, int nc, int hwidth, 
			   MPI_Datatype dat, ADCL_vector *vec )
{
    int i;
    int numints, numaddr, numdats, combiner;
    ADCL_vector_t *tvec=NULL;
    int rndims, *rdims=NULL;

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

    ADCL_vector_get_realdim ( ndims, dims, nc, &rndims, &rdims );
    tvec->v_ndims  = rndims;
    tvec->v_nc     = nc;
    tvec->v_hwidth = hwidth;
    tvec->v_dims   = rdims;

    tvec->v_alloc = TRUE;
    tvec->v_dat   = dat;

    /* allocate the according data array */
    tvec->v_data = ADCL_allocate_matrix ( rndims, rdims, dat );
    if ( NULL == tvec->v_data ) {
	free ( tvec );
	*vec = NULL;
	return ADCL_ERROR_INTERNAL;
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
    int rndims, *rdims=NULL;

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

    ADCL_vector_get_realdim ( ndims, dims, nc, &rndims, &rdims );
    tvec->v_ndims  = rndims;
    tvec->v_nc     = nc;
    tvec->v_hwidth = hwidth;
    tvec->v_dims   = rdims;

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

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/* non-public function */
static int ADCL_vector_get_realdim ( int ndims, int *dims, int nc, 
				     int *rndims, int **rdims )
{
    int *ldims=NULL;
    int i, lndims;

    if (  nc == 0 ) {
	lndims = ndims;
    }
    else {
	lndims = ndims;
    }
    
    ldims = (int *) malloc ( lndims * sizeof ( int));
    if ( NULL == ldims ) {
	return ADCL_NO_MEMORY;
    }
    for ( i=0; i<ndims; i++ ) {
	ldims[i] = dims[i];
    }

    if ( nc == 0 ) {
	ldims[lndims-1] = nc;
    }

    return ADCL_SUCCESS;
}
