/*
 * Copyright (c) 2006-2007      University of Houston. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include "ADCL_internal.h"

static int ADCL_local_id_counter = 0;
ADCL_array_t *ADCL_data_array = NULL;

static void ADCL_data_add_to_file( ADCL_data_t* data, ADCL_emethod_t *e );

int ADCL_data_create ( ADCL_emethod_t *e ) 
{
    ADCL_data_t *data;
    ADCL_topology_t *topo = e->em_topo;
    ADCL_vector_t   *vec  = e->em_vec;
    int i, size, topo_status;
    int *dims, *coords;

    /* For now we support only cartesian topology */
    MPI_Topo_test ( topo->t_comm, &topo_status );
    if ( ( MPI_CART != topo_status ) || ( ADCL_VECTOR_NULL == vec ) ) {
        return ADCL_INVALID_ARG;
    }
    size = topo->t_size;
    data = (ADCL_data_t *) calloc (1, sizeof(ADCL_data_t));
    data->d_rcnts = (int *)calloc (size, sizeof(int));
    data->d_displ = (int *)calloc (size, sizeof(int));
    if ( (NULL == data) || (NULL == data->d_rcnts) || (NULL == data->d_displ) ) {
        return ADCL_NO_MEMORY;
    }
    /* Internal info for object management */
    data->d_id = ADCL_local_id_counter++;
    ADCL_array_get_next_free_pos ( ADCL_data_array, &data->d_findex );
    ADCL_array_set_element ( ADCL_data_array, data->d_findex, data->d_id, data );
    data->d_refcnt = 1;
    /* Network Topology information */
    data->d_np = size;
    /* Logical Topology information */
    data->d_tndims = topo->t_ndims;
    data->d_tperiods = (int *)malloc( data->d_tndims*sizeof(int) );
    dims = (int *)malloc( data->d_tndims*sizeof(int) );
    coords = (int *)malloc( data->d_tndims*sizeof(int) );
    MPI_Cart_get ( topo->t_comm, topo->t_ndims, dims, data->d_tperiods, coords );
    free( dims );
    free( coords );
    /* Vector information */
    data->d_vndims = vec->v_ndims;
    data->d_vdims = (int *)malloc(data->d_vndims*sizeof(int) );
    for ( i=0; i<data->d_vndims ;i++ ) {
        data->d_vdims[i] = vec->v_dims[i];
    }
    data->d_nc = vec->v_nc;
    /* Vector map information */
    data->d_vectype = vec->v_map->m_vectype;
    data->d_hwidth = vec->v_map->m_hwidth;
    if ((NULL != vec->v_map->m_rcnts) && (NULL != vec->v_map->m_displ)) {
        for ( i=0; i<size; i++ ) {
            data->d_rcnts[i] = vec->v_map->m_rcnts[i];
            data->d_displ[i] = vec->v_map->m_displ[i];
        }
    }
    else {
        for ( i=0; i<size; i++ ) {
            data->d_rcnts[i] = 0;
            data->d_displ[i] = 0;
        }
    }
    data->d_op = vec->v_map->m_op;
    data->d_inplace = vec->v_map->m_inplace;
    /* Attribute information */
    if (NULL != e->em_orgfnctset->fs_attrset && ADCL_ATTRSET_NULL !=  e->em_orgfnctset->fs_attrset){
       data->d_asmaxnum = e->em_orgfnctset->fs_attrset->as_maxnum;
       data->d_attrvals = (int *)malloc( data->d_asmaxnum * sizeof(int) );
       /* Initialization */
       for (i=0; i<data->d_asmaxnum ; i++){
           data->d_attrvals[i] = -1;
       }
       if ( e->em_perfhypothesis ) {
           for (i=0; i<data->d_asmaxnum ; i++){
               if( e->em_hypo.h_attr_confidence[i] > 1 ) {
                   data->d_attrvals[i] = e->em_fnctset.fs_fptrs[0]->f_attrvals[i];
               }
           }
       }
    }
    /* Function set and winner function */
    data->d_fsname = strdup ( e->em_orgfnctset->fs_name );
    data->d_fsnum = e->em_orgfnctset->fs_maxnum;
    data->d_wfname = strdup ( e->em_wfunction->f_name );
    /* Performance data for H.L. */
    /* Execution times */
    data->d_perf = (double *)malloc(data->d_fsnum*sizeof(double));
    for(i=0; i<data->d_fsnum ; i++) {
        data->d_perf[i] = e->em_stats[i]->s_gpts[ e->em_filtering];
    }
#ifdef ADCL_KNOWLEDGE_TOFILE
    /* Update the data file */
    ADCL_data_add_to_file( data, e );
