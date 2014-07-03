#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "ECA_Opt.h"


/* returns 1 if s makes sense as a good int and 0 otherwise.  To test for it
being a good int, it checks to see if the first character is either a "-" or a non-zero
digit.  Then, all remaining characters must be digits.  */
int string_is_good_int(const char *s)
{
	int i, result = 1;
	
	if(isdigit(s[0])==0 && s[0] != '-' ) {
		return(0);
	}
	
	for(i=1;s[i]!='\0';i++) {
		result *= isdigit(s[i]);
	}
	
	return(result);
}


void print_indented_and_max_length(const char *str, int len)
{
	int notfirsttime = 0;
	int slen = strlen(str);
	int lo=0, hi=0;

	while(hi<slen) {
		hi = lo+len;

		/* if we are at the end of the string, deal with that */
		if(hi>=slen) {
			printf("        ");
			for(lo+=notfirsttime;lo<slen;lo++)
				printf("%c",str[lo]);
			printf("\n");
			return;
		}

		/* otherwise, work backward from hi, till you hit some white space */
		while( !(isspace(str[hi])) && hi>lo)  hi--;
		if(hi==lo) {
			/* in this case, we have to go forward till we hit white space */
			hi = lo + len;
			while( !(isspace(str[hi])) && hi<slen)  hi++;
		}

		printf("        ");
		for(lo+=notfirsttime;lo<hi;lo++)
			printf("%c",str[lo]);
		printf("\n");

		notfirsttime = 1;
	}
}



/* this prints stuff out indented by Num tabs */
void print_spec_indented_and_max_length(const char *str, int len, int Num)
{
	int notfirsttime = 0;
	int slen = strlen(str);
	int lo=0, hi=0;

	while(hi<slen) {
		hi = lo+len;

		/* if we are at the end of the string, deal with that */
		if(hi>=slen) {
			printf("        ");
			for(lo+=notfirsttime;lo<slen;lo++)
				printf("%c",str[lo]);
			printf("\n");
			return;
		}

		/* otherwise, work backward from hi, till you hit some white space */
		while( !(isspace(str[hi])) && hi>lo)  hi--;
		if(hi==lo) {
			/* in this case, we have to go forward till we hit white space */
			hi = lo + len;
			while( !(isspace(str[hi])) && hi<slen)  hi++;
		}

		printf("        ");
		for(lo+=notfirsttime;lo<hi;lo++)
			printf("%c",str[lo]);
		printf("\n");

		notfirsttime = 1;
	}
}



void eca_opt_print_usage_long(const char *shortopt,const char *longopt,const char *args,const char *comment, int __END_OF_OPTS)
{
	int i;
	char tempStr[1000];
	
	/* first get the options string made */
	if(strcmp(shortopt,"-") != 0 && strcmp(longopt,"--") != 0 ) {
		sprintf(tempStr,"%s , %s ",shortopt,longopt);
	}
	else if (strcmp(shortopt,"-") == 0) {
		sprintf(tempStr,"     %s ",longopt);
	}
	else if( strcmp(longopt,"--")==0) {
		sprintf(tempStr,"%s ",shortopt);
	}
	
	/* then print the options string and pad with space as necessary */
	printf("%s",tempStr);
	for(i=strlen(tempStr);i< __END_OF_OPTS;i++) printf(" ");
	
	/* then print the args and one returns */
	printf("%s\n",args);
	
	print_indented_and_max_length(comment, 70);
	printf("\n");


}

