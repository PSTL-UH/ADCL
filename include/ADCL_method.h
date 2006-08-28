#ifndef __ADCL_METHOD_H__
#define __ADCL_METHOD_H__

/*=================================================================*/
/* List of currently supported attributes 
   Each attribute consists of an attribute ID( given by the define statement),
   a name (for debugging purposes), the possible values (integer) and 
   the maximum number of values supported right now.
*/

/* Total number of attributes */
#define ADCL_ATTR_TOTAL_NUM 3 /* ignoring numblocks for right now */

/* Store for each attribute the maximum number of possible values */
extern int ADCL_attr_max[ADCL_ATTR_TOTAL_NUM];
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


/* Define function pointer for the method itself */
typedef int ADCL_fnct_ptr ( void* );

#define ADCL_MAX_ATTRLEN 32
#define ADCL_MAX_NAMELEN 32
/* The structre describing a communication method */
struct ADCL_method_s {
    int                        m_id; /* unique identifier */
    char   m_name[ADCL_MAX_NAMELEN]; /* name */
    int                     m_rfcnt; /* reference counter */
    ADCL_fnct_ptr*          m_ifunc; /* init-function pointer. This function 
			              will be set for single-block*/
    ADCL_fnct_ptr*          m_wfunc; /* wait-function pointer */
    int                        m_db; /* true for dual block, false for sb */
    int m_attr[ADCL_ATTR_TOTAL_NUM]; /* list of required attributes */
};
typedef struct ADCL_method_s  ADCL_method_t;


/* ADCL_method_init:
   Description: This routine initializes all communication methodologies.
                 It will be called from ADCL_Init();
   @ret ADCL_SUCCESS:   ok
   @ret ADCL_NO_MEMORY: memory could not be allocated
*/
int ADCL_method_init ( void );

/* ADCL_method_finalize
   Description: free all general structures allocated in ADCL_method_init.
                called from ADCL_finalize();
   @ret ADCL_SUCCESS:   everything ok;
*/
int ADCL_method_finalize ( void );

int ADCL_get_num_methods (void );
ADCL_method_t*  ADCL_get_method ( int i);


#endif /* __ADCL_METHOD_H__ */