#endif
    return ADCL_SUCCESS;
}

void ADCL_data_free ( void )
{
    int i, last;
    ADCL_data_t *data;

    last = ADCL_array_get_last ( ADCL_data_array );
    /* Free all the data objects */
    for ( i=0; i<= last; i++ ) {
        data = ( ADCL_data_t * ) ADCL_array_get_ptr_by_pos( ADCL_data_array, i );
        if ( NULL != data  ) {
            if ( NULL != data->d_tperiods ) {
                free ( data->d_tperiods );
            }
            if ( NULL != data->d_vdims ) {
                free ( data->d_vdims );
            }
            if ( NULL != data->d_rcnts ) {
                free ( data->d_rcnts );
            }
            if ( NULL != data->d_displ ) {
                free ( data->d_displ );
            }
            if ( NULL != data->d_attrvals ) {
                free ( data->d_attrvals );
            }           
            if ( NULL != data->d_fsname ) {
                free ( data->d_fsname );
            }
            if ( NULL != data->d_wfname ) {
                free ( data->d_wfname );
            }
            if ( NULL != data->d_perf ) {
                free ( data->d_perf );
            }
            ADCL_array_remove_element ( ADCL_data_array, data->d_findex );
            free ( data );
        }
    }
    return;
}

int ADCL_data_find ( ADCL_emethod_t *e, ADCL_data_t **found_data )
{
    ADCL_data_t *data;
    ADCL_topology_t *topo = e->em_topo;
    ADCL_vector_t   *vec  = e->em_vec;
    int ret = ADCL_UNEQUAL;
    int i, j, last, explored_data, found, size;
    int *dims, *periods, *coords;

    if ( ADCL_VECTOR_NULL == vec ) {
        return ret;
    }

#ifdef ADCL_KNOWLEDGE_TOFILE
    if( -2 == e->em_explored_data ) {
        ADCL_data_read_from_file (e);
        e->em_explored_data = -1;
    }
#else
        e->em_explored_data = -1;
#endif

    last = ADCL_array_get_last ( ADCL_data_array );
    explored_data = e->em_explored_data;
    if (last > explored_data) {
        e->em_explored_data = last;
        size = topo->t_size;
        for ( i=(explored_data+1); i<= last; i++ ) {
            data = ( ADCL_data_t * ) ADCL_array_get_ptr_by_pos( ADCL_data_array, i );
            if ( ( topo->t_ndims    == data->d_tndims  ) &&
                 ( vec->v_ndims     == data->d_vndims  ) &&
                 ( vec->v_nc        == data->d_nc      ) &&
                 ( vec->v_map->m_vectype == data->d_vectype ) &&
                 ( vec->v_map->m_hwidth  == data->d_hwidth ) &&
                 ( vec->v_map->m_op      == data->d_op ) &&
                 ( vec->v_map->m_inplace == data->d_inplace ) &&
                 ( 0 == strncmp (data->d_fsname,
                                 e->em_orgfnctset->fs_name,
                                 strlen(e->em_orgfnctset->fs_name))) ) {
                found = i;
                periods = (int *)malloc( topo->t_ndims * sizeof(int) );
                dims = (int *)malloc( topo->t_ndims * sizeof(int) );
                coords = (int *)malloc( topo->t_ndims * sizeof(int) );

                MPI_Cart_get ( topo->t_comm, topo->t_ndims, dims, periods, coords );
                for ( j=0; j<topo->t_ndims; j++ ) {
                    if ( periods[j] != data->d_tperiods[j] ) {
                        found = -1;
                        break;
                    }
                }

                free( periods );
                free( dims );
                free( coords );

                for ( j=0 ; j<vec->v_ndims; j++ ){
                    if ( vec->v_dims[j] != data->d_vdims[j] ) {
                        found = -1;
                        break;
                    }
                }
                if ((NULL != vec->v_map->m_rcnts) && (NULL != vec->v_map->m_displ)) {
                    if ( data->d_np != size ) {
                        found = -1;
                        break; 
                    }
                    for ( j=0 ; j<topo->t_size; j++ ){
	                if ( ( vec->v_map->m_rcnts[j] != data->d_rcnts[j] ) ||
                           ( vec->v_map->m_displ[j] != data->d_displ[j] ) ) {
                            found = -1;
                            break;
                        }
                    }
                }
            }
            else {
                continue;
            }
            if ( found == -1 ) {
                continue;
            }
	    else {
                *found_data = data;
                ret = ADCL_IDENT;
                ADCL_printf("#%d An identical problem is found, winner is %s \n",
                            topo->t_rank, data->d_wfname);
            }
        }
    }
    return ret;
}