void eca_opt_print_usage_long_nroff(const char *shortopt,const char *longopt,const char *args,const char *comment, int __END_OF_OPTS)
{
	char tempStr[1000];
	char NRC[55]; 
	
	if(strcmp(args,"")==0) {
		sprintf(NRC,".B ");
	}
	else {
		sprintf(NRC,".BI ");
	}
	
	/* first get the options string made  */
	if(strcmp(shortopt,"-") != 0 && strcmp(longopt,"--") != 0 ) {
		sprintf(tempStr,"%s , %s ",shortopt,longopt);
	}
	else if (strcmp(shortopt,"-") == 0) {
		sprintf(tempStr,"%s",longopt);
	}
	else if( strcmp(longopt,"--")==0) {
		sprintf(tempStr,"%s",shortopt);
	}
	
	
	
	/* then print the options string in quotes but do it after a .B or a .BI */
	printf("%s \"%s\" ",NRC,tempStr);
	
	
	/* then print the args and one return */
	if(strcmp(args,"")!=0) {
		printf("\"   %s\"",args);
	}
	printf("\n");
	
	/* then do the comment inside the appropriate indent sort of environment */
	printf(".RS\n%s\n.RE\n.PP\n",comment);



}

/* print four spaces Num times */
void PrintIndent(int Num)
{
	int i;
	for(i=0;i<Num;i++) {
		printf("    ");
	}
}

/* this is in development.  I'd like to print the setup out to some sort of XML format */
void eca_opt_print_option_xml(const char *shortopt,const char *longopt,const char *args,const char *short_comment,const char *long_comment, int __END_OF_OPTS, int indent_level)
{
	
	/* first we say that we are entering an option */
	PrintIndent(indent_level);
	printf("<option>\n");
	
	
	if(strcmp(shortopt,"-")!=0) {
		PrintIndent(indent_level+1);
		printf("<single_dash_tag>%s</single_dash_tag>\n",shortopt);
	}
	if(strcmp(longopt,"--")!=0) {
		PrintIndent(indent_level+1);
		printf("<double_dash_tag>%s</double_dash_tag>\n",longopt);
	}
	
	/* now, we can print out an arg field if it has it */
	if(strcmp(args,"")!=0) {
		PrintIndent(indent_level+1);
		printf("<args>%s</args>\n",args);
	}
	
	if(strcmp(short_comment,"")!=0) {
		PrintIndent(indent_level+1);
		printf("<short_help>%s</short_help>\n",short_comment);
	}
	
	if(strcmp(long_comment,"")!=0) {
		PrintIndent(indent_level+1);
		printf("<long_help>\n");
		print_spec_indented_and_max_length(long_comment, 70, indent_level+2);
		PrintIndent(indent_level+1);
		printf("</long_help>\n");
	}
	/* close out the option level */
	PrintIndent(indent_level);
	printf("</option>\n");
	
}

void option_name(char *tempStr, char *shortopt, char *longopt)
{
	if(strcmp(shortopt,"-") != 0 && strcmp(longopt,"--") != 0 ) {
		sprintf(tempStr,"%s/%s",shortopt,longopt);
	}
	else if (strcmp(shortopt,"-") == 0) {
		sprintf(tempStr,"%s",longopt);
	}
	else if( strcmp(longopt,"--")==0) {
		sprintf(tempStr,"%s",shortopt);
	}
}

void eca_opt_print_usage_short(const char *shortopt,const char *longopt,const char *args,
		const char *comment,int __END_OF_OPTS, int __LEN_OF_ARGS)
{
	int i;
	char tempStr[1000];
	
	
	/* first get the options string made */
	if(strcmp(shortopt,"-") != 0 && strcmp(longopt,"--") != 0 ) {
		sprintf(tempStr,"%s , %s ",shortopt,longopt);
	}
	else if (strcmp(shortopt,"-") == 0) {
		sprintf(tempStr,"     %s ",longopt);
	}
	else if( strcmp(longopt,"--")==0) {
		sprintf(tempStr,"%s ",shortopt);
	}
	
	/* then print the options string and pad with space as necessary */
	printf("%s",tempStr);
	for(i=strlen(tempStr);i< __END_OF_OPTS;i++) printf(" ");
	
	/* then print the args and pad with space as necessary */
	printf("%s",args);
	for(i=__END_OF_OPTS + strlen(args);i< __END_OF_OPTS + __LEN_OF_ARGS;i++) printf(" ");
	
	/* then print the short comment and a single return */
	printf("%s\n", comment);
}


