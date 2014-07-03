#define UN_EXTERN



#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "ranlib.h"

#include "MathStatRand.h"
#include "MCTypesEtc.h"

#include "GFMCMC.h"
#include "ECA_MemAlloc.h"


#define NUM_BX 500  /* number of BetaXs  */


// some global variables
ival *gInd, *gMH;
int gN;
double gp;

/* stuff for the beta distribution thing */
double *gBetaPrior, *gBetaPost, *gBetaXs;  
double gAlpha, gBeta;  /* parameters of the uniform dsn */
int gbN, gbX;  /* the sample size and the number of successes */
double gbp;  /* the proportion of successes */
dval *gbMC, *gbMCMC;
double gbMH_window = .023;  /* the width of the uniform proposal dsn for the MH sampling
						on the beta posterior */



// prototypes
void gfbsDrawProbsSkeleton(void);
void gfbsDrawIndBinom(void);
void gfbsDrawConsoleWindow(void);
void gfbsDrawRectHist(int_hist_struct *H, double Total, int Current);
void gfbsMH_Update(ival *L);
void gfbsDrawMHBinom(void);
void gfbsDrawPriorAndPost(void);
void gfbs_gbN_ParmClick(int button, int state, int x, int y);
void gfbsControl_gbN(void);
void gfbsSetupggNControl(void);
void gfbsDrawPostMC(void);
void gfbsDrawDoubleHistAsRec(hist_struct *H, double Total, double Current);
void gfbsDrawPostMCMC(void);
void gfbsMH_Update_Beta(dval *D);
void gfbsTestRainbow(void);



void gfmUserDefd_ResetAllAverages(void)
{
	double temp;
	InitIvalToZero(gInd);
	InitIvalSummaryToZero(gMH);
	
	InitDvalToZero(gbMC);
	
	temp = gbMCMC->v;
	InitDvalToZero(gbMCMC);
	gbMCMC->v = temp;
		
	gNumSweepsAfterBurnIn = 0;
	
}

void gfmUserDefd_InitializeChain(void)
{
	InitIvalToZero(gInd);
	InitIvalToZero(gMH);
	
	InitDvalToZero(gbMC);
	InitDvalToZero(gbMCMC);
	
	gMH->v = 0;
	gbMCMC->v = 0;

	gNumSweeps = 0;
	
} 
 
void gfmUserDefd_OneStepByGlobals(void)
{
		gInd->v = ignbin(gN,(float)gp);
		IncrementIval(gInd);
		
		gfbsMH_Update(gMH);
		IncrementIval(gMH);	
		
		gbMC->v = (double)genbet(gAlpha+gbX, gBeta+(gbN-gbX));
		IncrementDval(gbMC);
		
		gfbsMH_Update_Beta(gbMCMC);
		IncrementDval(gbMCMC);
}

