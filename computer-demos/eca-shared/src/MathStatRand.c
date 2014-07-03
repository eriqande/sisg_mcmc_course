/*!
\file MathStatRand.c
\brief Source code for Eric Anderson's MathStatRand library.

This is a c source code file to make Eric Anderson's 
"MathStatRand" library which includes routines that he has
commonly used in his programs for the last couple of years
and which he wanted to put together in one place

The types of functions herein are:
  - Math: Mathematical Special functions like the gamma function
  - Stat: Probability densities and mass functions and cdf's
  - Rand: Functions for generating random variates that are not widely available. 
      These functions rely on the functions available in the ranlib.c
	  				random number generator library
\author Eric C. Anderson
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include "ranlib.h"
#include "ECA_MemAlloc.h"
#include "MathStatRand.h"



/******************************************************************************************/
/*                                                                                        */
/*						          MATHEMATICAL FUNCTIONS                                  */
/*                                                                                        */  
/******************************************************************************************/




/*!
\brief Computes the ErrorFunction
			  
\param x The number for which to compute the Error function
*/
/*
   
   	Requires the function GammaP.
   	
   AUTHOR:  Eric C. Anderson
   DATE:  18 AUGUST 2000
*************/
double 
ErrFunct(double x) 
{
	if(x < 0.0) {
		return( -GammaP(.5, x*x));
	}
	else {
		return( GammaP(.5, x*x));
	}
}

/*************
*  FUNCTION:  double ErrFunctComp
*  PURPOSE:   Compute the ErrorFunction
			  
			
*  INPUT:     double x
   
   	Requires the function GammaP and GammaQ  Based on Numerical Recipes in C, page 220.
   	
   AUTHOR:  Eric C. Anderson
   DATE:  18 AUGUST 2000
*************/
double 
ErrFunctComp(double x) 
{
	if(x < 0.0) {
		return( 1.0 + GammaP(.5, x*x));
	}
	else {
		return( GammaQ(.5, x*x));
	}
}


/*
 * Lower tail quantile for standard normal distribution function.
 *
 * This function returns an approximation of the inverse cumulative
 * standard normal distribution function.  I.e., given P, it returns
 * an approximation to the X satisfying P = Pr{Z <= X} where Z is a
 * random variable from the standard normal distribution.
 *
 * The algorithm uses a minimax approximation by rational functions
 * and the result has a relative error whose absolute value is less
 * than 1.15e-9.
 *
 * Author:      Peter J. Acklam
 * Time-stamp:  2002-06-09 18:45:44 +0200
 * E-mail:      jacklam@math.uio.no
 * WWW URL:     http://www.math.uio.no/~jacklam
 *
 * C implementation adapted from Peter's Perl version
 
 * MODIFIED BY ERIC ANDERSON BY PUTTING THE GLOBAL STATICS INSIDE THE
   FUNCTION
 
 */
double
ltqnorm(double p)
{
	double LOW  = 0.02425;
	double HIGH = 0.97575;
	/* Coefficients in rational approximations. */
	static const double a[] =
	{
		-3.969683028665376e+01,
		 2.209460984245205e+02,
		-2.759285104469687e+02,
		 1.383577518672690e+02,
		-3.066479806614716e+01,
		 2.506628277459239e+00
	};

	static const double b[] =
	{
		-5.447609879822406e+01,
		 1.615858368580409e+02,
		-1.556989798598866e+02,
		 6.680131188771972e+01,
		-1.328068155288572e+01
	};

	static const double c[] =
	{
		-7.784894002430293e-03,
		-3.223964580411365e-01,
		-2.400758277161838e+00,
		-2.549732539343734e+00,
		 4.374664141464968e+00,
		 2.938163982698783e+00
	};

	static const double d[] =
	{
		7.784695709041462e-03,
		3.224671290700398e-01,
		2.445134137142996e+00,
		3.754408661907416e+00
	};

	double q, r;

	errno = 0;

	if (p < 0 || p > 1)
	{
		errno = EDOM;
		return 0.0;
	}
	else if (p == 0)
	{
		errno = ERANGE;
		return -HUGE_VAL /* minus "infinity" */;
	}
	else if (p == 1)
	{
		errno = ERANGE;
		return HUGE_VAL /* "infinity" */;
	}
	else if (p < LOW)
	{
		/* Rational approximation for lower region */
		q = sqrt(-2*log(p));
		return (((((c[0]*q+c[1])*q+c[2])*q+c[3])*q+c[4])*q+c[5]) /
			((((d[0]*q+d[1])*q+d[2])*q+d[3])*q+1);
	}
	else if (p > HIGH)
	{
		/* Rational approximation for upper region */
		q  = sqrt(-2*log(1-p));
		return -(((((c[0]*q+c[1])*q+c[2])*q+c[3])*q+c[4])*q+c[5]) /
			((((d[0]*q+d[1])*q+d[2])*q+d[3])*q+1);
	}
	else
	{
		/* Rational approximation for central region */
    		q = p - 0.5;
    		r = q*q;
		return (((((a[0]*r+a[1])*r+a[2])*r+a[3])*r+a[4])*r+a[5])*q /
			(((((b[0]*r+b[1])*r+b[2])*r+b[3])*r+b[4])*r+1);
	}
}

/*************
*  FUNCTION:  double GammaP
*  PURPOSE:   Compute the incomplete Gamma function.  P(a,x)
			  
			
*  INPUT:     double a;
			  double x;
   
   	Requires the functions IncGammaCF and IncGammaSer
   	
   AUTHOR:  Eric C. Anderson
   DATE:  18 AUGUST 2000
*************/
double 
GammaP(double a, double x) 
{
	if(x<0.0 || a <= 0.0)  {
		printf("\n\nx<0.0 or a<=0.0 in GammaP\n\nExiting to system...");
		exit(1);
	}
	if(x <= a + 1.0) {
		return(IncGammaSer(a,x));
	}
	else {
		return(1.0 - IncGammaCF(a,x));
	}
}


/*************
*  FUNCTION:  double GammaQ
*  PURPOSE:   Compute the incomplete Gamma function.  Q(a,x).
			  This is the part integrated from x to infinity.
			  
			
*  INPUT:     double a;
			  double x;
   
   	Requires the functions IncGammaCF and IncGammaSer
   	
   AUTHOR:  Eric C. Anderson
   DATE:  18 AUGUST 2000
*************/
double 
GammaQ(double a, double x) 
{
	if(x<0.0 || a <= 0.0)  {
		printf("\n\nx<0.0 or a<=0.0 in GammaP\n\nExiting to system...");
		exit(1);
	}
	if(x <= a + 1.0) {
		return(1.0 - IncGammaSer(a,x));
	}
	else {
		return(IncGammaCF(a,x));
	}
}

/*************
*  FUNCTION:  double IncGammaSer
*  PURPOSE:   Compute the incomplete Gamma function by its series representation.  
			  Note that this computes the part from 0 to x (called P(a,x) )
			  
			  This is the preferred method when x < (a+1)
			
*  INPUT:     double x

	This function is based on the series representation described
	in "Numerical Recipes in C" (Cambridge University Press), 
   
   	Requires the function GammaLog
   	
   AUTHOR:  Eric C. Anderson
   DATE:  18 AUGUST 2000
*************/
#define MAXIT 100
#define PRECIS 1.0e-7

double 
IncGammaSer(double a, double x) 
{
	double old_funct, new_funct;  /*  this is what we ultimately want to return; */
	double term;  /*  the next term to be added to the series */
	double i;
	double numer;  /*  the normalizing constant---Gamma(a).  Also turns out to be the numerator of the terms that */
				   /*  get summed. */
	double denom;  /*  the denominator of the terms that get summed */
	
	/*  set value of the log of the normalizing constant */
	numer = exp(LogGamma(a));
	denom = a * numer;
	
	term = numer/denom;
	old_funct = term;
	
	for(i=1;i<MAXIT;i++)  {
		term *= 1.0/(a + i) * x;  /*  the terms may be computed recursively */
		new_funct = old_funct + term;
		
		/*  check for convergence */
		if( fabs(new_funct - old_funct) < PRECIS)  {
			break;
		}
		else {
			old_funct = new_funct;
		}
	}
	
	if(i >= MAXIT - .001) {
		/*  done here we warn if it failed to converge.  */
		printf("\n\nFailed to converge in IncGammaSer\n\n");
		exit(1);
	}
	
	/*  otherwise, we return the result */
	/*  first we have to add the coefficients and normalize it: */
	new_funct *= exp(-x) * pow(x,a) * 1.0/numer;
	return(new_funct);
}
#undef MAXIT
#undef PRECIS

