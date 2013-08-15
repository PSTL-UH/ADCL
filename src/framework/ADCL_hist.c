/*
 * Copyright (c) 2006-2007      University of Houston. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include <float.h>
#include "ADCL_internal.h"

/*** STATIC VARIABLES DECLARATION ***/
static int ADCL_local_id_counter = 0;
ADCL_array_t *ADCL_hist_array = NULL;
/* Prediction algorithm to be used : WMV, CLOSEST, NBC, SVM */
int ADCL_hist_predictor = ADCL_PRED_ALGO;
/* Whether a clustering of the history entries is applied or not */
int ADCL_hist_cluster = 1;

/* Create a new hist entry consisting on the problem characteristics */
static int hist_create_new ( ADCL_emethod_t *e, ADCL_hist_t *hist );
/* Re-initialization of the history object */
static void hist_reinit(ADCL_hist_t *hist);
/* Add a history entry to the history file */
static void hist_add_to_file( ADCL_hist_t* hist, ADCL_emethod_t *e );
/* Classify the implementation according to a given acceptable performance window */
static int hist_classify_implementations( double *elapsed_time, int *impl_class, int nb_of_impl, int perf_win );
/* Function to read the header of the history file */
static int hist_read_header(FILE **fp, int *nhist);
/* Reading of the history entries and checking for indentical solved problems */
static int hist_read_entries(FILE *fp, int nhist, ADCL_emethod_t *e );


/* Function to create a history entry after solving a problem */
int ADCL_hist_create ( ADCL_emethod_t *e )
{
    ADCL_hist_t *hist;
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
    hist = (ADCL_hist_t *) calloc (1, sizeof(ADCL_hist_t));
    hist->h_rcnts = (int *)calloc (size, sizeof(int));
    hist->h_displ = (int *)calloc (size, sizeof(int));
    if ( (NULL == hist) || (NULL == hist->h_rcnts) || (NULL == hist->h_displ) ) {
        return ADCL_NO_MEMORY;
    }
    /* Internal info for object management */
    hist->h_id = ADCL_local_id_counter++;
    ADCL_array_get_next_free_pos ( ADCL_hist_array, &hist->h_findex );
    ADCL_array_set_element ( ADCL_hist_array, hist->h_findex, hist->h_id, hist );
    hist->h_refcnt = 1;
    /* Network Topology information */
    hist->h_np = size;
    /* Logical Topology information */
    hist->h_tndims = topo->t_ndims;
    hist->h_tperiods = (int *)malloc( hist->h_tndims*sizeof(int) );
    dims = (int *)malloc( hist->h_tndims*sizeof(int) );
    coords = (int *)malloc( hist->h_tndims*sizeof(int) );
    MPI_Cart_get ( topo->t_comm, topo->t_ndims, dims, hist->h_tperiods, coords );
    free( dims );
    free( coords );
    /* Vector information */
    hist->h_vndims = vec->v_ndims;
    hist->h_vdims = (int *)malloc(hist->h_vndims*sizeof(int) );
    for ( i=0; i<hist->h_vndims ;i++ ) {
        hist->h_vdims[i] = vec->v_dims[i];
    }
    hist->h_nc = vec->v_nc;
    /* Vector map information */
    hist->h_vectype = vec->v_map->m_vectype;
    hist->h_hwidth = vec->v_map->m_hwidth;
    if ((NULL != vec->v_map->m_rcnts) && (NULL != vec->v_map->m_displ)) {
        for ( i=0; i<size; i++ ) {
            hist->h_rcnts[i] = vec->v_map->m_rcnts[i];
            hist->h_displ[i] = vec->v_map->m_displ[i];
        }
    }
    else {
        for ( i=0; i<size; i++ ) {
            hist->h_rcnts[i] = 0;
            hist->h_displ[i] = 0;
        }
    }
    hist->h_op = vec->v_map->m_op;
    hist->h_inplace = vec->v_map->m_inplace;
    /* Attribute information */
    if (NULL != e->em_orgfnctset->fs_attrset && ADCL_ATTRSET_NULL !=  e->em_orgfnctset->fs_attrset){
       hist->h_asmaxnum = e->em_orgfnctset->fs_attrset->as_maxnum;
       hist->h_attrvals = (int *)malloc( hist->h_asmaxnum * sizeof(int) );
       /* Initialization */
       for (i=0; i<hist->h_asmaxnum ; i++){
           hist->h_attrvals[i] = -1;
       }
       if ( ADCL_PERF_HYPO == e->em_search_algo ) {
           for (i=0; i<hist->h_asmaxnum ; i++){
               if( e->em_hypo->h_attr_confidence[i] > 1 ) {
                   hist->h_attrvals[i] = e->em_fnctset.fs_fptrs[0]->f_attrvals[i];
               }
           }
       }
    }
    /* Function set and winner function */
    hist->h_fsname = strdup ( e->em_orgfnctset->fs_name );
    hist->h_fsnum = e->em_orgfnctset->fs_maxnum;
    hist->h_wfname = strdup ( e->em_wfunction->f_name );
    hist->h_wfnum = ADCL_fnctset_get_fnct_num ( e->em_orgfnctset, e->em_wfunction );
    /* Performance hist for H.L. */
    /* Execution times */
    hist->h_perf = (double *)malloc(hist->h_fsnum*sizeof(double));
    for(i=0; i<hist->h_fsnum ; i++) {
        hist->h_perf[i] = e->em_stats[i]->s_gpts[ e->em_filtering];
    }
    /* The acceptable performance window */
    hist->h_perf_win = ADCL_PERF_WIN;
    hist->h_class = (int *)malloc(hist->h_fsnum*sizeof(int));
    /* Classifying the implementations */
    hist_classify_implementations( hist->h_perf, hist->h_class, hist->h_fsnum, ADCL_PERF_WIN );
    /* Set to Invalid dmax */
    hist->h_dmax = -1.00;

#ifdef ADCL_KNOWLEDGE_TOFILE
    /* Update the hist file */
    hist_add_to_file( hist, e );
#endif
    return ADCL_SUCCESS;
}

