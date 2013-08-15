/*
 * Copyright (c) 2010-2013      University of Houston. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include <float.h>
#include <math.h>
#include "ADCL_internal.h"

/* Number of bins used in NBC */
#define ADCL_HIST_NUMBIN                10

/* Data structure for SVM prediction */
static ADCL_hist_svm_t *ADCL_hist_svm = NULL;
/* Data structure for SVM prediction */
static ADCL_hist_nbc_t *ADCL_hist_nbc = NULL;
/* Prediction algorithm to be used : 0-CLOSEST, 1-WMV, 2-NBC, 3-SVM */
extern int ADCL_hist_predictor;
/* Whether a clustering of the history entries is applied or not */
extern int ADCL_hist_cluster;


/***  Function for learning from neighbors  ***/
/* A function that filter an array of zeros and ones according to a window size */
static void filter_array( int *relation, int num_sizes );
/* Compute the maximum distance */
static double hist_compute_dmax(double *distance, int *relation, int num_sizes );
/* Compute the variances of the fitted Gaussian distribution */
static void hist_compute_variance( ADCL_hist_t *hist, ADCL_hist_list_t *hist_list );
/* Compute the probability density of Gaussian distribution */
static double hist_compute_gaussian( ADCL_hist_t *target, ADCL_hist_t *hist );

/***  Function for learning using a Naive Baysian Classifier (NBC) ***/
/* Initialization of the NBC data structure */
static void nbc_init( ADCL_emethod_t *e );
/* Training of the NBC from the history file */
static void nbc_train( ADCL_emethod_t *e );
/* Prediction of the solution using the NBC */
static void nbc_predict( ADCL_emethod_t *e );

/***  Function for learning using Support Vector Machine  ***/
/* Initialization of SVM parameters to the best values for c and gamma */
static void svm_init_params(ADCL_emethod_t *e);
/* To get the count of data items needed after scaling the data */
static int svm_scale_cnt(double *feature_max, double *feature_min, int index);
/* Does the actual scaling of the data */
static int svm_scale(double *feature_max, double *feature_min, int index, double value, struct svm_node *node);


