#ifndef ADCL_GLOBAL_H
#define ADCL_GLOBAL_H
#include "ADCL.h"


extern ADCL_Vector adcl_Vec_dq;
extern ADCL_Vector adcl_Vec_loes;
extern ADCL_Vector adcl_Vec_rhs;

extern ADCL_Request adcl_Req_dq;
extern ADCL_Request adcl_Req_loes;
extern ADCL_Request adcl_Req_rhs;

extern MPI_Comm ADCL_Cart_comm;

extern int ADCL_OUTLITER_FACTOR;
extern int ADCL_OUTLITER_FRACTION;

int ATF_Matmul(ADCL_Request , double ****, double ****);
int ATF_Read_Env(void);

#endif