void ADCL_data_add_to_file( ADCL_data_t* data, ADCL_emethod_t *e )
{
    int rank, ndata;
    FILE *fp;
    int nchar = 80, nch;
    char *line = NULL;

    MPI_Comm_rank ( MPI_COMM_WORLD, &rank );

    if ( 0 == rank ) {
        
        fp = fopen ("ADCL.xml", "r");
        if(NULL == fp) {
            fp = fopen ("ADCL.xml", "w");
            fprintf ( fp, "<?xml version=\"1.0\" ?>\n<?xml-stylesheet"
                      " type=\"text/xsl\" href=\"ADCL.xsl\"?>\n<ADCL>\n" );
            fprintf ( fp, "  <NUM>1</NUM>\n" );
	}
	else {
            fclose(fp);
            fp = fopen ("ADCL.xml", "r+");
            line = (char *)malloc( nchar * sizeof(char) );
            /* Read the XML file line by line */
            fgets( line, nchar, fp ); /* XML header lines */
            fgets( line, nchar, fp );
            fgets( line, nchar, fp ); /* ADCL Tag */
            fgets( line, nchar, fp );
            nch = strlen(line);
            get_int_data_from_xml ( line, &ndata );
            fseek(fp, -nch, SEEK_CUR);
            fprintf ( fp, "  <NUM>%d</NUM>\n", ndata+1 );
            fseek(fp, -7, SEEK_END);
	}
        /* Use the user defined writing function of the according function set */
        if( NULL != e->em_orgfnctset->fs_hist_functions ) {
            if( NULL != e->em_orgfnctset->fs_hist_functions->hf_writer ) {
                e->em_orgfnctset->fs_hist_functions->hf_writer(fp, data);
	    }
        }
        fprintf ( fp, "</ADCL>" );
        fclose ( fp );
    }
    return;
}

void ADCL_data_read_from_file ( ADCL_emethod_t *e )
{
    int i, j, ndata;
    int nchar = 80;
    char *line = NULL;
    char *perf = NULL;
    ADCL_data_t *data;
    FILE *fp;
    long int pos;
    ADCL_fnctset_t *fnctset = e->em_orgfnctset;

    fp = fopen ("ADCL.xml", "r");
    if ( NULL == fp ) {
        return;
    }
    line = (char *)malloc( nchar * sizeof(char) );
    /* Read the XML file line by line */
    fgets( line, nchar, fp ); /* XML header lines */
    fgets( line, nchar, fp );
    fgets( line, nchar, fp ); /* ADCL Tag */
    fgets( line, nchar, fp );
    get_int_data_from_xml ( line, &ndata );
    
    for ( i=0; i<ndata; i++ ) {
        data = (ADCL_data_t *) calloc (1, sizeof(ADCL_data_t));
        if ( NULL == data ) {
            return;
        }
        
        /* Internal info for object management */
        data->d_id = ADCL_local_id_counter++;
        ADCL_array_get_next_free_pos ( ADCL_data_array, &data->d_findex );
        ADCL_array_set_element ( ADCL_data_array, data->d_findex, data->d_id, data );
        data->d_refcnt = 1;
        /* Use the user defined reading function of the according function set */
	if( NULL != fnctset->fs_hist_functions ) {
            if( NULL != fnctset->fs_hist_functions->hf_reader ) {
                fnctset->fs_hist_functions->hf_reader( fp, data );
            }
        }
        else {
            break;
        }
    }
    fclose ( fp );
    if(NULL != perf) {
        free ( perf );
    }
    if (NULL != line) {
        free ( line );
    }
    return;
}

int get_int_data_from_xml (char *str, int *res)
{
    char *n, *p;
    char *ext;
    int num;
    int ret = ADCL_ERROR_INTERNAL;
    n = strstr (str,">");
    p = strstr (str,"/");
    num = p-n-2;
    if ( NULL != n && NULL != p && num>0 ) {
        ext = (char *)calloc( num, sizeof(char) );
        strncpy ( ext, n+1, num );
        *res = atoi( ext );
        free( ext );
        ret = ADCL_SUCCESS;
    }
    return ret;
}

int get_str_data_from_xml (char *str, char **dest)
{
    char *n, *p;
    int num;
    int ret = ADCL_ERROR_INTERNAL;
    n = strstr (str,">");
    p = strstr (str,"/");
    if ( NULL != n && NULL != p ) {
        num = p-n-2;
        (*dest) = (char *)calloc( num, sizeof(char) );
        strncpy ((*dest), n+1, num);
        ret = ADCL_SUCCESS;
    }
    return ret;
}
