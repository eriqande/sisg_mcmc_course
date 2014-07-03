/*  these are to be used internally in later #defines */
#define _gsNWD					gNumWinDefs
#define _gsIF   				if(gInitWinSettings[gNumWinDefs]==NULL) { gInitWinSettings[gNumWinDefs] = gfmAllocWindowSettings();  }
#define _gsIWS  				gInitWinSettings[gNumWinDefs]
#define _gsALLOC(X)  			gWinDefs[X] = gfmAllocWindowStruct();
#define _gsID(X,Y)   			gWinDefs[X]->WindowID = Y;
#define _gsRID(X)				gWinIDReverse[X] = _gsNWD;
#define _gsTITLE(X,Y)   		sprintf(gWinDefs[X]->Title,Y);
#define _gsIF_NO_MENU  			if(gCreateNewWindowsSubMenu==0) { gCreateNewWindowsSubMenu = glutCreateMenu(gfmCreateWindowByIndex); }
#define _gsAME(X)  				_gsIF_NO_MENU  glutAddMenuEntry(gWinDefs[X]->Title, gWinDefs[X]->WindowID );
#define _gsPAD  				_gsIWS->Fits->Padding				


/*  this should be called in UserDefdDefineWindows to name the predefined views File */
/*  and the other output files with a prefix telling us which application */
/*  they came from */
#define gsOUTPUT_FILE_PREFIX(X)  sprintf(gPredDefdViewsFileName,"%s_PreDefdViews.txt",X); \
								 sprintf(gUserDefdViewsFileName,"%s_UserDefdViews.txt",X);\
								 sprintf(gUserDefdColorSchemesFileName,"%s_UDColorSchemes.txt",X);


/*  the two macros below are now obsolete, but I keep them in here */
/*  commented out anyway. */
/* #define gsPREDEFINED_VIEWS(X)  sprintf(gPredDefdViewsFileName,X); */
/* #define gsUSER_DEFINED_VIEWS(X)  sprintf(gUserDefdViewsFileName,X); */


/*  use to set up the console window */
#define gsCONSOLE(Tit)   _gsALLOC(0)    _gsTITLE(0,Tit)     _gsID(0,0)  _gsAME(0)

/*  use to set up a non-console window */
#define gsNEW_WINDOW(X,Tit)  _gsNWD++;  _gsALLOC(_gsNWD)  _gsID(_gsNWD,X) \
					 _gsRID(X)  _gsTITLE(_gsNWD,Tit) _gsAME(_gsNWD)

/*  two required ones for each window */
#define gsDRAW_FUNC(X)   gWinDefs[gNumWinDefs]->DrawFunc = X;


/*  for defining the extra function to call when instating the window */
#define gsEXTRA_FUNC(X)  gWinDefs[gNumWinDefs]->ExtraFunction = X;
#define gsKEYBOARD_FUNC(X)  gWinDefs[gNumWinDefs]->KeyboardFunc = X;



/*  these are for customizing the initial settings for the windows */
/*  clipping volume customization */
#define gsXLO(X)    _gsIF    _gsIWS->Clips->xlo = (GLfloat)(X);
#define gsXHI(X)    _gsIF    _gsIWS->Clips->xhi = (GLfloat)(X);
#define gsYLO(X)    _gsIF    _gsIWS->Clips->ylo = (GLfloat)(X);
#define gsYHI(X)    _gsIF    _gsIWS->Clips->yhi = (GLfloat)(X);
#define gsZLO(X)    _gsIF    _gsIWS->Clips->zlo = (GLfloat)(X);
#define gsZHI(X)    _gsIF    _gsIWS->Clips->zhi = (GLfloat)(X);

/*  for customizing the fitting parameters */
#define gsPADDING(XL,XH,YL,YH)		 _gsIF _gsPAD->xlo=(GLfloat)(XL); \
									 _gsPAD->xhi=(GLfloat)(XH); _gsPAD->ylo=(GLfloat)(YL); \
									 _gsPAD->yhi = (GLfloat)(YH); 

