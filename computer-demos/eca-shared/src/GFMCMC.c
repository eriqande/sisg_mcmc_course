#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "GFMCMC.h"
#include "ECA_MemAlloc.h"


#define fmin(X,Y)  ( (X) < (Y) ? (X) : (Y) )
#define fmax(X,Y)  ( (X) > (Y) ? (X) : (Y) )



/************************************************************************************
*
*      				THE MAIN INITIALIZATION FUNCTION
*	
*				AND MAIN LOOP FUNCTIONS FOR REDRAWING ALL THE CURRENT WINDOWS
*
************************************************************************************/
void gfmInitGFM(void)
{
	int i;
	FILE *temp;
	
	
	/* before entering the GLUT loop, keep track of the current working directory so that
		we will be able to write stuff out to it */
	
	sprintf(gGFM_PWD,"%s",getenv("PWD"));
	
	/* then add that to the Errors File name */
	sprintf(gERROR_FILE,"%s/GFMCMC_Errors.txt",gGFM_PWD);
	
	
	/*  open and close the error file in write mode so that it deletes all its contents */
	temp = fopen(gERROR_FILE,"w");
	fclose(temp); 
	
	
	/*  For compatibility with X-windows systems it seems that glutInit */
	/*  must be called from within main().  So that is what must be done. */
	/*  glutInit is no longer called here. */
	
	/*  fill the window ID reverse lookup with itself, for compatibility with older implementations */
	for(i=0;i<=LARGEST_WINDOW_ID;i++)  {
		gWinIDReverse[i] = i;
	}
	
	/*  get the screen dimensions */
	if( (gSW = glutGet(GLUT_SCREEN_WIDTH)) == 0)  {
		printf("\n\nSorry.  Cannot automatically determine screen width.");
		printf("\nPlease enter screen width in pixels\n->");
		scanf("%d",&gSW);
	}
	if( (gSH = glutGet(GLUT_SCREEN_HEIGHT)) == 0)   {
		printf("\n\nSorry.  Cannot automatically determine screen height.");
		printf("\nPlease enter screen height in pixels\n->");
		scanf("%d",&gSH);
	}
	
	/*  assign reasonable values for the borders	 */
	gHH = 20;
	gWTB = 20;
	gWB = 6;
	gCCWW = gSW / 4;
	
	/*  set all the gWindowsArray elements to NULL */
	gWindowsArray[0] = NULL;  /*  have to set this separately, since CYC_WINDOWS doesn't */
							  /*  include i = 0; */
	CYC_WINDOWS(i)
		gWindowsArray[i] = NULL;
	END1CYCLE
	
	/*  then set all the gWinDefs and gInitWinSettings to NULL to. */
	/* for(i=0;i<=MAX_WIN_DEFS;i++)  { */
	CYC_WIN_DEFS(i)
		gWinDefs[i] = NULL;
		gInitWinSettings[i] = NULL;
	END1CYCLE
	
	/*  set the number of window defs to zero, in prep for getting them from  */
	/*  UserDefd_DefineWindows */
	gNumWinDefs = 0;
	gCreateNewWindowsSubMenu = 0;
	gCurrentlyProcessingConsole = 0;
	
	/*  define the color scheme picker window and the string input window */
	gfmDefineColorPickerWindow();
	gfmDefineStringInputWindow();
	
	
	/*  call the user-defined function that defines the UserDefdMenus */
	gfmUserDefd_DefineMenus();
	
	
	/*  before defining the windows, set the predefined views file name and other */
	/*  file names to a default value to be changed later by the developer */
	gsOUTPUT_FILE_PREFIX("Unnamed");
	
	/*  call the user-defined function that defines the windows */
	gfmUserDefd_DefineWindows();
	/*  after that we need to add one to gNumWinDefs */
	gNumWinDefs++;
	
	gfmDefineGFMMenus();
	
	
	
	Define216Colors();
		
	gfmReadColorSchemesFromFile(gUserDefdColorSchemes,gUserDefdColorSchemesFileName,
			&gNumUserDefdColorSchemes,gUserDefdColorSubMenu);

	gfmColorSchemePredefiningFunc(gPreDefdColorSchemes,
			&gNumPreDefdColorSchemes,gPreDefColorSubMenu);
			
	
	/*  initialize double buffered mode with RGB color model */
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	
	/*  set the global idle func */
	glutIdleFunc(gfmIdleFunc);
	
	/*  Here we will want to create the command console window */
	
	
	/*  finally, let us enter the glutMainLoop with gGo = 0 and with NumSweeps = 0 */
	gGo = 0;
	gNumSweeps = 0;
	gNumSweepsAfterBurnIn = 0;
	gNumViews = 0;
	gNumUDViews = 0;
	gNumOpenWindows = 0;
	gUseSameSeeds = 0;
	
	/* gfmCreateWindowPosSize(gWinDefs[0],gWB, gHH + gWTB, gCCWW,gSH - gHH - gWB - gWTB); */
	

/*  then read the user-defined views */
	gfmReadViewsFromFile(gUDViews, gUserDefdViewsFileName, &gNumUDViews, gUserDefdViewsSubMenu);
	
	/*  then read the predefined views */
	gfmReadViewsFromFile(gViews, gPredDefdViewsFileName, &gNumViews, gPredefinedViewsSubMenu);
	

	
	gfmCreateWindow(gWinDefs[0],gInitWinSettings[0]);
	
	
glutSetMenu(gChainControlSubMenu);
glutAddMenuEntry("Test and Debug this crap          !", 65);
	
	
	
	
	/*  and finally, we call the user's "Last Words" function */
	gfmUserDefd_LastWords();
	
	
	glutMainLoop();

}

void gfmUpdateImages(void)
{
	int i;
	int CurrentWindow = glutGetWindow();  /*  to get the current window */
	ColorScheme3f *CS;
	drag_zoom_struct *DZ;
	
	
	
	
	CYC_WINDOWS(i)
		if(gWindowsArray[i] != NULL) {  /*  if the window is defined, then set the current */
										/*  glut window to this window, execute its drawing */
										/*  commands and then post redisplay for it */
			/*  set the window to the one identified by i */
			glutSetWindow(i);
			
			/*  reset the projection according to the clipping volume. */
			/*  I am hoping that this will keep windows from getting "stuck" */
			gfmClipOrtho();
			
			/*  set the background color according to the ColorScheme */
			CS = gWindowsSettings[i]->ColorScheme;
			glClearColor(CS->Background[0],CS->Background[1],CS->Background[2],1.0f);
			
			/*  clear the background */
			glClear(GL_COLOR_BUFFER_BIT);
			
			/*  execute the drawing command */
			gWindowsArray[i]->DrawFunc();
			
			/*  now make a call to the legend drawing function. */
			/*  the function itself checks to see if it should be drawing */
			/*  a legend or not */
			if(gWindowsArray[i]->NumColorKeys != NULL) {  /*  must be careful here, */
								/*  if it is NULL you get into a world of seg faults */
								/*  on the next line under g++ on Linux.  */
				gfmDrawLegend(gWindowsArray[i]->NumColorKeys[0], gWindowsArray[i]->ColorKey, gWindowsSettings[i]->Clips, CS, gWindowsSettings[i]->Legend);
			}
			
			
			/*  and draw the drag_zoom_box if appropriate */
			if(gWindowsSettings[i]->DZ->DrawDZBox == 1)  {
				DZ = gWindowsSettings[i]->DZ;
				glColor3fv(CS->Axes);
				glBegin(GL_LINE_LOOP);
					glVertex2f(DZ->downX,DZ->downY);
					glVertex2f(DZ->downX,DZ->upY);
					glVertex2f(DZ->upX,DZ->upY);
					glVertex2f(DZ->upX,DZ->downY);
				glEnd();
			}
			
			
			
			/*  now, if the window is marked for it, fit it to the data in whatever way it ought */
			/*  be */
			if(gWindowsSettings[i]->Fits->FitToDefaultsNow == 1) {
				gfmCopyClippings(gWindowsSettings[i]->Fits->Default,gWindowsSettings[i]->Clips);
				gfmClipOrtho();
				
				gWindowsSettings[i]->Fits->FitToDefaultsNow = 0;
			}
			
			if(gWindowsSettings[i]->Fits->FitToExtremaNow == 1 && 
			   gWindowsSettings[i]->Fits->ExtremaSet == 1) {
				gfmSetPaddedExtremeClips(gWindowsSettings[i]);
				gfmClipOrtho();
				
				gWindowsSettings[i]->Fits->ExtremaSet = 0;
				gWindowsSettings[i]->Fits->FitToExtremaNow = 0;
			}
			
			if(gWindowsSettings[i]->Fits->AlwaysFit == 1)  /*  if it is to always be fit: */
				gWindowsSettings[i]->Fits->FitToExtremaNow = 1;
				
			
			/*  swap the window buffer---i.e. put the new image in there */
			glutSwapBuffers();
			
			glutPostRedisplay();
	
		}
	END1CYCLE
	
	/*  at the end, we return the current window to whatever was current when we */
	/*  entered the function, unless the current window was 0 in which case we */
	/*  set the current window to 1 */
	if(CurrentWindow != 0)
		glutSetWindow(CurrentWindow);
	else
		glutSetWindow(1);
}


/*  assign this as the Drawing callback for all windows.  We handle redrawing */
/*  differently, because we need to update all the window on every sweep */
void gfmSilly(void)
{
	return;
}

/*  this is the function that GLUT calls as its idle function.  */
/*  This one, in turn, calls  */
/*  the UserDefd_OneStepByGlobals function, but all */
/*  the stopping and going of the simulation */
/*  takes place within this function */
void gfmIdleFunc(void)
{
	if(gGo==1  || gDoASingleSweep == 1 )  {
		gfmUserDefd_OneStepByGlobals();
		gNumSweeps++;
		gNumSweepsAfterBurnIn++;
		
		gDoASingleSweep = 0;
	}
	
	gfmUpdateImages();
}



/************************************************************************************
*
*      WINDOW, SETTINGS, AND VIEWS MEMORY ALLOCATION/DEALLOCATION, VALUE INITIALIZATION, COPYING
*
************************************************************************************/
window_struct *gfmAllocWindowStruct(void)
{
	window_struct *temp;
	
	temp = (window_struct *)ECA_MALLOC(sizeof(window_struct));

	/*  the NumColorKeys and the names of things should be defined and allocated to elsewhere from */
	/*  within the global chain pointer, so here we just set them to NULL and leave the user to define */
	/*  them in the WinDefs */
	temp->ColorKey = NULL;
	temp->NumColorKeys = NULL;
	
	temp->DefaultColorSchemeIdx = 0;
	
	temp->MiddleMouseMenuIdx = -1;
	
	temp->NoMainMenu = 0;  /*  by default, everything gets a main menu */
	
	/*  then initialize everything else to "neutral-ish" values */
	temp->WindowID = 0;
	sprintf(temp->Title,"Untitled");
	temp->WT = CHANGING_WIN;  /*  by default it should be one that gets changed.  */
		
	temp->DrawFunc = gfmSilly;   /*  set the default drawing function to one that does nothing */
	temp->AtCreation = NULL;
	temp->AtDestruction = NULL;
	temp->KeyboardFunc = NULL;
	temp->ExtraFunction = NULL;

	
	return(temp);
}

window_settings *gfmAllocWindowSettings(void)
{
	window_settings *temp;
	int i;
	
	temp = (window_settings *)ECA_MALLOC(sizeof(window_settings));
	
	
	temp->MenuEntry = 0;
	/*********************
	*
	*	allocating and initializing fitting struct
	*
	**********************/
	temp->Fits = (fitting_struct *)ECA_MALLOC(sizeof(fitting_struct));
	
	temp->Fits->AlwaysFit = 0;
	temp->Fits->FitToExtremaNow = 0;
	temp->Fits->FitToDefaultsNow = 0;
	temp->Fits->ExtremaSet = 0;
	
	temp->Fits->Padding = (clipping_struct *)ECA_MALLOC(sizeof(clipping_struct));
	temp->Fits->Default = (clipping_struct *)ECA_MALLOC(sizeof(clipping_struct));
	temp->Fits->Extrema = (clipping_struct *)ECA_MALLOC(sizeof(clipping_struct));
	
	/*  allocate to the drag zoom struct */
	temp->DZ = (drag_zoom_struct *)ECA_MALLOC(sizeof(drag_zoom_struct));
	gfmInitDragZoom(temp->DZ, 0, 0.0f, 0.0f);
	
	/*  set default padding */
	temp->Fits->Padding->xlo = 0.2f;
	temp->Fits->Padding->xhi = 0.2f;
	temp->Fits->Padding->ylo = 0.2f;
	temp->Fits->Padding->yhi = 0.2f;
	temp->Fits->Padding->zlo = 0.2f;
	temp->Fits->Padding->zhi = 0.2f;
	
	/*  set default fitting clip values */
	temp->Fits->Default->xlo = -0.2f;
	temp->Fits->Default->xhi = 1.2f;
	temp->Fits->Default->ylo = -0.2f;
	temp->Fits->Default->yhi = 1.2f;
	temp->Fits->Default->zlo = -1.0f;
	temp->Fits->Default->zhi = 1.0f;
	
	/*  set the extrema to default values too, just because it is probably a */
	/*  good idea to initialize most things. */
	temp->Fits->Extrema->xlo = -0.2f;
	temp->Fits->Extrema->xhi = 1.2f;
	temp->Fits->Extrema->ylo = -0.2f;
	temp->Fits->Extrema->yhi = 1.2f;
	temp->Fits->Extrema->zlo = -1.0f;
	temp->Fits->Extrema->zhi = 1.0f;

	
	
	/*********************
	*
	*	allocating and initializing the XY_axis structs
	*
	**********************/
	/*  even though most of these won't need the axes, just allocate too them */
	/*  anyway.  Leave it up to the user to call an axis drawing function */
	/*  in the drawing routing that they define */
	temp->Xaxis = (XY_axis_struct *)ECA_MALLOC(sizeof(XY_axis_struct));
	temp->Yaxis = (XY_axis_struct *)ECA_MALLOC(sizeof(XY_axis_struct));
	
	/*  then, set to some default values */
	temp->Xaxis->DrawIt = 1;
	temp->Xaxis->ForceThroughPoint = 0;
	temp->Xaxis->Intersect = 0.0f;
	temp->Xaxis->AxisEdge = AXIS_LEFT_BELOW;
	temp->Xaxis->PropFromEdge = .13f;
	temp->Xaxis->ImposeUpperLimit = 0;
	temp->Xaxis->UpperLimit = 1.0f;
	temp->Xaxis->ImposeLowerLimit = 0;
	temp->Xaxis->LowerLimit = 0.0f;
	temp->Xaxis->OptimalNumMajor = 5;
	temp->Xaxis->TickPos = AXIS_TICKS_CENTERED;
	temp->Xaxis->WriteLabel = 0;
	temp->Xaxis->MajorProp = .022f;
	temp->Xaxis->MinorProp = .4f;
	temp->Xaxis->DrawNumLabels = 1;
	temp->Xaxis->NumbersDist = .09f;
	temp->Xaxis->NumsHeight = .06f;
	
	temp->Yaxis->DrawIt = 1;
	temp->Yaxis->ForceThroughPoint = 0;
	temp->Yaxis->Intersect = 0.0f;
	temp->Yaxis->AxisEdge = AXIS_LEFT_BELOW;
	temp->Yaxis->PropFromEdge = .13f;
	temp->Yaxis->ImposeUpperLimit = 0;
	temp->Yaxis->UpperLimit = 1.0f;
	temp->Yaxis->ImposeLowerLimit = 0;
	temp->Yaxis->LowerLimit = 0.0f;
	temp->Yaxis->OptimalNumMajor = 5;
	temp->Yaxis->TickPos = AXIS_TICKS_CENTERED;
	temp->Yaxis->WriteLabel = 0;
	temp->Yaxis->MajorProp = .022f;
	temp->Yaxis->MinorProp = .4f;
	temp->Yaxis->DrawNumLabels = 1;
	temp->Yaxis->NumbersDist = .025f;
	temp->Yaxis->NumsHeight = .06f;
	
	/*********************
	*
	*	allocating and initializing clipping struct
	*
	**********************/
	temp->Clips = (clipping_struct *)ECA_MALLOC(sizeof(clipping_struct));
	
	/*  set default initial values */
	temp->Clips->xlo = -0.2f;
	temp->Clips->xhi = 1.2f;
	temp->Clips->ylo = -0.2f;
	temp->Clips->yhi = 1.2f;
	temp->Clips->zlo = -1.0f;
	temp->Clips->zhi = 1.0f;
	
	temp->Legend = (LegendSettings *)ECA_MALLOC(sizeof(LegendSettings));
	/*  default initial values: */
	temp->Legend->DrawLegend = 0;
	temp->Legend->AlreadyResized = 0;
	temp->Legend->Type = BARS;
	temp->Legend->Location = LEGEND_TOP;
	temp->Legend->PercentH = .15f;
	temp->Legend->PercentW = .50f;
	temp->Legend->FontSize = 0;
	temp->Legend->xs = .1f;
	temp->Legend->ys = .2f;
	temp->Legend->BufferH = .05f;
	temp->Legend->BufferW = .05f;
	
	
	/*  set it to the first predefined color scheme */
	temp->ColorScheme = gPreDefdColorSchemes[0]; 
	
	/*  initialize the coordinates to the left bottom edge of the screen */
	temp->XCoord = temp->Clips->xlo;
	temp->YCoord = temp->Clips->ylo;
	
	temp->XPos =  4;    	/*  older stuff: gCCWW + 3 * gWB; */
	temp->YPos =  41;      	/*  older stuff: gHH + gWTB; */
	temp->Width = 528;      /*  older stuff:  (gSW - gCCWW - 4 * gWB)/5; */
	temp->Height = 324;     /*  older stuff:  (gSH - gHH - gWB - gWTB)/5; */
	
	temp->NumCols = 1;  /*  initialize to one column */
	temp->SelectedElement = 0;
	temp->DisplayedElement = 0;
	
	
	/*  allocate to the Integer and Float options and initialize them all to zero */
	temp->IntegerOptions = (int *)ECA_CALLOC((size_t)NUM_INTEGER_OPTIONS,sizeof(int));
	temp->FloatOptions = (float *)ECA_CALLOC((size_t)NUM_FLOAT_OPTIONS,sizeof(float));
	
	for(i=0;i<NUM_INTEGER_OPTIONS;i++)  {
		temp->IntegerOptions[i] = 0;
	}
	for(i=0;i<NUM_FLOAT_OPTIONS;i++)  {
		temp->FloatOptions[i] = 0.0f;
	}
	
	
	/* initialize the slider stuff  */
	temp->SlideNewVal[0] = '\0';
	temp->IntSlider=NULL;
	temp->DoubleSlider=NULL;
	temp->SliderHigh=0.0;
	temp->SliderLow=0.0;
	temp->SlideKeyFlag=0;
	
	return(temp);
							  
}


void gfmFreeWindowSettings(window_settings *W)
{
	free(W->Clips);
	free(W->Legend);
	
	/*  Note!  You DO NOT free the color scheme, because that exists separately */
		
	free(W->Fits->Padding);
	free(W->Fits->Extrema);
	free(W->Fits->Default);
	
	free(W->Fits);
	
	free(W->DZ);
	
	free(W->Xaxis);
	free(W->Yaxis);
	
	free(W->IntegerOptions);
	free(W->FloatOptions);
	
	
	free(W);
	
	

}


/*  this assumes that both From and To have been allocated to */
void gfmCopyWindowSettings(window_settings *From, window_settings *To)
{
	int i; 
	
	/*  copy over the clipping volumes */
	To->Clips->xlo = From->Clips->xlo;
	To->Clips->xhi = From->Clips->xhi;
	To->Clips->ylo = From->Clips->ylo;
	To->Clips->yhi = From->Clips->yhi;
	To->Clips->zlo = From->Clips->zlo;
	To->Clips->zhi = From->Clips->zhi;
	
	To->ColorScheme = From->ColorScheme;
	
	
	/*  initialize the coordinates to the left bottom edge of the screen */
	To->XCoord = From->XCoord;
	To->YCoord = From->YCoord;
	
	To->XPos = From->XPos;
	To->YPos = From->YPos;
	To->Width = From->Width;
	To->Height = From->Height;
	To->NumCols = From->NumCols;
	To->SelectedElement = From->SelectedElement;
	To->DisplayedElement = From->DisplayedElement;
	
	gfmCopyLegendSettings(From->Legend, To->Legend);
	
	gfmCopyXYAxisStructs(From->Xaxis,To->Xaxis);
	gfmCopyXYAxisStructs(From->Yaxis,To->Yaxis);
	
	/*  and then copy over the Integer and Float Options */
	for(i=0;i<NUM_INTEGER_OPTIONS;i++)  {
		To->IntegerOptions[i] = From->IntegerOptions[i];
	}
	for(i=0;i<NUM_FLOAT_OPTIONS;i++)  {
		To->FloatOptions[i] = From->FloatOptions[i];
	}
	
	/* copy the slider stuff  */
	sprintf(To->SlideName,"%s",From->SlideName);
	sprintf(To->SlideNewVal,"%s",From->SlideNewVal);
	To->IntSlider = From->IntSlider;
	To->DoubleSlider = From->DoubleSlider;
	To->SliderHigh = From->SliderHigh;
	To->SliderLow = From->SliderLow;
	
}




/*

	Allocate to a view struct pertaining to N open windows

*/
view_struct *gfmAllocView(int N)
{
	int i;
	view_struct *temp;
	
	temp = (view_struct *)ECA_MALLOC(sizeof(view_struct));
	
	temp->NumOpen = N;
	
	temp->OrigWidth = gSW;  /*  this is the default size---the current screen size */
	temp->OrigHeight = gSH;
	
	temp->IDs = (int *)ECA_CALLOC((size_t)temp->NumOpen,sizeof(int));
	
	temp->WS = (window_settings **)ECA_CALLOC((size_t)temp->NumOpen,sizeof(window_settings *));
	
	for(i=0;i<temp->NumOpen;i++)  {
		temp->WS[i] = gfmAllocWindowSettings();
	}
	
	
	return(temp);
	
}



