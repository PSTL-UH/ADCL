#ifndef __ADCL_FPROTOTYPES_H__
#define __ADCL_FPROTOTYPES_H__

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


#endif /*  __ADCL_FPROTOTYPES_H__ */
