#include "ATF.h"

char ComPatternText[100];

int ATF_Change_init(int npattern)
{

  memset(ComPatternText, 0, 100);

  switch(npattern){
  
    case(patt_fcfs):
      
      strcpy(ComPatternText, "Communication pattern: fcfs");
      break;
   
    default:
      
      return ATF_ERROR;
      break;
      
  }
  
  return ATF_SUCCESS;
} 