int opt_snl(int *how_many_times,  /* how many times this option has been used so far---requires that a flag
									be made for each option */
			char *argv,   /* the argument  array */
			const char *sh,  /* short version of the option,  i.e. -a */
			const char *lon,  /* long version of option, i.e. -all-of-it */
			const char *args,  /* string expressing what the argument to the option look like, i.e. [J] [R] */
			const char *short_comment,  /* the text that will appear in the --help listing */
			const char *long_comment,  /* the text that will appear in the --help-full listing */
			int CommentLevel,   /* 0=no comment, 1=print out the --help listing, 2=print out the --help-full listing */
			int *Recog,   /* usually is zero when you enter the function.  If argv is the option, then it gets set to 1 */ 
			char *CurrLong,  /* if argv is this option, then this gets set to sh */
			char *CurrShort, /* if argv is this option, then this gets set to lon */
			int FinalPass, /* Flag, which if 1 causes the function to verify some final things. So, this 
						is for a final pass over all the options to see if we missed anything. */
			int *Error,  /* output parameter.  Set to one if there's an error related to this option */
			int __END_OF_OPTS,  /* a length, for formatting the --help and --help-full output */
			int __LEN_OF_ARGS, /* a length, for formatting the --help output */
			int IsRequired,    /* 0==>not required,  1==>is always required, 2==>is sometimes required depending on other
																				options given */
			int CoDependent,  /* 0==>the other option(s) which makes this a required option have not been given.
								>0 ==> at least one of the other options, (or combination thereof) has been issued
								  making this option required */
			const char* CondReqOptString,  /* text string that will get printed out when something is conditionally required
									but absent. It will follow the phrase:  "Error! opt-name absent but required when using..." */
			int max_usages, /* the maximum number of times an option should be used */
			int CanClash,    /* 0==>no other options clash with this  1==>another option may clash with this */
			int Clashing,  /* 0==>the other option(s) which clashes with this one has not been given.
								>0 ==> at least one of the other options, (or combination thereof) has been issued
								 causing this option to clash */
			const char* ClashString /* the name of the conflicting option(s) */
 ) 
{
	char tempStr[1000];
	
	if( FinalPass==1) {
		if( *how_many_times == 0 && IsRequired==1 ) {
			option_name(tempStr, (char *)sh, (char *)lon);
			fprintf(stderr,"Error Processing Option %s!    The option %s is required but is absent.\n",tempStr, tempStr);
			*Error = 1;
		}
		if( *how_many_times == 0 && IsRequired==2 && CoDependent>0 ) {
			option_name(tempStr, (char *)sh, (char *)lon);
			fprintf(stderr,"Error Processing Option %s!    The option %s is absent, but is required when using %s\n",tempStr, tempStr, CondReqOptString);
			*Error = 1;
		}
		if( *how_many_times > max_usages ) {
			option_name(tempStr, (char *)sh, (char *)lon);
			fprintf(stderr,"Error Processing Option %s!    The option %s is used %d times but should have been used at most %d times.\n",
					tempStr,tempStr, *how_many_times, max_usages);
			*Error = 1;
		}
		if(CanClash && (Clashing && *how_many_times>0) ) {
			option_name(tempStr, (char *)sh, (char *)lon);
			fprintf(stderr,"Error Processing Option %s!    Conflicting options when %s %s\n",tempStr, ClashString);
			*Error = 1;
		}
		
		return(0);
	}
	if(CommentLevel==2) {
		if(strcmp(long_comment,"")==0) { /* if no long comment is provided, use the short comment */
			eca_opt_print_usage_long(sh,lon,args,short_comment, __END_OF_OPTS);
		}
		else {
			eca_opt_print_usage_long(sh,lon,args,long_comment, __END_OF_OPTS);
		}
		return(0);
	}
	if(CommentLevel==3) {  /* this is nroff output to make a man page */
		if(strcmp(long_comment,"")==0) { /* if no long comment is provided, use the short comment */
			eca_opt_print_usage_long_nroff(sh,lon,args,short_comment, __END_OF_OPTS);
		}
		else {
			eca_opt_print_usage_long_nroff(sh,lon,args,long_comment, __END_OF_OPTS);
		}
		return(0);
	}
	if(CommentLevel==4) {
		eca_opt_print_option_xml(sh,lon,args,short_comment,long_comment, __END_OF_OPTS,0);
		return(0);
	}
	if(CommentLevel==1) {
		eca_opt_print_usage_short(sh,lon,args,short_comment, __END_OF_OPTS, __LEN_OF_ARGS );
		return(0);
	}
	if( strcmp(argv,sh)==0  || strcmp(argv,lon)==0) {
		*Recog = 1;
		/* set the current environment knowledge of which options
		   were just processed */
		sprintf(CurrLong,"%s",lon);
		sprintf(CurrShort,"%s",sh);
		
		/* increment the how_many_times flag */
		*how_many_times = *how_many_times + 1;
		
		return(1);
	}
	return(0);
		
}