int hist_learn_from_neighbors( ADCL_emethod_t *e )
{
    ADCL_fnctset_t *fnctset = e->em_orgfnctset;
    ADCL_hist_list_t *hist_list1;
    ADCL_hist_list_t *hist_list2;
    ADCL_hist_t *hist1;
    ADCL_hist_t *hist2;
    double *dup_distances;
    /* For closest algo */
    int i, j, retval, init;
    double  prob, max_prob;
    /* For WMV algo */
    double dist, max_weight;
    double *prediction_weight;

    /* Initialization */
    retval = ADCL_UNEQUAL;

    if (ADCL_WMV == ADCL_hist_predictor) {
	prediction_weight = (double *)calloc(e->em_orgfnctset->fs_maxnum, sizeof(double));
    }
    /* Memory allocation for tmp distance array */
    dup_distances = (double *)malloc(e->em_hist_cnt*sizeof(double));

    /* Computing distances and relations */
    /* Outer loop on hist1 */
    hist_list1 = e->em_hist_list;
    i=0;
    init = 0;
    while ( NULL != hist_list1 ) {
	/* Get history entry handle */
	hist1 = hist_list1->hl_curr;
	/* Check if it is not NULL */
	if(NULL == hist1) {
	    break;
	}
	else {
	    hist1->h_distances = (double *)malloc(e->em_hist_cnt*sizeof(double));
	    hist1->h_relations = (int *)malloc(e->em_hist_cnt*sizeof(int));
	}
	/* Inner loop on hist2 */
	hist_list2 = e->em_hist_list;
	j = 0;
	while ( NULL != hist_list2 ) {
	    /* Get history entry handle */
	    hist2 = hist_list2->hl_curr;
	    /* Check if it is not NULL */
	    if( NULL == hist2 ) {
		break;
	    }
	    /* Compute distance */
	    //can be optimized since it is a symmetric matrix
	    hist1->h_distances[j] = fnctset->fs_hist_functions->hf_distance(hist1, hist2);
	    /* Compute relation */
	    hist1->h_relations[j] = 0;
	    /* Is the winner of pb size i among the best impl of pb size j ? */
	    if ( ADCL_BEST_CLASS == hist2->h_class[hist1->h_wfnum] ) {
		hist1->h_relations[j] = 1;
	    }
	    /* Go to the next */
	    hist_list2 = hist_list2->hl_next;
	    j++;
	}
        /* Copy the distances array for dmax computation */
	memcpy(dup_distances, hist1->h_distances, e->em_hist_cnt*sizeof(double));
	/* Compute dmax of hist 1 */
	hist1->h_dmax = hist_compute_dmax( dup_distances, hist1->h_relations, e->em_hist_cnt );
        /* Compute the variances of the fitted Gaussian distribution */
        hist_compute_variance( hist1, e->em_hist_list );

#ifdef DMAX
	/* Compute the distance of the emethod hist and hist1 */
	dist = fnctset->fs_hist_functions->hf_distance(e->em_hist, hist1);
	if( dist <= hist1->h_dmax ) {
	    if (ADCL_WMV == ADCL_hist_predictor) {
		/* Add the weight to the predicted winner by hist1 */
		prediction_weight[hist1->h_wfnum] += 1/(dist+1);
	    } else if (ADCL_CLOSEST == ADCL_hist_predictor) {
		if( 0 == init ) {
		    /* set minimum distance */
		    min_dist = dist;
		    /* set the estimated winner */
		    e->em_hist->h_wfnum = hist1->h_wfnum;
		    /* set initted */
		    init = 1;
		    /* Here we are sure at least we have one prediction */
		    retval = ADCL_SIMILAR;
		}
		else if( dist < min_dist ){
		    /* set minimum distance */
		    min_dist = dist;
		    /* set the estimated winner */
		    e->em_hist->h_wfnum = hist1->h_wfnum;
		}
	    }
	}
#else /* Gaussian distribution */
        /* Compute the probability density of the target problem size on the history entry prob distribution */
	dist = fnctset->fs_hist_functions->hf_distance(e->em_hist, hist1);
	if( dist <= hist1->h_dmax ) {
	    prob = hist_compute_gaussian(e->em_hist, hist1);
	    /* Prediction based on the probabilities */
	    if (ADCL_WMV == ADCL_hist_predictor) {
		/* Add the weight to the predicted winner by hist1 */
		prediction_weight[hist1->h_wfnum] += prob;
	    } else if (ADCL_CLOSEST == ADCL_hist_predictor) {
		if( 0 == init ) {
		    /* Init max prob */
		    max_prob = prob;
		    /* set the estimated winner */
		    e->em_hist->h_wfnum = hist1->h_wfnum;
		    /* set initted */
		    init = 1;
		    /* Here we are sure at least we have one prediction */
		    retval = ADCL_SIMILAR;
		}
		else if( prob > max_prob ){
		    /* set max prob */
		    max_prob = prob;
		    /* set the estimated winner */
		    e->em_hist->h_wfnum = hist1->h_wfnum;
		}
	    }
	}
#endif
	/* Go to the next */
	hist_list1 = hist_list1->hl_next;
	i++;
    }
    /* Find the most weighted solution */
    if (ADCL_WMV == ADCL_hist_predictor) {
	max_weight = prediction_weight[0];
	e->em_hist->h_wfnum = 0;
	for(i=1; i<e->em_orgfnctset->fs_maxnum; i++) {
	    if ( prediction_weight[i] > max_weight ) {
		max_weight = prediction_weight[i];
		e->em_hist->h_wfnum = i;
	    }
	}
	retval = ADCL_SIMILAR;
	free(prediction_weight);
    }
    /* Free allocated memory for tmp use */
    free (dup_distances);

    return retval;
}

/* A function that filter an array of zeros and ones according to a window size */
static void filter_array( int *relation, int num_sizes )
{
    int i, j, cnt;
    int p = ADCL_SMOOTH_WIN/2;
    int *tmp;

    /* Allocate a temporary buffer for a copy of the original array */
    tmp = (int *)malloc(num_sizes*sizeof(int));
    memcpy(tmp, relation, num_sizes*sizeof(int));
    /* Filtering operation */
    for(i=p+1; i<num_sizes-p;i++) {
        cnt = 0;
        for (j=(i-p); j<=(i+p); j++) {
            if (1 == tmp[j]){
                cnt ++;
            }
        }
        if(cnt > p) {
            relation[i] = 1;
        }
    }
    /* Free allocated memory */
    free(tmp);
    return;
}

