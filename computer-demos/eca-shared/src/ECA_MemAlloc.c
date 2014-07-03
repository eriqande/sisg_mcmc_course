/***********    ECA_MemAllocs
*
*	This is a c source code file to make Eric Anderson's 
*	"ECA_MemAllocs" library which includes routines for allocating
*	memory.  Nothing really fancy.  Just some functions and
*	macros that make it so that the program will exit if a ECA_MALLOC 
*	or ECA_CALLOC call fails, and a couple of global variables that
*	keep track of how much memory the program has requested (but
*	it does not keep track of how much it has freed!)
*
*	
*
************/

#include <stdio.h>
#include <stdlib.h>
#include "ECA_MemAlloc.h"


void *eca_malloc(size_t bytes, char *VarName)
{
	void *temp;
	
	if(bytes==0)
		return NULL;
	
	if( (temp = malloc(bytes) ) == NULL) {

		printf("\n\nMEMORY ALLOCATION FAILURE:");
		printf("\nAttempted function: malloc(%s)", VarName);
		printf("\n%s = %d bytes", VarName,(int)bytes);
		
		printf("\n\nExiting to system...\n\n");
		
		exit(1);
	}
	
	
	return(temp);
}



void *eca_calloc(size_t Num, size_t bytes, char *NumName, char *VarName)
{
	void *temp;
	
	if(Num*bytes==0) 
		return(NULL);
	if( (temp = calloc(Num, bytes)) == NULL) {

		printf("\n\nMEMORY ALLOCATION FAILURE:");
		printf("\nAttempted function: calloc(%s,%s)", NumName, VarName);
		
		printf("\n%s = %d Elements", NumName,(int)Num);
		printf("\n%s = %d bytes", VarName,(int)bytes);
				
		printf("\n\nExiting to system...\n\n");
		
		exit(1);
	}
		return(temp);
}




