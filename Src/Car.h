/* naPalm Racing

  Copyright (C) 2004

  Author: Alexander Semenov <acmain@gmail.com>
*/

#ifndef	_CAR_H_
#define	_CAR_H_

#include "unit.h"

struct player_prefs;

//=========================================================
//	������
//=========================================================
class car : public unit
{
private:
	long	top_speed;
	int		top_accelerate,
			back_accelerate,
			braking,
			handing;
	bool	comp;
	int		target_point,
			lap,		// ����
			position;	// ����� (������, ������, ���)
	long	distance;	// ������� ���������� �� checkpoint-a
	static	int	n_cars,
				n_laps,
				next_position;	// �����, ������� ������ ���������
								// �����, ������ �� ������
	player_prefs	*prefs;
	static	res_image	final_lap,
						rotate_back;
						
	Angle	ang_to_target;

	//-----------------------------------------------------------
	// ����
	static	point<>	waypoints[];
	static	struct	way_params
	{
		int	n;
	}way;
	
	//=========================================================
public:
	car(world *w, exterior *_ext, player_prefs *_prefs, bool human_player=true, float skill=1.0f);
	virtual	~car();
	void no_gear()		{accelerate=0;}
	void need_gear(const real &_gear);
	void need_d_angle(real _d_angle)	{ d_angle=_d_angle*=handing; }
	void draw_info(color *screen, font *f, bool selected=false);
	void friction(const real &f);
	void process(const real &time, int f);	// ������ ������� (���)
	void sound(const real &time);
	void reset();	//	����� � ����������� waypoint-�
	void ricochet();	// ������� �� �������
	void ai_control(const real &time, real &angle_k, const point<real> &to_target);	// ���������� ���������
	
	static	void load_way(MemHandle h, point<real> &p, Angle &a);	// ���������� ����� "�����" � ���������� ����
	void place(point<real> &p, Angle a);	// ���������� ������� �� ������

	//------------------------------------------------------------
	// �������� ��������� � �������, ������ ����������� ������� � p
	inline	bool hit(point<real> &to_target, const rect<real> &space)
	{
		to_target-=*this;
		return (space.order(to_target)==point<>::inside);
	}

	//--------------------------------------------------------------
	//	��������� ������� ��� ����������
	inline	static	bool compare_positions(car *c1, car *c2)
	{
		if (c1->lap!=c2->lap)
			return c1->lap>c2->lap;

		if (c1->target_point!=c2->target_point)
			return c1->target_point>c2->target_point;
		
		return c1->distance<c2->distance;
	}
	
	
	friend class world;
};

#endif	//_PLAYER_H_