/* Function to compute dmax given an array of distances and relations */
static double hist_compute_dmax(double *distance, int *relation, int num_sizes )
{
    int i, r, k, extendable, bound, last_swap;
    double d, dmax;

    bound = num_sizes-1;
    dmax = 0;

    /* Sort distance array and move relation array with it */
    while (bound) {
	last_swap = 0;
	for ( k=0; k<bound; k++ ) {
	    d = distance[k]; /* t is a maximum of A[0]..A[k] */
	    r = relation[k];
	    if ( d > distance[k+1] ) {
		distance[k] = distance[k+1];
		distance[k+1] = d; /*swap*/
		relation[k] = relation[k+1];
		relation[k+1] = r; /*swap*/           
		last_swap = k; /* mark the last swap position */
	    }
	}
	bound=last_swap;
    }
#ifdef ADCL_SMOOTH_HIST
    /* Filtering of the relation array */
    filter_array( relation, num_sizes );
#endif
    /* searching the first 0 in the sorted and filtered array */
    extendable = 1;
    for (i=0; i<num_sizes; i++) {
        if ((relation[i]==1) && (extendable==1)) {
            dmax = distance[i];
        }
        if(relation[i] == 0) {
            extendable = 0;
	    break;
	}
    }
    return dmax;
}

/* Compute the variances of the fitted Gaussian distribution */
static void hist_compute_variance( ADCL_hist_t *hist, ADCL_hist_list_t *hist_list )
{
    int i, j, k;
    ADCL_hist_list_t *hl;
    ADCL_hist_t *h;
    /* Memeory allocation of the means and variances array */
    hist->h_variance=(double *)calloc(hist->h_vndims, sizeof(double));
    hist->h_mean = (double *)calloc(hist->h_vndims, sizeof(double));

    ADCL_printf("PB size " );for(i=0; i<hist->h_vndims; i++) ADCL_printf("%d ", hist->h_vdims[i]); ADCL_printf("\n" );
    /* Special cases */
    if(0 == hist->h_dmax) {
        for(i=0; i<hist->h_vndims; i++) {
            hist->h_mean[i] = (double)(hist->h_vdims[i]);
            hist->h_variance[i] = 0;
        }
    }
    else {
        i = 0;
        k = 0;
        hl = hist_list;
        while ( NULL != hl ) {
            /* Get history entry handle */
            h = hl->hl_curr;
            /* Check if it is not NULL */
            if( NULL == h ) {
        	break;
            }
            /* Compute mean */
            if( hist->h_distances[k] <= hist->h_dmax ) {
		for(j=0; j<hist->h_vndims; j++) {
		    hist->h_mean[j] += (double)(h->h_vdims[j]);
		}
		i++;
	    }
            /* Go to the next */
            hl = hl->hl_next;
            k++;
        }
        for(j=0; j<hist->h_vndims; j++) {
            hist->h_mean[j] = hist->h_mean[j]/i;
        }
        /* Compute the variance for each direction */
        i = 0;
        k = 0;
        hl = hist_list;
        while ( NULL != hl ) {
            /* Get history entry handle */
            h = hl->hl_curr;
            /* Check if it is not NULL */
            if( NULL == h ) {
                break;
            }
            /* Compute variance */
            if( hist->h_distances[k] <= hist->h_dmax ) {
		for(j=0; j<hist->h_vndims; j++) {
		    hist->h_variance[j] += pow(((double)(h->h_vdims[j]) - hist->h_mean[j]), 2);
		}
		i++;
            }
            /* Go to the next */
            hl = hl->hl_next;
            k++;
        }
	for(j=0; j<hist->h_vndims; j++) {
            hist->h_variance[j] = sqrt(hist->h_variance[j]/(i-1));
        }
    }
    return;
}

