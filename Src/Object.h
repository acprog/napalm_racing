/* naPalm Racing

  Copyright (C) 2004

  Author: Alexander Semenov <acmain@gmail.com>
*/

#ifndef	_OBJECT_H_
#define	_OBJECT_H_

#include "exterior.h"
#include "fixed.h"


class camera;
class world;

//=========================================================
//	Пассивный обьект
//=========================================================
class object : public Vector
{
protected:
	exterior	*ext;
	Angle		angle;
	color		Color;

	//======================================================
public:
	object(world *w, exterior *_ext, const Vector &loc, Int8 _color=0, Angle _angle=0);
	virtual ~object(){}
	virtual void draw(color *screen, matrix<> m, const camera &cam, long _z);
	
	//-----------------------------------------------
	inline	void paint(UInt8 fill)
	{
	#ifdef COLOR_GRAY
		Color=fill|(fill<<4);
	#endif

	#ifdef COLOR_8
		Color=fill;
	#endif
	}
	
	friend class camera;
	friend class preview;
};

#endif	//_OBJECT_H_