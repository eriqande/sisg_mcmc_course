/*  This is a header file for some routines that Eric C. Anderson */
/*  has written in order to (with luck) quickly and easily design */
/*  windowing systems using the GLUT interface to track MCMC simulations. */

/*  The purpose of this package is to specify several data types and */
/*  to provide a few functions, so that a user can just define his  */
/*  own rendering schemes for each window that he wants, and then the */
/*  functions herein will sort of take care of the rest. */

/*  one of the main difficulties is in registering a callback to */
/*  render anew each active window (we want all open windows to be */
/*  updated on each sweep).  */



/*  Conditional preprocessing.  In all files except the one in which */
/*  the function main()  resides, the preprocessor replaces "GLOB" with "extern". */
/* The file containing main() should start with the line:

							#define UN_EXTERN
*/		

					
#ifndef GLUT_FOR_MCMC_HFLAG  /*  only include this if it has not already been included */
#define GLUT_FOR_MCMC_HFLAG

#ifndef UN_EXTERN
	#define GLOB extern
#else
	#define GLOB 
#endif

#if (!defined COMPILE_FOR_PC_WIN32) || (!defined HAVE_GL_GLUT_H)
	#include <GLUT/glut.h>
#else
	#include <GL/glut.h>
#endif


#include "GFMCMC_Shortcuts.h"



/* ***********************************************************************************/
/*  */
/*            #DEFINES AND TYPEDEFS */
/*  */
/* ***********************************************************************************/


/*  Here is a wacky thing I have to do because there is something funny */
/*  between glut and GL */
#define GLsizei int


/*  Some #define's */
#define MAX_WIN   250   /*  the maximum number of windows that one you can have open simultaneously */
#define MAX_COLOR_KEY_LENGTH  150  /*  max length of a string for a color key label */
#define MAX_CS  25  /*  max number of color schemes */
#define MAX_VIEWS 50
#define MAX_VIEW_NAME_LENGTH 250
#define MAX_AXIS_LABEL_LENGTH 250
#define MAX_TITLE 300
#define MAX_WIN_DEFS 300
#define MAX_SERIES_COLORS 40
#define MAX_INPUT_MSG_LENGTH 500
#define MAX_USER_DEFD_MENUS 30
#define LARGEST_WINDOW_ID 500
#define MAX_SLIDER_NAME_LENGTH 100
#define MAX_SLIDER_NEW_VAL_LENGTH 100

#define NUM_INTEGER_OPTIONS  500
#define NUM_FLOAT_OPTIONS	 500

#define COLOR_PICKER_WINDOW_ID 100




/*  the numbers of pixels surrounding a window on the left, right, bottom and */
/*  top.  This is system dependent.  This is for the MacOS */
#define GFM_L 6
#define GFM_R 6
#define GFM_B 6
#define GFM_T 23
#define GFM_TOP_BAR 21  /*  number of pixels in the top menu bar  */


#define CYC_WINDOWS(i) for(i=1;i<=MAX_WIN;i++)  {
#define CYC_WIN_DEFS(i)  for(i=0;i<=MAX_WIN_DEFS;i++)  {
#define END1CYCLE }


/*  for the position of the axis labels */
#define AXIS_LEFT_BELOW			0
#define AXIS_RIGHT_ABOVE		1
#define AXIS_NUMS_LEFT_BELOW	0  /*  put the numbers to the left of the Y axis or below the X axis */
#define AXIS_NUMS_RIGHT_ABOVE	1
#define AXIS_TICKS_LEFT_BELOW 	0
#define AXIS_TICKS_RIGHT_ABOVE	1
#define AXIS_TICKS_CENTERED		2

/*  macros for the Predefined Color Schemes */
#define FISHER_PRICE 0
#define OL_GRAYBACK 1
#define DEEP_BLUE 2
#define GILBERT_AND_SHERMAN 3

/*  enums */

enum win_type
{
	STATIC_WIN,  	/*  a window that needn't change every cycle */
	CHANGING_WIN	/*  a window that should be updated after every sweep */
};

enum LegendLocation
{
	LEGEND_LEFT,
	LEGEND_RIGHT,
	LEGEND_TOP,
	LEGEND_BOTTOM
};

enum LegendType
{
	BARS,
	LINES
};


