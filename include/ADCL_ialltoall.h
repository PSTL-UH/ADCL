/*
 * Copyright (c) 2008-2009      HLRS. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#ifndef __ADCL_IALLTOALL_H__
#define __ADCL_IALLTOALL_H__

#if defined(c_plusplus) || defined(__cplusplus)
extern "C" {
#endif

#include "ADCL.h"
#include "ADCL_internal.h" 

#define ADCL_ATTR_IALLTOALL_TOTAL_NUM    1

#define ADCL_ATTR_IALLTOALL_ALG_MAX      4

#define ADCL_METHOD_IALLTOALL_TOTAL_NUM  11


#define ADCL_IALLTOALL_LINEAR     1
#define ADCL_IALLTOALL_PAIRWISE   2
#define ADCL_IALLTOALL_DISS       3
#define ADCL_IALLTOALL_NATIVE     4

void ADCL_ialltoall_linear (ADCL_request_t *req );
void ADCL_ialltoall_pairwise( ADCL_request_t *req );
void ADCL_ialltoall_diss( ADCL_request_t *req );
void ADCL_ialltoall_native (ADCL_request_t *req );
void ADCL_ialltoall_wait( ADCL_request_t *req );


#if defined(c_plusplus) || defined(__cplusplus)
}
#endif

#endif /* __ADCL_IALLTOALL_H__ */
