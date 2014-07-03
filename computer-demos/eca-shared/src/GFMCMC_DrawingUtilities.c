#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "GFMCMC.h"
#include "MCTypesEtc.h"
#include "ECA_MemAlloc.h"
#include "GFMCMC_DrawingUtilities.h"




/* draw a histogram out of rectangles.  The height of the rectangles will be divided by 
 Total and divided by the step size d (to make them like densities)
 
 The Desired color should be set outside of this function.
 
*/

void gfduDrawDoubleHistAsRec(hist_struct *H, double Total, double Current)
{
	int i;
	GLfloat Top,Left,Right, Bottom;
	gsClips
	GLfloat v = Clips->yhi - Clips->ylo;

	
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




/*
	draws a line, plus a bar at either end,
	factor and pattern refer to the stippling
	factor = 1 and stipple = 65535 is No stippling.
	AsDensity = 1 means that the heights will be divided by Hist->d so that
	the thing is scaled the way a density would be
	
	you should set the color before calling this!
*/
void gfmduDrawOneHistAsCurveWith2Bars(dval *D, GLint factor, GLushort pattern, int AsDensity)
{
	int i;
	hist_struct *Hist = D->Hist;
	GLfloat d = (GLfloat)D->Hist->d;
	GLfloat x,y,h;
	
	
	
	
	/*  if nothing has been averaged so far, get out of here---don't do anything: */
	if(D->NumAved <= 0) {
		return;
	}
	
	
	/*  draw rectangles for the bins at the ends (<= Lo and >Hi) */
	/*  draw the one at the left end first */
	if(AsDensity == 1)
		h = (GLfloat)(Hist->H[0]/(Hist->dTot * d));
	else 
		h = (GLfloat)(Hist->H[0]/Hist->dTot);
	glRectf((GLfloat)Hist->Lo, 0.0f, (GLfloat)(Hist->Lo - Hist->d), h );
	
	/*  then the one at the right end */
	if(AsDensity == 1)
		h = (GLfloat)(Hist->H[0]/(Hist->dTot * d));
	else 
		h = (GLfloat)(Hist->H[0]/Hist->dTot);
	glRectf((GLfloat)Hist->Hi, 0.0f, (GLfloat)(Hist->Hi + Hist->d), h );
	
	
	
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(factor,pattern);
	/*  then draw the line */
	glBegin(GL_LINE_STRIP);
		
		for(i=1;i<D->Hist->NumBins-1;i++)  {
			x = d * (GLfloat)( (i-1) + i) / 2.0f;
			y = (GLfloat)(Hist->H[i] / Hist->dTot);
			if(AsDensity==1)
				y /= d;
				
			glVertex2f(x,y);
		}
	glEnd();
	glDisable(GL_LINE_STIPPLE);
	
	/*  then draw the vertical line showing the current value */
	x = (GLfloat)D->v;
	if(D->v >= Hist->Hi)  {
		y = (GLfloat) (Hist->H[Hist->NumBins-1] / Hist->dTot);
	}
	else if( (Hist->HasLeftovers==1) && D->v >= Hist->Left)  {
		y = (GLfloat)(Hist->H[Hist->NumBins-2] / Hist->dTot);
	}
	else if(D->v <= Hist->Lo)  {
		y = (GLfloat)(Hist->H[0] / Hist->dTot);
	}
	else {
		y = (GLfloat)(Hist->H[ (int)((D->v - Hist->Lo) / d) + 1] / Hist->dTot);
	}
	
	if(AsDensity==1)
		y /= d;
		
	glBegin(GL_LINES);
		glVertex2f(x,0);
		glVertex2f(x,y);
	glEnd();

	/*  and that should be it */
}




/*  for allocating memory to a sliding_trace_struct */
sliding_trace_struct *gfduAllocSlidingTrace(int Length, int Interval)
{
	sliding_trace_struct *temp;
	
	temp = (sliding_trace_struct *)ECA_MALLOC(sizeof(sliding_trace_struct));
	
	temp->N = Length;
	temp->Lo = 0;
	temp->Hi = 0;
	temp->Span = 0.0f;
	temp->Start = 0;
	temp->Interval = Interval;
	
	temp->OverallMax = -999999999999999.9;  /*  sort of klugie, but ought to work for now */
	temp->OverallMin = 9999999999999999.9;
	
	temp->Trace = (double *)ECA_CALLOC((size_t)temp->N, sizeof(double));
	
	gfduInitSlidingTraceToZero(temp);
	
	return(temp);
}

void gfduInitSlidingTraceToZero(sliding_trace_struct *temp)
{
	int i;
	
	temp->Lo = 0;
	temp->Hi = 0;
	temp->Span = 0.0f;
	temp->Start = 0;
	
	temp->OverallMax = -999999999999999.9;  /*  sort of klugie, but ought to work for now */
	temp->OverallMin = 9999999999999999.9;
	
	for(i=0;i<temp->N;i++)  {
		temp->Trace[i] = 0;
	}

}


/*  for incrementing a sliding_trace_struct */
/*  most of the time Iter should be gNumSweepsSinceBurnIn, so, in fact, */
/*  we will set it to be so. */
void gfduIncrementSlidingTrace(sliding_trace_struct *S, double value)
{
	int iter = gNumSweepsAfterBurnIn;
	
	/*  get out immediately if iter is not on the collection interval */
	if(iter % S->Interval != 0)
		return;
		
	/*  otherwise we have to figure out where to put the new value and we have */
	/*  to update Lo and Hi and Start */
	
	/*  the high value always gets pumped up by Interval */
	S->Hi += S->Interval;
	
	/*  start and lo will only change if we have filled everything already which occurs */
	/*  when iter / interval is greater than or equal to N */
	if( (iter / S->Interval) >= S->N ) {
	
		/*  the new value gets put in where the current Starting point is */
		S->Trace[S->Start] = value;
		
		/*  we want to start one ahead of the last starting place so */
		S->Start = (S->Start + 1)  %  S->N;
		
		/*  and the lo value gets pumped up by Interval */
		S->Lo += S->Interval;
		
		/*  and span is the distance from lo to hi */
		S->Span = (GLfloat)(S->Hi - S->Lo);
		
	}
	else {  /*  otherwise, the new value goes into the now high spot: */
		S->Trace[iter / S->Interval] = value;
		
		/*  and span remains the distance from Lo to the N + Interval */
		S->Span = (GLfloat)(S->N * S->Interval) - (GLfloat)S->Lo;
	}
	
	
	/*  and deal with the overall mins and maxes */
	if(S->OverallMax < value)
		S->OverallMax = value;
	if(S->OverallMin > value)
		S->OverallMin = value;
	
}


/*  for incrementing a sliding_trace_struct */
/*  most of the time Iter should be gNumSweepsSinceBurnIn, so, in fact, */
/*  we will set it to be so.
This is a quick version I hacked up into which you can pass the current iteration
number instead of using the global value. */
void gfduIncrementSlidingTraceNoGlobalSweepCounter(sliding_trace_struct *S, double value, int iter)
{
	
	/*  get out immediately if iter is not on the collection interval */
	if(iter % S->Interval != 0)
		return;
		
	/*  otherwise we have to figure out where to put the new value and we have */
	/*  to update Lo and Hi and Start */
	
	/*  the high value always gets pumped up by Interval */
	S->Hi += S->Interval;
	
	/*  start and lo will only change if we have filled everything already which occurs */
	/*  when iter / interval is greater than or equal to N */
	if( (iter / S->Interval) >= S->N ) {
	
		/*  the new value gets put in where the current Starting point is */
		S->Trace[S->Start] = value;
		
		/*  we want to start one ahead of the last starting place so */
		S->Start = (S->Start + 1)  %  S->N;
		
		/*  and the lo value gets pumped up by Interval */
		S->Lo += S->Interval;
		
		/*  and span is the distance from lo to hi */
		S->Span = (GLfloat)(S->Hi - S->Lo);
		
	}
	else {  /*  otherwise, the new value goes into the now high spot: */
		S->Trace[iter / S->Interval] = value;
		
		/*  and span remains the distance from Lo to the N + Interval */
		S->Span = (GLfloat)(S->N * S->Interval) - (GLfloat)S->Lo;
	}
	
	
	/*  and deal with the overall mins and maxes */
	if(S->OverallMax < value)
		S->OverallMax = value;
	if(S->OverallMin > value)
		S->OverallMin = value;
	
}




/*  this function just plots out a curve given by a sliding trace struct */
/*  the color should be chosen by glColor before calling this function */
void gfduDrawSlidingTrace(sliding_trace_struct *S)
{
	int i,iter,idx;
	int CW = glutGetWindow();
	GLfloat ymax = -99999999999.99f, ymin = 99999999999999.99f;
	char Str[100];
	
	clipping_struct *C = gWindowsSettings[CW]->Clips;
	fitting_struct *Fits  = gWindowsSettings[CW]->Fits;
	
	
	/*  first off, fit the sides of the window to Lo and Hi (or N * Interval) */
	/*  use the padding in the fitting struct to do so */
	C->xlo = (GLfloat)(S->Lo -  (Fits->Padding->xlo * S->Span));
	C->xhi = (GLfloat)(S->Lo + S->Span +  (Fits->Padding->xhi * S->Span));
	

	
	glBegin(GL_LINE_STRIP);
		
		for(i=0,iter = S->Lo;  i<S->N && iter < S->Hi ; i++, iter += S->Interval)  {

			idx = (S->Start + i) % S->N;
			glVertex2f((GLfloat)iter,(GLfloat)(S->Trace[idx])  ) ;
			
			/*  deal with recording the max and min stuff here for the auto-fit feature. */

			if(Fits->FitToExtremaNow == 1)
			{
				
				if(i==0) {  /*  initialize to find mins and maxes */
/* 					if(isfinite(double(S->Trace[idx]))) {  only re-initialize ymin and ymax if the value is a number */
											/*  (This protects us again -Inf and +Inf problems */
											/*  that may occur in someone's program */
						ymin = (GLfloat)(S->Trace[idx]);
						ymax = (GLfloat)(S->Trace[idx]);
/* 					} */
				}
				
					if(S->Trace[idx] > ymax)
						ymax = (GLfloat)(S->Trace[idx]);
					if(S->Trace[idx] < ymin)
						ymin = (GLfloat)(S->Trace[idx]);
					
			}
		}
	glEnd();
	
	/* and now, type the current value out in a small helvetical bitmap font at the far right
	   side, and we will let it be in the color of the series  */
	if(idx >= 0 && idx < S->N) {
		sprintf(Str,"%.3f",S->Trace[idx]);
		gfmRenderBitmapString((GLfloat)(S->Lo + S->Span +  .05*(Fits->Padding->xhi * S->Span)),
					S->Trace[idx],GLUT_BITMAP_HELVETICA_10, Str);
	}
	   
	
	
	/*  at the end, if necessary, record the extrema information, and set ExtremaSet to 1 */
	if(Fits->FitToExtremaNow == 1 && Fits->ExtremaSet == 0) {
		Fits->Extrema->xlo = (GLfloat)S->Lo;
		Fits->Extrema->xhi = (GLfloat)(S->Lo + S->Span);
		Fits->Extrema->ylo = ymin;
		Fits->Extrema->yhi = ymax;
		gWindowsSettings[CW]->Fits->Extrema->zlo = -1.0f;
		gWindowsSettings[CW]->Fits->Extrema->zhi = 1.0f;
		
		gWindowsSettings[CW]->Fits->ExtremaSet = 1;
		
	}
	/* following covers the when you draw two or more traces in the same
	    window and you want to fit to the extrema over all of them  */
	else if(Fits->FitToExtremaNow == 1 && Fits->ExtremaSet == 1)  {
		if(Fits->Extrema->ylo > ymin)  Fits->Extrema->ylo = ymin;
		if(Fits->Extrema->yhi < ymax)  Fits->Extrema->yhi = ymax;
	}
}


/*  draw a frame of a rectangle */
void gfduWireRectf(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
	glBegin(GL_LINE_LOOP);
		glVertex2f(x1,y1);
		glVertex2f(x1,y2);
		glVertex2f(x2,y2);
		glVertex2f(x2,y1);
	glEnd();
}