/*************
*  FUNCTION:  double IncGammaCF
*  PURPOSE:   Compute the incomplete Gamma function by its continued
			  fraction representation.  Note that this computes the 
			  part from x to infinity (called Q(a,x) )
			  
			  This is the preferred method for x > (a + 1)
			
*  INPUT:     double x

	This function is based on the continued fraction representation described
	in "Numerical Recipes in C" (Cambridge University Press), and evaluated using
	Lentz's method for continued fractions.  Basically following the pseudocode on
	page 171.
   
   	Requires the function GammaLog
   	
   AUTHOR:  Eric C. Anderson
   DATE:  18 AUGUST 2000
*************/
#define SMIDGEN 1.0e-30
#define MAXIT 100
#define PRECIS 2.0e-7

double 
IncGammaCF(double a, double x) 
{
	double funct;  /*  this is what we ultimately want to return; */
	double A,B,C,D;
	double Delta;
	double j;
	double log_normo;  /*  the log of the normalizing constant---Gamma(a) */
	
	/*  set value of the log of the normalizing constant */
	log_normo = LogGamma(a);
	
	/*  here for the zero subscript part: */
	B = 0.0;  /*  b_0 is really zero, so we just make it tiny */
	funct = SMIDGEN;
	C = funct;
	D = 0.0;
	
	/*  then for the 1 subscript part things are sort of different still */
	A = 1.0;
	B = x + 1.0 - a;
	D = B + A * D;
	if(fabs(D) < SMIDGEN) D = SMIDGEN;
	C = B +  A/C;
	if(fabs(C) < SMIDGEN) C = SMIDGEN;
	D = 1.0/D;
	Delta = C * D;
	funct = Delta * funct;
	/*  don't even bother checking for convergence after this first "iteration" */
	
	
	/*  now we iterate through the 2,3... and so forth subscript parts until converged */
	/*  so each loop corresponds to the j+1-th subscript of the continued fraction */
	for(j=1.0;j<MAXIT;j++)  {
		/*  now, we define the new values for A and B on the j-th level of the continued fraction */
		A = -j * (j - a);
		B = x + 1.0 + (2 * j) - a;	
		
		D = B + A * D;
		if(fabs(D) < SMIDGEN) D = SMIDGEN;
		C = B + A/C;
		if(fabs(C) < SMIDGEN) C = SMIDGEN;
		D = 1.0/D;
		Delta = C * D;
		funct = Delta * funct;

		/*  here we check for convergence.  If it has, we return the appropriate result */
		if(fabs(Delta - 1.0) < PRECIS) {
			/*  add the coefficients and the normalizing constant */
			funct *= exp(-x - log_normo) * pow(x,a);
			return(funct);
		}
	}
	
	/*  done here we warn if it failed to converge.  Should never get here */
	printf("\n\nFailed to converge in IncGammaCF\n\n");
	exit(1);
	
	return(-55.55);  /*  just put this in so that it doesn't give a compile warning */
					 /*  about having no return value. */

}
#undef SMIDGEN
#undef MAXIT
#undef PRECIS

/*************
*  FUNCTION:  double LogGamma
*  PURPOSE:   Compute log of the Gamma Function
			
*  INPUT:     double x

	This function is based on the six term series of Lanczos as described
	in "Numerical Recipes in C" (Cambridge University Press).  I use the 
	choice of gamma = 5 and N = 6.  
	
*************/
double 
LogGamma(double x) 
{
	/*  declare the coefficients in the series as a static double array */
	static double Coeff[6] = {76.18009172947146,-86.50532032941677,
		24.01409824083091,-1.231739572450155,
		0.1208650973866179e-2,-0.5395239384953e-5};
	double Prelim;  /*  for the part before the series */
	double Series;
	int i;
	double denom;	
	
	/*  We start off by computing Gamma(x+1) by the series: */
	
	/*  compute the preliminary part */
	Prelim = (x+.5) * log(x+5.5) - (x+5.5);

	/*  add the log of sqrt(2\pi) to that: */
	Prelim += 0.91893853320467274;
	
	/*  now compute the series.  Start with the constant term c_0 */
	Series = 1.000000000190015;

	denom = x;
	for(i=0;i<6;i++)  {
		Series += Coeff[i]/(++denom);
	}
	
	/*  now we just have to return the right thing.  Since we just computed */
	/*  Recall Gamma(x+1) = x * Gamma(x), so we have to subtract log(x) from this. */
	/*  We can do this with only one call to log by dividing Series by x. */
	return(Prelim + log(Series/x));


}

/*!
\brief Return the log of x!
	
This function returns the log of the factorial of x.
Based on the implementation in Numerical Recipes in C.
			
\param double x
   
\author  Eric C. Anderson
\date 18 AUGUST 2000
*************/
double 
LogFactorial(int x) 
{
	static double PreStored[201];  /*  we will precompute and store value for up to 200 */
	int i;
	
	if(x<0) {
		printf("\n\nNegative argument in call of LogFactorial\n\nExiting...");
		exit(1);
	}
	if(x<=1) {
		return(0.0);
	}
	if(x <= 200)  {
		if(!PreStored[x])  {  /*  if PreStored[x] is zero, then we must fill in the table */
			for(i=2;i<201;i++)  {
				PreStored[i] = LogGamma((double)i + 1.0);
			}
		}
		return(PreStored[x]);
	}
	else    /*  otherwise we have to compute it. */
		return(LogGamma((double)x + 1.0));
	
}


/*!
\brief Return x!
	
This function returns the factorial of x.
Based on the implementation in Numerical Recipes in C.
			
\param double x
   
\author  Eric C. Anderson
\date 18 AUGUST 2000
*************/
double 
Factorial(int x) 
{
	static double PreStored[70];  /*  we will precompute and store value for up to 200 */
	int i;
	
	if(x<0) {
		printf("\n\nNegative argument in call of Factorial\n\nExiting...");
		exit(1);
	}
	if(x<=1) {
		return(1.0);
	}
	if(x <= 69)  {
		if(!PreStored[x])  {  /*  if PreStored[x] is zero, then we must fill in the table */
			for(i=2;i<70;i++)  {
				PreStored[i] = exp(LogGamma((double)i + 1.0));
			}
		}
		return(PreStored[x]);
	}
	else    /*  otherwise we have to compute it. */
		return(exp(LogGamma((double)x + 1.0)));
}


/*
	this simply returns the log of the binomial coefficient.  It is only defined for n,x >= 0, and n >= x. 
*/
double LogBinomCoeff(int n, int x)
{
	if(n<0 || x<0 ) {
		fprintf(stderr,"WARNING: Negative arg of LogBinomCoeff.  n=%d  x=%d\n",n,x);
		exit(1);
	}
	if(n<x) {
		fprintf(stderr,"WARNING: Negative n<x in LogBinomCoeff.  n=%d  x=%d\n",n,x);
		exit(1);
	}
	if(n==x) {
		return(0.0);
	}
	return( LogFactorial(n) - LogFactorial(n-x) - LogFactorial(x) );
}



/*
	This recursive function returns the Stirling number of the third kind 
	as described on page 6 of Johnson, Kotz, and Balakrishnan's "Discrete
	Multivariate Distributions."  The Stirling number of the third kind,
	s(n,i) is defined as those coefficients which satisfy:
	
	x(x+1)...(x+n-1) = \sum_{i=0}^n s(n,i)x^i
	
	Which is pretty cool. 
	
	s(n,i) also satisfies the recursions:
	
		s(n,1) = (n-1)!
		
		s(n,n) = 1
		
		s(n,i) = s(n-1,i-1) + (n-1)s(n-1,i)     for i = 1,2,...,n-1
		
	These recursions are used in this function.
	
	The way you use this function:  The variable you pass in as M should be a double ***
	should be the address of a double ** variable. it
	might be simplest to give it global scope.  It is within M that this function will
	store previous results to be used in later recursions.  If M==NULL, then this function
	will allocate memory to M as needed.  If M != NULL but it doesn't have sufficient
	memory allocated to it, you could be screwed.  MaxN is the largest n that will ever
	get passed to this function. 
	
	M is subscripted by [n][i]
*/
double RecursiveLogStirling3(double ***M, int n, int i, int MaxN)
{
	int j,k;
	double norm;  /* a factor to be able to do the calculation without overflow */
	
	if(n > MaxN) {
		fprintf(stderr, "\n\nn = %d > MaxN = %d in RecursiveStirling3.",n,MaxN);
		fprintf(stderr, "\n...Exiting to system...\n\n");
		exit(1);
	}
	if(n<1 || i < 1 || i>n) {
		fprintf(stderr, "\n\nError in RecursiveStirling3. n = %d, i = %d",n,i);
		fprintf(stderr, "\n...Exiting to system...\n\n");
		exit(1);
	}
	if(*M==NULL)  {
		(*M) = (double **)ECA_CALLOC( (size_t)(MaxN+1), sizeof(double *));
		for(j=0;j<MaxN+1;j++)  {
			(*M)[j] = (double *)ECA_CALLOC( (size_t)j+1, sizeof(double));
		}
		/* initialize to -1.0's */
		for(j=0;j<=MaxN;j++)  {
			for(k=0;k<=j;k++)  {
				(*M)[j][k] = -1.0;
			}
		}
		/* do a few simple initializations */
		for(j=1;j<=MaxN;j++)  {
			(*M)[j][j] = 0.0;
			(*M)[j][1] = LogFactorial(j-1);
		}
	}
	
	/* take care of the easy cases first */
	if(n==i) {
		return(0.0);
	}
	if(i==1) {
		return((*M)[n][i]);
		
	}
	/*  now implement the recursion. */
	if((*M)[n][i]==-1.0) {
		if((*M)[n-1][i]==-1.0) {
			RecursiveLogStirling3(M,n-1,i,MaxN);
		}
		if((*M)[n-1][i-1]==-1.0) {
			RecursiveLogStirling3(M,n-1,i-1,MaxN);
		}
		
		/* now that these have been taken care of, we can return the result, but only AFTER
		   recording the result in M  */
		norm = (*M)[n-1][i-1] < (*M)[n-1][i] ? (*M)[n-1][i-1]  : (*M)[n-1][i];
		
		(*M)[n][i] = norm + log( exp((*M)[n-1][i-1] - norm) + (double)(n-1) * 
							exp((*M)[n-1][i] - norm) );
	}
	return((*M)[n][i]);
}