/*
	This is a keyboard callback function that resizes windows to the full screen
	size, and in doing so records their original position, so the next time
	you call it, it returns the window to its original position.
*/
void gfmResizeToFull(void)  
{
	window_settings *WS = gWindowsSettings[glutGetWindow()];
	
	if(WS->IsFullSize == 0)  {
		/*  record current settings */
		WS->bbWidth = glutGet(GLUT_WINDOW_WIDTH);
		WS->bbHeight = glutGet(GLUT_WINDOW_HEIGHT);
		WS->bbXPos = glutGet(GLUT_WINDOW_X);
		WS->bbYPos = glutGet(GLUT_WINDOW_Y);
		
		/*  change to full screen.  Remember to account for screen borders, etc */
		WS->Width = gSW - 2 * gWB;
		WS->Height = gSH - gHH - gWTB - gWB;
		WS->XPos = gWB;
		WS->YPos = gHH + gWTB;
		
		glutPositionWindow(WS->XPos,WS->YPos);
		glutReshapeWindow(WS->Width, WS->Height);
		
		WS->IsFullSize = 1;
		
		gfmClipOrtho();
	
	}
	
	else {
	
		/*  change it back to its original position, etc */
		WS->Width = WS->bbWidth;
		WS->Height = WS->bbHeight;
		WS->XPos = WS->bbXPos;
		WS->YPos = WS->bbYPos;
		
		glutPositionWindow(WS->XPos,WS->YPos);
		glutReshapeWindow(WS->Width, WS->Height);
		
		WS->IsFullSize = 0;
		
		gfmClipOrtho();
		
	}
}



/*

	Initialize the values in the drag_zoom_struct to values that you pass it
*/
void gfmInitDragZoom(drag_zoom_struct *Z, int DrawBox, GLfloat x, GLfloat y)
{
	Z->DrawDZBox = DrawBox;
	
	Z->downX = x;
	Z->downY = y;
	
	Z->upX = x;
	Z->upY = y;
}

/************************************************************************************
*
*      			 WINDOW CREATION / DESTRUCTION / MOVEMENT
*
************************************************************************************/
/*  destroys window with identifier CW, also erases its entry in the "bring to front" */
/*  menu, and decrements the MenuEntry field of all the windows' above it (created after */
/*  it).  Also, frees the memory pointed to by gWindowsSettings[CW] */
void gfmDestroyWindow(int CW)
{
	int i;
	/*  first destroy the menu entry */
	glutSetMenu(gBringToFrontSubMenu);
	glutRemoveMenuItem(gWindowsSettings[CW]->MenuEntry);
	
	/*  then cycle over the remaining windows and decrement the Menu Entries if appropriate */
	CYC_WINDOWS(i)
		if(gWindowsSettings[i] != NULL) {
			if(gWindowsSettings[i]->MenuEntry > gWindowsSettings[CW]->MenuEntry)
				gWindowsSettings[i]->MenuEntry--;
		}
	END1CYCLE
	
	/*  then destroy the window itself */
	glutDestroyWindow(CW);
	
	/*  and make sure that its corresponding pointer in gWindowsArray is NULL */
	gWindowsArray[CW] = NULL;
	
	/*  and decrement the number of open windows, too! */
	gNumOpenWindows--;
	
	/*  then free gWindowSettings[CW] */
	gfmFreeWindowSettings(gWindowsSettings[CW]);
	gWindowsSettings[CW] = NULL;
	
	/*  and set the current window back to the info window */
	/* glutSetWindow(1); */
	/*  actually probably not necessary, so I have commented it out. */
	
	

}



/*  create a window with the specifications in W having  */
/*  settings initialized as in InitWS, unless InitWS == NULL */
/*  in which case the default values set in gfmAllocWindowSettings */
/*  are used */
void gfmCreateWindow(window_struct *W, window_settings *InitWS) 
{	
	int temp;
	int CW = glutGetWindow();
	window_settings *WS;
	
	/*  call the function that should be called at window creation */
	if(W->AtCreation != NULL)
		W->AtCreation();
	
	/*  first, create a default windows setting struct */
	WS = gfmAllocWindowSettings();
	
	/*  if there are initial settings stored in InitWS, then use those */
	/*  AND copy the initial clipping volumes into the default fitting volumes */
	if(InitWS != NULL)  {
		gfmCopyWindowSettings(InitWS,WS);
		
		WS->Fits->Default->xlo = InitWS->Clips->xlo;
		WS->Fits->Default->xhi = InitWS->Clips->xhi;
		WS->Fits->Default->ylo = InitWS->Clips->ylo;
		WS->Fits->Default->yhi = InitWS->Clips->yhi;
		WS->Fits->Default->zlo = InitWS->Clips->zlo;
		WS->Fits->Default->zhi = InitWS->Clips->zhi;
	}
	
	/*  set the default color scheme according to what is in the window_struct */
	WS->ColorScheme = gPreDefdColorSchemes[W->DefaultColorSchemeIdx];
	
	
	
	
	/*  set the default size and position of the window */
	glutInitWindowSize( WS->Width, WS->Height);
	glutInitWindowPosition(WS->XPos, WS->YPos);
	
	WS->IsFullSize = 0;
	WS->bbWidth = WS->Width;
	WS->bbHeight = WS->Height;
	WS->bbXPos = WS->XPos;
	WS->bbYPos = WS->YPos;
	
	/*  create the window and let the global window array point to it */
	/*  doing some acrobatics if too many windows are already open */
	temp = glutCreateWindow(W->Title);
	if( temp > MAX_WIN )  {
		printf("\nWHOA!  Too many windows open.  Can't create new window!\n\n");
		
		/*  destroy the window that was just created */
		glutDestroyWindow(temp);
		
		/*  and then set the current window back to where it was when the */
		/*  program entered the function */
		glutSetWindow(CW);
	}
	else {  /*  if there is plenty of window space, point gWindowsArray[temp] to W */
			/*  and then define all the callbacks and things, and set the background */
			/*  color */
		gWindowsArray[temp] = W;
		glutDisplayFunc(gfmSilly);  
	    glutReshapeFunc(gfmChangeSize);
	    if(W->KeyboardFunc == NULL)
		    glutKeyboardFunc(gfmProcessNormalKeys);
		else
			glutKeyboardFunc(W->KeyboardFunc);
	    glutMotionFunc(gfmMouseLocation);
	    glutSpecialFunc(gfmProcessSpecialKeys);
	    glutMouseFunc(gfmStandardMouseClick);	     

	
		glutSetMenu(gMainMenu);
		glutAttachMenu(GLUT_RIGHT_BUTTON);
		

		/*  attach the appropriate user-defined menu to the right hand mouse-click button */
		if(W->MiddleMouseMenuIdx > -1)  {
			glutSetMenu(gUserDefdMenus[W->MiddleMouseMenuIdx]);
			glutAttachMenu(GLUT_MIDDLE_BUTTON);
		}
		/*  then direct a pointer in gWindowsSettings to WS */
		gWindowsSettings[glutGetWindow()] = WS;
		
		/*  NOW, we increment the gNumOpenWindows, and use that to tell which menu entry */
		/*  in the BringToFrontSubMenu the window is attached to.  AND, we set that */
		/*  menu entry */
		gNumOpenWindows++;
		gWindowsSettings[temp]->MenuEntry = gNumOpenWindows;
		glutSetMenu(gBringToFrontSubMenu);
		glutAddMenuEntry(W->Title,temp);	
		
		/*  at the end here we will call the "Extras Function" which just */
		/*  allows the user to do some extra little things as if working directly with GLUT */
		if(W->ExtraFunction != NULL) 
			W->ExtraFunction();
	
	}	
		
}


/*  To be a menu callback, so that the menu options can */
/*  open windows. */
void gfmCreateWindowByIndex(int idx)
{
	idx = gWinIDReverse[idx];
	gfmCreateWindow(gWinDefs[idx],gInitWinSettings[idx]);
}


/*  this is the basic function for opening the windows associated */
/*  with the L-th view in the array of views V. */
void gfmCreateWindowsForView(int L, view_struct **V)
{
	int CW,i,j;
	GLfloat w_scale = (GLfloat)gSW/(GLfloat)V[L]->OrigWidth, 
			h_scale = (GLfloat)gSH/(GLfloat)V[L]->OrigHeight;
			
	window_settings *tempWS = gfmAllocWindowSettings();
	
	
	if(V[L]->NumOpen <= 0)  /*  get out of here if the view doesn't hold anything special */
		return;

	/*  then obliterate all the other current windows including the info window */
	CYC_WINDOWS(CW)
		if(CW > 1 && gWindowsArray[CW] != NULL)  {
			gfmDestroyWindow(CW);
		}
	END1CYCLE
			
	/*  then cycle over the windows in the view and create them all */
	for(i=0;i<V[L]->NumOpen;i++)  {
		j = gWinIDReverse[ V[L]->IDs[i] ];
		
		/*  copy the window settings from the view, to a temporary variable */
		gfmCopyWindowSettings(V[L]->WS[i], tempWS);
		
		/*  now, change the position and size variables in this window */
		/*  so it corresponds to scaling the view to fit in the new window area */
		gfmScaleWindowPosAndSizeViewWise(tempWS, w_scale, h_scale);
		
		/*  now, here we copy the clipping settings from gInitWinSettings to */
		/*  the window about to be created.  But only for the non-info windows */
		if(j != 0)  {
			if(gInitWinSettings[j] != NULL)  {
				gfmCopyClippings(gInitWinSettings[j]->Clips,tempWS->Clips);
				/* and here we will copy all the slider bar stuff, in case it is a slider bar 
					window.  This is a huge hack, but, it's the way it is as the moment */
				sprintf(tempWS->SlideName, "%s",gInitWinSettings[j]->SlideName);
				tempWS->SlideNewVal[0] = '\0';
				tempWS->IntSlider = gInitWinSettings[j]->IntSlider;
				tempWS->DoubleSlider = gInitWinSettings[j]->DoubleSlider;
				tempWS->SliderHigh = gInitWinSettings[j]->SliderHigh;
				tempWS->SliderLow = gInitWinSettings[j]->SliderLow;
			}	
			else {
				tempWS->Clips->xlo = -.2f;
				tempWS->Clips->ylo = -.2f;
				tempWS->Clips->zlo = -1.0f;
				tempWS->Clips->xhi = 1.2f;
				tempWS->Clips->yhi = 1.2f;
				tempWS->Clips->zhi = 1.0f;
				
			}
		}
		
		
		
		
		
		
		/*  then create the window */
		gfmCreateWindow(gWinDefs[j],tempWS);
	}
	
	/*  then destroy the temporary window */
/* 	glutDestroyWindow(TempWin); */
	
	
	
	/*  NOW, because the info window does not seem to get approriately resized on the PC,  */
	/*  I am going to try a new approach.  So long as the number of open windows in the view */
	/*  is greater than one, then by this point we have opened up a bunch of new windows */
	/*  (or at least one) so we should be free to close the info window, and then open  */
	/*  a new version of it with the new window settings.   */
/* 	gfmCopyWindowSettings(V[L]->WS[0], tempWS); */
			
	free(tempWS);
		 
}





/*  this is the menu callback for predefined menus */
void gfmCreateWindowsForPreDefdViews(int L)
{
	gfmCreateWindowsForView(L,gViews);
}

/*  this is the menu callback for user-defined menus */
void gfmCreateWindowsForUserDefdViews(int L)
{
	gfmCreateWindowsForView(L,gUDViews);
}




void gfmBringWindowToFront(int W)
{
	glutSetWindow(W);
	glutPopWindow();
}




/************************************************************************************
*
*     RECORDING NEW VIEWS, READING AND WRITING SETTINGS AND VIEWS TO FILES
*
************************************************************************************/

/* 

	Record the current view into memory (into gViews), then create a sub-menu
	entry to it, and also write a new Views text file with this new addition in it

*/
void gfmRecordNewView(int Dummy)
{
	int i,c;
	int NumW = 0;  /*  the current number of windows open */
	
	/*  just set it to zero so the compiler thinks it is being used */
	Dummy = 0;
	
	if(gNumUDViews == MAX_VIEWS)  /*  if we already have as many as we can hold, do nothing */
		return;
	
	/*  otherwise we will allocate memory for a new view */
	
	/*  first determine how many windows are open */
	CYC_WINDOWS(i)
		if(gWindowsArray[i] != NULL)
			NumW++;
	END1CYCLE
	
	/*  then allocate to the view, and add it to the end of the gUDViews array */
	gUDViews[gNumUDViews] = gfmAllocView(NumW);
	
	/*  let c be the index of the view we are currently recording */
	c = gNumUDViews;
	
	/*  increment the total number of views */
	gNumUDViews++;
	
	/*  now, cycle over all the windows again, and this time record all their info */
	NumW = 0;
	CYC_WINDOWS(i)
		if(gWindowsArray[i] != NULL)  {
		
			/*  here is a nasty thing I have to do because gfmChangeSize only gets */
			/*  called if the window is resized, not if it is just changed.  So we have */
			/*  to cycle through the windows here and take care of things */
			glutSetWindow(i);
		
			/*  now record all the new stuff in the appropriate windows setting box */
			gWindowsSettings[i]->XPos = glutGet(GLUT_WINDOW_X);
			gWindowsSettings[i]->YPos = glutGet(GLUT_WINDOW_Y);
			gWindowsSettings[i]->Width = glutGet(GLUT_WINDOW_WIDTH);
			gWindowsSettings[i]->Height = glutGet(GLUT_WINDOW_HEIGHT);
			
			
			
			gUDViews[c]->IDs[NumW] = gWindowsArray[i]->WindowID;
			gfmCopyWindowSettings(gWindowsSettings[i],gUDViews[c]->WS[NumW]);
			
			NumW++;
		}
	END1CYCLE
	
	
	gfmHandleStringInput("OK! The view information is stored",
						"Enter a name for this view:", 
						"Hit return when done.", gUDViews[c]->Name,
						gfmUponEntryOfViewName );


}


void gfmUponEntryOfViewName(void)
{
	char TempName[100];
	
	/*  add the menu entry for this new view */
	glutSetMenu(gUserDefdViewsSubMenu);
	
	sprintf(TempName,"%s       alt-%d",gUDViews[gNumUDViews-1]->Name,  gNumUDViews);
	
	glutAddMenuEntry(TempName,gNumUDViews-1);
	
	/*  then write the views file over with this new addition */
	gfmWriteViewsToFile(gUDViews,gUserDefdViewsFileName);
}

/*

	Write the settings in WS to a text file, that will be easy to read back in with a 
	companion function (gfmReadWindowSettingsFromFile)

*/
void gfmWriteWindowSettings(window_settings *WS, FILE *F)
{
	int i;
	
	/*  print out the position and the dimensions of the windows */
	fprintf(F,"\n\t\tXPos %d",WS->XPos);
	fprintf(F,"\n\t\tYpos %d",WS->YPos);
	fprintf(F,"\n\t\tWidth %d",WS->Width);
	fprintf(F,"\n\t\tHeight %d",WS->Height);
	
	/*  then print out the clips */
	fprintf(F,"\n\t\tClips:");
	gfmWriteClippingStructToFile(WS->Clips,F);
	
	fprintf(F,"\n\t\tColorSchemeID: %d",WS->ColorScheme->ID);
	
	fprintf(F,"\n\t\tFits:");
	gfmWriteFittingStructToFile(WS->Fits,F);
	
	fprintf(F,"\n\t\tXAxis:");
	gfmWriteXYAxisSettingsToFile(WS->Xaxis,F);
	fprintf(F,"\n\t\tYAxis:");
	gfmWriteXYAxisSettingsToFile(WS->Yaxis,F);
	
	
	fprintf(F,"\n\t\tXCoord %f",WS->XCoord);
	fprintf(F,"\n\t\tYCoord %f",WS->YCoord);
	fprintf(F,"\n\t\tNumCols %d",WS->NumCols);
	fprintf(F,"\n\t\tSelectedElement %d",WS->SelectedElement);
	fprintf(F,"\n\t\tDisplayedElement %d",WS->DisplayedElement);
	
	/*  finally, do the integer and the float options */
	fprintf(F,"\n\t\tIntegerOptions:");
	for(i=0;i<NUM_INTEGER_OPTIONS;i++)  {
		if(i%50==0)
			fprintf(F,"\n\t\t\t");
		fprintf(F,"%d ",WS->IntegerOptions[i]);
	}
	fprintf(F,"\n\t\tFloatOptions:");
	for(i=0;i<NUM_FLOAT_OPTIONS;i++)  {
		if(i%20==0)
			fprintf(F,"\n\t\t\t");
		fprintf(F,"%f ",WS->FloatOptions[i]);
	}
	
		
}



/*

	Read in all the settings for a window from the current 
	stream of file F.  

*/
void gfmReadWindowSettingsFromFile(window_settings *WS, FILE *F)
{
	int i;
	
	/*  print out the position and the dimensions of the windows */
	fscanf(F," XPos %d",&(WS->XPos));
	fscanf(F," Ypos %d",&(WS->YPos));
	fscanf(F," Width %d",&(WS->Width));
	fscanf(F," Height %d",&(WS->Height));
	
	/*  then print out the clips */
	fscanf(F," Clips:");
	gfmReadClippingStructFromFile(WS->Clips,F);
	
	fscanf(F," ColorSchemeID: %d",&(WS->ColorScheme->ID));
	
	fscanf(F," Fits:");
	gfmReadFittingStructFromFile(WS->Fits,F);
	
	fscanf(F," XAxis:");
	gfmReadXYAxisSettingsToFile(WS->Xaxis,F);
	fscanf(F," YAxis:");
	gfmReadXYAxisSettingsToFile(WS->Yaxis,F);
	
	fscanf(F," XCoord %f",&(WS->XCoord));
	fscanf(F," YCoord %f",&(WS->YCoord));
	fscanf(F," NumCols %d",&(WS->NumCols));
	fscanf(F," SelectedElement %d",&(WS->SelectedElement));
	fscanf(F," DisplayedElement %d",&(WS->DisplayedElement));
	
	/*  finally, do the integer and the float options */
	fscanf(F," IntegerOptions:");
	for(i=0;i<NUM_INTEGER_OPTIONS;i++)  {
		fscanf(F,"%d ",&(WS->IntegerOptions[i]));
	}
	fscanf(F," FloatOptions:");
	for(i=0;i<NUM_FLOAT_OPTIONS;i++)  {
		fscanf(F,"%f ",&(WS->FloatOptions[i]));
	}

}





/*

	Write the currently available views to a file named FileName.
	Obliterates the contents of the file and writes all the views
	back to it.

*/
void gfmWriteViewsToFile(view_struct **V, char *FileName)
{
	int i,j;
	FILE *F;
	char *temp;
	char another[5000];
	char TempName[1000];
	
	
	/* by default we are going to write this thing to ~/Library/gfmcmc/FileName */
	if((temp=getenv("HOME")) == NULL) {
		sprintf(TempName,"\nWarning!  Cannot getenv $HOME to write to user defined view file %s",FileName);
		gfmAppendToFile(gERROR_FILE,TempName);
		return;
	}
	else {
		sprintf(another,"%s/Library/gfmcmc/%s",temp,FileName); 
	}
	/*  open the file */
	F = fopen(another,"w");
	
	/*  write the number of views available and leave two returns after that */
	fprintf(F,"NumberOfViews %d",gNumUDViews);
	
	/*  then cycle through the views to print them */
	for(i=0;i<gNumUDViews;i++)  {
		fprintf(F,"\n\n\n\n\n\n");
		/*  print the name of the view and the number of open windows in each view */
		fprintf(F,"ViewName: %s NumberOfWindowsInView: %d",V[i]->Name, V[i]->NumOpen);
		
		fprintf(F,"\n\tScreenWidthAndHeight: %d %d",V[i]->OrigWidth, V[i]->OrigHeight);
		
		/*  then cycle over all those windows and write them all out */
		for(j=0;j<V[i]->NumOpen;j++)  {
			fprintf(F,"\n\tWindowIdentifier: %d  ",V[i]->IDs[j]);
			gfmWriteWindowSettings(V[i]->WS[j],F);
		}	
		
	}



	fclose(F);
}




/*

	Allocates memory to the view structs and then reads
	in their values

*/
void gfmReadViewsFromFile(view_struct **V, char *FileName, int *Num, int Menu)
{
	int i,j;
	FILE *F;
	char TempName[500];
	int TempN;
	char *temp;
	char another[5000];
	int tries=0;
	
	/* if we have a good environment variable set, then use that */
	if((temp=getenv("GFMCMC_PATH")) != NULL) {
		sprintf(another,"%s/%s",temp,FileName);
	}
	else {
		
	}
	
	/* start looking for the file in the home directory */
/*	sprintf(another,"%s/%s",gGFM_PWD,FileName); */
	
	/*  try opening the file in a couple of different places */
	/* I had originally set it up so that the thing looked first in the home directory, and then 
		  in another directory and so forth.  But I think it will be better for it to just always go
		  to the Library/gfmcmc directory.  So I comment the line below and set tries to 2.  I can 
		  easily resurrect this if I wish.  */
/*	while( (F = fopen(another,"r"))  == 0 && tries<4 ) {
	
	
		tries=
		switch (tries) {
			case 0:
				sprintf(TempName,"\nWarning!  Cannot open file %s in working directory...",FileName, gGFM_PWD);
				sprintf(another,"%s/Library/gfmcmc/%s",getenv("HOME"),FileName);  
				break;
			case 1:
				sprintf(TempName,"\nWarning!  Cannot open file %s in %s/Library/gfmcmc.  Home is %s...",FileName,getenv("HOME"),getenv("HOME"));
				sprintf(another,"%s/%s",getenv("GFMCMC_PATH"),FileName);
				break;
			case 2:
				sprintf(TempName,"\nWarning!  Cannot open file %s in GFMCMC_PATH=%s...",FileName,getenv("GFMCMC_PATH"));
				sprintf(another,"/usr/local/share/gfmcmc/%s",FileName);
				break;
			case 3:
				sprintf(TempName,"\nWarning!  Cannot open file %s in /usr/local/share/gfmcmc...",FileName);
				return;
				break;
		}
	}
*/
	
#ifdef COMPILE_FOR_PC_WIN32
	sprintf(another,"%s/%s",gGFM_PWD,FileName);
#else
	sprintf(another,"%s/Library/gfmcmc/%s",getenv("HOME"),FileName);
#endif
		if(  (F=fopen(another,"r"))==NULL) {
			sprintf(TempName,"\nWarning!  Cannot open file %s in %s/Library/gfmcmc.  Home is %s...",FileName,getenv("HOME"),getenv("HOME"));
			gfmAppendToFile(gERROR_FILE,TempName);
			return;
		}
		*Num = 0;
		tries++;

		
		
	
	/*  read the number of views available */
	fscanf(F,"NumberOfViews %d",Num);
	
/*	printf("NumberOfViews: %d\n",*Num); */
	
	/*  if there are too many, then only do as many as can be handled */
	if(*Num > MAX_VIEWS)
		*Num = MAX_VIEWS;
	
	/*  then cycle through the views to read them */
	for(i=0;i<*Num;i++)  {
	
		/*  scan the name of the view and the number of open windows in each view */
		fscanf(F," ViewName: %s NumberOfWindowsInView: %d",TempName, &TempN );
		
		/*  allocate space to the View */
		V[i] = gfmAllocView(TempN);
		
		/*  record the size of screen when the view was made */
		fscanf(F," ScreenWidthAndHeight: %d %d",&(V[i]->OrigWidth), &(V[i]->OrigHeight));
		
		/*  Then copy those two values into memory */
		sprintf(V[i]->Name,"%s",TempName);
		V[i]->NumOpen = TempN;
		
		/*  then cycle over all those windows and read them all in */
		for(j=0;j<V[i]->NumOpen;j++)  {
			fscanf(F," WindowIdentifier: %d  ",&(V[i]->IDs[j]));
			gfmReadWindowSettingsFromFile(V[i]->WS[j],F);
		}	
		
		/*  add the menu entry for this new view */
		glutSetMenu(Menu);
		/*  make the name for the menu entry include the number of the view too, if it is */
		/*  for one of our two menus */
		if(Menu == gPredefinedViewsSubMenu)	
			sprintf(TempName,"%s       %d",V[i]->Name,i+1);
		else if(Menu == gUserDefdViewsSubMenu)
			sprintf(TempName,"%s       alt-%d",V[i]->Name,i+1);
		else
			sprintf(TempName,"%s",V[i]->Name);

/* printf("Menu = %d;  gPredefinedViewsSubMenu = %d, TempName = %s\n",Menu,gPredefinedViewsSubMenu,TempName); */
			
		glutSetMenu(Menu);
		glutAddMenuEntry(TempName,i);
	}
	
	
	

}