/*  typedefs and data structures */
typedef struct
{
	int ID;  /*  the index of the color scheme */
	int N;  /*  the number of colors available in the scheme */
	
	char Title[MAX_TITLE];
	
	GLfloat **Series;  /*  the "Series" color values stored as three floats.  When plotting things, */
					/*  the element of the series, subscripted by i gets the color CS[i%N]. */
	GLfloat Background[3];  /*  the background color stored as three floats */
	GLfloat Axes[3];  /*  the color for axes, grids, ticks and axis labels, if any,  */
						 /*  stored as three floats */
	GLfloat Text[3];	/*  the color for text on the screen that is not axis labels */
	GLfloat LegendBoundary[3];  /*  color for a box that surrounds the legend */
	GLfloat LegendBackground[3];
	
} ColorScheme3f;




typedef struct 
{
	int WindowID;			/*  the index of this window definition */
	
	char Title[MAX_TITLE];		/*  the title of the window */
	
	enum win_type WT;
	
	int QT;		/*  denotes whether the window displays current values or averages, etc. */
	
	char **ColorKey;		/*  an array of names for colors (like "F1" or "Allele 5") */
	int *NumColorKeys;		/*  the number of things that the different colors refer to */
							/*  this should be used to point at something in the data structures */
							/*  of the MCMC program */
							
	int DefaultColorSchemeIdx;   /*  so that the default color scheme may be applied to it */
								 /*  in CreateWindow, this gets used in changing the  */
								 /*  window settings */
								 
	int MiddleMouseMenuIdx;  /*  if this is greater than 0, then we will attach that menu */
						 /*  to the right mouse button.   */
						 
	int NoMainMenu;     /*  this shall be a one if we want to NOT attach the main menu */
						/*  to the window (this will then let use mouse clicks on the  */
						/*  PC. */
	
		/*  pointer to the rendering function that should be called for the window */
	void (*DrawFunc)(void);	/*  the argument of DrawFunc is here specified to */
						/*  be a quant_type, the top-level data structure must */
						/*  be available as a global variable to pass in to the function */
						
	void (*AtCreation)(void);  /*  Function to be called upon window creation */
	void (*AtDestruction)(void);  /*  Function to be called upon window destruction */
	void (*ExtraFunction)(void);
	void (*KeyboardFunc)(unsigned char, int, int);
	
	
	
} window_struct;

typedef struct 
{
	int DrawLegend;   	  /*  1 if a legend should be drawn, 0 otherwise */
	int AlreadyResized;   /*  0 for no and 1 for yes */
	enum LegendType Type;
	enum LegendLocation Location;
	GLfloat PercentH;   
	GLfloat PercentW;	/*  the percentage of the height or width of the window that the */
						/*  legend should take up.  Notice that if we have a function that */
						/*  can change this during program execution, we must be careful to */
						/*  ensure that it is always set to the proportion of the *original* */
						/*  screen. */
	int FontSize;   /*  0 = GLUT_BITMAP_HELVETICA_10 */
					/*  1 = GLUT_BITMAP_HELVETICA_12 */
					/*  2 = GLUT_BITMAP_HELVETICA_18 */
					
	GLfloat xs;	 /*  separation between bars as a proportion of their width, yatta yatta... */
	GLfloat ys;
				 /*  note that in the case of line legends, these will have some meaning, too. */
				 
	GLfloat BufferH;  /*  margins around the area in which the legend is rendered in the height */
	GLfloat BufferW;  /*  and the width directions, respectively. */
	
} LegendSettings; 

typedef struct 
{
	GLfloat xlo;
	GLfloat xhi;
	GLfloat ylo;
	GLfloat yhi;
	GLfloat zlo;
	GLfloat zhi;
} clipping_struct;



