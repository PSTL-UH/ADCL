#include "ADCL.h"
#include "ADCL_internal.h"
#include "ADCL_fprototypes.h"

/* Note: Vector_allocate, vector_free and vector_get_data_ptr are 
   intentionally not offered in the fortran interface. */

#pragma weak adcl_vector_register_  = adcl_vector_register
#pragma weak adcl_vector_register__ = adcl_vector_register
#pragma weak ADCL_VECTOR_REGISTER   = adcl_vector_register

#pragma weak adcl_vector_deregister_  = adcl_vector_deregister
#pragma weak adcl_vector_deregister__ = adcl_vector_deregister
#pragma weak ADCL_Vector_Deregister   = adcl_vector_deregister

void adcl_vector_register ( int *ndims, int *dims, int *nc, int *hwidth, 
			    int *dat, void *data, int *vec, int *ierror)
{
    ADCL_vector_t *cvec;
    MPI_Datatype cdat;

    if ( ( NULL == ndims ) || 
	 ( NULL == dims )  ||
	 ( NULL == nc   )  ||
	 ( NULL == hwidth) ||
	 ( NULL == dat )   ||
	 ( NULL == data )  ||
	 ( NULL == vec )   ) {
	*ierror = ADCL_INVALID_ARG;
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

    *ierror = ADCL_vector_register (*ndims, dims, *nc, *hwidth, cdat, 
				    data, &cvec );
    if ( *ierror == ADCL_SUCCESS ) {
	*vec = cvec->v_findex;
    } 

    return;
}

void adcl_vector_deregister ( int *vec, int *ierror )
{
    ADCL_vector_t *cvec;

    cvec = (ADCL_vector_t *) ADCL_array_get_ptr_by_pos ( ADCL_vector_farray, 
							 *vec );
    *ierror = ADCL_vector_deregister ( &cvec );

    return;
}

