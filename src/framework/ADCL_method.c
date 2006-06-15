#include "ADCL_internal.h"
#include "ADCL_config.h"

static int ADCL_local_id_counter=0;
static int ADCL_method_total_num=0;
static ADCL_method_t* ADCL_method_array=NULL;


#ifndef ADCL_NO_COMM_DEBUG
int ADCL_method_init ( void )
{
    int count=0;
    
    /* Three steps: 
       1. generate a list of all available single-block methods
       2. determine the characteristics of the current environment
       3. generate a new, shorter list of single-block methods available
       4. generate a list of all available dual-block methods
       5. generate a new, shorter list of dual-block methods
    */

    /* right now we are just setting some values for code
       development and verification purposes. No attributes 
       set right now! 
    */

    ADCL_method_total_num = 7;
    ADCL_method_array=(ADCL_method_t*)calloc(1, ADCL_method_total_num
					     * sizeof( ADCL_method_t));
    if ( NULL == ADCL_method_array ) {
	return ADCL_NO_MEMORY;
    }
#ifdef DERIVED_TYPES

#ifdef CHANGE_AAO
#  ifdef ISEND_IRECV
      ADCL_method_array[count].m_id = ADCL_local_id_counter++;
      ADCL_method_array[count].m_ifunc = (ADCL_fnct_ptr*) ADCL_change_sb_aao_IsendIrecv;
      strncpy ( ADCL_method_array[count].m_name, "IsendIrecv_aao", 32 );
      count++; 
#  endif /* ISEND_IRECV */

#  ifdef SEND_IRECV
      ADCL_method_array[count].m_id = ADCL_local_id_counter++;
      ADCL_method_array[count].m_ifunc = (ADCL_fnct_ptr*) ADCL_change_sb_aao_SendIrecv;
      strncpy ( ADCL_method_array[count].m_name, "SendIrecv_aao", 32 );
      count++; 
#  endif /* SEND_IRECV */
#endif /* CHANGE_AAO */

#ifdef CHANGE_PAIR
#  ifdef ISEND_IRECV
      ADCL_method_array[count].m_id = ADCL_local_id_counter++;
      ADCL_method_array[count].m_ifunc = (ADCL_fnct_ptr*) ADCL_change_sb_pair_IsendIrecv;
      strncpy ( ADCL_method_array[count].m_name, "IsendIrecv_pair", 32 );
      count++; 
#  endif /* ISEND_IRECV */
#  ifdef SEND_RECV
      ADCL_method_array[count].m_id = ADCL_local_id_counter++;
      ADCL_method_array[count].m_ifunc = (ADCL_fnct_ptr*) ADCL_change_sb_pair_SendRecv;
      strncpy ( ADCL_method_array[count].m_name, "SendRecv_pair", 32 );
      count++; 
#  endif /* SEND_RECV */
#  ifdef SEND_IRECV
      ADCL_method_array[count].m_id = ADCL_local_id_counter++;
      ADCL_method_array[count].m_ifunc = (ADCL_fnct_ptr*) ADCL_change_sb_pair_SendIrecv;
      strncpy ( ADCL_method_array[count].m_name, "SendIrecv_pair", 32 );
      count++; 
#  endif /* SEND_IRECV */
#endif /* CHANGE_PAIR */

#endif /* DERIVED_TYPES */

#ifdef PACK_UNPACK

#ifdef CHANGE_AAO
#  ifdef ISEND_IRECV
      ADCL_method_array[count].m_id = ADCL_local_id_counter++;
      ADCL_method_array[count].m_ifunc = (ADCL_fnct_ptr*) ADCL_change_sb_aao_IsendIrecv_pack;
      strncpy ( ADCL_method_array[count].m_name, "IsendIrecv_aao_pack", 32 );
      count++; 
#  endif /* ISEND_IRECV */
#endif /* CHANGE_AAO */

#ifdef CHANGE_PAIR
#  ifdef ISEND_IRECV
      ADCL_method_array[count].m_id = ADCL_local_id_counter++;
      ADCL_method_array[count].m_ifunc = (ADCL_fnct_ptr*) ADCL_change_sb_pair_IsendIrecv_pack;
      strncpy ( ADCL_method_array[count].m_name, "IsendIrecv_pair_pack", 32 );
      count++; 
#  endif /* ISEND_IRECV */
#endif /* CHANGE_PAIR  */

#endif /* PACK_UNPACK */


    return ADCL_SUCCESS;
}
#endif

int ADCL_method_finalize (void)
{
    if ( NULL != ADCL_method_array  ) {
	free ( ADCL_method_array );
    }

    return ADCL_SUCCESS;
}

int ADCL_method_get_num_attr ( ADCL_method_t *meth)
{
    return meth->m_nattr;
}

char* ADCL_method_get_attr_name ( ADCL_method_t *meth, int pos )
{
    char *attr;

    if ( pos >= meth->m_nattr ) {
	return NULL;
    }
    attr = strdup((const char *)(meth->m_attr[pos]) );
    return attr;
}

