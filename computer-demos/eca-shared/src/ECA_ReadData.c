#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ECA_ReadData.h"




/*  test to see if the next character (apart from those that return 1 from function */
/*  Ignore) is a \n (line terminator)  if it is, then eat it and return 1.  If it  */
/*  isn't, then put that character back onto the stream and return 0. */
int erdEatLineEnd(int Ignore(char), FILE *in)
{
	int c;
	int Status = 0;
	
	while(Ignore((char)(c=getc(in)) ) && c != EOF)  /*  eat through any extra spaces, etc, as defined in TermFunc */
		;
	
	/*  now, c contains the first non-ignored character */
	if(c==(int)'\n') {  /*  if c is a line ending, then we have eaten it and we return 1 */
		Status = 1;
	}
	else {  /*  if it is not a line ending, then we put it back on the input stream and return 0 */
		ungetc(c,in);
	}
	return(Status);
}


/*  get the next string (not longer than length) from the input stream File. */
/*  Strings are separated */
/*  by the things that return 1 from TermFunc */
int erdKeyString(char *p, int TermFunc(char), int length, FILE *File)
{
	int c;
	char *s;
	if(length < 1)
		return 1;  /*  return a 1 if the call is erroneous in some way */
	s = p + length - 1;
	
	while(TermFunc ((char)(c=getc(File)) ) && c != EOF)  /*  eat through any extra spaces, etc, as defined in TermFunc */
		;
	ungetc(c,File);	/*  return the first non-terminator to the input stream */
	while(!TermFunc((char)(c=getc(File))) &&  c != EOF) 
		if(p < s) 
			*p++ = (char)c;
			
	/*  here we are going to put the first terminator back onto the stream.  It will get eaten by the */
	/*  next call to erdKeyString, BUT, we might want to have it there to test to see if it was an end of */
	/* line or something  */
	ungetc(c,File);
	*p = '\0';
	return(c==EOF) ? EOF : 0;  
}


/*  get the next string from the input stream.  Strings are separated */
/*  by the things that return 1 from TermFunc.  In this case, though, */
/*  The function returns a 2 when you reach a character for which  */
/*  LineFunc returns a 1 before it hits any substantial character, */
/*  and a 3 when it terminates a string with a LineFunc character */
int erdKeyStringLineBreak(char *p, int TermFunc(char), int LineFunc(char), int length, FILE *File)
{
	int c;
	char *s;
	if(length < 1)
		return 1;  /*  return a 1 if the call is erroneous in some way */
	s = p + length - 1;
	
	while(TermFunc((char)(c=getc(File))) && c != EOF)  /*  eat through any extra spaces, etc, as defined in TermFunc */
		;
		
	if( LineFunc((char)c) )  /*  if the loop above terminated on a "line-ending" character, then it */
		return(2);			 /*  has been eaten, as it should be, and we return 2. */
			
	ungetc(c,File);	/*  return the first non-terminator to the input stream */
	while(!TermFunc((char)(c=getc(File))) &&  c != EOF)  {
		if(p < s) 
			*p++ = (char)c;
		if(LineFunc((char)c)) {   /*  if you just ate the end of line character */
			*(--p) = '\0'; 		/*  terminate the string _over the top of that character_ */
			return(3);
		}
	}
	*p = '\0';
	return(c==EOF) ? EOF : 0;  
}




int erdStdItemSeparators(char t)
{
	return( (t=='\t') + (t==' ') + (t=='\n') );
}

/*  returns 0 if the character is not a space or a tab */
/*  1 if it is a space or a tab.  Returns a zero for newline! */
int erdSingleLineWhiteSeparators(char t)
{
	return( (t=='\t') + (t==' ') );
}

int erdStdLineSeparator(char t)
{
	return( (t=='\n') );
}


int erdIsDigit(char c)
{
	return( c >= '0' && c<= '9' );
}

int erdIsCap(char c)
{
	return( c >= 'A' && c <= 'Z' );
}

int erdIsLetter(char c)
{
	return( erdIsCap(c) || (c >= 'a' && c<= 'z') );
}

