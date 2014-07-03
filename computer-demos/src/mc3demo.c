#define UN_EXTERN



#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "ranlib.h"

#include "MathStatRand.h"
#include "MCTypesEtc.h"

#include "GFMCMC.h"
#include "ECA_MemAlloc.h"


#define NUMP 1000 /* how many points to use to make the plot of the mixture density */



/* prototypes */
void mc3demo_Console(void);
double mixdens(double x, double *p, double *m, double *v, int k);
void DrawMixDens(void);
void DrawMixDensBetas(void);
void ChainUpdate(double *x, double beta);
void SwapRandomChains(void);
void DrawParticleHists(hist_struct *H, double Total, double Current);
void DrawColdChain(void);
void DrawHotChain(void);
void DrawDensityJ(int j);
void DrawChain0(void);
void DrawChain1(void);
void DrawChain2(void);
void DrawChain3(void);
void DrawChain4(void);
void ToggleCoupling(unsigned char key, int x, int y);


/* some global variables */
double gPi[1000]; /* mixing proportions for the different normal dists */
double mu[1000]; /* means  */
double var[1000]; /* variances */
int K; /* the numbe of components */
double gLo, gHi;  /* the area over which you want to do the integration */
double gMaxD;  /* maximum value of the mixture density */
double gMaxDBetas;  /* the max value over all values of beta */
double MixXs[NUMP];  /* x's for displaying the density */
double MixYs[NUMP];	/* y's for displaying the density */
double Xs[NUMP], **Ys;  /* x's and y's for the densities raised to different betas */
double Betas[1000];
int NumBetas;  /* note that the first beta should always be 1.0 */
double BetaScales[1000];  /* for scaling the densities so they have the same area.   */
dval *StatXCold;  /* for the X values that get simulated but aren't M-coupled to anything */
dval *StatXHot; /* for a hotter version that won't be getting coupled anywhere */
dval **CoupX;  /* for the X values that get simulated and are M-coupled */
double HotBeta;  /* the beta for the StatXHot chain */
int HotBetaIdx;
int gDoCoupled = 0;
int gSwI=0, gSwJ=0, gSwitched=0;  /* for recording which were involved in the switch, and whether the swap was successful */

void gfmUserDefd_ResetAllAverages(void)
{
	int i;
	double temp;
	
	temp = StatXCold->v;
	InitDvalToZero(StatXCold);
	StatXCold->v = temp;
	
	temp = StatXHot->v;
	InitDvalToZero(StatXHot);
	StatXHot->v = temp;
	
	for(i=0;i<NumBetas;i++)  {	
		temp = CoupX[i]->v;
		InitDvalToZero(CoupX[i]);
		CoupX[i]->v = temp;
	}
	
}

void gfmUserDefd_InitializeChain(void)
{
	int i;

	gfmUserDefd_ResetAllAverages();
	
	StatXCold->v = ranf() * 63.0 + 1.0;
	StatXHot->v = ranf() * 63.0 + 1.0;
	
	for(i=0;i<NumBetas;i++)  {
		CoupX[i]->v = ranf() * 63.0 + 1.0;
	}
	
} 
 
void gfmUserDefd_OneStepByGlobals(void)
{
	int i;
	
	ChainUpdate(&(StatXCold->v),1.0);
	ChainUpdate(&(StatXHot->v),HotBeta);
	IncrementDval(StatXCold);
	IncrementDval(StatXHot);
	
	
	for(i=0;i<NumBetas;i++)  {
		ChainUpdate(&(CoupX[i]->v),Betas[i]);
		IncrementDval(CoupX[i]);
	}
	
	if(gDoCoupled==1) {
		SwapRandomChains();
	}
	
	return;		
}