/* Compute the probability density of Gaussian distribution */
double hist_compute_gaussian( ADCL_hist_t *target, ADCL_hist_t *hist )
{
    double prob;
    int i;
    prob = 0;
    for(i=0; i<target->h_vndims; i++) {
	if( 0 == hist->h_variance[i] ) {
	    return 0.0;
	}
	prob += pow( (target->h_vdims[i]-hist->h_mean[i])/hist->h_variance[i], 2);
    }
    prob = exp(-0.5*prob);

    return prob;
}

/* Prediction algorithm using a Naive bayes classifier */
int hist_learn_with_nbc( ADCL_emethod_t *e )
{

    ADCL_hist_nbc_t *hn;

    /* Initializations */
    hn = ADCL_hist_nbc;

    /* Creating the NBC model */
    if ( NULL == hn) {
        /* Memory allocation of the data structure for NBC prediction */
	ADCL_hist_nbc = (ADCL_hist_nbc_t *)calloc(1, sizeof(ADCL_hist_nbc_t));
	hn = ADCL_hist_nbc; /* for shorter and easier use */
	/* Initialize the nbc parameters to the default */
	nbc_init(e);
	/* Training of the NBC from the history file */
	nbc_train(e);
    }
    /* Predict the winner for the current problem size */
    nbc_predict(e);

    return ADCL_SIMILAR;
}

/* Initialization of the NBC data structure */
static void nbc_init( ADCL_emethod_t *e )
{
    int i, j;
    ADCL_hist_nbc_t *hn;
    ADCL_hist_list_t *hl;

    /* Initializations */
    hn = ADCL_hist_nbc;
    hl = e->em_hist_list;
    /* Number of data entries in history */
    hn->hn_histnum = e->em_hist_cnt;
    /* Dimension of the data = number of features */
    hn->hn_fnum = hl->hl_curr->h_vndims;
    /* Number of classes */
    if(ADCL_hist_cluster) {  // chk config later
	hn->hn_classnum = NB_OF_CLUSTERS;
    }
    else { /* Number of functions in function-set */
           /* Some of the classes might be empty  */
	hn->hn_classnum = hl->hl_curr->h_fsnum;
    }
    /* Bins configuration */
    hn->hn_binnum = ADCL_HIST_NUMBIN;
    /* Bin start / end */
    hn->hn_binstart = hl->hl_curr->h_vdims[0];
    hn->hn_binend = hl->hl_curr->h_vdims[0];
    for(i=0; i<hn->hn_histnum; i++ ) {
	for( j=0; j<hn->hn_fnum; j++ ) {
	    if(hl->hl_curr->h_vdims[j] > hn->hn_binend) {
		hn->hn_binend = hl->hl_curr->h_vdims[j];
	    }
	    if(hl->hl_curr->h_vdims[j] < hn->hn_binstart) {
		hn->hn_binstart = hl->hl_curr->h_vdims[j];
	    }
	}
	hl = hl->hl_next;
    }
    hn->hn_binsize = (hn->hn_binend-hn->hn_binstart)/((double) hn->hn_binnum);
    /* Memory allocations */
    hn->hn_p2train = (int **) malloc(hn->hn_histnum * sizeof(int *));
    for(i=0; i<hn->hn_histnum; i++) {
	hn->hn_p2train[i] = (int *) malloc((hn->hn_fnum+1)*sizeof(int));
    }
    hn->hn_p2x = (int **) malloc(hn->hn_classnum * sizeof(int *));
    for(i=0; i<hn->hn_classnum; i++) {
	hn->hn_p2x[i] = (int *) calloc(hn->hn_fnum * hn->hn_binnum, sizeof(int));
    }
    hn->hn_pc = (double *) calloc(hn->hn_classnum, sizeof(double));
    hn->hn_p2t = (int *) calloc(hn->hn_fnum, sizeof(int));

    return;
}

