/* naPalm Racing

  Copyright (C) 2004

  Author: Alexander Semenov <acmain@gmail.com>
*/

#ifndef	_MODEL_H_
#define	_MODEL_H_

#include "cfg.h"
#include "Fixed.h"
#include "geometric.h"

#define	VIEW_FROM_BACK	8

// ������������ ����
typedef	vector<Int8>	Vertex;
typedef	struct
{
	UInt8	color;
	Int8	vertex_id[4];	// ���� id == -1, �� ��� ���
}Polygon;

class	camera;

#include "matrix.h"

//============================================================
//	����� ��������������� �� �����
//============================================================
class projected_point : public point<Int16>
{
private:
	bool	out_screen;
	
public:
	projected_point(){};

	//---------------------------------------------------------------
	inline	void transform(const matrix<> &m, const Vertex &v)
	{
		static	rect<Int16>	scr_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		m.transform(v, this);
		out_screen=(scr_rect.order(*this)!=inside);
	}
	
	friend class model;
};



//============================================================
//	���������� ������
//============================================================
class	model
{
private:
	UInt8			n_vertexes;	// ���-�� ������
	Vertex		*vertexes;	// �������
	Polygon		*polygons;	// ��������
	Int8			*rendering[9];	//	������������������ ���������� ��� 8� �������� ������ + ��� �����
	UInt8			*transform_vertex;	// ������ ����� ��� 8 �������� ������ �� ��������� ������ (����-�� ����� �� ����������������)

	//	��������
	Vertex	min, max;
	
	static	color	two_color;
	static	UInt32	four_color;
	static	color	*end_screen;

	//======================================================
	void make_rendering_info(Int8 sector, Int8 n_polygon);
	static	void 	draw_line(color *screen, Int8 up, Int8 down, projected_point *projection);
	static	void 	draw_triangle(color *screen, Int8 up, Int8 mid, Int8 down, projected_point *projection);
	static	void 	draw_quadrangle(color *screen, Int8 up, Int8 left, Int8 right, Int8 down, projected_point *projection);
	static	color	*fill_trapezium_fast(color *screen, Int16 h, fixed<> &x1, fixed<> &x2, const fixed<> &dx1, const fixed<> &dx2);
	static	color	*fill_trapezium_safe(color *screen, Int16 h, fixed<> &x1, fixed<> &x2, const fixed<> &dx1, const fixed<> &dx2);

	//------------------------------------------------------
	#ifdef COLOR_GRAY
	static	inline	void fill_line(UInt8 *screen, const fixed<> &x, int l)
	{
		screen+=(x.value>>(FRACT+1));
		if (x.value & (1L<<FRACT))
		{
			(*screen)&=0xf0;
			(*screen++)|=(two_color&0xf);
			l--;
		}
		
		if ( (((UInt32)screen) & 1) && l>0 )
		{	// �������� 1 ����
			(*screen++)=two_color;
			l-=2;
		}
		// �� 4 �����
		for (; l>=8; l-=8)
			*(((UInt32*)screen)++)=four_color;
		// �� �����
		for (; l>=2; l-=2)
			*(screen++)=two_color;

		if (l>0)
		{
			(*screen)&=0xf;
			(*screen)|=(two_color&0xf0);
		}
	}
	#endif

	//========================================================
	#ifdef COLOR_8
	static	inline	void fill_line(UInt8 *screen, const fixed<> &x, int l)
	{
		screen+=(x.value>>FRACT);
		
		if ( (((UInt32)screen) & 1) && l>0 )
		{	// �������� 1 ����
			(*screen++)=two_color;
			l--;
		}
		// �� 4 �����
		for (; l>=4; l-=4)
			*(((UInt32*)screen)++)=four_color;
		// �� �����
		for (; l>=1; l--)
			*(screen++)=two_color;
	}
	#endif

	//========================================================
	#ifdef COLOR_16
	static	inline	void fill_line(color *screen, const fixed<> &x, int l)
	{
		screen+=(x.value>>FRACT);

		// �� 4 �����
		l>>=1;
		while (l--)
			*(((UInt32*)screen)++)=four_color;
	}
	#endif
	
	//========================================================
	static	inline	fixed<>	get_dx(const point<Int16> &p1, const point<Int16> &p2)
	{
		return fixed<>(p2.x-p1.x)/=(p1.y!=p2.y ? p2.y-p1.y : 1);
	}

	//========================================================
public:
	model(UInt8 _n_vertexes, Vertex *_vertexes, Polygon *_polygons, Int8 *_rendering);
	virtual	~model();
	void transform(const matrix<> &m, Int8 sector, projected_point *projection);	// ������� ������������� � ������ ����������
	void render(color *screen, Int8 sector, projected_point *projection, Int8 color);
};

//	��� ������������ ������
extern model	car_low,
					car_med,
					car_high;
					
					
					

//=========================================================
// ���� ������� - ������� �� ������
struct	solid
{
	// ���� - ��� ������� ������������ ������� �������..
	struct	atom
	{
		long	d,	// �������� �� ��� x
				r;	// ������
	}*atoms;
	long	n,	// ���-�� ������
			r;	// ������ ������ �������
};

extern	solid	car_solid;


#endif	//	_MODEL_H_