void gfmUserDefd_DefineWindows(void)
{
	
	
	// name the views files 
	gsOUTPUT_FILE_PREFIX("mc3demo");
	
	gsCONSOLE("\"Info\" Window");
		gsDRAW_FUNC(mc3demo_Console);
		
		
	gsNEW_WINDOW(1,	"mixture density");
		gsDRAW_FUNC(DrawMixDens);
		gsXLO(-.1 * (gHi-gLo) );
		gsXHI(1.1 * gHi);
		gsYHI(gMaxD * 1.10);
		gsYLO(-.1 * gMaxD);
		gsAXES_POS_QUADRANT;
		gsCOLOR_SCHEME(DEEP_BLUE);

		
	gsNEW_WINDOW(2,	"mixture densities with betas");
		gsDRAW_FUNC(DrawMixDensBetas);
		gsXLO(-.1 * (gHi-gLo) );
		gsXHI(1.1 * gHi);
		gsYHI(gMaxDBetas * 1.10);
		gsYLO(-.1 * gMaxDBetas);
		gsAXES_POS_QUADRANT;
		gsCOLOR_SCHEME(DEEP_BLUE);

		
	gsNEW_WINDOW(3,	"Cold Chain");
		gsDRAW_FUNC(DrawColdChain);
		gsXLO(-.1 * (gHi-gLo) );
		gsXHI(1.1 * gHi);
		gsYHI(gMaxD * 1.10);
		gsYLO(-.1 * gMaxD);
		gsAXES_POS_QUADRANT;
		gsCOLOR_SCHEME(DEEP_BLUE);
		gsKEYBOARD_FUNC(ToggleCoupling);
		
	gsNEW_WINDOW(4,	"Hot Chain");
		gsDRAW_FUNC(DrawHotChain);
		gsXLO(-.1 * (gHi-gLo) );
		gsXHI(1.1 * gHi);
		gsYHI(gMaxD * 1.10);
		gsYLO(-.1 * gMaxD);
		gsAXES_POS_QUADRANT;
		gsCOLOR_SCHEME(DEEP_BLUE);
		gsKEYBOARD_FUNC(ToggleCoupling);
		
	
	gsNEW_WINDOW(5,	"Beta = 1");
		gsDRAW_FUNC(DrawChain0);
		gsXLO(-.1 * (gHi-gLo) );
		gsXHI(1.1 * gHi);
		gsYHI(gMaxD * 1.10);
		gsYLO(-.1 * gMaxD);
		gsAXES_POS_QUADRANT;
		gsCOLOR_SCHEME(DEEP_BLUE);
		gsKEYBOARD_FUNC(ToggleCoupling);
		
		
	gsNEW_WINDOW(6,	"Beta = 0.5");
		gsDRAW_FUNC(DrawChain1);
		gsXLO(-.1 * (gHi-gLo) );
		gsXHI(1.1 * gHi);
		gsYHI(gMaxD * 1.10);
		gsYLO(-.1 * gMaxD);
		gsAXES_POS_QUADRANT;
		gsCOLOR_SCHEME(DEEP_BLUE);
		gsKEYBOARD_FUNC(ToggleCoupling);
		
		
	gsNEW_WINDOW(7,	"Beta = 0.15");
		gsDRAW_FUNC(DrawChain2);
		gsXLO(-.1 * (gHi-gLo) );
		gsXHI(1.1 * gHi);
		gsYHI(gMaxD * 1.10);
		gsYLO(-.1 * gMaxD);
		gsAXES_POS_QUADRANT;
		gsCOLOR_SCHEME(DEEP_BLUE);
		gsKEYBOARD_FUNC(ToggleCoupling);
		
		
	gsNEW_WINDOW(8,	"Beta = 0.03");
		gsDRAW_FUNC(DrawChain3);
		gsXLO(-.1 * (gHi-gLo) );
		gsXHI(1.1 * gHi);
		gsYHI(gMaxD * 1.10);
		gsYLO(-.1 * gMaxD);
		gsAXES_POS_QUADRANT;
		gsCOLOR_SCHEME(DEEP_BLUE);
		gsKEYBOARD_FUNC(ToggleCoupling);
		
	gsNEW_WINDOW(9,	"Beta = 0.0");
		gsDRAW_FUNC(DrawChain4);
		gsXLO(-.1 * (gHi-gLo) );
		gsXHI(1.1 * gHi);
		gsYHI(gMaxD * 1.10);
		gsYLO(-.1 * gMaxD);
		gsAXES_POS_QUADRANT;
		gsCOLOR_SCHEME(DEEP_BLUE);
		gsKEYBOARD_FUNC(ToggleCoupling);
		
		
		
}


void ToggleCoupling(unsigned char key, int x, int y)
{
	switch(key) {
		case('c'): 
			gDoCoupled = (gDoCoupled + 1) % 2;
			break;
		default:
			gfmProcessNormalKeys(key, x, y);
	}
}

void gfmUserDefd_DefineMenus(void)    // to define menus for the right click of the mouse
{
	return;  // no special right click menus for now
}


void gfmUserDefd_LastWords(void)
{
	return;  // no special last words here
}


void mc3demo_Console(void)
{
	char temp[250];
	int CW = glutGetWindow();	
	ColorScheme3f *CS = gWindowsSettings[CW]->ColorScheme;

	
	
	glColor3fv(CS->Text);
	gfmRenderBitmapString(-.1f,1.0f,GLUT_BITMAP_HELVETICA_12, "Program: tpm");
	gfmRenderBitmapString(-.1f,.9f,GLUT_BITMAP_HELVETICA_12, "Author: E.C. Anderson");
	
	
	sprintf(temp,"Num of Iterations: %d",gNumSweeps);
	gfmRenderBitmapString(-.1f,.3f,GLUT_BITMAP_HELVETICA_12, temp);
	
	if(gGo) 
		sprintf(temp,"RUNNING!");
	else 
		sprintf(temp,"STOPPED!");
		
	gfmRenderBitmapString(-.1f,.2f,GLUT_BITMAP_HELVETICA_12, temp);
	
}