/*  scale all the windows in a view by the factors w and h (in the width and height */
/*  directions respectively.  This takes into account the borders around the windows */
/*  (specified currently as #defined number of pixels. */
void gfmScaleTheView(GLfloat w, GLfloat h)
{
	int TrueTop, TrueLeft, TrueWidth, TrueHeight;  /*  the window dimensions and */
												   /*  positions counting from the */
												   /*  edges INCLUDING the borders. */
								/*  also, TrueTop's "zero" position is at the bottom of */
								/*  the top menu bar, if any. */
	int NTTop, NTLeft, NTWidth, NTHeight;  /*  "New True Top" etc.  The dimensions */
										   /*  of the outer edges of the borders desired */
										   /*  after rescaling */
	int ITop, ILeft, IWidth, IHeight;  /*  The "inner" top, left, etc.  (inner meaning */
									   /*  the actual window area inside the border) */
									   
									   
	int i,CW = glutGetWindow();
	
	CYC_WINDOWS(i)
		if(gWindowsArray[i] != NULL)  {
			glutSetWindow(i);
			
			/*  compute the true outer boundaries of the window */
			TrueTop = glutGet(GLUT_WINDOW_Y) - GFM_TOP_BAR - GFM_T;
			TrueLeft = glutGet(GLUT_WINDOW_X) - GFM_L;
			TrueWidth = glutGet(GLUT_WINDOW_WIDTH) + GFM_L + GFM_R;
			TrueHeight = glutGet(GLUT_WINDOW_HEIGHT) + GFM_T + GFM_B;
			
			/*  then compute what the new outer boundaries of the window should be */
			/*  for now just round down by casting to int */
			NTTop = (int)( (GLfloat)TrueTop * h );
			NTLeft = (int) ( (GLfloat)TrueLeft * w);
			NTWidth = (int) ( (GLfloat)TrueWidth * w);
			NTHeight = (int)( (GLfloat)TrueHeight * h);
			
			/*  then set the true inner dimensions to what they need to be */
			ITop = NTTop + GFM_TOP_BAR + GFM_T;
			ILeft = NTLeft + GFM_L;
			IWidth = NTWidth - GFM_L - GFM_R;
			IHeight = NTHeight - GFM_T - GFM_B;
			
			/*  catch problems with width and height being zero */
			if(IWidth<=0)
				IWidth = 1;
			if(IHeight<=0)
				IHeight = 1;
			
			/*  then readjust the size and position on that thing */
			glutPositionWindow(ILeft,ITop);
			glutReshapeWindow(IWidth,IHeight);
			
			gfmClipOrtho();  /*  and at the end of that, reset the clipping volume, etc			 */
		
		}
	END1CYCLE
	
	/*  finally, having gotten through all the windows, we reset the current window */
	/*  back to the one it was in when the function was called */
	glutSetWindow(CW);
}


/*  this will look at the original size and position of a window, as given in */
/*  the relevant variables in WS, and it will change those variables in a manner */
/*  that has the effect of scaling the entire view that this window may be a part */
/*  of by the width and height proportions w and h. */
/*  This function should be called on the window's initial settings, BEFORE the  */
/*  window gets created */
void gfmScaleWindowPosAndSizeViewWise(window_settings *W, GLfloat w, GLfloat h)
{
	int TrueTop, TrueLeft, TrueWidth, TrueHeight;  /*  the window dimensions and */
												   /*  positions counting from the */
												   /*  edges INCLUDING the borders. */
								/*  also, TrueTop's "zero" position is at the bottom of */
								/*  the top menu bar, if any. */
	int NTTop, NTLeft, NTWidth, NTHeight;  /*  "New True Top" etc.  The dimensions */
										   /*  of the outer edges of the borders desired */
										   /*  after rescaling */
	int ITop, ILeft, IWidth, IHeight;  /*  The "inner" top, left, etc.  (inner meaning */
									   /*  the actual window area inside the border) */
									   
	/*  compute the true outer boundaries of the window */
	TrueTop = W->YPos - GFM_TOP_BAR - GFM_T;
	TrueLeft = W->XPos - GFM_L;
	TrueWidth = W->Width + GFM_L + GFM_R;
	TrueHeight = W->Height + GFM_T + GFM_B;
	
	/*  then compute what the new outer boundaries of the window should be */
	/*  for now just round down by casting to int */
	NTTop = (int)( (GLfloat)TrueTop * h );
	NTLeft = (int) ( (GLfloat)TrueLeft * w);
	NTWidth = (int) ( (GLfloat)TrueWidth * w);
	NTHeight = (int)( (GLfloat)TrueHeight * h);
	
	/*  then set the true inner dimensions to what they need to be */
	ITop = NTTop + GFM_TOP_BAR + GFM_T;
	ILeft = NTLeft + GFM_L;
	IWidth = NTWidth - GFM_L - GFM_R;
	IHeight = NTHeight - GFM_T - GFM_B;
	
	/*  catch problems with width and height being zero */
	if(IWidth<=0)
		IWidth = 1;
	if(IHeight<=0)
		IHeight = 1;
		
	
	/*  now, store the new values in W: */
	W->XPos = ILeft;
	W->YPos = ITop;
	W->Width = IWidth;
	W->Height = IHeight;
			
									   
	
}

void gfmWriteFittingStructToFile(fitting_struct *F, FILE *out)
{
	fprintf(out,"\n\t\t\tAlwaysFit %d",F->AlwaysFit);
	fprintf(out,"\n\t\t\tFitToExtremaNow %d",F->FitToExtremaNow);
	fprintf(out,"\n\t\t\tFitToDefaultsNow %d",F->FitToDefaultsNow);
	fprintf(out,"\n\t\t\tExtremaSet %d",F->ExtremaSet);
	fprintf(out,"\n\t\t\tPadding");
	gfmWriteClippingStructToFile(F->Padding,out);
	fprintf(out,"\n\t\t\tExtrema");
	gfmWriteClippingStructToFile(F->Extrema,out);
	fprintf(out,"\n\t\t\tDefault");
	gfmWriteClippingStructToFile(F->Default,out);
	
}

void gfmReadFittingStructFromFile(fitting_struct *F, FILE *in)
{
	fscanf(in," AlwaysFit %d",&(F->AlwaysFit));
	fscanf(in," FitToExtremaNow %d",&(F->FitToExtremaNow));
	fscanf(in," FitToDefaultsNow %d",&(F->FitToDefaultsNow));
	fscanf(in," ExtremaSet %d",&(F->ExtremaSet));
	fscanf(in," Padding");
	gfmReadClippingStructFromFile(F->Padding,in);
	fscanf(in," Extrema");
	gfmReadClippingStructFromFile(F->Extrema,in);
	fscanf(in," Default");
	gfmReadClippingStructFromFile(F->Default,in);
	
}

void gfmWriteClippingStructToFile(clipping_struct *C,FILE *out)
{
	fprintf(out,"\n\t\t\t\txlo %f",C->xlo);
	fprintf(out,"\n\t\t\t\txhi %f",C->xhi);
	fprintf(out,"\n\t\t\t\tylo %f",C->ylo);
	fprintf(out,"\n\t\t\t\tyhi %f",C->yhi);
	fprintf(out,"\n\t\t\t\tzlo %f",C->zlo);
	fprintf(out,"\n\t\t\t\tzhi %f",C->zhi);
}

void gfmReadClippingStructFromFile(clipping_struct *C,FILE *in)
{
	fscanf(in," xlo %f",&(C->xlo));
	fscanf(in," xhi %f",&(C->xhi));
	fscanf(in," ylo %f",&(C->ylo));
	fscanf(in," yhi %f",&(C->yhi));
	fscanf(in," zlo %f",&(C->zlo));
	fscanf(in," zhi %f",&(C->zhi));
}

void gfmWriteXYAxisSettingsToFile(XY_axis_struct *A, FILE *out)
{
	/*  then, set to some default values */
	fprintf(out,"\n\t\t\tDrawIt = %d",A->DrawIt);
	fprintf(out,"\n\t\t\tForceThroughPoint = %d",A->ForceThroughPoint);
	fprintf(out,"\n\t\t\tIntersect = %f",A->Intersect);
	fprintf(out,"\n\t\t\tAxisEdge = %d",A->AxisEdge);
	fprintf(out,"\n\t\t\tPropFromEdge = %f",A->PropFromEdge);
	fprintf(out,"\n\t\t\tImposeUpperLimit = %d",A->ImposeUpperLimit);
	fprintf(out,"\n\t\t\tUpperLimit = %f",A->UpperLimit);
	fprintf(out,"\n\t\t\tImposeLowerLimit = %d",A->ImposeLowerLimit);
	fprintf(out,"\n\t\t\tLowerLimit = %f",A->LowerLimit);
	fprintf(out,"\n\t\t\tOptimalNumMajor = %d",A->OptimalNumMajor);
	fprintf(out,"\n\t\t\tTickPos = %d",A->TickPos);
	fprintf(out,"\n\t\t\tWriteLabel = %d",A->WriteLabel);
	fprintf(out,"\n\t\t\tMajorProp = %f",A->MajorProp);
	fprintf(out,"\n\t\t\tMinorProp = %f",A->MinorProp);
	fprintf(out,"\n\t\t\tDrawNumLabels = %d",A->DrawNumLabels);
	fprintf(out,"\n\t\t\tNumbersDist = %f",A->NumbersDist);
	fprintf(out,"\n\t\t\tNumsHeight = %f",A->NumsHeight);
	
}

void gfmReadXYAxisSettingsToFile(XY_axis_struct *A, FILE *out)
{
	/*  then, set to some default values */
	fscanf(out," DrawIt = %d",&(A->DrawIt));
	fscanf(out," ForceThroughPoint = %d",&(A->ForceThroughPoint));
	fscanf(out," Intersect = %f",&(A->Intersect));
	fscanf(out," AxisEdge = %d",&(A->AxisEdge));
	fscanf(out," PropFromEdge = %f",&(A->PropFromEdge));
	fscanf(out," ImposeUpperLimit = %d",&(A->ImposeUpperLimit));
	fscanf(out," UpperLimit = %f",&(A->UpperLimit));
	fscanf(out," ImposeLowerLimit = %d",&(A->ImposeLowerLimit));
	fscanf(out," LowerLimit = %f",&(A->LowerLimit));
	fscanf(out," OptimalNumMajor = %d",&(A->OptimalNumMajor));
	fscanf(out," TickPos = %d",&(A->TickPos));
	fscanf(out," WriteLabel = %d",&(A->WriteLabel));
	fscanf(out," MajorProp = %f",&(A->MajorProp));
	fscanf(out," MinorProp = %f",&(A->MinorProp));
	fscanf(out," DrawNumLabels = %d",&(A->DrawNumLabels));
	fscanf(out," NumbersDist = %f",&(A->NumbersDist));
	fscanf(out," NumsHeight = %f",&(A->NumsHeight));
	
}


/************************************************************************************
*
*    						 VIEWABLE AREA AND SUCH
*
************************************************************************************/
/*  return the aspect ratio of things on the screen for 2-D images.  THis is Width Over Height */
/*  of the current window */
GLfloat gfmWoH(void)
{
	GLfloat temp;
	clipping_struct *C = gWindowsSettings[glutGetWindow()]->Clips;
	
	temp = (GLfloat)glutGet(GLUT_WINDOW_WIDTH) / (C->xhi - C->xlo);
	temp /= (GLfloat)glutGet(GLUT_WINDOW_HEIGHT) / (C->yhi - C->ylo);
	
	return(temp);
}

/*  define the clipping for a unit square with a border */
void gfmUnitSquareWithBorder(window_settings *W, float border)
{
	W->Clips->xlo = 0.0f - border;
	W->Clips->xhi = 1.0f + border;
	W->Clips->ylo = 0.0f - border;
	W->Clips->yhi = 1.0f + border;
	W->Clips->zlo = -1.0f;
	W->Clips->zhi = 1.0f;
}


/*  copy one clipping struct into another.  Both must be already allocated to */
void gfmCopyClippings(clipping_struct *From,clipping_struct *To)
{
	To->xlo = From->xlo;
	To->xhi = From->xhi;
	To->ylo = From->ylo;
	To->yhi = From->yhi;
	To->zlo = From->zlo;
	To->zhi = From->zhi;
}


/*  copy one clipping struct into another.  Both must be already allocated to */
void gfmSetPaddedExtremeClips(window_settings *WS)
{
	WS->Clips->xlo = WS->Fits->Extrema->xlo - WS->Fits->Padding->xlo * (WS->Fits->Extrema->xhi - WS->Fits->Extrema->xlo);
	WS->Clips->xhi = WS->Fits->Extrema->xhi + WS->Fits->Padding->xhi * (WS->Fits->Extrema->xhi - WS->Fits->Extrema->xlo);
	WS->Clips->ylo = WS->Fits->Extrema->ylo - WS->Fits->Padding->ylo * (WS->Fits->Extrema->yhi - WS->Fits->Extrema->ylo);
	WS->Clips->yhi = WS->Fits->Extrema->yhi + WS->Fits->Padding->yhi * (WS->Fits->Extrema->yhi - WS->Fits->Extrema->ylo);
	WS->Clips->zlo = WS->Fits->Extrema->zlo - WS->Fits->Padding->zlo * (WS->Fits->Extrema->zhi - WS->Fits->Extrema->zlo);
	WS->Clips->zhi = WS->Fits->Extrema->zhi + WS->Fits->Padding->zhi * (WS->Fits->Extrema->zhi - WS->Fits->Extrema->zlo);
}


/*  Called by GLUT library when the window has changed size.  */
/*  Uses the dimension specifications in the gWinDef for the active */
/*  window */
void gfmChangeSize(GLsizei w, GLsizei h)
{
	int CW = glutGetWindow();  /*  identifier of the current window */
	
	/*  Prevent a divide by zero */
	if(h == 0)
		h = 1;

	/*  Set Viewport to window dimensions */
    glViewport(0, 0, w, h);

	gfmClipOrtho();
	
	/*  now record all the new stuff in the appropriate windows setting box */
	gWindowsSettings[CW]->XPos = glutGet(GLUT_WINDOW_X);
	gWindowsSettings[CW]->YPos = glutGet(GLUT_WINDOW_Y);
	gWindowsSettings[CW]->Width = glutGet(GLUT_WINDOW_WIDTH);
	gWindowsSettings[CW]->Height = glutGet(GLUT_WINDOW_HEIGHT);
	
}

