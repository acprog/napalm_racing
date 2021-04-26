#include <PalmOS.h>

#include "Flat3D.h"
#include "Flat3DRsc.h"
#include "world.h"


static	player_prefs	random_players[MAX_PLAYERS],
						*players;
static	int	n_players;

//=====================================================================
void make_players(player_prefs *p, int n, bool demo)
{
	for (int i=0; i<n; i++, p++)
	{
		p->id=(i==n-1 ? 0 : i+1);
		p->Color=SysRandom(0)%(MAX_ADD_COLOR+1);
		p->position=i+1;
		p->points=0;
	}
}


//=====================================================================
void start_race(int track, int n_laps, int n, int skill, player_prefs *p, bool demo)
{
	if (!p)
	{
		p=random_players;
		make_players(p, n, demo);
	}
	players=p;
	n_players=n;
	
	if (!demo)	// поиск игрока и замена цвета машины
		for (int i=0; i<n; i++)
			if (p[i].id==0)
			{
				p[i].Color=g_prefs.player.Color;
				break;
			}
	
	World=new world(
		tracks_names[track],
		n_laps,
		players,
		n_players,
		skill,
		demo
		);

	FrmGotoForm(MainForm);
}


//=================================================================
static bool select_control(UInt16 id)
{
	switch (id)
	{
	case RaceResultsDoneButton:
		FrmGotoForm(prev_form_id);
		prev_form_id=RaceResultsForm;
		return true;
	}

	return false;
}


//=====================================================================
Boolean RaceResultsFormHandleEvent(EventType * eventP)
{
	switch (eventP->eType)
	{
	case ctlSelectEvent:
		return select_control(eventP->data.ctlSelect.controlID);
    	
	case frmOpenEvent:
		FormType	*frmP=FrmGetActiveForm();
		player_prefs	*p=players;
		for (int i=0; i<n_players; i++, p++)
			FrmCopyLabel(frmP, RaceResultsFirstLabel+p->position-1, p->id==0 ? g_prefs.player.name : comp_names[p->id]);
		FrmDrawForm(frmP);
		return true;
	}
	return false;
}