/* return the density at x, given the mixture of normals */
double mixdens(double x, double *p, double *m, double *v, int k)
{
	int i;
	double temp = 0.0;
	
	for(i=0;i<k;i++)  {
		temp += p[i] * NormalPDF(m[i],v[i],x);
	}
	
	return(temp);
}


void SwapRandomChains(void)
{
	int i,j;
	double xi, xj, betai, betaj;
	double MH;
	
	OrderedPairRV(0,NumBetas,&i,&j);
	
	xi = CoupX[i]->v;
	xj = CoupX[j]->v;
	betai = Betas[i];
	betaj = Betas[j];
	
	MH = pow(mixdens(xj,gPi,mu,var,K),betai) *  pow(mixdens(xi,gPi,mu,var,K),betaj)  / 
		( pow(mixdens(xi,gPi,mu,var,K),betai) * pow(mixdens(xj,gPi,mu,var,K),betaj) );
		
	gSwitched = 0;
	if(ranf() < MH) {
		CoupX[j]->v = xi;
		CoupX[i]->v = xj;
/*		gSwitched=1; This is silly---you don't want all that color flashing. */  
	}
	
	gSwI = i;
	gSwJ = j;
	
	
}

void ChainUpdate(double *x, double beta)
{
	double sd = 1.1;
	double newx,oldx=*x;
	
	/* propose a new value */
	newx = gennor((float)oldx,(float)sd);
	if(newx < 0 || newx > 65) /* reject out of hand if it is out of bounds */
		return;
		
	if(ranf() < pow(  mixdens(newx,gPi,mu,var,K),beta) / pow(mixdens(oldx,gPi,mu,var,K),beta) ) {
		*x = newx;
	}
}

void DrawMixDens(void)
{
	int i;
	gsCS;
	
	glColor3fv(CS->Series[0]);
	glBegin(GL_LINE_STRIP);
		for(i=0;i<NUMP;i++)  {
			glVertex2f((GLfloat)MixXs[i],(GLfloat)MixYs[i]);
		}
	glEnd();
	
	gfmDrawXYAxes();
}

void DrawDensityJ(int j)
{
	int i;
	gsCS;
	
	glColor3fv(CS->Series[j+1]);
	glBegin(GL_LINE_STRIP);
		for(i=0;i<NUMP;i++)  {
			glVertex2f((GLfloat)Xs[i],(GLfloat)Ys[j][i] /  BetaScales[j]);
		}
	glEnd();
	
	gfmDrawXYAxes();
}

void DrawColdChain(void)
{
	
	DrawParticleHists(StatXCold->Hist, gNumSweepsAfterBurnIn, StatXCold->v);
	DrawMixDens();
}

void DrawHotChain(void)
{
	
	DrawParticleHists(StatXHot->Hist, gNumSweepsAfterBurnIn, StatXHot->v);
	DrawDensityJ(HotBetaIdx);
}

void DrawChain0(void)
{
	int i = 0;
	
	if(gSwitched==1 && (gSwI==i || gSwJ==i) ) {
		gfmSetToPreDefdColorScheme(3);
	}
	else {
		gfmSetToPreDefdColorScheme(2);
	}
	DrawParticleHists(CoupX[i]->Hist, gNumSweepsAfterBurnIn, CoupX[i]->v);
	DrawDensityJ(i);
}

void DrawChain1(void)
{
	int i = 1;
		
	if(gSwitched==1 && (gSwI==i || gSwJ==i) ) {
		gfmSetToPreDefdColorScheme(3);
	}
	else {
		gfmSetToPreDefdColorScheme(2);
	}
	DrawParticleHists(CoupX[i]->Hist, gNumSweepsAfterBurnIn, CoupX[i]->v);
	DrawDensityJ(i);
}

void DrawChain2(void)
{
	int i = 2;
		
	if(gSwitched==1 && (gSwI==i || gSwJ==i) ) {
		gfmSetToPreDefdColorScheme(3);
	}
	else {
		gfmSetToPreDefdColorScheme(2);
	}
	DrawParticleHists(CoupX[i]->Hist, gNumSweepsAfterBurnIn, CoupX[i]->v);
	DrawDensityJ(i);
}


