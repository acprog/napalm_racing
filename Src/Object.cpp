/* naPalm Racing

  Copyright (C) 2004

  Author: Alexander Semenov <acmain@gmail.com>
*/
#include "object.h"
#include "matrix.h"
#include "cfg.h"
#include "camera.h"
#include "world.h"

//=========================================================
//	Пассивный объект
//=========================================================
object::object(world *w, exterior *_ext, const Vector &loc, Int8 _color, Angle _angle)
	:vector	(loc)
	,ext		(_ext)
	,angle	(_angle)
{
	paint(_color);
	if (w)
		w->objects.push_back(this);
}

//======================================================
void object::draw(color *screen, matrix<> m, const camera &cam, long _z)
{
	Vector	offset(*this);
	offset-=cam;

	m*=matrix<>(offset, angle);

	ext->draw(screen, m, (atan(cam.x-x, cam.y-y)-angle)>>13, _z, Color);
}