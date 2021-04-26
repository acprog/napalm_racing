/* naPalm Racing

  Copyright (C) 2004

  Author: Alexander Semenov <acmain@gmail.com>
*/
#include "unit.h"
#include "fixed.h"
#include "cfg.h"
#include "camera.h"
#include "world.h"
#include "flat3D.h"

//=========================================================
//	�������� ������
//=========================================================
unit::unit(world *w, exterior *_ext, const Vector &loc, Int8 _color, Angle _angle)
	:object	(w, _ext, loc, _color, _angle)
	,d_angle			(0)
	,accelerate		(0)
	,ricochet_move	(false)
{
	if (w)
		w->units.push_back(this);
}


//=========================================================
void unit::process(const real &time, int f)
{
	if (ricochet_move)
	{
		accelerate=0;
		d_angle=0;
	}

	if (d_angle)
	{	
		Angle	a=d_angle;
		angle+=a;

		// ���������� ������ �������� �� -a
		point<int>	v(speed);
		speed.x=SIN(a)*v.y+COS(a)*v.x;
		speed.y=COS(a)*v.y-SIN(a)*v.x;
	}

	if (!ricochet_move)
	{
		speed.x+=time*accelerate;
		friction(time*f);
	}
	
	// ���������� ������� ��������, ����������� �� angle
	x+=(COS(angle)*speed.x-SIN(angle)*speed.y)/100;
	y+=(SIN(angle)*speed.x+COS(angle)*speed.y)/100;

	// ����������� �� �����
	static	real 	zero=0,
						max_coord=MAX_COORD;
	if (x<zero || x>max_coord || y<zero || y>max_coord)
		ricochet();
}


//=========================================================
void unit::friction(const real &f)
{
	speed.x=to_slow_down(speed.x, f);
	speed.y=to_slow_down(speed.y, f);
}


//=========================================================
void unit::impact(unit *p)
{
	if (z>0 || p->z>0)
		return;	// ������ �������� - ����������
	
	point<long>	v(p->x, p->y);
	// ������ �� this �� p
	v.x-=(long)x;
	v.y-=(long)y;
	long	d=sqr(v.x)+sqr(v.y);

	if (d>sqr(ext->Solid->r+p->ext->Solid->r))
		return;	// ���������� ������������� � ������
		
	point<long>	t_v,
				p_v;
	int	i, j;
	long	d_min;	// ���������� ���������� ���������� ����� �������
	for (i=0; i<p->ext->Solid->n; i++)
	{
		// ������ �� ���� ������� p
		p_v.x=COS(p->angle)*p->ext->Solid->atoms[i].d;
		p_v.y=SIN(p->angle)*p->ext->Solid->atoms[i].d;
		p_v+=v;
		for (j=0; j<ext->Solid->n; j++)
		{
			// ������ �� ���� ������� this
			t_v.x=COS(angle)*ext->Solid->atoms[j].d;
			t_v.y=SIN(angle)*ext->Solid->atoms[j].d;
			// ������ ����� �� p � this
			t_v-=p_v;
			// ������� ��������� �������������
			d=sqr(t_v.x)+sqr(t_v.y);
			d_min=sqr(ext->Solid->atoms[j].r+p->ext->Solid->atoms[i].r);
			if (d<d_min && d>0)
			{	// ����!!
				// ���� ����� ��������������� �������������
				t_v*=(d_min-d);
				t_v/=d;
				push(ext->Solid->atoms[j], t_v);
				t_v*=-1;
				p->push(p->ext->Solid->atoms[i], t_v);
			}
		}
	}		
}


//=========================================================
void	unit::push(const solid::atom &a, const point<long> &v)
{
	// ������ ����� � ��� (���������� �� -angle)
	point<long>	p(
		SIN(angle)*v.y+COS(angle)*v.x,
		COS(angle)*v.y-SIN(angle)*v.x
	);
	
	// ��������� ��������
	speed.x+=p.x*PUSH_X;
	speed.y+=p.y*PUSH_Y;
	
	// ��������� �������� ���������
	angle+=a.d*p.y*PUSH_ANGLE;
}


//=========================================================
void unit::ricochet()
{
	if (ricochet_move)
		return;
	ricochet_move=true;
	
	#define	ABS(x)	((x)>0 ? (x) : -(x))
	#define	SIGN(x)	((x)>0 ? 1 : -1)

	if (ABS(speed.x)<100*HARD_OF_BORDER)
		speed.x=100*HARD_OF_BORDER*SIGN(speed.x);

	if (ABS(speed.y)<100*HARD_OF_BORDER)
		speed.y=100*HARD_OF_BORDER*SIGN(speed.y);

	speed/=-HARD_OF_BORDER;
}
