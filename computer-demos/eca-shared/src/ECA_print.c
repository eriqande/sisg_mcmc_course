/***********    ECA_print
 *
 *	This is a c source code file for Eric Anderson's 
 *	"ECA_print" library which includes some simple 
 *	printf-like functions that take an extra argument
 *	that says whether the print statement should be
 *	executed or not.  This is for convenient 
 *	verbosity control via bit-masking
 *
 ************/



#include <stdio.h>
#include <stdarg.h>
#include "ECA_print.h"




/* simple printf function that prints if expr evaluates to true */
int eprintf(unsigned long int expr, const char *fmt, ... ) 
{
	va_list ap;
	int x=0;
	
	if(expr) {
		va_start(ap,fmt);
		x=vprintf(fmt,ap);
		va_end(ap);
	}
	return(x);
}



/* simple fprintf function that prints if expr evaluates to true */
int efprintf(unsigned long int expr, FILE *s, const char *fmt, ... ) 
{
	va_list ap;
	int x=0;
	
	if(expr) {
		va_start(ap,fmt);
		x=vfprintf(s,fmt,ap);
		va_end(ap);
	}
	return(x);
}




/* simple sprintf function that prints if expr evaluates to true */
int esprintf(unsigned long int expr, char *p, const char *fmt, ... ) 
{
	va_list ap;
	int x=0;
	
	if(expr) {
		va_start(ap,fmt);
		x=vsprintf(p,fmt,ap);
		va_end(ap);
	}
	return(x);
}