/*  this is used to automatically expand or contract the clipping volume so that  */
/*  the contents of window (primarily I am thinking here about plotted points  */
/*  or curves) will fit into it.  It has several flags in it, to keep it from */
/*  trying to change the clipping volume if it does not know how big to make it. */
/*  It is the responsibility of the user to determine the max and the min of plotted */
/*  points in the function that is being called to plot those points.   */
typedef struct 
{
	int AlwaysFit;  /*  if 1 it will resize the thing on every iteration (seldom used) */
	int FitToExtremaNow;   /*  if it is a one, it will try to re-clip, but it also requires */
						  /*  that the new mins and maxes have been set */
	int FitToDefaultsNow;  /*  this being 1 will cause the clipping volume to be set back to */
						   /*  the default clipping */
	int ExtremaSet;  /*  this should get to set to 1 after the max ranges have been set */
					 /*  and then should be set back to 0 after the clipping volume has */
					 /*  been changed.   */
					 
	
	/*  these are the "extra bits" between the extreme points and the edge of the */
	/*  clipping volume.  So, these values are not really clipping edges--they are */
	/*  proportions by which the clipping volume should exceed the extreme in all directions. */
	
	clipping_struct *Padding;  
	
	clipping_struct *Extrema;  /*  this records the max and min in every direction */
							   /*  upon initialization of a window, these are set to */
							   /*  the initial window clips. */
	clipping_struct *Default;  /*  this gets set to the default values, of the clipping */
						   	   /*  struct, and when a window gets clipped to these values, */
						   	   /*  it does not include the Padding. */
	
} fitting_struct;


typedef struct
{
	int DrawIt;  /*  1 if it should be drawn, 0 otherwise */
	
	int ForceThroughPoint;  /*  1 if the axis should be forced through a point  */
	GLfloat Intersect;  /*  the point to force it through---this controls the Y position of the */
					    /*  X axis and the X position of the Y axis */
					    
	int AxisEdge;  /*  takes one of  AXIS_LEFT_BELOW  or AXIS_RIGHT_ABOVE depending on which edge */
					/*  the axis should be placed off of, if this it is not forced through a point */
					    
	GLfloat AxisPos;  /*  This gets set by DrawXaxis() or DrawYAxis() and tells where it is in the */
					  /*  scene.  Then the ticks and grids can know where it is too. */
					    
	GLfloat PropFromEdge;  /*  the proportion of the screen from the left side that the Y axis */
						   /*  should be put and from the bottom that the X axis should be put */
						   
	int ImposeUpperLimit;  /*  1 if there is an upper limit beyond which no axis gets drawn */
						   /*  otherwise the axis gets drawn across the entire clipping region */
	GLfloat UpperLimit;
	
	int ImposeLowerLimit;
	GLfloat LowerLimit;

	int TickPos;    /*  Determines where relative to the axis the ticks get drawn.  Uses one of: */
				/*  AXIS_TICKS_LEFT_BELOW or AXIS_TICKS_RIGHT_ABOVE or AXIS_TICKS_CENTERED */
					
	int WriteLabel;  /*  1 if the label should be drawn */
	char Label[MAX_AXIS_LABEL_LENGTH];  /*  for the name that will appear below the axis */
	
	int OptimalNumMajor;  /*  the user can set this to determine the number of Major ticks and */
						  /*  hence number labels along the axis */
	GLfloat MajorProp;  /*  proportion of the viewable screen area that the major ticks should be */
	GLfloat MinorProp;  /*  proportion of the Major tick size that the minor ticks should be */
						  
	/*  the function "gfmDetermineMajorTicksX (or Y)" sets the following values */
	GLfloat MajorSep;	/*  the distance between major ticks */
	GLfloat MajorLo;	/*  the lowest value for which a tick/number label will be drawn */
	GLfloat MajorHi;	/*  the highest value... */
	GLfloat MinorSep;   /*  the distance between the minor ticks.  They will get drawn in between */
					/*  MajorLo and MajorHi */
	int SigFigs;    /*  the number of digits right of the decimal place that should be printed */
					
					
	int DrawGrids;  /*  1 if stippled grids should be drawn at Major Tick spacing */

	
	int DrawNumLabels;  /*  1 if the number labels should be drawn */
	GLfloat NumbersDist;  /*  distance from axis for the number labels.  Down (or to the left) */
						  /*  if positive and above (to the right) if negative, as a proportion */
						  /*  of the viewable width or height */
	GLfloat NumsHeight;  /*  height of axis number label text as proportion of total height */

	

} XY_axis_struct;


/*  variables to deal with drawing a rectangle on the screen that can be zoomed into: */
typedef struct 
{
	int DrawDZBox;  /*  1 if we should draw a DragZoomBox */
	
	GLfloat downX;	/*  coordinates of the mouse upon GLUT_DOWN mouseclick */
	GLfloat downY;
	
	GLfloat upX;	/*  coordinates of the mouse upon GLUT_UP mouseclick */
	GLfloat upY;	/*  AND while dragging the mouse. */
	
	/*  eventually will want to add a stippling pattern thing here */
} drag_zoom_struct;






