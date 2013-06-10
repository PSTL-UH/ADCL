
#ifndef __ADCL_IBCAST__
#define __ADCL_IBCAST__

#if defined(c_plusplus) || defined(__cplusplus)
extern "C" {
#endif

#include "ADCL.h"
#include "ADCL_internal.h"

#define ADCL_ATTR_IBCAST_TOTAL_NUM  2
#define ADCL_ATTR_IBCAST_ALG_MAX 4
#define ADCL_ATTR_IBCAST_FANOUT 5

#define ADCL_IBCAST_LINEAR 1
#define ADCL_IBCAST_BINOMIAL 2
#define ADCL_IBCAST_CHAIN 3
#define ADCL_IBCAST_GENERIC 4

#define ADCL_METHOD_IBCAST_TOTAL_NUM 8

void ADCL_ibcast_linear( ADCL_request_t *req );
void ADCL_ibcast_binomial (ADCL_request_t *req );
void ADCL_ibcast_chain( ADCL_request_t *req );
void ADCL_ibcast_generic( ADCL_request_t *req );
void ADCL_ibcast_wait( ADCL_request_t *req );

#if defined(c_plusplus) || defined(__cplusplus)
}
#endif

#endif