/******************************************************************************************/
/*                                                                                        */
/*						          STATISTICAL FUNCTIONS                                   */
/*                                                                                        */  
/******************************************************************************************/


/*************
*  FUNCTION:  double LogBinomiallPMF(int n, double p, int x)
*  PURPOSE:   Return the log of the binomial probability function
*
*  INPUT: 	  int n  -->  number of trials
			  double p  --> the probability of success
*  			  int x  -->  the number of successes
			  
*************/
double LogBinomialPMF(int n, double p, int x)
{
	double pa[2];
	int xa[2];
	
	pa[0] = p;
	pa[1] = 1.0 - p;
	
	xa[0] = x;
	xa[1] = n-x;
	
	return(LogMultinomialPMF(n,pa,xa,2));
}


/*************
*  FUNCTION:  double LogMultinomialPMF(int n, double *p, int *x, int inNum)
*  PURPOSE:   Return the log of the multinomial probability function
*
*  INPUT: 	  int n  -->  number of trials
			  double *p  --> the probability of success
*  			  int *x  -->  the number of successes
			  int inNum --> the number of categories
			  
	Originally function log_multi_prob
*************/
double 
LogMultinomialPMF(int n, double *p, int *x, int inNum)
{
	int k;  /*  subscript over categories */
	double log_prob; 
	int check_n;
	double check_p;
	
	
	log_prob = LogFactorial(n);  /*  initialize to log n! to accumulate the sum */
	check_n = 0;
	check_p = 0.0;
	
	for(k=0;k<inNum;k++) {
		if(p[k] != 0 ) {
			log_prob += x[k] * log(p[k]) - LogFactorial(x[k]);
		}
		else if(x[k] != 0) {
			printf("\n\nYou're bumming my friend.  You have a zero probability event \nin LogMultinomialPMF!\n\n");	
			printf("\nCrashing and burning\n");
			printf("\n...Exiting to system...\n");
			exit(1);
		}
		check_n += x[k];
		check_p += p[k];
	}
	
	if(check_n != n) {
		printf("\n\nSum of x's is not n in LogMultinomialPMF\n\n");
		printf("\nThis cannot be tolerated!!  Get me out of here!");
		printf("\n...Exiting to system...\n");
		exit(1);
	}
	if(check_p < .99999999999 || check_p > 1.00000000001) {
		printf("\n\nSum of p's is not 1.0 in LogMultinomialPMF\n\n");
		printf("\nThis is a grave problem!!  Going away now!");
		printf("\n...Exiting to system...\n");
		exit(1);
	}
	return(log_prob);
}



/*!
\brief Computes the probability of a Dirichlet-compound multinomial random vector in a way that is efficient for small N.

This function returnes the probability of a Dirichlet-compound multinomial random
vector N, given the parameters (a_1,...,a_K) of the Dirichlet dsn.
This function follows the pmf given in "Discrete Multivariate Distributions"
Equation 35.152 on page 80, though we denote n_i by N_i and n by SumN
and alpha_i by a_i and a_dot by Sum_a, and k by K (the number of categories)
A big note here is that sometimes it will be convenient to allow for categories
that "don't exist."  In other words, if one of the a_i's is zero, then the corresponding
N_i had better be zero, and if it is, then we will want to go ahead and 
calculate the probability of a random vector with only the K-1 components.

This version is intended for N on the order of 2 to 8 or so.  It is more efficient than
LogCMDPMF() in such cases if K is large.  

If the SumOfN's (i.e. the number of balls drawn from the urn) has been computed elsewhere and 
is available, then it can be included.  Same with the SumOfAs.  If they aren't available, then
negative values should be put in (like -1's).  

\author  Eric C. Anderson
\date Created 2006-7-15
*/

double 
CMDPMF_small_N(int *N, double *a, int K, int SumOfNs, double SumOfAs)
{
	int i,k;
	int SumN=0;
	double Sum_a=0.0;
	double numer = 1.0, denom=1.0;  /*  these will accumulate the result */
	
	/* if need be,  compute SumN and Sum_a */
	if(SumOfNs<0) {
		for(i=0;i<K;i++)  {
			SumN += N[i];
		}
	}
	else {
		SumN = SumOfNs;
	}
	if(SumOfAs<0) {
		for(i=0;i<K;i++)  {
			Sum_a += a[i];
		}
	}
	else {
		Sum_a = SumOfAs;
	}
	
	
	/*  Make a pass to compute the products of the numerator and denominator, and also
	    check to make sure that there aren't any negative
	    numbers there  */
	for(i=0;i<K;i++)  {
		if(a[i] < 0.0 || N[i] < 0) {  /*  we do not allow negative parameters or variables */
			fprintf(stderr,"\n\na[%d] or N[%d] < zero in LogCMDPMF()", i,i);
			fprintf(stderr,"Exiting to system...");
			exit(1);
		}
		if(N[i] > 0)  {  /* we just disregard N[i]==0 */
			if(a[i]>0.0) {
				for(k=N[i]-1;k>=0;k--) {
					numer *= (k + a[i]);
				}
				denom *= Factorial(N[i]);
			}
			else { /* we can't tolerate N[i]>0 with a[i]==0 */
				fprintf(stderr,"Error in LogCMDPMF().  a[%d]==0 but N[%d]=%d (>0). Exiting...\n",i,i,N[i]);
			}
		}
	}
	
	/* and down here we multiply in the normalizing constant part */
	numer *= Factorial(SumN);
	for(k=SumN-1;k>=0;k--)  {
		denom *= (k + Sum_a);
	}
	
	
	return(numer/denom);
}



/*!
\brief Computes the log of the probability of a Dirichlet-compound multinomial random vector

This function returnes the log of the probability of a Dirichlet-compound multinomial random
vector N, given the parameters (a_1,...,a_K) of the Dirichlet dsn.
This function follows the pmf given in "Discrete Multivariate Distributions"
Equation 35.152 on page 80, though we denote n_i by N_i and n by SumN
and alpha_i by a_i and a_dot by Sum_a, and k by K (the number of categories)
A big note here is that sometimes it will be convenient to allow for categories
that "don't exist."  In other words, if one of the a_i's is zero, then the corresponding
N_i had better be zero, and if it is, then we will want to go ahead and 
calculate the probability of a random vector with only the K-1 components.
\author  Eric C. Anderson
\date Created 2000-11-02
*/

