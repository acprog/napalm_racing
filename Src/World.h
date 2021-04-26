/* naPalm Racing

  Copyright (C) 2004

  Author: Alexander Semenov <acmain@gmail.com>
*/
#ifndef _WORLD_H_
#define _WORLD_H_

#include "cfg.h"
#include "fixed.h"
#include "car.h"
#include "ground.h"
#include "camera.h"
#include "exterior.h"
#include "image.h"
#include "flash_image.h"
#include "flat3d.h"
#include "database.h"

#include "list"
using std::list;

typedef	list<object*>::iterator	obj_iter;
typedef	list<unit*>::iterator	unit_iter;
typedef	list<car*>::iterator		car_iter;

struct player_prefs;

extern	enum world_state
{
	move_to_car,
	wait_3,
	wait_2,
	wait_1,
	wait_go,
	racing,
	finish,
	move_out_car,
//	pause
}state;

//=====================================================
//	������� ���
//=====================================================
class world
{
private:
	list<object*>	objects;	// ��� �������
	list<unit*>		units;	// ������������ �������
	list<car*>		cars;	// ������
		// � �������� ����� ���������� ��� ���������� �������������

	image	minimap;

	//-------------------------------------------------------
	// ������� ����: ����+�����������
	database		*world_base;

	//	����+�������� �� ���
	class sky_image : public res_image
	{
	private:
		animation	**anims;
		int			n_anims;
	
	public:
		sky_image(UInt16 id, database *db);
		virtual ~sky_image();
		void draw(color *screen, Angle angle);
		void animate(const real &time);
	}sky;

	// ����������� ���� � ������������ �������
	ground	water_join,
			unused_water,	// � �������������� ����������� ��-���������
			bridge_join,
			unused_bridge,
			grass_join,
			grass,
			road_join0,
			road_join3,
			sand,
			unused_sand,
			road_join1,
			unused_road,
			special_join,	// ������� ���������� � ��� ������ ����������
			special,
			road_join2,
			road_marks0;

	//-------------------------------------------------------
	
	exterior	devil;

	car		*player;
	camera	cam;
	
	res_image	fps,
					speed,
					lap,
					position;
	flash_image	big_three,
					big_two,
					big_one,
					big_go,
					big_finish;
	font			numbers,
					small_numbers,
					*active_font;

//=====================================================
public:
	world(db_info *track, int n_laps, player_prefs *players, int n_players, int opponents_skill, bool demo=false);
	virtual	~world();
	void draw(color *screen);
	void draw_fps(color *screen);
	void process(const real &time);	// ������ ����� (���)
	void load_track(db_info *track);
	void change_handing(float dh);	// ��������� ������������� � ���� ����� (� ����)
	
	//=========================================================
	// ����������
	inline	void analog_gear(const real &gear)
	{
		player->need_gear(gear);
	}
	
	//---------------------------------------------------------
	inline	void analog_rudder(const real &rudder)
	{
		player->need_d_angle(rudder);
	}

	//---------------------------------------------------------
	inline	void free_gear()
	{
		player->no_gear();
	}

	//---------------------------------------------------------
	inline	void free_rudder()
	{
		player->need_d_angle(0);
	}
	
	//---------------------------------------------------------
	inline	void reset_car()
	{
		player->reset();
	}

	friend class object;
	friend class unit;
	friend class car;
	friend void configuration_form();
};


#endif	// _WORLD_H_