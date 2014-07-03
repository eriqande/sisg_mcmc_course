/***********    MCTypesEtc HEADER FILE
*
*	This is a header code file for Eric Anderson's 
*	"MCTypesEtc" library which includes data types that he has
*	found useful in doing Monte Carlo simulations.  The basic idea
*	is that rather than defining the variables directly as ints or doubles,
*	it is better to define each variable as a struct which includes, for
*	example, its:
*		
*		-Value
*		-Average value
*		-Variance and/or sum of squares
*		-A pointer to a histogram of its values encountered over the run
*		-A pointer to a counter of the result of attempted updates to it
*			(this is particularly useful for Metropolis-Hastings simulations)
*		-Eventually a place to store estimated scale reduction potential information
*			and other things about convergence (when using multiple chains, for example).
*		
*	and probably some more things as time goes along.
*
*	The definitions of these data types are all given is MCTypesEtc.h.  In this file
*	are prototypes of functions dealing with:
*		-allocation/de-allocation of memory to the types
*		-storing results of sweeps in the structs
*			(including averaging and finding the variance)
*		-outputting the information in a struct to a file
*		-copying structs, etc..
*
************/
#ifndef _STDIO_H
#include <stdio.h>
#endif

 /*  don't include the file if it already has been read */
#ifndef ECA_MCTYPES_READ 
#define ECA_MCTYPES_READ

/************************************************************************************
*
*                      STRUCTURE TYPEDEFS
*
************************************************************************************/
/*  A struct for storing histogram info that will go inside dval */
typedef struct
{
	int *H;  /*  the array of values which will be counts */
	double dTot; /*  the total number of entries (i.e. sum of elements in H) */
	double Lo;  /*  the value of the lowest Bin */
	double Hi;  /*  the value of the highest Bin */
	double d;  /*  the step size between bins */
	int NumBins;  /*  Calculated from Lo, Hi, and d.  Will be  */
				  /*  (Hi-Lo/d) + 2.  The extra 2 is for the first bin which is */
				  /*  <= Lo and the last bin which is >= Hi. */
	int HasLeftovers;  /*  if Hi is not Lo + an integer multiple of d this is 1 */
	double Left;  /*  if HasLeftovers==1 this is the left endpoint of that */
				  /*  final, partial bin.  Its right endpoint is an open endpoint */
				  /*  at Hi.  */
}  hist_struct;

/*  A struct for storing histogram info that will go inside ival.  */
/*  Using the hist_struct for doubles encounters rounding problems */
/*  and is a little slower. */
typedef struct
{
	int *H;  /*  the array of values which will be counts */
	double dTot; /*  the total number of entries (i.e. sum of elements in H) */
	int Lo;  /*  the value of the lowest Bin */
	int Hi;  /*  the value of the highest Bin */
	int d;  /*  the step size between bins */
	int NumBins;  /*  Calculated from Lo, Hi, and d.   */
				  /*  there will be Hi-Lo + 2 bins.  If (Hi-Lo)%d == 0, then HasLeftOvers=0, */
				  /*  and the final bin (indexed by NumBins-1) will denote occurrences */
				  /*  of numbers strictly greater than Hi.  If, however,  */
				  /*  (Hi-Lo)%d != 0, then HasLeftOvers=1 and the final bin  */
				  /*  includes >= Hi */
	int HasLeftovers;  /*  if (Hi-Lo)/d != 0, then this is 1.  O otherwise */
/* 	The following stuff I might one day make into an option that would affect */
/*  	how things are down in IncrementIntHist.  But for now, assume that it is */
/*  	always done lumpy! */
/* 	int Lumpy;  // Takes a one if the user wants the occurrences in skipped bins */
				/*  to get stuck into the bin that is, for example, x/d.  And a zero */
				/*  otherwise.  Currently I just set it to one. */

}  int_hist_struct;