double 
LogCMDPMF(int *N, double *a, int K)
{
	int i;
	int SumN=0;
	double Sum_a=0.0;
	double Result = 0.0;  /*  this will accumulate the result (a sum of logs of  */
						  /*  the factors in Equation 35.152) */
	
	/*  First we compute SumN and Sum_a, and check to make sure that there aren't any negative */
	/*  numbers there  */
	for(i=0;i<K;i++)  {
		SumN += N[i];
		Sum_a += a[i];
		if(a[i] < 0.0 || N[i] < 0) {  /*  we do not allow negative parameters or variables */
			printf("\n\na[%d] or N[%d] < zero in LogCMDPMF()", i,i);
			printf("Exiting to system...");
			exit(1);
		}
	}
	
	/*  then we compute the log of the pmf */
	/*  first do the coefficient part: */
	Result = LogFactorial(SumN) + LogGamma(Sum_a) - LogGamma(SumN + Sum_a);
	for(i=0;i<K;i++)  {
		if(a[i] > 0.0)  {
			Result +=  LogGamma(N[i] + a[i]) - LogFactorial(N[i]) - LogGamma(a[i]);
		}
		else {  /*  in this case, a[i] == 0.0 */
			if(N[i] > 0) {
				printf("\n\nN[%d] > 0 while a[i] == 0 in LogCMDPMF()", i);
				printf("Exiting to system...");
				exit(1);
			}
		}
	}  /*  closes loop over i */
	
	return(Result);
}


double 
LogPEUrnModelPMF(int *N, int *X, double s, int K)
{
/*************
*  FUNCTION:  
*  PURPOSE:   To compute the log of the probability of a Dirichlet-compound multinomial random
			  vector N, given the parameters in the form of a Polya Urn Model.
			  
			  X is an array of ints giving the initial number of balls of K different colors in the urn 
			  s is a double which is the number of balls of like color to the one drawn
			  that get stochastically replaced (along with the original drawn ball). 
			  N is an array of ivals of the number of balls drawn of different colors 
			  
			  Hence s == 0.0 is Multinomial, but we can't use this function to compute 
			  the multinomial probability function because the LogGamma function will croak
			  on the thing.
			  
			  We allow for colors to exist amongst the K, even if there aren't any
			  initial balls of that color.  This is done just by calling LogCompMultProb. 
   
*  AUTHOR:  Eric C. Anderson
*  DATE:  11/02/00 Created.  
***************/

	int i;
	double *alpha;  /*  for an array of alphas */
	double result;
	
	alpha = (double *)ECA_CALLOC(K,sizeof(double));
	

	for(i=0;i<K;i++)  {
		alpha[i] = (double)X[i]/s;
	}
	
	result = LogCMDPMF(N,alpha,K);
	
	free(alpha);
	
	return(result);
}



double 
LogMultinomUrnPMF(int *N, int *X, int K)
{
/*************
*  FUNCTION:  
*  PURPOSE:   To compute the log of the probability of a multinomial random
			  vector N, given the parameters in the form of a Polya Urn Model.
			  (i.e. with the counts in the urn from which one is drawing, and
			  assuming that the stochastic replacement quantity is zero.---so
			  sampling is with replacement)
			  
			  X is an array of ints giving the initial number of balls of K different colors in the urn 
 			  N is an array of ints of the number of balls drawn of different colors 
			  K is the number of categories
			  
			  We allow for colors to exist amongst the K, even if there aren't any
			  initial balls of that color, so long as there aren't any balls of that
			  color drawn!   
   
*  AUTHOR:  Eric C. Anderson
*  DATE:    Created 4/16/01
***************/

	int i;
	double *p;  /*  for an array of proportions */
	double X_sum=0.0;
	int N_sum = 0;
	double result;
	
	p = (double *)ECA_CALLOC(K,sizeof(double));
	
	for(i=0;i<K;i++)  {
		X_sum += (double)X[i];
		N_sum += N[i];
	}
	
	for(i=0;i<K;i++)  {
		p[i] = (double)X[i]/X_sum;
	}
	
	/*  start with the factorial of the number of draws: */
	result = LogFactorial(N_sum);
	
	/*  then cycle over the different categories */
	for(i=0;i<K;i++)  {
		if(p[i] > 0.0)  {
			result +=     (double)N[i] 
					   *  log(p[i])
					   -  LogFactorial(N[i]);
		}
		else {  /*  in this case, p[i] == 0.0 */
			if(N[i] > 0) {
				printf("\n\nN[%d] = %d (> 0) while p[%d] == 0.0 in LogMultinomUrnPMF()",
						i,N[i],i);
				printf("Exiting to system...");
				exit(1);
			}
		}
	}  /*  closes loop over i */
	
	
	free(p);
	
	return(result);
}


/*!
\brief Computes the log of the pmf of a multivariate hypergeometric random variable

To compute the log of the probability of a multivariate hypergeometric random
vector N, given the parameters in the form of a Polya Urn Model.
(i.e. with the counts in the urn from which one is drawing, and
assuming that the stochastic replacement quantity is -1.---so
sampling is without replacement)


We allow for colors to exist amongst the K, even if there aren't any
initial balls of that color, so long as there aren't any balls of that
color drawn!   

\param X An array of ints giving the initial number of balls of K different colors in the urn 
\param N An array of ints of the number of balls drawn of different colors 
\param K  The number of categories

   
\author Eric C. Anderson
\date Created 4/16/01
\callgraph
*/
double 
LogHGUrnPMF(int *N, int *X, int K)
{
	int i;
	int X_sum=0;
	int N_sum = 0;
	double result;
	
	
	for(i=0;i<K;i++)  {
		X_sum += X[i];
		N_sum += N[i];
		if(N[i] > X[i])  {
			printf("\n\nN[%d] = %d > X[%d] = %d in LogHGUrnPMF()",
					i,N[i],i,X[i]);
			printf("Exiting to system...");
			exit(1);
		}
	}

	
	/*  start with the normalizing constant: */
	result = LogFactorial(X_sum-N_sum) + LogFactorial(N_sum) - LogFactorial(X_sum);
	
	/*  then cycle over the different categories */
	for(i=0;i<K;i++)  {
		result +=   LogFactorial(X[i])
				  - LogFactorial(X[i] - N[i])
				  - LogFactorial(N[i]);
	}  /*  closes loop over i */
		
	return(result);
}














/*************
*  FUNCTION:  NormalPDF(double mean, double var, double x)
*  PURPOSE:   Return the value of the density function for a given
			  normal random variable with mean and var.  
*
*  INPUT:     double mean -->  the mean of the normal distribution 
			  double var  -->  the variance of the normal distribution
*  			  double x  -->  the value whose density we shall return

	Originally "normal_density"


*************/



double
NormalPDF(double mean, double var, double x)
{
	return(  (1.0/(sqrt(2.0*3.14159*var) ) ) * 
			exp ( (-pow(x-mean,2))/ (2.0*var) ) );
}



/*

	Returns the log of the density of a dirichlet random vector

*/
double
LogDirichletPDF(double *pars, double *rvs, int numComp)
{
	int i;
	double sum=0.0;
	double output=0.0;
	
	for(i=0;i<numComp;i++)  {
		output += ((pars[i]-1.0) * log(rvs[i]) ) - LogGamma(pars[i]);
		sum += pars[i];
	}
	
	return(output + LogGamma(sum));

}

/*
	returns the log PDF of a beta r.v. (special case of dirichlet---but don't have to
	pass arrays
*/
double LogBetaPDF(double alpha, double beta, double x)
{
	double result;
	
	if(alpha<=0.0 || beta<=0.0) {
		fprintf(stderr, "\nalpha = %.3f, beta = %.3f. One is <= 0.0 in LogBetaPDF()\n\n",alpha,beta);
		fprintf(stderr,"...Exiting to system...");
		exit(1);
	}
	
	result = LogGamma(alpha+beta) - LogGamma(alpha) - LogGamma(beta) + (alpha-1.0)*log(x) +
			(beta-1.0)*log(1.0-x);
	return(result);
}


/*
	returns the log of the PDF of a gamma random variable with shape parameter a and scale parameter b at the point x.
	exits if a,b, or x is <= 0
*/
double LogGammaPDF(double x, double a, double b)
{
	if(x<=0 || x<=0 || b<=0) {
		fprintf(stderr,"Error in GammaPDF.  One of a, b, or x is <= 0.  a=%f  b=%f  x=%f\n....Exiting....\n",a, b, x);
	}
	
	return( (a-1)*log(x) - (x/b) - ( a*log(b) + LogGamma(a) ) );
}


/*
	This puts the Cumulative distribution function for a beta binomial r.v. with
	parameters a and b (both positive), and n balls drawn total, into the array P.
	
	i.e. X ~ BetaBinomial(a,b)
	
	It does not allocate memory to P.  That must be done ahead of time.  It must have
	space allocated from subscript 0 to subscript n.  (that is it needs n+1 elements).
	
	At the end P[0] is Prob(X=0)
			   P[1] is Prob(X<=1)
			   .
			   .
			   .
			   P[j] is Prob(X <= j).
			  
	This is pretty simple and straightforward, and computes things a little recursively.
	
*/
void BetaBinomialCDF_Array(double a, double b, int n, double *P)
{
	double z,y=1.0,q,j=0.0;
	int i=0;
	
	z = exp( LogFactorial(n) + LogGamma(a+b) - LogGamma(n + a + b));
	
	q = exp( LogGamma(n+b) - LogFactorial(n) - LogGamma(b));
	
	P[i] = z * y * q;
	
	while(i<n) {
		j += 1.0;
		i++;
		
		y *= (a + j - 1.0)/j;
		
		q *= ( 1.0*n - j + 1.0 ) / (b + 1.0*n - j);
		
		P[i] = P[i-1] + z * y * q; 
		
	}
	
	/* now we want to make sure that these are really properly normalized,
		this step is to account for any weird small errors that make the sum
		slightly less than or equal to one... */
	for(i=0;i<=n;i++)  {
		P[i] /= P[n];
	}
	
}

