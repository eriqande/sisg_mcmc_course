/* 
This file was written by Eric C. Anderson (eric.anderson@noaa.gov)

It is the product of work on US Government time and machines. It is 
released into the Public Domain with the disclaimer from the The MIT licence, 
which is listed below: 

Permission is hereby granted, free of charge, to any person obtaining a copy 
of this software and associated documentation files (the "Software"), to deal in 
the Software without restriction, including without limitation the rights to use, 
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the 
Software, and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions: 
The above copyright notice and this permission notice shall be included in 
all copies or substantial portions of the Software. 
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF 
ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTIC- 
ULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CON- 
TRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CON- 
NECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
IN THE SOFTWARE. 
*/




/* SOME COMPILING OPTIONS
 The following can be #defined on the command line, if desired:
 
 QUIET_COMMAND_FILE_INSERTION       --   Suppresses printing out of commands in command files when being inserted
 
 
 
*/

#include <stdlib.h>


/* enums */

/* these are for returning the status of SimpleProcArgs */
typedef enum {
	BOOLEAN_PANEL,
	FILE_CHOOSER_PANEL,
	CHOICE_PANEL,
	COMPLEX_PANEL,
	MFLAG_PANEL
} ArgPanelType;


typedef enum {
	__Unknown_tok,
	__J_tok,
	__R_tok,
	__S_tok,
	__F_tok,
	__D_tok,
	__G_tok,
	__C_tok
} ArgToksEnum;



/*! @name #defined Limits and Integers */
/*@{*/
#define MAX_COMM_FILE_TOKENS  5000000   /*!< Maximum number of strings that can be read out of a command file */
#define MAX_FORMATTED_ARGLIST_STR_LENGTH  10000 /*!< Maximum length of the arglist string.  This should be plenty, as most are no more than 5 to 10 characters */
#define NUM_ARG_TOK_TYPES 8  /*!< This is the number of types of enums in the #ArgToksEnum.  */
#define MAX_INTEGER_ARG_LENGTH 2000  /*!< The maximum length of an integer option argument that can be put on the command line. */
/*@}*/



/*! @name Macros Used to Declare Option Flags of Various Varieties */
/*@{*/
/*!
This is the standard macro to use to test the command line for an option that is optional (i.e. it is not required to be
on the command line for program execution) but which, if it appears in the command line, may appear only once.  This macro
provides a convenient way of using the function opt_snl(), appropriately passing the scads of arguments that function needs.
Indeed, you would never want to use opt_snl() directly.  \b Note: this macro and the related ones make heavy use of the preprocessor's
"string-pasteizer", which means that if you are supplying a string argument to it, <b>you leave that string unquoted</b>.  There is one
nasty problem with this: if the string you are supplying includes a comma, it is going to completely screw up the macro (the preprocessor
will interpret the comma as something that is separating macro parameters).   The handy way around this is to use the backslash-preceded ascii
code for the comma:   \\054.  For example, if you were documenting an option in the LONG field and you wanted to say, "This option
is sometimes useful, sometimes not." You would supply the unquoted string: <em>This option
is sometimes useful\\054 sometimes not.</em>  There are other times you may wish to use the octal code for punctuation; sometimes 
it is necessary to put in an unmatched parenthesis \\050 or \\051 or quotation marks: \\042, etc.

\param ENGLISH_NAME  This is the name that you would give to the option if you were just speaking plain English and not code-ese.  It is also
the title that will be given to the option in the GuiLiner option tree.
\param USAGE_COUNTER This should be an integer, initialized to zero, that is declared within the scope of the function where you are using 
ECA_Opt's main loop that is contained within the #BEGIN_OPT_LOOP / #END_OPT_LOOP pair.  It is used to count the number of occurrences of the flag for 
this particular option.  In this case, if that number is greater than 1, then the program issues an error.  With the #MULT_USE_OPTION and the #MULT_USE_REQ_OPTION
macros, the number of occurrences of the option's flag can be greater than 1.  
\param SHORT  This is a single character that defines the "short" flag for this option.  For example, if you set this parameter to the character
"A" then this option could be invoked with the flag -A on the command line.  The "short" format of an option flag is always just a single dash followed
by a single letter.  It has to be an alphabetical letter.  <b>IMPORTANT NOTE! You do not enclose the parameter value in any quotation marks!</b>  
It gets quoted using the string paste-izer.  Please don't pass in a
non-letter character for this parameter.  That oughtn't work.
\param LONG This is a word composed of characters, that can be digits or alphabetical characters or other characters that don't have special significance
in the shell.  I recommend restricting yourself to alphabetical letters and the hyphen.  This parameter is used to make the "long" version of the 
option flag.  The "long" version is a word that is preceded by two dashes.  In passing 
in this parameter the user does not pass the preceding dashes in.  So, for example if the parameter value were: "no-warnings"
then the long option flag associated with this option would be "--no-warnings".  <b>AGAIN! You do not enclose the parameter value in any quotation marks!</b>  
It gets quoted using the string paste-izer. 
\param ARGS This is an unquoted string describing the expected format of the arguments to this option.  These formats are used to provide
documentation in the --help options and it determines the type of panel that will be requested in the GuiLiner XML file.  There are currently
#NUM_ARG_TOK_TYPES = 8 types of arguments that are expected.  One of those is the "Unknown" type. So there are really only 7 expected types.
Each expected argument is named by a word
starting with a first letter which denotes what type of argument it is and followed by 0 or more letters or numbers that can be used to describe the  
argument.  The 7 expected types, named by the initial letters that signify them are:
  - F : a file path.  Example: \e infile.txt
  - D : a directory path. Example: \e /Users/eriq/MyCode/
  - J : an integer.  Example:  \e 18
  - R : a real number.  Example:  \e 2.718
  - S : a string.  Example:  \e yippie  or it may be a quoted string like: <em>"This is a quoted string argument"</em>
  - G : a range. This signifies a range of integers like you might use with the unix cut utility. Example:  <em>1,5-10,17</em>
  - C : a choice or a "constrained string". This signifies an argument that can take a small set of possible string options.  The possible choices should
be listed between curly braces and separated by pipes immediately following the C-word naming the argument.  This would look like, for example: 
C1{boing|bong|foo|bar|100}.
  .
So, for example, suppose you have an option to your program called --looking-for that wants to gather from the user whether
they are looking for a male/female who is tall/short with between a minimum age and a maximum age that can be specified as a real
number.  Then you might do an ARGS string that looked like: <em>Csex{male|female} Cheight{tall|short} R_min_age R_max_age</em>.

\param SHORT_COMMENT This is the text that you would like to appear in the short help format (obtained by invoking the program with
the --help option).  This also appears in blue in the GuiLiner information window for the option.  Typically about 50 characters or less 
giving a quick description of the option.  Remember <b>this is an unquoted string</b> and beware of commas (see above).

\param LONG_COMMENT This is the text that you want to appear in the long version of the documentation.  This should provide a full
description of the option.  It will come out with the --help-full, --help-nroff, and --help-xml options.  It will appear in the information 
window in GuiLiner.   Remember <b>this is an unquoted string</b> and beware of commas (see above).
 */
