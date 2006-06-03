#ifndef __ADCL_FPROTOTYPES_H__
#define __ADCL_FPROTOTYPES_H__

#include "ADCL.h"
#include "ADCL_internal.h"

void adcl_init   ( int *ierror );
void adcl_init_  ( int *ierror );
void adcl_init__ ( int *ierror );
void ADCL_INIT   ( int *ierror );

void adcl_finalize   ( int *ierror );
void adcl_finalize_  ( int *ierror );
void adcl_finalize__ ( int *ierror );
void ADCL_FINALIZE   ( int *ierror );

void adcl_vector_register   ( int *ndims, int *dims, int *nc, int *hwidth, 
			      int *dat, void *data, int *vec, int *ierror);
void adcl_vector_register_  ( int *ndims, int *dims, int *nc, int *hwidth, 
			      int *dat, void *data, int *vec, int *ierror);
void adcl_vector_register__ ( int *ndims, int *dims, int *nc, int *hwidth, 
			      int *dat, void *data, int *vec, int *ierror);
void ADCL_VECTOR_REGISTER   ( int *ndims, int *dims, int *nc, int *hwidth, 
			      int *dat, void *data, int *vec, int *ierror);

void adcl_vector_deregister   ( int *vec, int *ierror );
void adcl_vector_deregister_  ( int *vec, int *ierror );
void adcl_vector_deregister__ ( int *vec, int *ierror );
void ADCL_VECTOR_DEREGISTER   ( int *vec, int *ierror );

void adcl_request_create   ( int *vec, int *comm, int *req, int *ierror ); 
void adcl_request_create_  ( int *vec, int *comm, int *req, int *ierror ); 
void adcl_request_create__ ( int *vec, int *comm, int *req, int *ierror ); 
void ADCL_REQUEST_CREATE   ( int *vec, int *comm, int *req, int *ierror ); 

void adcl_request_free   ( int *req, int *ierror );
void adcl_request_free_  ( int *req, int *ierror );
void adcl_request_free__ ( int *req, int *ierror );
void ADCL_REQUEST_FREE   ( int *req, int *ierror );

void adcl_request_start   ( int *req, int *ierror );
void adcl_request_start_  ( int *req, int *ierror );
void adcl_request_start__ ( int *req, int *ierror );
void ADCL_REQUEST_START   ( int *req, int *ierror );

void adcl_request_init   ( int *req, int *ierror );
void adcl_request_init_  ( int *req, int *ierror );
void adcl_request_init__ ( int *req, int *ierror );
void ADCL_REQUEST_INIT   ( int *req, int *ierror );

void adcl_request_wait   ( int *req, int *ierror );
void adcl_request_wait_  ( int *req, int *ierror );
void adcl_request_wait__ ( int *req, int *ierror );
void ADCL_REQUEST_WAIT   ( int *req, int *ierror );

void adcl_request_start_overlap ( int *req, ADCL_work_fctn_ptr *mid,
				  ADCL_work_fctn_ptr *end, 
				  ADCL_work_fctn_ptr *total, 
				  void *arg1, void* arg2, void* arg3, 
				  int *ierror );
void adcl_request_start_overlap_ ( int *req, ADCL_work_fctn_ptr *mid,
				   ADCL_work_fctn_ptr *end, 
				   ADCL_work_fctn_ptr *total, 
				   void *arg1, void* arg2, void* arg3, 
				   int *ierror );
void adcl_request_start_overlap__ ( int *req, ADCL_work_fctn_ptr *mid,
				    ADCL_work_fctn_ptr *end, 
				    ADCL_work_fctn_ptr *total, 
				    void *arg1, void* arg2, void* arg3, 
				    int *ierror );
void ADCL_REQUEST_START_OVERLAP   ( int *req, ADCL_work_fctn_ptr *mid,
				    ADCL_work_fctn_ptr *end, 
				    ADCL_work_fctn_ptr *total, 
				    void *arg1, void* arg2, void* arg3, 
				    int *ierror );



#endif /*  __ADCL_FPROTOTYPES_H__ */
