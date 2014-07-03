#define UN_EXTERN



#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "ranlib.h"

#include "MathStatRand.h"
#include "MCTypesEtc.h"

#include "GFMCMC.h"
#include "GFMCMC_DrawingUtilities.h"
#include "ECA_MemAlloc.h"


/* prototypes */
void TPM_Console(void);
void DrawBouncingBlob(void);
void DrawWalkTrace(void);
void DrawParticleHists(int_hist_struct *H, double Total, int Current);
void DrawHistScene(void);

/* some global variables */
int gParticle;  /* the state of the random walk (in the set 0,...,4) */
ival *gReported;  /* this is basically gParticle, but plus one (so it goes from 1 to 5). */
double **tpm;
sliding_trace_struct *gWalkTrace;



void gfmUserDefd_ResetAllAverages(void)
{
	
		
	gNumSweepsAfterBurnIn = 0;
	
	InitIvalToZero(gReported);
	
	gReported->v = gParticle + 1;
	
	gfduInitSlidingTraceToZero(gWalkTrace);
	
}

void gfmUserDefd_InitializeChain(void)
{
	/* start at a random point */
	gParticle = UniformRV(0,4);
	
	/* reset the averages */
	InitIvalToZero(gReported);
	
	/* give the reported variable the current position plus 1 */
	gReported->v = gParticle + 1;
	

	gNumSweeps = 0;
	
} 
 
void gfmUserDefd_OneStepByGlobals(void)
{
		int temp;
		
		temp = IntFromProbsRV(tpm[gParticle],0,5);
		
		gParticle = temp;
		
		gReported->v = gParticle + 1;
		
		IncrementIval(gReported);
		gfduIncrementSlidingTrace(gWalkTrace,gReported->v);
}

void gfmUserDefd_DefineWindows(void)
{
	
	
	/* name the views files */ 
	gsOUTPUT_FILE_PREFIX("tpm");
	
	gsCONSOLE("\"Info\" Window");
		gsDRAW_FUNC(TPM_Console);
		
	gsNEW_WINDOW(1,	"Bouncing Blob");
		gsDRAW_FUNC(DrawBouncingBlob);
		gsXLO(-1);
		gsXHI(6.8);
		gsYHI(1);
		gsYLO(-.3);
		gsNO_Y_AXIS
		gsAXES_POS_QUADRANT;
		gsCOLOR_SCHEME(DEEP_BLUE);
	gsNEW_WINDOW(2, "Histograms");
		gsDRAW_FUNC(DrawHistScene);
		gsCOLOR_SCHEME(DEEP_BLUE);
		gsXLO(-1);
		gsXHI(6.8);
		gsYLO(-.12);
		gsYHI(.5);
		gsAXES_POS_QUADRANT;
	gsNEW_WINDOW(8,"Walk Trace");
		gsYLO(-1.6)
		gsYHI(6.4)
		gsDRAW_FUNC(DrawWalkTrace);
		gsCOLOR_SCHEME(FISHER_PRICE);
				
}



void gfmUserDefd_DefineMenus(void)    /* to define menus for the right click of the mouse */
{
	return;  // no special right click menus for now
}


void gfmUserDefd_LastWords(void)
{
	return;  // no special last words here
}


void TPM_Console(void)
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

void DrawBouncingBlob(void)
{
	char temp[100];
	gsCS
	GLfloat ylo = .25, yhi = .75;
	GLfloat d = .4;
	GLfloat x = (GLfloat)gReported->v;
		
	glColor3fv(CS->Series[1]);
	glRectf(x-d,ylo,x+d,yhi);
	
	gfmDrawXYAxes();
	
	/* now, draw its position in scaled text */
	glColor3fv(CS->Series[4]);
	sprintf(temp,"%d",gReported->v);
	gfmStrokeString(temp, .17, (GLfloat)gReported->v, 0.04, 1, 
				0.0, gfmWoH());
}

void DrawWalkTrace(void)
{		
	glColor3fv(gWindowsSettings[glutGetWindow()]->ColorScheme->Series[0]);
	
	gfduDrawSlidingTrace(gWalkTrace);
	
	gfmDrawXYAxes();

}



/* draw a histogram out of rectangles.  The height of the rectangles will be divided by 
	Total.  */
void DrawParticleHists(int_hist_struct *H, double Total, int Current)
{
	int i;
	gsCS
	GLfloat Top,Left,Right, Bottom,d=.5;
	
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
	Top = -.03f;
	Bottom = -.12f;
	Left = (GLfloat)Current-d;
	Right = (GLfloat)Current+d;
	glRectf(Left,Bottom,Right,Top);
	
	
	gfmDrawXYAxes();
	
}

void DrawHistScene(void)
{
	DrawParticleHists(gReported->Hist,gNumSweepsAfterBurnIn,gReported->v);
}










int main(int argcp, char **argv)
{	
	
	double row1[] = {.2,.2,.2,.2,.2};
	double row2[] = {.2,.3,.5,0,0};
	double row3[] = {0,.3,.4,.3,0};
	double row4[] = {0,0,.5,.3,.2};
	double row5[] = {.2,.2,.2,.2,.2};
	
	
	/* allocate memory to the particle reporter */
	gReported = AllocIval(0,5,1);
	gWalkTrace = gfduAllocSlidingTrace(1000,1);
	
	
	/* establish the tpm */
	tpm = (double **)ECA_CALLOC(5,sizeof(double*));
	tpm[0] = row1;
	tpm[1] = row2;
	tpm[2] = row3;
	tpm[3] = row4;
	tpm[4] = row5;
	
	// here must call glutInit from within main, with the command line option
	// pointer, and then call gfmInitGFM, and that does it.  
	glutInit(&argcp, argv);
	gfmInitGFM();

	return(1);
	
}

