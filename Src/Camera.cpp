/* naPalm Racing

  Copyright (C) 2004

  Author: Alexander Semenov <acmain@gmail.com>
*/
#include "camera.h"
#include "world.h"
#include "Flat3D.h"

//=========================================================
//	Точка наблюдения
//=========================================================
// камера смонтирована на объект
camera::camera(object *_look_to, int _z, int _distance, Angle _look_angle)
	:object(NULL, NULL, Vector(0, 0, _z))
	,d_angle(0)
{
	make_projection();
	if (_look_to)
		mount(_look_to, _z, _distance, _look_angle);
}


//=========================================================
// неподвижная камера. в центре - объект
camera::camera(object *_look_to, const Vector &loc)
	:object		(NULL, NULL, loc)
	,look_angle	(0)
	,distance	(0)
	,look_to	(_look_to)
	,mount_to_obj	(false)
	,d_angle	(0)
{
	make_projection();
}


//=========================================================
// свободная камера
camera::camera(const Vector &loc, Angle _look_angle)
	:object		(NULL, NULL, loc)
	,look_angle	(_look_angle)
	,distance	(0)
	,look_to	(NULL)
	,mount_to_obj	(false)
	,d_angle	(0)
{
	make_projection();
}


//=========================================================
void camera::make_projection()
{
	// матрица проекции
	projection*=matrix<float>(Vector(SCREEN_WIDTH/2, SKY_HEIGHT-SKIP_UP, 0));
	projection*=matrix<float>(volume<float>(-1, 1, 1));
	projection*=matrix<float>(-C);
	projection*=matrix<float>(Vector(0, 0, D0));
	projection*=matrix<float>(ANGLE_PI/2, 0, 0);
	projection*=matrix<float>(0, 0, ANGLE_PI/2);
	projection*=matrix<float>(volume<float>(1, 1, -1));	// масштаб
}


//=========================================================
void camera::mount(object *_look_to, int _z, int _distance, Angle _look_angle)
{
	// установка параметров
	z=_z;
	look_angle=_look_angle;
	distance=_distance;
	look_to=_look_to;
	mount_to_obj=true;
	
	// пререндеринг
	matrix<float>	m(projection);
	m*=matrix<float>(Vector(distance, 0, -_z));
	look_to->ext->transform(m, VIEW_FROM_BACK, distance*distance);
}



//=========================================================
void camera::process(const real &time)
{
	if (d_angle)
		look_angle+=time*d_angle;
	angle=look_to->angle+look_angle;
	x=look_to->x-COS(angle)*distance;
	y=look_to->y-SIN(angle)*distance;
}


//=========================================================
void camera::draw(color *screen, list<object*> &objects)
{
	// параметры объектов для сортировки
	static	struct
	{
		object	*obj;
		long		z;
	}to_draw[MAX_OBJECTS],	*p;
	int	n_to_draw=0;
	long	z;
	
	static	Vector	offset;
	Angle	view_angle;

	for (obj_iter i=objects.begin(); i!=objects.end(); i++)
	{
		offset=*(*i);
		offset-=(*this);
		z=sqr((long)offset.x)+sqr((long)offset.y)+sqr((long)offset.z);
		
		// проверка попадания обьекта в видимую область
		if (z<VISIBLE_DISTANCE && z>VISIBLE_HIGHT)
		{
			view_angle=atan(offset.x, offset.y)-angle;
			if (view_angle<VISIBLE_ANGLE || view_angle>-VISIBLE_ANGLE)
			{
				// вставка объекта в отсортированную очередь
				for (p=to_draw+n_to_draw++; p-- > to_draw; p[1]=p[0])
					if (p->z <= z)
						break;
				p++;
				p->obj=(*i);
				p->z=z;
			}
		}
	}

	//-----------------------------------------------------------
	// здесь озвучить ближайший объект

	//-----------------------------------------------------------
	// отрисовка
	if (n_to_draw==1 && mount_to_obj && look_to->z==0)
		look_to->ext->render(screen, look_to->Color);	// единственный объект в поле зрения
	else
	{
		// камера
		matrix<float>	cameraview(projection);
		cameraview*=matrix<float>(0, 0, -angle);

		matrix<>	cam_matrix(cameraview);

		// рисование обьектов
		if (!mount_to_obj || look_to->z>0)
			for (int i=n_to_draw; i--;)
				to_draw[i].obj->draw(screen, cam_matrix, *this, to_draw[i].z);
		else
			for (int i=n_to_draw; i--;)
				if (to_draw[i].obj==look_to)
					look_to->ext->render(screen, look_to->Color);
				else
					to_draw[i].obj->draw(screen, cam_matrix, *this, to_draw[i].z);
	}
}






//=========================================================
//	Предпросмотр
//=========================================================
preview::preview(object *_look_to, int _z, int _distance, Angle _d_angle)
	:camera(_look_to, _z, _distance)
{
	// смещение сектора обзора наверх
	matrix<float>	m(Vector(0, -110));
	m*=projection;
	m_fast=m;
	
	d_angle=_d_angle;
	look_to->angle=(ANGLE_PI)+ANGLE_PI/6;
	real_angle=(ANGLE_PI)+ANGLE_PI/6;
}


//=========================================================
void preview::process(const real &time)
{
	// вращаем не камеру а объект
	look_to->angle+=time*d_angle;
	look_to->angle=real_angle+=time*d_angle;
	x=look_to->x-COS(angle)*distance;
	y=look_to->y-SIN(angle)*distance;
}


//=========================================================
void preview::draw(color *screen)
{
	look_to->draw(screen, m_fast, *this, VISIBLE_HIGHT);
}