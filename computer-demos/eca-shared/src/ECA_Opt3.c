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



#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "ECA_Opt3.h"



/*! @name String-operating functions

These are things that munch strings
*/
/*@{*/

/*! This function takes as input the string S and converts the line endings in S to html linebreaks and it converts the tabs to 
&nbsp's (TabStops of them---i.e. TabStops gives the number of spaces to a convert a tab character into.)  

It allocates memory to a new string and returns it
*/
char *ConvertWhiteSpaceToHTML_Friendly(const char *S, int TabStops)
{
	int i,j,k=0,len;
	char *ret;
	int newlen;
	int factor;
	
	len=strlen(S);
	
	/* allocate enough memory to turn a whole string of \n's or \t's into their
		equivalent */
	if(TabStops*5>4) {
		factor = TabStops*5;
	}
	else {
		factor = 4;
	}
	newlen = len *  factor;
	ret = (char *)calloc(len,sizeof(char));
	
	for(i=0;i<len;i++)  {
		if(S[i]=='\t')  {
			for(j=0;j<TabStops;j++)  {
				ret[k++]='&';
				ret[k++]='n';
				ret[k++]='b';
				ret[k++]='s';
				ret[k++]='p';
			}
			ret[k++]=' ';
		}
		else if(S[i]=='\n') {
			ret[k++]='<';
			ret[k++]='b';
			ret[k++]='r';
			ret[k++]='>';
		}
		else {
			ret[k++]=S[i];
		}
		
	}
	ret[k++]='\0';
	
	return(ret);
	
}

void PrintExpectedArgListElementsXML(int *ArgToksUsed)
{
	int i;
	int PrintIt=0;
	
	/* check to see if any non-unknown args have been used */
	for(i=1;i<NUM_ARG_TOK_TYPES;i++)  {
		PrintIt+=ArgToksUsed[i];
	}
	
	if(PrintIt==0) {
		return;
	}
	
	printf("\n<br><br><hr><br>The elements of the expected format string are described by their first letter as follows:<br>\n");
	printf("\n<DL>");
	
	if(ArgToksUsed[__J_tok]) {
		printf("\n\t<DT>J</DT>");
		printf("\n\t<DD>refers to an integer argument to an option.  For example:  15</DD>");
	}
	
	if(ArgToksUsed[__R_tok]) {
		printf("\n\t<DT>R</DT>");
		printf("\n\t<DD>refers to a real number argument to an option. For example:  2.3</DD>");
	}
	
	if(ArgToksUsed[__S_tok]) {
		printf("\n\t<DT>S</DT>");
		printf("\n\t<DD>refers to a string argument to an option. For example: boing</DD>");
	}

	if(ArgToksUsed[__F_tok]) {
		printf("\n\t<DT>F</DT>");
		printf("\n\t<DD>refers to a file path argument to an option. For example:  /Users/eriq/myfile.txt</DD>");
	}

	if(ArgToksUsed[__D_tok]) {
		printf("\n\t<DT>D</DT>");
		printf("\n\t<DD>refers to a directory path argument to an option, typically with the trailing slash.  For example:  /Users/eriq/data_directory/</DD>");
	}
		
	if(ArgToksUsed[__G_tok]) {
		printf("\n\t<DT>G</DT>");
		printf("\n\t<DD>refers to a string that gives a (possibly) discontinous range of  \
                nonnegative integers.  For example:  1-5,7,9,10-15 specifies  \
                the integers 1 through 5, 7, 9, and 10 through 15.  There can be no  \
                whitespace in the string specifying the range, and the numbers must all \
                be increasing.  Also, the string cannot start nor end with a comma or a dash. \
                Finally, you should not use - to denote two ranges without placing any \
                commas in between.</DD>");
	}

	if(ArgToksUsed[__C_tok]) {
		printf("\n\t<DT>C</DT>");
		printf("\n\t<DD>refers to a \"constrained\" string argument to an option, \
			i.e., the argument is a string that may only be drawn from a small \
			set of alternatives, which are usually explained above.</DD>");
	}
	printf("\n");

}