/* 
	returns a 0 or a 1 or 2 as to whether a string is 
	not an option (0),  is an option---i.e.
	it is of the form -a, where a is an alphabetical character (1), or
	is an option of the long form --string (2).
*/
int is_opt(char *a)
{
	if(a[0]=='-' && isalpha(a[1]) )
		return(1);
	if(a[0]=='-' && a[1]=='-')
		return(2);
		
	return(0);
}

/* counts the number of non-option (i.e. non -char or --string) arguments
in argv after the i-th element, until the next option, or until the end of 
argv is reached */
int args_to_next_opt(int i,int argc, char **argv)
{
	int j=0;
	
	i++;
	while(i<argc  && is_opt(argv[i]) == 0 ) {
		i++;
		j++;
	}
	return(j);
}
/* return a 1 if the number of args until the next -char or --string
   option (or until the argc-th string in argv) is equal to Num.  i is 
   the index of the element in argv that holds the current option, and argv 
   and argc are what they always are.
 */
int args_eq(int Num, int *Opt_i, int argc, char **argv, char *shortopt, char *longopt, int *Error )
{	
	char tempStr[1000];
	int i = *Opt_i;
	int act;
	
	act = args_to_next_opt(i,argc,argv);
	if(act==Num) 
		return(1);
	
	option_name(tempStr,shortopt,longopt);
	
	fprintf(stderr,"Error Processing Option %s!   Incorrect number of arguments (%d instead of %d) to option %s\n",tempStr,act,Num,tempStr);
	*Error = 1;
	
	/* then increment your way past all of the arguments to the current option so the don't become unrecognized options */
	for(i=0;i<act;i++) {
		(*Opt_i) = (*Opt_i) + 1;
	}
	return(0);
}
/* return a 1 if the number of args until the next -char or --string
   option (or until the argc-th string in argv) is greater than  Num.  i is 
   the index of the element in argv that holds the current option, and argv 
   and argc are what they always are.
 */
int args_gt(int Num, int *Opt_i, int argc, char **argv, char *shortopt, char *longopt, int *Error )
{	
	char tempStr[1000];
	int act, i=*Opt_i;
	
	act = args_to_next_opt(i,argc,argv);
	if(act>Num) 
		return(1);
	
	option_name(tempStr,shortopt,longopt);
	
	fprintf(stderr,"Error Processing Option %s! Incorrect number of arguments (%d instead of >%d) to option %s\n",tempStr,act,Num,tempStr);
	*Error = 1;
	
	/* then increment your way past all of the arguments to the current option so the don't become unrecognized options */
	for(i=0;i<act;i++) {
		(*Opt_i) = (*Opt_i) + 1;
	}
	return(0);
}

/* return a 1 if the number of args until the next -char or --string
   option (or until the argc-th string in argv) is greater than or equal to  Num.  i is 
   the index of the element in argv that holds the current option, and argv 
   and argc are what they always are.
 */