/*************
*  FUNCTION:  double NormalCDF(double mean, double var, double x)
*  PURPOSE:   NORM(al) LOW(er) TAIL (probability)
*			  Compute the probability that X < x when
			  X is a N(mean,var) random variable.
*
*  INPUT:     double mean -->  the mean of the normal distribution to draw from
			  double var  -->  the variance of the normal distribution to draw from
			  double x  -->  the value of little x
			  
	Originally norm_low_tail			  
			  
*************/



double 
NormalCDF(double mean, double var, double x)
{
	double q;
	
	q = (x-mean)/(sqrt(2.0*var));   /*  q is N(0,1/2). */
	
	
	if(q <= 0.0) {
		return(.5 * ErrFunctComp(-q) );
	}
	else {
		return(.5 + .5 *ErrFunct(q) );
	}

}


/*************
*  FUNCTION:  LogisticCDF(double mu, double beta, double x)
*  PURPOSE:   Return the value F(x;mu,beta) of the cdf for a logistic dsn which is 
			  parametrized in terms of its mean and variance.
			  
*	Originally "norm_up_tail"
*************/

double
LogisticCDF(double mu, double var, double x)
{
	double beta;
	
	beta = sqrt(3.0*var)/3.1415926535897932385;
	
	return( 1.0 /  
		( 1.0 + exp( -(x-mu)/beta))    );
}


/*************
*  FUNCTION:  double NormalInvCDF(double mean, double var, double x)
*  PURPOSE:   NORM(al) LOW(er) TAIL (probability)
*			  Compute the probability that X > x when
			  X is a N(mean,var) random variable.
*
*  INPUT:     double mean -->  the mean of the normal distribution to draw from
			  double var  -->  the variance of the normal distribution to draw from
			  double x  -->  the value of little x
			  
	Originally norm_low_tail
*************/



double 
NormalInvCDF(double mean, double var, double x)
{
	double q;
	
	q = (x-mean)/(sqrt(2.0*var));  /*  q is N(0,1/2). */
	
	
	if(q >= 0.0) {
		return(.5 * ErrFunctComp(q) );
	}
	else {
		return(.5 + .5 * ErrFunct(-q) );
	}

}



/******************************************************************************************/
/*                                                                                        */
/*								RANDOM NUMBER GENERATION FUNCTIONS                        */
/*                                                                                        */  
/******************************************************************************************/


/*
	Functions to generate and write to a seedfile whatever name you wish to use
*/

/*
	Get seeds from the named file.  If that file is not there, then seed it from
	the clock.  Use the seeds to seed ranlib's number generator.  
*/
void SeedFromFile(const char *filename) 
{

	FILE *sfile;
	long seed1, seed2;
	
	/* deal with the random number seeds */
	if( (sfile = fopen(filename,"r"))==NULL) {
		seed1 = (long)time(NULL);
		seed2 = seed1/3;
		printf("SEEDFILE: No file \"%s\".  Using clock to set RNG: %ld %ld\n", filename, seed1,seed2);
	}
	else {
		fscanf(sfile,"%ld %ld",&seed1,&seed2);
		printf("SEEDFILE: Using contents of %s to set RNG: %ld %ld\n", filename,seed1,seed2);
		fclose(sfile);
	}
	setall(seed1,seed2);
}


void QuietSeedFromFile(const char *filename) 
{

	FILE *sfile;
	long seed1, seed2;
	
	/* deal with the random number seeds */
	if( (sfile = fopen(filename,"r"))==NULL) {
		seed1 = (long)time(NULL);
		seed2 = seed1/3;
	}
	else {
		fscanf(sfile,"%ld %ld",&seed1,&seed2);
		fclose(sfile);
	}
	setall(seed1,seed2);
}




/*
	write seeds to the named file
*/
void SeedToFile(const char *filename) 
{
	FILE *sfile;
	long seed1, seed2;
	
	getsd(&seed1,&seed2);
	if( (sfile=fopen(filename,"w"))==NULL) {
		printf("SEEDFILE: WARNING!  Can't open file %s to write seeds %ld %ld to it\n",filename,seed1,seed2);
	}
	else {
		printf("SEEDFILE: Finished on random seeds: %ld %ld : printed to %s\n",seed1,seed2,filename);
		fprintf(sfile,"%ld %ld\n",seed1,seed2);
		fclose(sfile);
	}
}


void QuietSeedToFile(const char *filename) 
{
	FILE *sfile;
	long seed1, seed2;
	
	getsd(&seed1,&seed2);
	if( (sfile=fopen(filename,"w"))==NULL) {
	}
	else {
		fprintf(sfile,"%ld %ld\n",seed1,seed2);
		fclose(sfile);
	}
}



/*!
\brief Generates uniform random integer between a and n.

This function returns a random integer uniformly distributed between a and n,
inclusive.  It uses ranlib's ignuin function which does exactly
the same thing, but I wrote this so it accepts ints instead of longs.
\param  a      low endpoint of the interval (inclusive)
\param	n	   high endpoint of the interval (inclusive)
*/
int 
UniformRV(int a, int n)
{
	return( (int)ignuin( (long)a, (long)n ) );
}



/*!
\brief Draws a discrete uniform integer between a and n, excluding x

This function returns a discrete uniform integer between a and n, inclusive, but 
excluding x.  It issues a warning to stderr if x is not between a and n.  Erroneous
results may occur if this is the case.
\param a		low endpoint of the interval (inclusive)
\param n		high endpoint of the interval (inclusive)
\param x		the value between a and n to be excluded	
*/
int 
DuwoxRV(int a, int n, int x)
{

	int r;
	
	if(x > n || x < a) {
		fprintf(stderr,"\nWARNING: x is not between a and n in duwox(a,n,x)\n");
	}
	
	r = UniformRV(a,n-1);
	
	if(r >= x) {
		r += 1;
	}
	
	return(r);
}



/*!
	\brief Returns the random pair (i,j) where i < j and they are both between Lo and Hi, inclusive
	
	This function returns a random pair of distinct integers, (i,j), where i < j and they 
	are both between Lo and Hi, inclusive
	
*/
void 
OrderedPairRV(int Lo, int Hi, int *i, int *j)
{


	int a,b;
	
	if(Lo >= Hi) {
		printf("Lo = %d >= Hi = %d, in function draw_ordered_pair()",Lo,Hi);
		printf("\nExiting to system...\n\n");
		exit(1);
	}
	else {
		a = UniformRV(Lo,Hi);
		b = DuwoxRV(Lo,Hi,a);
		
		if(a<b) {
			*i = a;
			*j = b;
		}
		else if(a>b) {
			*i = b;
			*j = a;
		}
		else {  /*  in this case a == b */
			printf("\n\ndraw_ordered_pair working incorrectly.\nExiting to System...\n\n");
			exit(1);
		}
	}

}



/*
	Given a vector of n probabilities summing to one, and each
	one corresponding to an index between 0 and n-1 and an integer
	between  lo and lo + n - 1, this 
	function returns the integer drawn randomly from the probabilities.
	
	DOES NOT CHECK TO VERIFY THE PROBS SUM TO ONE!!
*/
int IntFromProbsRV(double *Probs, int lo, int n)
{
	int i;
	double cum = 0.0;
	double rando;
	
	rando = (double)ranf();
	
	for(i=0;i<n;i++)  {
		cum += Probs[i];
		if(cum >= rando)
			break;
	}
	
	if(i==lo+n) i--;  /*  if for some reason it ran over the bounds, reign it in. */
	
	return(lo + i);
}



/*
**********************************************************************
 
This is taken from RANLIB, but I wanted it to operate on an array of 
ints instead of longs, in case there is a difference on some platforms:

	void genprmINT(int *iarray,int larray)
               GENerate random PeRMutation of iarray
                              Arguments
     iarray <--> On output IARRAY is a random permutation of its
                 value on input
     larray <--> Length of IARRAY
**********************************************************************
*/
void genprmINT(int *iarray,int larray)
{
static int i,itmp,iwhich,D1,D2;

    for(i=1,D1=1,D2=(larray-i+D1)/D1; D2>0; D2--,i+=D1) {
        iwhich = ignuin(i,larray);
        itmp = *(iarray+iwhich-1);
        *(iarray+iwhich-1) = *(iarray+i-1);
        *(iarray+i-1) = itmp;
    }
}