typedef struct
{
	/*  here is an entry that does NOT get written out to file when recording window settings! */
	int MenuEntry;
	
	/*  variables to store the x, y, and z coordinate limits */
	clipping_struct *Clips;
	
	ColorScheme3f *ColorScheme;	/*  an array of the colors used by the window */
	
	LegendSettings *Legend;
	
	/*  variables to help with resizing the window to fit available points on screen */
	fitting_struct *Fits;
	
	/*  variable to hold info about the axes. */
	XY_axis_struct *Xaxis;
	XY_axis_struct *Yaxis;
	
	drag_zoom_struct *DZ;
	
	GLfloat XCoord;  /*  for recording the position of the mouse during */
	GLfloat YCoord;  /*  MotionFunc() callbacks */
	
	int XPos;
	int YPos;
	int Width;
	int Height;
	
	int bbXPos;  /*  the ''bounce-back" versions of these things.  They get set */
	int bbYPos;		/*  to the original window values when it is created, but */
	int bbWidth;	/*  then they will get reset to current values when the */
	int bbHeight;	/*  backslash "\" command is issued in the window, and it gets */
					/*  resized, to full screen.  The next "\" command to the window */
					/*  returns it from whence it came before the auto-full-size resizing. */
	int IsFullSize; /*  starts as zero, but gets reset when it gets full-sized.s */
	
	int NumCols;   /*  the number of columns in Bar-Type graphs */
	int SelectedElement;  /*  The Element currently selected by a mouse click */
	int DisplayedElement; /*  for screens where you view just one of many possible things */
						  /*  in the window, this tells which element of the array to view. */
						  
	int *IntegerOptions;  /*  for user-defined options */
	float *FloatOptions;
	
	/*  these will only be used if the window is a slider-control type thing */
	/*  these will point to the variables that will be changed when the slider changes */
	char SlideNewVal[MAX_SLIDER_NEW_VAL_LENGTH];  /* this stores user input that might become a new value */
	char SlideName[MAX_SLIDER_NAME_LENGTH];
	int *IntSlider;
	double *DoubleSlider;
	double SliderHigh;
	double SliderLow;
	int SlideKeyFlag;  /* gets set to 1 when a slider is changed by keyboard input */
	
} window_settings;


typedef struct 
{
	int NumOpen;   	/*  the number of open windows in the view */
	char Name[MAX_VIEW_NAME_LENGTH];   	/*  the name of this particular view */
	
	int OrigWidth;  /*  width and height in pixels of the window that originally */
	int OrigHeight; /*  the view was made on.  These will allow scaling to */
					/*  smaller or larger screens. */
	
	int *IDs;      	/*  the ID's of the different windows that are open */
	window_settings **WS;  /*  the window settings of the open windows */
	
	
	
} view_struct;


/* ***********************************************************************************/
/*  */
/*            GLOBAL VARIABLES */
/*  */
/* ***********************************************************************************/
/*  some global variables that have to do with the screen size and the size */
/*  of borders around windows, and the height of title bars for them, etc. */
GLOB int gSH;  /*  Screen Height.  This is the number of pixels of screen height */
GLOB int gHH;  /*  Header Height.  This is the number of pixels taken up by the menu bar at the */
		   	   /*  top of the screen.  On a System 9 Mac it seems to be about 20. */
GLOB int gSW;  /*  Screen Width */
GLOB int gWTB; /*  height of a window top border in pixels (about 20 on the mac) */
GLOB int gWB;  /*  size of the other window borders in pixels. (about 5 or 6 on the mac) */

GLOB int gCCWW;  /*  command console window width---the window width of the window */
				 /*  that lists all the views, etc. */



/*  GLOBAL Variables of the Win Def Set and Views sort: */
GLOB int gWinIDReverse[LARGEST_WINDOW_ID + 1];
GLOB int gNumWinDefs;
GLOB window_struct *gWinDefs[MAX_WIN_DEFS + 1];  /*  the array of windows_structs that hold the  */
								/*  definitions of the windows.  Will be allocated to dynamically */
								/*  during run time when it is known how many different types of  */
								/*  windows one can display */
