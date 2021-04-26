/* naPalm Racing

  Copyright (C) 2004

  Author: Alexander Semenov <acmain@gmail.com>
*/

#ifndef	_EXTERIOR_H_
#define	_EXTERIOR_H_

#include "model.h"

//=========================================================
//	Внешние параметры объекта	(вроде-бы и ненужный класс)
//=========================================================
class exterior
{
private:
	int	radius;	// потом заменить массив пар dx, radius
	// 3 модели объекта разной детализации
	model	*high,
			*medium,
			*low;
	solid	*Solid;
	
	// для быстрого рендеринга одной модели
	static	projected_point immovable[MAX_VERTEXES];
	static	Int8	immov_sector;
	static	model	*immov_model;
	
	//======================================================
public:
	exterior(model *_high, model *_medium, model *_low, solid *_Solid);
	virtual	~exterior(){}

	//----------------------------------------------------------------
	inline	model *detail(long z)
	{
		if (z>=VISIBLE_LOW)
			return low;
		else if (z>=VISIBLE_MED)
			return medium;
		else
			return high;
	}

	//----------------------------------------------------------------
	inline	void draw(color *screen, const matrix<> &m, Int8 sector, long z, Int8 color)
	{
		static	projected_point projection[MAX_VERTEXES];
		model		*mod=detail(z);

		mod->transform(m, sector, projection);
		mod->render(screen, sector, projection, color);
	}

	//----------------------------------------------------------------
	//	преобразование вершин одного неподвижного объекта для последующей быстрой отрисовки
	inline	void transform(const matrix<> &m, Int8 sector, long z)
	{
		immov_model=detail(z);
		immov_model->transform(m, immov_sector=sector, immovable);
	}
	
	//----------------------------------------------------------------
	//	быстрый рендеринг одного неподвижного объекта
	inline	void render(color *screen, Int8 color)
	{
		immov_model->render(screen, immov_sector, immovable, color);
	}
	
	friend class unit;
};


#endif	//_EXTERIOR_H_