/*
	Given a vector of n positive reals, summing to sum, and each
	one corresponding to an index between 0 and n-1, this 
	function returns the integer drawn randomly from the probabilities that one would 
	get if each array entry were divided by sum.
	
	DOES NOT CHECK TO VERIFY THE PROBS SUM TO sum!!
*/
int IntFromDoublesRV(double *Probs, double sum, int n)
{
	int i;
	double cum = 0.0;
	double rando;
	
	rando = (double)ranf() * sum;
	
	for(i=0;i<n;i++)  {
		cum += Probs[i];
		if(cum >= rando)
			break;
	}
	
	if(i==n) i--;  /*  if for some reason it ran over the bounds, reign it in. */
	
	return(i);
}


/* 
	This returns the index of the 
	least element in an array of sorted doubles, A, which is greater
	than P and between lo and hi, inclusive.
	
	lo and hi must be greater than zero.
	
	hi should be greater than lo.
	
	This is based on binary search.
	
	Returns -1 if P > hi.
	
	Only call this if P > A[0].  
*/
int IntFromArray_BinarySearch(double P, double *A, int lo, int hi)
{	
	int mid = (lo + hi)/2;
		
	if(P<=A[lo])
		return(lo);
	else if(P>A[hi])
		return(-1);
	if(lo>=hi)  {
		printf("\nlo >= hi in IntFromArray_BinarySearch().  Quitting!...\n\n)");
		exit(1);
	}
	if(hi == lo + 1)  /*  if we've whittled it down to a single interval */
		return(hi);
	
	else if(P<A[mid])  
		return(IntFromArray_BinarySearch(P,A,lo,mid));
	else if(P>A[mid])
		return(IntFromArray_BinarySearch(P,A,mid,hi));
	else if(P==A[mid])
		return(mid);
	
	/*  returns a -2 if it got here.  This is a problem!!! It should never get here!! */
	return(-2);
}




/*

	generates a dirichlet random vector by normalized gammas


*/
void
DirichletRV(double *par, int n, double *out)
{
	int i;
	double normo=0.0;
	
	for(i=0;i<n;i++)  {
	
		/*  have it print an error message if any of the parameters are less than  */
		/*  _or_ equal to zero. */
		if(par[i] <= 0.0)  {
			printf("\n\nDRAT!! par[%d] <= 0.0 in DirichletRV",i);
		}	
		out[i] = (double)gengam(.5f,(float)par[i]);   /*  draw a gamma with proper scale and shape pars */
										/*  use scale parameter .5 */
										
		
		/*  check here---if any of them are less than or equal to zero, there is */
		/*  a problem.  gengam returns floats that are sometimes equal to zero.  Very stupid! */
		/*  so, the fix here is going to to be, convert those zeroes into 1.0e-44. That is very */
		/*  small (about the limit of floating point precision on many machines.  So, this is not */
		/*  very elegant, but it should work. */
		if(out[i] <= 0.0) {
			/* fprintf(stderr,"\nRealized a component of Dirichlet Random Vector <= 0.0\n"); */
			out[i] = 1.0e-44;
		}
		
		normo += out[i];
	}

	/*  one more loop to normalize those so they sum to one: */
	for(i=0;i<n;i++)  {
		out[i] /= normo;  
		
		
	}
}

void ftnstop(char* msg);
/*
**********************************************************************
 
     void genmul(int n,double *p,int ncat,int *ix)
     GENerate an observation from the MULtinomial distribution
                              Arguments
     N --> Number of events that will be classified into one of
           the categories 1..NCAT
     P --> Vector of probabilities.  P(i) is the probability that
           an event will be classified into category i.  Thus, P(i)
           must be [0,1]. Only the first NCAT-1 P(i) must be defined
           since P(NCAT) is 1.0 minus the sum of the first
           NCAT-1 P(i).
     NCAT --> Number of categories.  Length of P and IX.
     IX <-- Observation from multinomial distribution.  All IX(i)
            will be nonnegative and their sum will be N.
                              Method
     Algorithm from page 559 of
 
     Devroye, Luc
 
     Non-Uniform Random Variate Generation.  Springer-Verlag,
     New York, 1986.
 
**********************************************************************
*/
void Dgenmul(long n,double *p,long ncat,long *ix)

{
static double prob,ptot,sum;
static long i,icat,ntot;
    if(n < 0) ftnstop("N < 0 in GENMUL");
    if(ncat <= 1) ftnstop("NCAT <= 1 in GENMUL");
    ptot = 0.0;
    for(i=0; i<ncat-1; i++) {
        if(*(p+i) < 0.0) ftnstop("Some P(i) < 0 in GENMUL");
        if(*(p+i) > 1.0) ftnstop("Some P(i) > 1 in GENMUL");
        ptot += *(p+i);
    }
    if(ptot > 1.000000000001) {
    	fprintf(stderr,"\nBummer Dude!  ptot = %f in GENMUL\n\n",ptot);
    	ftnstop("Sum of P(i) > 1 in GENMUL");
    }
/*
     Initialize variables
*/
    ntot = n;
    sum = 1.0;
    for(i=0; i<ncat; i++) ix[i] = 0;
/*
     Generate the observation
*/
    for(icat=0; icat<ncat-1; icat++) {
        prob = *(p+icat)/sum;
        *(ix+icat) = ignbin(ntot,(float)prob);
        ntot -= *(ix+icat);
	if(ntot <= 0) return;
        sum -= *(p+icat);
    }
    *(ix+ncat-1) = ntot;
/*
     Finished
*/
    return;
}



/* a multinomial RV function that takes ints and doubles and can deal with
zero-elements in the p-vector */
void MultinomialRV(int n,double *p,int ncat,int *ix)
{
	float *tempP=(float *)calloc(ncat, sizeof(float));
	long *tempix=(long *)calloc(ncat, sizeof(long));
	long z = 0;
	long tempn = (long)n;
	int i;
	int *nz=(int *)calloc(ncat,sizeof(int));
	double tot=0.0;
	
	for(i=0;i<ncat;i++)  {
		ix[i] = 0;
		if(p[i]>0) {
			nz[z]=i;
			tempP[z++] = (float)p[i];
			tot += (double)p[i];
		}
	}
	
	if(z<1) {
		printf("\n\nError in call to MultinomialRV().  z<1\n\n");
		exit(1);
	}
	if(z==1) {
		for(i=0;i<ncat;i++)
			ix[i]=0;
		ix[nz[0]] = n;
		return;
	}
	
	/* here is a thing to catch problems in genmul --- since it does floats, sometimes
	   it can't deal with the sum of everything being exactly 1.0  */
	if(tot >= .9999999) {
		for(i=0;i<z;i++)  {
			tempP[i] *= .99999;
		}
	}
	
	
	genmul(tempn,tempP,z,tempix);
	
	for(i=0;i<z;i++)  
		ix[nz[i]] = (int)tempix[i];
		
	free(tempP);
	free(tempix);
	free(nz);
	
}

/* a multinomial RV function that takes ints and doubles and can deal with
zero-elements in the p-vector, and this one uses my "double-ized" version
of genmul */
void D_MultinomialRV(int n,double *p,int ncat,int *ix)
{
	double *tempP=(double *)calloc(ncat, sizeof(double));
	long *tempix=(long *)calloc(ncat, sizeof(long));
	long z = 0;
	long tempn = (long)n;
	int i;
	int *nz=(int *)calloc(ncat,sizeof(int));
	double tot=0.0;

		
	for(i=0;i<ncat;i++)  {
		ix[i] = 0;
		if(p[i]>0) {
			nz[z]=i;
			tempP[z++] = p[i];
			tot += p[i];
		}
	}
	
	if(z<1) {
		printf("\n\nError in call to D_MultinomialRV().  z<1.  ncat=%d  n=%d. p=: ",ncat,n);
		for(i=0;i<ncat;i++) {
			printf("%f ",p[i]);
		}
		printf("\n");
		exit(1);
	}
	if(z==1) {
		for(i=0;i<ncat;i++)
			ix[i]=0;
		ix[nz[0]] = n;
		return;
	}
	
	/* here is a thing to catch problems in genmul --- since it does floats, sometimes
	   it can't deal with the sum of everything being exactly 1.0  */
	if(tot != 1.0) {
		/* fprintf(stderr, "WARNING: p-vector sums to %.20f, in D_MultinomialRV. Normalized to one.\n",tot); */
		for(i=0;i<z;i++)  {
			tempP[i] /= tot;
		}
	}
	
	
	Dgenmul(tempn,tempP,z,tempix);
	
	for(i=0;i<z;i++)  
		ix[nz[i]] = (int)tempix[i];
		
	free(tempP);
	free(tempix);
	free(nz);
	
}