GLOB window_settings *gInitWinSettings[MAX_WIN_DEFS + 1];
GLOB window_struct *gWindowsArray[MAX_WIN + 1];
GLOB window_settings *gWindowsSettings[MAX_WIN + 1];
GLOB int gNumViews; 			/*  the number of predefined views */
GLOB view_struct *gViews[MAX_VIEWS + 1];  /*  for PREDEFINED Views */
GLOB int gNumUDViews;		/*  for user-defined views */

GLOB view_struct *gUDViews[MAX_VIEWS + 1];
GLOB int gNumOpenWindows;


GLOB int gGo;  /*  global variable for Stopping and Going with the space bar toggle */
GLOB int gDoASingleSweep;  /*  for controlling "Single Sweep Mode" */
GLOB int gNumSweeps;
GLOB int gNumSweepsAfterBurnIn;
		
/*  silly flag for whether or not we are currently processing the console */
/*  window in UserDefd_DefineWindows() */
GLOB int gCurrentlyProcessingConsole;				 
						 						 
/*  global variables for storing menu numbers */
GLOB int gMainMenu;
GLOB int gChainControlSubMenu;
GLOB int gSwitchWindowSubMenu;
GLOB int gCreateNewWindowsSubMenu;
GLOB int gViewsSubMenu;
GLOB int gCloseWindowsSubMenu;
GLOB int gPredefinedViewsSubMenu;  /*  this holds all the views that the developer has defined. */
GLOB int gUserDefdViewsSubMenu; /*  for views that the user defines */



GLOB int gLegendControlSubMenu;
GLOB int gUserDefdColorSubMenu;
GLOB int gPreDefColorSubMenu;
GLOB int gAutoFitSubMenu;
GLOB int gBringToFrontSubMenu;
GLOB int gQuitSubMenu;
GLOB int gColorSchemeMenu;
GLOB int gUserDefdMenus[MAX_USER_DEFD_MENUS];



/*  global arrays of pointers to color schemes that are available */
GLOB ColorScheme3f *gPreDefdColorSchemes[MAX_CS + 1];
GLOB ColorScheme3f *gUserDefdColorSchemes[MAX_CS + 1];
GLOB ColorScheme3f *gColorSchemes[MAX_CS + 1];
/*  global variables for keeping track of color schemes in the above arrays */
GLOB int gNumPreDefdColorSchemes;
GLOB int gNumUserDefdColorSchemes;


/*  global variables for defining and setting the color picker window */
/*  and for identifying it */
GLOB window_struct *gColorPickerWinDef;
GLOB window_settings *gColorPickerWinSettings;
GLOB int gColorPickerWindowNumber;


/*  global variables to have while running the "Color Picker" window */
GLOB int gCSPState;   /*  Color scheme picker state */
GLOB int gCSPHiState;  /*  the highest value that CSPState has yet reached */
GLOB int gCSPExit;
GLOB int gCSPComplete;  /*  gets a one if the whole color scheme has been done right */
GLOB int gCSPHiN;
GLOB int gCSPCurrentN;
GLOB int gCSPSpaceForN;  /*  records how much space has been allocated for the Series */
GLOB GLfloat gCSPr;  /*  red, blue and green values to make the instructions shimmer */
GLOB GLfloat gCSPb;  /*  so that they can be seen on any background color	 */
GLOB GLfloat gCSPg;	 /*  these are actually arguments to a sin function */
/*  pointer to the array of 216 colors. */
GLOB GLfloat **g216Colors3f;


/*  global variables for keeping track of the creation of a string on screen */
GLOB int gCurrentCharacter;
GLOB char *gInputString;  /*  this should get pointed to wherever the string's */
						  /*  destination should be. */
GLOB char gMsg[3][MAX_INPUT_MSG_LENGTH];
GLOB void (*gUponStringCompletion)(void);  /*  global pointer to function that should */
									/*  be called when string input has concluded */
GLOB int gWindowRequestingStringInput;  /*  gUponStringCompletionMayWishToUseThis */
GLOB window_struct *gStringInputWindowDef;
GLOB window_settings* gStringInputWindowSettings;

/*  global strings that get defined by appending the appropriate ending (i.e. PreDefdViews.txt) */
/*  to the prefix name provided by the developer.   */
GLOB char gPredDefdViewsFileName[100];
GLOB char gUserDefdViewsFileName[100];
GLOB char gUserDefdColorSchemesFileName[100];


