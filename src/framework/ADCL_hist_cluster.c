/*
 * Copyright (c) 2006-2007      University of Houston. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include "ADCL_internal.h"

/* History clustering verbosity */
//#define HC_V                            1

/* Initialization for the history clustering */
static void hist_cluster_init_vars( ADCL_hist_clusters_t *hist_clusters,  ADCL_hist_list_t *hist_list )
{
    int i;
    ADCL_hist_list_t *hl = hist_list;
    /* Initializations and memory allocations */
    hist_clusters->hc_ncolumns = hl->hl_curr->h_fsnum;
    /* Compute the number of hist entries in hl */
    hist_clusters->hc_nrows = 0;
    while(NULL != hl->hl_curr){
        hl = hl->hl_next;
        hist_clusters->hc_nrows++;
    }
    hist_clusters->hc_data = (double **)malloc(hist_clusters->hc_nrows * sizeof(double*));
    hist_clusters->hc_mask = (int **)malloc(hist_clusters->hc_nrows * sizeof(int*));
    for(i=0;i<hist_clusters->hc_nrows; i++){
        hist_clusters->hc_data[i] = (double *)malloc(hist_clusters->hc_ncolumns * sizeof(double));
        hist_clusters->hc_mask[i] = (int *)malloc(hist_clusters->hc_ncolumns * sizeof(int));
    }
    hist_clusters->hc_weight = (double *)malloc(hist_clusters->hc_ncolumns * sizeof(double));
    for(i=0;i<hist_clusters->hc_ncolumns; i++){
        hist_clusters->hc_weight[i] = 1;
    }
    /* Clustering col or row wise */
    hist_clusters->hc_transpose = 0;
    /* Use euclidian distance for now */
    hist_clusters->hc_dist = 'c';
    /* Average linkage clustering */
    hist_clusters->hc_method = 'a';
    /* The result of treecluster */
    hist_clusters->hc_tree = NULL;
    /* The results of cuttree */
    hist_clusters->hc_ids = (int *)malloc(hist_clusters->hc_nrows*sizeof(int));
}

void hist_cluster_init_data( ADCL_hist_clusters_t *hist_clusters,  ADCL_hist_list_t *hist_list )
{
    int i, j;
    double win_perf;
    ADCL_hist_list_t *hl;
    hl = hist_list;
    for(i=0; i<hist_clusters->hc_nrows; i++) {
        /* Copy performance array on the according row of data */
        memcpy(hist_clusters->hc_data[i], hl->hl_curr->h_perf,hist_clusters->hc_ncolumns*sizeof(double));
        win_perf = hl->hl_curr->h_perf[hl->hl_curr->h_wfnum];
        /* Init mask and normalize data */
        for(j=0; j<hist_clusters->hc_ncolumns; j++) {
            hist_clusters->hc_mask[i][j] = 1;
            hist_clusters->hc_data[i][j] = 200*(hist_clusters->hc_data[i][j] - win_perf) / (hist_clusters->hc_data[i][j] + win_perf);
        }
        /* Go to the nest hist */
        hl = hl->hl_next;
    }
#ifdef HC_V
    for(i=0; i<hist_clusters->hc_nrows; i++) {
        printf("L%d: ", i);
        for(j=0; j<hist_clusters->hc_ncolumns; j++) {
            printf(" %f ", hist_clusters->hc_data[i][j]);
        }
        printf("\n");
    }
#endif
}

