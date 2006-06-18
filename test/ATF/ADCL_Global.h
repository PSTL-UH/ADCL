#ifndef ADCL_GLOBAL_H
#define ADCL_GLOBAL_H
#include "ADCL.h"


extern ADCL_Vector ADCL_Vec_dq;
extern ADCL_Vector ADCL_Vec_loes;


extern ADCL_Request ADCL_Req_dq;
extern ADCL_Request ADCL_Req_loes;

extern MPI_Comm ADCL_Cart_comm;

int ADCL_Matmul(ADCL_Request , double ****, double ****);
  
#endif

