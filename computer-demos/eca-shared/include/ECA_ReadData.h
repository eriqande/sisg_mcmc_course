extern int erdEatLineEnd(int Ignore(char), FILE *in);

extern int erdKeyString(char *p, int TermFunc(char), int length, FILE *File);
extern int erdKeyStringLineBreak(char *p, int TermFunc(char), int LineFunc(char), int length, FILE *File);
extern int erdStdItemSeparators(char t);
extern int erdSingleLineWhiteSeparators(char t);
extern int erdStdLineSeparator(char t);
extern int erdIsDigit(char c);
extern int erdIsCap(char c);
extern int erdIsLetter(char c);

int erdGetNext(char *p, double *n, FILE *File);
int erdGetNextOnLine(char *p, double *n, FILE *File, int TermFunc(char), int LineFunc(char), int *EndLineStatus);

FILE *erdOpenFileOrRetry(char *FileName, char *How);


int erdGetNextChar(char *p, double *n, FILE *File);