/* Training of the NBC from the history file */
static void nbc_train( ADCL_emethod_t *e )
{
    int i, j, binidx;
    ADCL_hist_nbc_t *hn;
    ADCL_hist_list_t *hl;
    ADCL_hist_clusters_t *hc;

    /* Initializations */
    hn = ADCL_hist_nbc;
    hl = e->em_hist_list;
    hc = e->em_hist_clusters;

    for(i=0; i<hn->hn_histnum; i++ ) {
	for( j=0; j<hn->hn_fnum; j++ ) {
	    binidx = (int) floor((hl->hl_curr->h_vdims[j]-hn->hn_binstart)/hn->hn_binsize);
	    if(binidx == hn->hn_binnum) {
		binidx = hn->hn_binnum - 1;
	    }
	    hn->hn_p2train[i][j] = binidx;
	}
	if(ADCL_hist_cluster) {
	    hn->hn_p2train[i][j] = hc->hc_ids[i];
	}
	else {
	    hn->hn_p2train[i][j] = hl->hl_curr->h_wfnum;
	}
	hl = hl->hl_next;
    }
    /* Calculate marginal probability for each class */
    for(i=0; i<hn->hn_histnum; i++) {
	hn->hn_pc[ hn->hn_p2train[i][hn->hn_fnum] ]++;
    }
    /* Normalization, not needed
    for(i=0;i<numsc;i++) {
	pc[i] = pc[i]/numsl;
    } */
    /*  */
    for(i=0; i<hn->hn_histnum; i++) {
	for(j=0; j<hn->hn_fnum; j++) {
	    hn->hn_p2x[hn->hn_p2train[i][hn->hn_fnum] ][hn->hn_binnum*j+hn->hn_p2train[i][j]]++;
	}
    }

    return;
}

/* Prediction of the solution using the NBC */
static void nbc_predict( ADCL_emethod_t *e )
{

    int i, j, binidx, label;
    double *p2val, tmp;
    ADCL_hist_nbc_t *hn;

    /* Initializations */
    hn = ADCL_hist_nbc;
    p2val = (double *)malloc(hn->hn_classnum * sizeof(double));
    for( i=0; i<hn->hn_fnum; i++ ) {
	binidx = (int) floor((e->em_hist->h_vdims[i]-hn->hn_binstart)/hn->hn_binsize);
	if(binidx == hn->hn_binnum) {
	    binidx = hn->hn_binnum - 1;
	}
	hn->hn_p2t[i] = binidx;
    }

    for(i=0; i<hn->hn_classnum; i++) {
	tmp = 1;
	for(j=0; j<hn->hn_fnum; j++) {
	    tmp = tmp*((hn->hn_p2x[i][j*hn->hn_binnum+hn->hn_p2t[j]]+0.5)/hn->hn_pc[i]);
	}
	tmp = tmp * hn->hn_pc[i];
	p2val[i] = tmp;
    }

    tmp = 0;
    label = 0;
    for(i=0; i<hn->hn_classnum; i++) {
	if(p2val[i] > tmp) {
	    tmp = p2val[i];
	    label = i;
	}
    }

    if(ADCL_hist_cluster) {
	e->em_hist->h_wfnum = e->em_hist_clusters->hc_winners[label];
    }
    else {
	e->em_hist->h_wfnum = label;
    }
    free(p2val);

    return;
}