/* 
	This simulates a compound multinomial-dirichlet random vector with dirichlet
	parameters given in the array alpha.  Note that some of these values can 
	be zero (they are just ignored).  Unlike CompoundDirMultRV which simulates a Dirichlet
	RV and then a Multinomial RV off of that, this function is appropriate for small 
	n (like 2 or 4) and will be more efficient in that case, especially if ncat is
	large.
	
	It does an urn scheme with replacement by sampling from a cumulative sum sort 
	of array, and the "replaced balls" are represented by extra mass on the end of
	that cumulative sum array.
	
*/
void CompoundDirMult_ViaUrnRV(double *alpha, int n, int ncat, int *ix)
{
	int i,k,sel, true;
	double *cumul = (double *)calloc(ncat + n, sizeof(double));
	int *extras = (int *)calloc(n,sizeof(int));
	double max, rando;
	
	/* initialize output vector to 0.0 and make the cumul array */
	for(i=0;i<ncat;i++)  {
		ix[i] = 0;
	}
	cumul[0] = alpha[0];
	for(i=1;i<ncat;i++)  {
		cumul[i] = cumul[i-1] + alpha[i];
	}
	
	/* now start drawing balls from that */
	for(k=0;k<n;k++) {
		max = cumul[ncat-1+k];
		rando = max * ranf() - .0000000000001; /* don't want to get anything identically max */ 
		
		for(i=0;i<ncat+k;i++)  {
			if(cumul[i]>rando) {
				sel = i;
				break;
			}
		}
		
		/* at this point, sel holds the index of the selected cell */
		if(sel<ncat) {
			ix[sel]++;
			cumul[ncat+k] = cumul[ncat+k-1] + 1.0; /* add a new cell */
			extras[k] = sel;
		}
		if(sel>=ncat) {  /* in this case, we just drew a ball that was a stochastic replacement */
			true = extras[sel-ncat];
			ix[true]++;
			cumul[ncat+k] = cumul[ncat+k-1] + 1.0;
			extras[k] = true;
		}
	} 
	
	free(cumul);
	free(extras);
}


/* given K categories, with N_k occurrences in each category (where N_k is a double in the _d version), draw
occurences into X, without replacement n times */
void DrawWithoutReplacement_d(int K, double *N, int *X, int n) 
{
	int i, RV;
	double *Arr,sum=0.0;

	
/* for debugging
printf("In DrawWithoutReplacement_d:  K=%d : n=%d : N= ",K,n);
for(i=0;i<K;i++) printf("%f ",N[i]);
*/

	/* allocate memory to Arr */
	Arr = (double *)calloc(K, sizeof(double));
	 
	/* zero out X, and transfer N to Arr */
	for(i=0;i<K;i++)  {
		X[i] = 0;
		Arr[i] = N[i];
		sum+=Arr[i];
	}
	
	/* now, draw items n times without replacement */
	for(i=0;i<n;i++)  {
		RV = IntFromDoublesRV(Arr, sum, K);
		X[RV]++;
		Arr[RV] -= 1.0;
		sum -= 1.0;
		
		if(Arr[RV]<0.0 || sum <= 0.0) {
			fprintf(stderr,"Error! Arr[RV] = %f and sum = %f in DrawWithoutReplacement_d. Exiting\n",Arr[RV],sum);
			exit(1);
		}
		
	}
	
/* more for debugging 
printf(": X= ");
for(i=0;i<K;i++) printf("%d ",X[i]);
printf("\n");
*/

	free(Arr);
}  

/*  
	This simulates a compound multinomial-dirichlet random vector with dirichlet
	parameters given in the array alpha.  Note that some of these values can 
	be zero.  
*/
void CompoundDirMultRV(double *alpha, int n, int ncat, int *ix)
{
	int i;
	double *p = (double *)calloc(ncat,sizeof(double));
	double *tempalpha=(double *)calloc(ncat, sizeof(double));
	int *tempix=(int *)calloc(ncat, sizeof(int));
	int z = 0;
	int *nz=(int *)calloc(ncat,sizeof(int));
	
	/* condense things into a tempalpha array with no zero's */
	for(i=0;i<ncat;i++)  {
		ix[i] = 0;
		if(alpha[i]>0.0) {
			nz[z]=i;
			tempalpha[z++] = alpha[i];
		}
	}
	
	
	/* deal with special cases, z<1 and z==1 */
	if(z<1) {
		printf("\n\nError in call to CompoundDirMultRV().  z<1\n\n");
		exit(1);
	}
	if(z==1) {
		for(i=0;i<ncat;i++)
			ix[i]=0;
		ix[nz[0]] = n;
		return;
	}
	
	
	DirichletRV(tempalpha, z, p);
	MultinomialRV(n,p,z,tempix);
	
	
	/* then put the zero categories back into the array */
	for(i=0;i<z;i++)  
		ix[nz[i]] = (int)tempix[i];
	
	
	free(tempalpha);
	free(p);
	free(tempix);
	free(nz);
	
}

/* 
	This is a function that returns the numbers of balls of different colors remaining
	in the urn in a standard (s = 1) Polya Urn scheme when the number of balls in there
	is n (having started with sum_N[i]).  
	
	This is particularly useful as giving the distribution of allelic types on the n
	lineages of a coalescent, given that at some time in the past the distribution 
	of allelic types on the lineages was given by the array N.  
	
	N is an array of ints giving the initial number of balls of K different colors in the urn 
			  
	n is an int  -- the number of balls that will be drawn from the urn 
			  
	K is the number of colors.  Note that some of the colors may not be
			  represented in the balls in N.  This is OK!  We still allow that.
			  
	ix is an array where the result (the numbers of different colored balls that
			  	remain in the urn) is returned.
*/
void CoalescentUrnRV(int *N, int n, int K, int *ix)
{
	int i,R=0;
	
	/* first count R, the number of balls initially in the urn */
	for(i=0;i<K;i++)  {
		R += N[i];
	}
	
	/* if R is greater than n, we quit */
	if(R > n) {
		printf("\n\nError!  R greater than n in CoalescentUrnRV()\n\nExiting\n\n");
		exit(1);
	}
	if(R==n)  { /* in this case we just return N for ix */
		for(i=0;i<K;i++)  {
			ix[i] = N[i];
		}
		return;
	}
	else {
		PolyaUrnModelRV(N,n-R,1.0,K,ix);
		for(i=0;i<K;i++)  {
			ix[i] += N[i];
		}
	}
}



void PolyaUrnModelRV(int *N, int n, double s, int ncat,  int *ix)
{
/*************
*  FUNCTION:  
*  PURPOSE:   To simulate a realization from a Polya Urn Model.
			  
			  N is an array of ints giving the initial number of balls of K different colors in the urn 
			  
			  n is an int  -- the number of balls that will be drawn from the urn 
			  
			  s is a double which is the number of balls of like color to the one drawn
			  that get stochastically replaced (along with the original drawn ball). 
			  
			  ncat is the number of colors.  Note that some of the colors may not be
			  represented in the balls in N.  This is OK!  We still allow that.
			  
			  ix is an array where the result (the numbers of different colored balls that
			  	will have been drawn from the urn) is returned.
			  
			  Hence s == 0.0 is Multinomial, but we can't use this function to compute 
			  the multinomial probability function because the LogGamma function will croak
			  on the thing.
			  
			  And s == 1.0 is a standard Polya urn scheme.
			  
			  
   
*  AUTHOR:  Eric C. Anderson
*  DATE:  11/29/03 Created.  
***************/

	int i;
	double *alpha = (double *)calloc(ncat,sizeof(double));
	double *p = (double *)calloc(ncat,sizeof(double));
	int *tempN=(int *)calloc(ncat, sizeof(int));
	int *tempix=(int *)calloc(ncat, sizeof(int));
	int z = 0;
	int *nz=(int *)calloc(ncat,sizeof(int));
	
	/* condense things into an N array with no zero's */
	for(i=0;i<ncat;i++)  {
		ix[i] = 0;
		if(N[i]>0) {
			nz[z]=i;
			tempN[z++] = N[i];
		}
	}
	
	
	/* deal with special cases, z<1 and z==1 */
	if(z<1) {
		printf("\n\nError in call to PolyaUrnModelRV().  z<1\n\n");
		exit(1);
	}
	if(z==1) {
		for(i=0;i<ncat;i++)
			ix[i]=0;
		ix[nz[0]] = n;
		return;
	}
	
	/* determine parameters for corresponding Compound Multinomial */
	for(i=0;i<z;i++)  {
		alpha[i] = (double)tempN[i]/s;
	}
	
	DirichletRV(alpha, z, p);
	MultinomialRV(n,p,z,tempix);
	
	
	/* then put the zero categories back into the array */
	for(i=0;i<z;i++)  
		ix[nz[i]] = (int)tempix[i];
	
	
	free(p);
	free(alpha);
	free(tempN);
	free(tempix);
	free(nz);
	
}


