/*
 * Copyright (c) 2008-2009      HLRS. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#ifndef __ADCL_IALLGATHER_H__
#define __ADCL_IALLGATHER_H__

#if defined(c_plusplus) || defined(__cplusplus)
extern "C" {
#endif

#include "ADCL.h"
#include "ADCL_internal.h" 

#define ADCL_ATTR_IALLGATHER_TOTAL_NUM    1

#define ADCL_ATTR_IALLGATHER_ALG_MAX      1

#define ADCL_METHOD_IALLGATHER_TOTAL_NUM  1


#define ADCL_IALLGATHER_LINEAR     1

void ADCL_iallgather_linear (ADCL_request_t *req );
void ADCL_iallgather_wait( ADCL_request_t *req );


#if defined(c_plusplus) || defined(__cplusplus)
}
#endif

#endif /* __ADCL_IALLGATHER_H__ */
