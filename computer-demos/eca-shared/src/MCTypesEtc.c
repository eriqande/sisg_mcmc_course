/***********    MCTypesEtc
*
*	This is a c source code file to make Eric Anderson's 
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
*	are functions dealing with:
*		-allocation/de-allocation of memory to the types
*		-storing results of sweeps in the structs
*			(including averaging and finding the variance)
*		-outputting the information in a struct to a file
*		-copying structs, etc..
*
************/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "MCTypesEtc.h"
#include "ECA_MemAlloc.h"






/************************************************************************************
*
*                          MEMORY ALLOCATION
*
***********************************************************************************/


/*   Allocates memory to an ival and returns a pointer to it */
/*  while this is at it, it will set all the values to 0's */
/*  It takes a single argument. */
/*  If Hvals is not NULL, then it creates a hist_struct with  */
/*  the parameters contained in Hvals.  Else it set the Hist */
/*  field to NULL */
ival *AllocIval(int Lo, int Hi, int d)
{
	ival *temp;
	
	temp = (ival *)ECA_MALLOC(sizeof(ival));
	
	temp->Hist = AllocIntHist(Lo, Hi, d);
			
	/*  Set Update Results pointer to NULL.  We'll deal with this more fully  */
	/*  when doing real Metropolis-Hastings Stuff. */
	temp->UR = NULL;
	
	/*  Initialize all the values to zero */
	InitIvalToZero(temp);
	
	return(temp);
}


/*   Allocates memory to a dval and returns a pointer to it */
/*  while this is at it, it will set all the values to 0's */
/*  If Hvals is not NULL, then it creates a hist_struct with  */
/*  the parameters contained in Hvals.  Else it set the Hist */
/*  field to NULL */
dval *AllocDval(double Lo, double Hi, double d)
{
	dval *temp;
	
	temp = (dval *)ECA_MALLOC(sizeof(dval));
	
	/*  Deal with allocating to the histogram part */
	temp->Hist = AllocHist(Lo,Hi,d);
			
	/*  Set Update Results pointer to NULL.  We'll deal with this more fully  */
	/*  when doing real Metropolis-Hastings Stuff. */
	temp->UR = NULL;
	
	/*  Initialize all the values to zero */
	InitDvalToZero(temp);
	
	return(temp);
}



/*  	return a vector of pointers to ival */
/*  The Lo subscript is Lo and the Hi subscript is High */
/*  each of the individual pointers gets initialized as it */
/*  should, according to the type of Histogram it should have */
ival **IvalVector(int Lo,int Hi,int HistLo, int HistHi, int d)
{
	ival **temp;
	int Length = Hi - Lo + 1;
	int i;
	
	if(Hi<Lo) {
		printf("\n\nHi > Lo in ival_vector().  Allocating nothing");
		return(NULL);
	}
	
	temp = (ival **)ECA_MALLOC(Length * sizeof(ival *));
	
	temp -= Lo;  /*  this moves the pointer around so that  temp[Lo] is the */
				 /*  zero element of the block */
	
	for(i=Lo;i<=Hi;i++)  {
		temp[i] = AllocIval(HistLo,HistHi,d);
	}
	
	return(temp);
}



/*  	return a vector of pointers to dval */
/*  The Lo subscript is Lo and the Hi subscript is High */
/*  each of the individual pointers gets initialized as it */
/*  should, according to the type of Histogram it should have */
dval **DvalVector(int Lo,int Hi, double HistLo, double HistHi, double d)
{
	dval **temp;
	int Length = Hi - Lo + 1;
	int i;
	
	if(Hi<Lo) {
		printf("\n\nHi > Lo in DvalVector().  Allocating nothing");
		return(NULL);
	}
	
	temp = (dval **)ECA_MALLOC(Length * sizeof(dval *));
	
	temp -= Lo;  /*  this moves the pointer around so that  temp[Lo] is the */
				 /*  zero element of the block */
	
	for(i=Lo;i<=Hi;i++)  {
		temp[i] = AllocDval(HistLo,HistHi,d);
	}
	
	return(temp);
}


