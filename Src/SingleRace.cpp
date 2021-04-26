/* naPalm Racing

  Copyright (C) 2004

  Author: Alexander Semenov <acmain@gmail.com>
*/
#include <PalmOS.h>

#include "Flat3D.h"
#include "Flat3DRsc.h"
#include "world.h"


//=====================================================================
#ifdef LANG_RUS
static	char	*opponents_str[]=
{
	"Нету: и без них справлюсь",
	"Один",
	"Двое",
	"Трое",
	"Четыре",
	"Пять",
	"Шесть",
	"Семь",
	"Восемь"
};

char	*skills_str[]={"Начинающий", "Любитель", "Мастер"};

char	*comp_names[]=
{
	"Игрок",
	"Шустрый",
	"Газ",
	"Вега",
	"Бродяга",
	"Ветер дорог",
	"Селена",
	"Рыжий",
	"Танк"
};

#endif

#ifdef LANG_ENG
static	char	*opponents_str[]=
{
	"Not",
	"One",
	"Two",
	"Three",
	"Four",
	"Five",
	"Six",
	"Seven",
	"Eight"
};

char	*skills_str[]={"Novice", "Amateur", "Master"};

char	*comp_names[]=
{
	"Player",
	"Fast",
	"Spider",
	"Vega",
	"Rambler",
	"Wind of roads",
	"Selena",
	"Mad",
	"Tank"
};

#endif



//=====================================================================
static	void update_opponents(FormType *frmP)
{
	char	str[128];
	if (g_prefs.single_race.n_opponents)
		StrPrintF(str, 
	#ifdef LANG_RUS
		"%s. Навык: %s", 
	#endif		
	#ifdef LANG_ENG
		"%s. Skill: %s", 
	#endif		
		opponents_str[g_prefs.single_race.n_opponents], skills_str[g_prefs.single_race.opponents_skill]);
	else
		StrCopy(str, opponents_str[g_prefs.single_race.n_opponents]);
	FrmCopyLabel(frmP, SingleRaceOpponentsSelTrigger, str);
}


//=====================================================================
static	void	opponents_form()
{
	FormType *frmP=FrmInitForm(OpponentsForm);

	FrmSetControlGroupSelection(frmP, 1, OpponentsNotPushButton+g_prefs.single_race.n_opponents);
	FrmSetControlGroupSelection(frmP, 2, OpponentsNovicePushButton+g_prefs.single_race.opponents_skill);

	if (FrmDoDialog(frmP)==OpponentsBackButton)
	{
		g_prefs.single_race.n_opponents=FrmGetControlGroupSelection(frmP, 1)-3;
		g_prefs.single_race.opponents_skill=FrmGetControlGroupSelection(frmP, 2)-13;
	}

	FrmDeleteForm (frmP);
}


//=================================================================
static bool select_control(UInt16 id)
{
	switch (id)
	{
	case SingleRaceTrackSelTrigger:
		FrmGotoForm(TrackForm);
		return true;

	case SingleRaceOpponentsSelTrigger:
		opponents_form();
		FormPtr	frmP=FrmGetActiveForm();
		update_opponents(frmP);
		FrmDrawForm(frmP);
		return true;

	case SingleRacePlayerSelTrigger:
		FrmGotoForm(PlayerForm);
		return true;

	case SingleRaceBackButton:
		FrmGotoForm(MenuForm);
		return true;

	case SingleRaceGoButton:
		start_race(	g_prefs.single_race.curr_track,
					g_prefs.single_race.n_laps,
					g_prefs.single_race.n_opponents+1,
					g_prefs.single_race.opponents_skill);
		return true;
	}

	return false;
}


//=====================================================================
Boolean SingleRaceFormHandleEvent(EventType * eventP)
{
	switch (eventP->eType)
	{
	case ctlSelectEvent:
		return select_control(eventP->data.ctlSelect.controlID);
    	
	case frmOpenEvent:
		prev_form_id=SingleRaceForm;

		char	str[128];
		FormType	*frmP=FrmGetActiveForm();
		StrPrintF(str, 
		#ifdef LANG_RUS
			"%s, %d кругов", 
		#endif		
		#ifdef LANG_ENG
			"%s, %d laps", 
		#endif		
			tracks_names[g_prefs.single_race.curr_track], g_prefs.single_race.n_laps);
		FrmCopyLabel(frmP, SingleRaceTrackSelTrigger, str);
		FrmCopyLabel(frmP, SingleRacePlayerSelTrigger, g_prefs.player.name);
		update_opponents(frmP);
		FrmDrawForm(frmP);
		return true;
		
	case frmCloseEvent:
		return false;
 	}
	return false;
}