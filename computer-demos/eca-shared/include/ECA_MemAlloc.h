/***********    ECA_MemAllocs
*
*	This is a header file for Eric Anderson's 
*	"ECA_MemAllocs" library which includes routines for allocating
*	memory.  Nothing really fancy.  Just some functions and
*	macros that make it so that the program will exit if a malloc 
*	or calloc call fails, and a couple of global variables that
*	keep track of how much memory the program has requested (but
*	it does not keep track of how much it has freed!)
*
*	
*
************/

#ifndef __ECA_MEM_ALLOC_FLAG__
#define __ECA_MEM_ALLOC_FLAG__

#ifdef UN_EXTERN
	#define GLOB
#else 
	#define GLOB extern
#endif



/*  some macros */
#define ECA_MA_SET_FLAG 11773366  /*  just a weird number that nothing would normally */
								  /*  be set to. */

/*  these are what we actually use to call a memory allocating function */
/*  as the shape of the macro is the same as malloc or calloc respectively */
#define ECA_MALLOC(X)   	eca_malloc(X,#X)
#define ECA_CALLOC(X,Y)		eca_calloc(X,Y,#X,#Y)


/*  prototypes */
extern void *eca_malloc(size_t bytes, char *VarName);
extern void *eca_calloc(size_t Num, size_t bytes, char *NumName, char *VarName);


#endif





