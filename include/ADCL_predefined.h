/*
 * Copyright (c) 2006-2007      University of Houston. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#ifndef __ADCL_PREDEFINED_H__
#define __ADCL_PREDEFINED_H__

/*=================================================================*/
/* List of currently supported attributes
   Each attribute consists of an attribute ID( given by the define statement),
   a name (for debugging purposes), the possible values (integer) and
   the maximum number of values supported right now.
*/

/* Total number of attributes */
#define ADCL_ATTR_NN_TOTAL_NUM  3  /* ignoring numblocks for right now */
#define ADCL_ATTR_ALLGATHERV_TOTAL_NUM  1  /*  */
#define ADCL_ATTR_ALLREDUCE_TOTAL_NUM  1  /*  */
#define ADCL_ATTR_NOT_SET   -1  /* attributes not set*/
#define ADCL_ATTR_NEW_BLOCK -2  /* signal that we start a new block.
                   Used in the performance hypothesis v2 */

/* Store for each attribute the maximum number of possible values */
/*=================================================================*/
/*
** The mapping step
** Possible values: aao, pair, hierarch
*/

#define ADCL_ATTR_MAPPING  0
#define ADCL_ATTR_MAPPING_MAX 2 /* Ignore hierarch for now */

extern const int ADCL_attr_mapping_aao;
extern const int ADCL_attr_mapping_pair;
extern const int ADCL_attr_mapping_hierarch;

/*=================================================================*/
/*
** Methods for non-contiguous data transfer
** Possible values: derived datatypes, pack/unpack, individual
*/
#define ADCL_ATTR_NONCONT  1
#define ADCL_ATTR_NONCONT_MAX 2 /* Ignore individual for now */

extern const int ADCL_attr_noncont_ddt;
extern const int ADCL_attr_noncont_pack;
extern const int ADCL_attr_noncont_individual;

/*=================================================================*/
/*
** Data transfer primitives
** Possible values: IsendIrecv, SendIrecv, Send_Recv, Sendrecv,
**                  FenceGet, FencePut, PostStartGet, PostStartPut
*/
#define ADCL_ATTR_TRANSFER 2

#ifdef MPI_WIN
#define ADCL_ATTR_TRANSFER_MAX 8
#else
#define ADCL_ATTR_TRANSFER_MAX 4
#endif

extern const int ADCL_attr_transfer_nn_IsendIrecv;
extern const int ADCL_attr_transfer_nn_SendIrecv;
extern const int ADCL_attr_transfer_nn_Send_Recv;
extern const int ADCL_attr_transfer_nn_Sendrecv;
#ifdef MPI_WIN
extern const int ADCL_attr_transfer_nn_FenceGet;
extern const int ADCL_attr_transfer_nn_FencePut;
extern const int ADCL_attr_transfer_nn_PostStartGet;
extern const int ADCL_attr_transfer_nn_PostStartPut;
#endif

extern const int ADCL_attr_transfer_allgatherv_bruck;
extern const int ADCL_attr_transfer_allgatherv_linear;
extern const int ADCL_attr_transfer_allgatherv_neighborexch;
extern const int ADCL_attr_transfer_allgatherv_ring;
extern const int ADCL_attr_transfer_allgatherv_twoprocs;

/*=================================================================*/

/*
** indicate whether single or dual block method/operation
** (required for the overlap later on)
** Possible values: single, dual
*/
#define ADCL_ATTR_NUMBLOCKS 4
#define ADCL_ATTR_NUMBLOCKS_MAX 2

extern const int ADCL_attr_numblocks_single;
extern const int ADCL_attr_numblocks_dual;

/*=================================================================*/

#ifdef MPI_WIN
#define ADCL_METHOD_NN_TOTAL_NUM 20
#else
#define ADCL_METHOD_NN_TOTAL_NUM 12
#endif
#define ADCL_METHOD_ALLGATHERV_TOTAL_NUM 6 

#define ADCL_METHOD_ALLREDUCE_TOTAL_NUM 5 

extern ADCL_attribute_t *ADCL_neighborhood_attrs[ADCL_ATTR_NN_TOTAL_NUM];
extern ADCL_attrset_t *ADCL_neighborhood_attrset;

extern ADCL_function_t *ADCL_neighborhood_functions[ADCL_METHOD_NN_TOTAL_NUM];
extern ADCL_function_t *ADCL_allgatherv_functions[ADCL_METHOD_ALLGATHERV_TOTAL_NUM];
extern ADCL_function_t *ADCL_allreduce_functions[ADCL_METHOD_ALLREDUCE_TOTAL_NUM];


//void ADCL_neighborhood_set_criteria( void *filter_criteria, ADCL_request_t *req );

#endif
