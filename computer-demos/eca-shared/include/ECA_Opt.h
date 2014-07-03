/* Limits */
#define MAX_COMM_FILE_TOKENS  1000000   /* maximum number of strings that can be read out of a command file */

/* macros */
#define OPTION(USAGE_COUNTER,SHORT,LONG,ARGS,SHORT_COMMENT,LONG_COMMENT) \
                           opt_snl(&USAGE_COUNTER, argv[__Opt_i],"-"#SHORT,"--"#LONG,#ARGS,#SHORT_COMMENT,#LONG_COMMENT,\
					       __OptCommentLevel,&__OptRecog, __OptCurrLong, __OptCurrShort, __OptSeekUnusedRequiredOpts, \
					       	&__OptError, __OptEND_OF_OPTS ,__OptLEN_OF_ARGS, 0, 0, "" , 1 , 0,0,"")

#define REQUIRED_OPTION(USAGE_COUNTER,SHORT,LONG,ARGS,SHORT_COMMENT,LONG_COMMENT) \
                           opt_snl(&USAGE_COUNTER, argv[__Opt_i],"-"#SHORT,"--"#LONG,#ARGS,#SHORT_COMMENT,#LONG_COMMENT,\
					       __OptCommentLevel,&__OptRecog, __OptCurrLong, __OptCurrShort, __OptSeekUnusedRequiredOpts, \
					       	&__OptError, __OptEND_OF_OPTS ,__OptLEN_OF_ARGS, 1,0,"", 1, 0,0,"")
					       	
