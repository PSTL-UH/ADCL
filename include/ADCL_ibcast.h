#ifndef __ADCL_IBCAST__
#define __ADCL_IBCAST__

#if defined(c_plusplus) || defined(__cplusplus)
extern "C" {
#endif

#include "ADCL.h"
#include "ADCL_internal.h"

#define NBC_BCAST_LINEAR 1
#define NBC_BCAST_BINOMIAL 2
#define NBC_BCAST_CHAIN 3

#define ADCL_METHOD_IBCAST_TOTAL_NUM 3

void ADCL_ibcast_linear( ADCL_request_t *req );
void ADCL_ibcast_binomial (ADCL_request_t *req );
void ADCL_ibcast_chain( ADCL_request_t *req );
void ADCL_ibcast_wait( ADCL_request_t *req );

#if defined(c_plusplus) || defined(__cplusplus)
}
#endif

#endif