void gfmUserDefd_DefineWindows(void)
{
	int tempX[2];
	double tempP[2];
	GLfloat MaxProb;
	
	// determine the extent of this binomial distributions
	tempX[0] = (int)(gN * gp);
	tempX[1] = gN - tempX[0];
	tempP[0] = gp;
	tempP[1] = 1.0 - gp;
	MaxProb = (GLfloat)exp(LogMultinomialPMF(gN, tempP, tempX, 2));
	
	
	// name the views files 
	gsOUTPUT_FILE_PREFIX("Pedagogy");
	
	gsCONSOLE("\"Info\" Window");
		gsDRAW_FUNC(gfbsDrawConsoleWindow);
		
	gsNEW_WINDOW(1,	"Independent Realizations");
		gsDRAW_FUNC(gfbsDrawIndBinom);
		gsXLO(-.20 * gN);
		gsXHI(1.1 * gN);
		gsYHI(1.5 * MaxProb);
		gsYLO(-.25 * MaxProb);
		gsAXES_POS_QUADRANT;
		gsCOLOR_SCHEME(DEEP_BLUE);
		
	gsNEW_WINDOW(2,	"Markov Chain Realizations");
		gsDRAW_FUNC(gfbsDrawMHBinom);
		gsXLO(-.20 * gN);
		gsXHI(1.1 * gN);
		gsYHI(1.5 * MaxProb);
		gsYLO(-.25 * MaxProb);
		gsAXES_POS_QUADRANT;
		gsCOLOR_SCHEME(FISHER_PRICE);
		
	gsNEW_WINDOW(3,	"Beta Prior and Post");
		gsDRAW_FUNC(gfbsDrawPriorAndPost);
		gsXLO(-.20);
		gsXHI(1.1);
		gsYHI(10.0);
		gsYLO(-1.3);
		gsAXES_POS_UNIT_X;
		gsCOLOR_SCHEME(FISHER_PRICE);
	
	gsNEW_WINDOW(4,"SampleSize")
		gsDRAW_FUNC(gfbsControl_gbN)
		gsXHI(101.0f)
		gsXLO(-10)
		gsAXES_POS_QUADRANT
		gsYTICKS_MAJOR_SIZE(0.0f)
		gsNO_YAXIS_NUMS
		gsY_AXIS_HI(1.0f)
		gsEXTRA_FUNC(gfbsSetupggNControl)
		
	gsNEW_WINDOW(5,	"Posterior: Independent Monte Carlo");
		gsDRAW_FUNC(gfbsDrawPostMC);
		gsXLO(-.20);
		gsXHI(1.1);
		gsYHI(14.0);
		gsYLO(-2.0);
		gsAXES_POS_UNIT_X;
		gsCOLOR_SCHEME(FISHER_PRICE);
		
	gsNEW_WINDOW(6,	"Posterior: Markov Chain Monte Carlo");
		gsDRAW_FUNC(gfbsDrawPostMCMC);
		gsXLO(-.20);
		gsXHI(1.1);
		gsYHI(14.0);
		gsYLO(-2.0);
		gsAXES_POS_UNIT_X;
		gsCOLOR_SCHEME(FISHER_PRICE);
		
	gsNEW_WINDOW(7,	"Testing color rainbow");
		gsDRAW_FUNC(gfbsTestRainbow);
		gsXLO(-.20);
		gsXHI(1.1);
		gsYHI(1.1);
		gsYLO(-.1);
		gsAXES_POS_UNIT_X;
		gsCOLOR_SCHEME(FISHER_PRICE);
		
}



void gfmUserDefd_DefineMenus(void)    // to define menus for the right click of the mouse
{
	return;  // no special right click menus for now
}


void gfmUserDefd_LastWords(void)
{
	return;  // no special last words here
}


void gfbsTestRainbow(void)
{
	GLfloat i;
	GLfloat Col[3];
	
	for(i=0.0;i<=1.0;i+=.0001)  {
		gfmMap0_1ToColor(Col, (double)i);
		glColor3fv(Col);
		glBegin(GL_LINES);
			glVertex2f(i,.0f);
			glVertex2f(i,.3f);
		glEnd();
		
		
		
		gfmMap0_1ToColor(Col, (double)(1.0-i));
		glColor3fv(Col);
		glBegin(GL_LINES);
			glVertex2f(i,.6f);
			glVertex2f(i,.9f);
		glEnd();
	}
}

void gfbsDrawPriorAndPost(void)
{
	int k;
	gsCS gsWoH gsClips
	char temp[100];
	GLfloat y,v = Clips->yhi - Clips->ylo;
	
	glColor3fv(CS->Series[3]);
	glBegin(GL_LINE_STRIP);
		for(k=0;k<NUM_BX;k++)  {
			if(gBetaXs[k] < 1.0 && gBetaXs[k] > 0.0) {
				glVertex2f((GLfloat)gBetaXs[k], (GLfloat)gBetaPrior[k]);
			}
		}
	glEnd();
	
	
	if(gbN>0) {  /* only draw the posterior for positive values of gbN---this way you
					can have it not be drawn */			
		
		glColor3fv(CS->Series[4]);
		glBegin(GL_LINE_STRIP);
			for(k=0;k<NUM_BX;k++)  {
				if(gBetaXs[k] < 1.0 && gBetaXs[k] > 0.0) {
					glVertex2f((GLfloat)gBetaXs[k], (GLfloat)gBetaPost[k]);
				}
			}
		glEnd();
	}
	
	gfmDrawXYAxes();
	
	if(gbN>0) {
		/*  draw text on the screen */
		glColor3fv(CS->Text);
		sprintf(temp,"N = %d", gbN);
		y = Clips->yhi - .2f * v;
		gfmStrokeString(temp, .1f*v, 1.0f, y, 4, 0.0f, WoH);
		
		sprintf(temp,"X = %d", gbX);
		y = Clips->yhi - .4f * v;
		gfmStrokeString(temp, .1f*v, 1.0f, y, 4, 0.0f, WoH);
	}
}


