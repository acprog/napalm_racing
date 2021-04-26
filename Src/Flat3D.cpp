// Flat3DMain.c
//
// main file for Flat3D
//
// This wizard-generated code is based on code adapted from the
// stationery files distributed as part of the Palm OS SDK 4.0.
//
// Copyright (c) 1999-2000 Palm, Inc. or its subsidiaries.
// All rights reserved.

#include <PalmOS.h>
//#include <CWCallbackThunks.h>

#include "Flat3D.h"
#include "Flat3DRsc.h"

#include "fixed.h"

// ********************************************************************
// Entry Points
// ********************************************************************

// ********************************************************************
// Global variables
// ********************************************************************

// g_prefs
// cache for application preferences during program execution
Flat3DPreferenceType g_prefs;
//bool	audio_driver;
UInt16	prev_form_id=MenuForm;

cpu_type	current_cpu;
bool	os5_available;

// MainFormHandleEventThunk
//     holds callback thunk for main form event handler

//static _CW_CallbackThunk MainFormHandleEventThunk;

// ********************************************************************
// Internal Constants
// ********************************************************************

// Define the minimum OS version we support
#define os5Version    	 sysMakeROMVersion(5,0,0,sysROMStageDevelopment,0)
#define ourMinVersion    sysMakeROMVersion(3,5,0,sysROMStageDevelopment,0)
#define kPalmOS10Version sysMakeROMVersion(1,0,0,sysROMStageRelease,0)

// ********************************************************************
// Internal Functions
// ********************************************************************


// FUNCTION: AppHandleEvent
//
// DESCRIPTION: 
//
// This routine loads form resources and set the event handler for
// the form loaded.
//
// PARAMETERS:
//
// event
//     a pointer to an EventType structure
//
// RETURNED:
//     true if the event was handled and should not be passed
//     to a higher level handler.

static Boolean AppHandleEvent(EventType * eventP)
{
    UInt16 formId;
    FormType * frmP;

    if (eventP->eType == frmLoadEvent)
    {    	
        // Load the form resource.
        formId = eventP->data.frmLoad.formID;
        frmP = FrmInitForm(formId);
        FrmSetActiveForm(frmP);

        // Set the event handler for the form.  The handler of the 
        // currently active form is called by FrmHandleEvent each 
        // time is receives an event.
        switch (formId)
        {
        case MainForm:
            FrmSetEventHandler(frmP, (FormEventHandlerType *)&MainFormHandleEvent/*Thunk*/);
            break;

        case MenuForm:
            FrmSetEventHandler(frmP, MenuFormHandleEvent);
            break;

        case SingleRaceForm:
            FrmSetEventHandler(frmP, SingleRaceFormHandleEvent);
            break;

        case PlayerForm:
            FrmSetEventHandler(frmP, PlayerFormHandleEvent);
            break;

        case TrackForm:
            FrmSetEventHandler(frmP, TrackFormHandleEvent);
            break;

        case ChampionshipForm:
            FrmSetEventHandler(frmP, ChampionshipFormHandleEvent);
            break;

        case RaceResultsForm:
            FrmSetEventHandler(frmP, RaceResultsFormHandleEvent);
            break;

        default:
            break;

        }
        return true;
    }

    return false;
}

// FUNCTION: AppEventLoop
//
// DESCRIPTION: This routine is the event loop for the application.
static void AppEventLoop(void)
{
	UInt16 error;
   EventType event;
  
   do
   {
		EvtGetEvent(&event, evtNoWait);
		if (! system_event(&event))
			if (! MenuHandleEvent(0, &event, &error))
				if (! AppHandleEvent(&event))
					FrmDispatchEvent(&event);
    }
    while (event.eType != appStopEvent);
}

// FUNCTION: AppStart
//
// DESCRIPTION:  Get the current application's preferences.
//
// RETURNED:
//     errNone - if nothing went wrong