/*  a function to allocate space to a hist_struct and return a pointer to */
/*  it.  The limits and attributes of the histogram are stored in Lo, Hi and */
/*  d.  d is the step size desired. */
/*  This function automatically computes NumBins, and then initializes */
/*  relevant quantities to zero. */
/*  Setting d to zero or a negative value causes this to return NULL */
hist_struct *AllocHist(double Lo, double Hi, double d) 
{
	hist_struct *temp;
	
	
	if(d <= 0.0) {
		temp = NULL;
	}
	
	else {
		/*  first allocate space to the thing */
		temp = (hist_struct *)ECA_MALLOC(sizeof(hist_struct));
		
		/*  copy relevant parameters */
		temp->Lo = Lo;
		temp->Hi = Hi;
		temp->d =  d;
		
		/*  now we determine the number of bins and whether there are */
		/*  any leftovers. */
		temp->NumBins = (int)( (temp->Hi - temp->Lo)/temp->d);
		temp->Left = temp->Lo + (temp->NumBins * temp->d)  ;
		if(temp->Left < temp->Hi)
			temp->HasLeftovers = 1;
		else 
			temp->HasLeftovers = 0;
		
		/*  it needs two extra bins plus one more if it has leftovers. */
		temp->NumBins += 2 + temp->HasLeftovers;
		
		/*  now allocate to the histogram */
		temp->H = (int *)ECA_CALLOC(temp->NumBins,sizeof(int));
		
		/*  and set all values to zero */
		InitHistToZero(temp);
	}
	
	return(temp);

}


/*  a function to allocate space to an int_hist_struct and return a pointer to */
/*  it.  The limits and attributes of the histogram are stored in Lo, Hi and */
/*  d.  d is the step size desired. */
/*  This function automatically computes NumBins, and then initializes */
/*  relevant quantities to zero. */
/*  Setting d to zero or a negative value causes this to return NULL */
int_hist_struct *AllocIntHist(int Lo, int Hi, int d)
{
	int_hist_struct *temp;
	
	
	if(d <= 0) {
		temp = NULL;
	}
	
	else {
		/*  first allocate space to the thing */
		temp = (int_hist_struct *)ECA_MALLOC(sizeof(int_hist_struct));
		
		/*  copy relevant parameters */
		temp->Lo = Lo;
		temp->Hi = Hi;
		temp->d =  d;
		
		/*  now we determine the number of bins and whether there are */
		/*  any leftovers. */
		temp->NumBins = (Hi - Lo)/d + 2;
		if((Hi - Lo)%d  == 0)
			temp->HasLeftovers = 0;
		else 
			temp->HasLeftovers = 1;
		
		/*  now allocate to the histogram */
		temp->H = (int *)ECA_CALLOC(temp->NumBins, sizeof(int));
		
		/*  and set all values to zero */
		InitIntHistToZero(temp);
	}
	
	return(temp);

}




DvalCovarSquare *AllocDvalCovarSquare(int Num)
{
	int i;
	DvalCovarSquare *temp;
	
	temp = (DvalCovarSquare *)ECA_MALLOC(sizeof(DvalCovarSquare));
	
	temp->Num = Num;
	temp->Sq = (dval ***)ECA_CALLOC((unsigned long)temp->Num, sizeof(dval **));
	for(i=0;i<temp->Num;i++)  {
		temp->Sq[i] = DvalVector(0,temp->Num-1,0.0,0.0,0.0);
	}
	
	return(temp);
}


/************************************************************************************
*
*                          VARIABLE INITIALIZATION
*
***********************************************************************************/


/*  Initialize to zeroes all the values in an ival */
void InitIvalToZero(ival *T)
{
	T->v = 0;
	T->Ave = 0.0;
	T->Var = 0.0;
	T->SS = 0.0;
	T->NumAved = 0;
	
	/*  If this has a histogram, then reset its entries to zero, too. */
	if(T->Hist != NULL)  {
		InitIntHistToZero(T->Hist);
	}
}


/*  Initialize to zeroes all the *summary* values in an ival. */
/*  i.e. you don't change the value itself, but you change set all the */
/*  averages and NumAved and histograms to zero. */
void InitIvalSummaryToZero(ival *T)
{
	T->Ave = 0.0;
	T->Var = 0.0;
	T->SS = 0.0;
	T->NumAved = 0;
	
	/*  If this has a histogram, then reset its entries to zero, too. */
	if(T->Hist != NULL)  {
		InitIntHistToZero(T->Hist);
	}
}


/*  Initialize to zeroes all the values in an dval */
void InitDvalToZero(dval *T)
{
	T->v = 0.0;
	T->Ave = 0.0;
	T->Var = 0.0;
	T->SS = 0.0;
	T->NumAved = 0;
	
	/*  If this has a histogram, then reset its entries to zero, too. */
	if(T->Hist != NULL)  {
		InitHistToZero(T->Hist);
	}
}


