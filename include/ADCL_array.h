#ifndef __ADCL_ARRAY_H__
#define __ADCL_ARRAY_H__

struct ADCL_array_elem_s {
    int    id;
    void *ptr;
};
typedef struct ADCL_array_elem_s ADCL_array_elem_t;

struct ADCL_array_s {
    char            name[64];  /* name of the type, for debugging purposes */
    int                 size;  /* length of the data array */
    int                 last;  /* last used element */
    ADCL_array_elem_t *array;
};
typedef struct ADCL_array_s ADCL_array_t;

int ADCL_array_init (ADCL_array_t **arr, const char name[64], int size );
int ADCL_array_free (ADCL_array_t **arr );

int ADCL_array_get_next_free_pos  ( ADCL_array_t *arr, int *pos);
void * ADCL_array_get_ptr_by_pos ( ADCL_array_t *arr, int pos );
void * ADCL_array_get_ptr_by_id  ( ADCL_array_t *arr, int id  );
int ADCL_array_set_element (ADCL_array_t *arr, int pos, int id, void *ptr);
int ADCL_array_remove_element ( ADCL_array_t *arr, int pos );

#endif /* __ADCL_ARRAY_H__ */

