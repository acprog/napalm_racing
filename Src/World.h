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
	list<car*>		cars;	// ������������ �������
		// � �������� ����� ���������� ��� ���������� �������������

	class sky_image : public res_image
	{
	public:
		sky_image(UInt16 id)
			:res_image(id){};
		virtual ~sky_image(){};
		void draw(color *screen, Angle angle);
	}sky;

	image	minimap;

	// �����������
	anim_ground		water_join;
	static_ground	grass_join,
						sand,
						special,
						// ������
						bridge,
						road,
						br_road;
	// ��� �������� �������������� ����������
	// ����������� �� ground
	ground			special2,
						water;
	static_ground	grass;

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

	//=====================================================================
//	void draw_sky(UInt32	*screen);
	
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