/*
 * Copyright (c) 2006-2007      University of Houston. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#ifndef __ADCL_HIST_INTERNAL_H__
#define __ADCL_HIST_INTERNAL_H__

#ifdef ADCL_DUMMY_MPI
#include "ADCL_dummy_mpi.h"
#else
#include "mpi.h"
#endif
#include "cluster.h"
#include "svm.h"

/* Number of clusters */
#define NB_OF_CLUSTERS                  2

/* History entries list data st */
struct ADCL_hist_list_s{
    ADCL_hist_t             *hl_curr;
    struct ADCL_hist_list_s *hl_next;
};
typedef struct ADCL_hist_list_s ADCL_hist_list_t;

/* Structure holding the history functions and related information */
struct ADCL_hist_functions_s{
    ADCL_hist_reader             *hf_reader; /* Hist reading function */
    ADCL_hist_writer             *hf_writer; /* Hist writing function */
    ADCL_hist_filter             *hf_filter; /* Filter function according to the given criteria */
    ADCL_hist_distance         *hf_distance; /* Distance function between two Hist */
};
typedef struct ADCL_hist_functions_s ADCL_hist_functions_t;

/* Structure holding the criteria and function to set them */
struct ADCL_hist_criteria_s{

    void                *hc_filter_criteria; /* Filter criteria structure */
    ADCL_hist_set_criteria *hc_set_criteria; /* Function setting the filter criteria structure */
    int                     hc_criteria_set; /* Flag whether the filtering criteria are set */
};
typedef struct ADCL_hist_criteria_s ADCL_hist_criteria_t;

/* Data structure for clustering */
struct ADCL_hist_clusters_s{
    int hc_nrows; /* Equal to the number of hist entries */
    int hc_ncolumns; /* Equal to the number of functions in function-set */
    double **hc_data; /* Performance data of size row X columns */
    int **hc_mask;
    double *hc_weight;
    int hc_transpose;
    char hc_dist;
    char hc_method;
    Node *hc_tree;
    int *hc_ids;
    int hc_winners[NB_OF_CLUSTERS];
};
typedef struct ADCL_hist_clusters_s ADCL_hist_clusters_t;

/* Data structure for prediction with SVMs */
struct ADCL_hist_svm_s{
    double *hs_feature_max;
    double *hs_feature_min;
    struct svm_parameter hs_param;
    struct svm_problem hs_prob;
    struct svm_node *hs_x_space;
    struct svm_model *hs_model;
    int hs_max_index;
};
typedef struct ADCL_hist_svm_s ADCL_hist_svm_t;

/* Data structure for prediction with NBC */
struct ADCL_hist_nbc_s{
 
    int hn_histnum;  /* Number of data entries in history */
    int hn_fnum;     /* Number of features */
    int hn_binnum;   /* Number of bins */
    int hn_binsize;  /* Size of a bin */
    int hn_binstart; /* Min value in pb sizes */
    int hn_binend;   /* Max value in pb sizes */
    int hn_classnum; /* Number of classes */
    int **hn_p2train;/* */
    int **hn_p2x;    /* */
    int *hn_p2t;     /* */
    double *hn_pc;   /* */

};
typedef struct ADCL_hist_nbc_s ADCL_hist_nbc_t;


/* Prediction algorithms from neighbors within dmax */
int hist_learn_from_neighbors( ADCL_emethod_t *e );
/* Prediction algorithm using a Naive bayes classifier */
int hist_learn_with_nbc( ADCL_emethod_t *e );
/* Prediction algorithm using a Support Vector Machine classifier */
int hist_learn_with_svm( ADCL_emethod_t *e );

void hist_svm_free();

#endif /* __ADCL_HIST_INTERNAL_H__ */
