#include "ADCL_internal.h"

static int ADCL_max ( int cnt, int *vecdim, int nc, int hwidth );

int ADCL_indexed_1D_init ( int vecdim, int hwidth, int nc, int order,  
			   MPI_Datatype **senddats, MPI_Datatype **recvdats)
{
    int j;
    int ret = ADCL_SUCCESS;
    int blength;
    MPI_Aint sdispl, rdispl;
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
		sdispl  = ( j== 0 ) ? hwidth * nc : (vecdim - 2*hwidth*nc);
		rdispl  = ( j== 0 ) ? 0 : (vecdim - hwidth*nc);
	    }
	    else {
	        blength = hwidth;
		sdispl  = ( j== 0 ) ? hwidth : (vecdim - 2*hwidth);
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
        MPI_Aint *sdspl, *rdspl;
	int i, *bl;

	if ( nc > 1 ) {
	    bl    = (int * )     malloc ( nc * sizeof(int));
	    sdspl = (MPI_Aint *) malloc ( 2 * nc * sizeof(MPI_Aint));
	    if (NULL == bl || NULL == sdspl ){
	        ret = ADCL_NO_MEMORY;
		return ret;
	    }
	    rdspl = &(sdspl[nc]);
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
		sdispl  = ( j== 0 ) ? hwidth : (vecdim - 2*hwidth);
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
    int j, k, maxdim;
    int ret = ADCL_SUCCESS;
    int *blength=NULL, baselen;
    MPI_Aint *sdispls=NULL, *rdispls=NULL, basedisp;
    MPI_Datatype *sdats=NULL, *rdats=NULL;

    maxdim = ADCL_max ( 2, vecdim, nc, hwidth );

    sdats   = ( MPI_Datatype *) malloc ( 2 * 4 * sizeof(MPI_Datatype));
    blength = (int *)           malloc ( maxdim * sizeof (int));
    sdispls = (MPI_Aint *)      malloc ( 2 * maxdim * sizeof(MPI_Aint));
    if ( NULL == sdats || NULL == blength || NULL == sdispls ) {
        ret = ADCL_NO_MEMORY;
	goto exit;
    }
    rdats   = &(sdats[4]);
    rdispls = &(sdispls[maxdim]);

    if ( MPI_ORDER_C == order ) {      
	/* Dimension 0 */
        baselen  = (nc < 1) ? vecdim[1]-2*hwidth : (vecdim[1]-2*hwidth) * nc;
	basedisp = (nc < 1) ? vecdim[1] : vecdim[1] * nc;

        for ( j = 0; j<2; j++ ) {
	    for ( k=1; k<=hwidth; k++ ) {
		blength[k-1] = baselen;
		sdispls[k-1] = (j == 0 ) ? k*basedisp + hwidth : 
        		                 vecdim[0]*vecdim[1] - 2*k*basedisp + hwidth;
		rdispls[k-1] = (j == 0 ) ? hwidth : 
 		                         vecdim[0]*vecdim[1] - k*basedisp + hwidth;
	    }
	    MPI_Type_indexed ( hwidth, blength, sdispls, MPI_DOUBLE, &(sdats[j]));
	    MPI_Type_indexed ( hwidth, blength, rdispls, MPI_DOUBLE, &(rdats[j]));
	    MPI_Type_commit ( &(sdats[j]) );
	    MPI_Type_commit ( &(rdats[j]) );
	}

        /* Dimension 1 */
        baselen  = (nc < 1) ? hwidth : hwidth * nc;
        basedisp = (nc < 1) ? vecdim[1] : vecdim[1] * nc;

        for ( j = 0; j<2; j++ ) {
	    for ( k=0; k< vecdim[0]-2*hwidth; k++ ) {
	        blength[k] = baselen;
		sdispls[k] = (j == 0 ) ? (k+hwidth)*basedisp+hwidth : 
		                         (k+hwidth)*basedisp + basedisp-2*hwidth;
		rdispls[k] = (j == 0 ) ? (k+hwidth)*basedisp : 
		                         (k+hwidth)*basedisp + basedisp-hwidth;
	    }
	    MPI_Type_indexed ( k, blength, sdispls, MPI_DOUBLE, &(sdats[2+j]));
	    MPI_Type_indexed ( k, blength, rdispls, MPI_DOUBLE, &(rdats[2+j]));
	    MPI_Type_commit ( &(sdats[2+j]) );
	    MPI_Type_commit ( &(rdats[2+j]) );
	}
	
	

    }
    else {
      /* MPI_ORDER_FORTRAN */
    }



 exit:
    if ( ADCL_SUCCESS != ret ) {
      if ( NULL != sdats ) {
	free ( sdats );
      }
      if ( NULL != blength ) {
	free ( blength );
      }
      if ( NULL != sdispls ) {
	free ( sdispls );
      }
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
    int i, j, maxdim;
    int ret = ADCL_SUCCESS;
    int *blength;
    MPI_Aint *sdispls, *rdispls;
    MPI_Datatype *sdats=NULL, *rdats=NULL;

    maxdim = ADCL_max ( 3, vecdim, nc, hwidth);

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
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
static int ADCL_max ( int cnt, int *vecdim, int nc, int hwidth )
{
    int i;
    int maxdim=vecdim[0];

    for ( i=1; i < cnt; i++ ) {
        if ( vecdim[i] > maxdim ) {
	    maxdim = vecdim[i];
	} 
    }
    
    if ( nc > 1 ) {
        maxdim *= nc * hwidth;
    }
    else {
        maxdim *= hwidth;
    }
    
    return maxdim;
}