/* prints information about how the expected argument lists are formatted.
This has a bunch of ugly stuff to ensure that we can get moderately well formatted
lines in html/xml output. */
void PrintExpectedArgListInfo(int CommentLevel) 
{
	char hardtab[200];  /* one that we definitely want in the xml output */
	char twolinebreak[200];  /* double line breaks that we definitely want in the xml output */
	char tab[200];
	char softlinebreak[200];  /* a line break we only want in the --help-full output */


	if(CommentLevel != 4) {
		sprintf(tab,"\t");
		sprintf(twolinebreak,"\n\n");
		sprintf(hardtab,"\t");
		sprintf(softlinebreak,"\n");
	}
	else if(CommentLevel==4)  {
		sprintf(tab," ");
		sprintf(twolinebreak,"\n<br>\n<br>");
		sprintf(hardtab,"&nbsp&nbsp&nbsp&nbsp&nbsp");
		sprintf(softlinebreak,"\n");
	}
	
	printf("%s",hardtab);
	printf("\"J\" refers to an integer argument to an option"); 
	printf("%s",twolinebreak);
	
	
	printf("%s",hardtab);
	printf("\"R\" refers to a real number argument to an option");
	printf("%s",twolinebreak);
	
	
	
	printf("%s",hardtab);
	printf("\"S\" refers to a string argument to an option");  
	printf("%s",twolinebreak);
	
	
	
	printf("%s",hardtab);
	printf("\"F\" refers to a file path argument to an option. For example,");  
	printf("%s",softlinebreak);
	
	
	
	
	printf("%s",tab);
	printf("%s",tab);
	printf("\"datfile.txt\" if the file is in the current working directory, or"); 
	printf("%s",softlinebreak);
	
	
	
	printf("%s",tab);
	printf("%s",tab);
	printf("something like \"~/eriq/Documents/data/datfile.txt\" if you want to");
	printf("%s",softlinebreak);
	
	    
	
	
	printf("%s",tab);
	printf("%s",tab);
	printf("provide a complete file path.  (Beware of spaces in file paths!)");  
	printf("%s",twolinebreak);
	
	
	
	printf("%s",hardtab);
	printf("\"D\" refers to a directory path argument to an option. For example,");  
	printf("%s",softlinebreak);
	
	
	
	printf("%s",tab);
	printf("%s",tab);
	printf("\"data_direcory/\" if the directory is in the current working directory, or"); 
	printf("%s",softlinebreak);
	
	
	
	
	printf("%s",tab);
	printf("%s",tab);
	printf("something like \"~/eriq/Documents/data_directory/\" if you want to");  
	printf("%s",softlinebreak);
	
	
	printf("%s",tab);
	printf("%s",tab);
	printf("provide a complete directory path.  Note that the trailing slash should be");  
	printf("%s",softlinebreak);
	
	
	
	printf("%s",tab);
	printf("%s",tab);
	printf("optional, but currently is not.  (ERIC ADD MACROS FOR GETTING FILES AND DIRECTORIES");  
	printf("%s",twolinebreak);
	
	
	
	
	printf("%s",hardtab);
	printf("\"G\" refers to a string that gives a (possibly) discontinous range of");
	printf("%s",softlinebreak);  
	
	
	
	printf("%s",tab);
	printf("%s",tab);
	printf("nonnegative integers.  For example:  \"1-5,7,9,10-15\" specifies");  
	printf("%s",softlinebreak);
	
	
	printf("%s",tab);
	printf("%s",tab);
	printf("the integers 1 through 5, 7, 9, and 10 through 15.  There can be no");  
	printf("%s",softlinebreak);
	
	
	
	
	printf("%s",tab);
	printf("%s",tab);
	printf("whitespace in the string specifying the range, and the numbers must all");  
	printf("%s",softlinebreak);
	
	
	
	
	printf("%s",tab);
	printf("%s",tab);
	printf("be increasing.  Also, the string cannot start nor end with a comma or a dash.");  
	printf("%s",softlinebreak);
	
	
	
	
	printf("%s",tab);
	printf("%s",tab);
	printf("Finally, you should not use \"-\" to denote two ranges without placing any");  
	printf("%s",softlinebreak);
	
	
	
	printf("%s",tab);
	printf("%s",tab);
	printf("commas in between.");  
	printf("%s",twolinebreak);
	
	
	
	
	printf("%s",hardtab);
	printf("\"C\" refers to a \"constrained\" string argument to an option,"); 
	printf("%s",softlinebreak);
	
	
	
	printf("%s",tab);
	printf("%s",tab);
	printf("i.e., the argument is a string that may only be drawn from a small");  
	printf("%s",softlinebreak);
	
	
	
	printf("%s",tab);
	printf("%s",tab);
	printf("set of alternatives, as specified in the help-full description."); 
	printf("%s",softlinebreak);
	
	
}


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

