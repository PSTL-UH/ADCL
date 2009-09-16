/*
 * Copyright (c) 2006-2007      University of Houston. All rights reserved.
 * Copyright (c) 2009           HLRS. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include "ADCL.h"
#include "ADCL_internal.h"

#ifndef PATH_MAX
#define PATH_MAX 80
#endif

#define MAXLINE 80

extern int ADCL_OUTLIER_FACTOR;
extern int ADCL_OUTLIER_FRACTION;
extern int ADCL_statistic_method;
extern int ADCL_merge_requests;
extern int ADCL_emethod_selection;
extern int ADCL_emethod_allgatherv_selection;
extern int ADCL_emethod_allreduce_selection;
extern int ADCL_emethod_alltoall_selection;
extern int ADCL_emethod_alltoallv_selection;
extern int ADCL_emethod_numtests;
extern int ADCL_printf_silence;
extern int ADCL_emethod_use_perfhypothesis;
extern int ADCL_emethod_learn_from_hist;
extern int ADCL_method_total_num;

extern char ADCL_display_ip[];
extern int ADCL_display_port;
extern int ADCL_display_rank;
extern int ADCL_display_flag;
//extern ADCL_method_t * ADCL_method_array;

static int ADCL_is_keyword (char* keyword, char* text );
static int ADCL_set_keyword_int_value ( char* strptr, char* keyword, int* value ); 
static int ADCL_set_keyword_string_value ( char* strptr, char* keyword, char* value );  
static int ADCL_set_predefined_function( char* strptr, char* keyword, int fs_maxnum, 
    ADCL_function_t** fs_fptrs, int* selectvar );

#define Debug 1

int ADCL_readenv()
{
    int result;
    int len;
    int i;
    char *HomeDir;
    char FilePath[PATH_MAX];
    char buffer[MAXLINE];
    char keyword[MAXLINE];
    char valstring[MAXLINE];
    char *ptr;

    FILE* fp;

    /* New strategy; check first for a configuration
    ** file in the current working directory. If it is not
    ** found there, checkl ${HOME}/.adcl/
    */

    fp = fopen ("config.adcl", "r");
    if ( NULL == fp ) {
        HomeDir = getenv("HOME");
        if (HomeDir == NULL) {
            ADCL_printf("Environment variable HOME not found\n");
            return ADCL_NOT_FOUND;
        }

        memset ( FilePath, 0, PATH_MAX );
        result = snprintf(FilePath, PATH_MAX, "%s/.adcl/config.adcl", HomeDir);

        if (result >= PATH_MAX) {
            ADCL_printf("Warning: path \"%s\" too long. Truncated.\n", FilePath);
        }

        fp = fopen(FilePath, "r");
        if (fp == NULL) {
            ADCL_printf("Could not open %s\n", FilePath);
            return ADCL_NOT_FOUND;
        }

#ifdef Debug
        ADCL_printf("#Outputting contents of %s:\n", FilePath);
#endif
    }

    /*Read lines from configure file*/
    while (fgets(buffer, MAXLINE, fp) != NULL) {

        /* check blank line or # comment */
        if( buffer[0] == '#' ){
            continue;
        }
        len = strlen(buffer);

        /* Parse one single line! */
        i = (int) strcspn (buffer, ":");
        if ( i == len ) {
            continue; /* No colon found */
        }
        /* Reinitialize the buffer keword each time */
        memset( keyword, 0x00, MAXLINE );
        strncpy ( keyword, buffer, i);
        ptr = &(buffer[++i]);
#ifdef Debug
        ADCL_printf("#Keyword is %s\n",keyword);
#endif

        /*  ADCL_OUTLIER_FACTOR; */
        if ( ADCL_is_keyword(keyword, "ADCL_OUTLIER_FACTOR") ) {
            ADCL_set_keyword_int_value ( ptr, "ADCL_OUTLIER_FACTOR", &ADCL_OUTLIER_FACTOR );
        }
        /*  ADCL_OUTLIER_FRACTION */
        else if( ADCL_is_keyword(keyword, "ADCL_OUTLIER_FRACTION") ) {
            ADCL_set_keyword_int_value ( ptr, "ADCL_OUTLIER_FRACTION", &ADCL_OUTLIER_FRACTION );
        }
        /*  ADCL_STATISTIC_METHOD */
        else if ( ADCL_is_keyword(keyword, "ADCL_STATISTIC_METHOD") ) {
            ADCL_set_keyword_string_value ( ptr, "ADCL_STATISTIC_METHOD", valstring ); 
            if ( ADCL_is_keyword(valstring,"ADCL_STATISTIC_MAX") ) {
                ADCL_statistic_method = ADCL_STATISTIC_MAX;
            }
        }
        /*  ADCL_MERGE_REQUESTS  */
        else if ( ADCL_is_keyword(keyword, "ADCL_MERGE_REQUESTS") ) {
            ADCL_set_keyword_int_value ( ptr, "ADCL_MERGE_REQUESTS", &ADCL_merge_requests);
        }
        /*  ADCL_EMETHOD_NUMTESTS  */
        else if (  ADCL_is_keyword(keyword, "ADCL_EMETHOD_NUMTESTS") ) {
            ADCL_set_keyword_int_value ( ptr,"ADCL_EMETHOD_NUMTESTS", &ADCL_emethod_numtests);
        }
        /*  ADCL_EMETHOD_SELECTION */
        else if ( ADCL_is_keyword(keyword,"ADCL_EMETHOD_SELECTION") ) {
            ADCL_set_predefined_function( ptr, "ADCL_EMETHOD_SELECTION", ADCL_neighborhood_fnctset->fs_maxnum,
                    ADCL_neighborhood_fnctset->fs_fptrs, &ADCL_emethod_selection );
        }
        /*  ADCL_EMETHOD_ALLGATHERV_SELECTION */
        else if ( ADCL_is_keyword(keyword,"ADCL_EMETHOD_ALLGATHERV_SELECTION") ) {
            ADCL_set_predefined_function( ptr, "ADCL_EMETHOD_ALLGATHERV_SELECTION", ADCL_allgatherv_fnctset->fs_maxnum,
                    ADCL_allgatherv_fnctset->fs_fptrs, &ADCL_emethod_allgatherv_selection );
        }
        /*  ADCL_EMETHOD_ALLREDUCE_SELECTION */
        else if ( ADCL_is_keyword(keyword,"ADCL_EMETHOD_ALLREDUCE_SELECTION") ) {
            ADCL_set_predefined_function( ptr, "ADCL_EMETHOD_ALLREDUCE_SELECTION", ADCL_allreduce_fnctset->fs_maxnum,
                    ADCL_allreduce_fnctset->fs_fptrs, &ADCL_emethod_allreduce_selection );
        }
        /*  ADCL_EMETHOD_ALLTOALL_SELECTION */
        else if ( ADCL_is_keyword(keyword,"ADCL_EMETHOD_ALLTOALL_SELECTION") ) {
            ADCL_set_predefined_function( ptr, "ADCL_EMETHOD_ALLTOALL_SELECTION", ADCL_alltoall_fnctset->fs_maxnum,
                    ADCL_alltoall_fnctset->fs_fptrs, &ADCL_emethod_alltoall_selection );
        }
        /*  ADCL_EMETHOD_ALLTOALLV_SELECTION */
        else if ( ADCL_is_keyword(keyword,"ADCL_EMETHOD_ALLTOALLV_SELECTION") ) {
            ADCL_set_predefined_function( ptr, "ADCL_EMETHOD_ALLTOALLV_SELECTION", ADCL_alltoallv_fnctset->fs_maxnum,
                    ADCL_alltoallv_fnctset->fs_fptrs, &ADCL_emethod_alltoallv_selection );
        }
        /*  ADCL_PRINTF_SILENCE  */
        else if ( ADCL_is_keyword(keyword, "ADCL_PRINTF_SILENCE") ) {
            ADCL_set_keyword_int_value ( ptr, "ADCL_PRINTF_SILENCE", &ADCL_printf_silence); 
        }
        /*  ADCL_EMETHOD_USE_PERFHYPOTHESIS  */
        else if ( ADCL_is_keyword(keyword, "ADCL_EMETHOD_USE_PERFHYPOTHESIS") ) {
            ADCL_set_keyword_int_value ( ptr, "ADCL_EMETHOD_USE_PERFHYPOTHESIS", &ADCL_emethod_use_perfhypothesis);
        }
        /*  ADCL_HIST_LEARNING */
        else if ( ADCL_is_keyword(keyword, "ADCL_HIST_LEARNING") ) {
            ADCL_set_keyword_int_value ( ptr,"ADCL_HIST_LEARNING", &ADCL_emethod_learn_from_hist );
        }
        /* ADCL_DISPLAY_IP */
        else if ( ADCL_is_keyword(keyword, "ADCL_DISPLAY_IP") ) {
            ADCL_display_flag = 1;
            ADCL_set_keyword_string_value ( ptr,"ADCL_DISPLAY_IP", ADCL_display_ip );
        }
        /* ADCL_DISPLAY_PORT */
        else if ( ADCL_is_keyword(keyword, "ADCL_DISPLAY_PORT") ) {
            ADCL_set_keyword_int_value ( ptr, "ADCL_DISPLAY_PORT", &ADCL_display_port );
        }
        /* ADCL_DISPLAY_RANK */
        else if ( ADCL_is_keyword(keyword, "ADCL_DISPLAY_RANK") ) {
            ADCL_set_keyword_int_value ( ptr,"ADCL_DISPLAY_RANK", &ADCL_display_rank );
        }
    }
    return ADCL_SUCCESS;
}

