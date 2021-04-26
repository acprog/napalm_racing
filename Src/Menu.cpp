#include <PalmOS.h>

#include "Flat3D.h"
#include "Flat3DRsc.h"

#include "world.h"

extern	bool	os5_available;

//=================================================================
static	void options_form()
{
	FormType *frmP=FrmInitForm(OptionsForm);

  	CtlSetValue(GetObjectPtr<ControlType>(OptionsSmallMapCheckbox, frmP), g_prefs.minimap.small);
  	CtlSetValue(GetObjectPtr<ControlType>(OptionsTransparentMapCheckbox, frmP), g_prefs.minimap.transparent);

	if (FrmDoDialog(frmP)==OptionsBackButton)
	{
		g_prefs.minimap.transparent=CtlGetValue(GetObjectPtr<ControlType>(OptionsTransparentMapCheckbox, frmP));
		g_prefs.minimap.small=CtlGetValue(GetObjectPtr<ControlType>(OptionsSmallMapCheckbox, frmP));
	}

	FrmDeleteForm (frmP);
}


//=================================================================
static bool select_control(UInt16 id)
{
	switch (id)
	{
	case MenuSingleRaceButton:
        FrmGotoForm(SingleRaceForm);
		return true;

	case MenuChampionshipButton:
/*
		if (n_tracks<8)
			FrmAlert(NeedMoreTracksAlert);
		else*/
			FrmGotoForm(ChampionshipForm);
		return true;

	case MenuDemoButton:
		start_race(	SysRandom(0)%n_tracks, MIN_LAPS,
					SysRandom(0)%(MAX_PLAYERS-2)+2,
					COMP_AMATEUR, NULL, true);
//		demo_mode();
		return true;
		
	case MenuOptionsButton:
		options_form();
		return true;
		
	case MenuSafeModeCheckbox:
		g_prefs.safe_mode=!g_prefs.safe_mode;
		return true;
	}

	return false;
}


//=====================================================================
Boolean MenuFormHandleEvent(EventType * eventP)
{
	switch (eventP->eType)
	{
	case ctlSelectEvent:
    	return select_control(eventP->data.ctlSelect.controlID);
    	
	case frmOpenEvent:
//		SndPlaySmfResource('tSMF', 2001, prefGameSoundVolume);
		FormType	*frmP=FrmGetActiveForm();
		FrmCopyLabel(frmP, MenuBuildDateLabel, __DATE__);
		CtlSetValue(GetObjectPtr<ControlType>(MenuSafeModeCheckbox, frmP), g_prefs.safe_mode);
		FrmDrawForm(frmP);
		prev_form_id=MenuForm;
		return true;
 	}
	return false;
}




//=====================================================================
void DefaultCfg()
{
	g_prefs.rudder_speed=1;
	g_prefs.handing=12;
	g_prefs.analog_control=true;
	g_prefs.mute_sound=true;
	g_prefs.small_font=true;
	g_prefs.show_fps=false;
	g_prefs.kmh_speed=true;

	g_prefs.safe_mode=os5_available;

	g_prefs.minimap.small=true;
	g_prefs.minimap.transparent=true;
	
	g_prefs.single_race.n_opponents=2;
	g_prefs.single_race.opponents_skill=COMP_AMATEUR;
	g_prefs.single_race.n_laps=MIN_LAPS;
	g_prefs.single_race.curr_track=0;

	g_prefs.player.Color=0;
	StrCopy(g_prefs.player.name, 
	#ifdef LANG_RUS
		"Игрок");
	#endif
	#ifdef LANG_ENG
		"Player");
	#endif
	
	// чемпионат
	g_prefs.championship.comp_skill=COMP_AMATEUR;
	g_prefs.championship.n_laps=MIN_LAPS;
	g_prefs.championship.n_comps=3;

	new_championship(false);
}