/* this returns a negative binomial random variable. The parameterization is 
one in which the NegBinom arises as a Poisson whose mean follows a gamma
distribution with shape parameter A (alpha) and scale parameter B (beta).

This is the parameterization in which the mean of the gamma dsn is AB and
the variance is AB^2.
*/
int NegBinomAB_RV(double A, double B)
{
	float lambda;
	
	lambda = gengam( 1.0/(float)B, (float)A);
	
	return(ignpoi(lambda));
	
}



/* POPULATION GENETIC QUANTITIES OF SORTS */


/* this computes the P(A_t=j|A_0=i).  i.e. g_{ij}(t) in Tavare's 1984 paper.
This is the probability that if you start with i lineages at time 0, you will
have j lineages left at time t.
Note that time is in units of 2N generations (or otherwise scaled appropriately). */
double Tavare1984prob(int i, int j, double t)
{
	int k;
	double sum=0.0;
	
	if(j>i || j<1)  return(0.0);
	
	else if(j>1) {
		for(k=j;k<=i;k++)  {
			sum += Tavare1984summand(t,i,j,k);
		}
	}
	else { /* j==1 */
		for(k=2;k<=i;k++)  {
			sum += Tavare1984summand(t,i,j,k);
		}
		sum = 1.0 - sum;
	}
	
	return(sum);  
}

/* 
	this computes the summand in Tavare's 1984 probability of having 
	j lineages at time t given you started with i lineages 
	
	This computes the log of a summand, then exponentiates it, and determines
	if it shall be positive or negative according to the -1^(k-j).
	We handle the case j==1 a little differently as can be seen below.
*/
#define SMALLEST_1984 -300
double Tavare1984summand(double t, int i, int j, int k)
{
	double easy, hairy, sign=1.0;
	
	
	easy = -k*(k-1)*t/2.0 + log(2.0*k - 1.0);
	
	/* determine the sign */
	if( (k-j)%2==1 ) sign = -1.0;
	
	if(j>1) {
		hairy = LogFactorial(j+k-2) - LogFactorial(j-1)
			  + LogFactorial(i) - LogFactorial(i-k)
			  - LogFactorial(j)
			  - LogFactorial(k-j) 
			  - LogFactorial(i+k-1) + LogFactorial(i-1);
	}
	else if(j==1) {
		hairy = LogFactorial(i) - LogFactorial(i-k) 
			  - LogFactorial(i+k-1) + LogFactorial(i-1);
		
		/* the sign gets set too */
		if(k%2==1)  sign = -1.0;
		else 		sign = 1.0;
	}
	
	/* this is a little something to catch underflow.  If the size of this
	  number (once it has been exponentiated) is less than the smallest double
	  that can be represented on the system, then we call it zero */
	  
	 /* this is not sensitive to the type of system because I could't take logs of 
	  DBL_MIN.  So, instead, anything that has a log10 smaller than -300 is called zero */ 
	
/*	printf("\nSummand: %d  %d  %d    %f    %e \n",i,j,k,(easy+hairy)/log(10), sign * exp( easy + hairy ) ); */
	
	if( (easy+hairy)/log(10) < SMALLEST_1984) {
		return(0.0);
	}	  
	
	
	return( sign * exp( easy + hairy ) );
	
}
#undef SMALLEST_1984



/******************************************************************************************/
/*                                                                                        */
/*								LITTLE MATHEMATICAL UTILITIES                             */
/*                                                                                        */  
/******************************************************************************************/




/*************
*  FUNCTION:  ParabolicMaxInterp()
*  PURPOSE:   Given three points, fit a  parabola to them and then return the max y
*			  and the x that gives the max y.  It returns the max y and sends back
			  the corresponding x as an output variable.
*
*  INPUT:     int a  -->  low endpoint of the interval (inclusive)
*			  int n  -->  high endpoint of the interval (inclusive)
*************/

double ParabolicMaxInterp(double x1, double x2, double x3, 
						double y1, double y2, double y3, double *max_x)
{
	double A,B,C;
	
	A = -((-(x2*y1) + x3*y1 + x1*y2 - x3*y2 - 
          x1*y3 + x2*y3)/
        ((x1 - x2)*(x1*x2 - x1*x3 - x2*x3 + pow(x3,2))));
        
    B = -((pow(x2,2)*y1 - pow(x3,2)*y1 - 
          pow(x1,2)*y2 + pow(x3,2)*y2 + pow(x1,2)*y3 - 
          pow(x2,2)*y3)/
        ((x1 - x2)*(x1*x2 - x1*x3 - x2*x3 + pow(x3,2))));
        
    C = -((-(pow(x2,2)*x3*y1) + x2*pow(x3,2)*y1 + 
          pow(x1,2)*x3*y2 - x1*pow(x3,2)*y2 - 
          pow(x1,2)*x2*y3 + x1*pow(x2,2)*y3)/
        (pow(x1,2)*x2 - x1*pow(x2,2) - pow(x1,2)*x3 + 
          pow(x2,2)*x3 + x1*pow(x3,2) - x2*pow(x3,2)));
          
    *max_x = -B / (2.0 * A);
    
    return(A * pow(*max_x,2) + B * (*max_x) + C);
    
}


/*************
*  FUNCTION:  StringToIntArray()
*  PURPOSE:   Given a string of nonegative numbers in ascending order separated 
			  by ,'s or -'s, (like "1-5,7,10-15"), this function will
			  return an array that is at most top+1 in length that has 1's
			  for all values that are in the range implied by the string
			  and 0's elsewhere.  If verbose==1 this prints a warning when your 
			  range extends beyond top.

*************/
int *StringToIntArray(char *Str, int bottom, int top, int verbose)
{
	char *temp,*thedash;
	int lo=-1,hi=-1,i,now=0,old=-1;
	int *result = (int *)ECA_CALLOC(top+1,sizeof(int));
	
	if(strcmp(Str,"all")==0 || strcmp(Str,"All")==0 || strcmp(Str,"ALL")==0) {
		for(i=bottom;i<=top;i++)  {
			result[i] = 1;
		}
		return(result);
	}
	
	
	temp = (char *)ECA_CALLOC(strlen(Str)+10,sizeof(char));
	
	sprintf(temp,"%s",Str);
	
	temp=strtok(temp,",");
	
	while(temp) {
		
		if(strchr(temp,'-'))  {
			thedash = strchr(temp,'-');
			hi = atoi(thedash+1);
			*thedash = '\0';
			lo = atoi(temp);
			if(lo>=hi) {
				fprintf(stderr,"Error in StringToIntArray().  Number string %s includes a range that is non-increasing\n",Str);
				exit(1);
			}
			for(i=ECA_MAX(lo,bottom);i<=hi && i<=top;i++)  {
				result[i] = 1;
			}
			if(lo>top && verbose) {
				printf("WARNING : in StringToIntArray().  Range lo value = %d is greater than top = %d\n",lo,top);
			}
			if(hi>top && verbose) {
				printf("WARNING : in StringToIntArray().  Range hi value = %d is greater than top = %d\n",hi,top);
			}
			if(lo<bottom && verbose) {
				printf("WARNING : in StringToIntArray().  Range lo value = %d is less than bottom = %d\n",lo,bottom);
			}
			if(hi<bottom && verbose) {
				printf("WARNING : in StringToIntArray().  Range hi value = %d is less than bottom = %d\n",hi,bottom);
			}
		}
		else {	
			now = atoi(temp);
			if(now<=hi || now<=old) {
				fprintf(stderr,"Error in StringToIntArray().  Number string %s includes a singleton that is non-increasing\n",Str);
				exit(1);
			}
			if(now<=top && now>=bottom) {
				result[now] = 1;
			}
			else if(now>top && verbose) {
				printf("WARNING : in StringToIntArray().  Singleton value = %d is greater than top = %d\n",now,top);
			}
			else if(now<bottom && verbose) {
				printf("WARNING : in StringToIntArray().  Singleton value = %d is less than bottom = %d\n",now,bottom);
			}
			old=now;
		}
		temp=strtok(NULL,",");
	}
	return(result);
}
