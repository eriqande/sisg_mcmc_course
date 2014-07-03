/*!
\file MathStatRand.h
\brief Header for Eric Anderson's MathStatRand library.

This is a C header file for make Eric Anderson's 
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


/* Some #defined things */
#define ECA_ABS(x) ((x) >= 0 ? (x) : -(x))
#define ECA_MIN(a,b) ((a) <= (b) ? (a) : (b))
#define ECA_MAX(a,b) ((a) >= (b) ? (a) : (b))



/* MATHEMATICAL FUNCTIONS */
extern double LogGamma(double x);
extern double IncGammaCF(double a, double x);
extern double IncGammaSer(double a, double x);
extern double GammaP(double a, double x);
extern double GammaQ(double a, double x) ;
extern double ErrFunct(double x);
extern double ErrFunctComp(double x);
extern double ltqnorm(double p);  /* inverse left tail prob of standard normal */
extern double Factorial(int x);
extern double LogFactorial(int x);
double LogBinomCoeff(int n, int x);
extern double RecursiveLogStirling3(double ***M, int n, int i, int MaxN);

/* STATISTICAL FUNCTIONS */
extern double LogBinomialPMF(int n, double p, int x);
extern double LogMultinomialPMF(int n, double *p, int *x, int inNum);
extern double CMDPMF_small_N(int *N, double *a, int K, int SumOfNs, double SumOfAs);
extern double LogCMDPMF(int *N, double *a, int K);
extern double LogPEUrnModelPMF(int *N, int *X, double s, int K);
extern double LogMultinomUrnPMF(int *N, int *X, int K);
extern double LogHGUrnPMF(int *N, int *X, int K);
extern double NormalPDF(double mean, double var, double x);
extern double LogDirichletPDF(double *pars, double *rvs, int numComp);
extern double LogBetaPDF(double alpha, double beta, double x);
double LogGammaPDF(double x, double a, double b);
extern void BetaBinomialCDF_Array(double a, double b, int n, double *P);
extern double NormalCDF(double mean, double var, double x);
extern double LogisticCDF(double mu, double var, double x);
extern double NormalInvCDF(double mean, double var, double x);


/* RANDOM NUMBER GENERATION FUNCTIONS */
void SeedFromFile(const char *filename);
void QuietSeedFromFile(const char *filename);
void SeedToFile(const char *filename);
void QuietSeedToFile(const char *filename); 
int UniformRV(int a, int n);
int DuwoxRV(int a, int n, int x);
extern void OrderedPairRV(int Lo, int Hi, int *i, int *j);
void genprmINT(int *iarray,int larray);
extern int IntFromProbsRV(double *Probs, int lo, int n);
extern int IntFromDoublesRV(double *Probs, double sum, int n);
extern int IntFromArray_BinarySearch(double P, double *A, int lo, int hi);
extern void DirichletRV(double *par, int n, double *out);
extern void Dgenmul(long n,double *p,long ncat,long *ix);
extern void MultinomialRV(int n,double *p,int ncat,int *ix);
extern void D_MultinomialRV(int n,double *p,int ncat,int *ix);
extern void DrawWithoutReplacement_d(int K, double *N, int *X, int n);
extern void CompoundDirMult_ViaUrnRV(double *alpha, int n, int ncat, int *ix);
extern void CompoundDirMultRV(double *alpha, int n, int ncat, int *ix);
extern void CoalescentUrnRV(int *N, int n, int K, int *ix);
extern void PolyaUrnModelRV(int *N, int n, double s, int ncat,  int *ix);
extern int NegBinomAB_RV(double A, double B);


/* POPULATION GENETIC QUANTITIES OF SORTS */
double Tavare1984prob(int i, int j, double t);
double Tavare1984summand(double t, int i, int j, int k);


/* Little mathematical utilities */
double ParabolicMaxInterp(double x1, double x2, double x3, 
						double y1, double y2, double y3, double *max_x);
int *StringToIntArray(char *Str, int bottom, int top, int verbose);