/*  a variable to tell us whether we should use the current random number seeds when */
/*  we initialize the chain or not */
GLOB int gUseSameSeeds;

GLOB char gGFM_PWD[10000]; /* to store the pwd just before entering GFMCMC */
GLOB char gERROR_FILE[10000];  


/* ***********************************************************************************/
/*  */
/*       PROTYPES FOR FUNCTIONS THAT MUST BE DEFINED BY THE USER */
/*  */
/* ***********************************************************************************/
GLOB void gfmUserDefd_ResetAllAverages(void);
GLOB void gfmUserDefd_InitializeChain(void);
GLOB void gfmUserDefd_OneStepByGlobals(void);
GLOB void gfmUserDefd_DefineWindows(void);
GLOB void gfmUserDefd_DefineMenus(void);  /*  to define menus for the right click of the mouse */
GLOB void gfmUserDefd_LastWords(void);


/* ***********************************************************************************/
/*  */
/*       				THE MAIN INITIALIZATION FUNCTION */
/* 	 */
/* 				AND MAIN LOOP FUNCTIONS FOR REDRAWING ALL THE CURRENT WINDOWS */
/*  */
/* ***********************************************************************************/
GLOB void gfmInitGFM(void);
GLOB void gfmUpdateImages(void);
GLOB void gfmSilly(void);
GLOB void gfmIdleFunc(void);



/* ***********************************************************************************/
/*  */
/*     WINDOW, SETTINGS, AND VIEWS MEMORY ALLOCATION/DEALLOCATION, VALUE INITIALIZATION, COPYING */
/*  */
/* ***********************************************************************************/
GLOB window_struct *gfmAllocWindowStruct(void);
GLOB window_settings *gfmAllocWindowSettings(void);
GLOB void gfmFreeWindowSettings(window_settings *W);
GLOB void gfmCopyWindowSettings(window_settings *From, window_settings *To);
GLOB view_struct *gfmAllocView(int N);
GLOB void gfmResizeToFull(void);


/* ***********************************************************************************/
/*  */
/*     MORE MEMORY ALLOCATION AND VARIABLE INITIALIZATION, COPYING */
/*  */
/* ***********************************************************************************/
GLOB void gfmInitDragZoom(drag_zoom_struct *Z, int DrawBox, GLfloat x, GLfloat y);

/* ***********************************************************************************/
/*  */
/*       			 WINDOW CREATION / DESTRUCTION / MOVEMENT */
/*  */
/* ***********************************************************************************/
GLOB void gfmDestroyWindow(int CW);
GLOB void gfmCreateWindow(window_struct *W, window_settings *InitWS);
GLOB void gfmCreateWindowByIndex(int idx);
GLOB void gfmCreateWindowsForView(int L, view_struct **V);
GLOB void gfmCreateWindowsForPreDefdViews(int L);
GLOB void gfmCreateWindowsForUserDefdViews(int L);
GLOB void gfmBringWindowToFront(int W);


/* ***********************************************************************************/
/*  */
/*      RECORDING NEW VIEWS, READING AND WRITING SETTINGS AND VIEWS TO FILES */
/*  */
/* ***********************************************************************************/
GLOB void gfmRecordNewView(int Dummy);
GLOB void gfmUponEntryOfViewName(void);
GLOB void gfmWriteWindowSettings(window_settings *WS, FILE *F);
GLOB void gfmReadWindowSettings(window_settings *WS, FILE *F);
GLOB void gfmReadWindowSettingsFromFile(window_settings *WS, FILE *F);
GLOB void gfmWriteViewsToFile(view_struct **V, char *FileName);
GLOB void gfmReadViewsFromFile(view_struct **V, char *FileName, int *Num, int Menu);
GLOB void gfmScaleTheView(GLfloat w, GLfloat h);
GLOB void gfmScaleWindowPosAndSizeViewWise(window_settings *W, GLfloat w, GLfloat h);
GLOB void gfmWriteFittingStructToFile(fitting_struct *F, FILE *out);
GLOB void gfmReadFittingStructFromFile(fitting_struct *F, FILE *in);
GLOB void gfmWriteClippingStructToFile(clipping_struct *C,FILE *out);
GLOB void gfmReadClippingStructFromFile(clipping_struct *C,FILE *in);
GLOB void gfmWriteXYAxisSettingsToFile(XY_axis_struct *A, FILE *out);
GLOB void gfmReadXYAxisSettingsToFile(XY_axis_struct *A, FILE *out);