/*  Initialize to zeroes all the *summary* values in a dval. */
/*  i.e. you don't change the value itself, but you change set all the */
/*  averages and NumAved and histograms to zero. */
void InitDvalSummaryToZero(dval *T)
{
	T->Ave = 0.0;
	T->Var = 0.0;
	T->SS = 0.0;
	T->NumAved = 0;
	
	/*  If this has a histogram, then reset its entries to zero, too. */
	if(T->Hist != NULL)  {
		InitHistToZero(T->Hist);
	}
}


/*  Initialize to zeroes all the value in a histogram struct */
void InitHistToZero(hist_struct *Hist)
{
	int i; 
	
	/*  Only do this if the pointer to Hist is not null */
	if(Hist != NULL)  {
 		/*   set all values to zero in the histogram array */
		for(i=0;i<Hist->NumBins;i++)  {
			Hist->H[i] = 0;
		}
		
		/*  and set the Tot to 0 as well */
		Hist->dTot = 0.0;
 
 	}
	
}

/*  Initialize to zeroes all the value in an integer histogram struct */
void InitIntHistToZero(int_hist_struct *Hist)
{
	int i; 
	
	/*  Only do this if the pointer to Hist is not null */
	if(Hist != NULL)  {
 		/*   set all values to zero in the histogram array */
		for(i=0;i<Hist->NumBins;i++)  {
			Hist->H[i] = 0;
		}
		
		/*  and set the Tot to 0 as well */
		Hist->dTot = 0.0;
 
 	}
	
}



void InitDvalCovarSquareToZero(DvalCovarSquare *D)
{
	int i,j;
	for(i=0;i<D->Num;i++)  {
		for(j=0;j<D->Num;j++)  {
			InitDvalToZero(D->Sq[i][j]);
		}
	}
}




/************************************************************************************
*
*                       VALUE AVERAGING AND INCREMENTING
*
************************************************************************************/
/*  takes the current value in the ival and uses that to update the average, the */
/*  sum of squares, the NumAved, the variance and the histogram */
void IncrementIval(ival *D)
{
	double n;
	/*  first thing: increment the NumAved, so it averages over realizations 1,2,3,... */
	/*  rather than a base-zero subscripted thing */
	D->NumAved++;
	
	n = (double)D->NumAved;
	
	/*  now compute the running average based on the old average */
	D->Ave += ((double)D->v - D->Ave)/n;
	
	/*  incrementing the sum of squares */
	D->SS += (double)(D->v * D->v);
	
	/*  computing the variance of the average */
	if(D->NumAved>1) {
		D->Var = ( D->SS - n * (D->Ave * D->Ave) ) / 
						( n * (n-1.0) );
	}
	
	/*  then incrementing the histogram */
	IncrementIntHist(D->Hist, D->v);
}


/*  takes the current value in the dval and uses that to update the average, the */
/*  sum of squares, the NumAved, the variance and the histogram */
void IncrementDval(dval *D)
{
	double n;
	/*  first thing: increment the NumAved, so it averages over realizations 1,2,3,... */
	/*  rather than a base-zero subscripted thing */
	D->NumAved++;
	
	n = (double)D->NumAved;
	
	/*  now compute the running average based on the old average */
	D->Ave += (D->v - D->Ave)/n;
	
	/*  incrementing the sum of squares */
	D->SS += D->v * D->v;
	
	/*  computing the variance of the average */
	if(D->NumAved>1) {
		D->Var = ( D->SS - n * (D->Ave * D->Ave) ) / 
						( n * (n-1.0) );
	}
	
	/*  then incrementing the histogram */
	IncrementHist(D->Hist, D->v);
}



/*  Increments a histogram based on the value inV */
void IncrementHist(hist_struct *Hist, double inV) 
{
	if(Hist != NULL)  {  /*  only do this if the histogram has been defined, etc. */
		/*  right off the bat, we increment dTot; */
		Hist->dTot += 1.0;
		
		/*  then we figure out which bin to increment */
		if(inV >= Hist->Hi)  {
			Hist->H[Hist->NumBins-1]++;
		}
		else if( (Hist->HasLeftovers==1) && inV >= Hist->Left)  {
			Hist->H[Hist->NumBins-2]++;
		}
		else if(inV <= Hist->Lo)  {
			Hist->H[0]++;
		}
		else {
			Hist->H[ (int)((inV - Hist->Lo) / Hist->d) + 1]++;
		}
	}
}