/*************************************************************************************************/
int ADCL_is_keyword (char* keyword, char* text )
/*************************************************************************************************/
{
    if ( strncmp(keyword, text, strlen(text)) == 0) {
        return 1;
    }
    else {
        return 0;
    }
}

/*************************************************************************************************/
int ADCL_set_keyword_int_value ( char* strptr, char* keyword, int* value ) 
/*************************************************************************************************/
{
    sscanf(strptr, "%d", value);
#ifdef  Debug
    ADCL_printf("#%s: %d\n", keyword, *value); 
#endif
    return ADCL_SUCCESS; 
}


/*************************************************************************************************/
int ADCL_set_keyword_string_value ( char* strptr, char* keyword, char* value ) 
/*************************************************************************************************/
{
    sscanf(strptr,"%s", value);
#ifdef  Debug
    ADCL_printf("#%s: %s\n", keyword, value);
#endif
}

/*************************************************************************************************/
int ADCL_set_predefined_function( char* strptr, char* keyword, int fs_maxnum, 
        ADCL_function_t** fs_fptrs, int* selectvar )
/*************************************************************************************************/
{
    char valstring[MAXLINE];
    int i, found=0;

    ADCL_set_keyword_string_value ( strptr, keyword, valstring ); 

    for ( i=0; i<fs_maxnum; i++ ) {
        if ( strcmp(valstring, fs_fptrs[i]->f_name) == 0) {
            found = 1; 
            *selectvar = i;
            break;
        }
    }
    if ( !found ) {
        printf("Warning: function %s does not exist in function set %s\n", valstring, keyword); 
    }

    return ADCL_SUCCESS;
}