/* ***********************************************************************************/
/*  */
/*     						 VIEWABLE AREA AND SUCH */
/*  */
/* ***********************************************************************************/
GLOB GLfloat gfmWoH(void);
GLOB void gfmUnitSquareWithBorder(window_settings *W, float border);
GLOB void gfmCopyClippings(clipping_struct *From,clipping_struct *To);
GLOB void gfmSetPaddedExtremeClips(window_settings *WS);
GLOB void gfmChangeSize(GLsizei w, GLsizei h);
GLOB void gfmClipOrtho(void);
GLOB void gfmChangeClippedByProportion(GLfloat P, clipping_struct *Clips, int Direction);
GLOB void gfmTranslateClippedByProportion(GLfloat P, clipping_struct *Clips, int Direction);
GLOB void gfmZoomToDragBox(void);

/* ***********************************************************************************/
/*  */
/*        						WEIRD THINGS */
/*  */
/* ***********************************************************************************/
GLOB void gfmResizeAllForProperInit(void);
GLOB void gfmResizeForProperInit(void);
GLOB void gfmColor3fvAlphaf(GLfloat *c, GLfloat a);

/* ***********************************************************************************/
/*  */
/*        SOME STANDARD ROUTINES FOR PROCESSING KEYBOARD INPUT */
/*  */
/* ***********************************************************************************/
GLOB void gfmProcessNormalKeys(unsigned char key, int x, int y);
GLOB void gfmProcessSpecialKeys(int key, int x, int y);



/* ***********************************************************************************/
/*  */
/*        ROUTINES FOR DETERMINING THE X,Y COORDINATES OF MOUSE CLICKS */
/*  */
/* ***********************************************************************************/
GLOB GLfloat gfmReturnXCoord(int XPix);
GLOB GLfloat gfmReturnYCoord(int YPix);
GLOB void gfmMouseLocation(int x, int y);
GLOB void gfmStandardMouseClick(int button, int state, int x, int y);


/* ***********************************************************************************/
/*  */
/*       	DEFINING THE MENUS AND SUCH */
/*  */
/* ***********************************************************************************/
GLOB void gfmDefineGFMMenus(void);
GLOB void gfmKeysMirroredInMenuFunc(int Num);


/*  Function Prototypes: */

/* ***********************************************************************************/
/*  */
/*                COLOR SCHEME MEMORY ALLOCATION */
/*  */
/* ***********************************************************************************/
GLOB ColorScheme3f *gfmAllocColorScheme(int N);
GLOB void gfmFreeColorScheme(ColorScheme3f *C);



/* ***********************************************************************************/
/*  */
/*                       COLOR PICKER WINDOW FUNCTIONS */
/*  */
/* ***********************************************************************************/
GLOB void gfmDefineColorPickerWindow(void);
GLOB void Define216Colors(void);
GLOB void gfmDrawColorPicker(void);
GLOB void gfmColorPickerKeyboardFunc(unsigned char key, int x, int y);
GLOB void gfmInitiateColorPicking(int input);
GLOB void gfmCallAfterColorSchemeStringInput(void);


/* ***********************************************************************************/
/*  */
/*                  FUNCTIONS FOR SETTING COLOR SCHEMES IN WINDOWS */
/* 				- used mainly as functions that menu options call - */
/* ***********************************************************************************/
GLOB void gfmSetToUserDefdColorScheme(int input);
GLOB void gfmSetToPreDefdColorScheme(int state);


/* ***********************************************************************************/
/*  */
/*              FUNCTIONS FOR WRITING AND READING COLOR SCHEMES TO FILE */
/*  */
/* ***********************************************************************************/
GLOB void gfmWriteColorScheme3f(ColorScheme3f *C, FILE *F);
GLOB void gfmReadColorScheme3f(ColorScheme3f *C, FILE *F);
GLOB void gfmWriteColorSchemesToFile(ColorScheme3f **C, int N, char *FileName );
GLOB void gfmWriteColorSchemesToFileAsSource(ColorScheme3f **C, int N, char *FileName );
GLOB void gfmReadColorSchemesFromFile(ColorScheme3f *Temp[MAX_CS + 1],const char *FileName,
	int *NumC, int Menu);
