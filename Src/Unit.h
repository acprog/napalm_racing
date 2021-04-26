/* naPalm Racing

  Copyright (C) 2004

  Author: Alexander Semenov <acmain@gmail.com>
*/
#ifndef	_UNIT_H_
#define	_UNIT_H_

#include "object.h"
#include "fixed.h"
#include "image.h"
#include "cfg.h"

//=========================================================
//	Активный объект
//=========================================================
class unit : public object
{
protected:
	point<int>	speed;// x - по продольной оси, y - по поперечной объекта
	int	accelerate,
			d_angle,
			weight;			// в кг
	bool	ricochet_move;	// рикошет от барьера

	//-------------------------------------------------------------
	// уменьшение скорости от трения
	int to_slow_down(int speed, int f)
	{
		if (speed>0)
			return (speed>f ? speed-f : 0);
		else
			return (speed<-f ? speed+f : 0);
	}

	//=========================================================
public:
	unit(world *w, exterior *_ext, const Vector &loc, Int8 _color=0, Angle _angle=0);
	virtual	~unit(){}
	virtual	void process(const real	&time, int f);	// прошло времени (сек)
	virtual	void friction(const real &f);	// f - коэф-т трения поверхности
	virtual	void draw_info(color *screen, font *f, bool selected=false)=0;
	void	impact(unit *p);	// проверка столкновения объектов
	void	push(const solid::atom &a, const point<long> &v);	// столкновение атомов
	virtual	void ricochet();

/*
	//---------------------------------------------------------
	virtual	bool ricochet();
	{
		if (ricochet_move)
			return false;
		return	ricochet_move=true;
	}
/**/
	friend class world;
	friend class ground;
};

#endif	//_UNIT_H_