void hist_cluster_hierarchical( ADCL_hist_clusters_t *hist_clusters )
{

    /* Run the tree clustering */
    hist_clusters->hc_tree = treecluster( hist_clusters->hc_nrows,
                                   hist_clusters->hc_ncolumns,
                                   hist_clusters->hc_data,
                                   hist_clusters->hc_mask,
                                   hist_clusters->hc_weight,
                                   hist_clusters->hc_transpose,
                                   hist_clusters->hc_dist,
                                   hist_clusters->hc_method,
                                   NULL);

    /* Create the two clusters from the tree */
    cuttree( hist_clusters->hc_nrows,
	     hist_clusters->hc_tree, NB_OF_CLUSTERS,
	     hist_clusters->hc_ids );

#ifdef HC_V
    if( NULL == hist_clusters->hc_tree ) {
        printf("Clustering error \n");
    }
    else {
        printf("Clustering done successfully, processing the results \n");
        for(i=0; i<hist_clusters->hc_nrows-1; i++) {
             printf("node %d right=%d left =%d distance =%f\n", i,
                   hist_clusters->hc_tree[i].right,
                   hist_clusters->hc_tree[i].left,
                   hist_clusters->hc_tree[i].distance);
        }
        for(i=0; i<hist_clusters->hc_nrows; i++) {
            printf("elt%d -> C%d \n", i, hist_clusters->hc_ids[i]);
        }
    }
#endif
    return;
}

/* Deternime the winners to be used for each cluster */
void hist_cluster_get_winners( ADCL_hist_clusters_t *hist_clusters )
{
    int i, j;
    double **clusters_perf;
    double min[NB_OF_CLUSTERS];

    clusters_perf = (double **)malloc(NB_OF_CLUSTERS * sizeof(double *));
    for(i=0; i<NB_OF_CLUSTERS; i++) {
        clusters_perf[i] = (double *)calloc(hist_clusters->hc_ncolumns, sizeof(double));
    }
    for(i=0; i<hist_clusters->hc_nrows; i++) {
        for(j=0; j<hist_clusters->hc_ncolumns; j++) {
            clusters_perf[hist_clusters->hc_ids[i]][j] += hist_clusters->hc_data[i][j];
        }
    }
    /* Winners initialization to 0 */
    hist_clusters->hc_winners[0] = 0;
    hist_clusters->hc_winners[1] = 0;
    min[0] = clusters_perf[0][0];
    min[1] = clusters_perf[1][0];
    for(j=1; j<hist_clusters->hc_ncolumns; j++) {
        if(clusters_perf[0][j]< min[0]) {
            min[0] = clusters_perf[0][j];
            hist_clusters->hc_winners[0] = j;
        }
        if(clusters_perf[1][j]< min[1]) {
            min[1] = clusters_perf[1][j];
            hist_clusters->hc_winners[1] = j;
        }
    }
#ifdef HC_V
    printf("selected winners for cluster 0 is %d and for cluster 1 is %d \n", 
           hist_clusters->hc_winners[0],  hist_clusters->hc_winners[1]);
#endif
}

int hist_cluster ( ADCL_emethod_t *e )
{
   
    ADCL_hist_list_t *hl;
    ADCL_hist_clusters_t *hc;

    /* Initialization of the hist list */
    hl = e->em_hist_list;
    /* Memory Allocation of the data structure */
    e->em_hist_clusters = (ADCL_hist_clusters_t *)malloc(sizeof(ADCL_hist_clusters_t));
    hc = e->em_hist_clusters;
    /* Initalization of the clustering parameters */
    hist_cluster_init_vars(hc, hl);
    /* Initalization of the data */
    hist_cluster_init_data(hc, hl);
    /* Perform hierarchical clustering */
    hist_cluster_hierarchical(hc);
    /* Determine a winner for each cluster */
    hist_cluster_get_winners(hc);

    return ADCL_SUCCESS;
}

#if 0
/* Allocate and initialize the clusters data structure */
static int hist_clusters_create ( ADCL_hist_clusters_t **hist_clusters, int clusters_cnt );
/* Count the number of different solutions in the history file */
static int hist_initial_clusters_cnt( ADCL_hist_list_t *hist_list );
/*  */
static int hist_init_cluster( ADCL_hist_clusters_t *hcs, ADCL_hist_t *hist, int pos );
/*  */
static int hist_addto_cluster( ADCL_hist_clusters_t *hcs, ADCL_hist_t *hist, int pos );

