#include "ADCL.h"

int ADCL_subarray_c ( int numdims, int *ldims, int hwidth, int *neighbors,
		      MPI_Datatype **senddats, MPI_Datatype **recvdats )
{
    int i, j, k;
    int ret = ADCL_SUCCESS;
    int *subdims=NULL, *sstarts=NULL, *rstarts=NULL;
    MPI_Datatype *sdats=NULL, *rdats=NULL;

    subdims  = ( int*) malloc ( numdims * sizeof(int) );
    if ( NULL == subdims ) {
	return ADCL_NO_MEMORY;
    }

    sstarts  = ( int*) malloc ( numdims * sizeof(int) );
    rstarts  = ( int*) malloc ( numdims * sizeof(int) );
    if ( NULL == sstarts || NULL == rstarts  ) {
	ret = ADCL_NO_MEMORY;
	goto exit;
    }

    sdats = ( MPI_Datatype *) malloc ( numdims * 2 * sizeof(int));
    rdats = ( MPI_Datatype *) malloc ( numdims * 2 * sizeof(int));
    if ( NULL == sdats || NULL == rdats  ) {
	ret = ADCL_NO_MEMORY;
	goto exit;
    }
    
    /* Loop over all dimensions */
    for ( i = 0; i < numdims; i++ ) {

	/* handle left and right neighbor separatly */
	for ( j=2*i; j<= 2*i+1; j++ ) 

	    /* Only define the datatype if the neighbor really exists */
	    if ( neighbors[j] != MPI_PROC_NULL ) {
		/* Set subdims and starts arrays. Basically,
		   subdims is in each direction the total extent of the  
		   according dimension of the data array without the halo-cells
		   except for the dimension which we are currently dealing 
		   with. For this dimension it is 1.

		   The starts arrays are 1 for all dimensions except 
		   for the dimension (lets say k)  which we are dealing with. 
		   There it is for sending:
                      - 1 for the left neighbor, 
		      - ldims[k]-2*HWIDTH for the right neighbor
		   for receiving: 
		      - 0 for the left neighbor
		      - ldims[k]-HWDITH for the right neighbor
		*/
		for ( k=0; k < numdims; k++ ) {
		    if ( k == i ) {
			subdims[k] = 1;
			sstarts[k] = (j == 2*i) ? 1 : (ldims[k]-2*hwidth);
			rstarts[k] = (j == 2*i) ? 0 : (ldims[k]-hwidth);
		    }
		    else {
			subdims[k] = ldims[k]- 2*hwidth;
			sstarts[k] = 1;
			rstarts[k] = 1;
		    }
		}
		MPI_Type_create_subarray ( numdims, ldims, subdims, sstarts,
					   MPI_ORDER_C, MPI_DOUBLE, 
					   &(sdats[j]));
		MPI_Type_create_subarray ( numdims, ldims, subdims, rstarts,
					   MPI_ORDER_C, MPI_DOUBLE, 
					   &(rdats[j]));
		MPI_Type_commit ( &(sdats[j]));
		MPI_Type_commit ( &(rdats[j]));
	    }
	    else {
		sdats[j] = MPI_DATATYPE_NULL;
		rdats[j] = MPI_DATATYPE_NULL;
	    }
    }


 exit:
    if ( ret != ADCL_SUCCESS ) {
	if ( NULL != subdims ) {
	    free ( subdims ) ;
	}
	if ( NULL != sstarts ) {
	    free ( sstarts );
	}
	if ( NULL != rstarts ) {
	    free ( rstarts );
	}
	if ( NULL != sdats ) {
	    free ( sdats );
	}
	if ( NULL != rdats ) {
	    free ( rdats );
	}
    }

    *senddats = sdats;
    *recvdats = rdats;
    return ret;
}