/*
	 a single function to call in order to set the the clipping volume
	 via orthographic projection for the current window
*/
void gfmClipOrtho(void)
{
	int CW = glutGetWindow();
	
	/*  Reset coordinate system */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	/*  Set the clipping volume */
	glOrtho(gWindowsSettings[CW]->Clips->xlo, gWindowsSettings[CW]->Clips->xhi,
			gWindowsSettings[CW]->Clips->ylo, gWindowsSettings[CW]->Clips->yhi, 
			gWindowsSettings[CW]->Clips->zlo, gWindowsSettings[CW]->Clips->zhi);
	
	/*  now go back to Modelview and set to identity */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/*
	a single function to call which looks at whether the window is an orthographic window
	or a perspective window, and then it does the appropriate projection given the
	current Clips.
*/
/* void gfmClipIt(void) */


/*
	if P is positive, expand the clipping volume in the direction implied by Direction
	by a proportion P of the width or height 
	
	if P is negative, shrink the clipping volume so that you will get back to the place
	you would have started from, had you expanded by P to get to where you currently are

*/
void gfmChangeClippedByProportion(GLfloat P, clipping_struct *Clips, int Direction)
{
	GLfloat w,h,wold,hold;
	
	
	/*  compute the current width and height */
	w = Clips->xhi - Clips->xlo;
	h = Clips->yhi - Clips->ylo;
	wold =  w / (1.0f - P);  /*  these are negative P's because these quantities */
							/*  come into play when we are shrinking, so the function  */
							/*  P will be called with a negative quantity */
	hold =  h / (1.0f - P);
	
	/*  then expand according to direction and P if P is positive */
	
	if(P > 0.0) {
		switch(Direction) {
			case(0):  /*  Left */
				Clips->xlo -= w * P;
				break;
			case(1):  /*  Right */
				Clips->xhi += w * P;
				break;
			case(2):  /*  Top */
				Clips->yhi += h * P;
				break;
			case(3):  /*  Bottom */
				Clips->ylo -= h * P;
				break;
		}
	}
	else if (P < 0.0) {
		switch(Direction) {
			case(0):  /*  Left */
				Clips->xlo += w - wold;
				break;
			case(1):  /*  Right */
				Clips->xhi -= w - wold;
				break;
			case(2):  /*  Top */
				Clips->yhi -= h - hold;
				break;
			case(3):  /*  Bottom */
				Clips->ylo += h - hold;
				break;
		}
	}
	
	gfmClipOrtho();
	
}



void gfmTranslateClippedByProportion(GLfloat P, clipping_struct *Clips, int Direction)
{
	GLfloat w,h;
	
	
	/*  compute the current width and height */
	w = Clips->xhi - Clips->xlo;
	h = Clips->yhi - Clips->ylo;
		
	/*  then translate according to direction and P */
	switch(Direction) {
		case(0):  /*  Left */
			Clips->xlo -= w * P;
			Clips->xhi -= w * P;
			break;
		case(1):  /*  Right */
			Clips->xhi += w * P;
			Clips->xlo += w * P;
			break;
		case(2):  /*  Top */
			Clips->yhi += h * P;
			Clips->ylo += h * P;
			break;
		case(3):  /*  Bottom */
			Clips->ylo -= h * P;
			Clips->yhi -= h * P;
			break;
	}

	gfmClipOrtho();
	
}


/*  zoom out or in to the size of the drag-zoom box, then turn off */
/*  drawing the drag zoom box.  Only have an effect if the drag zoom box */
/*  is currently on */
void gfmZoomToDragBox(void)
{
	int CW = glutGetWindow();
	drag_zoom_struct *DZ = gWindowsSettings[CW]->DZ;
	clipping_struct *C = gWindowsSettings[CW]->Clips;
	
	
	/*  change the clipping volume so that the low part is still on the left and the bottom */
	if(DZ->DrawDZBox == 1)  {
		if(DZ->upX < DZ->downX) {
			C->xlo = DZ->upX;
			C->xhi = DZ->downX;
		}
		else {
			C->xhi = DZ->upX;
			C->xlo = DZ->downX;
		}
		
		if(DZ->upY < DZ->downY) {
			C->ylo = DZ->upY;
			C->yhi = DZ->downY;
		}
		else {
			C->yhi = DZ->upY;
			C->ylo = DZ->downY;
		}
		
		DZ->DrawDZBox = 0;  /*  then stop drawing the box */
	}
	
	gfmClipOrtho();
	
}



/************************************************************************************
*
*       						WEIRD THINGS
*
************************************************************************************/

/*  Changes the size of the window, then changes it back.  This is */
/*  a useful "tweak" for windows that didn't get properly */
/*  initialized.  This is really obsolete, now that I have learned that */
/*  i needed to be calling glOrtho more than I was! */
void gfmResizeAllForProperInit(void)
{
	int i;
	int CurrentWindow = glutGetWindow();  /*  to get the current window */
	int CurWidth, CurHt;
	
	CYC_WINDOWS(i)
		if(gWindowsArray[i] != NULL) {  /*  if the window is defined, then set the current */
										/*  glut window to this window, execute its drawing */
										/*  commands and then post redisplay for it */
			/*  set the window to the one identified by i */
			glutSetWindow(i);
			
			CurWidth = glutGet(GLUT_WINDOW_WIDTH);
			CurHt = glutGet(GLUT_WINDOW_HEIGHT);
			
			glutReshapeWindow(CurWidth - 1, CurHt - 1);
			glutReshapeWindow(CurWidth,CurHt);
			
			
		}
	END1CYCLE
	
	/*  at the end, we return the current window to whatever was current when we */
	/*  entered the function, unless the current window was 0 in which case we */
	/*  set the current window to 1 */
	if(CurrentWindow != 0)
		glutSetWindow(CurrentWindow);
	else
		glutSetWindow(1);
}

/*  this one just resizes a single window */
void gfmResizeForProperInit(void)
{
	int CW = glutGetWindow();  /*  to get the current window */
	int CurWidth, CurHt;
	
		if(gWindowsArray[CW] != NULL) {  /*  if the current window is defined, then do it */
		
			CurWidth = glutGet(GLUT_WINDOW_WIDTH);
			CurHt = glutGet(GLUT_WINDOW_HEIGHT);
			
			glutReshapeWindow(CurWidth - 1, CurHt - 1);
			glutReshapeWindow(CurWidth,CurHt);
		}
		else 
			glutSetWindow(1);
			
}


/*  to easily call a function with a vector for colors and then specifying an */
/*  alpha */
void gfmColor3fvAlphaf(GLfloat *c, GLfloat a)
{
	glColor4f(c[0],c[1],c[2],a);
}

/************************************************************************************
*
*       SOME STANDARD ROUTINES FOR PROCESSING KEYBOARD INPUT
*
************************************************************************************/
/*  handles keyboard input.  */
void gfmProcessNormalKeys(unsigned char key, int x, int y)
{   
	int CW = glutGetWindow();   /*  Current Window Identifier; */
	int tempx=x,tempy=y;  /*  just some variables so x and y are used in the function, even though */
						  /*  they aren't really used (just so the compiler doesn't bark at us) */
	int Integer;

	
	/*  more silliness, just to keep the compiler from barking */
	tempx = tempy;
	tempy = tempx;
	
	/*  first we check to see if it is a number between 1 and 9.  If so, it */
	/*  will be opening a stored view */
	if((int)key >= (int)'1' && key <= (int)'9') {
		Integer = (int)key - 48; /*  get the integer value expressed by the numeric character */
		
		if(glutGetModifiers() == GLUT_ACTIVE_ALT)  {  /*  if the alt key is pressed simultaneously */
			if(Integer <= gNumUDViews)  { /*  if there is a view as requested */
				gfmCreateWindowsForUserDefdViews(Integer-1);
				return;
			}
			else 
				return;   /*  otherwise return and do nothing */
				
		}
		else if(glutGetModifiers() != GLUT_ACTIVE_CTRL)  {  /*  otherwise if it's just a plain number */
			if(Integer <= gNumViews)  {  /*  if there is a view as requested */
				gfmCreateWindowsForPreDefdViews(Integer-1);
				return;
			}
			else 
				return;   /*  otherwise return and do nothing */
		}
	}
	
	
	
	
	switch(key) {
		case(32):    /*  the space bar toggles go and stop */
			gGo =  (gGo+1) % 2;
			break;
		case('o'):  /*  lowercase oh */
			gGo = 0;  
			gDoASingleSweep = 1;
			break;
		case(101):  /*  if we press an "e" then we "rEset" all the averages and histograms to zero */
			gfmUserDefd_ResetAllAverages();
			gNumSweepsAfterBurnIn = 0; 
			gfmUpdateImages();
			break;
	    case(115):           /*  if we have an "s" then we "Start over" by simulating random values */
		                     /*  for everything again, and we also reset all the averages */
			
			if(glutGetModifiers() == GLUT_ACTIVE_ALT) {
				gUseSameSeeds = 1;
			}
			else {
				gUseSameSeeds = 0;
			}
			
			gfmUserDefd_InitializeChain();
			gNumSweeps = 0;
			gfmUserDefd_ResetAllAverages();
			gNumSweepsAfterBurnIn = 0;
			gfmUpdateImages();
			break;
		case(119):	/*  lowercase 'w' */
			CW = glutGetWindow();
			if(CW>1) {  /*  don't allow destruction of the Number 1 window. */
				gfmDestroyWindow(CW);
			}
			break;
		case(120):  /*  lowercase 'x' -->  Close all windows except the Number 1 window	 */
			CYC_WINDOWS(CW)
				if(CW > 1 && gWindowsArray[CW] != NULL)  {
					gfmDestroyWindow(CW);
				}
			END1CYCLE 
			break;
		case(76):  /*  uppercase 'L' -->  Toggle the legend */
			CW = glutGetWindow();
			if(gWindowsSettings[CW]->Legend->DrawLegend == 0)
				gWindowsSettings[CW]->Legend->DrawLegend = 1;
			else 
				gfmZapTheLegend(gWindowsSettings[CW]);
			break;
			
		case(66):  /*  uppercase 'B' -->  Move the legend to the bottom of the screen */
			CW = glutGetWindow();
			gfmZapTheLegend(gWindowsSettings[CW]);
			gWindowsSettings[CW]->Legend->Location = LEGEND_BOTTOM;
			gWindowsSettings[CW]->Legend->DrawLegend = 1;
			break;
		case(84):  /*  uppercase 'T' -->  Move the legend to the top of the screen */
			CW = glutGetWindow();
			gfmZapTheLegend(gWindowsSettings[CW]);
			gWindowsSettings[CW]->Legend->Location = LEGEND_TOP;
			gWindowsSettings[CW]->Legend->DrawLegend = 1;
			break;
		case(82):  /*  uppercase 'R' -->  Move the legend to the right of the screen */
			CW = glutGetWindow();
			gfmZapTheLegend(gWindowsSettings[CW]);
			gWindowsSettings[CW]->Legend->Location = LEGEND_RIGHT;
			gWindowsSettings[CW]->Legend->DrawLegend = 1;
			break;
		case(77):  /*  uppercase 'M' --> increase the number of columns by one */
			CW = glutGetWindow();
			gWindowsSettings[CW]->NumCols++;
			break;
		case(70):  /*  uppercase 'F' --> decrease the number of columns by one */
			CW = glutGetWindow();
			if(gWindowsSettings[CW]->NumCols > 1) 
				gWindowsSettings[CW]->NumCols--;
			break;
		case((int)'b'): /*  'b' cycles through legend bitmap fonts in an increasing direction */
			gWindowsSettings[CW]->Legend->FontSize = (gWindowsSettings[CW]->Legend->FontSize+1)%3;
			break;
		case((int)'+'):  /*  "+" increases displayed element */
			gWindowsSettings[CW]->DisplayedElement++;
			break;
		case(45):  /*  "-" decreases displayed element */
			gWindowsSettings[CW]->DisplayedElement--;
			break;
		case(86):  /*  capital "V" marks window for resizing to default settings */
			gWindowsSettings[CW]->Fits->FitToDefaultsNow = 1;
			break;
		case(118):  /*  lowercase "v" marks window for auto-resizing to fit everything */
			gWindowsSettings[CW]->Fits->FitToExtremaNow = 1;
			gWindowsSettings[CW]->Fits->ExtremaSet = 0;
			
			/* now, if we do alt-v then we auto-resize all the windows! */
			if(glutGetModifiers() == GLUT_ACTIVE_ALT) {
				CYC_WINDOWS(CW)
					if(CW > 1 && gWindowsArray[CW] != NULL)  {
						gWindowsSettings[CW]->Fits->FitToExtremaNow = 1;
						gWindowsSettings[CW]->Fits->ExtremaSet = 0;
					}
				END1CYCLE
			}
			break;
		case((int)';'):  /*  a semicolon */
			gfmScaleTheView(.5f,.5f);
			break;
		case((int)':'):  /*  a colon */
			gfmScaleTheView(2.0f,2.0f);
			break;
		case((int)'z'):
			gfmZoomToDragBox();
			break;
		case(92):   /*  a backslash */
			gfmResizeToFull();
			break;
			
		
	}
}



void gfmProcessSpecialKeys(int key, int x, int y)
{
	int dummyx = x, dummyy = y;  /*  just to use x and y so the compiler does */
								/*  not issue a warning */
	int CW = glutGetWindow();
	GLfloat LT=.01f,BT=.25f;  /*  Little translation and big translation */

	/*  more shenanigans to keep the compiler for making noise */
	dummyx = dummyy;
	dummyy = dummyx;
	
	switch(key)  {
		case(GLUT_KEY_LEFT):
			if(glutGetModifiers() == GLUT_ACTIVE_SHIFT) 
				gfmChangeClippedByProportion(.1f,gWindowsSettings[CW]->Clips,LEGEND_LEFT);
			else if(glutGetModifiers() == GLUT_ACTIVE_CTRL)  
				gfmChangeClippedByProportion(-.1f,gWindowsSettings[CW]->Clips,LEGEND_LEFT);
			else if(glutGetModifiers() == GLUT_ACTIVE_ALT)
				gfmTranslateClippedByProportion(BT,gWindowsSettings[CW]->Clips,LEGEND_LEFT);
			else  /*  no modifying keys */
				gfmTranslateClippedByProportion(LT,gWindowsSettings[CW]->Clips,LEGEND_LEFT);
			break;
		case(GLUT_KEY_RIGHT):
			if(glutGetModifiers() == GLUT_ACTIVE_SHIFT) 
				gfmChangeClippedByProportion(.1f,gWindowsSettings[CW]->Clips,LEGEND_RIGHT);
			else if(glutGetModifiers() == GLUT_ACTIVE_CTRL)  
				gfmChangeClippedByProportion(-.1f,gWindowsSettings[CW]->Clips,LEGEND_RIGHT);
			else if(glutGetModifiers() == GLUT_ACTIVE_ALT)
				gfmTranslateClippedByProportion(BT,gWindowsSettings[CW]->Clips,LEGEND_RIGHT);
			else  /*  no modifying keys */
				gfmTranslateClippedByProportion(LT,gWindowsSettings[CW]->Clips,LEGEND_RIGHT);
			break;
		case(GLUT_KEY_UP):
			if(glutGetModifiers() == GLUT_ACTIVE_SHIFT) 
				gfmChangeClippedByProportion(.1f,gWindowsSettings[CW]->Clips,LEGEND_TOP);
			else if(glutGetModifiers() == GLUT_ACTIVE_CTRL)  
				gfmChangeClippedByProportion(-.1f,gWindowsSettings[CW]->Clips,LEGEND_TOP);
			else if(glutGetModifiers() == GLUT_ACTIVE_ALT)
				gfmTranslateClippedByProportion(BT,gWindowsSettings[CW]->Clips,LEGEND_TOP);
			else  /*  no modifying keys */
				gfmTranslateClippedByProportion(LT,gWindowsSettings[CW]->Clips,LEGEND_TOP);
			break;
		case(GLUT_KEY_DOWN):
			if(glutGetModifiers() == GLUT_ACTIVE_SHIFT) 
				gfmChangeClippedByProportion(.1f,gWindowsSettings[CW]->Clips,LEGEND_BOTTOM);
			else if(glutGetModifiers() == GLUT_ACTIVE_CTRL)  
				gfmChangeClippedByProportion(-.1f,gWindowsSettings[CW]->Clips,LEGEND_BOTTOM);
			else if(glutGetModifiers() == GLUT_ACTIVE_ALT)
				gfmTranslateClippedByProportion(BT,gWindowsSettings[CW]->Clips,LEGEND_BOTTOM);
			else  /*  no modifying keys */
				gfmTranslateClippedByProportion(LT,gWindowsSettings[CW]->Clips,LEGEND_BOTTOM);
			break;
		default:
			return;
	}
}



/************************************************************************************
*
*       ROUTINES FOR DETERMINING THE X,Y COORDINATES OF MOUSE CLICKS
*
************************************************************************************/
/*
	Return the real X-coordinate of the current window given 
	a position in terms of pixels from the origin
*/ 
GLfloat gfmReturnXCoord(int XPix)
{
	int CW;  /*  current window */
	GLsizei Width;
	GLfloat xlo, xhi;
	
	/*  get the current window */
	CW = glutGetWindow();
	
	/*  determine xlo and xhi */
	xlo = gWindowsSettings[CW]->Clips->xlo;
	xhi = gWindowsSettings[CW]->Clips->xhi;
	
	/*  determine the width in pixels of the window */
	Width = glutGet(GLUT_WINDOW_WIDTH);
	
	return(xlo + (GLfloat)XPix * (xhi - xlo) / (GLfloat)Width);
	
}


/*
	Return the real Y-coordinate of the current window given 
	a position in terms of pixels from the origin
*/ 
GLfloat gfmReturnYCoord(int YPix)
{
	int CW;  /*  current window */
	GLsizei Height;
	GLfloat ylo, yhi;
	
	/*  get the current window */
	CW = glutGetWindow();
	
	/*  determine xlo and xhi */
	ylo = gWindowsSettings[CW]->Clips->ylo;
	yhi = gWindowsSettings[CW]->Clips->yhi;
	
	/*  determine the width in pixels of the window */
	Height = glutGet(GLUT_WINDOW_HEIGHT);
	
	return(yhi - (GLfloat)YPix * (yhi - ylo) / (GLfloat)Height);
	
}

/*  This is the function you actually attach to the mouse motion or */
/*  passive motion funcs  */
/*  which will change the X and Y coords in the window settings */
void gfmMouseLocation(int x, int y)
{
	int CW = glutGetWindow();
	
	gWindowsSettings[CW]->XCoord = gfmReturnXCoord(x);
	gWindowsSettings[CW]->YCoord = gfmReturnYCoord(y);
	
	gWindowsSettings[CW]->DZ->upX = gWindowsSettings[CW]->XCoord;
	gWindowsSettings[CW]->DZ->upY = gWindowsSettings[CW]->YCoord;
	
	
}


void gfmStandardMouseClick(int button, int state, int x, int y)
{
	int CW = glutGetWindow();
	
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN &&  glutGetModifiers() != GLUT_ACTIVE_SHIFT)  {
		gWindowsSettings[CW]->XCoord = gfmReturnXCoord(x);
		gWindowsSettings[CW]->YCoord = gfmReturnYCoord(y);
		
		/*  set the DZ struct to innocuous values on a standard (shift-less) mouse click */
		gfmInitDragZoom(gWindowsSettings[CW]->DZ,0,0.0f,0.0f);
	}

	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && glutGetModifiers() == GLUT_ACTIVE_SHIFT
			&& gWindowsSettings[CW]->DZ->DrawDZBox == 0) {  /*  for the first time set them all to zero */
		gfmInitDragZoom(gWindowsSettings[CW]->DZ,1,gfmReturnXCoord(x),gfmReturnYCoord(y));
	}
	
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && glutGetModifiers() == GLUT_ACTIVE_SHIFT
			&& gWindowsSettings[CW]->DZ->DrawDZBox == 1) {  /*  if draw box is already set to one then */
															/*  just change the upX and upY */
		gWindowsSettings[CW]->DZ->upX = gfmReturnXCoord(x);
		gWindowsSettings[CW]->DZ->upY = gfmReturnYCoord(y);
	}
}





/*********************************
*
*      	DEFINING THE MENUS AND SUCH
*
************************************************************************************/
/*  Create the menu and submenu structure */
void gfmDefineGFMMenus(void)
{
	int i;
	
	/*  create the main menu, which will be composed entirely of sub-menus, */
	/*  so it can take NULL as its function */
	gMainMenu = glutCreateMenu(NULL);
	
	
	/*  make the chain control sub-menu */
	gChainControlSubMenu = glutCreateMenu(gfmKeysMirroredInMenuFunc);
	glutAddMenuEntry("Stop Chain Execution          Space Bar", 32);
	glutAddMenuEntry("UnStop Chain Execution          Space Bar", 32);
	glutAddMenuEntry("Start Chain From New Random State          s", 115);
	glutAddMenuEntry("Reset Monte Carlo Averages          e", 101);
	
	gCloseWindowsSubMenu = glutCreateMenu(gfmKeysMirroredInMenuFunc);
	glutAddMenuEntry("Close The Current Window          w", 119);
	glutAddMenuEntry("Close All Non-\"Info\" Windows          x", 120);
	
	/*  the following should be changed into a function that autmatically */
	/* takes care of this based on the UserDef_WindowDefinitions function */
	/*  for compatibility with old versions we now just do the next few lines */
	/*  when gCreateNewWindowsSubMenu == 0;  Otherwise we rely entirely on */
	/*  setting up the menu in theUserDef_WindowDefinitions function */
	if(gCreateNewWindowsSubMenu == 0) {
		gCreateNewWindowsSubMenu = glutCreateMenu(gfmCreateWindowByIndex);
		for(i=0;i<gNumWinDefs;i++)  {
			glutAddMenuEntry(gWinDefs[i]->Title, i);
		}
	}
	
	/*  take care of the predefined views submenus menu */
	gPredefinedViewsSubMenu = glutCreateMenu(gfmCreateWindowsForPreDefdViews);
	i=gPredefinedViewsSubMenu;
	gUserDefdViewsSubMenu = glutCreateMenu(gfmCreateWindowsForUserDefdViews);
	gViewsSubMenu = glutCreateMenu(gfmRecordNewView);
	glutAddSubMenu("Restore Predefined View",gPredefinedViewsSubMenu);
	glutAddSubMenu("Restore User-Defined View",gUserDefdViewsSubMenu);
	glutAddMenuEntry("Save the Current View",0);
	
	/*  do the submenus for picking the color scheme of a window */
	gUserDefdColorSubMenu = glutCreateMenu(gfmSetToUserDefdColorScheme);
	gPreDefColorSubMenu = glutCreateMenu(gfmSetToPreDefdColorScheme);
	
	/*  then set the main color scheme window */
	gColorSchemeMenu = glutCreateMenu(gfmInitiateColorPicking);
	glutAddSubMenu("Select Pre-Defined Scheme",gPreDefColorSubMenu);
	glutAddSubMenu("Select User-Defined Scheme",gUserDefdColorSubMenu);
	glutAddMenuEntry("Create New Color Scheme", COLOR_PICKER_WINDOW_ID);
	
	/*  set up the legend control sub-menu: */
	gLegendControlSubMenu = glutCreateMenu(gfmKeysMirroredInMenuFunc);
	glutAddMenuEntry("Hide or Show Legend          L", 76);
	glutAddMenuEntry("Put Legend On Right          R", 82);
	glutAddMenuEntry("Put Legend On Top            T", 84);
	glutAddMenuEntry("Put Legend On Bottom         B", 66);
	
	
	/*  set up the auto-control viewable area submenu */
	gAutoFitSubMenu = glutCreateMenu(gfmKeysMirroredInMenuFunc);
	glutAddMenuEntry("Set Viewable Area To Default      V", 86);  /*  capital V */
	glutAddMenuEntry("Auto Size Window To Fit Content     v", 118);  /*  lowercase v */
	
	/*  set up the bring windows to front submenu */
	gBringToFrontSubMenu = glutCreateMenu(gfmBringWindowToFront);
	
	/*  set up the quit sub menu */
	gQuitSubMenu = glutCreateMenu(exit);
	glutAddMenuEntry("Yes.  I Really Want To Quit",1);
	
	
	
	
	
	/*  set up the sub-menus in the main menu */
	glutSetMenu(gMainMenu);
	glutAddSubMenu("Control Markov Chain",gChainControlSubMenu);
	glutAddSubMenu("Bring Window To Front",gBringToFrontSubMenu);
	glutAddSubMenu("Open New Windows",gCreateNewWindowsSubMenu);
	glutAddSubMenu("Close Windows",gCloseWindowsSubMenu);
	glutAddSubMenu("Handle Views",gViewsSubMenu);
	glutAddSubMenu("Adjust Viewable Area",gAutoFitSubMenu);
	glutAddSubMenu("View Legend",gLegendControlSubMenu);
	glutAddSubMenu("Change Color Scheme", gColorSchemeMenu);	
	
	glutAddSubMenu("***QUIT THIS PROGRAM***",gQuitSubMenu);
	
	
	
	
	
	
} 


/*  This is a good callback for menu functions that want to mimic ascii keystroke options */
void gfmKeysMirroredInMenuFunc(int Num)
{
	unsigned char key = (unsigned char)Num;
	gfmProcessNormalKeys(key,0,0);
}




/************************************************************************************
*
*            COLOR SCHEME MEMORY ALLOCATION
*
************************************************************************************/


/*  
allocates to a color scheme with N series colors.  If N is zero, it doesn't
really seem to make a whole lot of difference except that temp N gets set to 
zero.  A bit silly.
*/
ColorScheme3f *gfmAllocColorScheme(int N)
{
	int i,j;
	
	ColorScheme3f *temp;
	
	temp = (ColorScheme3f *)ECA_MALLOC(sizeof(ColorScheme3f));
	
	temp->N = N;
	
	temp->ID = -1;  /*  set the default ID to -1. */
	
	sprintf(temp->Title, "UnnamedColorScheme");
	

	temp->Series = (GLfloat **)ECA_CALLOC(MAX_SERIES_COLORS,sizeof(GLfloat*));
	for(i=0;i<MAX_SERIES_COLORS;i++) 
		temp->Series[i] = (GLfloat *)ECA_CALLOC(3,sizeof(GLfloat));


	
	/*  now set default values for the background. etc.  Make it all white */
	for(i=0;i<3;i++)  {
		temp->Background[i] = 1.0f;
		temp->Axes[i] = 1.0f;
		temp->Text[i] = 1.0f;
		temp->LegendBoundary[i] = 1.0f;
		temp->LegendBackground[i] = 1.0f;
		for(j=0;j<MAX_SERIES_COLORS;j++)  
			temp->Series[j][i] = 1.0f;
	}
	return(temp);
}

/*
	returns memory allocated to a color scheme to the heap
*/
void gfmFreeColorScheme(ColorScheme3f *C)
{
	int i;
	
	for(i=0;i<MAX_SERIES_COLORS;i++)  {
		free(C->Series[i]);
	}
	free(C->Series);
		
	free(C);
	
}




/************************************************************************************
*
*                      COLOR PICKER WINDOW FUNCTIONS
*
************************************************************************************/

/*
	Defines the color picker window so it may be initialized by GLUT. 

*/
void gfmDefineColorPickerWindow(void)
{		
	/*  A Color Picker Window */
	gColorPickerWinDef = gfmAllocWindowStruct();
	sprintf(gColorPickerWinDef->Title,"Color Picker");
	gColorPickerWinDef->WindowID = COLOR_PICKER_WINDOW_ID;
	gColorPickerWinDef->WT = STATIC_WIN;
	gColorPickerWinDef->DrawFunc = gfmDrawColorPicker;
	gColorPickerWinDef->KeyboardFunc = gfmColorPickerKeyboardFunc;
    gColorPickerWinDef->NumColorKeys = &gCSPHiN;  /*  needs to know how many color keys */
    											  /*  from the start or else it crashes */
    											  /*  on Linux */


	/*  and now we change the default clipping volume */
	gColorPickerWinSettings = gfmAllocWindowSettings();
	gColorPickerWinSettings->Clips->yhi = 1.6f;
	gColorPickerWinSettings->Clips->ylo = -0.05f;
	gColorPickerWinSettings->XPos = gWB + gSW/4;
	gColorPickerWinSettings->YPos = gWTB + gHH;
	gColorPickerWinSettings->Width = gSW / 2;
	gColorPickerWinSettings->Height = (int)((double)gSH / 1.3f);

	
}

