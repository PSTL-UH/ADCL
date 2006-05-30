#ifndef __ADCL_MEMORY_H__
#define __ADCL_MEMORY_H__


int ADCL_allocate_2D_double_matrix(double ***matrix,int dims[2]);
int ADCL_allocate_3D_double_matrix(double ****matrix,int dims[3]);
int ADCL_allocate_4D_double_matrix(double *****matrix,int dims[4]);
int ADCL_allocate_5D_double_matrix(double ******matrix, int dims[5]);

int ADCL_free_2D_double_matrix(double ***matrix);
int ADCL_free_3D_double_matrix(double ****matrix);
int ADCL_free_4D_double_matrix(double *****matrix);
int ADCL_free_5D_double_matrix(double ******matrix);

int ADCL_allocate_2D_float_matrix(float ***matrix,int dims[2]);
int ADCL_allocate_3D_float_matrix(float ****matrix,int dims[3]);
int ADCL_allocate_4D_float_matrix(float *****matrix,int dims[4]);
int ADCL_allocate_5D_float_matrix(float ******matrix, int dims[5]);

int ADCL_free_2D_float_matrix(float ***matrix);
int ADCL_free_3D_float_matrix(float ****matrix);
int ADCL_free_4D_float_matrix(float *****matrix);
int ADCL_free_5D_float_matrix(float ******matrix);

int ADCL_allocate_2D_int_matrix(int ***matrix,int dims[2]);
int ADCL_allocate_3D_int_matrix(int ****matrix,int dims[3]);
int ADCL_allocate_4D_int_matrix(int *****matrix,int dims[4]);
int ADCL_allocate_5D_int_matrix(int ******matrix, int dims[5]);

int ADCL_free_2D_int_matrix(int ***matrix);
int ADCL_free_3D_int_matrix(int ****matrix);
int ADCL_free_4D_int_matrix(int *****matrix);
int ADCL_free_5D_int_matrix(int ******matrix);

#endif /* __ADCL_MEMORY_H__ */