#define OPTION(ENGLISH_NAME,USAGE_COUNTER,SHORT,LONG,ARGS,SHORT_COMMENT,LONG_COMMENT) \
                           opt_snl(&USAGE_COUNTER, argv[__Opt_i],"-"#SHORT,"--"#LONG,#ARGS,#SHORT_COMMENT,#LONG_COMMENT,\
					       __OptCommentLevel,&__OptRecog, __OptCurrLong, __OptCurrShort, __OptSeekUnusedRequiredOpts, \
					       	&__OptError, __OptEND_OF_OPTS ,__OptLEN_OF_ARGS, 0, 0, "" , 1 , 0,0,"",__OptCurrentIndentLevel,#ENGLISH_NAME,1)

/*!
This macro is essentially identical to the #OPTION macro, except that it is for options that must appear exactly once
on the command line.  For an explanation of all the parameters, see #OPTION */
#define REQUIRED_OPTION(ENGLISH_NAME,USAGE_COUNTER,SHORT,LONG,ARGS,SHORT_COMMENT,LONG_COMMENT) \
                           opt_snl(&USAGE_COUNTER, argv[__Opt_i],"-"#SHORT,"--"#LONG,#ARGS,#SHORT_COMMENT,#LONG_COMMENT,\
					       __OptCommentLevel,&__OptRecog, __OptCurrLong, __OptCurrShort, __OptSeekUnusedRequiredOpts, \
					       	&__OptError, __OptEND_OF_OPTS ,__OptLEN_OF_ARGS, 1,0,"", 1, 0,0,"",__OptCurrentIndentLevel,#ENGLISH_NAME,1)

/*!
This macro provides a convenient way to invoke opt_snl() in a way that is configured for options that are not always required
but which become required options when another option (or set of options) is invoked. For an explanation of the first 8 parameters
see #OPTION.  The two remaining parameters are documented below.

\param WHEN_REQUIRED  This may be any integer expression involving variables available in the current scope where the #COND_REQ_OPTION macro is being
used.  It should be an expression that evaluates to 0 (zero) or non-zero.  The situation where it evaluates to non-zero is the situation in which 
the option described in the #COND_REQ_OPTION is required and when the expression evaluates to zero, this option is not required.  Often the 
WHEN_REQUIRED parameter will be an expression involving the USAGE_COUNTER variables of other options.  See #OPTION for a discussion of the USAGE_COUNTER.

\param CODEPEND_STATEMENT  This is an unquoted string (it gets quoted by the preprocessor) that is used in the error statement that occurs in situations when
 the option is required but is absent from the command line.  It gets appended to a statement that starts off with: "Error Processing Option %s!    
 The option %s is absent, but is required".  So you might consider filling this field with: <em>when you have invoked the --boing option or the --foo option but not
 the --bar option</em>
*/					       	
#define COND_REQ_OPTION(ENGLISH_NAME,USAGE_COUNTER,SHORT,LONG,ARGS,SHORT_COMMENT,LONG_COMMENT, WHEN_REQUIRED, CODEPEND_STATEMENT) \
                           opt_snl(&USAGE_COUNTER, argv[__Opt_i],"-"#SHORT,"--"#LONG,#ARGS,#SHORT_COMMENT,#LONG_COMMENT,\
					       __OptCommentLevel,&__OptRecog, __OptCurrLong, __OptCurrShort, __OptSeekUnusedRequiredOpts, \
					       	&__OptError, __OptEND_OF_OPTS ,__OptLEN_OF_ARGS, 2 , WHEN_REQUIRED , #CODEPEND_STATEMENT , 1 , 0,0,"",__OptCurrentIndentLevel,#ENGLISH_NAME,1)
					       	
/*!
This macro is identical to #REQUIRED_OPTION except that it is to be used for options that must be invoked at least once, but
may be invoked more than once---up to MAX_USES times, in fact.
All but the last parameter are described in the documentation for the #OPTION macro.  The last one is documented below.
\param MAX_USES This is the maximum number of times the option can be issued.  It can be an integer constant or even an integer expression using
variables availabe in the current scope.  
*/
#define MULT_USE_REQ_OPTION(ENGLISH_NAME,USAGE_COUNTER,SHORT,LONG,ARGS,SHORT_COMMENT,LONG_COMMENT, MAX_USES) \
                           opt_snl(&USAGE_COUNTER, argv[__Opt_i],"-"#SHORT,"--"#LONG,#ARGS,#SHORT_COMMENT,#LONG_COMMENT,\
					       __OptCommentLevel,&__OptRecog, __OptCurrLong, __OptCurrShort, __OptSeekUnusedRequiredOpts, \
					       	&__OptError, __OptEND_OF_OPTS ,__OptLEN_OF_ARGS, 1,0,"", MAX_USES, 0,0,"",__OptCurrentIndentLevel,#ENGLISH_NAME,1)
					       	
							
/*!
This macro is identical to #OPTION except it is for options that are not required but may be invoked multiple times.  See the documentation for
#OPTION for the first 8 parameters.
\param MAX_USES This is the maximum number of times the option can be issued.  It can be an integer constant or even an integer expression using
variables availabe in the current scope.  
*/
#define MULT_USE_OPTION(ENGLISH_NAME,USAGE_COUNTER,SHORT,LONG,ARGS,SHORT_COMMENT,LONG_COMMENT, MAX_USES) \
                           opt_snl(&USAGE_COUNTER, argv[__Opt_i],"-"#SHORT,"--"#LONG,#ARGS,#SHORT_COMMENT,#LONG_COMMENT,\
					       __OptCommentLevel,&__OptRecog, __OptCurrLong, __OptCurrShort, __OptSeekUnusedRequiredOpts, \
					       	&__OptError, __OptEND_OF_OPTS ,__OptLEN_OF_ARGS, 0, 0, "" , MAX_USES, 0,0,"",__OptCurrentIndentLevel,#ENGLISH_NAME,1)



/*!
This macro is for options that can be used multiple times and which are required only in certain circumstances.  The parameters and their use should be
obvious by reading the documentation for #OPTION, #MULT_USE_REQ_OPTION and #COND_REQ_OPTION
*/					       	
#define MULT_USE_COND_REQ_OPTION(ENGLISH_NAME,USAGE_COUNTER,SHORT,LONG,ARGS,SHORT_COMMENT,LONG_COMMENT, MAX_USES, WHEN_REQUIRED, CODEPEND_STATEMENT) \
                           opt_snl(&USAGE_COUNTER, argv[__Opt_i],"-"#SHORT,"--"#LONG,#ARGS,#SHORT_COMMENT,#LONG_COMMENT,\
					       __OptCommentLevel,&__OptRecog, __OptCurrLong, __OptCurrShort, __OptSeekUnusedRequiredOpts, \
					       	&__OptError, __OptEND_OF_OPTS ,__OptLEN_OF_ARGS, 2 , WHEN_REQUIRED , #CODEPEND_STATEMENT , MAX_USES, 0,0,"",__OptCurrentIndentLevel,,#ENGLISH_NAME,1)


/*!
DEPRECATED!
This is a macro used to declare an option that might conflict with another option, i.e. should cause an error when both this option and another one
are issued on the same command line.   It turns out, however, that it seems easier to just catch those sorts of conflicts by testing for the presence of 
the other option using the #HAS_BUT_SHOULD_NOT macro.
*/
#define CLASHABLE_OPTION(ENGLISH_NAME,USAGE_COUNTER,SHORT,LONG,ARGS,SHORT_COMMENT,LONG_COMMENT, WHEN_CLASHES, CLASH_STATEMENT ) \
                           opt_snl(&USAGE_COUNTER, argv[__Opt_i],"-"#SHORT,"--"#LONG,#ARGS,#SHORT_COMMENT,#LONG_COMMENT,\
					       __OptCommentLevel,&__OptRecog, __OptCurrLong, __OptCurrShort, __OptSeekUnusedRequiredOpts, \
					       	&__OptError, __OptEND_OF_OPTS ,__OptLEN_OF_ARGS, 0, 0, "" , 1 , 1, WHEN_CLASHES, #CLASH_STATEMENT,__OptCurrentIndentLevel,#ENGLISH_NAME,1)


/*! this is for the things like --version and --help that don't involve any further execution of the program or
parsing of the command line, etc.  It differs only by have a 1 for the UsedDuringExecution flag. */
#define NON_EXEC_OPTION(ENGLISH_NAME,USAGE_COUNTER,SHORT,LONG,ARGS,SHORT_COMMENT,LONG_COMMENT) \
                           opt_snl(&USAGE_COUNTER, argv[__Opt_i],"-"#SHORT,"--"#LONG,#ARGS,#SHORT_COMMENT,#LONG_COMMENT,\
					       __OptCommentLevel,&__OptRecog, __OptCurrLong, __OptCurrShort, __OptSeekUnusedRequiredOpts, \
					       	&__OptError, __OptEND_OF_OPTS ,__OptLEN_OF_ARGS, 0, 0, "" , 1 , 0,0,"",__OptCurrentIndentLevel,#ENGLISH_NAME,0)
/*@} */



/*! @name  Macros for lumping options into certain groups or categories */
/*@{*/
/*!
This macro can be used to "open" a block of related options.  After all the related options have been declared with #OPTION or #REQUIRED_OPTION, etc.
then the block should be closed with a #CLOSE_SUBSET macro.
\param SubName The name to be given to the block of options being opened with this #OPEN_SUBSET in the --help, --help-full, and --help-nroff
output modes.  This is an \b unquoted string.  For example: <em>Output-Controlling Options</em>.  In those output modes it gets printed as: 
"****** SubName ******".
\param XMLName  The name to be given to the block of options being opened with this #OPEN_SUBSET in the --help-xml
output modes.  This is an \b unquoted string.  For example: <em>Output-Controlling Options</em>.  XMLName is thus the name of the subset of options
that will appear in GuiLiner output.
\param Desc This is an unquoted string that describes the subset of parameters for (eventual?) use in GuiLiner.
*/
#define OPEN_SUBSET(SubName,XMLName,Desc)  OpenSubset(#SubName,#XMLName,#Desc,&__OptCurrentIndentLevel,__OptCommentLevel);

/*!
Place this macro at the end of a subset of options opened with the #OPEN_SUBSET macro.
*/				       	
#define CLOSE_SUBSET  CloseSubset(&__OptCurrentIndentLevel,__OptCommentLevel);	
/*@}*/



/*! @name Macros for enumerating or testing the number or arguments preceding the next option */
/*@{*/
/*!
This macro returns the number of arguments present on the command line between the current position (always given 
by the __Opt_i variable, that you should never have to directly access) and the next option flag (i.e. -a or --boing) or
the end of the command line. 
*/
#define COUNT_ARGS args_to_next_opt(__Opt_i,argc,argv)

/*!
This macro tests whether the number of remaining arguments on the command line until the next option flag
or the end of the command line is \e N.  If it is \e N it evaluates to 1.  If not, it evaluates to 0,
issues an error statement to stderr saying that the current option has the wrong number of arguments, and 
then it sets the __OptError error variable to 1 (this will cause the program to exit after it finished parsing
through the rest of the command line.  It also chews through all the non-option arguments on the command
line to get to the next option flag.
*/
#define ARGS_EQ(N) args_eq(N,&__Opt_i,argc,argv,__OptCurrShort,__OptCurrLong,&__OptError)

/*! This macro tests whether the number of remaining arguments on the command line until the next option flag
or the end of the command line is greater than \e N.  Otherwise it behaves like #ARGS_EQ.
*/
#define ARGS_GT(N) args_gt(N,&__Opt_i,argc,argv,__OptCurrShort,__OptCurrLong,&__OptError)

/*! This macro tests whether the number of remaining arguments on the command line until the next option flag
or the end of the command line is less than \e N.  Otherwise it behaves like #ARGS_EQ.
*/
#define ARGS_LT(N) args_lt(N,&__Opt_i,argc,argv,__OptCurrShort,__OptCurrLong,&__OptError)

/*! This macro tests whether the number of remaining arguments on the command line until the next option flag
or the end of the command line is greater than or equal to \e N.  Otherwise it behaves like #ARGS_EQ.
*/
#define ARGS_GEQ(N) args_geq(N,&__Opt_i,argc,argv,__OptCurrShort,__OptCurrLong,&__OptError)

/*! This macro tests whether the number of remaining arguments on the command line until the next option flag
or the end of the command line is less than or equal to \e N.  Otherwise it behaves like #ARGS_EQ.
*/
#define ARGS_LEQ(N) args_leq(N,&__Opt_i,argc,argv,__OptCurrShort,__OptCurrLong,&__OptError)
/*@}*/


/*! @name Marcros For Testing The Occurrence of Another Option and for Noting Errors */
/*@{*/
/*!
This macro retuns a 1 if the option whose USAGE_COUNTER is given in the Flag argument has already been issued.  If not,
it returns a 0, 
__OptError is set to 1, and an error message is printed to stderr with this format:
<br><br>
"Error Processing Option xxxx!   The option xxxx requires establishing option Option first\n"
\param Flag The USAGE_COUNTER (see #OPTION) for the option whose invocation status you wish to test.
\param Option An unquoted string that should give the tag for option you wish to test, like "-a/--all-files" or
whatever it might be.
*/
#define ALREADY_HAS(Flag, Option)  already_has(Flag, #Option, __OptCurrShort, __OptCurrLong, &__OptError)

/*!
This macro retuns a 0 if the option whose USAGE_COUNTER is given in the Flag argument has not already been issued.  If it has,
already been issued, then it returns a 1, 
__OptError is set to 1, and an error message is printed to stderr with this format:
<br><br>
"Error Processing Option xxxx!    The option xxxx should not be called subsequent to establishing option Option\n"
\param Flag The USAGE_COUNTER (see #OPTION) for the option whose invocation status you wish to test.
\param Option An unquoted string that should give the tag for option you wish to test, like "-a/--all-files" or
whatever it might be.
*/
#define HAS_BUT_SHOULD_NOT(Flag, Option)  has_but_should_not(Flag, #Option, __OptCurrShort, __OptCurrLong, &__OptError)


/*!
This macro merely sets ECA_Opts interval error flag to 1, meaning that there has been an error.  Typical behavior is for
ECA_Opt to continue processing options until the end of the command line, and, after that, exit with status 1.  This allows
errors in the later part of the command line to show up as errors, too.  Of course, it doesn't always work so well.
*/
#define OPT_ERROR __OptError = 1;
/*@}*/



/*! @name Marcros For Extracting Option Arguments off the Command Line */
/*@{*/
/*!
This macro is used to collect the next argument off the command line, that is expected to be an integer.  It gets it off of argv and then aoi's it whilst
also incrementing the counter that tells you how far along the command line you have gotten so far (__Opt_i).  It returns this integer.  Then it checks to make
sure that it was really an integer and not some weird string that got converted into a wacky integer.  This uses the function string_is_good_int() which seems to
cause problems on Linux.  Must fix that.  <b>This macro assumes that the array of strings in which the command line resides is named argv!! So, in practice, you 
should not rename argv, or call it something else within your function where the actual command line gets parsed.</b>
*/

/*  For some reason, this fails on linux---doesn't know a good int when it sees it, so I will just remove it. 
	#define GET_INT atoi(argv[++__Opt_i]); if(!string_is_good_int(argv[__Opt_i])) { char tempstr[MAX_INTEGER_ARG_LENGTH]; \
			option_name(tempstr,__OptCurrShort, __OptCurrLong); \
			fprintf(stderr,"Error Processing Option %s!    Argument \"%s\" to option %s is not a valid integer\n",tempstr, argv[__Opt_i],tempstr); OPT_ERROR; }
*/
#define GET_INT atoi(argv[++__Opt_i]);
 
/*!
Use this macro to collect the next string off the command line and convert it to a real number and return it.  It increments the counter that keeps track of where 
in argv you are (that counter is __Opt_i).  <b>This macro assumes that the array of strings in which the command line resides is named argv!! So, in practice, you 
should not rename argv, or call it something else within your function where the actual command line gets parsed.</b>
*/
#define GET_DUB atof(argv[++__Opt_i]);

/*!
This macro is used to collect the next string off the command line (off of argv) and copy it to the array of chars X.  Note that <em>you must allocate
memory to X before you include it as a parameter of this macro</em>.  It gets it and copies it and increments the counter that tells you how far along 
argv you've gone.  <b>This macro assumes that the array of strings in which the command line resides is named argv!! So, in practice, you 
should not rename argv, or call it something else within your function where the actual command line gets parsed.</b>
*/ 
#define GET_STR(X) sprintf(X,"%s",argv[++__Opt_i]);
/*@}*/



/*! @name Macros That Help In Providing Information About the Program */
/*@{*/	
/*!
This sets the width (in number of characters) given for printing the names of the options when using the --help option.
\param X The width in number of characters.  This has to be set long if you have really long option names, like:
--a-really-silly-option-name-that-is-too-long.
*/
#define SET_OPT_WIDTH(X) __OptEND_OF_OPTS = X;

/*!
This sets the width (in number of characters) given for printing the list of arguments for the argument lists when
using the --help option.
\param X The width in number of characters.
*/
#define SET_ARG_WIDTH(X) __OptLEN_OF_ARGS = X;

/*!
Macro for setting the program name.  
\param  X This is a \b quoted string that gives your program's name.
*/
#define SET_PROGRAM_NAME(X) sprintf(__OptProgName,X);

/*!
Macro for setting the brief program description.  
\param  X This is a \b quoted string that gives a brief (half sentence, typically) description of your program.
*/
#define SET_PROGRAM_SHORT_DESCRIPTION(X) sprintf(__OptProgShortDescription,X);

/*!
Macro for setting the long program description.  
\param  X This is a <b>UN</b>quoted string that gives a long  description of your program.  Recall you have to use octal codes for commas in this
situation.  See documentation for #OPTION for info on writing text in these sorts of unquoted strings.
*/
#define SET_PROGRAM_LONG_DESCRIPTION(X) sprintf(__OptProgLongDescription,#X);

/*!
Macro for setting the program author's name.  
\param  X This is a \b quoted string that gives the name of the author.
*/
#define SET_PROGRAM_AUTHOR_STRING(X) sprintf(__OptProgAuthorString,X);

/*!
Macro for setting the program version.  
\param  X This is a \b quoted string that gives the version of the program.
*/
#define SET_VERSION(X) sprintf(__OptVersionString,X);

/*!
Macro for setting the program version.  
\param  X This is a \b quoted string that lets you record the history of earlier versions of the program, if desired.
*/
#define SET_VERSION_HISTORY(X) sprintf(__OptVersionsHistoryString,X);
/*@}*/





/*! @name Some Ridiculously Long Macros Where All The Action in ECA_Opt Gets Implemented "Behind The Scenes" */
/*@{*/	

/*!
	This macro is used to easily declare all the variables that ECA_Opt will be using while processing the command line and to
	initialize some of those variables.  For the most part, these should all be "hidden" variables.  If you are using ECA_Opt
	you should not have to ever directly access any of these variables.  Note that these variables are always named so that they
	start with "__Opt".  You should try not to declare any of your own variables named that way within the scope where you use
	this macro.
*/
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
	int __OptCurrentIndentLevel = 0; \
	int __OptQuietCommandFileInsertion = 1; \
	char __OptVersionString[2500]; \
	char __OptVersionsHistoryString[10000];  \
	char __OptProgName[250]; \
	char __OptProgShortDescription[2500]; \
	char __OptProgAuthorString[2500];  \
	char *__OptProgLongDescription;  \
	char *__OptTempStr; \
	int __OptVersionHistoryFlag = 0; \
	sprintf(__OptVersionString,"Developer! Please Supply Version Information with SET_VERSION macro\n\n");  \
	sprintf(__OptVersionsHistoryString,"Developer! Please Supply Version History with SET_VERSION_HISTORY macro\n\n");  \
	sprintf(__OptProgName,"Please Supply Program Name with SET_PROGRAM_NAME macro\n\n");  \
	sprintf(__OptProgShortDescription,"Please Supply Short Description with SET_PROGRAM_SHORT_DESCRIPTION macro\n\n"); \
	sprintf(__OptProgAuthorString,"\n\n  --- Please Supply Author String with SET_PROGRAM_AUTHOR_STRING macro --- \n\n");  \
	__OptProgLongDescription = (char *)calloc(10000, sizeof(char));  \
	sprintf(__OptProgLongDescription,"--- Developer! Please Supply Long Description with SET_PROGRAM_LONG_DESCRIPTION macro ---"); 

	

/*!
	This macro contains a collection of code that gets called at the end of the ECA_Opt loop.  
	The basic purpose of this code is to check to see if the program has encountered any options
	it does not recognize and to make sure that all required options have been issued by the user.
	
	See the tutorial for how this macro is to be used.
*/
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
				if(__OptCommentLevel==4) { \
					PrintIndent(1);  \
					printf("</options>\n");\
					printf("</guiLiner>\n"); \
				}	\
				break; \
			} \
	}  \
	if(__OptError == 1) {fprintf(stderr,"\nErrors encountered processing command line.  Use\n\t%s --help\nor\n\t%s --help-full\nfor information on available options\n", progname(argv),progname(argv));  exit(1);} \
	if(__OptCommentLevel==3) { printf(".\\\" done with program-generated options part\n"); } \
	if(__OptCommentLevel > 0 || __OptVersionFlag > 1) {exit(0);} \

	

/*!
	This macro defines a bunch of code that, in some sense, does most of the work in ECA_Opt.  It is a heinous and nasty bit of code that would
	earn any undergrad in computer science a solid F in their introductory programming course...but, it has been working well for me for quite some time.
	
	The main things it is doing are:
	- Setting up a loop with __Opt_i over the elements of argv
	- Establishing the options --help, --help-full, --help-nroff,  --help-xml or any of the --version options and --command-file, etc.
	- Establishing code which executes when the --help, --help-full, --help-nroff, or --help-xml or any of the --version options or the --command-file option are given.  It is in the implementation
	of the printing of some of the information in the --help routines that the truly disgusting, twisted, lam-o nature of this code is revealed.  But, then again,
	it seems to work OK.
*/
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
		if(  NON_EXEC_OPTION (Brief Help, \
						__OptHelpFlag, , \
						help,  \
						,  \
						short listing of options,  \
			 			this returns a short list of all program options and associated arguments) )  \
		 {  \
		   	__OptCommentLevel = 1;   \
		   	printf("%s  --  %s\n\n",__OptProgName,__OptProgShortDescription);  \
/*		   	printf("\n   ****  Program-description and documentation  parameters  ****\n\n"); \  */ \
		   	/* And now this part is a total hack to get the --help line to print out right */  \
		   	NON_EXEC_OPTION (Brief Help, __OptHelpFlag, , \
						help, \
						,  \
						short listing of options,  \
			 			this returns a short list of all program options and associated arguments);  \
		 }  \
		/* long help option */  \
		if(  NON_EXEC_OPTION (Long Help, __OptHelpFullFlag , ,  \
						help-full,  \
						,  \
						long listing of options,  \
			 			this returns a full list of all program options and associated arguments) )  \
		 {  \
		   	__OptCommentLevel = 2;  \
		   	printf("%s  --  %s\n\n",__OptProgName,__OptProgShortDescription);  \
			printf("Author(s):\n\t%s\n\n",__OptProgAuthorString); \
			printf("About the Program:\n"); \
			print_spec_indented_and_max_length(__OptProgLongDescription,80,2,""); \
		   	printf("\nIn the following:\n"); \
		   	PrintExpectedArgListInfo(__OptCommentLevel); \
		   	printf("\n   ****  Program-description and documentation  parameters  ****\n\n"); \
		   	/* And now this part is a total hack to get the --help line to print out right */  \
		   	NON_EXEC_OPTION (Brief Help, __OptHelpFlag , ,  \
						help, \
						,  \
						short listing of options,  \
			 			this returns a short list of all program options and associated arguments);  \
		   	NON_EXEC_OPTION (Long Help, __OptHelpFullFlag , ,  \
						help-full,   \
						,  \
						long listing of options,  \
			 			this returns a full list of all program options and associated arguments);  \
		 } \
		 /* nroff help option */  \
		if(  NON_EXEC_OPTION (Help In Nroff Format, __OptHelpNroffFlag , ,  \
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
		   	printf(".SH NAME\n%s  --  %s\n",__OptProgName,__OptProgShortDescription); \
			printf(".SH AUTHOR(S)\n%s\n",__OptProgAuthorString); \
			printf(".SH ABOUT THE PROGRAM\n%s",__OptProgLongDescription); \
		   	printf(".\\\" starting program-generated options part\n"); \
		   	printf(".SH OPTIONS\n.PP\n"); \
		   	printf("In the following:\n\n"); \
			printf(".IR J \"  refers to an integer argument to an option\"\n\n");  \
		   	printf(".IR R \"  refers to a real number argument to an option\"\n\n");  \
		   	printf(".IR S \"  refers to a string argument to an option\"\n\n");  \
			printf(".IR F \"  refers to a file path argument to an option. For example,\"\n");  \
			printf("\"datfile.txt\" if the file is in the current working directory, or\n"); \
			printf("something like \"~/eriq/Documents/data/datfile.txt\" if you want to\n");  \
			printf("provide a complete file path.  (Beware of spaces in file paths!)\n\n");  \
			printf(".IR D \"  refers to a directory path argument to an option. For example,\"\n");  \
			printf("\"data_direcory/\" if the directory is in the current working directory, or\n"); \
			printf("something like \"~/eriq/Documents/data_directory/\" if you want to\n");  \
			printf("provide a complete directory path.  Note that the trailing slash should be\n");  \
			printf("optional, but currently is not.  (ERIC ADD MACROS FOR GETTING FILES AND DIRECTORIES)\n\n");  \
		   	printf(".IR G \"  refers to a string that gives a (possibly) discontinous range of\"\n");  \
		   	printf("nonnegative integers.  For example:  \"1-5,7,9,10-15\" specifies\n");  \
		   	printf("the integers 1 through 5, 7, 9, and 10 through 15.  There can be no\n");  \
		   	printf("whitespace in the string specifying the range, and the numbers must all\n");  \
		   	printf("be increasing.  Also, the string cannot start nor end with a comma or a dash.\n");  \
		   	printf("Finally, you should not use \"-\" to denote two ranges without placing any\n");  \
		   	printf("commas in between.\n\n");  \
			printf(".IR C \"  refers to a constrained string argument to an option,\"\n"); \
			printf("i.e., the argument is a string that may only be drawn from a small\n");  \
			printf("set of alternatives, as specified in the help-full description.\n\n");  \
		   	printf("\n   ****  Program-description and documentation  parameters  ****\n.PP\n"); \
		   	/* And now this part is a total hack to get the --help line to print out right */  \
		   	NON_EXEC_OPTION (Brief Help, __OptHelpFlag , ,  \
						help, \
						,  \
						short listing of options,  \
			 			this returns a short list of all program options and associated arguments);  \
		   	NON_EXEC_OPTION (Long Help, __OptHelpFullFlag , ,  \
						help-full,   \
						,  \
						long listing of options,  \
			 			this returns a full list of all program options and associated arguments);  \
			NON_EXEC_OPTION (Help in Nroff Format, __OptHelpNroffFlag , ,  \
						help-nroff,  \
						,  \
						long listing of options in nroff man-page format,  \
			 			this returns a full list of all program options and associated arguments \
			 			using the formatting styles in nroff that give you the look of a man page.  \
			 		     View the formatted ouput by doing: 'prog --help-nroff | nroff -man | more' where prog is the name of the program. );   \
		 } \
		 /* spits it out to XML */ \
		 if(NON_EXEC_OPTION (Help In XML/Guiliner Format, __OptHelpXMLFlag , ,  \
						help-xml,  \
						,  \
						output all options in XML format,  \
			 			This returns a list of all options in a simple XML format which is \
						suitable for input to the guiLiner front end.))  \
		{     \
			__OptCommentLevel = 4; \
			printf("<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?>\n");\
			printf("<!DOCTYPE guiLiner SYSTEM \"guiliner.dtd\">\n");\
			printf("<!--A GuiLiner specification for %s-->\n",__OptProgName);  \
			printf("<guiLiner>\n");\
			printf("<program>\n"); \
			PrintIndent(1);\
			printf("<binary_name>%s</binary_name>\n",__OptProgName);     \
			PrintIndent(1);\
			printf("<binary_location></binary_location>\n");     \
			PrintIndent(1);\
			printf("<binary_html_manual_index></binary_html_manual_index>\n");     \
			PrintIndent(1);\
			printf("<binary_short_info>%s</binary_short_info>\n",__OptProgShortDescription);     \
			PrintIndent(1);\
			printf("<binary_long_info>\n");\
			PrintIndent(2);\
			printf("<![CDATA[\n");  \
			__OptTempStr = ConvertWhiteSpaceToHTML_Friendly(__OptProgLongDescription, 5);\
			print_spec_indented_and_max_length(__OptTempStr,80,4,""); \
			free(__OptTempStr);\
			PrintIndent(2);\
			printf("]]>\n");\
			PrintIndent(1); \
			printf("</binary_long_info>\n");\
			PrintIndent(1);\
			printf("<gL_window_size>800</gL_window_size>\n"); \
			PrintIndent(1);\
			printf("<look_feel>java</look_feel> <!-- MAY BE 'SYSTEM' OR 'JAVA' -->\n"); \
			PrintIndent(1);\
			printf("<wizard>false</wizard>\n");  \
			printf("</program>\n");  \
			printf("<options>\n"); \
			__OptCurrentIndentLevel = 1; \
		} \
		 /* version option */ \
		if(  NON_EXEC_OPTION (Version,__OptVersionFlag, , \
						version,  \
						,  \
						prints program version information,  \
			 			) )  \
		 {  \
		 	printf("%s -- %s\n",__OptProgName,__OptVersionString); \
		 	exit(0);  \
		 }  \
		 /* version history option */  \
		 if(  NON_EXEC_OPTION (Version History, __OptVersionHistoryFlag, , \
						version-history,  \
						,  \
						prints history of different versions,  \
			 			) )  \
		 {  \
		 	printf("%s\n",__OptVersionsHistoryString); \
		 	exit(0);  \
		 }  \
		 if(  MULT_USE_OPTION (Command File, __OptCommFileFlag, , \
						command-file,  \
						F,  \
						inserts contents of file F into the command line,  \
						By using this option you can store command line arguments in the  \
						file named in F.  You may have any kind of white space (including   \
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
			 		insert_file_into_commline(&argv,&argc,&__Opt_i,temp_filename, __OptCommFileFlag);  \
			 		if(!__OptQuietCommandFileInsertion) { \
						printf("INSERTING_COMMANDS_FROM_FILE : %s\n",temp_filename);  \
						printf("NEW_COMMAND_LINE_LOOKS_LIKE : "); \
						for(__Opt_j=0;__Opt_j<argc;__Opt_j++)  {  \
							printf("%s ",argv[__Opt_j]);  \
						}  \
						printf("\n"); \
					}  \
		 	} \
		 }  \
		 
/*@}*/



/* prototypes */
int string_is_good_int(const char *s);
void print_indented_and_max_length(const char *str, int len);
void print_spec_indented_and_max_length(const char *str, int len, int Num, const char *AddToEnd);
void PrintIndent(int Num);
void eca_opt_print_option_xml(const char *shortopt,const char *longopt,const char *args,const char *short_comment,
			const char *long_comment, int __END_OF_OPTS, int indent_level, int IsRequired,int MaxUsages, const char *OptionName,
			int UsedDuringExecution);
void eca_opt_print_usage_long(const char *shortopt,const char *longopt,
			const char *args,const char *comment, int __END_OF_OPTS);
void eca_opt_print_usage_long_nroff(const char *shortopt,const char *longopt,const char *args,const char *comment, int __END_OF_OPTS);
void eca_opt_print_usage_short(const char *shortopt,const char *longopt,const char *args,
		const char *comment,int __END_OF_OPTS, int __LEN_OF_ARGS);
void option_name(char *tempStr, char *shortopt, char *longopt);
int opt_snl(int *how_many_times,  /*!< how many times this option has been used so far---requires that a flag
									be made for each option */
			char *argv,   /*!< the argument  array */
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
			const char* ClashString, /* the name of the conflicting option(s) */
			int CurrentIndexLevel,
			const char *OptionName,  /* for an "english name" that the option will be given */
			int UsedDuringExecution  /* this is 1 if the option is something that is used during program execution, and 
										a zero if it is just an info thing like --help, or --version-history.  If 1 then it
										gets a true in guiLiner's <option_can_appear_in_command> field. if zero then false. */
 ); 
int is_opt(char *a);
int args_to_next_opt(int __Opt_i,int argc, char **argv);
int args_eq(int Num, int *__Opt_i, int argc, char **argv, char *shortopt, char *longopt, int *Error );
int already_has(int Flag, char *str, char *CurrShort, char *CurrLong, int *Error);
int has_but_should_not(int Flag, char *str, char *CurrShort, char *CurrLong, int *Error);
char *progname(char *argv[]);
char **file2charstarstar(char *FileName, int *N);
void insert_file_into_commline(char ***argv, int *argc, int *__Opt_i, char *FileName, int CommFileCounter);
int args_gt(int Num, int *__Opt_i, int argc, char **argv, char *shortopt, char *longopt, int *Error );
int eat_comments(FILE *in, char delim);
int args_geq(int Num, int *__Opt_i, int argc, char **argv, char *shortopt, char *longopt, int *Error );
int args_lt(int Num, int *__Opt_i, int argc, char **argv, char *shortopt, char *longopt, int *Error );
int args_leq(int Num, int *__Opt_i, int argc, char **argv, char *shortopt, char *longopt, int *Error );
void OpenSubset(const char *SubName,  const char *xml_name, const char *SubDesc, int *IndentLevel, int CommentLevel);
void CloseSubset(int *IndentLevel, int CommentLevel);
void PrintExpectedArgListInfo(int CommentLevel);
void PrintExpectedArgListElementsXML(int *ArgToksUsed);
char *ConvertWhiteSpaceToHTML_Friendly(const char *S, int TabStops);

/* prototypes for ArgList processing */
ArgPanelType SimpleProcArgs(const char *args, char *choices, char *choices_free, int *ArgToksUsed, int GiveXML_Warnings);