/* Function to create a new history entry for a problem without a solution */
static int hist_create_new ( ADCL_emethod_t *e, ADCL_hist_t *hist )
{
    ADCL_topology_t *topo = e->em_topo;
    ADCL_vector_t *vec  = e->em_vec;
    int i, size, topo_status;
    int *dims, *coords;

    /* For now we support only cartesian topology */
    MPI_Topo_test ( topo->t_comm, &topo_status );
    if ( ( MPI_CART != topo_status ) || ( ADCL_VECTOR_NULL == vec ) ) {
        return ADCL_INVALID_ARG;
    }
    size = topo->t_size;
    hist->h_rcnts = (int *)calloc (size, sizeof(int));
    hist->h_displ = (int *)calloc (size, sizeof(int));
    if ( (NULL == hist) || (NULL == hist->h_rcnts) || (NULL == hist->h_displ) ) {
        return ADCL_NO_MEMORY;
    }
    /* Internal info for object management */
    hist->h_id = ADCL_local_id_counter++;
    ADCL_array_get_next_free_pos ( ADCL_hist_array, &hist->h_findex );
    ADCL_array_set_element ( ADCL_hist_array, hist->h_findex, hist->h_id, hist );
    hist->h_refcnt = 1;
    /* Network Topology information */
    hist->h_np = size;
    /* Logical Topology information */
    hist->h_tndims = topo->t_ndims;
    hist->h_tperiods = (int *)malloc( hist->h_tndims*sizeof(int) );
    dims = (int *)malloc( hist->h_tndims*sizeof(int) );
    coords = (int *)malloc( hist->h_tndims*sizeof(int) );
    MPI_Cart_get ( topo->t_comm, topo->t_ndims, dims, hist->h_tperiods, coords );
    free( dims );
    free( coords );
    /* Vector information */
    hist->h_vndims = vec->v_ndims;
    hist->h_vdims = (int *)malloc(hist->h_vndims*sizeof(int) );
    for ( i=0; i<hist->h_vndims ;i++ ) {
        hist->h_vdims[i] = vec->v_dims[i];
    }
    hist->h_nc = vec->v_nc;
    /* Vector map information */
    hist->h_vectype = vec->v_map->m_vectype;
    hist->h_hwidth = vec->v_map->m_hwidth;
    if ((NULL != vec->v_map->m_rcnts) && (NULL != vec->v_map->m_displ)) {
        for ( i=0; i<size; i++ ) {
            hist->h_rcnts[i] = vec->v_map->m_rcnts[i];
            hist->h_displ[i] = vec->v_map->m_displ[i];
        }
    }
    else {
        for ( i=0; i<size; i++ ) {
            hist->h_rcnts[i] = 0;
            hist->h_displ[i] = 0;
        }
    }
    hist->h_op = vec->v_map->m_op;
    hist->h_inplace = vec->v_map->m_inplace;

    return ADCL_SUCCESS;
}

