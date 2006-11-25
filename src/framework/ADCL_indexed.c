#include "ADCL_internal.h"

int ADCL_indexed_1D_init ( int vecdim, int hwidth, int nc, int order,  
			   MPI_Datatype **senddats, MPI_Datatype **recvdats)
{
    int j;
    int ret = ADCL_SUCCESS;
    int blength;
    MPI_Aint sdispls, rdispls;
    MPI_Datatype *sdats=NULL, *rdats=NULL;

    sdats = ( MPI_Datatype *) malloc ( 2 * sizeof(MPI_Datatype));
    rdats = ( MPI_Datatype *) malloc ( 2 * sizeof(MPI_Datatype));
    if ( NULL == sdats || NULL == rdats  ) {
	ret = ADCL_NO_MEMORY;
	return ret;
    }


    if ( MPI_ORDER_C == order ) {
        for ( j = 0; j<2; j++ ) {
	    if ( nc > 1 ) {
	        blength = hwidth * nc;
		sdispl  = ( j== 0 ) ? hwdith * nc : (vecdim - 2*hwidth*nc);
		rdispl  = ( j== 0 ) ? 0 : (vecdim - hwidth*nc);
	    }
	    else {
	        blength = hwidth;
		sdispl  = ( j== 0 ) ? hwdith : (vecdim - 2*hwidth);
		rdispl  = ( j== 0 ) ? 0 : (vecdim - hwidth);
	    }
	    MPI_Type_indexed ( 1, &blength, &sdispl, MPI_DOUBLE, &(sdats[j]));
	    MPI_Type_indexed ( 1, &blength, &rdispl, MPI_DOUBLE, &(rdats[j]));

	    MPI_Type_commit ( &(sdats[j]));
	    MPI_Type_commit ( &(rdats[j]));
	}
    }
    else {
        /* MPI_ORDER_FORTRAN */
        MPI_Aint *sdspl, *rdslp;
	int *bl;

	if ( nc > 1 ) {
	    bl = (int * ) malloc ( nc * sizeof(int));
	    sdspl = (MPI_Aint *) malloc ( nc * sizeof(MPI_Aint));
	    rdspl = (MPI_Aint *) malloc ( nc * sizeof(MPI_Aint));
	    if (NULL == bl || NULL == sdspl || NULL == rdspl){
	        ret = ADCL_NO_MEMORY;
		return ret;
	    }
	}

	for ( j = 0; j<2; j++ ) {

	    if ( nc > 1 ) {
	        for ( i = 0; i< nc; i++ ) {
		    bl[i] = hwidth;
		    
		    sdspl[i]=(j==0) ? (hwidth + i * vecdim) : (((i+1)*vecdim) - 2*hwidth);
		    rdspl[i]=(j==0) ? (i*vecdim) : (((i+1)*vecdim) - hwidth);

		    MPI_Type_indexed ( nc, bl, sdspl, MPI_DOUBLE, &(sdats[j]));
		    MPI_Type_indexed ( nc, bl, rdspl, MPI_DOUBLE, &(rdats[j]));
		}
	    }
	    else {
	        blength = hwidth;
		sdispl  = ( j== 0 ) ? hwdith : (vecdim - 2*hwidth);
		rdispl  = ( j== 0 ) ? 0 : (vecdim - hwidth);

		MPI_Type_indexed ( 1, &blength, &sdispl, MPI_DOUBLE, &(sdats[j]));
		MPI_Type_indexed ( 1, &blength, &rdispl, MPI_DOUBLE, &(rdats[j]));
	    }
	    
	    MPI_Type_commit ( &(sdats[j]));
	    MPI_Type_commit ( &(rdats[j]));
	}

	if ( nc > 1 ) {
	    free ( bl );
	    free ( sdspl );
	    free ( rdspl );
	}
    }

    *senddats = sdats;
    *recvdats = rdats;
    return ret;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_indexed_2D_init ( int *vecdim, int hwidth, int nc, int order,  
			   MPI_Datatype **senddats, MPI_Datatype **recvdats)
{
    int j;
    int ret = ADCL_SUCCESS;
    int blength;
    MPI_Aint sdispls, rdispls;
    MPI_Datatype *sdats=NULL, *rdats=NULL;

    sdats = ( MPI_Datatype *) malloc ( 4 * sizeof(MPI_Datatype));
    rdats = ( MPI_Datatype *) malloc ( 4 * sizeof(MPI_Datatype));

    if ( NULL == sdats || NULL == rdats  ) {
	ret = ADCL_NO_MEMORY;
	return ret;
    }


    if ( MPI_ORDER_C == order ) {
        for ( j = 0; j<2; j++ ) {
	}
    }
    else {
      /* MPI_ORDER_FORTRAN */
    }

    *senddats = sdats;
    *recvdats = rdats;
    return ret;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_indexed_3D_init ( int *vecdim, int hwidth, int nc, int order,  
			   MPI_Datatype **senddats, MPI_Datatype **recvdats)
{
    int j;
    int ret = ADCL_SUCCESS;
    int blength;
    MPI_Aint sdispls, rdispls;
    MPI_Datatype *sdats=NULL, *rdats=NULL;

    sdats = ( MPI_Datatype *) malloc ( 6 * sizeof(MPI_Datatype));
    rdats = ( MPI_Datatype *) malloc ( 6 * sizeof(MPI_Datatype));
    if ( NULL == sdats || NULL == rdats  ) {
	ret = ADCL_NO_MEMORY;
	return ret;
    }


    if ( MPI_ORDER_C == order ) {
        for ( j = 0; j<2; j++ ) {
	}
    }
    else {
      /* MPI_ORDER_FORTRAN */
    }
    *senddats = sdats;
    *recvdats = rdats;
    return ret;
}

