/* naPalm Racing

  Copyright (C) 2004

  Author: Alexander Semenov <acmain@gmail.com>
*/

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
				kmh_speed;
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

// fatal error message
extern	void error(const char *module, int line, const char *fmt);//, ...);
#define	ERROR(args)	error(__FILE__, __LINE__, args)

#define	MSG		FrmAlert(MsgAlert)
#define	MSG1(one)	FrmCustomAlert(MsgAlert, one, NULL, NULL)
#define	MSG2(one, two)	FrmCustomAlert(MsgAlert, one, two, NULL)
#define	MSG3(one, three)	FrmCustomAlert(MsgAlert, one, two, three)

// ********************************************************************
// Global variables
// ********************************************************************
extern	UInt16	prev_form_id;

extern	Flat3DPreferenceType g_prefs;
//extern bool	audio_driver;
extern	class world	*World;

extern	int	n_tracks;

// ********************************************************************
// Internal Constants
// ********************************************************************
#define TRACK_CREATOR			'ACnr'
#define TRACK_TYPE				'Trek'
#define WORLD_TYPE				'Wrld'

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

#define SandBitmapFamily                          20800
#define WaterBitmapFamily                         20000
#define BrRoadBitmapFamily                        21000
#define GrassJoinBitmapFamily                     20400
#define RoadBitmapFamily                          20600
#define SkyBitmapFamily                           10000
#define SpecialBitmapFamily                       21200
#define BridgeBitmapFamily                        20200
#define GrassBitmapFamily                         20500
#define Road3BitmapFamily                         20700
#define Road4BitmapFamily                         21400
#define RoadMarks0BitmapFamily                    21500
/*
#define SandBitmapFamily                          1500
#define WaterBitmapFamily                         1200
#define BrRoadBitmapFamily                        2000
#define GrassJoinBitmapFamily                     1300
#define RoadBitmapFamily                          1000
#define SkyBitmapFamily                           1100
#define SpecialBitmapFamily                       1900
#define BridgeBitmapFamily                        2100
#define GrassBitmapFamily                         2200
#define Road3BitmapFamily                         20000
#define Road4BitmapFamily                         20100
#define RoadMarks0BitmapFamily                    20200
/**/

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