void ADCL_hist_free ( void )
{
    int i, last;
    ADCL_hist_t *hist;

    last = ADCL_array_get_last ( ADCL_hist_array );
    /* Free all the hist objects */
    for ( i=0; i<= last; i++ ) {
        hist = ( ADCL_hist_t * ) ADCL_array_get_ptr_by_pos( ADCL_hist_array, i );
        if ( NULL != hist  ) {
            if ( NULL != hist->h_tperiods ) {
                free ( hist->h_tperiods );
            }
            if ( NULL != hist->h_vdims ) {
                free ( hist->h_vdims );
            }
            if ( NULL != hist->h_rcnts ) {
                free ( hist->h_rcnts );
            }
            if ( NULL != hist->h_displ ) {
                free ( hist->h_displ );
            }
            if ( NULL != hist->h_attrvals ) {
                free ( hist->h_attrvals );
            }           
            if ( NULL != hist->h_fsname ) {
                free ( hist->h_fsname );
            }
            if ( NULL != hist->h_wfname ) {
                free ( hist->h_wfname );
            }
            if ( NULL != hist->h_perf ) {
                free ( hist->h_perf );
            }
            ADCL_array_remove_element ( ADCL_hist_array, hist->h_findex );
            free ( hist );
        }
    }
#ifdef ADCL_EXT_CONTRIB
    /* Free the SVM objects if it has been used */
    hist_svm_free();
#endif
    return;
}

/* Function to reinitialize an useless history object to be reused */
static void hist_reinit(ADCL_hist_t *hist)
{
    if ( NULL != hist ) {
        if ( NULL != hist->h_tperiods ) {
            free ( hist->h_tperiods );
	    hist->h_tperiods = NULL;
        }
        if ( NULL != hist->h_vdims ) {
            free ( hist->h_vdims );
	    hist->h_vdims = NULL;
        }
        if ( NULL != hist->h_rcnts ) {
            free ( hist->h_rcnts );
	    hist->h_rcnts = NULL;
        }
        if ( NULL != hist->h_displ ) {
            free ( hist->h_displ );
	    hist->h_displ = NULL;
        }
        if ( NULL != hist->h_attrvals ) {
            free ( hist->h_attrvals );
	    hist->h_attrvals = NULL;
        }           
        if ( NULL != hist->h_fsname ) {
            free ( hist->h_fsname );
	    hist->h_fsname = NULL;
        }
        if ( NULL != hist->h_wfname ) {
            free ( hist->h_wfname );
	    hist->h_wfname = NULL;
        }
        if ( NULL != hist->h_perf ) {
            free ( hist->h_perf );
	    hist->h_perf = NULL;
        }
        /* may be more stuff should be done */
    }
}