/*  axis customization */
#define gsY_AXIS_CROSSES(X)  _gsIF    _gsIWS->Yaxis->ForceThroughPoint = 1;  _gsIWS->Yaxis->Intersect = (GLfloat)(X);
#define gsX_AXIS_CROSSES(X)  _gsIF    _gsIWS->Xaxis->ForceThroughPoint = 1;  _gsIWS->Xaxis->Intersect = (GLfloat)(X);

#define gsY_AXIS_LO(X)   	_gsIF    _gsIWS->Yaxis->ImposeLowerLimit = 1;  _gsIWS->Yaxis->LowerLimit = (GLfloat)(X);
#define gsX_AXIS_LO(X)   	_gsIF    _gsIWS->Xaxis->ImposeLowerLimit = 1;  _gsIWS->Xaxis->LowerLimit = (GLfloat)(X);

#define gsY_AXIS_HI(X)   	_gsIF    _gsIWS->Yaxis->ImposeUpperLimit = 1;  _gsIWS->Yaxis->UpperLimit = (GLfloat)(X);
#define gsX_AXIS_HI(X)   	_gsIF    _gsIWS->Xaxis->ImposeUpperLimit = 1;  _gsIWS->Xaxis->UpperLimit = (GLfloat)(X);

#define gsXTICKS_BELOW		_gsIF	 _gsIWS->Xaxis->TickPos = AXIS_TICKS_LEFT_BELOW;
#define gsXTICKS_ABOVE		_gsIF	 _gsIWS->Xaxis->TickPos = AXIS_TICKS_RIGHT_ABOVE;

#define gsYTICKS_LEFT		_gsIF	 _gsIWS->Yaxis->TickPos = AXIS_TICKS_LEFT_BELOW;
#define gsYTICKS_RIGHT		_gsIF	 _gsIWS->Yaxis->TickPos = AXIS_TICKS_RIGHT_ABOVE;

#define gsXTICKS_MAJOR_SIZE(X)		_gsIF	 _gsIWS->Xaxis->MajorProp = (GLfloat)X;
#define gsYTICKS_MAJOR_SIZE(X)		_gsIF	 _gsIWS->Yaxis->MajorProp = (GLfloat)X;

#define gsX_MAJOR_NUM(X)		_gsIF	 _gsIWS->Xaxis->OptimalNumMajor = X;
#define gsY_MAJOR_NUM(X)		_gsIF	 _gsIWS->Yaxis->OptimalNumMajor = X;

#define gsNO_XAXIS_NUMS			_gsIF	_gsIWS->Xaxis->DrawNumLabels = 0;
#define gsNO_YAXIS_NUMS			_gsIF	_gsIWS->Yaxis->DrawNumLabels = 0;

#define gsNO_Y_AXIS				_gsIF	_gsIWS->Yaxis->DrawIt = 0;
#define gsNO_X_AXIS				_gsIF	_gsIWS->Xaxis->DrawIt = 0;

#define gsAXES_POS_QUADRANT		gsY_AXIS_CROSSES(0.0f)  gsX_AXIS_CROSSES(0.0f) \
								gsY_AXIS_LO(0.0f) gsX_AXIS_LO(0.0f) gsXTICKS_BELOW gsYTICKS_LEFT
#define gsAXES_POS_UNIT_X	 	gsAXES_POS_QUADRANT  gsX_AXIS_HI(1.0f);

/*  for legend customization */
#define gsNUM_COLOR_KEYS(X) 	gWinDefs[_gsNWD]->NumColorKeys = X;
#define gsCOLOR_KEYS(X)			gWinDefs[_gsNWD]->ColorKey = X;
/* in the following, X should be one of 
 LEGEND_RIGHT, LEGEND_LEFT, LEGEND_TOP, or LEGEND_BOTTOM*/
 #define gsSET_LEGEND(X)         _gsIF _gsIWS->Legend->DrawLegend = 1;    _gsIWS->Legend->Location = X;


