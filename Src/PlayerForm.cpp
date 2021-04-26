/* naPalm Racing

  Copyright (C) 2004

  Author: Alexander Semenov <acmain@gmail.com>
*/

#include <PalmOS.h>

#include "Flat3D.h"
#include "Flat3DRsc.h"

#include "fixed.h"

#include "world.h"

static	car		*player_car;
static	exterior	devil(&car_high, &car_med, &car_low, &car_solid);
static	preview		*cam;
static	image		*scr_img=NULL;
static	BitmapType	*scr_bmp=NULL;
static	player_prefs	prefs;

//=================================================================
static	void init_preview()
{
	prefs.Color=g_prefs.player.Color;
	player_car=new car(NULL, &devil, &prefs, true);
	cam=new preview(player_car, 20, 60);

	if (!g_prefs.safe_mode)
	{
		scr_img=new image(size<>(SCREEN_WIDTH, 60));
		scr_img->draw_type=image::Put;
	}
	else
	{
		Err	error;
		scr_bmp=BmpCreate(SCREEN_WIDTH, 60, COLOR_SIZE, NULL, &error);
	}
}

//=================================================================
static	void destroy_preview()
{
	delete cam;
	delete player_car;
	if (scr_img)
		delete scr_img;
	if (scr_bmp)
		BmpDelete(scr_bmp);
}


//=================================================================
static	void car_preview(real time)
{
	cam->process(time);

	if (!g_prefs.safe_mode)
	{
		scr_img->clear(0);
		cam->draw((color*)(*scr_img));
		color	*screen=(color*)BmpGetBits(WinGetBitmap(WinGetDisplayWindow()));
		scr_img->draw(screen, point<>(0, 20));
	}
	else
	{
		color	*p=(color*)BmpGetBits(scr_bmp);
		for (int i=0; i<BPP(SCREEN_WIDTH*60); i++)
			p[i]=0;
		cam->draw(p);
		WinDrawBitmap(scr_bmp, 0, 20);
	}	
}



//=================================================================
static bool select_control(UInt16 id)
{
	switch (id)
	{
	case PlayerBackButton:
		FrmGotoForm(prev_form_id);
		return true;
		
	case PlayerButtonsPushButton:
		g_prefs.analog_control=false;
		return true;
			
	case PlayerGraffitiPushButton:
		g_prefs.analog_control=true;
		return true;
		
	case PlayerColorSliderControl:
		g_prefs.player.Color=CtlGetValue(GetObjectPtr<ControlType>(PlayerColorSliderControl));
		player_car->paint(g_prefs.player.Color);
		return true;
	}

	return false;
}


//=====================================================================
Boolean PlayerFormHandleEvent(EventType * eventP)
{
	static	bool	preview=false;
	static	UInt32	ticks;

	if (preview)
	{
		car_preview(real(TimGetTicks()-ticks)/=SysTicksPerSecond());
		ticks=TimGetTicks();
	}

	switch (eventP->eType)
	{
	case ctlSelectEvent:
		return select_control(eventP->data.ctlSelect.controlID);
    	
	case frmOpenEvent:
		ticks=TimGetTicks();
		preview=true;
		init_preview();

		FormType	*frmP=FrmGetActiveForm();
		
		FieldType	*fldP=GetObjectPtr<FieldType>(PlayerNameField, frmP);
		FldDelete(fldP, 0, FldGetMaxChars(fldP));
		FldInsert(fldP, g_prefs.player.name, StrLen(g_prefs.player.name));

		FrmSetControlGroupSelection(frmP, 1, g_prefs.analog_control ? PlayerGraffitiPushButton : PlayerButtonsPushButton);
 	 	CtlSetValue(GetObjectPtr<ControlType>(PlayerColorSliderControl, frmP), g_prefs.player.Color);
		FrmDrawForm(frmP);
		return true;
		
	case frmCloseEvent:
		prev_form_id=PlayerForm;
		destroy_preview();
		preview=false;
		StrCopy(g_prefs.player.name, FldGetTextPtr(GetObjectPtr<FieldType>(PlayerNameField)));
		return false;
 	}
 	 	
	return false;
}