void gfbsDrawPostMC(void)
{
	int k;
	gsCS gsWoH gsClips
	char temp[100];
	GLfloat y,v = Clips->yhi - Clips->ylo;
	
	
	/* draw the histogram approximation one the bottom, but only if 
	the number averaged so far is greater than zero */
	if(gbMC->NumAved > 0)
		gfbsDrawDoubleHistAsRec(gbMC->Hist, gbMC->Hist->dTot, gbMC->v);
	
	/* then draw the known posterior density */
	glColor3fv(CS->Series[4]);
	glBegin(GL_LINE_STRIP);
		for(k=0;k<NUM_BX;k++)  {
			if(gBetaXs[k] < 1.0 && gBetaXs[k] > 0.0) {
				glVertex2f((GLfloat)gBetaXs[k], (GLfloat)gBetaPost[k]);
			}
		}
	glEnd();
	
	gfmDrawXYAxes();
		
	/*  draw text on the screen */
	glColor3fv(CS->Text);
	sprintf(temp,"N = %d", gbN);
	y = Clips->yhi - .2f * v;
	gfmStrokeString(temp, .1f*v, 1.0f, y, 4, 0.0f, WoH);
	
	sprintf(temp,"X = %d", gbX);
	y = Clips->yhi - .4f * v;
	gfmStrokeString(temp, .1f*v, 1.0f, y, 4, 0.0f, WoH);
}


void gfbsDrawPostMCMC(void)
{
	int k;
	gsCS gsWoH gsClips
	char temp[100];
	GLfloat y,v = Clips->yhi - Clips->ylo;
	
	
	/* draw the histogram approximation one the bottom, but only if 
	the number averaged so far is greater than zero */
	if(gbMC->NumAved > 0)
		gfbsDrawDoubleHistAsRec(gbMCMC->Hist, gbMCMC->Hist->dTot, gbMCMC->v);
	
	/* then draw the known posterior density */
	glColor3fv(CS->Series[4]);
	glBegin(GL_LINE_STRIP);
		for(k=0;k<NUM_BX;k++)  {
			if(gBetaXs[k] < 1.0 && gBetaXs[k] > 0.0) {
				glVertex2f((GLfloat)gBetaXs[k], (GLfloat)gBetaPost[k]);
			}
		}
	glEnd();
	
	gfmDrawXYAxes();
		
	/*  draw text on the screen */
	glColor3fv(CS->Text);
	sprintf(temp,"N = %d", gbN);
	y = Clips->yhi - .2f * v;
	gfmStrokeString(temp, .1f*v, 1.0f, y, 4, 0.0f, WoH);
	
	sprintf(temp,"X = %d", gbX);
	y = Clips->yhi - .4f * v;
	gfmStrokeString(temp, .1f*v, 1.0f, y, 4, 0.0f, WoH);
}



/* for the window in which you can control the sample size */
void gfbsControl_gbN(void)
{
	int CW = glutGetWindow();
	ColorScheme3f *CS = gWindowsSettings[CW]->ColorScheme;
	
	gfmDrawXYAxes();
	
	/*  then draw a line of unit length where rho is in the first series color */
	glColor3fv(CS->Series[0]);
	glBegin(GL_LINES);
		glVertex2f((GLfloat)gbN, 0.0f);
		glVertex2f((GLfloat)gbN, 1.0f);
	glEnd();
}



void gfbs_gbN_ParmClick(int button, int state, int x, int y)
{
	GLfloat X,Y;
	int k;
	
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
		X = gfmReturnXCoord(x);
		Y = gfmReturnYCoord(y);
		
		
		if(Y >= 0.0f && Y <= 1.0f && X > 0.0f)  { /*  if the click is in the right place,  */
												/*  change the value of gbN */
			
			gbN = (int)X;
			gbX = (int)( (double)X * gbp );
			
			/* then recompute all the posteriors and things */
			for(k=0;k<NUM_BX;k++)  {
				if(gBetaXs[k]>0.0 && gBetaXs[k]<1.0) {
					gBetaPrior[k] = exp(LogBetaPDF(gAlpha,gBeta,gBetaXs[k]));
					gBetaPost[k] = exp(LogBetaPDF(gAlpha + gbX,gBeta + (gbN-gbX),gBetaXs[k]));
				}
			}
		
		}
	}
}


void gfbsSetupggNControl(void)
{
	glutMouseFunc(gfbs_gbN_ParmClick);
}