/*  a struct for integer valued quantities that will get updated during the MCMC */
/*  run.  This will include places to store information about their averages and  */
/*  variances---so it would be easy to compute some Gelaman-esque Estimated Potential */
/*  Scale Reduction Potential quantities between different chains running on these */
/*  things.   */
typedef struct 
{
	int v;  /*  v for the actual Value */
	double Ave;  /*  the average value */
	double SS;   /*  the sum of squares */
	double Var;  /*  the variance OF THE AVERAGE (not the variance of the realizations  */
				 /*  themselves  (one is a simple function of the other, but it is good to */
				 /*  keep that in mind...) */
	
	
	int NumAved; /*  the number of sweeps that have been averaged over */
	
	int_hist_struct *Hist;  /*  pointer to array of ints for a histogram if desired */
	
	int *UR;  /*  array to count the number of different types of update results */
	
	/*  I will add more later as desired for ESRP stuff */

} ival;



/*  a struct for double-valued quantities that will get updated during the MCMC */
/*  run.  This will include places to store information about their averages and  */
/*  variances---so it would be easy to compute some Gelaman-esque Estimated Potential */
/*  Scale Reduction Potential quantities between different chains running on these */
/*  things.   */
typedef struct 
{
	double v;  /*  v for the actual Value */
	double Ave;  /*  the average value */
	double SS;   /*  the sum of squares */
	double Var;  /*  the variance OF THE AVERAGE (not the variance of the realizations  */
				 /*  themselves  (one is a simple function of the other, but it is good to */
				 /*  keep that in mind...) */
	
	int NumAved; /*  the number of sweeps that have been averaged over */
	
	hist_struct *Hist;  /*  pointer to array of ints for a histogram if desired */
	
	int *UR;  /*  array to count the number of different types of update results */

	/*  I will add more later as desired for ESRP stuff */

} dval;


/*   here is a data structure to help with keeping track of the  */
/*  covariances between different variables.  In this case, the */
/*  we keep track of the covariance between the i and j th elements */
/*  in the Var field.  So the var is then the Covariance of the two */
/*  variables...not of their averages. */
typedef struct 
{
	int Num;  /*  the number of elements you wish to compute covariances for */
	dval ***Sq;

} DvalCovarSquare;


/************************************************************************************
*
*                          MEMORY ALLOCATION
*
***********************************************************************************/
extern ival *AllocIval(int Lo, int Hi, int d);
extern dval *AllocDval(double Lo, double Hi, double d);
extern ival **IvalVector(int Lo,int Hi,int HistLo, int HistHi, int d);
extern dval **DvalVector(int Lo,int Hi, double HistLo, double HistHi, double d);
extern hist_struct *AllocHist(double Lo, double Hi, double d); 
extern int_hist_struct *AllocIntHist(int Lo, int Hi, int d);
extern DvalCovarSquare *AllocDvalCovarSquare(int Num);


/************************************************************************************
*
*                          VARIABLE INITIALIZATION
*
************************************************************************************/
extern void InitIvalToZero(ival *T);
extern void InitIvalSummaryToZero(ival *T);
extern void InitDvalToZero(dval *T);
extern void InitDvalSummaryToZero(dval *T);
extern void InitHistToZero(hist_struct *Hist);
extern void InitIntHistToZero(int_hist_struct *Hist);
extern void InitDvalCovarSquareToZero(DvalCovarSquare *D);


/************************************************************************************
*
*                     VALUE AVERAGING AND INCREMENTING
*
***********************************************************************************/
extern void IncrementIval(ival *D);
extern void IncrementDval(dval *D);
extern void IncrementHist(hist_struct *Hist, double inV);
extern void IncrementIntHist(int_hist_struct *Hist, int inV);
extern void IncrementDvalCovarSquare(DvalCovarSquare *D, dval **Values);


/************************************************************************************
*
*                        OUTPUT OF VALUES AND SUCH
*
************************************************************************************/
extern void fprint_HistTopRow(hist_struct *Hist, char *FileName);
void fprint_IntHistTopRow(int_hist_struct *Hist, char *FileName);
extern void fprint_HistLine(FILE* outfile, hist_struct *Hist);
extern void fprint_HistLineProp(FILE* outfile, hist_struct *Hist);
extern void fprint_IntHistLine(FILE* outfile, int_hist_struct *Hist);
extern void fprint_IntHistLineProp(FILE* outfile, int_hist_struct *Hist);
extern double QuantileFromHist(hist_struct *H, dval *D, double Q);




#endif    




















