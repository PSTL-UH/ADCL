#include "ADCL.h"
#include "ADCL_internal.h"


#define PATH_MAX 40
#define MAXLINE 40

extern int ADCL_OUTLITER_FACTOR;
extern int ADCL_OUTLITER_FRACTION;

int ADCL_read_env()
{
    int result;
    int len;
    int i;
    int flag;

    int ADCL_OUTLITER_FACTOR;
    int ADCL_OUTLITER_FRACTION;

    char *HomeDir;
    char FilePath[PATH_MAX];
    char buffer[MAXLINE];
    char temp[MAXLINE];
    char keyword[MAXLINE];
    char *ptr;

    FILE* fp;

    HomeDir = getenv("HOME");

    if (HomeDir == NULL) {
	   ADCL_printf("Environment variable HOME not found\n");
	   exit(1);
    }
    result = snprintf(FilePath, PATH_MAX, "%s/.adcl/config.adcl", HomeDir);

	if (result >= PATH_MAX) {
	   ADCL_printf("Warning: path \"%s\" too long. Truncated.\n", FilePath);
    }

    fp = fopen(FilePath, "r");

	if (fp == NULL) {
	   ADCL_printf("Could not open %s\n", FilePath);
	   exit(1);
    }

#ifdef Debug
    ADCL_printf("Outputting contents of %s:\n\n", FilePath);
#endif 

	/*Read lines from configure file*/
	while (fgets(buffer, MAXLINE, fp) != NULL) {

		/* check blank line or # comment */
	    if( buffer[0] != '#' ){
		
			/* Parse one single line! */
			i = 0;
			flag =0;
			len = strlen(buffer);

			while(i<len) {
		    	temp[i]= buffer[i];
		    	if(buffer[i] == ':'){
					strncpy(keyword, temp, i);
#ifdef Debug
					ADCL_printf("Keyword is %s\n",keyword);
#endif
					i++;
					flag =1;	/* this means this line has keywords; */
					ptr = &buffer[i];
					break;	
		    }
		    /* 	else{ */
		    /* 		printf("This is a NULL line!\n"); */
		    /* 	} */
		    i++;
			}
			
			/*  If this is a keyworkd...... */
			if(flag ==1){
				/*  ADCL_OUTLIER_FACTOR; */
				if(strncmp(keyword, "ADCL_OUTLIER_FACTOR", strlen("ADCL_OUTLIER_FACTOR")) == 0){
				
				sscanf(ptr, "%d", &ADCL_OUTLITER_FACTOR);
#ifndef	Debug	
				ADCL_printf("ADCL_OUTLITER_FACTOR : %d\n", ADCL_OUTLITER_FACTOR);	
#endif
				ptr = NULL;
	
				}
	
				if(strncmp(keyword, "ADCL_OUTLIER_FRACTION", strlen("ADCL_OUTLIER_FRACTION")) == 0){
				
				sscanf(ptr, "%d", &ADCL_OUTLITER_FRACTION);
#ifndef	Debug	
				ADCL_printf("ADCL_OUTLITER_FRACTION : %d\n", ADCL_OUTLITER_FRACTION);	
#endif
				ptr = NULL;
	
				}
				
	
			}
		}
	}

	return ADCL_SUCCESS;
}
