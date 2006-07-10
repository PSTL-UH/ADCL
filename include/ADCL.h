#ifndef __ADCL_H__
#define __ADCL_H__

#include "mpi.h"


/* define constants and error codes */
#define ADCL_SUCCESS        0
#define ADCL_NO_MEMORY      -1
#define ADCL_ERROR_INTERNAL -2
#define ADCL_USER_ERROR     -3
#define ADCL_UNDEFINED      -4
#define ADCL_NOT_FOUND      -5

#define ADCL_INVALID_ARG     10
#define ADCL_INVALID_NDIMS   11
#define ADCL_INVALID_DIMS    12
#define ADCL_INVALID_HWIDTH  13
#define ADCL_INVALID_DAT     14
#define ADCL_INVALID_DATA    15
#define ADCL_INVALID_COMM    16
#define ADCL_INVALID_REQUEST 17
#define ADCL_INVALID_NC      18
#define ADCL_INVALID_TYPE    19

#define ADCL_VECTOR_NULL  (void*)   -1
#define ADCL_REQUEST_NULL (void*)   -2
#define ADCL_NULL_FCTN_PTR (void* ) -3

/* define the object types visible to the user */
typedef struct ADCL_vector_s*   ADCL_Vector;
typedef struct ADCL_request_s*  ADCL_Request;



/* Prototypes of the User level interface functions */

int ADCL_Init (void );
int ADCL_Finalize (void );

int ADCL_Vector_allocate ( int ndims, int *dims, int nc, int hwidth, 
			   MPI_Datatype dat, ADCL_Vector *vec );
int ADCL_Vector_free     ( ADCL_Vector *vec );
int ADCL_Vector_register ( int ndims, int *dims, int nc, int hwidth, 
			   MPI_Datatype dat, void *data, 
			   ADCL_Vector *vec );
int ADCL_Vector_deregister ( ADCL_Vector *vec );
int ADCL_Vector_get_data_ptr ( ADCL_Vector vec, void *data );

int ADCL_Request_create ( ADCL_Vector vec, MPI_Comm comm, ADCL_Request *req );
int ADCL_Request_free   ( ADCL_Request *req );
int ADCL_Request_start ( ADCL_Request req );
int ADCL_Request_init ( ADCL_Request req );
int ADCL_Request_wait ( ADCL_Request req );

typedef void ADCL_work_fctn_ptr ( void *arg1, void *arg2, void *arg3 );
int ADCL_Request_start_overlap ( ADCL_Request req, ADCL_work_fctn_ptr* midfctn,
				 ADCL_work_fctn_ptr *endfcnt, 
				 ADCL_work_fctn_ptr *totalfcnt,
				 void *arg1, void* arg2, void *arg3 );


#endif /* __ADCL_H__ */