/*@}*/

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
void print_spec_indented_and_max_length(const char *str, int len, int Num, const char *AddToEnd)
{
	int notfirsttime = 0;
	int slen = strlen(str);
	int lo=0, hi=0;
	int i;
	int NumRet=0,k;

	
	while(hi<slen) {
		hi = lo+len;

		/* if we are at the end of the string, deal with that */
		if(hi>=slen) {
			for(i=1;i<Num;i++)  {
				printf("    ");
			}
			for(lo+=notfirsttime;lo<slen;lo++)
				printf("%c",str[lo]);
			printf("%s\n",AddToEnd);
			return;
		}

		/* otherwise, work backward from hi, till you hit some white space */
		while( !(isspace(str[hi])) && hi>lo)  hi--;
		if(hi==lo) {
			/* in this case, we have to go forward till we hit white space */
			hi = lo + len;
			while( !(isspace(str[hi])) && hi<slen)  hi++;
		}
		
		
		for(i=1;i<Num;i++)  {
			printf("    ");
		}
		for(lo+=notfirsttime;lo<hi;lo++) {
			printf("%c",str[lo]);
			if(str[lo]=='\n') {
				/* seek forward to count the number of \n's */
				for(NumRet=0,k=lo;str[k]=='\n';k++) {
					NumRet++;
				}
				for(i=0;i<NumRet-2;i++) {
					printf("\n");
				}
				for(i=1;i<Num;i++)  {
					printf("    ");
				}
				lo=k-2;
				hi-=lo;
			}
		}
		printf("\n");

		notfirsttime = 1;
	}
}



void eca_opt_print_usage_long(const char *shortopt,const char *longopt,const char *args,const char *comment, int __END_OF_OPTS)
{
	int i;
	char tempStr[1000];
	char ChoiceElements[500];  /* for the choices associated with the arg list */
	char ChoiceLessString[500];
	int ArgToksUsed[NUM_ARG_TOK_TYPES];

	
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
	if(strcmp(args,"")!=0) {
		SimpleProcArgs(args, ChoiceElements, ChoiceLessString,ArgToksUsed,0);
		printf("%s",ChoiceLessString);
	}
	printf("\n");
	
	print_indented_and_max_length(comment, 70);
	printf("\n");


}