int args_geq(int Num, int *Opt_i, int argc, char **argv, char *shortopt, char *longopt, int *Error )
{	
	char tempStr[1000];
	int act, i=*Opt_i;
	
	 act = args_to_next_opt(i,argc,argv);
	if(act>=Num) 
		return(1);
	
	option_name(tempStr,shortopt,longopt);
	
	fprintf(stderr,"Error Processing Option %s!  Incorrect number of arguments (%d instead of >=%d) to option %s\n",tempStr,act,Num,tempStr);
	*Error = 1;
	
	/* then increment your way past all of the arguments to the current option so the don't become unrecognized options */
	for(i=0;i<act;i++) {
		(*Opt_i) = (*Opt_i) + 1;
	}
	return(0);
}

/* return a 1 if the number of args until the next -char or --string
   option (or until the argc-th string in argv) is less than  Num.  i is 
   the index of the element in argv that holds the current option, and argv 
   and argc are what they always are.
 */
int args_lt(int Num, int *Opt_i, int argc, char **argv, char *shortopt, char *longopt, int *Error )
{	
	char tempStr[1000];
	int act,i=*Opt_i;
	
	act = args_to_next_opt(i,argc,argv);
	if(act<Num) 
		return(1);
	
	option_name(tempStr,shortopt,longopt);
	
	fprintf(stderr,"Error Processing Option %s!  Incorrect number of arguments (%d instead of <%d) to option %s\n",tempStr,act,Num,tempStr);
	*Error = 1;
	
	/* then increment your way past all of the arguments to the current option so the don't become unrecognized options */
	for(i=0;i<act;i++) {
		(*Opt_i) = (*Opt_i) + 1;
	}
	
	return(0);
}

/* return a 1 if the number of args until the next -char or --string
   option (or until the argc-th string in argv) is less than or equal to  Num.  i is 
   the index of the element in argv that holds the current option, and argv 
   and argc are what they always are.
 */
int args_leq(int Num, int *Opt_i, int argc, char **argv, char *shortopt, char *longopt, int *Error )
{	
	char tempStr[1000];
	int act, i=*Opt_i;
	
	act = args_to_next_opt(i,argc,argv);
	if(act<=Num) 
		return(1);
	
	option_name(tempStr,shortopt,longopt);
	
	fprintf(stderr,"Error Processing Option %s!  Incorrect number of arguments (%d instead of <=%d) to option %s\n",tempStr,act,Num,tempStr);
	*Error = 1;
	
	/* then increment your way past all of the arguments to the current option so the don't become unrecognized options */
	for(i=0;i<act;i++) {
		(*Opt_i) = (*Opt_i) + 1;
	}
	
	return(0);
}


int already_has(int Flag, char *str, char *CurrShort, char *CurrLong, int *Error)
{
	char tempStr[1000];
	if(Flag > 0) 
		return(1);
	
	option_name(tempStr,CurrShort,CurrLong);
	fprintf(stderr,"Error Processing Option %s!   The option %s requires establishing option %s first\n",tempStr,tempStr,str);
	*Error = 1;
	return(0);	
}

int has_but_should_not(int Flag, char *str, char *CurrShort, char *CurrLong, int *Error)
{
	char tempStr[1000];
	if(Flag == 0) 
		return(0);
	
	option_name(tempStr,CurrShort,CurrLong);
	fprintf(stderr,"Error Processing Option %s!    The option %s should not be called subsequent to establishing option %s\n",tempStr,tempStr,str);
	*Error = 1;
	return(1);	
}


/* 
	return a string which is the entry after the last '/'  Hence this strips
	off all the initial directory path stuff.  It takes a single argument---which 
	should typically be argv.  
*/
char *progname(char *argv[]) 
{
	if(strstr(argv[0],"/")==NULL) {
		return(argv[0]);
	}
	return( strrchr(argv[0],'/') + 1);	
}