#ifdef ADCL_EXT_CONTRIB
/* Prediction algorithm using a Support Vector Machine classifier */
int hist_learn_with_svm( ADCL_emethod_t *e )
{
    int i, j, k, K, pos, x_size;
    int elements, index, next_index;
    ADCL_hist_svm_t *hs;
    ADCL_hist_list_t *hl;
    ADCL_hist_clusters_t *hc;
    struct svm_node *x;
    double result;

    /* Initializations */
    hs = ADCL_hist_svm;
    hl = e->em_hist_list;
    hc = e->em_hist_clusters;
    /* Number of features for lib SVM use */
    K = hl->hl_curr->h_vndims;

    /* Creating the SVM model */
    if ( NULL == hs) {
        /* Memory allocation of the data structure for SVM prediction */
	ADCL_hist_svm = (ADCL_hist_svm_t *)calloc(1, sizeof(ADCL_hist_svm_t));
	hs = ADCL_hist_svm; /* for shorter and easier use */
	/* Initialize prob */
	/* Number of history data entries for training */
	hs->hs_prob.l = hc->hc_nrows;
	/* For scaling */
	hs->hs_max_index = 0;
	/* Memeory allocation */
	hs->hs_prob.x = (struct svm_node **)malloc( hs->hs_prob.l * sizeof(struct svm_node *) );
	hs->hs_prob.y = (double *)malloc( hs->hs_prob.l * sizeof(double) );
	/* Parsing the history list and filling up the SVM nodes */
	for(i=0; i<hs->hs_prob.l; i++ ) {
	    /* The labels */
	    if(0 == hc->hc_ids[i]) {
		hs->hs_prob.y[i] = -1.0;
	    }
	    else {
		hs->hs_prob.y[i] = 1.0;
	    }

	    for( j=0; j<K; j++ ) {
		if(hl->hl_curr->h_vdims[j] >hs->hs_max_index) {
		    hs->hs_max_index = hl->hl_curr->h_vdims[j];
		}
	    }
	    hl = hl->hl_next;
	}
	/* Default value for gamma if not set */
	if(hs->hs_param.gamma == 0) {
	    hs->hs_param.gamma = 1.0/hs->hs_max_index;
	}
	/* Allocating the features min/max arrays */
	hs->hs_feature_max = (double *)malloc((hs->hs_max_index+1)* sizeof(double));
	hs->hs_feature_min = (double *)malloc((hs->hs_max_index+1)* sizeof(double));
	/* Initializing the features min/max arrays */
	for(i=0; i<=hs->hs_max_index; i++) {
	    hs->hs_feature_max[i] = -DBL_MAX;
	    hs->hs_feature_min[i] = DBL_MAX;
	}
	/* Computing the features min/max arrays */
	hl = e->em_hist_list;
	for(i=0; i<hs->hs_prob.l; i++ ) {
	    next_index = 1;
	    for( j=0; j<K; j++ ) {
		index = hl->hl_curr->h_vdims[j];
		for(k=next_index; k<index; k++) {
		    hs->hs_feature_max[k] = max(hs->hs_feature_max[k], 0.0);
		    hs->hs_feature_min[k] = min(hs->hs_feature_min[k], 0.0);
		}
		hs->hs_feature_max[index] = max(hs->hs_feature_max[index], 1.0);
		hs->hs_feature_min[index] = min(hs->hs_feature_min[index], 1.0);
		
		next_index = index+1;
	    }
	    
	    for(j=next_index; j<=hs->hs_max_index; j++)
	    {
		hs->hs_feature_max[j] = max(hs->hs_feature_max[j],0.0);
		hs->hs_feature_min[j] = min(hs->hs_feature_min[j],0.0);
	    }
	    hl = hl->hl_next;
	}
	/* Counting */
	elements = 0;
	hl = e->em_hist_list;
	for(i=0; i<hs->hs_prob.l; i++ ) {
	    next_index=1;
	    for( j=0; j<K; j++ ) {
		index = hl->hl_curr->h_vdims[j];
		for(k=next_index; k<index; k++) {
		    elements += svm_scale_cnt(hs->hs_feature_max, hs->hs_feature_min, k);
		}
		elements += svm_scale_cnt(hs->hs_feature_max, hs->hs_feature_min, index);
		next_index = index + 1;
	    }
	    for(j=next_index;j<=hs->hs_max_index;j++){
		elements += svm_scale_cnt(hs->hs_feature_max, hs->hs_feature_min, j);
	    }
	    hl = hl->hl_next;
	    elements++; /* for the -1 at the end */
	}
	/* Memory allocation for the needed svm nodes */
	hs->hs_x_space = (struct svm_node *)malloc(elements * sizeof(struct svm_node));
	/* Actual Scaling of the data */
	pos = 0;
	hl = e->em_hist_list;
	for(i=0; i<hs->hs_prob.l; i++ ) {
	    next_index = 1;
	    hs->hs_prob.x[i] = &(hs->hs_x_space[pos]);
	    for( j=0; j<K; j++ ) {
		index = hl->hl_curr->h_vdims[j];
		for(k=next_index; k<index; k++) {
		    pos += svm_scale(hs->hs_feature_max, hs->hs_feature_min, k, 0.0, &(hs->hs_x_space[pos]));
		}
		pos += svm_scale(hs->hs_feature_max, hs->hs_feature_min, index, 1.0, &(hs->hs_x_space[pos]));
		next_index = index + 1;
	    }
	    for(j=next_index;j<=hs->hs_max_index;j++){
		pos += svm_scale(hs->hs_feature_max, hs->hs_feature_min, j, 0.0, &(hs->hs_x_space[pos]));
	    }
	    hl = hl->hl_next;
	    hs->hs_x_space[pos].index = -1;
	    pos++;
	}
	/* Initialize the svm parameters to the bests values for c and gamma */
	svm_init_params(e);
	/* Creating the SVM model using the history data for training */
	hs->hs_model = svm_train( &(hs->hs_prob), &(hs->hs_param));
//	svm_save_model("adcl.model",hs->hs_model);
    }
    /* count for x */
    next_index=1;
    x_size = 0;
    for( j=0; j<K; j++ ) {
	index = e->em_hist->h_vdims[j];
	for(k=next_index; k<index; k++) {
	    x_size += svm_scale_cnt(hs->hs_feature_max, hs->hs_feature_min, k);
	}
	x_size += svm_scale_cnt(hs->hs_feature_max, hs->hs_feature_min, index);
	next_index = index + 1;
    }
    for(j=next_index;j<=hs->hs_max_index;j++){
	x_size += svm_scale_cnt(hs->hs_feature_max, hs->hs_feature_min, j);
    }
    x_size++;
    x = (struct svm_node *)malloc(x_size * sizeof(struct svm_node));
    /* Scaling x */
    next_index = 1;
    pos = 0;
    for( j=0; j<K; j++ ) {
	index = e->em_hist->h_vdims[j];
	for(k=next_index; k<index; k++) {
	    pos += svm_scale(hs->hs_feature_max, hs->hs_feature_min, k, 0.0, &(x[pos]));
	}
	pos += svm_scale(hs->hs_feature_max, hs->hs_feature_min, index, 1.0, &(x[pos]));
	next_index = index + 1;
    }
    for(j=next_index;j<=hs->hs_max_index;j++){
	pos += svm_scale(hs->hs_feature_max, hs->hs_feature_min, j, 0.0, &(x[pos]));
    }
    x[pos].index = -1;

    /*** Prediction for the new problem x ***/

    /* Do the prediction using the constructed svm model */
    result = svm_predict(hs->hs_model, x);
    /* Select the predicted winner function */
    if (-1.0 == result) {
	e->em_hist->h_wfnum = e->em_hist_clusters->hc_winners[0];
    }
    else {
	e->em_hist->h_wfnum = e->em_hist_clusters->hc_winners[1];
    }
    free(x);
    return ADCL_SIMILAR;
}

