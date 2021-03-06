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

#define ADCL_ATTR_IALLTOALL_TOTAL_NUM    2

//#define ADCL_ATTR_IALLTOALL_ALG_MAX      2
//#define ADCL_ATTR_IALLTOALL_ALG_MAX      3
//#define ADCL_ATTR_IALLTOALL_TYPE_MAX      2
#define ADCL_ATTR_IALLTOALL_ALG_MAX      10
#define ADCL_ATTR_IALLTOALL_DEPTH_MAX      6

//#define ADCL_METHOD_IALLTOALL_TOTAL_NUM  2
//#define ADCL_METHOD_IALLTOALL_TOTAL_NUM  3
//#define ADCL_METHOD_IALLTOALL_TOTAL_NUM  4
//#define ADCL_METHOD_IALLTOALL_TOTAL_NUM  10
//#define ADCL_METHOD_IALLTOALL_TOTAL_NUM  11
#if MAX_PROGRESS_FOR_DEPTH == 5
    #define ADCL_METHOD_IALLTOALL_TOTAL_NUM  25
#else
    #define ADCL_METHOD_IALLTOALL_TOTAL_NUM  19
#endif


#define ADCL_IALLTOALL_LINEAR     1
#define ADCL_IALLTOALL_PAIRWISE   2
#define ADCL_IALLTOALL_DISS       3

void ADCL_ialltoall_linear (ADCL_request_t *req );
void ADCL_ialltoall_pairwise( ADCL_request_t *req );
void ADCL_ialltoall_diss( ADCL_request_t *req );
void ADCL_ialltoall_wait( ADCL_request_t *req );


#if defined(c_plusplus) || defined(__cplusplus)
}
#endif

#endif /* __ADCL_IALLTOALL_H__ */