/* reads from a stream.  If it sees a comment delimiter it reads until it
hits the next comment delimiter, or until EOF. This returns a 1 if it ate
a comment, and a zero otherwise.  */ 
int eat_comments(FILE *in, char delim)
{
	char now;
	int ate_something = 0;

do { 
	
	/* first eat up any white-space */
	do {
		now = fgetc(in);
	} while(isspace(now));
	
	/* then go ahead and eat any comments */
	if(now==delim) {
		now = fgetc(in);
		while(now != delim || now==EOF) {
			now = fgetc(in);
		}
		ate_something = 1;
	}
	
	if(ate_something==0) { /* in this  case it only ate up some white space and we have to put 
							the last thing we got back onto the stream */
		ungetc(now,in);
	}
} while(now==delim);
	
	if(ate_something==1)
		ungetc(now,in);
	return(ate_something);
}

/* convert a file to an array of strings.  This takes a filename.
returns an array of strings.  N is set to the number of strings.
This allows for one to have comments in a command line file ---
comments are anything between a pair of & symbols.   */
char **file2charstarstar(char *FileName, int *N)
{
	char **s = (char **)calloc(MAX_COMM_FILE_TOKENS,sizeof(char *));
	int n=0;
	FILE *in;
	char temp[5000];
	int stop=0;
	
	if( (in=fopen(FileName,"r"))==NULL ) {
		printf("Couldn't open file %s to create array of strings from its contents\nExiting...\n",FileName);
		exit(1);
	}
	
	while(!stop) {
	
		eat_comments(in,'&');
		
		if(fscanf(in," %s",temp) ) {
			/* this line is a bunch of stupid stuff so that I can deal with final lines in the file
			having whitespace or not.  stupid C stuff... */
			if( strchr(temp,'-') != NULL || isalnum(temp[0]) || temp[0] == '.' || strchr(temp,'/') != NULL ) {
				s[n] = (char *)calloc(strlen(temp)+10,sizeof(char));
				sprintf(s[n++],"%s",temp);
			}
			sprintf(temp,"");
		}
		if(feof(in)) { 
			stop = 1;
		}
	}
	fclose(in);
	
	
	*N = n;
	
	return(s);
}

/* take the contents of a file, turn them into an array of strings, and then 
insert those into argv. i is the position in argv immediately after the point
where the filename from which
the new commands are taken was.  Ultimately, this gets decremented by 2, and the
strings will be inserted starting with argv[i+1]. */
void insert_file_into_commline(char ***argv, int *argc, int *i, char *FileName)
{
	int j,N,t;
	char **contents, **newone;
	int start = *i;
	
	/* decrement the i counter by two */
	*i = *i - 2;
	
	/* first, get the contents of the file */
	contents = file2charstarstar(FileName,&N);
	
	if(N==0 && start == *argc-1) {
		printf("Error Processing Option --command-file! The last option you gave was for inserting a command file that appears to be empty.  Illegal\n");
		exit(1);
	}
	
	/* now, allocate enough extra memory to newone to accomodate the new strings */
	/* (typically you could reallocate argv, but it isn't malloc-ed in the first place,
		I think, so, we do this different version, 	WHICH IS DEFINITELY A LITTLE MEMORY-LEAKY */
	newone = (char **)calloc(*argc+N-2, sizeof(char *));
	
	/* then take the first values in argv and put them into newone exept for the two strings
	 associated with the call that sent us to the file in the 
	first place */
	for(j=0;j<=*i;j++) {
		newone[j] = (*argv)[j];
	}
	
	/* then take the old values in i+1 till the end and copy them to i+1+N and beyond */
	for(j=start+1;j<*argc;j++)  {
		newone[j+N-2] = (*argv)[j];
	}
	
	/* then copy the strings in contents into the appropriate places in newone  */
	for(t=0,j=*i+1;t<N;j++)  {
		newone[j] = contents[t++];
	}
	
	/* then free contents (this will free just the char ** part */
	free(contents);
	
	/* then put newone into the place of argv */
	(*argv) = newone;
	
	/* update argc to reflect the new additions */
	*argc = *argc + N - 2;
	
	/* increment *i by one so that it will continue on its merry way processing everything as before */
	 *i = *i + 1;
	
}

