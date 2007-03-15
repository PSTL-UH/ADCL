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

#define ADCL_INVALID_ARG       10
#define ADCL_INVALID_NDIMS     11
#define ADCL_INVALID_DIMS      12
#define ADCL_INVALID_HWIDTH    13
#define ADCL_INVALID_DAT       14
#define ADCL_INVALID_DATA      15
#define ADCL_INVALID_COMM      16
#define ADCL_INVALID_REQUEST   17
#define ADCL_INVALID_NC        18
#define ADCL_INVALID_TYPE      19
#define ADCL_INVALID_TOPOLOGY  20
#define ADCL_INVALID_ATTRIBUTE 21
#define ADCL_INVALID_ATTRSET   22
#define ADCL_INVALID_FUNCTION  23
#define ADCL_INVALID_WORK_FUNCTION_PTR  24
#define ADCL_INVALID_FNCTSET   25
#define ADCL_INVALID_VECTOR    26
#define ADCL_INVALID_DIRECTION 27

#define ADCL_VECTOR_NULL    (void*) -1
#define ADCL_REQUEST_NULL   (void*) -2
#define ADCL_TOPOLOGY_NULL  (void*) -3
#define ADCL_ATTRIBUTE_NULL (void*) -4
#define ADCL_ATTRSET_NULL   (void*) -5
#define ADCL_FUNCTION_NULL  (void*) -6
#define ADCL_FNCTSET_NULL   (void*) -7
#define ADCL_NULL_FNCT_PTR  (void*) -8


#define ADCL_MAX_ATTRLEN 32
#define ADCL_MAX_NAMELEN 32

#define ADCL_DIRECTION_BOTH          1
#define ADCL_DIRECTION_LEFT_TO_RIGHT 2
#define ADCL_DIRECTION_RIGHT_TO_LEFT 3


/* define the object types visible to the user */
typedef struct ADCL_vector_s*    ADCL_Vector;
typedef struct ADCL_request_s*   ADCL_Request;
typedef struct ADCL_topology_s*  ADCL_Topology;
typedef struct ADCL_attribute_s* ADCL_Attribute;
typedef struct ADCL_attrset_s*   ADCL_Attrset;
typedef struct ADCL_function_s*  ADCL_Function;
typedef struct ADCL_fnctset_s*   ADCL_Fnctset;


/* Prototypes of the User level interface functions */

/* ADCL environment functions */
int ADCL_Init (void );
int ADCL_Finalize (void );

/* ADCL Vector functions */
int ADCL_Vector_allocate ( int ndims, int *dims, int nc, int hwidth, 
			   MPI_Datatype dat, void *data, ADCL_Vector *vec );
int ADCL_Vector_free     ( ADCL_Vector *vec );
int ADCL_Vector_register ( int ndims, int *dims, int nc, int hwidth, 
			   MPI_Datatype dat, void *data, 
			   ADCL_Vector *vec );
int ADCL_Vector_deregister ( ADCL_Vector *vec );

/* ADCL Topology functions */
int ADCL_Topology_create  ( MPI_Comm cart_comm, ADCL_Topology *topo);
int ADCL_Topology_free    ( ADCL_Topology *topo );
int ADCL_Topology_create_generic ( int ndims, int *lneighbors, 
				   int *rneighbors, int *coords, int direction,  
				   MPI_Comm comm, ADCL_Topology *topo);


/* ADCL Attributes and Attributeset fucntions */
int ADCL_Attribute_create ( int maxnvalues, int *array_of_values, 
			    ADCL_Attribute *attr );
int ADCL_Attribute_free   ( ADCL_Attribute *attr );

int ADCL_Attrset_create   ( int maxnum, ADCL_Attribute *array_of_attributes, 
			    ADCL_Attrset *attrset );
int ADCL_Attrset_free     ( ADCL_Attrset *attrset );

/* ADCL Function and ADCL Functiongroup functions */
typedef void ADCL_work_fnct_ptr ( ADCL_Request req );

int ADCL_Function_create       ( ADCL_work_fnct_ptr *fnctp, 
				 ADCL_Attrset attrset, 
				 int *array_of_attrvalues, 
				 char *name, 
				 ADCL_Function *fnct);
int ADCL_Function_create_async ( ADCL_work_fnct_ptr *init_fnct, 
				 ADCL_work_fnct_ptr *wait_fnct, 
				 ADCL_Attrset attrset, 
				 int *array_of_attrvalues, char *name,  
				 ADCL_Function *fnct);

int ADCL_Function_free         ( ADCL_Function *fnct );

int ADCL_Fnctset_create ( int maxnum, ADCL_Function *fncts, char *name, 
			  ADCL_Fnctset *fnctset );
int ADCL_Fnctset_free   ( ADCL_Fnctset *fnctset );


/* ADCL Request functions */
int ADCL_Request_create         ( ADCL_Vector vec, ADCL_Topology topo, 
				  ADCL_Request *req );
int ADCL_Request_create_generic ( ADCL_Vector *array_of_send_vecs, 
				  ADCL_Vector *array_of_recv_vecs, 
				  ADCL_Topology topo, 
				  ADCL_Request *req );
int ADCL_Request_create_fnctset ( ADCL_Topology topo, ADCL_Fnctset fnctset, ADCL_Request *req );

int ADCL_Request_get_comm  ( ADCL_Request req, MPI_Comm *comm, int *rank, int *size );
int ADCL_Request_free      ( ADCL_Request *req );

int ADCL_Request_start ( ADCL_Request req );
int ADCL_Request_init  ( ADCL_Request req );
int ADCL_Request_wait  ( ADCL_Request req );

int ADCL_Request_start_overlap ( ADCL_Request req, ADCL_work_fnct_ptr* midfctn,
				 ADCL_work_fnct_ptr *endfcnt, 
				 ADCL_work_fnct_ptr *totalfcnt );


#endif /* __ADCL_H__ */