/*  Increments an integer histogram based on the value inV */
void IncrementIntHist(int_hist_struct *Hist, int inV) 
{
	if(Hist != NULL)  {  /*  only do this if the histogram has been defined, etc. */
		/*  right off the bat, we increment dTot; */
		Hist->dTot += 1.0;
		
		/*  then we figure out which bin to increment.  With the integers it is pretty easy */
		/*  in this case all the intervening ones get lumped in */
		if(inV <= Hist->Hi)
			Hist->H[  (inV - Hist->Lo) / Hist->d ] += 1;
		else
			Hist->H[Hist->NumBins - 1] += 1;
		
	}
}


/*  given an array of values that also have their means in there */
/*  compute the covariances and store them in the Var field */
/*  This assumes that the Values have been recently incremented */
void IncrementDvalCovarSquare(DvalCovarSquare *D, dval **Values)
{
	int i,j;
	double n;
	
	n = Values[0]->NumAved;
	
	for(i=0;i<D->Num;i++)  {
		for(j=0;j<D->Num;j++)  {
			if(n != Values[i]->NumAved)
				printf("\n\nNumAved Not equal across DvalCovarSquare!!\n\n");
			
			/*  accumulate the sum of cross products in the SS field */
			D->Sq[i][j]->SS += Values[i]->v * Values[j]->v;
			
			/*  Once all the cross-products are available, */
			/*  store the Covariance of the values in Var */
			if(n>1) {
				D->Sq[i][j]->Var = (D->Sq[i][j]->SS - n * Values[i]->Ave * Values[j]->Ave) / (n-1.0);
			}
			
		}
	}
	
	
}


/************************************************************************************
*
*                        OUTPUT OF VALUES AND SUCH
*
************************************************************************************/


/*  Prints a title line to a file for outputting the histograms of the */
/*  different variables.  Hist is a variable that tells what kind of format */
/*  it's going to be, and FileName is the name of the file to output to.   */
/*  It erases the contents of that file and replaces it with this new top line. */
void fprint_HistTopRow(hist_struct *Hist, char *FileName)
{
	FILE *in;
	double i;
	
	/*  open the file and obliterate all that was in it. */
	if ( ( in = fopen(FileName,"w")) == NULL) {
		fprintf(stderr,"Can't open file %s to write in fprintf_HistTopRow\n",FileName);
	}
	
	/*  then write out the row as space-delimited text showing the intervals that correspond */
	/*  to the bins */
	/*  first the title for the labels column */
	fprintf(in,"Variable");
	
	/*  then the Lo bin */
	fprintf(in,"   <=%.3f",Hist->Lo);
	
	/*  then the next bin which is a little funny in terms of whether its */
	/*  endpoints are open or closed. */
	fprintf(in,"   (%.3f,%.3f)",Hist->Lo,Hist->Lo + Hist->d);
	
	/*  then we cycle over the remaining bins up to NumBins-3   */
	for(i=2.0; i <= Hist->NumBins-3;  i+=1.0)  {  
		fprintf(in,"   [%.3f,%.3f)",Hist->Lo + Hist->d*(i-1.0) ,Hist->Lo + Hist->d*i);
	}
	
	/*  then we alter the final bins because of the Leftovers thing. */
	/*  at this point, note that i=NumBins-2 */
	fprintf(in,"     [%.3f,%.3f)",Hist->Lo + Hist->d*(i-1.0) ,Hist->Hi);
	
	/*  then put the last bin title in there: */
	fprintf(in,"    >=%.3f",Hist->Hi);
	
	/*  then we must close the file */
	fclose(in);
}



