/***********    ECA_print
 *
 *	This is a c header file for Eric Anderson's 
 *	"ECA_print" library which includes some simple 
 *	printf-like functions that take an extra argument
 *	that says whether the print statement should be
 *	executed or not.  This is for convenient 
 *	verbosity control via bit-masking
 *
 ************/




int eprintf(unsigned long int expr, const char *fmt, ... );
int efprintf(unsigned long int expr, FILE *s, const char *fmt, ... );
int esprintf(unsigned long int expr, char *p, const char *fmt, ... );