/* Allocate and initialize the clusters data structure */
int hist_clusters_create ( ADCL_hist_clusters_t **hist_clusters, int clusters_cnt )
{
    int i;
    ADCL_hist_clusters_t *hcs;
    /* Memory allocation */
    hcs = (ADCL_hist_clusters_t *)calloc(1, sizeof(ADCL_hist_clusters_t) );
    if (NULL == hcs) {
	return ADCL_NO_MEMORY;
    }
    /* Count initialization */
    hcs->hcs_cnt = clusters_cnt;
    /* Memory allocation of the data st for each cluster */
    hcs->hcs_clusters = (ADCL_hist_cluster_t *)calloc(clusters_cnt, sizeof(ADCL_hist_cluster_t) );
    if (NULL == hcs->hcs_clusters) {
	return ADCL_NO_MEMORY;
    }
    /* NxN similarity matrix allocation */
    hcs->hcs_similarity = (double **)calloc(clusters_cnt, sizeof(double *) );
    if (NULL == hcs->hcs_similarity) {
	return ADCL_NO_MEMORY;
    }
    for (i=0; i<clusters_cnt; i++) {
	hcs->hcs_similarity[i] = (double *)calloc(clusters_cnt, sizeof(double) );
    }
    /* Return the according pointer */
    *hist_clusters = hcs;
    return ADCL_SUCCESS;
}

/* Count the number of different solutions in the history */
int hist_initial_clusters_cnt( ADCL_hist_list_t *hist_list )
{
    int i, found, clusters_cnt;
    int *found_solutions;
    ADCL_hist_list_t *hl = hist_list;
    ADCL_hist_t *hist;

    /* Initialization */
    if ( NULL != hl ) {
        hist = hl->hl_curr;
        found_solutions = (int *)calloc(hist->h_fsnum, sizeof(int));
        found_solutions[0] = hist->h_wfnum;
        clusters_cnt = 1;
    }
    else {
        return 0;
    }
    /* Get next history entry handle */
    hl = hl->hl_next;
    hist = hl->hl_curr;
    while ( NULL != hist ) {
	found = 0;
	for(i=0; i<clusters_cnt; i++) {
	    if(hist->h_wfnum == found_solutions[i]) {
		found = 1;
		break;
	    }
	}
	if(!found) {
	    found_solutions[clusters_cnt] = hist->h_wfnum;
	    clusters_cnt++;
	}
	hl = hl->hl_next;
	hist = hl->hl_curr;
    }

#ifdef HC_V
    printf("%d elementary clusters found\n", clusters_cnt);
    for(i=0; i<clusters_cnt; i++) printf("sol%d: %d\n",i,found_solutions[i]);
#endif

    free(found_solutions);
    /* return solution */
    return clusters_cnt;
}

static int hist_init_cluster( ADCL_hist_clusters_t *hcs, ADCL_hist_t *hist, int pos )
{

    hcs->hcs_clusters[pos].hc_cnt = 1;
    hcs->hcs_clusters[pos].hc_func = hist->h_wfnum;
    hcs->hcs_clusters[pos].hc_hists = (ADCL_hist_list_t *)malloc(sizeof(ADCL_hist_list_t));
    hcs->hcs_clusters[pos].hc_hists->hl_curr = hist;
    hcs->hcs_clusters[pos].hc_hists->hl_next = NULL;
    hcs->hcs_clusters[pos].hc_parents[0] = NULL;
    hcs->hcs_clusters[pos].hc_parents[1] = NULL;
}

static int hist_addto_cluster( ADCL_hist_clusters_t *hcs, ADCL_hist_t *hist, int pos )
{
    ADCL_hist_list_t *hl;
    hcs->hcs_clusters[pos].hc_cnt++;
    hl = hcs->hcs_clusters[pos].hc_hists;
    while (NULL != hl) {
	hl = hl->hl_next;
    }
    hl = (ADCL_hist_list_t *)malloc(sizeof(ADCL_hist_list_t));
    hcs->hcs_clusters[pos].hc_hists->hl_curr = hist;
    hcs->hcs_clusters[pos].hc_hists->hl_next = NULL;

}