int erdGetNext(char *p, double *n, FILE *File)
{
	int Status;
	double Frac = 0;
	char Sign, *r;
	enum {string, integer, decimal, name};
	Status = erdKeyString(p,erdStdItemSeparators,90, File);
	
	if(Status != EOF) {
		Sign = (char)((*p == '+' || *p == '-')   ?   *p   :   0);
		*n = 0.0;
		for(r = Sign ? p+1 : p; erdIsDigit(*r); ++r)
			*n = *n * 10 + (*r - '0');
		if(*r == '.')
			for(++r, Frac = 1; erdIsDigit(*r); ++r)
				*n += (*r - '0') / (Frac *= 10);
		if(Sign == '-')
			*n *= -1.0;
		if(!*r) 
			Status = Frac ? decimal : integer;
		else  {
			if(erdIsLetter(*p) )
				while(erdIsLetter(*p) || erdIsDigit(*p) )
					++p;
			Status = !*p ? name : string;
		
		}
	}
	return(Status);
}


/*  returns the next non-whitespace character */
int erdGetNextChar(char *p, double *n, FILE *File)
{
	int Status;
	double Frac = 0;
	char Sign, *r;
	enum {string, integer, decimal, name};
	Status = erdKeyString(p,erdStdItemSeparators,2, File);
	
	if(Status != EOF) {
		Sign = (char)((*p == '+' || *p == '-')   ?   *p   :   0);
		*n = 0.0;
		for(r = Sign ? p+1 : p; erdIsDigit(*r); ++r)
			*n = *n * 10 + (*r - '0');
		if(*r == '.')
			for(++r, Frac = 1; erdIsDigit(*r); ++r)
				*n += (*r - '0') / (Frac *= 10);
		if(Sign == '-')
			*n *= -1.0;
		if(!*r) 
			Status = Frac ? decimal : integer;
		else  {
			if(erdIsLetter(*p) )
				while(erdIsLetter(*p) || erdIsDigit(*p) )
					++p;
			Status = !*p ? name : string;
		
		}
	}
	return(Status);
}



/*  like GetNext, but this reads characters separated by TermFunc values that return 1 */
/*  and it returns EndLineStatus == 1 if it just hit a LineEnding character without  */
/*  reading any other values, and a 2 if it has a value and the line ends after that */
/*  value.  */
int erdGetNextOnLine(char *p, double *n, FILE *File, int TermFunc(char), int LineFunc(char), int *EndLineStatus)
{
	int Status;
	double Frac = 0;
	char Sign, *r;
	enum {string, integer, decimal, name, nothing};
	Status = erdKeyStringLineBreak(p,TermFunc, LineFunc,900, File);
	
	*EndLineStatus = 0;  /*  default setting is that there was no line ending of any sort */
	
	if(Status == 2)  {  /*  go straightaway letting them know you got nothing new */
		*EndLineStatus = 1;
		return(nothing);  
	}
	if(Status == 3)  {
		*EndLineStatus = 2;
	}
	
	if(Status != EOF) {
		Sign = (char)((*p == '+' || *p == '-')   ?   *p   :   0);
		*n = 0.0;
		for(r = Sign ? p+1 : p; erdIsDigit(*r); ++r)
			*n = *n * 10 + (*r - '0');
		if(*r == '.')
			for(++r, Frac = 1; erdIsDigit(*r); ++r)
				*n += (*r - '0') / (Frac *= 10);
		if(Sign == '-')
			*n *= -1.0;
		if(!*r) 
			Status = Frac ? decimal : integer;
		else  {
			if(erdIsLetter(*p) )
				while(erdIsLetter(*p) || erdIsDigit(*p) )
					++p;
			Status = !(*p) ? name : string;
		
		}
	}
	return(Status);
}


/*  tries to open file FileName. */
/*  in mode How (i.e. "w" or "r" or "a") */
/*  If it fails it asks for a new name */
/*  and it uses that name */
FILE *erdOpenFileOrRetry(char *FileName, char *How)
{
	FILE *temp;
	char F[300];
	
	sprintf(F,"%s",FileName);
	
	while( !(temp = fopen(F, How) ) ) {
		printf("\n\nI'm terribly sorry.  I can't open file \"%s\"\n",F);
		printf("\nPlease enter a new file name, or x or X to quit\n->");
		scanf("%s",F);
		if( (strcmp(F,"X") == 0) || (strcmp(F,"x")==0) )
			exit(1); 
	}
	
	return(temp);
}