/*
	Define the 216 colors in the array g216Colors3f
*/
void Define216Colors(void)
{

	int i;
	GLfloat r,g,b;
	
	/*  allocate memory to the 216 standard colors */
	g216Colors3f = (GLfloat **)ECA_CALLOC(216,sizeof(GLfloat *));
	for(i=0;i<216;i++)  {
		g216Colors3f[i] = (GLfloat *)ECA_CALLOC(3,sizeof(GLfloat));
	}
	
	i=0;
	for(r=0.0f;r<=1.0f;r+=.2f) 
		for(g=0.0f;g<=1.0f;g+=.2f)
			for(b=0.0f;b<=1.0f;b+=.2f)  {
				g216Colors3f[i][0] = r; 
				g216Colors3f[i][1] = g; 
				g216Colors3f[i][2] = b;
				i++;
			}

}

/*
	OpenGL code to draw the color picker window
*/
void gfmDrawColorPicker(void)
{
	int i,cols = 6;
	int N = 216;
	int NumInCol;
	GLfloat w,h;
	GLfloat xs = .01f, ys = .15f;  /*  increase these to increase the black space between colors */
	int SelectedBar;
	char Str[200];
	int CW = glutGetWindow();
	GLfloat x,y;
	char NumString[10];
	GLfloat *BG;
	GLfloat *Text;
	GLfloat *Axes;
	GLfloat *LB;
	GLfloat *LGB;
	GLfloat **Series;
	LegendSettings *Leg;
	int CSN;  
	GLfloat r,g,b;
	GLfloat Width;
	ColorScheme3f *Temp;
	
	
	/*  if we are all done, we want to print a simple message and get out of this */
	/*  function */
	if(gCSPComplete == 1)  {
		
		Temp = gUserDefdColorSchemes[gNumUserDefdColorSchemes-1];
	
		glClearColor(Temp->Background[0],Temp->Background[1],
				Temp->Background[2],1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		sprintf(Str,"CONGRATULATIONS!! YOU ARE DONE.");
		glColor3f(Temp->Text[0],Temp->Text[1],Temp->Text[2]);
		gfmRenderBitmapString(-.1f,.9f,GLUT_BITMAP_HELVETICA_18, Str);
		
		sprintf(Str,"That scheme is now available as %d",gNumUserDefdColorSchemes);
		gfmRenderBitmapString(-.1f,.75f,GLUT_BITMAP_HELVETICA_18, Str);

		gfmRenderBitmapString(-.1f,.6f,GLUT_BITMAP_HELVETICA_18, 
			gUserDefdColorSchemes[gNumUserDefdColorSchemes]->Title);
		
		return;
	}
	
	/*  to make for less typing: */
	BG = gWindowsSettings[CW]->ColorScheme->Background;
	Text = gWindowsSettings[CW]->ColorScheme->Text;
	Axes = gWindowsSettings[CW]->ColorScheme->Axes;
	LB = gWindowsSettings[CW]->ColorScheme->LegendBoundary;
	LGB = gWindowsSettings[CW]->ColorScheme->LegendBackground;
	Series = gWindowsSettings[CW]->ColorScheme->Series;
	Leg = gWindowsSettings[CW]->Legend;

	
	
	
	/*  set the window settings number of colors to the global highest number so far */
	gWindowsSettings[CW]->ColorScheme->N = gCSPHiN;
	CSN = gWindowsSettings[CW]->ColorScheme->N;
	
	/*  we also have to set the number of ColorKeyLabels so the legend comes out right */
	gWindowsArray[CW]->NumColorKeys[0] = CSN;
	
	
	
	
	gCSPr += .006;
	gCSPg += .01;
	gCSPb += .02;
	
	
	
	/*  define the instruction text colors */
	if(gCSPHiState < 2) {
		r = fabs(sin(gCSPr));
		g = fabs(sin(gCSPg));
		b = fabs(sin(gCSPb));
	}
	else {
		r = Text[0];
		g = Text[1];
		b = Text[2];
	}
	 
	glClearColor(BG[0],BG[1],BG[2],1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	gfmSetParsForBarGraphs(N,cols, xs, ys,
						&NumInCol, &w, &h);
						
	/*  first draw a big black rectangle underneath everything */
	glColor3f(0.0f,0.0f,0.0f);
	glRectf(-xs * w,1.0f,1.0f + xs * w ,0.0f - ys * h);
	
	for(i=0;i<N;i++)  {
		x = gfmBarGraphXCoord(NumInCol, w, xs, i);
		y = gfmBarGraphYCoord(NumInCol, h, ys, i);
		
		 gfmDrawRectLeaf6f( x, y, w, h, 0.0f,1.0f, g216Colors3f[i]);
	}
	
	/*  and here we include the mouse location element selection part: */
	SelectedBar = gfmBarGraphIdxFromXY(NumInCol,N,w,h,xs,ys,
						gWindowsSettings[CW]->XCoord,
						gWindowsSettings[CW]->YCoord);
	
	
	
	
	
	
	
	
	if(gCSPState == 0) {   /*  reset background color */

		sprintf(Str,"Click on a color from the palette to be the BACKGROUND COLOR");
		glColor3f(r,g,b);
		gfmRenderBitmapString(-.1f,1.1f,GLUT_BITMAP_HELVETICA_12, Str);
		
		sprintf(Str,"Press \"c\" to confirm your choice");
		glColor3f(r,g,b);
		gfmRenderBitmapString(-.1f,1.02f,GLUT_BITMAP_HELVETICA_12, Str);
		
		if(SelectedBar >= 0) {
			BG[0] = g216Colors3f[SelectedBar][0];
			BG[1] = g216Colors3f[SelectedBar][1];
			BG[2] = g216Colors3f[SelectedBar][2];
		}	
	}		
	
	if(gCSPState == 1)  {  /*  Time to choose a Text Color */
		sprintf(Str,"Choose a TEXT COLOR from the palette");
		glColor3f(r,g,b);
		gfmRenderBitmapString(-.1f,1.1f,GLUT_BITMAP_HELVETICA_12, Str);
		
		sprintf(Str,"Press \"c\" to confirm OR \"b\" to change the previous choice");
		glColor3f(r,g,b);
		gfmRenderBitmapString(-.1f,1.02f,GLUT_BITMAP_HELVETICA_12, Str);
		
		if(SelectedBar >= 0) {
			Text[0] = g216Colors3f[SelectedBar][0];
			Text[1] = g216Colors3f[SelectedBar][1];
			Text[2] = g216Colors3f[SelectedBar][2];
		}	
		
		sprintf(Str,"TEXT!");
		glColor3f(Text[0],Text[1],Text[2]);
		gfmRenderBitmapString(.87f,1.05f,GLUT_BITMAP_TIMES_ROMAN_24, Str);
	}
	
	
	if(gCSPState == 2)  {  /*  Time to choose a background color for our legend */
	
		Leg->Location = LEGEND_RIGHT;
		Leg->DrawLegend = 1;
		
		sprintf(Str,"Choose a color for the LEGEND BACKGROUND");
		glColor3f(Text[0],Text[1],Text[2]);
		gfmRenderBitmapString(-.1f,1.1f,GLUT_BITMAP_HELVETICA_12, Str);
		
		sprintf(Str,"Press \"c\" to confirm OR \"b\" to change the previous choice");
		glColor3f(Text[0],Text[1],Text[2]);
		gfmRenderBitmapString(-.1f,1.02f,GLUT_BITMAP_HELVETICA_12, Str);
		
		if(SelectedBar >= 0) {
			LGB[0] = g216Colors3f[SelectedBar][0];
			LGB[1] = g216Colors3f[SelectedBar][1];
			LGB[2] = g216Colors3f[SelectedBar][2];
		}	
	}
	
	if(gCSPState == 3)  {  /*  Time to choose a border color for our legend */
		
		
		sprintf(Str,"Choose a color for the LEGEND BORDER");
		glColor3fv(Text);
		gfmRenderBitmapString(-.1f,1.1f,GLUT_BITMAP_HELVETICA_12, Str);
		
		sprintf(Str,"Press \"c\" to confirm OR \"b\" to change the previous choice");
		glColor3f(Text[0],Text[1],Text[2]);
		gfmRenderBitmapString(-.1f,1.02f,GLUT_BITMAP_HELVETICA_12, Str);
		
		if(SelectedBar >= 0) {
			LB[0] = g216Colors3f[SelectedBar][0];
			LB[1] = g216Colors3f[SelectedBar][1];
			LB[2] = g216Colors3f[SelectedBar][2];
		}	
	}
	
	if(gCSPHiState >= 4)  {  /*  Time to choose a color for axes */
	
		if(gCSPState == 4)  {
			sprintf(Str,"Choose a color for THE  COORDINATE AXES");
			glColor3f(Text[0],Text[1],Text[2]);
			gfmRenderBitmapString(-.1f,1.1f,GLUT_BITMAP_HELVETICA_12, Str);
			
			sprintf(Str,"Press \"c\" to confirm OR \"b\" to change the previous choice");
			glColor3f(Text[0],Text[1],Text[2]);
			gfmRenderBitmapString(-.1f,1.02f,GLUT_BITMAP_HELVETICA_12, Str);
			
			if(SelectedBar >= 0) {
				Axes[0] = g216Colors3f[SelectedBar][0];
				Axes[1] = g216Colors3f[SelectedBar][1];
				Axes[2] = g216Colors3f[SelectedBar][2];
			}	
		}
		
		glBegin(GL_LINES);
			glColor3fv(Axes);
			
			/*  horizontal line   this should all be changed to Clips variables  (or maybe not!) */
			glVertex2f(-.2f,1.2f);
			glVertex2f(1.2f,1.2f);
			
			/*  vertical line */
			glVertex2f(.5f,1.2f);
			glVertex2f(.5f,1.58f);
			
		glEnd();
	}
	
	
	if(gCSPHiState >= 5)  {   /*  defining the Series colors here */
	
		if(gCSPState == 5)  {

			sprintf(Str,"Choose a color for element %d IN THE SERIES",gCSPCurrentN+1);
			glColor3f(Text[0],Text[1],Text[2]);
			gfmRenderBitmapString(-.1f,1.1f,GLUT_BITMAP_HELVETICA_12, Str);
			
			sprintf(Str,"Press \"c\" to confirm. \"b\" to go back. \"x\" to save scheme and exit");
			glColor3f(Text[0],Text[1],Text[2]);
			gfmRenderBitmapString(-.1f,1.02f,GLUT_BITMAP_HELVETICA_12, Str);
			
			
			if(SelectedBar >= 0) {
				Series[gCSPCurrentN][0] = g216Colors3f[SelectedBar][0];
				Series[gCSPCurrentN][1] = g216Colors3f[SelectedBar][1];
				Series[gCSPCurrentN][2] = g216Colors3f[SelectedBar][2];
			}	
			
		}
		
		
	
		/*  now, print the rectangles */
		Width = 1.0f/(GLfloat)(CSN+1);
		for(i=0;i<=CSN;i++)  {
			glColor3fv(Series[i]);
			
			/*  draw the rectangle piece */
			glRectf(Width * i,1.50f,Width * (i+1),1.40f);
			
			/*  draw the number */
			sprintf(NumString,"%d",i+1);
			gfmRenderBitmapString(Width * i, 1.52f,GLUT_BITMAP_HELVETICA_18,NumString);
			
			/*  draw a line below the center of the rectangle */
			glBegin(GL_LINES);
				glVertex2f( (Width * i + Width * (i+1))/2.0f , 1.40f);
				glVertex2f( (Width * i + Width * (i+1))/2.0f , 1.20f);
			glEnd();
		}
	}
	
	

	
}

/*
	Keyboard callback function for the color picker window
*/
void gfmColorPickerKeyboardFunc(unsigned char key, int x, int y)
{   
	int tempx=x,tempy=y;  /*  just some variables so x and y are used in the function, even though */
						  /*  they aren't really used (just so the compiler doesn't bark at us) */
	
	int CW = glutGetWindow();
	
	tempx = tempy;
	tempy = tempx;
	
	
	switch(key) {
		case(99):    /*  lowercase 'c' locks in the color choice */
			if(gCSPState < 5)
				gCSPState++;
			else
				gCSPCurrentN++;
				 
			if(gCSPState > gCSPHiState)
				gCSPHiState = gCSPState;
			if(gCSPCurrentN >= MAX_SERIES_COLORS)
				gCSPCurrentN = MAX_SERIES_COLORS - 1;
			if(gCSPCurrentN > gCSPHiN)
				gCSPHiN = gCSPCurrentN;
			/*  and, this is a sort of klugie way to do it, but when we go back, we have */
			/*  to make sure that there is no selected bar, so we set the XCoord and the */
			/*  YCoord to be way off the charts */
			gWindowsSettings[CW]->XCoord = -9999.99f;
			gWindowsSettings[CW]->YCoord = -9999.99f;
			break;	
		case(98):    /*  lowercase 'b' goes back to the previous thing changed */
			if(gCSPState>0 && gCSPCurrentN == 0)
				gCSPState--;
			if(gCSPCurrentN > 0)
				gCSPCurrentN--;
				
			/*  and, this is a sort of klugie way to do it, but when we go back, we have */
			/*  to make sure that there is no selected bar, so we set the XCoord and the */
			/*  YCoord to be way off the charts */
			gWindowsSettings[CW]->XCoord = -9999.99f;
			gWindowsSettings[CW]->YCoord = -9999.99f;
			break;		
		case(120):  /*  lowercase 'x' -->  only if at least one series color has been picked */
					/*  this should point the global color scheme pointer to the color scheme */
					/*  that has just been made.  Set the color scheme pointer associated with */
					/*  the picker to NULL, add a menu entry to the self-made color scheme choices menu */
					/*  and finally write out all the self-made color schemes to a file */
			/*  only do this if we haven't already "completed" the process */
			if(gCSPComplete == 0) {
				
				gfmHandleStringInput("Well done.  Now name your scheme",
						"Type the name in.  Spaces will become underscores", 
						"Hit return when done.", gWindowsSettings[CW]->ColorScheme->Title,
						gfmCallAfterColorSchemeStringInput );
			}
			break;
		case(119):	/*  lowercase 'w' */
			if(CW>1) {  /*  don't allow destruction of the Number 1 window. */
				gfmDestroyWindow(CW);
			}
			break;
	}
}




/*
	Callback for the color picker menu.  If you select the "Define New Color Scheme Option"
	then this should be called with state = COLOR_PICKER_WINDOW_ID.  Sets all the global variables
	associated with color picking, and then creates the color picker window.    
*/
void gfmInitiateColorPicking(int state)
{

	if(state == COLOR_PICKER_WINDOW_ID)  {
		/*  set all the global variables */
		gCSPState = 0;
		gCSPHiState = 0;
		gCSPExit = 0;
		gCSPComplete = 0;
		gCSPr = 0.0f;
		gCSPb = 0.0f;
		gCSPg = 0.0f;
		
		gCSPHiN = 0;
		gCSPCurrentN = 0;
		gCSPSpaceForN = 0;
		
		gfmCreateWindow(gColorPickerWinDef,gColorPickerWinSettings);
		
		/*  if the above function was successful then the current window is the colorpicker window, so */
		gColorPickerWindowNumber = glutGetWindow();
		
		/*  and we allocate space for a fresh color scheme that is set to all whites */
		gWindowsSettings[gColorPickerWindowNumber]->ColorScheme = gfmAllocColorScheme(0);
	}

}



/*  once we are done with the string input for naming a color scheme, we should have told */
/*  gfmHandleStringInput to call this function */
void gfmCallAfterColorSchemeStringInput(void) 
{
	
	gUserDefdColorSchemes[gNumUserDefdColorSchemes] = 
		gWindowsSettings[gColorPickerWindowNumber]->ColorScheme;
		
	gWindowsSettings[gColorPickerWindowNumber]->ColorScheme = NULL;

	gUserDefdColorSchemes[gNumUserDefdColorSchemes]->ID = gNumUserDefdColorSchemes;

	glutSetMenu(gUserDefdColorSubMenu);

	glutAddMenuEntry(gUserDefdColorSchemes[gNumUserDefdColorSchemes]->Title,gNumUserDefdColorSchemes);

	/*  increment the number of color schemes */
	gNumUserDefdColorSchemes++;

	/*  make it known that we are complete! */
	gCSPComplete = 1;

	/*  and finally, erase the color scheme window, too: */
	if(gWindowRequestingStringInput>1) {  /*  don't allow destruction of the Number 1 window. */
		gfmDestroyWindow(gWindowRequestingStringInput);
	}	
	
	/*  at the end of that, make the Number 1 window the current window */
	glutSetWindow(1);

	/*  and then write all the user-defined schemes to a file */
	gfmWriteColorSchemesToFile(gUserDefdColorSchemes,
				gNumUserDefdColorSchemes, gUserDefdColorSchemesFileName );
				
	/*  write it as source too   */
	gfmWriteColorSchemesToFileAsSource(gUserDefdColorSchemes,
				gNumUserDefdColorSchemes, "UserDefdColorSchemesSource.c" );
	
}



/************************************************************************************
*
*                 FUNCTIONS FOR SETTING COLOR SCHEMES IN WINDOWS
*				- used mainly as functions that menu options call -
************************************************************************************/

void gfmSetToUserDefdColorScheme(int state)
{
	int CW = glutGetWindow();
	
	/*  catch any errors and do nothing: */
	if(state > gNumUserDefdColorSchemes - 1)
		return;
		
	else
		gWindowsSettings[CW]->ColorScheme = gUserDefdColorSchemes[state];

}

void gfmSetToPreDefdColorScheme(int state)
{
	int CW = glutGetWindow();
	
	/*  catch any errors and do nothing: */
	if(state > gNumPreDefdColorSchemes - 1)
		return;
		
	else
		gWindowsSettings[CW]->ColorScheme = gPreDefdColorSchemes[state];

}





/************************************************************************************
*
*             FUNCTIONS FOR WRITING AND READING COLOR SCHEMES TO FILE
*
************************************************************************************/


/*
	write a color scheme to a file
*/
void gfmWriteColorScheme3f(ColorScheme3f *C, FILE *F)
{
	int i;
	
	fprintf(F,"\n\n%s ID= %d N= %d",C->Title, C->ID, C->N);
	fprintf(F,"\n\tBackground= %f %f %f",C->Background[0],C->Background[1],C->Background[2]);
	fprintf(F,"\n\tText= %f %f %f",C->Text[0],C->Text[1],C->Text[2]);
	fprintf(F,"\n\tAxes= %f %f %f",C->Axes[0],C->Axes[1],C->Axes[2]);
	fprintf(F,"\n\tLegendBackground= %f %f %f",C->LegendBackground[0],C->LegendBackground[1],C->LegendBackground[2]);
	fprintf(F,"\n\tLegendBoundary= %f %f %f",C->LegendBoundary[0],
			C->LegendBoundary[1],C->LegendBoundary[2]);
			
	fprintf(F,"\n\tSeries=");
	for(i=0;i<C->N;i++)  {
		fprintf(F,"\n\t\t%f %f %f",C->Series[i][0],C->Series[i][1],C->Series[i][2]);
	}
}


/*
	write a color scheme to a file in compiler readable format.
	i.e. this writes out the code that one would write if defining
	the color scheme in a fucntion that can be compiled so the
	info can be kept within the executable.  Assumes that in the function
	the color scheme it wants to deal with has a pointer
	to it called CS.
*/
void gfmWriteColorScheme3fAsSource(ColorScheme3f *C, FILE *F)
{
	int i;
	
	
	
	fprintf(F,"\n\tsprintf(C->Title,\"%s\");",C->Title);
	fprintf(F,"\n\tC->ID = %d;",C->ID);
	fprintf(F,"\n\tC->N = %d;", C->N);
	fprintf(F,"\n\tC->Background[0] = %ff;\n\tC->Background[1] = %ff;\n\tC->Background[2] = %ff;",C->Background[0],C->Background[1],C->Background[2]);
	fprintf(F,"\n\tC->Text[0] = %ff;\n\tC->Text[1] = %ff;\n\tC->Text[2]=%ff;",C->Text[0],C->Text[1],C->Text[2]);
	fprintf(F,"\n\tC->Axes[0] = %ff;\n\tC->Axes[1] = %ff;\n\tC->Axes[2] = %ff;",C->Axes[0],C->Axes[1],C->Axes[2]);
	fprintf(F,"\n\tC->LegendBackground[0] = %ff;\n\tC->LegendBackground[1] = %ff;\n\tC->LegendBackground[2] = %ff;",C->LegendBackground[0],C->LegendBackground[1],C->LegendBackground[2]);
	fprintf(F,"\n\tC->LegendBoundary[0] = %ff;\n\tC->LegendBoundary[1] = %ff;\n\tC->LegendBoundary[2] = %ff;",C->LegendBoundary[0],
			C->LegendBoundary[1],C->LegendBoundary[2]);
			
	for(i=0;i<C->N;i++)  {
		fprintf(F,"\n\tC->Series[%d][0] = %ff; C->Series[%d][1] = %ff; C->Series[%d][2] = %ff;",i,C->Series[i][0],i,C->Series[i][1],i,C->Series[i][2]);
	}
}




/*
	read a color scheme from a file stream
*/
void gfmReadColorScheme3f(ColorScheme3f *C, FILE *F)
{
	int i;
	
	fscanf(F,"\n\n%s ID= %d N= %d",C->Title, &(C->ID), &(C->N));
	fscanf(F,"\n\tBackground= %f %f %f",&(C->Background[0]),&(C->Background[1]),
					&(C->Background[2]));
	fscanf(F,"\n\tText= %f %f %f",&(C->Text[0]),&(C->Text[1]),&(C->Text[2]));
	fscanf(F,"\n\tAxes= %f %f %f",&(C->Axes[0]),&(C->Axes[1]),&(C->Axes[2]));
	fscanf(F,"\n\tLegendBackground= %f %f %f",&(C->LegendBackground[0]),&(C->LegendBackground[1]),&(C->LegendBackground[2]));
	fscanf(F,"\n\tLegendBoundary= %f %f %f",&(C->LegendBoundary[0]),
			&(C->LegendBoundary[1]),&(C->LegendBoundary[2]));
			
	fscanf(F,"\n\tSeries=");
	for(i=0;i<C->N;i++)  {
		fscanf(F,"\n\t\t%f %f %f",&(C->Series[i][0]),&(C->Series[i][1]),&(C->Series[i][2]));
	}
}
/*
	write an array of N color schemes to a file names FileName
*/
void gfmWriteColorSchemesToFile(ColorScheme3f **C, int N, char *FileName )
{
	int j;
	
	FILE *F = fopen(FileName, "w");
	
	fprintf(F,"%d", N);
	
	for(j=0;j<N;j++)  {
		gfmWriteColorScheme3f(C[j],F);
	}
	fclose(F);
}

/*
	write an array of N color schemes to a file named FileName which will
	have the format of a source file
*/
void gfmWriteColorSchemesToFileAsSource(ColorScheme3f **C, int N, char *FileName )
{
	int j;
	
	FILE *F = fopen(FileName, "w");
	
	fprintf(F,"/*  function generated by GFMCMC\n\n");
	fprintf(F,"#include <stdio.h>\n");
	fprintf(F,"#include \"GFMCMC.h\"\n");
	fprintf(F,"#include \"ECA_MemAlloc.h\"");
	
	fprintf(F,"\n\n\nvoid gfmColorSchemePredefiningFunc(ColorScheme3f *CS[MAX_CS+1],int *NumC, int Menu )\n{");
	fprintf(F,"\n\tint N = %d, c = 0, i=0;",N);
	fprintf(F,"\n\tColorScheme3f *C = *CS;");
	
	fprintf(F,"\n\n*NumC = N;\n");
	
	fprintf(F,"\n\nglutSetMenu(Menu);\n");
	
	for(j=0;j<N;j++)  {
		/*  now for each of these we have to allocate some memory */
		fprintf(F,"\n\tCS[i] = gfmAllocColorScheme(0);");
		fprintf(F,"\n\tC = CS[i];");
		gfmWriteColorScheme3fAsSource(C[j],F);
		
		fprintf(F,"\n\nglutAddMenuEntry(C->Title,i);");
		
		fprintf(F,"\n\ni++;\n");
	}
	
	fprintf(F,"}");
	fclose(F);
}




/*

	Allocates space to color schemes, and fills them with information from the file.
	NumC should be the global variable associated
	with the array if you're passing a global array like gUserDefdColorSchemes.
	
	Also adds the names and their numbers to the menu specified.

*/
void gfmReadColorSchemesFromFile(ColorScheme3f *Temp[MAX_CS + 1],const char *FileName,
	int *NumC, int Menu)
{
	int i;
	FILE *F;
	char TempStr[500];
	
	glutSetMenu(Menu);
	
	if( (F = fopen(FileName,"r")) == 0) {  /*  if it can't open files for the color scheme */
		sprintf(TempStr,"\nWarning!  Cannot open color scheme file %s...",FileName);
		gfmAppendToFile(gERROR_FILE,TempStr);
		return;
	}
	
	fscanf(F,"%d", NumC);
	
	/* Temp = (ColorScheme3f **)ECA_CALLOC(NumC,sizeof(ColorScheme3f *)); */
	
	for(i=0;i<*NumC;i++)  {
		Temp[i] = gfmAllocColorScheme(0);  
		gfmReadColorScheme3f(Temp[i],F);
		
		glutAddMenuEntry(Temp[i]->Title,i);
	}
}


/************************************************************************************
*
*          FUNCTION FOR REPRESENTING REALS IN [0,1] AS A COLOR
*
************************************************************************************/

/*  
	Takes a real number between 0 and 1 inclusive and maps it to
	a color in the RGB scheme.  Large values are Red and low values
	are violet, with intervening values being linearly interpolated
	in a rainbow, sort of.  
*/
void gfmMap0_1ToColor(GLfloat *C, double x)
{
	GLfloat y = 1.0f - (GLfloat)x;
	int seg;
	
	seg = (int)(y*5.0);
	
		
	if(y==1.0) {
		C[1]=0.0f;
		C[2]=1.0f;
		C[0]=1.f;
	}
	
	switch(seg) {
		case(0):   /* 0 <= y < .2 */
			C[0]=1.0f;
			C[2]=0.0f;
			C[1]=y/.2f;
			break;
		case(1):    /* .2 <= y < .4 */
			C[1]=1.0f;
			C[2]=0.0f;
			C[0]=(.4f - y)/.2f;
			break;
		case(2):  /* .4 <= y < .6 */
			C[1]=1.0f;
			C[0]=0.0f;
			C[2]= 1.f - (.6f-y)/.2f;
			break;
		case(3):  /* .6 <= y < .8 */
			C[2]=1.0f;
			C[0]=0.0f;
			C[1]=(.8f - y)/.2f;
			break;
		case(4):  /* .8 <= y < 1.0 */
			C[1]=0.0f;
			C[2]=1.0f;
			C[0]= 1.f - (1.f-y)/.2f;
			break;
	}
}



/************************************************************************************
*
*                    LEGEND FUNCTIONS
*
************************************************************************************/

void gfmCopyLegendSettings(LegendSettings *From, LegendSettings *To)
{
	To->DrawLegend = From->DrawLegend;
	To->AlreadyResized = From->AlreadyResized;
	To->Type = From->Type;
	To->Location = From->Location;
	To->PercentH = From->PercentH;
	To->FontSize = From->FontSize;
	To->xs = From->xs;
	To->ys = From->ys;
	To->BufferH = From->BufferH;
	To->BufferW = From->BufferW;
	
}




/*
	Draw a legend at the top, bottom, left or right of the screen, depending on the
	values in LegendSettings.  This does this by 
	enlarging the "margin" at the top or the bottom of the screen so that it 
	fills "Percent" (part of the LegendSettings struct) of the height of the screen,
	and the rest of the stuff gets
	scrunched up a little bit.  If there is stuff that is outside of the current
	clipping volume, then it will get drawn over.
	
	The argument AlreadyResized should be zero on the first time this function
	gets called in a rendering function for any of the screens.
	
	N is the number of different Color Key Labels.  
*/

void gfmDrawLegend(int N, char **Names, clipping_struct *Clips, ColorScheme3f *C,
	LegendSettings *L)
{
	int i;
	GLfloat d, step;
	GLfloat BarWidth, BarHeight;
	GLfloat BarUpLeftY, BarUpLeftX;
  
	GLfloat SpanW,SpanH;  /*  the distance from right to left (top to bottom for SpanH) */
						  /*  of the legend area that will be rendered to.  For example, */
						  /*  SpanW = BotRightX - UpLeftX; */
	
	GLfloat p;
	GLfloat x1,x2,y1,y2,hmargin,wmargin,textx,texty;
	void *Font;
	char tempStr[40];
	

	/*  get out of here if we aren't supposed to be drawing a legend */
	if(L->DrawLegend == 0) 
		return;
		
		
		
	/*  set the font sizes */
	switch(L->FontSize) {
		case(0):
			Font = GLUT_BITMAP_HELVETICA_10;
			break;
		case(1):
			Font = GLUT_BITMAP_HELVETICA_12;
			break;
		case(2):
			Font = GLUT_BITMAP_HELVETICA_18;
			break;
	}
	
	/*  determine whether we use the percent for height or width smashing	 */
	if(L->Location == LEGEND_LEFT || L->Location == LEGEND_RIGHT)
			p = L->PercentW;
		else	
			p = L->PercentH;
						  
	/*  first, if the, area has not been resized yet, do so and set AlreadyResized to 1 */
	if(L->AlreadyResized == 0)  {
		
		gfmChangeClippedByProportion(p,Clips, L->Location);

		/*  and don't forget to tell the world that it has now been resized: */
		L->AlreadyResized = 1;
	}
	
	/*  now determine which part of the screen belongs to the legend---i.e. define x1,y1,x2,y2. */
	switch(L->Location)  {
		case(0):  /*  on the left */
			x1 = Clips->xlo;
			x2 = Clips->xlo + (p/(1.0f+p)) * (Clips->xhi - Clips->xlo);
			SpanW = x2-x1;
			y1 = Clips->yhi;
			y2 = Clips->ylo;
			SpanH = y1-y2;
			break;
		case(1):   /*  on the right */
			x1 = Clips->xhi - (p/(1.0f+p)) * (Clips->xhi - Clips->xlo);
			x2 = Clips->xhi;
			SpanW = x2-x1;
			y1 = Clips->yhi;
			y2 = Clips->ylo;
			SpanH = y1-y2;
			break;
		case(2):   /*  on the top */
			x1 = Clips->xlo;
			x2 = Clips->xhi;
			SpanW = x2-x1;
			y1 = Clips->yhi;
			y2 = Clips->yhi - (p/(1.0f+p)) * (Clips->yhi - Clips->ylo);
			SpanH = y1-y2;
			break;
		case(3):   /*  on the bottom */
			x1 = Clips->xlo;
			x2 = Clips->xhi;
			SpanW = x2-x1;
			y1 = Clips->ylo + (p/(1.0f+p)) * (Clips->yhi - Clips->ylo);
			y2 = Clips->ylo;
			SpanH = y1-y2;
			break;
		default:
			break;
			
	}	

	/*  now, draw a box which will come to have line width of 1/2 wmargin and hmargin */
	glColor3fv(C->LegendBoundary);
	glRectf(x1,y1,x2,y2);
	
	/*  now we must shrink that area by the width and height margins */
	/*  determine the amount of each margin */
	wmargin = L->BufferW * SpanW;
	hmargin = L->BufferH * SpanH;
	
	/*  now draw a rectangle of the appropriate background color over the previous one */
	/*  so that we get a boundary */
	glColor3fv(C->LegendBackground);
	glRectf(x1 + .25f * wmargin,
			y1 - .5f * hmargin,
			x2 - .25f * wmargin,
			y2 + .5f * hmargin);
	
	/*  then subtract or add as appropriate */
	x1 += wmargin;
	x2 -= wmargin;
	
	y1 -= hmargin;
	y2 += hmargin;
	
	/*  and reset the spans, too */
	SpanW = x2 - x1;
	SpanH = y1 - y2;
	
	/*  now, draw a box which will come to have line width of 1/2 wmargin and hmargin */
	glRectf(x1,y1,x2,y2);
	
	
	
	
	/*  and now we are ready to actually print out the legend: */
	if(L->Type == BARS)  {
		
		/*  do the drill for the horizontal legends */
		if(L->Location == LEGEND_TOP || L->Location == LEGEND_BOTTOM)  {
			BarWidth = SpanW / ((GLfloat)N*(1.0f + L->xs) );
			step = BarWidth * (1.0f + L->xs);
			BarHeight = SpanH/2.7f;
			BarUpLeftY = y1;
			/*  now start cycling over the color key labels */
			d = x1;
			for(i=0;i<N;i++)  {
					
					glColor3fv(C->Series[i]);
					glRectf(d,BarUpLeftY,d+BarWidth,BarUpLeftY - BarHeight);
					
					/*  then, also print the Key Label associated with the color */
					textx = d;
					texty = y2 + .1f * SpanH;
					if(Names != NULL)
						gfmRenderBitmapString(textx,texty,Font,Names[i]);
					else  {
						sprintf(tempStr,"%d",i+1);
						gfmRenderBitmapString(textx,texty,Font,tempStr);
					}
					d += step;
			}
		}
		
		/*  otherwise, do the drill for the horizontal legends */
		else  {
			BarWidth = SpanW / 5.0f;
			BarHeight = SpanH/((GLfloat)N*(1.0f + L->xs) );
			step = BarHeight * (1.0f + L->xs);
			
			BarUpLeftX = x1;
			/*  now start cycling over the color key labels */
			d = y1;
			for(i=0;i<N;i++)  {
					
					glColor3fv(C->Series[i]);
					glRectf(BarUpLeftX, d, BarUpLeftX + BarWidth,  d-BarHeight);
					
					/*  then, also print the Key Label associated with the color */
					textx = BarUpLeftX + BarWidth + .05f * SpanW;
					texty = d-BarHeight;
					
					if(Names != NULL)
						gfmRenderBitmapString(textx,texty,Font,Names[i]);
					else  {
						sprintf(tempStr,"%d",i+1);
						gfmRenderBitmapString(textx,texty,Font,tempStr);
					}
					
					d -= step;
			}
		}
	}
		

			
} 

/*
	Gets rid of a legend on the screen.  In doing so, it also returns the 
	screen to the proportions it would have had if the legend were not there.
	
	You only want to do this if the window has actually been resized
	
*/
void gfmZapTheLegend(window_settings *WS)
{
	int CW = glutGetWindow();
	GLfloat p;
	
		
	if(WS->Legend->AlreadyResized == 1)  {  /*  if this is the case, we know that we have to  */
											/*  stretch the screen back to it's pre-legend proportions */
		if(WS->Legend->Location == LEGEND_LEFT || WS->Legend->Location == LEGEND_RIGHT)
			p = WS->Legend->PercentW;
		else	
			p = WS->Legend->PercentH;
			
		gfmChangeClippedByProportion(-p,gWindowsSettings[CW]->Clips, WS->Legend->Location);

	}
	
	/*  otherwise, just turn it off */
	WS->Legend->AlreadyResized = 0;
	WS->Legend->DrawLegend = 0;
}



/************************************************************************************
*
*                 FOR RENDERING TEXT ON THE SCREEN
*
************************************************************************************/
/*
	For rendering a string in the proportional roman font 
		s		->  the string to be rendered
		h		->  the height (top to bottom) of the max-height xter (in the alphabet) in real units 
		x		->  x coordinate in the plane
		y		->  y coordinate in the plane where the bottom-line of the text should be
		On		->  how the text is to be aligned:
						0	->  left edge of first letter
						1	->  center of first letter
						2	->  center of the string
						3	->  center of last letter
						4	->  right edge of last letter
		r		->  the rotation angle of the text.  Zero is horizontal to the right
		WoH		->  "width over height" and aspect ratio of sorts 
	
*/
void gfmStrokeString(char *s, GLfloat h, GLfloat x, GLfloat y,  int On, 
				GLfloat r, GLfloat WoH)
{
	char *c;
	GLfloat flw, llw, sw=0.0f;  /*  first letter width, last letter width, string width */
	int Num = 0;  /*  number of characters in string */
	GLfloat sy = h/119.05f, sx = sy / WoH;  /*  scale factors to keep the height correct and */
											/*  the aspect ratio correct as well */
	
	
	/*  first determine the width of everything */
	for (c=s; *c != '\0'; c++) {
		Num++;
		sw += glutStrokeWidth(GLUT_STROKE_ROMAN, (int)(*c));
	}
	flw = glutStrokeWidth(GLUT_STROKE_ROMAN, (int)(s[0]));
	llw = glutStrokeWidth(GLUT_STROKE_ROMAN, (int)(s[Num-1]));
	
	/*  push the current matrix to save it */
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	
	/*  then do the necessary transformations */
	glLoadIdentity();
	glTranslatef(x,y,0.0f);  /*  translate out to the point x,y */
	
	glScalef(sx, sy, 1.0f);	/*  scale the axes for size */
	
	/*  then translate to accommodate the positioning of the text */
	switch(On) {
		case(1):
			glTranslatef(-flw * .5f, 0.0f, 0.0f);
			break;
		case(2):
			glTranslatef(-sw * .5f, 0.0f, 0.0f);
			break;
		case(3):
			glTranslatef(-sw + (.4f * llw),0.0f,0.0f);
			break;
		case(4):
			glTranslatef(-sw,0.0f,0.0f);
			break;
	}
	
	glRotatef(r,0.0f,0.0f,-1.0f);  /*  rotate it around the z-axis */
	
	/*  then lay down the text */
	for (c=s; *c != '\0'; c++) {
		glutStrokeCharacter(GLUT_STROKE_ROMAN, (int)(*c));
	}
	
	/*  then restore whatever matrix was in effect before */
	glPopMatrix();
	
	 
}
/*  render a Bitmap String */
void gfmRenderBitmapString(float x, float y, void *font, char *string) 
{  
	char *c;
	glRasterPos2f(x, y);
	for (c=string; *c != '\0'; c++) {
	  glutBitmapCharacter(font, *c);
	}
}




/*  render the first Num characters in string */
void gfmRenderBitmapCharacters(float x, float y, void *font, char *string,
	 int Num) 
{  
	int i;
	char *c;
	glRasterPos2f(x, y);
	c=string;
	for (i=0; i<Num; i++) {
	  glutBitmapCharacter(font, *c);
	  c++;
	}
}


/************************************************************************************
*
*   FOR COLLECTING STRING INPUT FROM THE USER THROUGH A WINDOW
*
************************************************************************************/

/*  at creation of string input window this should be called.  May not actually */
/*  need to use this */
void gfmAtCreationOfStringInput(void)
{
	gCurrentCharacter = 0;
}


/*  keyboard callback for string input.  Turn spaces into underscores. */
/*  if it is a RETURN, then add a termination '\0' and then point */
/*  the gInputString back to NULL, and close the window */
void gfmStringInputKeyboardFunc(unsigned char key, int x, int y)
{
	int CW = glutGetWindow();
	int dummyx = x, dummyy=y;
	int for_now = (int)key;
	
	/*  just stuff so the compiler doesn't worry about this */
	dummyx = dummyy;
	dummyy = dummyx;
	for_now = dummyx;
	
	/*  if it is the delete key, then go back one step, IF YOU CAN... */
	if((int)key == 8) {  /*  ASCII code for BACKSPACE */
		if(gCurrentCharacter > 0)
			gCurrentCharacter--;
		return;  /*  get out before doing anything else */
	}
	/*  if it is a space-bar, then we turn it into an underscore */
	if((int)key == 32) {  /*  ASCII code for space */
		key = 95;
	}
	/*  if it is a RETURN, then terminate and close the current window */
	if((int)key == 13) {
		gInputString[gCurrentCharacter] = '\0';
		gInputString = NULL;
		if(CW>1) {  /*  don't allow destruction of the Number 1 window. */
			gfmDestroyWindow(CW);
		}
		/*  at the end of that, make the Number 1 window the current window */
		glutSetWindow(1);
		
		/*  then we also must call the UponCompletion function */
		if(gUponStringCompletion != NULL) 
			gUponStringCompletion();
		
		/*  then set that function back to NULL */
		gUponStringCompletion = NULL;
	}
	
	if(key >= 32 && key <= 126)
	/*  now, we add the character to the string we are building */
	gInputString[gCurrentCharacter] = (char)key;
	
	/*  and then increment the current character */
	gCurrentCharacter++;
}

/*  create a string input window to input a string into String.  Hope that String has */
/*  enough space.  Prompt the user with up to three lines of messages in M1, M2, and M3 */
void gfmHandleStringInput(char *M1,char *M2, char *M3, char *String,
	void (*FinishUpFunc)(void) )
{
	int CW = glutGetWindow();
	
	gWindowRequestingStringInput = CW;
	gCurrentCharacter = 0;
	gInputString = String;
	
	/*  open the window directly on top of, and looking somewhat like */
	/*  the window from which the callback for this function came from */
	gfmCreateWindow(gStringInputWindowDef, gWindowsSettings[CW]);
	
	sprintf(gMsg[0], "%s", M1);
	sprintf(gMsg[1], "%s", M2);
	sprintf(gMsg[2], "%s", M3);
	
	gUponStringCompletion = FinishUpFunc;
	
}  


void gfmDrawStringInputWindow(void)
{
	int CW = glutGetWindow();
	GLfloat yhi = gWindowsSettings[CW]->Clips->yhi,
			ylo = gWindowsSettings[CW]->Clips->ylo,
			xlo = gWindowsSettings[CW]->Clips->xlo,
			xhi = gWindowsSettings[CW]->Clips->xhi;
	GLfloat Height, Width;
	
	Height = yhi - ylo;
	Width = xhi - xlo;
	
	glColor3fv(gWindowsSettings[CW]->ColorScheme->Text);
	
	gfmRenderBitmapString(xlo + .05f * Width, yhi - .2f * Height,
			GLUT_BITMAP_HELVETICA_12,gMsg[0]);
			
	gfmRenderBitmapString(xlo + .05f * Width, yhi - .4f * Height,
			GLUT_BITMAP_HELVETICA_12,gMsg[1]);
			
	gfmRenderBitmapString(xlo + .05f * Width, yhi - .6f * Height,
			GLUT_BITMAP_HELVETICA_12,gMsg[2]);
			
	gfmRenderBitmapCharacters(xlo + .05f * Width, yhi - .8f * Height,
			GLUT_BITMAP_HELVETICA_12,gInputString, gCurrentCharacter);

}


void gfmDefineStringInputWindow(void)
{
	gStringInputWindowDef = gfmAllocWindowStruct();
	sprintf(gStringInputWindowDef->Title,"String Input Window");
	gStringInputWindowDef->WindowID = -99;
	gStringInputWindowDef->WT = CHANGING_WIN;
	gStringInputWindowDef->DrawFunc = gfmDrawStringInputWindow;
	gStringInputWindowDef->KeyboardFunc = gfmStringInputKeyboardFunc;

}




/************************************************************************************
*
*   FUNCTIONS FOR POSITIONING, DRAWING, AND SELECTING BAR GRAPHS
*
************************************************************************************/
/*
	This function returns the y coordinate in a unit y-axis [0,1]
	for the i-th bar (base-0 subscripted) given n bars in each column
	and a height of each bar being h, and the separation space between
	bars being ys.  This returns the UPPER edge of the bar
	
*/
GLfloat gfmBarGraphYCoord(int n, GLfloat h, GLfloat ys, int i)
{
	GLfloat y;  /*  the value that we will return */
	int c;  /*  the index of the column the bar is actually in */
	int b;  /*  the number of bars down from the top bar in a column that */
			/*  the i-th bar is. */

	/*  to find y, we must first determine which column the thing is in */
	c = i/n;
	
	/*  and how far down in that column it is */
	b = i - c  *  n;
	
	y = 1.0f - (1.0f + ys) * h  * ( (GLfloat)b + 1.0f) + h;
	
	/*  return y */
	return(y);
}


/*
	This function returns the x coordinate in a unit x-axis [0,1]
	for the i-th bar (base-0 subscripted) in a C-column array with
	each column containing n bars
	with each bar having width w with a separation space of xs times w 
	
*/
GLfloat gfmBarGraphXCoord(int n, GLfloat w, GLfloat xs, int i)
{
	GLfloat x;  /*  the value that we will return */
	int c;  /*  the index of the column the bar is actually in */
	
	/*  to find x, we must first determine which column the thing is in */
	c = i/n;
	
	/*  then... */
	x = w * (1.0f + xs) * (GLfloat)c;
	
	/*  return x */
	return(x);
}



/*
	This is a function that should be called within the drawing function
	of bar graph plots with the window settings XCoord and YCoord as arguments
	along with:
		n  ->  number of elements in each column
		N  ->  total number of elements
		w  ->  total width of each bar
		h  ->  total height of each bar
		xs ->  proportional spacing in the horizontal direction
		ys ->  spacing in the vertical direction
		
	If nothing is selected, this returns a -1.

*/
int gfmBarGraphIdxFromXY(int n, int N, GLfloat w, GLfloat h, GLfloat xs, GLfloat ys,
					double X, double Y)
{
	double dCol,dDown;
	int Col,Down;
	double d;
	int Idx;
	
	
	/*  first determine which column the X puts us in */
	dCol = X/ (w * (1.0 + xs)); 
	Col = (int)dCol;
	
	
	if(dCol < 0.0) 
		return(-1);
	
	if( (dCol - (double)Col) > (1.0/(1.0+xs)) )  /*  if the cursor is in between columns */
		return(-1);
		
	/*  then determine how far down in the column that is.   */
	dDown = ( (h + 1.0 - Y) / ((1.0 + ys) * h) ) - 1.0;
	
	if(dDown < 0)
		return(-1);
	
	Down = (int)dDown;
	
	
	d = dDown - Down;
	if(d > 1.0 / (1.0 + ys) )  /*  if the cursor is between rows */
		return(-1);
	
	/*  now, put a catch in so it doesn't select something below the last row: */
	if(Down > n-1)
		return(-1);
		
	/*  otherwise: */
	Idx = n*Col + Down;
	
	if(Idx > N)
		return(-1);
		
	return(Idx); 
	
	
}


/*
	This function returns, in output parameters, the number of elements (*n) 
	in each full column and the width (*w) of each bar and the height (*h) of each bar
	given that you want to break N bars over C columns with horizontal spacing
	being a proportion xs of each bar width and vertical spacing being a 
	proportion ys of each height.  This function assumes that the bars are
	to be distributed in a grid within the unit square.
	
*/
void gfmSetParsForBarGraphs(int N, int C, GLfloat xs, GLfloat ys,
						int *n, GLfloat *w, GLfloat *h)
{
	
	if(C<1)  {
		printf("\n\nWarning! Number of columns < 1 in gfmBarGraphXCoord()");
		printf("\nSetting Number of columns to 1 and continuing...\n\n");
		C = 1;
	}
	
	if(C > N)	/*  if asking for more columns than items, don't allow it */
		C = N;  
		
	/*  determine n */
	if(N % C == 0)  /*  if all the columns have equal numbers of bars */
		*n = N/C;
	else  			/*  otherwise */
		*n = (int)(  (double)N/(double)C + 1.0); 
		
	/*  assign value to w */
	*w = 1.0f / ((GLfloat)C * (1.0f + xs) - xs);	
	
	/*  assign value to h */
	*h = 1.0f / (GLfloat)((1.0f + ys) * (*n));	 

}


/*
	draws a leaf on a rectangle that represents the proportions of a total
*/
void gfmDrawRectLeaf6f(GLfloat x,   /*   x and y are the upper left of the "value = 0" rectangle */
				GLfloat y,
				GLfloat w, /*  the width of the rectangle from the "value = 0" to the */
							/*  "value = total" point */
				GLfloat h, /*  the height of the rectangle */
				GLfloat value, /*  the value currently being represented */
				GLfloat total, /*  the value represented by the sum of all the values in the array */
				GLfloat *Color)
{
	GLfloat leftx = x + (value/total) * w;  /*  for the bottom left point of this leaf */
	
	/*  set the color  */
	glColor3fv(Color);
	
	glRectf(leftx, y, x+w, y-h);
	
	/*  and that's it folks */
}



void gfmAppendToFile(char *FileName, char *String)
{
	FILE *F;
	
	F = fopen(FileName, "a");
	fprintf(F,"\n%s",String);
	fclose(F);
}

/************************************************************************************
*
*    		ULTRA-SIMPLE SLIDER BAR CONTROLS
*
************************************************************************************/
void gfmDrawVertIntSlider(void)
{
	gsSettings gsCS
	int lo,hi, cur,x,i;
	char temp[250];
	
	
	glPushAttrib(GL_ALL_ATTRIB_BITS); 
	
	lo = (int)Settings->SliderLow;
	hi = (int)Settings->SliderHigh;
	
	cur = *(Settings->IntSlider);
	
	
	
	
	/* now allow for changes of the value by mouse input */
	if(Settings->SlideKeyFlag ==0 && cur != (int)Settings->YCoord && Settings->XCoord > 0.0 && Settings->XCoord < .5 )
		cur = (int)Settings->YCoord;
	
	/* now make the change from keyboard input and change the X coordinate so that it is out of the slider box
	   and won't affect things in the future */
	if(Settings->SlideKeyFlag==1)  {
		Settings->XCoord = 5.0;
		Settings->SlideKeyFlag=0;
	}
	
	
	if(cur > hi)
		cur = hi;
	if(cur < lo)
		cur = lo;
		
	/* now, make the change to the refresh rate */
	*(Settings->IntSlider) = cur;
	
	/* draw a box in the axis color */
	glColor3fv(CS->Axes);
	
	glBegin(GL_LINE_LOOP);
		glVertex2i(0,lo);
		glVertex2i(0,hi);
		glVertex2f(.5f,(GLfloat)hi);
		glVertex2f(.5f,(GLfloat)lo);
	glEnd();
	
	/* then put 5 equally spaced number labels on that  */
	glColor3fv(CS->Text);
	for(x=lo,i=0;i<4; x+=hi/4,i++) {
		sprintf(temp,"%d",x);
		gfmRenderBitmapString(.6f,x,GLUT_BITMAP_HELVETICA_10, temp);
	}
	x = hi;
	sprintf(temp,"%d",x);
	gfmRenderBitmapString(.6f,x,GLUT_BITMAP_HELVETICA_10, temp);
	
	/* then draw a little rectangle at the current value in the first series color */
	glColor3fv(CS->Series[0]);
	glLineWidth(2.0);
	glBegin(GL_LINES);
		glVertex2i(0,cur);
		glVertex2f(.5f,(GLfloat)cur);
	glEnd();
	
	/* then write the name of the variable at the bottom of the box */
	gfmRenderBitmapString(0.f,(GLfloat)(lo - .05*(hi-lo)),GLUT_BITMAP_HELVETICA_10, Settings->SlideName);

	/* then write the current value of the variable at the top of the box */
	sprintf(temp,"%d",cur);
	gfmRenderBitmapString(0.f,(GLfloat)(hi + .05*(hi-lo)),GLUT_BITMAP_HELVETICA_10, temp);	
	
	
	/* draw the user input part if there is any  */
	if(Settings->SlideNewVal[0] != '\0')  { 
		/* first draw a background rectangle to write on.  Make this in the text color */
		glColor3fv(CS->Text);
		glRectf(-.5,(hi-lo)/2.0 - (hi-lo)*.1, 5.0,(hi-lo)/2.0 + (hi-lo)*.1);
		
		/* then, in the first series color, write the new value that is being input */
		glColor3fv(CS->Series[0]);
		gfmRenderBitmapString(0.f,(GLfloat)((hi + lo)/2.0),GLUT_BITMAP_HELVETICA_10, Settings->SlideNewVal);
	}
	
	glPopAttrib();
}


void gfmDrawVertDoubleSlider(void)
{
	gsSettings gsCS
	double lo,hi, cur, x;
	int i;
	char temp[250];
	
	
	glPushAttrib(GL_ALL_ATTRIB_BITS); 
	
	lo = Settings->SliderLow;
	hi = Settings->SliderHigh;
	
	cur = *(Settings->DoubleSlider);
	
	
	
	
	/* now allow for changes of the value by mouse input */
	if(Settings->SlideKeyFlag ==0 && cur != Settings->YCoord && Settings->XCoord > 0.0 && Settings->XCoord < .5 )
		cur = Settings->YCoord;
	
	/* now make the change from keyboard input and change the X coordinate so that it is out of the slider box
	   and won't affect things in the future */
	if(Settings->SlideKeyFlag==1)  {
		Settings->XCoord = 5.0;
		Settings->SlideKeyFlag=0;
	}
	
	
	if(cur > hi)
		cur = hi;
	if(cur < lo)
		cur = lo;
		
	/* now, make the change to the refresh rate */
	*(Settings->DoubleSlider) = cur;
	
	/* draw a box in the axis color */
	glColor3fv(CS->Axes);
	
	glBegin(GL_LINE_LOOP);
		glVertex2f(0,(GLfloat)lo);
		glVertex2f(0,(GLfloat)hi);
		glVertex2f(.5f,(GLfloat)hi);
		glVertex2f(.5f,(GLfloat)lo);
	glEnd();
	
	/* then put 5 equally spaced number labels on that  */
	glColor3fv(CS->Text);
	for(x=lo,i=0;i<4; x+=hi/4.0,i++) {
		sprintf(temp,"%.3f",x);
		gfmRenderBitmapString(.6f,x,GLUT_BITMAP_HELVETICA_10, temp);
	}
	x = hi;
	sprintf(temp,"%.3f",x);
	gfmRenderBitmapString(.6f,x,GLUT_BITMAP_HELVETICA_10, temp);
	
	/* then draw a little rectangle at the current value in the first series color */
	glColor3fv(CS->Series[0]);
	glLineWidth(2.0);
	glBegin(GL_LINES);
		glVertex2f(0,(GLfloat)cur);
		glVertex2f(.5f,(GLfloat)cur);
	glEnd();
	
	/* then write the name of the variable at the bottom of the box */
	gfmRenderBitmapString(0.f,(GLfloat)(lo - .05*(hi-lo)),GLUT_BITMAP_HELVETICA_10, Settings->SlideName);

	/* then write the current value of the variable at the top of the box */
	sprintf(temp,"%.4f",cur);
	gfmRenderBitmapString(0.f,(GLfloat)(hi + .05*(hi-lo)),GLUT_BITMAP_HELVETICA_10, temp);	
	
	
	/* draw the user input part if there is any  */
	if(Settings->SlideNewVal[0] != '\0')  { 
		/* first draw a background rectangle to write on.  Make this in the text color */
		glColor3fv(CS->Text);
		glRectf(-.5,(hi-lo)/2.0 - (hi-lo)*.1, 5.0,(hi-lo)/2.0 + (hi-lo)*.1);
		
		/* then, in the first series color, write the new value that is being input */
		glColor3fv(CS->Series[0]);
		gfmRenderBitmapString(0.f,(GLfloat)((hi + lo)/2.0),GLUT_BITMAP_HELVETICA_10, Settings->SlideNewVal);
	}
	
	glPopAttrib();
}


/* to handle slider-bar keyboard input, to allow changing variables */
void gfmSliderBarNormalKeys(unsigned char key, int x, int y)
{
	gsSettings
	
	char *snv = Settings->SlideNewVal;
	
	/* if it is a number between 0 and 9 or a period, then we let this fill the
	   SlideNewVal string and return  */
	
	if( (glutGetModifiers() != GLUT_ACTIVE_CTRL && glutGetModifiers() != GLUT_ACTIVE_ALT) && (
		(int)key >= (int)'0' && (int)key <= (int)'9' || (int)key == (int)'.') ) {
		
		sprintf(snv,"%s%c",snv,key);
		
		return;
	}
	
	if((int)key==127 || (int)key==8 )  {  /* if it is the delete key or backspace key, remove the last character */
		snv[strlen(snv)-1] = '\0';
	}
	
	if((int)key==13 && snv[0] != '\0')  {  /* if it is a carriage return, and the string is non-empty then we store the value  */
		if(Settings->IntSlider!=NULL) 
			*Settings->IntSlider = atoi(snv);
		if(Settings->DoubleSlider != NULL) 
			*Settings->DoubleSlider = (double)atof(snv);
		
		/* account for the different settings it can take */
		if(Settings->IntSlider!=NULL) {
			*Settings->IntSlider = (*Settings->IntSlider > Settings->SliderHigh) ?  Settings->SliderHigh :  *Settings->IntSlider;
			*Settings->IntSlider = (*Settings->IntSlider < Settings->SliderLow) ? Settings->SliderLow : *Settings->IntSlider;
			
		}
		if(Settings->DoubleSlider != NULL)  {
			*Settings->DoubleSlider = (*Settings->DoubleSlider > Settings->SliderHigh) ? Settings->SliderHigh :  *Settings->DoubleSlider;
			*Settings->DoubleSlider = (*Settings->DoubleSlider < Settings->SliderLow) ? Settings->SliderLow :  *Settings->DoubleSlider;
		}
	
		snv[0] = '\0';
		
		/* set the flag saying that a change was made */
		Settings->SlideKeyFlag=1;
	}
	
	gfmProcessNormalKeys(key,x,y);
}

/************************************************************************************
*
*    						 AXES
*
************************************************************************************/
/*  draws an x-axis using the info in the XY_axis_struct Xaxis in the window settings of */
/*  the current window */
void gfmDrawXaxis(void)
{	
	int CW = glutGetWindow();
	XY_axis_struct *A = gWindowsSettings[CW]->Xaxis;
	XY_axis_struct *AY = gWindowsSettings[CW]->Yaxis;
	clipping_struct *C =  gWindowsSettings[CW]->Clips;
	GLfloat lo,hi,pos;  
	
	if(A->DrawIt != 1) 
		return;  /*  get the hell out, doing nothing if this is the case */
		
	/*  set the position of the axis */
	if(A->ForceThroughPoint == 1)
		pos = A->Intersect;
	else if(A->AxisEdge == AXIS_LEFT_BELOW)  {
		pos = C->ylo + (C->yhi - C->ylo) * A->PropFromEdge;
		if(AY->ImposeLowerLimit == 1) {  /*  don't let it fly off the lower end of the other axis, though */
			pos = (GLfloat)fmax((double)pos, (double)AY->LowerLimit);
		}
	}
	else if(A->AxisEdge == AXIS_RIGHT_ABOVE)  {
		pos = C->yhi - (C->yhi - C->ylo) * A->PropFromEdge;
		if(AY->ImposeUpperLimit == 1) {  /*  don't let it fly off the lower end of the other axis, though */
			pos = (GLfloat)fmin((double)pos, (double)AY->UpperLimit);
		}
	}
	
	/*  set for future reference */
	A->AxisPos = pos;
	
	
	/*  set the upper extent of the axis */
	if(A->ImposeUpperLimit == 1)
		hi = (GLfloat)fmin((double)A->UpperLimit,(double)C->xhi);
	else
		hi = C->xhi;
		
	/*  set the lower extent of the axis */
	if(A->ImposeLowerLimit == 1)
		lo = (GLfloat)fmax((double)A->LowerLimit,(double)C->xlo);
	else
		lo = C->xlo;
		
	/*  then draw the dude */
	glColor3fv(gWindowsSettings[CW]->ColorScheme->Axes);
	glBegin(GL_LINES);
		glVertex2f(lo,pos);
		glVertex2f(hi,pos);
	glEnd();

}


/*  draws a y-axis using the info in the XY_axis_struct Xaxis in the window settings of */
/*  the current window */
void gfmDrawYaxis(void)
{	
	int CW = glutGetWindow();
	XY_axis_struct *A = gWindowsSettings[CW]->Yaxis;
	XY_axis_struct *AX = gWindowsSettings[CW]->Xaxis;
	clipping_struct *C =  gWindowsSettings[CW]->Clips;
	GLfloat lo,hi,pos;  
	
	if(A->DrawIt != 1) 
		return;  /*  get the hell out, doing nothing if this is the case */
		
	/*  set the position of the axis */
	if(A->ForceThroughPoint == 1)
		pos = A->Intersect;
	else if(A->AxisEdge == AXIS_LEFT_BELOW)  {
		pos = C->xlo + (C->xhi - C->xlo) * A->PropFromEdge;
		if(AX->ImposeLowerLimit == 1) {  /*  don't let it fly off the lower end of the other axis, though */
			pos = (GLfloat)fmax((double)pos, (double)AX->LowerLimit);
		}
	}
	else if(A->AxisEdge == AXIS_RIGHT_ABOVE)  {
		pos = C->xhi - (C->xhi - C->xlo) * A->PropFromEdge;
		if(AX->ImposeUpperLimit == 1) {  /*  don't let it fly off the lower end of the other axis, though */
			pos = (GLfloat)fmin((double)pos, (double)AX->UpperLimit);
		}
	}


		
	A->AxisPos = pos;
	
	/*  set the upper extent of the axis */
	if(A->ImposeUpperLimit == 1)
		hi = (GLfloat)fmin((double)A->UpperLimit,(double)C->yhi);
	else
		hi = C->yhi;
		
	/*  set the lower extent of the axis */
	if(A->ImposeLowerLimit == 1)
		lo = (GLfloat)fmax((double)A->LowerLimit,(double)C->ylo);
	else
		lo = C->ylo;
		
	/*  then draw the dude */
	glColor3fv(gWindowsSettings[CW]->ColorScheme->Axes);
	glBegin(GL_LINES);
		glVertex2f(pos,lo);
		glVertex2f(pos,hi);
	glEnd();

}


/*  copies the contents of an axis struct.  assumes that memory has already */
/*  been allocated to both */
void gfmCopyXYAxisStructs(XY_axis_struct *From, XY_axis_struct *To)
{
	To->DrawIt = From->DrawIt;
	To->ForceThroughPoint = From->ForceThroughPoint;
	To->Intersect = From->Intersect;
	To->AxisEdge = From->AxisEdge;
	To->AxisPos = From->AxisPos;
	To->PropFromEdge = From->PropFromEdge;
	To->ImposeUpperLimit = From->ImposeUpperLimit;
	To->UpperLimit = From->UpperLimit;
	To->ImposeLowerLimit = From->ImposeLowerLimit;
	To->LowerLimit = From->LowerLimit;
	
	To->TickPos = From->TickPos;
	To->WriteLabel = From->WriteLabel;
	
	sprintf(To->Label,"%s",From->Label);
	
	To->OptimalNumMajor = From->OptimalNumMajor;
	
	To->MajorProp = From->MajorProp;
	To->MinorProp = From->MinorProp;
	
	To->MajorSep = From->MajorSep;
	To->MajorLo = From->MajorLo;
	To->MajorHi = From->MajorHi;
	To->MinorSep = From->MinorSep;
	
	To->DrawGrids = From->DrawGrids;
	
	To->DrawNumLabels = From->DrawNumLabels;
	To->NumbersDist = From->NumbersDist;
	To->NumsHeight = From->NumsHeight;
	
	
}


/*  determines the spacing of the major ticks for the current window  */
/*  only does this if it is supposed to draw the axes */
void gfmDetermineMajorTicksX(void)
{
	int CW = glutGetWindow();
	XY_axis_struct *A = gWindowsSettings[CW]->Xaxis;
	clipping_struct *C =  gWindowsSettings[CW]->Clips;
	GLfloat s = C->xhi - C->xlo;
	GLfloat L;
	int SigFigs[]  = { 1,	3,		1,		2,		1,	0,		2,	  0,	 1,		0};
	GLfloat Divs[] = {.1f,  .125f,  .2f,  .25f,  .5f,  1.0f,  1.25f,  2.0f,  2.5f,  5.0f};
	GLfloat Minors[]={.02f, .025f,  .04f, .05f,  .1f,  .2f,   .25f,   .4f,  .5f,   1.0f};
	GLfloat BestSep;
	int BestI,i;
	GLfloat Leftovers;
	
	if(A->DrawIt != 1) 
		return;  /*  get the hell out, doing nothing if this is the case */
		
	L = floorf(log10(s)+.000001f);
	
	s *= pow(10,-L);  /*  turn it into a value between 1 and 10; */
	
	/*  find the major tick spacing closest to Optimal */
	BestSep = fabs((GLfloat)A->OptimalNumMajor - s/.1f);
	BestI = 0;
	for(i=1;i<10;i++)  {
		if(fabs((GLfloat)A->OptimalNumMajor - s/Divs[i]) < BestSep) {
			BestSep = fabs((GLfloat)A->OptimalNumMajor - s/Divs[i]);
			BestI = i;
		}
	}
	
	A->MajorSep = Divs[BestI] * pow(10.0f,L);
	A->MinorSep = Minors[BestI] * pow(10.0f,L);
	
	A->SigFigs = (int)(SigFigs[BestI] - L);
	
	/*  then determine where we should start ticking from, and where we should end */
	if(A->ImposeLowerLimit == 1)  {
		Leftovers = fabs(A->LowerLimit/A->MajorSep - floorf(A->LowerLimit/A->MajorSep));
		A->MajorLo = floorf(A->LowerLimit/A->MajorSep) * A->MajorSep + (GLfloat)(Leftovers > .001 * pow(10,L)) * A->MajorSep;
	}
	else {
		A->MajorLo = floorf(C->xlo/A->MajorSep) * A->MajorSep;
	}
	/*  set the high to whatever the top part is, and we will just use that as a stopping condition in a  */
	/*  for loop  */
	if(A->ImposeUpperLimit == 1)  {
		A->MajorHi = A->UpperLimit;
	}
	else {
		A->MajorHi = C->xhi;
	}
	
}


/*  relatively straightforward function that must be called AFTER gfmDetermineMajorTicksX */
void gfmDrawTicksX(void)
{
	GLfloat d;
	int CW = glutGetWindow();
	XY_axis_struct *A = gWindowsSettings[CW]->Xaxis;
	clipping_struct *C =  gWindowsSettings[CW]->Clips;
	GLfloat h = (C->yhi - C->ylo) * A->MajorProp;
	GLfloat lo,hi,pos;  
	
	if(A->DrawIt != 1) 
		return;  /*  get the hell out, doing nothing if this is the case */
		
	/*  set the position of the axis */
	pos = A->AxisPos;
	
	/*  do the major ticks first */
	if(A->TickPos == AXIS_TICKS_LEFT_BELOW)  {
		hi = pos;
		lo = pos - h;
	}
	if(A->TickPos == AXIS_TICKS_RIGHT_ABOVE)  {
		lo = pos;
		lo = pos + h;
	}
	if(A->TickPos == AXIS_TICKS_CENTERED)  {
		hi = pos + .5f * h;
		lo = pos - .5f * h;
	}
		
	
	glColor3fv(gWindowsSettings[CW]->ColorScheme->Axes);
	
	glBegin(GL_LINES);
		for(d=A->MajorLo;d<=A->MajorHi;d+=A->MajorSep) {
			glVertex2f(d,lo);
			glVertex2f(d,hi);
		}
	glEnd();
	
	/*  then do the minor ticks */
	h *= A->MinorProp;
	if(A->TickPos == AXIS_TICKS_LEFT_BELOW)  {
		hi = pos;
		lo = pos - h;
	}
	if(A->TickPos == AXIS_TICKS_RIGHT_ABOVE)  {
		lo = pos;
		lo = pos + h;
	}
	if(A->TickPos == AXIS_TICKS_CENTERED)  {
		hi = pos + .5f * h;
		lo = pos - .5f * h;
	}
		
	
	glColor3fv(gWindowsSettings[CW]->ColorScheme->Axes);
	
	glBegin(GL_LINES);
		for(d=A->MajorLo;d<=A->MajorHi;d+=A->MinorSep) {
			glVertex2f(d,lo);
			glVertex2f(d,hi);
		}
	glEnd();

}

/*  determines the spacing of the major ticks for the current window  */
/*  only does this if it is supposed to draw the axes */
void gfmDetermineMajorTicksY(void)
{
	int CW = glutGetWindow();
	XY_axis_struct *A = gWindowsSettings[CW]->Yaxis;
	clipping_struct *C =  gWindowsSettings[CW]->Clips;
	GLfloat s = C->yhi - C->ylo;
	GLfloat L;
	int SigFigs[]  = { 1,	3,		1,		2,		1,	0,		2,	  0,	 1,		0};
	GLfloat Divs[] = {.1f,  .125f,  .2f,  .25f,  .5f,  1.0f,  1.25f,  2.0f,  2.5f,  5.0f};
	GLfloat Minors[]={.02f, .025f,  .04f, .05f,  .1f,  .2f,   .25f,   .4f,  .5f,   1.0f};
	GLfloat BestSep;
	int BestI,i;
	GLfloat Leftovers;
	
	if(A->DrawIt != 1) 
		return;  /*  get the hell out, doing nothing if this is the case */
		
	L = floorf(log10(s)+.000001f);
	
	s *= pow(10,-L);  /*  turn it into a value between 1 and 10; */
	
	/*  find the major tick spacing closest to optimal closest to Optimal */
	BestSep = fabs((GLfloat)A->OptimalNumMajor - s/.1f);
	BestI = 0;
	for(i=1;i<10;i++)  {
		if(fabs((GLfloat)A->OptimalNumMajor - s/Divs[i]) < BestSep) {
			BestSep = fabs((GLfloat)A->OptimalNumMajor - s/Divs[i]);
			BestI = i;
		}
	}
	
	A->MajorSep = Divs[BestI] * pow(10.0f,L);
	A->MinorSep = Minors[BestI] * pow(10.0f,L);
	
	A->SigFigs = (int)(SigFigs[BestI] - L);
	
	/*  then determine where we should start ticking from, and where we should end */
	if(A->ImposeLowerLimit == 1)  {
		Leftovers = fabs(A->LowerLimit/A->MajorSep - floorf(A->LowerLimit/A->MajorSep));
		A->MajorLo = floorf(A->LowerLimit/A->MajorSep) * A->MajorSep + (GLfloat)(Leftovers > .001 * pow(10,L)) * A->MajorSep;
	}
	else {
		A->MajorLo = floorf(C->ylo/A->MajorSep) * A->MajorSep;
	}
	/*  set the high to whatever the top part is, and we will just use that as a stopping condition in a  */
	/*  for loop  */
	if(A->ImposeUpperLimit == 1)  {
		A->MajorHi = A->UpperLimit;
	}
	else {
		A->MajorHi = C->yhi;
	}
	
}




/*  relatively straightforward function that must be called AFTER gfmDetermineMajorTicksX */
void gfmDrawTicksY(void)
{
	GLfloat d;
	int CW = glutGetWindow();
	XY_axis_struct *A = gWindowsSettings[CW]->Yaxis;
	clipping_struct *C =  gWindowsSettings[CW]->Clips;
	GLfloat h = (C->xhi - C->xlo) * A->MajorProp;
	GLfloat lo,hi,pos;  
	
	if(A->DrawIt != 1) 
		return;  /*  get the hell out, doing nothing if this is the case */
		
	/*  set the position of the axis */
	pos = A->AxisPos;
	
	/*  do the major ticks first */
	if(A->TickPos == AXIS_TICKS_LEFT_BELOW)  {
		hi = pos;
		lo = pos - h;
	}
	if(A->TickPos == AXIS_TICKS_RIGHT_ABOVE)  {
		lo = pos;
		lo = pos + h;
	}
	if(A->TickPos == AXIS_TICKS_CENTERED)  {
		hi = pos + .5f * h;
		lo = pos - .5f * h;
	}
		
	
	glColor3fv(gWindowsSettings[CW]->ColorScheme->Axes);
	
	glBegin(GL_LINES);
		for(d=A->MajorLo;d<=A->MajorHi;d+=A->MajorSep) {
			glVertex2f(lo,d);
			glVertex2f(hi,d);
		}
	glEnd();
	
	/*  then do the minor ticks */
	h *= A->MinorProp;
	if(A->TickPos == AXIS_TICKS_LEFT_BELOW)  {
		hi = pos;
		lo = pos - h;
	}
	if(A->TickPos == AXIS_TICKS_RIGHT_ABOVE)  {
		lo = pos;
		lo = pos + h;
	}
	if(A->TickPos == AXIS_TICKS_CENTERED)  {
		hi = pos + .5f * h;
		lo = pos - .5f * h;
	}
		
	
	glColor3fv(gWindowsSettings[CW]->ColorScheme->Axes);
	
	glBegin(GL_LINES);
		for(d=A->MajorLo;d<=A->MajorHi;d+=A->MinorSep) {
			glVertex2f(lo,d);
			glVertex2f(hi,d);
		}
	glEnd();

}



/*  once the major grids have been determined, draw number labels on them */
void gfmDrawXAxisNumLabels(void)
{
	GLfloat d;
	int CW = glutGetWindow();
	XY_axis_struct *A = gWindowsSettings[CW]->Xaxis;
	clipping_struct *C =  gWindowsSettings[CW]->Clips;
	GLfloat h = (C->yhi - C->ylo) * A->NumsHeight;
	GLfloat ypos = A->AxisPos - (C->yhi - C->ylo) * A->NumbersDist;
	char String[100];
	
	if(A->DrawIt != 1) 
		return;  /*  get the hell out, doing nothing if this is the case */
		
	if(A->DrawNumLabels == 0)  /*  get out now if we aren't supposed to draw them */
		return;
		
		
	for(d=A->MajorLo;d<=A->MajorHi;d+=A->MajorSep) {
		/*  make the default format string with 0 signicant digits */
		/*  to the right of the decimal point, then work up from there */
		/*  as necessary */
		sprintf(String,"%.0f",d);
		
		switch(A->SigFigs) {
			case(1):
				sprintf(String,"%.1f",d);
				break;
			case(2):
				sprintf(String,"%.2f",d);
				break;
			case(3):
				sprintf(String,"%.3f",d);
				break;
			case(4):
				sprintf(String,"%.4f",d);
				break;
			case(5):
				sprintf(String,"%.5f",d);
				break;
			case(6):
				sprintf(String,"%.6f",d);
				break;
			case(7):
				sprintf(String,"%.7f",d);
				break;
			/*  beyond that and I really should do scientific notation */
			/*  I'll deal with that later */
			
		}
		gfmStrokeString(String, h, d, ypos, 2, 0.0f, gfmWoH());
	}
}


/*  once the major grids have been determined, draw number labels on them */
/*  for the y-axis num labels, we make the height of the letters a proportion of */
/*  the width of the screen */
void gfmDrawYAxisNumLabels(void)
{
	GLfloat d;
	int CW = glutGetWindow();
	XY_axis_struct *A = gWindowsSettings[CW]->Yaxis;
	clipping_struct *C =  gWindowsSettings[CW]->Clips;
	GLfloat h = (C->yhi - C->ylo) * A->NumsHeight;
	GLfloat xpos = A->AxisPos - (C->xhi - C->xlo) * A->NumbersDist;
	char String[100];
	GLfloat dd;
	
	if(A->DrawIt != 1)  /*  also, if we are not drawing any axes, get out!! */
		return;
		
	if(A->DrawNumLabels == 0)  /*  get out now if we aren't supposed to draw them */
		return;
	
	for(d=A->MajorLo;d<=A->MajorHi;d+=A->MajorSep) {
		/*  make the default format string with 0 signicant digits */
		/*  to the right of the decimal point, then work up from there */
		/*  as necessary */
		sprintf(String,"%.0f",d);
		
		switch(A->SigFigs) {
			case(1):
				sprintf(String,"%.1f",d);
				break;
			case(2):
				sprintf(String,"%.2f",d);
				break;
			case(3):
				sprintf(String,"%.3f",d);
				break;
			case(4):
				sprintf(String,"%.4f",d);
				break;
			case(5):
				sprintf(String,"%.5f",d);
				break;
			case(6):
				sprintf(String,"%.6f",d);
				break;
			case(7):
				sprintf(String,"%.7f",d);
				break;
			/*  beyond that and I really should do scientific notation */
			/*  I'll deal with that later */
		}
		
		/*  drop the text down by .5 of the height of the letters so they  */
		/*  are centered on the grids */
		dd = d - .42f * h;
		
		gfmStrokeString(String, h, xpos, dd, 4, 0.0f, gfmWoH());
	}
}


/*  just collects all the necessary functions into one */
void gfmDrawXYAxes(void)
{
	gfmDrawXaxis();
	gfmDrawYaxis();
	gfmDetermineMajorTicksX();
	gfmDrawTicksX();
	gfmDetermineMajorTicksY();
	gfmDrawTicksY();
	gfmDrawXAxisNumLabels();
	gfmDrawYAxisNumLabels();
}


void gfmColorSchemePredefiningFunc(ColorScheme3f *CS[MAX_CS+1],int *NumC, int Menu )
{
	int N = 4, i=0;
	ColorScheme3f *C = *CS;

*NumC = N;


glutSetMenu(Menu);

	CS[i] = gfmAllocColorScheme(0);
	C = CS[i];
	sprintf(C->Title,"Fisher_Price_25");
	C->ID = 0;
	C->N = 25;
	C->Background[0] = 0.000000f;
	C->Background[1] = 0.000000f;
	C->Background[2] = 0.000000f;
	C->Text[0] = 1.000000f;
	C->Text[1] = 1.000000f;
	C->Text[2]=1.000000f;
	C->Axes[0] = 1.000000f;
	C->Axes[1] = 1.000000f;
	C->Axes[2] = 1.000000f;
	C->LegendBackground[0] = 0.000000f;
	C->LegendBackground[1] = 0.000000f;
	C->LegendBackground[2] = 0.000000f;
	C->LegendBoundary[0] = 1.000000f;
	C->LegendBoundary[1] = 1.000000f;
	C->LegendBoundary[2] = 1.000000f;
	C->Series[0][0] = 1.000000f; C->Series[0][1] = 0.000000f; C->Series[0][2] = 0.000000f;
	C->Series[1][0] = 1.000000f; C->Series[1][1] = 1.000000f; C->Series[1][2] = 0.000000f;
	C->Series[2][0] = 0.000000f; C->Series[2][1] = 0.000000f; C->Series[2][2] = 1.000000f;
	C->Series[3][0] = 0.000000f; C->Series[3][1] = 1.000000f; C->Series[3][2] = 0.000000f;
	C->Series[4][0] = 0.600000f; C->Series[4][1] = 0.000000f; C->Series[4][2] = 1.000000f;
	C->Series[5][0] = 0.000000f; C->Series[5][1] = 1.000000f; C->Series[5][2] = 1.000000f;
	C->Series[6][0] = 0.400000f; C->Series[6][1] = 0.200000f; C->Series[6][2] = 0.000000f;
	C->Series[7][0] = 1.000000f; C->Series[7][1] = 0.400000f; C->Series[7][2] = 0.000000f;
	C->Series[8][0] = 0.600000f; C->Series[8][1] = 0.600000f; C->Series[8][2] = 0.400000f;
	C->Series[9][0] = 0.400000f; C->Series[9][1] = 0.000000f; C->Series[9][2] = 0.000000f;
	C->Series[10][0] = 0.200000f; C->Series[10][1] = 0.400000f; C->Series[10][2] = 1.000000f;
	C->Series[11][0] = 1.000000f; C->Series[11][1] = 0.400000f; C->Series[11][2] = 1.000000f;
	C->Series[12][0] = 0.000000f; C->Series[12][1] = 0.400000f; C->Series[12][2] = 0.000000f;
	C->Series[13][0] = 1.000000f; C->Series[13][1] = 0.800000f; C->Series[13][2] = 0.400000f;
	C->Series[14][0] = 0.400000f; C->Series[14][1] = 0.600000f; C->Series[14][2] = 0.000000f;
	C->Series[15][0] = 0.800000f; C->Series[15][1] = 0.200000f; C->Series[15][2] = 0.200000f;
	C->Series[16][0] = 0.000000f; C->Series[16][1] = 0.200000f; C->Series[16][2] = 1.000000f;
	C->Series[17][0] = 0.600000f; C->Series[17][1] = 0.000000f; C->Series[17][2] = 0.200000f;
	C->Series[18][0] = 0.600000f; C->Series[18][1] = 1.000000f; C->Series[18][2] = 0.000000f;
	C->Series[19][0] = 1.000000f; C->Series[19][1] = 0.200000f; C->Series[19][2] = 0.000000f;
	C->Series[20][0] = 0.600000f; C->Series[20][1] = 1.000000f; C->Series[20][2] = 0.800000f;
	C->Series[21][0] = 0.400000f; C->Series[21][1] = 0.000000f; C->Series[21][2] = 0.400000f;
	C->Series[22][0] = 1.000000f; C->Series[22][1] = 1.000000f; C->Series[22][2] = 1.000000f;
	C->Series[23][0] = 0.200000f; C->Series[23][1] = 0.200000f; C->Series[23][2] = 0.400000f;
	C->Series[24][0] = 1.000000f; C->Series[24][1] = 0.000000f; C->Series[24][2] = 0.400000f;

	glutAddMenuEntry(C->Title,i);
i++;

	CS[i] = gfmAllocColorScheme(0);
	C = CS[i];
	sprintf(C->Title,"Ol_Grayback_20");
	C->ID = 1;
	C->N = 20;
	C->Background[0] = 0.800000f;
	C->Background[1] = 0.800000f;
	C->Background[2] = 0.800000f;
	C->Text[0] = 0.000000f;
	C->Text[1] = 0.000000f;
	C->Text[2]=0.000000f;
	C->Axes[0] = 0.000000f;
	C->Axes[1] = 0.000000f;
	C->Axes[2] = 0.000000f;
	C->LegendBackground[0] = 0.800000f;
	C->LegendBackground[1] = 0.800000f;
	C->LegendBackground[2] = 0.800000f;
	C->LegendBoundary[0] = 0.600000f;
	C->LegendBoundary[1] = 0.600000f;
	C->LegendBoundary[2] = 0.600000f;
	C->Series[0][0] = 0.000000f; C->Series[0][1] = 0.000000f; C->Series[0][2] = 1.000000f;
	C->Series[1][0] = 1.000000f; C->Series[1][1] = 0.000000f; C->Series[1][2] = 0.000000f;
	C->Series[2][0] = 1.000000f; C->Series[2][1] = 1.000000f; C->Series[2][2] = 0.200000f;
	C->Series[3][0] = 0.000000f; C->Series[3][1] = 1.000000f; C->Series[3][2] = 0.000000f;
	C->Series[4][0] = 0.600000f; C->Series[4][1] = 0.000000f; C->Series[4][2] = 0.800000f;
	C->Series[5][0] = 1.000000f; C->Series[5][1] = 0.400000f; C->Series[5][2] = 0.000000f;
	C->Series[6][0] = 1.000000f; C->Series[6][1] = 0.400000f; C->Series[6][2] = 1.000000f;
	C->Series[7][0] = 0.000000f; C->Series[7][1] = 0.000000f; C->Series[7][2] = 0.400000f;
	C->Series[8][0] = 0.600000f; C->Series[8][1] = 0.000000f; C->Series[8][2] = 0.000000f;
	C->Series[9][0] = 0.000000f; C->Series[9][1] = 1.000000f; C->Series[9][2] = 1.000000f;
	C->Series[10][0] = 0.200000f; C->Series[10][1] = 0.200000f; C->Series[10][2] = 0.600000f;
	C->Series[11][0] = 0.400000f; C->Series[11][1] = 0.200000f; C->Series[11][2] = 0.000000f;
	C->Series[12][0] = 1.000000f; C->Series[12][1] = 0.200000f; C->Series[12][2] = 0.000000f;
	C->Series[13][0] = 0.400000f; C->Series[13][1] = 0.400000f; C->Series[13][2] = 0.400000f;
	C->Series[14][0] = 0.600000f; C->Series[14][1] = 0.800000f; C->Series[14][2] = 0.000000f;
	C->Series[15][0] = 0.200000f; C->Series[15][1] = 0.000000f; C->Series[15][2] = 0.000000f;
	C->Series[16][0] = 1.000000f; C->Series[16][1] = 1.000000f; C->Series[16][2] = 1.000000f;
	C->Series[17][0] = 1.000000f; C->Series[17][1] = 0.800000f; C->Series[17][2] = 0.000000f;
	C->Series[18][0] = 0.200000f; C->Series[18][1] = 0.000000f; C->Series[18][2] = 0.800000f;
	C->Series[19][0] = 0.800000f; C->Series[19][1] = 0.400000f; C->Series[19][2] = 1.000000f;

	glutAddMenuEntry(C->Title,i);
	
i++;

	CS[i] = gfmAllocColorScheme(0);
	C = CS[i];
	sprintf(C->Title,"Deep_Blue_15");
	C->ID = 2;
	C->N = 15;
	C->Background[0] = 0.000000f;
	C->Background[1] = 0.000000f;
	C->Background[2] = 0.200000f;
	C->Text[0] = 1.000000f;
	C->Text[1] = 1.000000f;
	C->Text[2]=0.000000f;
	C->Axes[0] = 1.000000f;
	C->Axes[1] = 1.000000f;
	C->Axes[2] = 0.000000f;
	C->LegendBackground[0] = 0.000000f;
	C->LegendBackground[1] = 0.000000f;
	C->LegendBackground[2] = 0.200000f;
	C->LegendBoundary[0] = 0.000000f;
	C->LegendBoundary[1] = 0.000000f;
	C->LegendBoundary[2] = 0.400000f;
	C->Series[0][0] = 1.000000f; C->Series[0][1] = 0.000000f; C->Series[0][2] = 0.000000f;
	C->Series[1][0] = 0.000000f; C->Series[1][1] = 1.000000f; C->Series[1][2] = 0.000000f;
	C->Series[2][0] = 0.000000f; C->Series[2][1] = 0.200000f; C->Series[2][2] = 1.000000f;
	C->Series[3][0] = 1.000000f; C->Series[3][1] = 0.000000f; C->Series[3][2] = 1.000000f;
	C->Series[4][0] = 1.000000f; C->Series[4][1] = 0.800000f; C->Series[4][2] = 0.000000f;
	C->Series[5][0] = 0.400000f; C->Series[5][1] = 0.200000f; C->Series[5][2] = 0.000000f;
	C->Series[6][0] = 0.000000f; C->Series[6][1] = 1.000000f; C->Series[6][2] = 1.000000f;
	C->Series[7][0] = 0.000000f; C->Series[7][1] = 0.400000f; C->Series[7][2] = 0.000000f;
	C->Series[8][0] = 1.000000f; C->Series[8][1] = 0.200000f; C->Series[8][2] = 0.000000f;
	C->Series[9][0] = 0.600000f; C->Series[9][1] = 0.600000f; C->Series[9][2] = 0.600000f;
	C->Series[10][0] = 0.400000f; C->Series[10][1] = 0.000000f; C->Series[10][2] = 0.800000f;
	C->Series[11][0] = 0.600000f; C->Series[11][1] = 0.200000f; C->Series[11][2] = 0.000000f;
	C->Series[12][0] = 0.600000f; C->Series[12][1] = 0.400000f; C->Series[12][2] = 1.000000f;
	C->Series[13][0] = 0.000000f; C->Series[13][1] = 0.400000f; C->Series[13][2] = 0.400000f;
	C->Series[14][0] = 0.800000f; C->Series[14][1] = 0.000000f; C->Series[14][2] = 0.000000f;

	glutAddMenuEntry(C->Title,i);
	
	i++;
	
	CS[i] = gfmAllocColorScheme(0);
	C = CS[i];
	sprintf(C->Title,"GilbertAndSherman_15");
	C->ID = 1;
	C->N = 15;
	C->Background[0] = 0.000000f;
	C->Background[1] = 0.400000f;
	C->Background[2] = 0.000000f;
	C->Text[0] = 1.000000f;
	C->Text[1] = 1.000000f;
	C->Text[2]=0.400000f;
	C->Axes[0] = 1.000000f;
	C->Axes[1] = 1.000000f;
	C->Axes[2] = 0.200000f;
	C->LegendBackground[0] = 0.000000f;
	C->LegendBackground[1] = 0.400000f;
	C->LegendBackground[2] = 0.000000f;
	C->LegendBoundary[0] = 0.200000f;
	C->LegendBoundary[1] = 0.400000f;
	C->LegendBoundary[2] = 0.000000f;
	C->Series[0][0] = 1.000000f; C->Series[0][1] = 0.400000f; C->Series[0][2] = 0.800000f;
	C->Series[1][0] = 0.200000f; C->Series[1][1] = 0.200000f; C->Series[1][2] = 0.200000f;
	C->Series[2][0] = 1.000000f; C->Series[2][1] = 1.000000f; C->Series[2][2] = 1.000000f;
	C->Series[3][0] = 1.000000f; C->Series[3][1] = 0.800000f; C->Series[3][2] = 0.000000f;
	C->Series[4][0] = 0.000000f; C->Series[4][1] = 0.000000f; C->Series[4][2] = 0.000000f;
	C->Series[5][0] = 0.400000f; C->Series[5][1] = 0.200000f; C->Series[5][2] = 0.000000f;
	C->Series[6][0] = 0.200000f; C->Series[6][1] = 0.000000f; C->Series[6][2] = 0.400000f;
	C->Series[7][0] = 1.000000f; C->Series[7][1] = 0.000000f; C->Series[7][2] = 0.000000f;
	C->Series[8][0] = 0.000000f; C->Series[8][1] = 0.000000f; C->Series[8][2] = 1.000000f;
	C->Series[9][0] = 0.000000f; C->Series[9][1] = 1.000000f; C->Series[9][2] = 1.000000f;
	C->Series[10][0] = 1.000000f; C->Series[10][1] = 0.200000f; C->Series[10][2] = 0.000000f;
	C->Series[11][0] = 0.200000f; C->Series[11][1] = 0.400000f; C->Series[11][2] = 1.000000f;
	C->Series[12][0] = 0.400000f; C->Series[12][1] = 0.800000f; C->Series[12][2] = 0.400000f;
	C->Series[13][0] = 0.400000f; C->Series[13][1] = 0.000000f; C->Series[13][2] = 0.200000f;
	C->Series[14][0] = 0.600000f; C->Series[14][1] = 0.000000f; C->Series[14][2] = 1.000000f;

	glutAddMenuEntry(C->Title,i);

	i++;

}


