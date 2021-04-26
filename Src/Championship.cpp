/* naPalm Racing

  Copyright (C) 2004

  Author: Alexander Semenov <acmain@gmail.com>
*/
#include <PalmOS.h>

#include "Flat3D.h"
#include "Flat3DRsc.h"
#include "world.h"

#include "algorithm.h"
using namespace std;

int points[]=
{
	0,
	30,
	25,
	21,
	17,
	13,
	9,
	6,
	3,
	0
};


static	char *players_list[9];


//=================================================================
static	void redraw_info()
{
	FormType	*frmP=FrmGetActiveForm();
	FrmCopyLabel(frmP, ChampionshipPlayerSelTrigger, g_prefs.player.name);
	char str[10];
	StrPrintF(str, "%d/%d", g_prefs.championship.curr_track+1, n_tracks<8 ? n_tracks : 8);
	FrmCopyLabel(frmP, ChampionshipTrackNLabel, str);
	select_track(g_prefs.championship.curr_track, ChampionshipTrackNameLabel);

	player_prefs	*p=g_prefs.championship.players;
	for (int i=0; i<g_prefs.championship.n_comps+1; i++)
		StrPrintF(players_list[i], "%d %s", p[i].points, p[i].id==0 ? g_prefs.player.name : comp_names[p[i].id]);
	ListType	*players=GetObjectPtr<ListType>(ChampionshipPlayersList, frmP);
	LstSetListChoices(players, players_list, g_prefs.championship.n_comps+1);
	LstDrawList(players);
}


//=================================================================
static	void end_of_championship()
{
	FormType	*frmP=FrmInitForm(ChampsResultsForm);
	player_prefs	*p=g_prefs.championship.players;
	for (int i=0; i<g_prefs.championship.n_comps+1; i++, p++)
		FrmCopyLabel(frmP, ChampsResultsFirstButton+i, p->id==0 ? g_prefs.player.name : comp_names[p->id]);

	FrmDoDialog(frmP);
	FrmDeleteForm (frmP);
	new_championship();
}


//=================================================================
void new_championship(bool redraw)
{
	if (redraw)
	{
		FormType	*frmP=FrmInitForm(NewChampsForm);

		FrmSetControlGroupSelection(frmP, 1, NewChampsTwoLapsPushButton+g_prefs.championship.n_laps-2);
		FrmSetControlGroupSelection(frmP, 3, NewChampsOnePushButton+g_prefs.championship.n_comps-1);
		FrmSetControlGroupSelection(frmP, 2, NewChampsNovicePushButton+g_prefs.championship.comp_skill);

		if (FrmDoDialog(frmP)==NewChampsDoneButton)
		{
			g_prefs.championship.n_laps=FrmGetObjectId(frmP, FrmGetControlGroupSelection(frmP, 1))-NewChampsTwoLapsPushButton+2;
			g_prefs.championship.n_comps=FrmGetObjectId(frmP, FrmGetControlGroupSelection(frmP, 3))-NewChampsOnePushButton+1;
			g_prefs.championship.comp_skill=FrmGetObjectId(frmP, FrmGetControlGroupSelection(frmP, 2))-NewChampsNovicePushButton;
		}
	}

	g_prefs.championship.curr_track=0;
	make_players(g_prefs.championship.players, g_prefs.championship.n_comps+1);
	
	if (redraw)
		redraw_info();
}


//=================================================================
static bool select_control(UInt16 id)
{
	switch (id)
	{
	case ChampionshipBackButton:
		FrmGotoForm(MenuForm);
		return true;

	case ChampionshipPlayerSelTrigger:
		FrmGotoForm(PlayerForm);
		return true;

	case ChampionshipRestartButton:
		new_championship();
		return true;

	case ChampionshipRaceButton:
		start_race(	g_prefs.championship.curr_track,
					g_prefs.championship.n_laps,
					g_prefs.championship.n_comps+1,
					g_prefs.championship.comp_skill,
					g_prefs.championship.players);
		return true;
	}

	return false;
}


//=====================================================================
Boolean ChampionshipFormHandleEvent(EventType * eventP)
{
	switch (eventP->eType)
	{
	case ctlSelectEvent:
		return select_control(eventP->data.ctlSelect.controlID);

	case frmOpenEvent:
		for (int i=0; i<9; i++)
			players_list[i]=new char[60];
		if (prev_form_id==RaceResultsForm)	// проехана трасса
		{
			int n=++g_prefs.championship.curr_track;

			player_prefs	*p=g_prefs.championship.players;
			for (int i=0; i<g_prefs.championship.n_comps+1; i++)
				p[i].points+=points[p[i].position+(8-g_prefs.championship.n_comps)];

			// сортировка игроков по очкам
			for (int i=0; i<g_prefs.championship.n_comps; i++)
				for (int j=i+1; j<g_prefs.championship.n_comps+1; j++)
					if (p[j].points>p[i].points)
						swap(p[j], p[i]);

			if (n==n_tracks || n==8)	// конец чемпионата
				end_of_championship();
		}

		redraw_info();
		FrmDrawForm(FrmGetActiveForm());
		return true;
		
	case frmCloseEvent:
		for (int i=0; i<9; i++)
			delete[] players_list[i];
		prev_form_id=ChampionshipForm;
		return false;
 	}
	return false;
}