// given gN and gp, this proposes an update via a simple
// random walk proposal to a new state and it accepts it 
// according to the MH ratio.  
void gfbsMH_Update(ival *L)
{
	double qpart, MH, topProb, bottomProb;
	int Lprime;
	
	int tempX[2];
	double tempP[2];
	
	
	// make the proposal
	qpart = 1.0;
	if(ranf()<.5)
		Lprime = L->v+1;
	else
		Lprime = L->v-1;
		
	// deal with the special cases on the end
	if(L->v == 0) {
		Lprime = 1;
		qpart = .5;
	}
	if(L->v == gN) {
		Lprime = gN - 1;
		qpart = .5;
	}
	
	if(Lprime == 0 || Lprime == gN)
		qpart = 2;
	
	// compute the bottom and top limit dsn probs:
	tempX[0] = L->v;
	tempX[1] = gN - tempX[0];
	tempP[0] = gp;
	tempP[1] = 1.0 - gp;
	bottomProb = LogMultinomialPMF(gN, tempP, tempX, 2);
	
	tempX[0] = Lprime;
	tempX[1] = gN - tempX[0];
	tempP[0] = gp;
	tempP[1] = 1.0 - gp;
	topProb = LogMultinomialPMF(gN, tempP, tempX, 2);
	
	// compute the MH ratio
	MH = log(qpart) +  topProb - bottomProb;
	
	// then accept if appropriate
	if(log(ranf())<MH)
		L->v = Lprime;
	
}


void gfbsMH_Update_Beta(dval *D)
{
	/* proposed value is drawn uniformly in a window */
	double pD = D->v + gbMH_window * ranf() - (.5 * gbMH_window);
	double ratio;
	double alpha = gAlpha + gbX, beta = gBeta + (gbN-gbX);
	
	if(pD <= 0.0 || pD >= 1.0)  /* reject out of hand if it is beyond the bounds */
		return;
		
	ratio = LogBetaPDF(alpha, beta, pD) - LogBetaPDF(alpha, beta, D->v);
	
	if(log(ranf()) < ratio)
		D->v = pD;
}

void gfbsDrawProbsSkeleton(void)
{
	int i;
	ColorScheme3f *CS = gWindowsSettings[glutGetWindow()]->ColorScheme;
	GLfloat Top,Left,Right;
	int tempX[2];
	double tempP[2];
	
	
	// set the color to the first series color
	glColor3fv(CS->Series[0]);
	
	// cycle over all the probabilities
	for(i=0;i<=gN;i++)  {
		// compute the binomial probability
		tempX[0] = i;
		tempX[1] = gN - tempX[0];
		tempP[0] = gp;
		tempP[1] = 1.0 - gp;
		Top = (GLfloat)exp(LogMultinomialPMF(gN, tempP, tempX, 2));
		Left = (GLfloat)i;
		Right = Left + 1.0f;
		glBegin(GL_LINE_LOOP);
			glVertex2f(Left,0.0f);
			glVertex2f(Left,Top);
			glVertex2f(Right,Top);
			glVertex2f(Right,0.0f);
		glEnd();
	}
	
	
}

void gfbsDrawIndBinom(void)
{	
	
	gfbsDrawRectHist(gInd->Hist, gInd->Hist->dTot, gInd->v);
	
	gfbsDrawProbsSkeleton();
	
	// then draw the axes
	gfmDrawXYAxes();
}

void gfbsDrawMHBinom(void)
{	
	
	gfbsDrawRectHist(gMH->Hist, gMH->Hist->dTot, gMH->v);
	
	gfbsDrawProbsSkeleton();
	
	// then draw the axes
	gfmDrawXYAxes();
}


// draw a histogram out of rectangles.  The height of the rectangles will be divided by 
// Total.
void gfbsDrawRectHist(int_hist_struct *H, double Total, int Current)
{
	int i;
	ColorScheme3f *CS = gWindowsSettings[glutGetWindow()]->ColorScheme;
	GLfloat Top,Left,Right, Bottom;
	
	int tempX[2];
	double tempP[2];
	GLfloat MaxProb;
	
	// determine the extent of this binomial distributions
	tempX[0] = (int)(gN * gp);
	tempX[1] = gN - tempX[0];
	tempP[0] = gp;
	tempP[1] = 1.0 - gp;
	MaxProb = (GLfloat)exp(LogMultinomialPMF(gN, tempP, tempX, 2));
	
	// set the color to the second series color
	glColor3fv(CS->Series[1]);
	
	// cycle over all the bins
	for(i=0;i<H->NumBins;i++)  {
		// deterine the proportions of the rectangles
		Top = (GLfloat)( (double)H->H[i] / Total) ;
		Left = (GLfloat)i;
		Right = Left + 1.0f;
		glRectf(Left,0.0f,Right,Top);
	}
	
	// now draw the little rectangle beneath the current position
	Top = -.03f * MaxProb;
	Bottom = -.12f * MaxProb;
	Left = (GLfloat)Current;
	Right = Left + 1;
	glRectf(Left,Bottom,Right,Top);
	
}


