#include "ATF.h"
#include "ATF_Memory.h"

#include "ADCL.h"
#include "ADCL_Global.h"
/* global variables for ATF_Init_matrix */

int ATF_n1g;
int ATF_n2g;
int ATF_n3g;

int ATF_dim[3];

double *****ATF_rm000;
double *****ATF_rmb00;
double *****ATF_rmf00;
double *****ATF_rm0b0;
double *****ATF_rm0f0;
double *****ATF_rm00b;
double *****ATF_rm00f;

double ****ATF_rhs;
double ****ATF_dq;
double ****ATF_loes;

double ATF_solv_ende, ATF_solv_anfang, ATF_solvtime;
double ATF_comm_ende, ATF_comm_anfang, ATF_commtime;

/*Set the matrix , rhs to zero */
int ATF_Reset()
{
    int i,j,k,l,n, nc;

    nc =1;

    /*rm000, rmb00......*/
    /*The boundary is also set to zero*/
    
    for( i=0; i<= ATF_dim[0]+1; i++){
        for( j=0; j <= ATF_dim[1]+1; j++){
            for( k=0; k<=ATF_dim[2]+1; k++){
                for( l=0; l < nc; l++){
		    ATF_loes[i][j][k][l] = 0.0;
		    ATF_dq[i][j][k][l] = 0.0;
                    for(n=0; n < nc; n++){
			ATF_rm000[i][j][k][l][n] = 0.0;
			ATF_rmb00[i][j][k][l][n] = 0.0;
			ATF_rmf00[i][j][k][l][n] = 0.0;
			ATF_rm0b0[i][j][k][l][n] = 0.0;
			ATF_rm0f0[i][j][k][l][n] = 0.0;
			ATF_rm00b[i][j][k][l][n] = 0.0;
			ATF_rm00f[i][j][k][l][n] = 0.0;
                    }
		}
            }
        }            
    }

    /*Rhs */
    for(i=1; i < ATF_dim[0]+1; i++){
        for(j=1; j< ATF_dim[1]+1; j++){
            for(k=1; k< ATF_dim[2]+1; k++){
                for(l=0; l<nc; l++){
                    ATF_rhs[i][j][k][l] = 1.0;
		}
            }
	}
    }
    
    return ATF_SUCCESS;
}

/*Allocate and initialize matrix and vectors*/

ADCL_Vector ADCL_Vec_dq;
ADCL_Vector ADCL_Vec_loes;

ADCL_Request ADCL_Req_dq;
ADCL_Request ADCL_Req_loes;
MPI_Comm ADCL_Cart_comm;

int ATF_Init_matrix(int px, int py, int pz)
{
    /* Maximu is 5,so I define 5 dimension. */
    int dim5[5];
    int dim4[4];
    int dim3[3];
    
    int rank,size;
    int nc;

    /*New added*/
    
    nc =0;
    int cdims[]={0,0,0};
    int periods[] = {0,0,0};
    
    ATF_n1g = px;
    ATF_n2g = py;
    ATF_n3g = pz;
  
    /* px = py = pz = 32 */
    /* ATF_np=2, 1, 1 */
    ATF_dim[0] = ATF_n1g/ATF_np[0];
    ATF_dim[1] = ATF_n2g/ATF_np[1];
    ATF_dim[2] = ATF_n3g/ATF_np[2];
    
    
    MPI_Comm_rank ( MPI_COMM_WORLD, &rank);
    MPI_Comm_size( MPI_COMM_WORLD, &size);
   
    /* Allocate dynamic arrays */

    dim5[0] = ATF_dim[0]+2;
    dim5[1] = ATF_dim[1]+2;
    dim5[2] = ATF_dim[2]+2;	
    dim5[3] = 1;
    dim5[4] = 1;
    
    ATF_allocate_5D_double_matrix(&ATF_rm000,dim5 );
    ATF_allocate_5D_double_matrix(&ATF_rmb00,dim5 );
    ATF_allocate_5D_double_matrix(&ATF_rmf00,dim5 );
    ATF_allocate_5D_double_matrix(&ATF_rm0b0,dim5 );
    ATF_allocate_5D_double_matrix(&ATF_rm0f0,dim5 );
    ATF_allocate_5D_double_matrix(&ATF_rm00b,dim5 );
    ATF_allocate_5D_double_matrix(&ATF_rm00f,dim5 );
    
    dim4[0] = ATF_dim[0]+2;
    dim4[1] = ATF_dim[1]+2;
    dim4[2] = ATF_dim[2]+2;
    dim4[3] = 1;

    ATF_allocate_4D_double_matrix(&ATF_rhs,dim4);
    ATF_allocate_4D_double_matrix(&ATF_dq,dim4);
    ATF_allocate_4D_double_matrix(&ATF_loes,dim4);


    /*Generate an ADCL-Vector object out of dq and loes*/
    dim3[0] = ATF_dim[0]+2;
    dim3[1] = ATF_dim[1]+2;
    dim3[2] = ATF_dim[2]+2;

    /*Vector register here!*/
    ADCL_Vector_register( 3, dim3, nc, 1, MPI_DOUBLE, ATF_dq, &ADCL_Vec_dq );
    ADCL_Vecotr_register( 3, dim3, nc, 1, MPI_DOUBLE, ATF_loes, &ADCL_Vec_loes );

    /*Describe the neigborhood relations*/
    MPI_Dims_create( size, 3, cdims );
    MPI_Cart_create( MPI_COMM_WORLD, 3, cdims, periods, 0, &ADCL_Cart_comm );
    
    /*Generate now the ADC_Request object for dq*/
    /*I thinks it should be later!*/
    ADCL_Request_create( ADCL_Vec_dq, ADCL_Cart_comm, &ADCL_Req_dq );
    ADCL_Request_create( ADCL_Vec_loes, ADCL_Cart_comm, &ADCL_Req_loes );
      
    
    /*Not done yet*/    
    if(!ATF_Set_Datatypes(ATF_rhs)){
		printf("Set data types is not correct\n");
		return ATF_ERROR;
    }
    
    /* Initiate timing variables */
    ATF_solv_ende   = 0.0 ;
    ATF_solv_anfang = 0.0;
    ATF_solvtime    = 0.0;
    ATF_comm_ende   = 0.0;
    ATF_comm_anfang = 0.0;
    ATF_commtime    = 0.0;

    /* Reset matrix and rhs to zero */
    if(!ATF_Reset()){
	printf("Reset Matrix and rhs zero, error!\n");
	return ATF_ERROR;
    }

    
    /*Set matrix and rhs*/
    /*The dimension is 5*/
    if(!ATF_Set()){
	printf("Set matrix error!\n");
	return ATF_ERROR;
    }
    
    /*   Initiate the rhs   */
    if(!ATF_Set_rhs(ATF_dim)){
	printf("Set matrix error!\n");
	return ATF_ERROR;
    }
    
    return ATF_SUCCESS;
}
