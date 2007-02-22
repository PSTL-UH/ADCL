#ifndef __ADCL_METHOD_H__
#define __ADCL_METHOD_H__



/* Define function pointer for the method itself */
typedef int ADCL_fnct_ptr ( void* );



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