/* Function to find a (identical or similar) solution in the history entries */
int ADCL_hist_find ( ADCL_emethod_t *e )
{

    ADCL_fnctset_t *fnctset = e->em_orgfnctset;
    FILE *fp;
    int nhist = 0;
    int retval;

    if( -2 == e->em_explored_hist ) {
        /* Check if a reading function of the according function set do exist */
        if( NULL != fnctset->fs_hist_functions ) {
            if( NULL == fnctset->fs_hist_functions->hf_reader ) {
                ADCL_printf("No history reader function registered \n");
                goto exit;    
            }
            if( NULL == fnctset->fs_hist_functions->hf_distance ) {
                ADCL_printf("No history distance function registered \n");
                goto exit;    
            }
        }
        else {
            ADCL_printf("No history functions registered \n");
            goto exit;
        }
        /* Check if a filtering mechanism is set up */
        if( (NULL == fnctset->fs_hist_functions->hf_filter)||(NULL == e->em_hist_criteria) ) {
            ADCL_printf("No filtering function/criteria structures are registered \n");
	    ADCL_printf("This might lead to a very inaccurate prediction from the history data \n");
            goto exit;
        }

        /* Create a hist entry of the current problem without a solution */
        e->em_hist = (ADCL_hist_t *)calloc(1, sizeof(ADCL_hist_t));
        hist_create_new ( e, e->em_hist );

        /*** Read Header Function can be provided by the user ***/
        /* Read the history file header */
	if ( ADCL_SUCCESS != hist_read_header(&fp, &nhist) ) {
	    ADCL_printf("Unable to read history file\n");
	    goto exit;
	}

	/*** Reading the history entries and checking for indentical solved problems ***/
	retval = hist_read_entries(fp, nhist, e);
        /* close history file */
        fclose ( fp );
	if( ADCL_SUCCESS != retval ) {
            /* A memory error happened */
	    ADCL_printf("Not enough memory\n");
	    goto exit;
	}
	e->em_explored_hist = -1;
    }

    /*** Prediction of a solution from similar solved problems ***/
    if ( -1 == e->em_explored_hist ) {
        /* Check if a sufficient number of history entries 
           is available to try to make a prediction */
        if( ADCL_MIN_HIST > e->em_hist_cnt ) {
            ADCL_printf("The number of history entries is unsifficient to make a prediction\n");
            goto exit;
        }
        
        /*** Clustering of the history entries ***/
        if (ADCL_hist_cluster) {
#ifdef ADCL_EXT_CONTRIB
            hist_cluster ( e );
#else
	    printf("You can not use the clustering feature while disabling external contributions to ADCL\n");
	    exit(0);
#endif
        }
#ifdef OUTPUT_LIBSVM
	if ( ADCL_hist_cluster ) { /* Otherwise will seg fault if no clustering */
            /* In case we need the SVM format for an offline work */
            FILE *fp1;
            int i;
            int *dim;
            fp1 = fopen("hist.svm", "w");
            i = 0;
            while (NULL != hist_list->hl_curr) {
                dim = hist_list->hl_curr->h_vdims;
                /* for 3 D here, can be generalized*/
                if(0 == e->em_hist_clusters->hc_ids[i]) {
                    fprintf(fp1,"-1 %d:1 %d:1 %d:1 \n", dim[0], dim[1], dim[2]);
                }
                else {
                    fprintf(fp1, "+1 %d:1 %d:1 %d:1 \n", dim[0], dim[1], dim[2]);
                }
                i++;
                hist_list = hist_list->hl_next;
            }
            fclose(fp1);
        }
#endif
	switch ( ADCL_hist_predictor ) {
	    case ADCL_WMV:
	    case ADCL_CLOSEST:
                /* Prediction algorithms from neighbors within dmax */
		retval = hist_learn_from_neighbors( e );
		break;
	    case ADCL_NBC:
		/* Prediction algorithm using a Naive bayes classifier */
		retval = hist_learn_with_nbc( e );
		break;
	    case ADCL_SVM:
#ifndef ADCL_EXT_CONTRIB
		printf("You can not use SVM while disabling external contributions to ADCL\n");
		exit(0);
#else
		/* Prediction algorithm using a Support Vector Machine classifier */
		retval = hist_learn_with_svm( e );
#endif
		break;
	    default:
		break;
	}

	if ( ADCL_SIMILAR == retval ) {
	    switch ( ADCL_hist_predictor ) {
		case ADCL_WMV:
		    ADCL_printf("#%d A solution to the problem is predicted from similar problem(s) in history, winner is %d\n",
				e->em_topo->t_rank, e->em_hist->h_wfnum);
		    break;
		case ADCL_CLOSEST:
		    ADCL_printf("#%d A solution to the problem is predicted from the closest history entry, winner is %d\n",
				e->em_topo->t_rank, e->em_hist->h_wfnum);
		    break;
		case ADCL_NBC:
		    ADCL_printf("#%d A solution to the problem is predicted using a Naive Baysian classifier, winner is %d\n",
				e->em_topo->t_rank, e->em_hist->h_wfnum);
		    break;
		case ADCL_SVM:
		    ADCL_printf("#%d A solution to the problem is predicted using an SVM classification model, winner is %d\n",
				e->em_topo->t_rank, e->em_hist->h_wfnum);
		    break;
		default:
		    break;
	    }
	}
	else if ( ADCL_UNEQUAL == retval ) {
	    ADCL_printf("#No prediction can be done for this PS with the current history file\n");
	}
    }
    
exit:

    e->em_explored_hist = 0;
    return retval;
}