static void svm_init_params( ADCL_emethod_t *e )
{
    ADCL_hist_svm_t *hs;
    ADCL_topology_t *topo;

    /* Initializations */
    hs = ADCL_hist_svm;
    topo = e->em_topo;
    /* Basic Initializations */
    hs->hs_param.svm_type = C_SVC;
    hs->hs_param.kernel_type = RBF;
    hs->hs_param.degree = 3;
    hs->hs_param.gamma = 0.001953125;  // 1/k  or defined by grid.py
    hs->hs_param.coef0 = 0;
    hs->hs_param.nu = 0.5;
    hs->hs_param.cache_size = 100;
    hs->hs_param.C = 128;             // defined with grid.py
    hs->hs_param.eps = 1e-3;
    hs->hs_param.p = 0.1;
    hs->hs_param.shrinking = 1;
    hs->hs_param.probability = 0;
    hs->hs_param.nr_weight = 0;
    hs->hs_param.weight_label = NULL;
    hs->hs_param.weight = NULL;

#ifndef PARAM_SEARCH
    /* Looking for the best parameter values for c and gamma */
    int c_begin, c_end, c_step, c_num;
    int g_begin, g_end, g_step, g_num;
    int perm_num, fold, i, j, best, total_correct;
    int *c_array, *g_array;
    double *target, *accuracy, *g_accuracy, best_accuracy;
    c_begin = -5;
    c_end = 15;
    c_step = 2;
    g_begin = 3;
    g_end = -15;
    g_step = -2;
    fold = 5;
    c_num = abs(c_end-c_begin)/2 + 1;
    g_num = abs(g_end-g_begin)/2 + 1;
    perm_num = c_num * g_num;
    /* Memory allocation */
    c_array = (int *)malloc(perm_num * sizeof(int));
    g_array = (int *)malloc(perm_num * sizeof(int));
    accuracy = (double *)calloc(perm_num, sizeof(double));
    g_accuracy = (double *)calloc(perm_num, sizeof(double));

    /* Generating the possible combinations of c and g */
    for(i=0; i<c_num; i++) {
	c_array[i] = c_begin + c_step * i;
	g_array[i] = g_begin;
    }
    for (j=1; j<g_num; j++) {
	memcpy(&c_array[j*c_num], c_array, c_num*sizeof(int));
	for(i=j*c_num; i<(j+1)*c_num; i++) {
	    g_array[i] = g_begin + g_step * j;
	}
    }

    /* Testing the parameter combinations  */
    target = (double *)malloc(hs->hs_prob.l * sizeof(double));
    for(i=topo->t_rank; i<perm_num; i+=topo->t_size) {
	hs->hs_param.C = pow(2.0, c_array[i]);
	hs->hs_param.gamma = pow(2.0, g_array[i]);
	svm_cross_validation(&(hs->hs_prob), &(hs->hs_param), fold, target);
	total_correct = 0;
	for(j=0; j<hs->hs_prob.l; j++){
	    if(target[j] == hs->hs_prob.y[j]) {
		++total_correct;
	    }
	}
	accuracy[i] = 100.0*total_correct/hs->hs_prob.l;
    }
    /* Global reduction */
    MPI_Allreduce(accuracy, g_accuracy, perm_num, MPI_DOUBLE, MPI_MAX, topo->t_comm);
    /* Get the best one */
    best = 0;
    best_accuracy = 0.0;
    for(i=0; i<perm_num; i++) {
	if ((g_accuracy[i] > best_accuracy)||
	    ((g_accuracy[i] == best_accuracy) && (g_array[i]==g_array[best]) && (c_array[i]<c_array[best]) )){
	    best = i;
	    best_accuracy = g_accuracy[i];
	}
    }
    /* Setting the best values for C and Gamma */
    hs->hs_param.C = pow(2.0, c_array[best]);
    hs->hs_param.gamma = pow(2.0,g_array[best]);
    /* Free allocated memory */
    free(target);
    free(c_array);
    free(g_array);
    free(accuracy);
    free(g_accuracy);
#endif
    return;
}

