#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAXLINE 120

int main (int argc, char **argv )
{

  FILE *infd=NULL,  **outfd=NULL;
  int numrequests=-1;

  int reqid, i;
  char inname[50], outname[50];
  char line[MAXLINE];
  char *basestr, reqstr[50];

  if (argc < 3 )
    {
      printf(" Usage : split <infilename> <numrequests> \n\n");
      printf(" This program splits a single file given by \n"
	     " <infilename> into multiple output files per request.\n");
      exit ( 1 ) ;
    }

  strcpy ( inname, argv[1] );
  numrequests = atoi ( argv[2] );
  
  infd = fopen ( inname, "r" );
  outfd = (FILE **) malloc ( numrequests * sizeof(FILE *) );
  if ( NULL == outfd ) {
      exit (-1);
  }
  
  for (i=0; i < numrequests; i++ ) {
      sprintf(outname, "%s.%d", inname, i );
      outfd[i] = fopen ( outname, "w" );
      if ( NULL == outfd[i] ) {
	  exit (-1);
      }
  }

  /* Read infile and store the values in the according list */
  while ( fscanf ( infd, "%[^\n]\n", line ) != EOF ) 
    {
      if ( line[0] == '#' ) {
	  /* Skip comment lines */
	  continue;
      }
      if ( line[0] == 'A' ) {
	  /* skip the lines having the Assigning stament in it */
	  continue;
      }

      /*read the parameters of the run */
      basestr = strstr ( line, "request" );
      sscanf ( basestr, "%7s %d", reqstr, &reqid );

      fprintf ( outfd[reqid], "%s\n", line );
    }

  for ( i=0; i< numrequests; i++ ) {
      fclose ( outfd[i]);
  }

  free ( outfd );


  return ( 0 );
}