// draw a histogram out of rectangles.  The height of the rectangles will be divided by 
// Total and divided by the step size d (to make them like densities)
void gfbsDrawDoubleHistAsRec(hist_struct *H, double Total, double Current)
{
	int i;
	ColorScheme3f *CS = gWindowsSettings[glutGetWindow()]->ColorScheme;
	GLfloat Top,Left,Right, Bottom;
	gsClips
	GLfloat v = Clips->yhi - Clips->ylo;
	

	
	// set the color to the second series color
	glColor3fv(CS->Series[1]);
	
	// cycle over all the bins
	for(i=1;i<H->NumBins;i++)  {
		// deterine the proportions of the rectangles (make them the height of the corresponding
		// density.
		Top = (GLfloat)( (double)H->H[i] / Total) / H->d  ;
		Left = (GLfloat)( (double)(i-1) * H->d);
		Right = Left + (GLfloat)H->d;
		glRectf(Left,0.0f,Right,Top);
	}
	
	// now draw the little rectangle beneath the current position
	Top = -.03f * v;
	Bottom = -.12f * v;
	Left = (GLfloat)(Current - .5f * H->d);
	Right = (GLfloat)(Left + H->d);
	glRectf(Left,Bottom,Right,Top);
	
}


void gfbsDrawConsoleWindow(void)
{
	char temp[250];
	int CW = glutGetWindow();	
	ColorScheme3f *CS = gWindowsSettings[CW]->ColorScheme;

	
	
	glColor3fv(CS->Text);
	gfmRenderBitmapString(-.1f,1.0f,GLUT_BITMAP_HELVETICA_12, "Program: MCMC Pedagogy");
	gfmRenderBitmapString(-.1f,.9f,GLUT_BITMAP_HELVETICA_12, "Author: E.C. Anderson");
	
	sprintf(temp,"N: %d", gN);
	gfmRenderBitmapString(-.1f,.8f,GLUT_BITMAP_HELVETICA_12, temp);
	
	sprintf(temp,"p: %f",gp);
	gfmRenderBitmapString(-.1f,.7f,GLUT_BITMAP_HELVETICA_12, temp);
	
	sprintf(temp,"Num of Iterations: %d",gNumSweeps);
	gfmRenderBitmapString(-.1f,.3f,GLUT_BITMAP_HELVETICA_12, temp);
	
	if(gGo) 
		sprintf(temp,"RUNNING!");
	else 
		sprintf(temp,"STOPPED!");
		
	gfmRenderBitmapString(-.1f,.2f,GLUT_BITMAP_HELVETICA_12, temp);
	
}

int main(int argcp, char **argv)
{	
	double x, step = 1.0/NUM_BX;
	int k;
	
	gN = 75;
	gp = .25;
	
	// ask the user for n
/*	printf("\n\nWe will be simulating binomial random variables.");
	printf("\n\nInput N and p\n\n->");
	scanf("%d %lf",&gN,&gp);
*/

	
	
	gInd = AllocIval(0,gN, 1);
	gMH = AllocIval(0,gN, 1);
	
	gMH->v = ignbin(gN,(float)gp);  // just set it to this for now.
	
	
	/* take care of initializing stuff for the beta distribution part of things */
	gBetaPrior = (double *)ECA_CALLOC(NUM_BX,sizeof(double));
	gBetaPost =  (double *)ECA_CALLOC(NUM_BX,sizeof(double));
	gBetaXs =  (double *)ECA_CALLOC(NUM_BX,sizeof(double));
	
	gbMC = AllocDval(0.0,1.0,.005);
	gbMCMC = AllocDval(0.0,1.0,.005);
	
	gAlpha = 1.0;
	gBeta = 1.0;
	gbN = 200;
	gbX = 73;
	gbp= .3;
	
	for(x=.001,k=0;x<1.0 && k<NUM_BX;k++,x+=step)  {
		gBetaXs[k] = x;
		if(k==NUM_BX-1)
			gBetaXs[k] = .999;
		
		gBetaPrior[k] = exp(LogBetaPDF(gAlpha,gBeta,x));
		gBetaPost[k] = exp(LogBetaPDF(gAlpha + gbX,gBeta + (gbN-gbX),x));
	}
	
	
	
	// here must call glutInit from within main, with the command line option
	// pointer, and then call gfmInitGFM, and that does it.  
	glutInit(&argcp, argv);
	gfmInitGFM();

	return(1);
	
}