#define COND_REQ_OPTION(USAGE_COUNTER,SHORT,LONG,ARGS,SHORT_COMMENT,LONG_COMMENT, WHEN_REQUIRED, CODEPEND_STATEMENT) \
                           opt_snl(&USAGE_COUNTER, argv[__Opt_i],"-"#SHORT,"--"#LONG,#ARGS,#SHORT_COMMENT,#LONG_COMMENT,\
					       __OptCommentLevel,&__OptRecog, __OptCurrLong, __OptCurrShort, __OptSeekUnusedRequiredOpts, \
					       	&__OptError, __OptEND_OF_OPTS ,__OptLEN_OF_ARGS, 2 , WHEN_REQUIRED , #CODEPEND_STATEMENT , 1 , 0,0,"")
					       	
#define MULT_USE_REQ_OPTION(USAGE_COUNTER,SHORT,LONG,ARGS,SHORT_COMMENT,LONG_COMMENT, MAX_USES) \
                           opt_snl(&USAGE_COUNTER, argv[__Opt_i],"-"#SHORT,"--"#LONG,#ARGS,#SHORT_COMMENT,#LONG_COMMENT,\
					       __OptCommentLevel,&__OptRecog, __OptCurrLong, __OptCurrShort, __OptSeekUnusedRequiredOpts, \
					       	&__OptError, __OptEND_OF_OPTS ,__OptLEN_OF_ARGS, 1,0,"", MAX_USES, 0,0,"")
					       	
#define MULT_USE_OPTION(USAGE_COUNTER,SHORT,LONG,ARGS,SHORT_COMMENT,LONG_COMMENT, MAX_USES) \
                           opt_snl(&USAGE_COUNTER, argv[__Opt_i],"-"#SHORT,"--"#LONG,#ARGS,#SHORT_COMMENT,#LONG_COMMENT,\
					       __OptCommentLevel,&__OptRecog, __OptCurrLong, __OptCurrShort, __OptSeekUnusedRequiredOpts, \
					       	&__OptError, __OptEND_OF_OPTS ,__OptLEN_OF_ARGS, 0, 0, "" , MAX_USES, 0,0,"")
					       	
#define MULT_USE_COND_REQ_OPTION(USAGE_COUNTER,SHORT,LONG,ARGS,SHORT_COMMENT,LONG_COMMENT, MAX_USES, WHEN_REQUIRED, CODEPEND_STATEMENT) \
                           opt_snl(&USAGE_COUNTER, argv[__Opt_i],"-"#SHORT,"--"#LONG,#ARGS,#SHORT_COMMENT,#LONG_COMMENT,\
					       __OptCommentLevel,&__OptRecog, __OptCurrLong, __OptCurrShort, __OptSeekUnusedRequiredOpts, \
					       	&__OptError, __OptEND_OF_OPTS ,__OptLEN_OF_ARGS, 2 , WHEN_REQUIRED , #CODEPEND_STATEMENT , MAX_USES, 0,0,"")

#define CLASHABLE_OPTION(USAGE_COUNTER,SHORT,LONG,ARGS,SHORT_COMMENT,LONG_COMMENT, WHEN_CLASHES, CLASH_STATEMENT ) \
                           opt_snl(&USAGE_COUNTER, argv[__Opt_i],"-"#SHORT,"--"#LONG,#ARGS,#SHORT_COMMENT,#LONG_COMMENT,\
					       __OptCommentLevel,&__OptRecog, __OptCurrLong, __OptCurrShort, __OptSeekUnusedRequiredOpts, \
					       	&__OptError, __OptEND_OF_OPTS ,__OptLEN_OF_ARGS, 0, 0, "" , 1 , 1, WHEN_CLASHES, #CLASH_STATEMENT)

					       	

#define COUNT_ARGS args_to_next_opt(__Opt_i,argc,argv)
#define ARGS_EQ(N) args_eq(N,&__Opt_i,argc,argv,__OptCurrShort,__OptCurrLong,&__OptError)
#define ARGS_GT(N) args_gt(N,&__Opt_i,argc,argv,__OptCurrShort,__OptCurrLong,&__OptError)
#define ARGS_LT(N) args_lt(N,&__Opt_i,argc,argv,__OptCurrShort,__OptCurrLong,&__OptError)
#define ARGS_GEQ(N) args_geq(N,&__Opt_i,argc,argv,__OptCurrShort,__OptCurrLong,&__OptError)
#define ARGS_LEQ(N) args_leq(N,&__Opt_i,argc,argv,__OptCurrShort,__OptCurrLong,&__OptError)
#define ALREADY_HAS(Flag, Option)  already_has(Flag, #Option, __OptCurrShort, __OptCurrLong, &__OptError)
#define HAS_BUT_SHOULD_NOT(Flag, Option)  has_but_should_not(Flag, #Option, __OptCurrShort, __OptCurrLong, &__OptError)
#define GET_INT atoi(argv[++__Opt_i]); if(!string_is_good_int(argv[__Opt_i])) { char tempstr[2000]; \
			option_name(tempstr,__OptCurrShort, __OptCurrLong); \
			fprintf(stderr,"Error Processing Option %s!    Argument \"%s\" to option %s is not a valid integer\n",tempstr, argv[__Opt_i],tempstr); OPT_ERROR; }
#define GET_DUB atof(argv[++__Opt_i]);
#define GET_STR(X) sprintf(X,"%s",argv[++__Opt_i]);
#define OPT_ERROR __OptError = 1;

#define DECLARE_ECA_OPT_VARS  \
	int __Opt_i;   \
	int __Opt_j;    \
	int __OptCommentLevel = 0; \
	int __OptRecog;   \
	int __OptError = 0;   \
	char __OptCurrLong[1000];    \
	char __OptCurrShort[1000];   \
	int __OptSeekUnusedRequiredOpts = 0; \
	int __OptEND_OF_OPTS = 30; \
	int __OptLEN_OF_ARGS = 15; \
	int __OptHelpFlag = 0; \
	int __OptHelpFullFlag = 0; \
	int __OptHelpNroffFlag = 0; \
	int __OptHelpXMLFlag = 0; \
	int __OptVersionFlag = 0;  \
	int __OptCommFileFlag = 0; \
	int __OptIncludeCommFileVerbose = 1; \
	char __OptVersionString[2500]; \
	char __OptVerionsHistoryString[10000];  \
	char __OptProgDescription[2500]; \
	int __OptVersionHistoryFlag = 0; \
	sprintf(__OptVersionString,"\n\n      ----    No Version Information Given    ----  \n\n");  \
	sprintf(__OptVerionsHistoryString,"\n\n      ----    No Version History Given    ----  \n\n");  \
	sprintf(__OptProgDescription," "); 
	
	
	
	
#define SET_OPT_WIDTH(X) __OptEND_OF_OPTS = X;
#define SET_ARG_WIDTH(X) __OptLEN_OF_ARGS = X;
#define SET_VERSION(X) sprintf(__OptVersionString,X);
#define SET_VERSION_HISTORY(X) sprintf(__OptVerionsHistoryString,X);
#define SET_DESCRIBE(X) sprintf(__OptProgDescription,X);
	
#define BEGIN_OPT_LOOP  \
	__Opt_i=0;  \
	while(__Opt_i<argc) {\
		__OptRecog = 0; \
		__Opt_i++;\
		if(__Opt_i==argc)  {  \
			if( __OptSeekUnusedRequiredOpts == 0 ) {  \
				 __OptSeekUnusedRequiredOpts = 1; /* take an extra pass ONLY to see if we missed any required options */ \
				 __OptRecog = 1;  \
				 __Opt_i = argc-1;  \
			}  \
		} \
		/* short help option */ \
		if(  OPTION (__OptHelpFlag, , \
						help,  \
						,  \
						short listing of options,  \
			 			this returns a short list of all program options and associated arguments) )  \
		 {  \
		   	__OptCommentLevel = 1;   \
		   	printf("%s\n\n",__OptProgDescription);  \
/*		   	printf("\n   ****  Program-description and documentation  parameters  ****\n\n"); \  */ \
		   	/* And now this part is a total hack to get the --help line to print out right */  \
		   	OPTION (__OptHelpFlag, , \
						help, \
						,  \
						short listing of options,  \
			 			this returns a short list of all program options and associated arguments);  \
		 }  \
		/* long help option */  \
		if(  OPTION (__OptHelpFullFlag , ,  \
						help-full,  \
						,  \
						long listing of options,  \
			 			this returns a full list of all program options and associated arguments) )  \
		 {  \
		   	__OptCommentLevel = 2;  \
		   	printf("\n%s\n",__OptProgDescription); \
		   	printf("\nIn the following:\n"); \
		   	printf("\t[J] refers to an integer argument to an option\n");  \
		   	printf("\t[R] refers to a real number argument to an option\n");  \
		   	printf("\t[S] refers to a string argument to an option\n");  \
		   	printf("\t[Range] refers to a string that gives a discontinous range of\n");  \
		   	printf("\t\tnonnegative integers.  For example:  \"1-5,7,9,10-15\" specifies\n");  \
		   	printf("\t\tthe integers 1 through 5, 7, 9, and 10 through 15.  There can be no\n");  \
		   	printf("\t\twhitespace in the string specifying the range, and the numbers must all\n");  \
		   	printf("\t\tbe increasing.  Also, the string cannot start nor end with a comma or a dash.\n");  \
		   	printf("\t\tFinally, you should not use \"-\" to denote two ranges without placing any\n");  \
		   	printf("\t\tcommas in between.\n");  \
		   	printf("\n   ****  Program-description and documentation  parameters  ****\n\n"); \
		   	/* And now this part is a total hack to get the --help line to print out right */  \
		   	OPTION (__OptHelpFlag , ,  \
						help, \
						,  \
						short listing of options,  \
			 			this returns a short list of all program options and associated arguments);  \
		   	OPTION (__OptHelpFullFlag , ,  \
						help-full,   \
						,  \
						long listing of options,  \
			 			this returns a full list of all program options and associated arguments);  \
		 } \
		 /* nroff help option */  \
		if(  OPTION (__OptHelpNroffFlag , ,  \
						help-nroff,  \
						,  \
						long listing of options in nroff man-page format,  \
			 			this returns a full list of all program options and associated arguments \
			 			using the formatting styles in nroff that give you the look of a man page.  \
			 		    View the formatted ouput by doing: 'prog --help-nroff | nroff -man | more' where prog is the name of the program.  ) )  \
		 {  \
		   	__OptCommentLevel = 3;  \
		   	printf(".\\\" Process this file with \n.\\\" groff -man -Tascii FileName.1 \n.\\\"\n");  \
		   	printf(".TH %s 1 \"Date Not Available\" AUTO_GENERATED_BY_ECA_OPTION_PACKAGE \"User Manuals\"\n",argv[0]);  \
		   	printf(".SH NAME\n%s\n",__OptProgDescription); \
		   	printf(".\\\" starting program-generated options part\n"); \
		   	printf(".SH OPTIONS\n.PP\n"); \
		   	printf("In the following:\n\n"); \
		   	printf(".IR [J] \" refers to an integer argument to an option\"\n\n");  \
		   	printf(".IR [R] \" refers to a real number argument to an option\"\n\n");  \
		   	printf(".IR [S] \" refers to a string argument to an option\"\n\n");  \
		   	printf(".IR [Range] \" refers to a string that gives a discontinous range of\"\n");  \
		   	printf("nonnegative integers.  For example:  \"1-5,7,9,10-15\" specifies\n");  \
		   	printf("the integers 1 through 5, 7, 9, and 10 through 15.  There can be no\n");  \
		   	printf("whitespace in the string specifying the range, and the numbers must all\n");  \
		   	printf("be increasing.  Also, the string cannot start nor end with a comma or a dash.\n");  \
		   	printf("Finally, you should not use \"-\" to denote two ranges without placing any\n");  \
		   	printf("commas in between.\n\n");  \
		   	printf("\n   ****  Program-description and documentation  parameters  ****\n.PP\n"); \
		   	/* And now this part is a total hack to get the --help line to print out right */  \
		   	OPTION (__OptHelpFlag , ,  \
						help, \
						,  \
						short listing of options,  \
			 			this returns a short list of all program options and associated arguments);  \
		   	OPTION (__OptHelpFullFlag , ,  \
						help-full,   \
						,  \
						long listing of options,  \
			 			this returns a full list of all program options and associated arguments);  \
			OPTION (__OptHelpNroffFlag , ,  \
						help-nroff,  \
						,  \
						long listing of options in nroff man-page format,  \
			 			this returns a full list of all program options and associated arguments \
			 			using the formatting styles in nroff that give you the look of a man page.  \
			 		     View the formatted ouput by doing: 'prog --help-nroff | nroff -man | more' where prog is the name of the program. );   \
			 OPTION (__OptHelpXMLFlag , ,  \
						help-xml,  \
						,  \
						output all options in XML format (in development),  \
			 			This returns a list of all options in a simple XML format. Ultimately \
			 			I hope to be able to use that to quickly and automatically design a GUI to  \
						construct an appropriate command line and run the program);  \
		 } \
		 /* spits it out to XML */ \
		 if(OPTION (__OptHelpXMLFlag , ,  \
						help-xml,  \
						,  \
						output all options in XML format (in development),  \
			 			This returns a list of all options in a simple XML format. Ultimately \
			 			I hope to be able to use that to quickly and automatically design a GUI to  \
						construct an appropriate command line and run the program))  \
		{     \
			__OptCommentLevel = 4; \
		} \
		 /* version option */ \
		if(  OPTION (__OptVersionFlag, , \
						version,  \
						,  \
						prints program version information,  \
			 			) )  \
		 {  \
		 	printf("%s",__OptVersionString); \
		 	exit(0);  \
		 }  \
		 /* version history option */  \
		 if(  OPTION (__OptVersionHistoryFlag, , \
						version-history,  \
						,  \
						prints history of different versions,  \
			 			) )  \
		 {  \
		 	printf("%s\n",__OptVerionsHistoryString); \
		 	exit(0);  \
		 }  \
		 if(  MULT_USE_OPTION (__OptCommFileFlag, , \
						command-file,  \
						[S],  \
						inserts contents of file [S] into the command line,  \
						By using this option you can store command line arguments in the  \
						file named in [S].  You may have any kind of white space (including   \
						line endings) in the file.  The line endings are treated as just another  \
						space.  Comments may be included in the file by enclosing them within a pair of \
						ampersands (the & character).  Note that you must have a & at the beginning and  \
						at the end of the comment.  You cannot use just a single & to comment to the end of \
						a line.  Your comments may spread over several lines---they will still be stripped from \
						the resulting command line so long as the are enclosed in ampersands. \
						This feature is helpful if you have long and  complex command lines that    \
						you wish to store if it makes it easier to read the command \
						line by breaking it across multiple lines or if you have certain clusters of options  \
						that you like to store together as a module.  This option may be used up to 10000 times. Optional., \
			 			10000) )  \
		 {  \
		 	if(ARGS_EQ(1) ) { char temp_filename[5000];\
			 		GET_STR(temp_filename);  \
			 		insert_file_into_commline(&argv,&argc,&__Opt_i,temp_filename);  \
			 		if(__OptIncludeCommFileVerbose) printf("INSERTING_COMMANDS_FROM_FILE : %s\n",temp_filename);  \
			 		if(__OptIncludeCommFileVerbose) printf("NEW_COMMAND_LINE_LOOKS_LIKE : "); \
			 		if(__OptIncludeCommFileVerbose) {for(__Opt_j=0;__Opt_j<argc;__Opt_j++)  {  \
			 			printf("%s ",argv[__Opt_j]);  \
			 		}}  \
			 		if(__OptIncludeCommFileVerbose) printf("\n"); \
		 	} \
		 }  \
		 


#define END_OPT_LOOP  \
		 if(__OptRecog == 0) { int act; int i; \
		 	fprintf(stderr,"Error! Unrecognized option: %s\n",argv[__Opt_i]);  \
			/* then eat any thing on the command line that doesn't start with - or -- */ \
			/* i.e., get rid of any arguments that might have gone with this unrecognized option */ \
			act = args_to_next_opt(__Opt_i,argc,argv);\
			for(i=0;i<act;i++) {   \
				__Opt_i++;  \
			} \
		 	__OptError = 1;  \
		 }  \
		 if(  __OptSeekUnusedRequiredOpts == 1 ) { \
				break; \
			} \
		if(  __OptCommentLevel > 0 ) { \
				break; \
			} \
	}  \
	if(__OptError == 1) {fprintf(stderr,"\nErrors encountered processing command line.  Use\n\t%s --help\nor\n\t%s --help-full\nfor information on available options\n", progname(argv),progname(argv));  exit(1);} \
	if(__OptCommentLevel==3) { printf(".\\\" done with program-generated options part\n"); } \
	if(__OptCommentLevel > 0 || __OptVersionFlag > 1) {exit(0);} \



/* prototypes */
int string_is_good_int(const char *s);
void print_indented_and_max_length(const char *str, int len);
void print_spec_indented_and_max_length(const char *str, int len, int Num);
void PrintIndent(int Num);
void eca_opt_print_option_xml(const char *shortopt,const char *longopt,const char *args,const char *short_comment,const char *long_comment, int __END_OF_OPTS, int indent_level);
void eca_opt_print_usage_long(const char *shortopt,const char *longopt,
			const char *args,const char *comment, int __END_OF_OPTS);
void eca_opt_print_usage_long_nroff(const char *shortopt,const char *longopt,const char *args,const char *comment, int __END_OF_OPTS);
void eca_opt_print_usage_short(const char *shortopt,const char *longopt,const char *args,
		const char *comment,int __END_OF_OPTS, int __LEN_OF_ARGS);
void option_name(char *tempStr, char *shortopt, char *longopt);
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
 ); 
int is_opt(char *a);
int args_to_next_opt(int __Opt_i,int argc, char **argv);
int args_eq(int Num, int *__Opt_i, int argc, char **argv, char *shortopt, char *longopt, int *Error );
int already_has(int Flag, char *str, char *CurrShort, char *CurrLong, int *Error);
int has_but_should_not(int Flag, char *str, char *CurrShort, char *CurrLong, int *Error);
char *progname(char *argv[]);
char **file2charstarstar(char *FileName, int *N);
void insert_file_into_commline(char ***argv, int *argc, int *__Opt_i, char *FileName);
int args_gt(int Num, int *__Opt_i, int argc, char **argv, char *shortopt, char *longopt, int *Error );
int eat_comments(FILE *in, char delim);
int args_geq(int Num, int *__Opt_i, int argc, char **argv, char *shortopt, char *longopt, int *Error );
int args_lt(int Num, int *__Opt_i, int argc, char **argv, char *shortopt, char *longopt, int *Error );
int args_leq(int Num, int *__Opt_i, int argc, char **argv, char *shortopt, char *longopt, int *Error );