void eca_opt_print_usage_long_nroff(const char *shortopt,const char *longopt,const char *args,const char *comment, int __END_OF_OPTS)
{
	char tempStr[1000];
	char NRC[55]; 
	char ChoiceElements[500];  /* for the choices associated with the arg list */
	char ChoiceLessString[500];
	int ArgToksUsed[NUM_ARG_TOK_TYPES];
	
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
		printf("\"   ");  
		SimpleProcArgs(args, ChoiceElements, ChoiceLessString,ArgToksUsed,0);
		printf("%s",ChoiceLessString);
		printf("\"");
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


/*  
	This is a simple function that takes the arg string and separates 
	the brace-delimited choices into "choices" and the remainder
	into "choices_free".

	choices and choices_free should have memory allocated to them elsewhere.
	
	 It then returns an integer which signifies the result of the operation
	 and this tells us what kind of panel to put the results into, which is
	 useful for doing the xml output.
	 
	 I need to code the return values up as enums with self-explanatory names.
	 
	 This also fills an array of the number of times different types of tokens show up 
	 in the format string.  These tokens are defined in the ArgToks enum.
	 

	GiveXML_Warnings is a flag that tells whether it should warn about things that only have to
	do with the XML output.  So, it should be 1 if this is being called by eca_opt_print_option_xml
	and 0 otherwise.
*/
ArgPanelType SimpleProcArgs(const char *args, char *choices, char *choices_free, int *ArgToksUsed, int GiveXML_Warnings)
{
	int i,len,j=0,k=0;
	int SeekTok=1; /* 1 means looking for another token */
	int NumToks=0;
	char Tok;
	int HasF=0, HasC=0, HasBraces=0;
	ArgToksEnum TokEnum;
	
	
	/* initialize the ArgToksUsed to 0 */
	for(i=0;i<NUM_ARG_TOK_TYPES;i++)  {
		ArgToksUsed[i] = 0;
	}
	choices[0] ='\0'; /* initialize to the empty string */
	len = strlen(args);
  
	if(len==0) {
		return(0);  /* this means no args */
	}
  
	for(i=0;i<len;i++) {
		if(isspace(args[i])) {
			SeekTok=1;
			choices_free[k++]=args[i];
		}
		else {
			if(SeekTok) {
			Tok=args[i];  /* record the first character of this token */
			NumToks++;
			SeekTok=0;
			
			/* record what the ArgTokEnum is */
			switch(Tok) {
				case('J'):
					TokEnum = __J_tok;
					break;
				case('R'):
					TokEnum = __R_tok;
					break;
				case('S'):
					TokEnum = __S_tok;
					break;
				case('F'):
					TokEnum = __F_tok;
					break;
				case('D'):
					TokEnum = __D_tok;
					break;
				case('G'):
					TokEnum = __G_tok;
					break;
				case('C'):
					TokEnum = __C_tok;
					break;
				default:
					TokEnum = __Unknown_tok;
					break;
			}	
			ArgToksUsed[TokEnum]++;		
			
			if(Tok=='F') {
				HasF=1;
			}
			if(Tok=='C') {
				HasC=1;
			}
			choices_free[k++]=Tok;
			}
			else {
				if(args[i]=='{') {  /* here we hit the beginning of the choices string, which should be semi-colon separated */
					HasBraces=1;
					while(args[++i]!='}') {
						if(i==len) {
							choices[j]='\0';
							fprintf(stderr,"Error! In Arg String \"%s\", there doesn't seem to be a closing brace around the choice elements. Exiting...\n",args);
							exit(1); 
													}
						choices[j++]=args[i];
					} 
					choices[j]='\0';  /* close off the choices string */
					SeekTok=1;
					choices_free[k++]=' ';
				}
				else {
				choices_free[k++]=args[i];
				}
			}
		}
	}
	choices_free[k]='\0';

	if(HasF && NumToks==1) {
		for(i=0;i<NUM_ARG_TOK_TYPES;i++)  {ArgToksUsed[i] = 0;}  /* set these all to zero, so that we don't talk about the args in the xml output (since there aren't any in the gui */
		return(FILE_CHOOSER_PANEL);
	}
	if(HasC && NumToks==1 && HasBraces==1) {
		for(i=0;i<NUM_ARG_TOK_TYPES;i++)  {ArgToksUsed[i] = 0;}  /* set these all to zero, so that we don't talk about the args in the xml output (since there aren't any in the gui */
		return(CHOICE_PANEL);
	}
	if(HasF && HasC) {
		if(GiveXML_Warnings) {fprintf(stderr,"Warning! Arg String \"%s\" requests a File dialog and a Choice panel.  Cannot accomodate both.  They will appear together in a complex panel.\n",args);}
		return(COMPLEX_PANEL);
	}
	if(HasF && NumToks>1) {
		if(GiveXML_Warnings) {fprintf(stderr,"Warning! Arg String \"%s\" requests a File dialog as well as other arguments.  Cannot accomodate all of them.  They will appear together in a complex panel.\n",args);}
		return(COMPLEX_PANEL);
	}
	if(HasC && NumToks>1) {
		if(GiveXML_Warnings) {fprintf(stderr,"Warning! Arg String \"%s\" requests a choice panel as well as other arguments.  Cannot accomodate all of them.  They will appear together in a complex panel.\n",args);}
		return(COMPLEX_PANEL);
	}
	if(HasC && HasBraces==0) {
		if(GiveXML_Warnings) {fprintf(stderr,"Warning! Arg String \"%s\" requests a choice panel but does not list the choices in curly braces. Thus it will just have to appear in a complex panel.\n",args);}
		return(COMPLEX_PANEL);
	}
	return(COMPLEX_PANEL);
}



/* this is in development.  I'd like to print the setup out to some sort of XML format */
void eca_opt_print_option_xml(const char *shortopt,
								const char *longopt,
								const char *args,
								const char *short_comment,
								const char *long_comment, 
								int __END_OF_OPTS, 
								int indent_level,
								int IsRequired,
								int MaxUsages,
								const char *OptionName,
								int UsedDuringExecution)
{

	char TempStr[500];
	char FlagName[500];
	char ChoiceElements[500];  /* for the choices associated with the arg list */
	char ChoiceLessString[500]; /* for the arg list after the the choice elements have been stripped out */
	ArgPanelType PanelType;
	int ArgToksUsed[NUM_ARG_TOK_TYPES];
	int PrintExpectedArgs=0;
	char *TempCharPtr;
	
	  
	
	
	
	/* use the -s style short name (if available) as the FlagName or, use the 
	--long-name if there is no short name available */ 
	if(strcmp(shortopt,"-")!=0) {
		sprintf(FlagName,"%s",shortopt);
	}
	else {
		sprintf(FlagName,"%s",longopt);
	}


	/* figure out what type of panel we are going to use */
	PanelType = SimpleProcArgs(args, ChoiceElements, ChoiceLessString,ArgToksUsed,1);
	
	
	/* if it is a multiple use option, we will bark a warning if it is asking for file selector or choice panel */
	if(MaxUsages > 1) {
		if(PanelType==FILE_CHOOSER_PANEL) {
			fprintf(stderr,"Warning! Arg string \"%s\" for option \"%s\" (\"%s\") should be a FileChooserPanel, but it is a multiple use option and guiLiner does not support that yet!  Using MultiFlagPanel.\n ",args,OptionName,FlagName);
		}
		if(PanelType==CHOICE_PANEL) {
			fprintf(stderr,"Warning! Arg string \"%s\" for option \"%s\" (\"%s\") should be a MultiChoicePanel, but it is a multiple use option and guiLiner does not support that yet!  Using MultiFlagPanel.\n ",args,OptionName,FlagName);
		}
		PanelType = MFLAG_PANEL;
	}
	
	
	
	
	/* first we say that we are entering an option */
	PrintIndent(indent_level);
	printf("<option>\n");
	
	
	
	/* do the option type */
	PrintIndent(indent_level+1);
	printf("<option_type>");
	if(UsedDuringExecution==0) {
		printf("info");
	}
	else {
		switch(PanelType) {
			case(BOOLEAN_PANEL):
				printf("bin");
				PrintExpectedArgs=0;
				break;
			case(FILE_CHOOSER_PANEL):
				printf("filechooser");
				PrintExpectedArgs=0;
				break;
			case(CHOICE_PANEL):
				printf("choice");
				PrintExpectedArgs=0;
				break;
			case(COMPLEX_PANEL):
				printf("complex");
				PrintExpectedArgs=1;
				break;
			case(MFLAG_PANEL):
				printf("mflag");
				PrintExpectedArgs=1;
				break;
			default:
				fprintf(stderr,"Error! Unknown PanelType %d requested at option: %s. Exiting.\n",PanelType,OptionName);
				exit(1);
		}
	}
	printf("</option_type>\n");
	
	
	
	
	
	PrintIndent(indent_level+1);
	printf("<option_name>%s</option_name>\n",OptionName);
	
	/* now, we give its name as just the "-s/--long" for now */
	/* also record this as the name of the option that will be referred to in error messages,
	currently unused, since the guiLiner spec didn't go this way. */
	/*option_name(TempStr,shortopt,longopt);
	PrintIndent(indent_level+1);
	printf("<option_name_in_error_messages>%s</option_name_in_error_messages>\n",TempStr);*/
	
	
	
	
	/* do the subtext and the choices if needed */
	if(PanelType==CHOICE_PANEL) {
		PrintIndent(indent_level+1);
		printf("<choice>%s</choice>\n",ChoiceElements);
	}	
	/* put the field where the value will be stored in there */
	if(PanelType==FILE_CHOOSER_PANEL) {
		PrintIndent(indent_level+1);
		printf("<file></file>\n");
	}
	if(PanelType==BOOLEAN_PANEL) {
		PrintIndent(indent_level+1);
		printf("<bin></bin>\n");
	}
	if(PanelType==COMPLEX_PANEL) {
		PrintIndent(indent_level+1);
		printf("<option_subtext>%s</option_subtext>\n",ChoiceLessString);
		PrintIndent(indent_level+1);
		printf("<complex></complex>\n");
	}
	if(PanelType==MFLAG_PANEL) {
		PrintIndent(indent_level+1);
		printf("<option_subtext>input format is: %s</option_subtext>\n",ChoiceLessString);
		PrintIndent(indent_level+1);
		printf("<values></values>\n");
	}

	
	
	PrintIndent(indent_level+1);
	printf("<option_flag>%s</option_flag>\n",FlagName);
	
	
		
	PrintIndent(indent_level+1);
	printf("<option_linker> </option_linker>\n");
	
	/* now, we record whether or not it is required */
	PrintIndent(indent_level+1);
	printf("<option_required>");
	switch(IsRequired) {
		case 0: 
			printf("false");
			break;
		case 1:
			printf("true");
			break;
		case 2:
			printf("false");  /* guiliner doesn't really have a facility for dealing with conditionally required stuff */
			break;
		default:
			fprintf(stderr,"Erorr processing ECA_Opt option %s.  Unrecognized IsRequired state = %d.  Exiting...\n",TempStr,IsRequired);
			exit(1);
	}
	printf("</option_required>\n");
	
	
	PrintIndent(indent_level+1);
	printf("<option_can_appear_in_command>");
	if(UsedDuringExecution) {
		printf("true");
	}
	else {
		printf("false");
	}
	printf("</option_can_appear_in_command>\n");
		
				
	PrintIndent(indent_level+1);
	printf("<option_short_description>%s</option_short_description>\n",short_comment);

	
	PrintIndent(indent_level+1);
	printf("<option_long_description>");
	if(strcmp(long_comment,"")!=0) {
		printf("\n");
		PrintIndent(indent_level+2);
		printf("<![CDATA[\n");
		TempCharPtr = ConvertWhiteSpaceToHTML_Friendly(long_comment, 5);
		print_spec_indented_and_max_length(TempCharPtr, 70, indent_level+3, "");
		free(TempCharPtr);
		if(PrintExpectedArgs) {
			printf("<br><br>Expected Format Of Arguments: %s",ChoiceLessString);
		}
		PrintExpectedArgListElementsXML(ArgToksUsed);
		PrintIndent(indent_level+2);
		printf("]]>\n");
		PrintIndent(indent_level+1);
	}
	printf("</option_long_description>\n");

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
	int i,arglen;
	char tempStr[1000];
	char ChoiceElements[500];  /* for the choices associated with the arg list */
	char ChoiceLessString[500];
	int ArgToksUsed[NUM_ARG_TOK_TYPES];
	
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
	if(strcmp(args,"")!=0) {
		SimpleProcArgs(args, ChoiceElements, ChoiceLessString,ArgToksUsed,0);
		printf("%s",ChoiceLessString);
		arglen=strlen(ChoiceLessString);
	}
	else {
		arglen=0;
	}
	
	

/*	printf("%s",args);  */
	for(i=__END_OF_OPTS + arglen;i< __END_OF_OPTS + __LEN_OF_ARGS;i++) printf(" ");
	
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
			const char* ClashString, /* the name of the conflicting option(s) */
			int CurrentIndexLevel,
			const char *OptionName,  /* for an "english name" that the option will be given */
			int UsedDuringExecution  /* this is 1 if the option is something that is used during program execution, and 
										a zero if it is just an info thing like --help, or --version-history.  If 1 then it
										gets a true in guiLiner's <option_can_appear_in_command> field. if zero then false. */
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
			fprintf(stderr,"Error Processing Option %s!    The option %s is absent, but is required  %s\n",tempStr, tempStr, CondReqOptString);
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
			fprintf(stderr,"Error Processing Option %s!    Conflicting options when %s\n",tempStr, ClashString);
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
		eca_opt_print_option_xml(sh,lon,args,short_comment,long_comment, __END_OF_OPTS,CurrentIndexLevel,IsRequired,max_usages,OptionName,UsedDuringExecution);
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
	char **s = (char **)calloc(MAX_COMM_FILE_TOKENS,sizeof(char *));  /* allocate a huge amount of space here, and if we need more, realloc a big block later.
																		 This is bad and hokey. I should realloc little blocks, etc.  But, don't want to deal with
																		 changing that right now */
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
				
				/* if n is a multiple of MAX_COMM_FILE_TOKENS then we need to realloc more memory */
				if(n % MAX_COMM_FILE_TOKENS == 0 ) {
					/*printf("Hey! We are about to reallocate to s in file2charstarstar. n= %d \n\n",n); */
					
					if ((s = (char **)realloc(s, (n + MAX_COMM_FILE_TOKENS) * sizeof(char *) )) == NULL) {
						fprintf(stderr, "ERROR: Ah Crap!  realloc failure in file2charstarstar. n= %d  MAX_COMM_FILE_TOKENS= %d\n", n, MAX_COMM_FILE_TOKENS);
						exit(1);
					}
				}
				
			}
			temp[0]='\0';
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
strings will be inserted starting with argv[i+1].
 
 Note that CommFileCounter tells us how many times the --command-file option has been 
 issued.  If this is >1 then we have to deallocate the memory we allocated to argv
 in a previous call to this function. */
void insert_file_into_commline(char ***argv, int *argc, int *i, char *FileName, int CommFileCounter)
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
	
	/* then free contents (this will free just the char ** part. The char * parts are still
	 pointed to by newone and they are needed there, so don't deallocate them now.  */
	free(contents);
	
	
	/* if this is the second, or later, commfile we have read in, then the argv vector is one that we must
	   have allocated the space too and now we can deallocate all of that. */
	if(CommFileCounter>1) {
		for(j=0;j<*argc;j++)  {
			free( (*argv)[j] );
		}
		free( (*argv) );
	}
	
	/* then put newone into the place of argv */
	(*argv) = newone;
	
	/* update argc to reflect the new additions */
	*argc = *argc + N - 2;
	
	/* increment *i by one so that it will continue on its merry way processing everything as before */
	 *i = *i + 1;
	
}


