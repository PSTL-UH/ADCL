#include "ATF.h"
#include "ATF_Memory.h"


#include "ADCL_Global.h"

int ATF_Free_matrix()
{
  ATF_free_5D_double_matrix(&ATF_rm000);
  ATF_free_5D_double_matrix(&ATF_rmb00);
  ATF_free_5D_double_matrix(&ATF_rmf00);
  ATF_free_5D_double_matrix(&ATF_rm0b0);
  ATF_free_5D_double_matrix(&ATF_rm0f0);
  ATF_free_5D_double_matrix(&ATF_rm00b);
  ATF_free_5D_double_matrix(&ATF_rm00f);

  ATF_free_4D_double_matrix(&ATF_rhs);
  ATF_free_4D_double_matrix(&ATF_dq);
  ATF_free_4D_double_matrix(&ATF_loes);

  ADCL_Request_free( &ADCL_Req_dq );
  ADCL_Request_free( &ADCL_Req_loes );
  
  ADCL_Vector_deregister( &ADCL_Vec_dq);
  ADCL_Vector_deregister( &ADCL_Vec_loes);
  

  return ATF_SUCCESS;  
}

