/* naPalm Racing

  Copyright (C) 2004

  Author: Alexander Semenov <acmain@gmail.com>
*/

#include <PalmOS.h>

#include "Flat3D.h"
#include "Flat3DRsc.h"

#include "world.h"

#include "fixed.h"

//==========================================================
world	*World=NULL;
static	bool	pause=true;

// исходные параметры кнопок
static	UInt16	initDelay,
						period,
						doubleTapDelay;
static	Boolean	queueAhead;






//=====================================================================
void	configuration_form()
{
	FormType *frmP=FrmInitForm(ConfigForm);

	FrmSetControlGroupSelection(frmP, 1, g_prefs.analog_control ? ConfigGraffitiPushButton : ConfigButtonsPushButton);
  	CtlSetValue(GetObjectPtr<ControlType>(ConfigRudderSpeedSliderControl, frmP), g_prefs.rudder_speed);
  	CtlSetValue(GetObjectPtr<ControlType>(ConfigHandingSliderControl, frmP), g_prefs.handing);
  	CtlSetValue(GetObjectPtr<ControlType>(ConfigMuteSoundCheckbox, frmP), g_prefs.mute_sound);
  	CtlSetValue(GetObjectPtr<ControlType>(ConfigShowFPSCheckbox, frmP), g_prefs.show_fps);

	if (FrmDoDialog(frmP)==ConfigDoneButton)
	{
		g_prefs.analog_control=CtlGetValue(GetObjectPtr<ControlType>(ConfigGraffitiPushButton, frmP));
		g_prefs.rudder_speed=CtlGetValue(GetObjectPtr<ControlType>(ConfigRudderSpeedSliderControl, frmP));
		g_prefs.mute_sound=CtlGetValue(GetObjectPtr<ControlType>(ConfigMuteSoundCheckbox, frmP));
		g_prefs.show_fps=CtlGetValue(GetObjectPtr<ControlType>(ConfigShowFPSCheckbox, frmP));
		
		// изменение управляемости у машин
		float old_handing=g_prefs.handing;
		g_prefs.handing=CtlGetValue(GetObjectPtr<ControlType>(ConfigHandingSliderControl, frmP));
		World->change_handing((float)g_prefs.handing/old_handing);
	}

	FrmDeleteForm (frmP);
}






//========================================================================
static Boolean MainFormDoCommand(UInt16 command)
{
    Boolean handled = false;
    FormType * frmP;

    switch (command)
    {
    case OptionsConfiguration:
   		MenuEraseStatus(0);
   		configuration_form();
		return true;

	case	OptionsLeaveTrack:
		FrmGotoForm(prev_form_id);
		return true;

	case	OptionsResetCar:
		World->reset_car();
		return true;

	case	OptionsControls:
		FrmHelp(ControlsString);
		return true;
	}
    
    return handled;
}






//================================================================
Boolean MainFormHandleEvent(EventType * eventP)
{
	Boolean handled = false;
	FormType * frmP;
	static	UInt32	ticks=0;
	static	WinHandle	win=(WinHandle)FrmGetFormPtr(MainForm);
	static	BitmapType	*screen_for_os5=NULL;
	static	Err	error;

	switch (eventP->eType)
	{
	case	winExitEvent:
		if (eventP->data.winExit.exitWindow==win)
			pause=true;
		break;

	case	winEnterEvent:
		if (eventP->data.winEnter.enterWindow==win && World)
		{
			ticks=TimGetTicks();
			pause=false;
		}
		break;

	case menuEvent:
		return MainFormDoCommand(eventP->data.menu.itemID);

	case frmOpenEvent:
		if (g_prefs.safe_mode)
			screen_for_os5=BmpCreate(SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_SIZE, NULL, &error);
		
		frmP = FrmGetActiveForm();
		FrmDrawForm(frmP);
//		prev_form_id=MainForm;
		handled = true;
		break;
		
	case frmCloseEvent:
		pause=true;
		delete World;
		World=NULL;
		if (screen_for_os5)
		{
			 BmpDelete(screen_for_os5);
			 screen_for_os5=NULL;
		}
		break;
            
	case frmUpdateEvent:
		// To do any custom drawing here, first call 
		// FrmDrawForm(), then do your drawing, and 
		// then set handled to true.
		break;

	default:
		break;
	}
	
	if (!pause)
	{
		World->process(real(TimGetTicks()-ticks)/=SysTicksPerSecond());
		ticks=TimGetTicks();
		
		if (!g_prefs.safe_mode)
		{
			World->draw((color*)WinScreenLock(winLockDontCare));
			WinScreenUnlock();
		}
		else if (screen_for_os5)
		{
			World->draw((color*)BmpGetBits(screen_for_os5));
			WinDrawBitmap(screen_for_os5, 0, 0);
		}
	}
    
   return handled;
}



//=====================================================================
bool system_event(EventType *event)
{
	if (pause)
		return SysHandleEvent(event);

	EvtResetAutoOffTimer();
	
	if (g_prefs.analog_control)
	{
		//------------------------------------------------------------------
		// управление по граффити
		static bool	pen_lock=false;
		static RectangleType	graffiti={ {GRAFFITI_LEFT, GRAFFITI_TOP}, {GRAFFITI_WIDTH, GRAFFITI_HEIGHT} };

		if (event->eType==penDownEvent && RctPtInRectangle(event->screenX, event->screenY, &graffiti))
			pen_lock=true;
	
		if (event->penDown && pen_lock)
		{
			real	rudder, gear;

			if (event->screenX<GRAFFITI_LEFT)
				rudder=-1;
			else	if (event->screenX>GRAFFITI_RIGHT)
				rudder=1;
			else
				rudder=real(event->screenX-GRAFFITI_LEFT-GRAFFITI_WIDTH/2)/=(GRAFFITI_WIDTH/2);
			World->analog_rudder(rudder);
					
			if (event->screenY>GRAFFITI_TOP)
				gear=real(GRAFFITI_HEIGHT*2/3+GRAFFITI_TOP-event->screenY)/=(GRAFFITI_HEIGHT*2/3);
			else
				gear=1;
			World->analog_gear(gear);
			return false;
		}else	if (event->eType==penUpEvent && pen_lock)
		{
			pen_lock=false;
			World->free_gear();
			World->free_rudder();
			return false;
		}
	}
	//-----------------------------------------------------------
	//	перехват кнопок
	else	if	(event->eType==keyDownEvent && 
		(	(event->data.keyDown.chr>=vchrHard1	&& event->data.keyDown.chr<=vchrHard4)
		||	event->data.keyDown.chr==vchrPageUp
		||	event->data.keyDown.chr==vchrPageDown))
			return false;
	
	return SysHandleEvent(event);
}