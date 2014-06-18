/*
 * Copyright (c) 2008-2009      HLRS. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#ifndef __ADCL_IREDUCE_H__
#define __ADCL_IREDUCE_H__

#if defined(c_plusplus) || defined(__cplusplus)
extern "C" {
#endif

#include "ADCL.h"
#include "ADCL_internal.h" 

#define ADCL_ATTR_IREDUCE_TOTAL_NUM    1

#define ADCL_ATTR_IREDUCE_ALG_MAX      2

#define ADCL_METHOD_IREDUCE_TOTAL_NUM  2


#define ADCL_IREDUCE_BINOMIAL     1
#define ADCL_IREDUCE_CHAIN     2

void ADCL_ireduce_binomial (ADCL_request_t *req );
void ADCL_ireduce_chain (ADCL_request_t *req );
void ADCL_ireduce_wait( ADCL_request_t *req );


#if defined(c_plusplus) || defined(__cplusplus)
}
#endif

#endif /* __ADCL_IREDUCE_H__ */