char* ADCL_method_get_attr_val (ADCL_method_t *meth, int i)
{
    char *val;

    if ( i >= meth->m_nattr ) {
	return NULL;
    }
    val = strdup(meth->m_val[i]);
    return val;
}

int ADCL_get_num_methods (void)
{
    return ADCL_method_total_num;
}


ADCL_method_t* ADCL_get_method ( int i )
{
    if ( i>=ADCL_method_total_num || i<0 ) {
	return NULL;
    }

    return (&(ADCL_method_array[i]));
}


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
#ifdef ADCL_NO_COMM_DEBUG

int singleblock_func0 ( void *req );
int singleblock_func1 ( void *req );
int singleblock_func2 ( void *req );

int dualblock_func0_init ( void *req );
int dualblock_func0_wait ( void *req );

int dualblock_func1_init ( void *req );
int dualblock_func1_wait ( void *req );

int ADCL_method_init ( void )
{
    /* Three steps: 
       1. generate a list of all available single-block methods
       2. determine the characteristics of the current environment
       3. generate a new, shorter list of single-block methods available

       4. generate a list of all available dual-block methods
       5. generate a new, shorter list of dual-block methods
    */

    /* right now we are just setting some values for code
       development and verification purposes. No attributes 
       set right now! 
    */


    ADCL_method_total_num = 5;
    ADCL_method_array=(ADCL_method_t*)calloc(1, ADCL_method_total_num
					     * sizeof( ADCL_method_t));
    if ( NULL == ADCL_method_array ) {
	return ADCL_NO_MEMORY;
    }
    ADCL_method_array[0].m_ifunc = (ADCL_fnct_ptr*)singleblock_func0;
    ADCL_method_array[1].m_ifunc = (ADCL_fnct_ptr*)singleblock_func1;
    ADCL_method_array[2].m_ifunc = (ADCL_fnct_ptr*)singleblock_func2;

    ADCL_method_array[3].m_ifunc = (ADCL_fnct_ptr*)dualblock_func0_init;
    ADCL_method_array[3].m_wfunc = (ADCL_fnct_ptr*)dualblock_func0_wait;
    ADCL_method_array[3].m_db    = 1;

    ADCL_method_array[4].m_ifunc = (ADCL_fnct_ptr*)dualblock_func1_init;
    ADCL_method_array[4].m_wfunc = (ADCL_fnct_ptr*)dualblock_func1_wait;
    ADCL_method_array[4].m_db    = 1;

    return ADCL_SUCCESS;
}
int singleblock_func0 ( void *req ) 
{
    int rank;
    ADCL_request_t *preq = (ADCL_request_t *)req;

    MPI_Comm_rank ( preq->r_comm, &rank );
    printf("%d: in singleblock_func0. Sleeping for 0.2 seconds\n", rank);
    usleep (200);
    return 0;
}

int singleblock_func1 ( void *req ) 
{
    int rank;
    ADCL_request_t *preq = (ADCL_request_t *)req;

    MPI_Comm_rank ( preq->r_comm, &rank );
    printf("%d: in singleblock_func1. Sleeping for 0.4 seconds\n", rank);
    usleep (400);
    return 0;
}

int singleblock_func2 ( void *req ) 
{
    int rank;
    ADCL_request_t *preq = (ADCL_request_t *)req;

    MPI_Comm_rank ( preq->r_comm, &rank );
    printf("%d: in singleblock_func2. Sleeping for 0.1 seconds\n", rank);
    usleep (100);
    return 0;
}

int dualblock_func0_init ( void *req ) 
{
    int rank;
    ADCL_request_t *preq = (ADCL_request_t *)req;

    MPI_Comm_rank ( preq->r_comm, &rank );
    printf("%d: in dualblock_func0_init. Not Sleeping.\n", rank);
    return 0;
}

int dualblock_func1_init ( void *req ) 
{
    int rank;
    ADCL_request_t *preq = (ADCL_request_t *)req;

    MPI_Comm_rank ( preq->r_comm, &rank );
    printf("%d: in dualblock_func1_init. Not sleeping.\n", rank);
    return 0;
}

int dualblock_func0_wait ( void *req ) 
{
    int rank;
    ADCL_request_t *preq = (ADCL_request_t *)req;

    MPI_Comm_rank ( preq->r_comm, &rank );
    printf("%d: in dualblock_func0_wait. Sleeping for 0.1 seconds\n", rank);
    usleep (100);
    return 0;
}

int dualblock_func1_wait ( void *req ) 
{
    int rank;
    ADCL_request_t *preq = (ADCL_request_t *)req;

    MPI_Comm_rank ( preq->r_comm, &rank );
    printf("%d: in dualblock_func1_wait. Sleeping for 0.1 seconds\n", rank);
    usleep (100);
    return 0;
}
#endif