/* Function to add a hist object to the history file */
static void hist_add_to_file( ADCL_hist_t* hist, ADCL_emethod_t *e )
{
    int rank, nhist;
    FILE *fp;
    int nchar = 80, nch;
    char *line = NULL;

    MPI_Comm_rank ( MPI_COMM_WORLD, &rank );
    /* Only rank 0 process will be in charge of writing in the history file */
    if ( 0 == rank ) {
        /* Make sure that history functions are registered */
        if( NULL == e->em_orgfnctset->fs_hist_functions ) {
	    return;
	}
	else {
             /* Make sure that history writer function is registered */
            if( NULL == e->em_orgfnctset->fs_hist_functions->hf_writer ) {
		return;
	    }
        }
        /* Check if the file exists */
        fp = fopen ("hist.adcl", "r");
        if(NULL == fp) {
            /* First entry ever */
            fp = fopen ("hist.adcl", "w");
            fprintf ( fp, "<?xml version=\"1.0\" ?>\n<?xml-stylesheet"
                      " type=\"text/xsl\" href=\"ADCL.xsl\"?>\n<ADCL>\n" );
            fprintf ( fp, "  <NUM>1</NUM>\n" );
	}
	else {
            /* Close in read mode */
            fclose(fp);
            /* Open in read write mode */
            fp = fopen ("hist.adcl", "r+");
            line = (char *)malloc( nchar * sizeof(char) );
            /* Read the XML file line by line */
            fgets( line, nchar, fp ); /* XML header lines */
            fgets( line, nchar, fp );
            fgets( line, nchar, fp ); /* ADCL Tag */
            fgets( line, nchar, fp );
            nch = strlen(line);
            get_int_data_from_xml ( line, &nhist );
            fseek(fp, -nch, SEEK_CUR);
            fprintf ( fp, "  <NUM>%d</NUM>\n", nhist+1 );
            fseek(fp, -7, SEEK_END);
	}
        /* Use the user defined writing function of the according function set */
        e->em_orgfnctset->fs_hist_functions->hf_writer(fp, hist);
        /* End of file */
        fprintf ( fp, "</ADCL>" );
        /* Close the file */
        fclose ( fp );
    }
    return;
}

/* Function to classify the implementation to best and worst according to perf res and perf_win */
static int hist_classify_implementations( double *elapsed_time, int *impl_class, int nb_of_impl, int perf_win )
{
    int i, cnt, best_impl;
    double best_threshold;

    /* Initialization */
    cnt = 0;
    best_impl = 0;
    best_threshold = 1 + (perf_win/100.00);
    /* Searching for the best performing implementation */
    /* Might be done more efficiently later on */
    for(i=1; i<nb_of_impl; i++) {
        if(elapsed_time[i] < elapsed_time[best_impl]) {
            best_impl = i;
	}
    }
    /* Classification of the implementations */
    for(i=0; i<nb_of_impl; i++) {
        if(elapsed_time[i]/elapsed_time[best_impl] <= best_threshold) {
            impl_class[i] = ADCL_BEST_CLASS;
            cnt ++;
	}  
        else {
            impl_class[i] = ADCL_WORST_CLASS;
	}
    }

    return cnt;// TBD check if we really need that
}


/* Function to read the header of the history file */
static int hist_read_header(FILE **fp, int *nhist)
{
    /* Local file pointer */
    FILE *lfp;
    int nchar;
    char *line;

    nchar = 80;
    line = NULL;

    lfp = fopen ("hist.adcl", "r");
    if ( NULL == lfp ) {
        return ADCL_ERROR_INTERNAL;
    }
    /* Read the history file header */
    line = (char *)malloc( nchar * sizeof(char) );
    fgets( line, nchar, lfp ); /* XML header lines */
    fgets( line, nchar, lfp );
    fgets( line, nchar, lfp ); /* ADCL Tag */
    fgets( line, nchar, lfp );
    get_int_data_from_xml ( line, nhist );
    *fp = lfp;
    if ( NULL != line ) {
	free ( line );
    }
    return ADCL_SUCCESS;
}

