#ifndef __ADCL_METHOD_H__
#define __ADCL_METHOD_H__



/* Define function pointer for the method itself */
typedef int ADCL_fnct_ptr ( void* );

#define ADCL_MAX_ATTRLEN 32

/* The structre describing a communication method */
struct ADCL_method_s {
    int                 m_id; /* unique identifier */
    int              m_rfcnt; /* reference counter */
    ADCL_fnct_ptr*   m_ifunc; /* init-function pointer. This function 
				 will be set for single-block*/
    ADCL_fnct_ptr*   m_wfunc; /* wait-function pointer */
    int                 m_db; /* true for dual block, false for sb */
    int              m_nattr; /* how many attributes are required by 
				 this method */
    char            **m_attr; /* list of required attributes */
    char             **m_val; /* list of values of the required attributes */
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

int ADCL_method_get_num_attr ( ADCL_method_t *meth);
char* ADCL_method_get_attr_name ( ADCL_method_t *meth, int pos );
char* ADCL_method_get_attr_val (ADCL_method_t *meth, int i);


#if 0
/*=================================================================*/
/*=================================================================*/
/*=================================================================*/
/* List of currently supported attributes */

/* 
** Indicate overlapping of communication and computation 
** Possible values: true/false 
*/
char ADCL_attr_overlap[]="adcl_attr_overlap";
char ADCL_attr_val_true[]="true";
char ADCL_attr_val_false[]="false";

/*=================================================================*/
/* 
** Indicate requirement for high bandwidth
** Possible values: true/false
*/
char ADCL_attr_high_bandwidth[]="adcl_attr_high_bandwidth";

/*=================================================================*/
/* 
** Indicate derived datatypes vs. pack/unpack 
** Possible values: true/false
*/
char ADCL_attr_derived_types[]="adcl_attr_derived_types";

/*=================================================================*/
/* 
** indicate direct or indirect communicatoin path 
** Possible values: true/false
*/
char ADCL_attr_direct_mapping[]="adcl_attr_direct_mapping";

/*=================================================================*/
/* 
** indicate whether single or dual block method/operation
** Possible values: true/false
*/
char ADCL_attr_single_block[]="adcl_attr_single_block";

/*=================================================================*/
/* 
** Indicateing prefered data transfer for single-block
** operations 
** Possible values: "blocking", "nonblocking", "persistent",
**                  "onesided", "collective"
*/
char ADCL_attr_transfer_sblock[]="adcl_attr_transfer_sblock";
char ADCL_attr_transfer_val_blocking[]="blocking";
char ADCL_attr_transfer_val_nonblocking[]="nonblocking";
char ADCL_attr_transfer_val_persistent[]="persistent";
char ADCL_attr_transfer_val_collective[]="collective";
char ADCL_attr_transfer_val_onesided[]="onesided";

/*=================================================================*/
/* attribute indicateing prefered data transfer for dual-block
** operations 
** Possible values: "nonblocking", "persistent", "onesided"
*/
char ADCL_attr_transfer_dblock[]="adcl_attr_transfer_dblock";

/*=================================================================*/
/*=================================================================*/
/*=================================================================*/

#endif

#endif /* __ADCL_METHOD_H__ */