/* in this, the SubName is for the text interface version, the xml_name is for the gui interface, and the
SubDesc is for a longer description that will go with the option in GuiLiner */
void OpenSubset(const char *SubName, const char *xml_name, const char *SubDesc, int *IndentLevel, int CommentLevel)
{
	switch(CommentLevel) {
		case(1):  /* short comment, long comment, and nroff --- it's the same */
		case(2):
		case(3):
			printf("\n   ****  %s  ****\n\n",SubName);
			return;
			break;
		case(4): /* for xml output it's a little more interesting */
			PrintIndent(*IndentLevel);
			printf("<subset>\n");
			*IndentLevel += 1;
			PrintIndent(*IndentLevel);
			if(strlen(xml_name)>0) {
				printf("<subset_name>%s</subset_name>\n",xml_name);
			}
			else {
				printf("<subset_name>%s</subset_name>\n",SubName);
			}
			PrintIndent(*IndentLevel);
			printf("<subset_description>%s</subset_description>\n",SubDesc);
			break;
	}
}

void CloseSubset(int *IndentLevel, int CommentLevel) 
{
	switch(CommentLevel) {
		case(1):
		case(2):
		case(3):
			return;
			break;
		case(4):
			*IndentLevel -= 1;
			PrintIndent(*IndentLevel);
			printf("</subset>\n");
			break;
	}
}