/* Populate the elementary clusters data structures with the according history data */
int hist_initialize_clusters( ADCL_hist_list_t *hist_list, ADCL_hist_clusters_t *hcs)
{
    int i, found, cnt = 0;
    ADCL_hist_list_t *hl = hist_list;
    ADCL_hist_t *hist;

    /* Initialize the first cluster with the first entry */
    hist = hl->hl_curr;
    /* Initialize the first cluster with the first hist entry */
    hist_init_cluster( hcs, hist, 0 );
    cnt = 1;

    /* Go to the next history entry */
    hl = hl->hl_next;
    hist = hl->hl_curr;
    while ( NULL != hist ) {
	found = 0;
	for(i=0; i<cnt; i++) {
	    if(hist->h_wfnum == hcs->hcs_clusters[i].hc_func) {
		/* We add the history entry to that cluster */
		hist_addto_cluster( hcs, hist, i );
		found = 1;
		break;
	    }
	}

	if(!found) {
            /* Go to the next cluster and initialize it with this entry */
	    hist_init_cluster( hcs, hist, cnt );
            /* Increment the current clusters cnt */
	    cnt++;
	}

        /* Go to the next history entry */
	hl = hl->hl_next;
	hist = hl->hl_curr;
    }

#ifdef HC_V
    printf("%d created, %d expected\n", cnt, hcs->hcs_cnt);
#endif

    /* return solution */
    return cnt;
}

/* Define the distance measure to be used */
double hist_distance( ADCL_hist_t *hist1, ADCL_hist_t *hist2 )
{
    return 200*abs(hist1->h_perf[hist1->h_wfnum]-hist1->h_perf[hist2->h_wfnum])/
                  (hist1->h_perf[hist1->h_wfnum]+hist1->h_perf[hist2->h_wfnum]);
}

/* Distance measure between two clusters */
double hist_cluster_distance( ADCL_hist_cluster_t *hc1, ADCL_hist_cluster_t *hc2 )
{
    int i, j;
    double distance;
    ADCL_hist_list_t *hl1, *hl2;
    distance = 0;

    hl1 = hc1->hc_hists;
    hl2 = hc2->hc_hists;
    /* single linkage */
    /* max-complete linkage */

    /* average linkage UPGMA */
    while( NULL != hl1 ) {
        while( NULL != hl2 ) {
            distance += hist_distance( hl1->hl_curr, hl2->hl_curr );
            hl2 = hl2->hl_next;
        }
        hl2 = hc2->hc_hists;
        hl1 = hl1->hl_next;
    }
    distance = distance /(hc1->hc_cnt * hc2->hc_cnt);

    /* centroid linkage */



    return distance;
}

void compute_similarity_matrix( ADCL_hist_clusters_t *hcs )
{
    int i, j;
    for(i=0; i<hcs->hcs_cnt; i++) {
        for(j=0; j<hcs->hcs_cnt; j++) {
            hcs->hcs_similarity[i][j] = hist_cluster_distance(&hcs->hcs_clusters[i],
                                                              &hcs->hcs_clusters[j]);
            printf("matrix[%d][%d] = %f \n", i, j, hcs->hcs_similarity[i][j]);
        }
    }
    return;
}

int hist_cluster_old ( ADCL_emethod_t *e )
{
   
    ADCL_hist_list_t *hl;
    ADCL_hist_clusters_t *hcs;
    int clusters_cnt;
    /* Initialization */
    hl = e->em_hist_list;
    hcs = e->em_hist_clusters;

    /* Get the initial clusters count */
    clusters_cnt = hist_initial_clusters_cnt( hl );
    /* Allocate and initialize the clusters data structure */
    hist_clusters_create ( &hcs, clusters_cnt );
    /* Initialize the elementary clusters data structures */
    hist_initialize_clusters( hl, hcs );
    /* Compute the distance matrix */
    compute_similarity_matrix( hcs );


    return ADCL_SUCCESS;
}

#endif