static int svm_scale_cnt(double* feature_max, double* feature_min, int index)
{
    int ret = 1;
    if(feature_max[index] == feature_min[index]) {
	ret = 0;
    }
    return ret;
}

static int svm_scale(double* feature_max, double* feature_min, int index, double value, struct svm_node *node)
{
    double lower, upper; 

    lower=-1.0;
    upper=1.0;

    /* skip single-valued attribute */
    if(feature_max[index] == feature_min[index]) {
	return 0;
    }
    if(value == feature_min[index]) {
	value = lower;
    }
    else if(value == feature_max[index]) {
	value = upper;
    }
    else {
	value = lower + (upper-lower) * 
	    (value-feature_min[index])/
	    (feature_max[index]-feature_min[index]);
    }

    if(value != 0) {
	node->index = index;
	node->value = value;
    }

    return 1;
}

void hist_svm_free()
{
    if( NULL != ADCL_hist_svm) {
	if( NULL != ADCL_hist_svm->hs_feature_max ) {
	    free(ADCL_hist_svm->hs_feature_max);
	}
	if( NULL != ADCL_hist_svm->hs_feature_min ) {
	    free(ADCL_hist_svm->hs_feature_min);
	}
	if( NULL != ADCL_hist_svm->hs_model ) {
	    svm_destroy_model(ADCL_hist_svm->hs_model);
	}
	if (NULL != ADCL_hist_svm->hs_prob.x ) {
	    free(ADCL_hist_svm->hs_prob.x);
	}
	if (NULL != ADCL_hist_svm->hs_prob.y ) {
	    free(ADCL_hist_svm->hs_prob.y);
	}
	if ( NULL != ADCL_hist_svm->hs_x_space ) {
	    free(ADCL_hist_svm->hs_x_space);
	}
	free(ADCL_hist_svm);
    }
}
#endif // ADCL_EXT_CONTRIB
