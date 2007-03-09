#ifndef __ADCL_PREDEFINED_H__
#define __ADCL_PREDEFINED_H__

/*=================================================================*/
/* List of currently supported attributes 
   Each attribute consists of an attribute ID( given by the define statement),
   a name (for debugging purposes), the possible values (integer) and 
   the maximum number of values supported right now.
*/

/* Total number of attributes */
#define ADCL_ATTR_TOTAL_NUM  3  /* ignoring numblocks for right now */
#define ADCL_ATTR_NOT_SET   -1  /* attributes not set*/
#define ADCL_ATTR_NEW_BLOCK -2  /* signal that we start a new block. 
				   Used in the performance hypothesis v2 */

/* Store for each attribute the maximum number of possible values */
//extern int ADCL_attr_max[ADCL_ATTR_TOTAL_NUM];
//extern int ADCL_attr_base[ADCL_ATTR_TOTAL_NUM];
/*=================================================================*/
/* 
** The mapping step
** Possible values: aao, pair, hierarch
*/

#define ADCL_ATTR_MAPPING  0
#define ADCL_ATTR_MAPPING_MAX 2 /* Ignore hierarch for now */

const int ADCL_attr_mapping_aao=100;
const int ADCL_attr_mapping_pair=101;
const int ADCL_attr_mapping_hierarch=102;

/*=================================================================*/
/* 
** Methods for non-contiguous data transfer
** Possible values: derived datatypes, pack/unpack, individual
*/
#define ADCL_ATTR_NONCONT  1
#define ADCL_ATTR_NONCONT_MAX 2 /* Ignore individual for now */

const int ADCL_attr_noncont_ddt=110;
const int ADCL_attr_noncont_pack=111;
const int ADCL_attr_noncont_individual=112;

/*=================================================================*/
/* 
** Data transfer primitives
** Possible values: IsendIrecv, SendIrecv, SendRecv, Sendrecv,
**                  FenceGet, FencePut, PostStartGet, PostStartPut
*/
#define ADCL_ATTR_TRANSFER 2

#ifdef MPI_WIN
#define ADCL_ATTR_TRANSFER_MAX 4 
#else
#define ADCL_ATTR_TRANSFER_MAX 8
#endif

const int ADCL_attr_transfer_IsendIrecv=120;
const int ADCL_attr_transfer_SendIrecv=121;
const int ADCL_attr_transfer_SendRecv=122;
const int ADCL_attr_transfer_Sendrecv=123;
#ifdef MPI_WIN
const int ADCL_attr_transfer_FenceGet=124;
const int ADCL_attr_transfer_FencePut=125;
const int ADCL_attr_transfer_StartPostGet=126;
const int ADCL_attr_transfer_StartPostPut=127;
#endif

/*=================================================================*/

/* 
** indicate whether single or dual block method/operation
** (required for the overlap later on)
** Possible values: single, dual
*/
#define ADCL_ATTR_NUMBLOCKS 4
#define ADCL_ATTR_NUMBLOCKS_MAX 2

const int ADCL_attr_numblocks_single=130;
const int ADCL_attr_numblocks_dual=131;

/*=================================================================*/

#ifdef MPI_WIN
#define ADCL_METHOD_TOTAL_NUM 20
#else
#define ADCL_METHOD_TOTAL_NUM 12
#endif

extern ADCL_attribute_t *ADCL_neighborhood_attrs[ADCL_ATTR_TOTAL_NUM];
extern ADCL_attrset_t *ADCL_neighborhood_attrset;

extern ADCL_function_t *ADCL_neighborhood_functions[ADCL_METHOD_TOTAL_NUM];
extern ADCL_fnctset_t *ADCL_neighborhood_fnctset; 


#endif