void DrawChain3(void)
{
	int i = 3;
	
	if(gSwitched==1 && (gSwI==i || gSwJ==i) ) {
		gfmSetToPreDefdColorScheme(3);
	}
	else {
		gfmSetToPreDefdColorScheme(2);
	}
		
	DrawParticleHists(CoupX[i]->Hist, gNumSweepsAfterBurnIn, CoupX[i]->v);
	DrawDensityJ(i);
}
void DrawChain4(void)
{
	int i = 4;
	
	if(gSwitched==1 && (gSwI==i || gSwJ==i) ) {
		gfmSetToPreDefdColorScheme(3);
	}
	else {
		gfmSetToPreDefdColorScheme(2);
	}
		
	DrawParticleHists(CoupX[i]->Hist, gNumSweepsAfterBurnIn, CoupX[i]->v);
	DrawDensityJ(i);
}

/* draw a histogram out of rectangles.  The height of the rectangles will be divided by 
	Total.  */
void DrawParticleHists(hist_struct *H, double Total, double Current)
{
	int i;
	gsCS
	gsClips
	GLfloat Top,Left,Right,d=.5;
	
	
	// set the color to the second series color
	glColor3fv(CS->Series[2]);
	
	// cycle over all the bins
	for(i=0;i<H->NumBins;i++)  {
		// deterine the proportions of the rectangles
		Top = (GLfloat)( (double)H->H[i] / Total) ;
		Left = (GLfloat)i-d;
		Right = i+d;
		glRectf(Left,0.0f,Right,Top);
	}
	
	// now draw the little rectangle beneath the current position
	glColor3fv(CS->Series[1]);
	glBegin(GL_LINES);
		glVertex2f((GLfloat)Current,Clips->ylo);
		glVertex2f((GLfloat)Current,Clips->yhi);
	glEnd();
	
	
	
	
}

void DrawMixDensBetas(void)
{
	int j,i;
	gsCS;
	
	for(j=0;j<NumBetas;j++)  {
		glColor3fv(CS->Series[j]);
		glBegin(GL_LINE_STRIP);
			for(i=0;i<NUMP;i++)  {
				glVertex2f((GLfloat)Xs[i],(GLfloat)Ys[j][i] / BetaScales[j]);
			}
		glEnd();
	}
	
	gfmDrawXYAxes();
}

int main(int argcp, char **argv)
{	
	int i,j;
	double x,div;
	
	K = 3; /* number of components */ 
	
	gPi[0] = .4;
	gPi[1] = .2;
	gPi[2] = .4;
	
	mu[0] = 16;
	mu[1] = 35;
	mu[2] = 53;
	
	var[0] = 5.5;
	var[1] = 4.7;
	var[2] = 4.7;
	
	gLo = 0.0;
	gHi = 70.0;
	
	NumBetas = 5;
	Betas[0] = 1.0;
	Betas[1] = .5;
	Betas[2] = .15;
	Betas[3] = .03;
	Betas[4] = 0.0;
	
	/* now make a thing to show the density */
	div = (gHi-gLo) / (double)NUMP;
	gMaxD = 0.0;
	for(x=0.0,i=0;i<NUMP;i++,x+=div)  {
		MixXs[i] = x;
		MixYs[i] = mixdens(x,gPi,mu,var,K);
		if(MixYs[i]  > gMaxD) gMaxD = MixYs[i];
	}
	
	Ys = (double **)ECA_CALLOC(NumBetas,sizeof(double *));
	/* now make some things to show all the densities that have been flattened and squashed */
	gMaxDBetas = 0.0;
	BetaScales[0] = 1.0;
	BetaScales[1] = 6.752;
	BetaScales[2] = 30.79;
	BetaScales[3] = 55.58;
	BetaScales[4] = 65;
	for(j=0;j<NumBetas;j++)  {
		Ys[j] = (double *)ECA_CALLOC(NUMP,sizeof(double));
		for(x=0.0,i=0;i<NUMP;i++,x+=div)  {
			Xs[i] = x;
			Ys[j][i] = pow(mixdens(x,gPi,mu,var,K),Betas[j]);
			if(Ys[j][i] / BetaScales[j] > gMaxDBetas) gMaxDBetas = Ys[j][i] / BetaScales[j];
		}
	}
	
	/* here we make some variables for doing the MCMC */
	StatXCold = AllocDval(0.0,65.0,1.0);
	StatXHot = AllocDval(0.0,65.0,1.0);
	CoupX = DvalVector(0,NumBetas,0.0,65.0,1.0);
	HotBeta = Betas[2];
	HotBetaIdx = 2;
	
	
	// here must call glutInit from within main, with the command line option
	// pointer, and then call gfmInitGFM, and that does it.  
	glutInit(&argcp, argv);
	gfmInitGFM();

	return(1);
	
}