/*  Prints a title line to a file for outputting the histograms of the */
/*  different variables.  Hist is a variable that tells what kind of format */
/*  it's going to be, and FileName is the name of the file to output to.   */
/*  It erases the contents of that file and replaces it with this new top line. */
void fprint_IntHistTopRow(int_hist_struct *Hist, char *FileName)
{
	FILE *in;
	double i;
	
	
	if(FileName==NULL) {
		in = stdout;
	}
	
	/*  open the file and obliterate all that was in it. */
	else if ( ( in = fopen(FileName,"w")) == NULL) {
		fprintf(stderr,"Can't open file %s to write in fprintf_HIstTopRow\n",FileName);
	}
	
	/*  then write out the row as space-delimited text showing the intervals (if any) that correspond */
	/*  to the bins */
	/*  first the title for the labels column */
	fprintf(in,"Variable");
	
	/*  then the bins */
	for(i=0;i<Hist->NumBins-1;i++) {
		if(Hist->d != 1)  /*  in this case, we will have intervals to deal with */
			fprintf(in,"   [%d,%d]", (int)(Hist->Lo + i*Hist->d), (int)(Hist->Lo + i*Hist->d + (Hist->d-1)));
		else  /*  otherwise we just use the number with no brackets */
			fprintf(in,"   %d", (int)(Hist->Lo + i));
	}
	
	/*  then deal with that final bin.  If HasLeftOvers == 0 then the final bin is all > Hi, */
	/*  Otherwise the final bin will be >= ( Lo + d * (NumBins-1) ) */
	if(Hist->HasLeftovers == 1)  /*  in this case, we will have intervals to deal with */
		fprintf(in,"   >=%d", Hist->Lo + Hist->d * (Hist->NumBins-1) );
	else  /*  otherwise it is strictly greater than Hi */
		fprintf(in,"   >%d", Hist->Hi);
	
	if(FileName) {		
		fclose(in);
	}
}




/*  These two functions print out the line of a histogram to the file pointed to by outfile. */
/*  They puts some whitespace after the line but not before the line.   */
/*  They do not put any carriage returns in.  That */
/*  must be taken care of before and after the function is called.  These functions */
/*  assume that there is currently a pointer to file that will be used.  It is  */
/*  recommended that before these functions are called, the name of the variable should */
/*  be fprinted, followed by some whitespace.   */
/*  THIS FUNCTION PRINTS THE ***COUNTS*** OF THE NUMBER OF OCCURRENCES IN EACH BIN. */
void fprint_HistLine(FILE* outfile, hist_struct *Hist)
{
	int i;
	
	for(i=0;i<Hist->NumBins;i++)  {
		fprintf(outfile, "%d  ", Hist->H[i]);
	}
}
/*  THIS FUNCTION PRINTS THE ***PROPORTIONS*** OF OCCURRENCES IN EACH BIN. */
void fprint_HistLineProp(FILE* outfile, hist_struct *Hist)
{
	int i;
	
	for(i=0;i<Hist->NumBins;i++)  {
		fprintf(outfile, "%.8f  ", (double)Hist->H[i]/Hist->dTot  );
	}
}



/*  Same as the above two, but for interval histograms */
/*  THIS FUNCTION PRINTS THE ***COUNTS*** OF THE NUMBER OF OCCURRENCES IN EACH BIN. */
void fprint_IntHistLine(FILE* outfile, int_hist_struct *Hist)
{
	int i;
	
	for(i=0;i<Hist->NumBins;i++)  {
		fprintf(outfile, "%d  ", Hist->H[i]);
	}
}
/*  THIS FUNCTION PRINTS THE ***PROPORTIONS*** OF OCCURRENCES IN EACH BIN. */
void fprint_IntHistLineProp(FILE* outfile, int_hist_struct *Hist)
{
	int i;
	
	for(i=0;i<Hist->NumBins;i++)  {
		fprintf(outfile, "%.8f  ", (double)Hist->H[i]/Hist->dTot  );
	}
}



/* for a quantile Q (between 0 and 1), this returns the
quantile of the associated dval variable estimated from the histogram.
This was coded up fast and loose without too much thought.  It could
be made more accurate and efficient, but it will have to do for now. */
double QuantileFromHist(hist_struct *H, dval *D, double Q)
{
	int i;
	double A=0.0,v,x;
	double F=1.0/H->dTot;
	
	if(H==NULL) {
		return(0.0);
		printf("WARNING: Hist is Null in QuantileFromHist");
	}
	
/*printf("F is %f\n",F);*/
	
	for(v=H->Lo,i=0;i<H->NumBins;v+=H->d,i++) {
		A += H->H[i] * F;
/*		printf("v= %f  A=%f   Q= %f",v,A,Q);*/
		if(A>Q) {
			if(i==0) {
				return(H->Lo);
			}
			else {
				x = (Q - (A - H->H[i]*F) ) / (H->H[i] * F);
			}
			return(v + x * H->d );
		}
		
	}
	return(v);
}

















