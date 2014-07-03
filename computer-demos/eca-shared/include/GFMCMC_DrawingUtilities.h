


#define GFMDU_NO_STIPPLE 65535


/*  here we define a data structure that is useful for watching traces of variables */
/*  but doesn't require a huge amount of data storage because the window slides */
/*  forward with iteration number */
typedef struct 
{
	int N;  /*  the number of iterations to store */
	int Lo;  /*  the value of the lowest iteration number stored */
	int Hi;  /*  the value of the highest iteration number stored */
	double Span;
	
	int Start;  /*  the index of the starting value  */
	int Interval;  /*  the interval at which one wants to record information */
	
	double *Trace;  /*  the array of values.  This gets dynamically allocated to */
	double OverallMax;  /*  to hold the overall max and mins observed since the last */
						/*  refreshing of averages */
	double OverallMin;
} sliding_trace_struct;




/*  prototypes */
void gfduDrawDoubleHistAsRec(hist_struct *H, double Total, double Current);
void gfmduDrawOneHistAsCurveWith2Bars(dval *D, GLint factor, GLushort pattern, int AsDensity);

/*  sliding trace stuff */
sliding_trace_struct *gfduAllocSlidingTrace(int Length, int Interval);
void gfduIncrementSlidingTrace(sliding_trace_struct *S, double value);
void gfduDrawSlidingTrace(sliding_trace_struct *S);
void gfduInitSlidingTraceToZero(sliding_trace_struct *temp);



void gfduWireRectf(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);