static Err AppStart(void)
{
    UInt16 prefsSize;

    // Read the saved preferences / saved-state information.
    prefsSize = sizeof(Flat3DPreferenceType);
    if (PrefGetAppPreferences(
        appFileCreator, appPrefID, &g_prefs, &prefsSize, true) == noPreferenceFound
        || g_prefs.size!=sizeof(Flat3DPreferenceType))
    {
   		g_prefs.size=sizeof(Flat3DPreferenceType);
   		// FIXME: setup g_prefs with default values
   		DefaultCfg();
    }
    
    // Setup main form event handler callback thunk (needed for "expanded" mode)
//    _CW_GenerateCallbackThunk(MainFormHandleEvent, &MainFormHandleEventThunk);

	initFixMath();

	UInt32	requiredDepth=0,
			width=160,
			height=160;

	#ifdef COLOR_GRAY
		requiredDepth = 4;
	#endif

	#ifdef COLOR_8
		requiredDepth = 8;
	#endif

	#ifdef COLOR_16
		requiredDepth = 16;
	#endif
	
	Err err = WinScreenMode(winScreenModeSet, &width, &height, &requiredDepth, NULL);
	if (err)
	{
		FrmAlert(NeedMoreColorsAlert);
		return err;
	}

	make_tracks_list();

	if (n_tracks==0)	
	{
		FrmAlert(NoTracksAlert);
		return appErrorClass;
	}
	/*
	// CPU
	UInt32	ticks=TimGetTicks(),
			i, j=0, a=0;
	for (i=0; i<100000L; i++)
		for (j=0; j<100000L; j++)
			a++;
	ticks=TimGetTicks()-ticks;
#define	ABS(x)	((x)>0 ? (x) : -(x))
	if (ABS(ticks-32)<ABS(ticks-16))
		current_cpu=mhz_16;
	else
		current_cpu=mhz_33;
/*
	char str[64];
	StrPrintF(str, "%d", (int)ticks);
	FrmCustomAlert(MsgAlert, str, NULL, NULL);
/**/	
    return errNone;
}

// FUNCTION: AppStop
//
// DESCRIPTION: Save the current state of the application.

static void AppStop(void)
{
    // Write the saved preferences / saved-state information.  This 
    // data will be saved during a HotSync backup.
    PrefSetAppPreferences(
        appFileCreator, appPrefID, appPrefVersionNum, 
        &g_prefs, sizeof(Flat3DPreferenceType), true);
        
    // Close all the open forms.
   FrmCloseAllForms();
//   main_form_destroy();
	destroy_tracks_list();

	destroyFixMath();
}

// all code from here to end of file should use no global variables
//#pragma warn_a5_access on

// FUNCTION: RomVersionCompatible
//
// DESCRIPTION: 
//
// This routine checks that a ROM version is meet your minimum 
// requirement.
//
// PARAMETERS:
//
// requiredVersion
//     minimum rom version required
//     (see sysFtrNumROMVersion in SystemMgr.h for format)
//
// launchFlags
//     flags that indicate if the application UI is initialized
//     These flags are one of the parameters to your app's PilotMain
//
// RETURNED:
//     error code or zero if ROM version is compatible

static Err RomVersionCompatible(UInt32 requiredVersion, UInt16 launchFlags)
{
    UInt32 romVersion;

    // See if we're on in minimum required version of the ROM or later.
    FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);
    if (romVersion < requiredVersion)
    {
        if ((launchFlags & 
            (sysAppLaunchFlagNewGlobals | sysAppLaunchFlagUIApp)) ==
            (sysAppLaunchFlagNewGlobals | sysAppLaunchFlagUIApp))
        {
            // Palm OS 1.0 will continuously relaunch this app unless 
            // we switch to another safe one.
            if (romVersion <= kPalmOS10Version)
            {
                AppLaunchWithCommand(
                    sysFileCDefaultApp, 
                    sysAppLaunchCmdNormalLaunch, NULL);
            }
        }

        return sysErrRomIncompatible;
    }

    return errNone;
}

// FUNCTION: Flat3DPalmMain
//
// DESCRIPTION: This is the main entry point for the application.
//
// PARAMETERS:
//
// cmd
//     word value specifying the launch code. 
//
// cmdPB
//     pointer to a structure that is associated with the launch code
//
// launchFlags
//     word value providing extra information about the launch
//
// RETURNED:
//     Result of launch, errNone if all went OK

static UInt32 Flat3DPalmMain(
    UInt16 cmd, 
    MemPtr /*cmdPBP*/, 
    UInt16 launchFlags)
{
    Err error;

    error = RomVersionCompatible (ourMinVersion, launchFlags);
    if (error)
    {
        FrmAlert(RomIncompatibleAlert);
    	return error;
    }
    
    UInt32 romVersion;
    switch (cmd)
    {
    case sysAppLaunchCmdNormalLaunch:
        FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);
    	os5_available=(romVersion>=os5Version);

        error = AppStart();
        if (error)
            return error;

        // start application by opening the main form
        // and then entering the main event loop
        FrmGotoForm(MenuForm);
        AppEventLoop();
        
        AppStop();
        break;

    default:
        break;
    }

    return errNone;
}

// FUNCTION: PilotMain
//
// DESCRIPTION: This is the main entry point for the application.
// 
// PARAMETERS:
//
// cmd
//     word value specifying the launch code. 
//
// cmdPB
//     pointer to a structure that is associated with the launch code
//
// launchFlags
//     word value providing extra information about the launch.
//
// RETURNED:
//     Result of launch, errNone if all went OK

UInt32 PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags)
{
    return Flat3DPalmMain(cmd, cmdPBP, launchFlags);
}

// turn a5 warning off to prevent it being set off by C++
// static initializer code generation
#pragma warn_a5_access reset
