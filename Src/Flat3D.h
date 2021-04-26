// Flat3D.h
//
// header file for Flat3D
//
// This wizard-generated code is based on code adapted from the
// stationery files distributed as part of the Palm OS SDK 4.0.
//
// Copyright (c) 1999-2000 Palm, Inc. or its subsidiaries.
// All rights reserved.

#ifndef FLAT3D_H_
#define FLAT3D_H_

#include "cfg.h"
#include "PalmOS.h"

// ********************************************************************
// Internal Structures
// ********************************************************************
struct	player_prefs
{
	color	Color;
	Int8	id,	// номер имени: 0 - человек, 1-8 компьютеры
			position;
	Int16	points;	// очки
};

struct	db_info
{
	char	name[40];
	UInt16 	card;
}*tracks_names[];

extern	char	*comp_names[];


//================================================================
typedef struct Flat3DPreferenceType
{
	UInt16	size;					// размер структуры
	Int8		rudder_speed,		// скорость поворота руля
				handing;		// управляемость машины
	bool		analog_control;	// управление по граффити
	bool		mute_sound;			// убрать звук
	bool		small_font;			// мелкий шрифт на экране
	bool		show_fps,
				kmh_speed,
				safe_mode;		// безопасный режим
	struct		// маленькая карта
	{
		bool		small;		
		bool		transparent;	// прозрачная
	}minimap;

	struct
	{
		//char	track_name[32];
		int		curr_track;
		Int8	n_opponents,
				n_laps,
				opponents_skill;
	}single_race;

	struct
	{
		char	name[60];
		color	Color;
	}player;

	struct
	{
		player_prefs	players[9];
		Int8	comp_skill,
				n_comps,
				n_laps;
		int		curr_track;	// текущая трасса
	}championship;
} Flat3DPreferenceType;


// ********************************************************************
// Internal Functions
// ********************************************************************
Boolean MainFormHandleEvent(EventType * eventP);
Boolean MenuFormHandleEvent(EventType * eventP);
Boolean SingleRaceFormHandleEvent(EventType * eventP);
Boolean PlayerFormHandleEvent(EventType * eventP);
Boolean TrackFormHandleEvent(EventType * eventP);
Boolean ChampionshipFormHandleEvent(EventType * eventP);
Boolean RaceResultsFormHandleEvent(EventType * eventP);
void new_championship(bool redraw=true);
bool system_event(EventType *eventP);
void DefaultCfg();
void make_tracks_list();
void destroy_tracks_list();
void make_players(player_prefs *p, int n_players, bool demo=false);
void start_race(int track, int n_laps, int n, int skill, player_prefs *p=NULL, bool demo=false);
db_info *select_track(int &tr, UInt16 label);


// ********************************************************************
// Global variables
// ********************************************************************
extern	UInt16	prev_form_id;

extern	Flat3DPreferenceType g_prefs;
//extern bool	audio_driver;
extern	class world	*World;

extern	int	n_tracks;

//extern	bool	os5_available;

// ********************************************************************
// Internal Constants
// ********************************************************************
#define TRACK_CREATOR			'ACnr'
#define TRACK_TYPE				'Trek'

#define appFileCreator			'ACnr'
#define appName					"Napalm Racing"
#define appVersionNum			0x01
#define appPrefID				0x00
#define appPrefVersionNum		0x01



#define OptionsMenu                               1000
#define OptionsConfiguration                      1000
#define OptionsLeaveTrack                         1001
#define OptionsResetCar                           1002
#define OptionsControls 						  1003

// ********************************************************************
// Helper template functions
// ********************************************************************

template <class T>
typename T * GetObjectPtr(UInt16 id, FormType *frmP=NULL)
{
	if (!frmP)
		frmP = FrmGetActiveForm();
	return static_cast<T *>(
		FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, id)));
}


//========================================================
template <class T>
inline	T sqr(T x)
{
	return x*x;
}


#endif // FLAT3D_H_