/* Reading the history entries and checking for indentical solved problems */
static int hist_read_entries(FILE *fp, int nhist, ADCL_emethod_t *e )
{
    int i, pass_filter;
    ADCL_fnctset_t *fnctset = e->em_orgfnctset;
    ADCL_hist_t *hist;
    ADCL_hist_list_t *hist_list = e->em_hist_list;

    /* If no filtering mechanism, we check only for Identical history */
    /* Then only a single history object is enough */
    hist = (ADCL_hist_t *) calloc (1, sizeof(ADCL_hist_t));
    if ( NULL == hist ) {
	return ADCL_NO_MEMORY;
    }
    /* Internal info for object management */
    hist->h_id = ADCL_local_id_counter++;
    ADCL_array_get_next_free_pos ( ADCL_hist_array, &hist->h_findex );
    ADCL_array_set_element ( ADCL_hist_array, hist->h_findex, hist->h_id, hist );
    hist->h_refcnt = 1;
	
    for ( i=0; i<nhist; i++ ) {
	/* Use the user defined functions of the according function set for reading and filtering */
	/* Read the entry using the user predefined reading function */
	fnctset->fs_hist_functions->hf_reader( fp, hist );
	
	/* Filter the history entry */
	pass_filter = fnctset->fs_hist_functions->hf_filter(hist,
								e->em_hist_criteria->hc_filter_criteria);
	if( 0 == pass_filter) { /* did not pass */
	    hist_reinit(hist);
	}
	else {
	    /* Increment the count of history entries */
	    e->em_hist_cnt++;
	    /* Copy the history object handle in current */
	    hist_list->hl_curr = hist;
	    /* Some pre-processing for the WMV and CLOSEST prediction algorithms */
	    if( (ADCL_CLOSEST == ADCL_hist_predictor) || (ADCL_WMV == ADCL_hist_predictor) ) {
		/* Update the classes if the performance window has been changed */
		if(ADCL_PERF_WIN != hist->h_perf_win) {
		    /* Re-classify the implementations */
		    hist_classify_implementations( hist->h_perf, hist->h_class, hist->h_fsnum, ADCL_PERF_WIN );
		    /* Set to Invalid dmax */
		    hist->h_dmax = -1;
		}
	    }
	    /* Next */
	    hist_list->hl_next = (ADCL_hist_list_t *)calloc(1, sizeof(ADCL_hist_list_t));
	    hist_list = hist_list->hl_next;
	    /* Allocate a new history object */
	    hist = (ADCL_hist_t *) calloc (1, sizeof(ADCL_hist_t));
	    if ( NULL == hist ) {
		return ADCL_NO_MEMORY;
	    }
	    /* Internal info for object management */
	    hist->h_id = ADCL_local_id_counter++;
	    ADCL_array_get_next_free_pos ( ADCL_hist_array, &hist->h_findex );
	    ADCL_array_set_element ( ADCL_hist_array, hist->h_findex, hist->h_id, hist );
	    hist->h_refcnt = 1;
	}
    }

    return ADCL_SUCCESS;    
}

/* Get an integer data from XML line */
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
        ext = (char *)calloc( num+1, sizeof(char) );
        strncpy ( ext, n+1, num );
        *res = atoi( ext );
        free( ext );
        ret = ADCL_SUCCESS;
    }
    return ret;
}

/* Get a string data from XML line */
int get_str_data_from_xml (char *str, char **dest)
{
    char *n, *p;
    int num;
    int ret = ADCL_ERROR_INTERNAL;
    n = strstr (str,">");
    p = strstr (str,"/");
    if ( NULL != n && NULL != p ) {
        num = p-n-2;
        (*dest) = (char *)calloc( num+1, sizeof(char) );
        strncpy ((*dest), n+1, num);
        ret = ADCL_SUCCESS;
    }
    return ret;
}