/*  menu attachment, etc */
#define gsMIDDLE_CLICK_MENU(X)  gWinDefs[_gsNWD]->MiddleMouseMenuIdx = X;

/*  for setting the default color scheme */
#define gsCOLOR_SCHEME(X) 	 	gWinDefs[_gsNWD]->DefaultColorSchemeIdx = X;

/*  something for setting a generic window setting to a particular value */
#define gsSET(X,Y)    		_gsIF	_gsIWS->X = Y;

/* a macro for making a slider control window for integers*/
#define gsINT_SLIDER(X,Y,LO,HI)  gsDRAW_FUNC(gfmDrawVertIntSlider) gsKEYBOARD_FUNC(gfmSliderBarNormalKeys)	gsSET(SliderHigh,(double)HI) \
		gsSET(SliderLow,(double)LO) gsSET(IntSlider,&Y) gsYHI(1.1*(double)HI) gsYLO(-.1*(double)HI) gsXLO(-.1) gsXHI(1.6) sprintf(_gsIWS->SlideName,"%s",X); \
		gsSET(YCoord,(double)Y)


#define gsDOUBLE_SLIDER(X,Y,LO,HI)  gsDRAW_FUNC(gfmDrawVertDoubleSlider) gsKEYBOARD_FUNC(gfmSliderBarNormalKeys)	gsSET(SliderHigh,(double)HI) \
		gsSET(SliderLow,(double)LO) gsSET(DoubleSlider,&Y) gsYHI(1.1*(double)HI) gsYLO(-.1*(double)HI) gsXLO(-.1) gsXHI(1.6) sprintf(_gsIWS->SlideName,"%s",X); \
		gsSET(YCoord,(double)Y)



/*  call at the beginning of a drawing function to set all the variable name pointers */
/*  to the ones for your window */
#define gsDECLARE_CURRENT_WINDOW_VARS   int CW = glutGetWindow();\
										clipping_struct *Clips = gWindowsSettings[CW]->Clips;\
										ColorScheme3f *CS = gWindowsSettings[CW]->ColorScheme;\
										XY_axis_struct *XA = gWindowsSettings[CW]->Xaxis;\
										XY_axis_struct *YA = gWindowsSettings[CW]->Yaxis;\
										fitting_struct *Fits = gWindowsSettings[CW]->Fits;\
										GLfloat WoH = gfmWoH();\
										window_settings *Settings = gWindowsSettings[CW];
										

/* unfortunately, under gcc with the -Wall option, gsDECLARE_CURRENT_WINDOW_VARS may
	create a number of unused variable warnings if you don't actually use the variables
	so, instead, you should just issue the following macros separately for declaring those
	variables that will be used  */
#define gsClips		 	clipping_struct *Clips = gWindowsSettings[glutGetWindow()]->Clips;
#define gsCS			ColorScheme3f *CS = gWindowsSettings[glutGetWindow()]->ColorScheme;
#define gsXA		  	XY_axis_struct *XA = gWindowsSettings[glutGetWindow()]->Xaxis;
#define gsYA			XY_axis_struct *YA = gWindowsSettings[glutGetWindow()]->Yaxis;
#define gsFits			fitting_struct *Fits = gWindowsSettings[glutGetWindow()]->Fits;
#define	gsWoH			GLfloat WoH = gfmWoH();
#define gsSettings		window_settings *Settings = gWindowsSettings[glutGetWindow()];



/*  These are for putting in the drawing functions themselves to handle setting */
/*  the extrema */
#define _gsFITS gWindowsSettings[glutGetWindow()]->Fits
#define _gsFITS_E _gsFITS->Extrema
#define gsHANDLE_EXTREMA(XL,XH,YL,YH)    if(_gsFITS->FitToExtremaNow==1) { \
			_gsFITS_E->xlo = (GLfloat)XL; _gsFITS_E->xhi = (GLfloat)XH;    \
			_gsFITS_E->ylo = (GLfloat)YL; _gsFITS_E->yhi = (GLfloat)YH;    \
			_gsFITS->ExtremaSet = 1; }