GLOB void gfmWriteColorScheme3fAsSource(ColorScheme3f *C, FILE *F);


/************************************************************************************
*
*          FUNCTION FOR REPRESENTING REALS IN [0,1] AS A COLOR
*
************************************************************************************/
GLOB void gfmMap0_1ToColor(GLfloat *C, double x);
	
/* ***********************************************************************************/
/*  */
/*                     LEGEND FUNCTIONS */
/*  */
/* ***********************************************************************************/
GLOB void gfmCopyLegendSettings(LegendSettings *From, LegendSettings *To);
GLOB void gfmDrawLegend(int N, char **Names, clipping_struct *Clips, ColorScheme3f *C,
	LegendSettings *L);
GLOB void gfmZapTheLegend(window_settings *WS);

/* ***********************************************************************************/
/*  */
/*                  FOR RENDERING TEXT ON THE SCREEN */
/*  */
/* ***********************************************************************************/
GLOB void gfmStrokeString(char *s, GLfloat h, GLfloat x, GLfloat y,  int On, 
				GLfloat r, GLfloat WoH);
GLOB void gfmRenderBitmapString(float x, float y, void *font, char *string);
GLOB void gfmRenderBitmapCharacters(float x, float y, void *font, char *string,
	 int Num);
	 
	 
/* ***********************************************************************************/
/*  */
/*    FOR COLLECTING STRING INPUT FROM THE USER THROUGH A WINDOW */
/*  */
/* ***********************************************************************************/
GLOB void gfmAtCreationOfStringInput(void);
GLOB void gfmStringInputKeyboardFunc(unsigned char key, int x, int y);
GLOB void gfmHandleStringInput(char *M1,char *M2, char *M3, char *String,
	void (*FinishUpFunc)(void) );
GLOB void gfmDrawStringInputWindow(void);
GLOB void gfmDefineStringInputWindow(void);

/* ***********************************************************************************/
/*  */
/*    FUNCTIONS FOR POSITIONING, DRAWING, AND SELECTING BAR GRAPHS */
/*  */
/* ***********************************************************************************/
GLOB GLfloat gfmBarGraphYCoord(int n, GLfloat h, GLfloat ys, int i);
GLOB GLfloat gfmBarGraphXCoord(int n, GLfloat w, GLfloat xs, int i);
GLOB int gfmBarGraphIdxFromXY(int n, int N, GLfloat w, GLfloat h, GLfloat xs, GLfloat ys,
					double X, double Y);
GLOB void gfmSetParsForBarGraphs(int N, int C, GLfloat xs, GLfloat ys,
						int *n, GLfloat *w, GLfloat *h);
GLOB void gfmDrawRectLeaf6f(GLfloat x,GLfloat y,GLfloat w,GLfloat h, 
							GLfloat value, GLfloat total,GLfloat *Color);



GLOB void gfmAppendToFile(char *FileName, char *String);



/************************************************************************************
*
*    		ULTRA-SIMPLE SLIDER BAR CONTROLS
*
************************************************************************************/
void gfmDrawVertIntSlider(void);
void gfmDrawVertDoubleSlider(void);
void gfmSliderBarNormalKeys(unsigned char key, int x, int y);



/* ***********************************************************************************/
/*  */
/*     						 AXES */
/*  */
/* ***********************************************************************************/
GLOB void gfmDrawXaxis(void);
GLOB void gfmDrawYaxis(void);
GLOB void gfmCopyXYAxisStructs(XY_axis_struct *From, XY_axis_struct *To);
GLOB void gfmDetermineMajorTicksX(void);
GLOB void gfmDrawTicksX(void);
GLOB void gfmDetermineMajorTicksY(void);
GLOB void gfmDrawTicksY(void);
GLOB void gfmDrawXAxisNumLabels(void);
GLOB void gfmDrawYAxisNumLabels(void);
GLOB void gfmDrawXYAxes(void);


/* ********************** */
/* 	Predefined Colors */
/* ***********************/
/*  here is a function that the program itself will define and write */
/*  out so that developers can put predefined color schemes in as hard-wired */
/*  code.  It goes at the end 'cuz it is long and ugly */
GLOB void gfmColorSchemePredefiningFunc(ColorScheme3f *CS[MAX_CS+1],int *NumC, int Menu );



#endif




