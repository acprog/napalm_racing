/* naPalm Racing

  Copyright (C) 2004

  Author: Alexander Semenov <acmain@gmail.com>
*/

#include "model.h"
#include "cfg.h"
#include "algorithm.h"
#include "matrix.h"

color		model::two_color;
UInt32	model::four_color;
color		*model::end_screen;

#define	END	-1


//============================================================
//	“рехмерна€ модель
//============================================================
model::model(UInt8 _n_vertexes, Vertex *_vertexes, Polygon *_polygons, Int8 *_rendering)
	:n_vertexes	(_n_vertexes)
	,vertexes	(_vertexes)
	,polygons	(_polygons)
{
	transform_vertex=new UInt8[n_vertexes];
	fill(transform_vertex, transform_vertex+n_vertexes, 0);

	// заполнение информации дл€ рендеринга
	for (int i=0; i<9; i++)
	{
		// ищем следующий сектор
		rendering[i]=_rendering;
		while (*(_rendering++)!=END)
			make_rendering_info(i, *(_rendering-1));
	}
}


//============================================================
void model::make_rendering_info(Int8 sector, Int8 n_polygon)
{
	if (sector==VIEW_FROM_BACK)
		return;	// сектор "виртуальный" и в нем рассчитывать ничего не надо
		
	Polygon	*p=polygons+n_polygon;
#ifdef COLOR_GRAY
	// дублирование цвета в байте 4+4
	p->color|=(p->color<<4);
#endif
	
	// информаци€ дл€ трансформировани€
	transform_vertex[p->vertex_id[0]]|=(1<<sector);
	transform_vertex[p->vertex_id[1]]|=(1<<sector);
	if (p->vertex_id[2]!=END)
		transform_vertex[p->vertex_id[2]]|=(1<<sector);
	if (p->vertex_id[3]!=END)
		transform_vertex[p->vertex_id[3]]|=(1<<sector);
}


//============================================================
model::~model()
{
	delete[] transform_vertex;
}



//=========================================================
void model::transform(const matrix<> &m, Int8 sector, projected_point *projection)
{
	if (sector==VIEW_FROM_BACK)
		for (UInt8 i=0; i<n_vertexes; i++)
			projection[i].transform(m, vertexes[i]);
	else	
		for (UInt8 sector_mask=(1<<sector), i=0; i<n_vertexes; i++)
			if (transform_vertex[i] & sector_mask)
				projection[i].transform(m, vertexes[i]);
}


//=========================================================
void model::render(color *screen, Int8 sector, projected_point *projection, Int8 color)
{
	//------------------------------------------------------------
	// цвета машин
	#ifdef COLOR_8
	// посто€нные цвета
	UInt8	color_statics[]=
	{
		0, 0, 0xa1, 0xdc, 0xdb, 0xda, 0xd9, 0xd8, 0xd7, 0xff
	};

	// цветовые схемы
	UInt8	color_sheme[][6]=
	{
		{0x00, 0xe0, 0xdf, 0xe1, 0xde, 0xdd},	//	бела€
		{0x00, 0xe0, 0x6b, 0xb9, 0xde, 0xdd},	//	сине-бела€
		{0x12, 0x25, 0x37, 0x4a, 0x5d, 0x63},	//	изумрудна€
		{0x5f, 0x65, 0x6b, 0xb9, 0xcb, 0xd1},	//	син€€
		{0xc0, 0xc1, 0xd4, 0xc3, 0xd5, 0xd6},	//	зелена€
		{0xdf, 0xe1, 0xdd, 0xdc, 0xdb, 0xda},	//	серебр€нна€
		{0x7c, 0x88, 0x8f, 0x9b, 0xa1, 0xe2},	//	красна€
		{0x8c, 0x99, 0x9f, 0xac, 0xb2, 0xc5},	//	охра
		{0x73, 0x79, 0x86, 0x8c, 0x99, 0x9f},	//	золотиста€
		{0xff, 0x79, 0x86, 0xd7, 0xd8, 0x9f},	//	черно-желта€
		{0xdb, 0xda, 0xd9, 0xd8, 0xd7, 0xff}	//	черна€
	};
	#endif


	end_screen=screen+BPP(SCREEN_WIDTH)*(SCREEN_HEIGHT);
	// потом рисуем полигоны
	Polygon	*p;
	for (Int8 *render=rendering[sector]; *render!=END; render++)
	{
		p=polygons+*render;

		// задание цвета
	#ifdef COLOR_GRAY
		two_color=(p->color<=REPLACE_TO_COLOR ? p->color+color : p->color);
	#endif

	#ifdef COLOR_8
		two_color=(p->color<=REPLACE_TO_COLOR ? color_sheme[color][p->color] : color_statics[p->color-REPLACE_TO_COLOR-1]);
	#endif

	#ifdef COLOR_16
//		two_color=(p->color<=REPLACE_TO_COLOR ? color_sheme[color][p->color] : color_statics[p->color-REPLACE_TO_COLOR-1]);
		two_color=p->color;
	#endif

		four_color=two_color;
		four_color|=four_color<<8;
		four_color|=four_color<<16;
		
		// рисование полигона
		if (p->vertex_id[2]==END)	// лини€
			draw_line(screen, p->vertex_id[0], p->vertex_id[1], projection);
		else	if (p->vertex_id[3]==END)	// треугольник
			draw_triangle(screen, p->vertex_id[0], p->vertex_id[1], p->vertex_id[2], projection);
		else	// четырехугольник
			draw_quadrangle(screen, p->vertex_id[0], p->vertex_id[1], p->vertex_id[2], p->vertex_id[3], projection);
	}
}


//=========================================================
void model::draw_line(color *screen, Int8 up, Int8 down, projected_point *p)
{
	// сортировка
	if (p[up].y>p[down].y)
		swap(up, down);

	// приращение по x
	fixed<>	dx=get_dx(p[up], p[down]),
				x=p[up].x;

	screen+=BPP(p[up].y*SCREEN_WIDTH);
	if (!p[up].out_screen && !p[down].out_screen)
		for (int h=p[down].y-p[up].y; h--; x+=dx,	screen+=BPP(SCREEN_WIDTH) )
			fill_line(screen, x, dx>1 ? (int)dx : 1);
	else
		for (int h=p[down].y-p[up].y; 
			h-- && screen<end_screen; x+=dx,	screen+=BPP(SCREEN_WIDTH) )
			if (x>0 && x<SCREEN_WIDTH)
				fill_line(screen, x, dx>1 ? (int)dx : 1);
}


//=========================================================
void model::draw_triangle(color *screen, Int8 up, Int8 mid, Int8 down, projected_point *p)
{
	// определение способа отрисовки
	color* (*fill_trapezium)(color*, Int16, fixed<>&, fixed<>&, const fixed<>&, const fixed<>&);
	if (p[up].out_screen || p[mid].out_screen || p[down].out_screen)
		fill_trapezium=fill_trapezium_safe;
	else
		fill_trapezium=fill_trapezium_fast;

	// сортировка
	if (p[up].y>p[down].y)
		swap(up, down);
	if (p[mid].y>p[down].y)
		swap(mid, down);
	else if (p[mid].y<p[up].y)
		swap(mid, up);

	// приращени€ по x
	fixed<>	dx_ud=get_dx(p[up], p[down]),
				dx_um=get_dx(p[up], p[mid]),
				dx_md=get_dx(p[mid], p[down]),
				x1=p[up].x,
				x2=x1;

	screen+=BPP(p[up].y*SCREEN_WIDTH);
	if (dx_um<dx_ud)
	{
		screen=fill_trapezium(screen, p[mid].y-p[up].y, x1, x2, dx_um, dx_ud);
		fill_trapezium(screen, p[down].y-p[mid].y, x1, x2, dx_md, dx_ud);
	}
	else
	{
		screen=fill_trapezium(screen, p[mid].y-p[up].y, x1, x2, dx_ud, dx_um);
		fill_trapezium(screen, p[down].y-p[mid].y, x1, x2, dx_ud, dx_md);
	}
}


//=========================================================
void 	model::draw_quadrangle(color *screen, Int8 up, Int8 left, Int8 right, Int8 down, projected_point *p)
{
	// определение способа отрисовки
	color* (*fill_trapezium)(color*, Int16, fixed<>&, fixed<>&, const fixed<>&, const fixed<>&);
	if (p[up].out_screen || p[left].out_screen || p[right].out_screen || p[down].out_screen)
		fill_trapezium=fill_trapezium_safe;
	else
		fill_trapezium=fill_trapezium_fast;

	// сортировка
	if (p[up].y>p[down].y)
		swap(up, down);

	if (p[left].y>p[down].y)
		swap(left, down);
	else if (p[left].y<p[up].y)
		swap(left, up);

	if (p[right].y>p[down].y)
		swap(right, down);
	else if (p[right].y<p[up].y)
		swap(right, up);


	// приращени€ по x
	fixed<>	dx_ud=get_dx(p[up], p[down]),
				dx_ul=get_dx(p[up], p[left]),
				dx_ur=get_dx(p[up], p[right]);
	
	if (dx_ul>dx_ur)
	{
		swap(dx_ul, dx_ur);
		swap(left, right);
	}

	fixed<>	x1=p[up].x,
				x2=x1;

#define mid_up		left
#define mid_down	right	
#define dx_um		dx_ul

	screen+=BPP(p[up].y*SCREEN_WIDTH);
	if (dx_ul>dx_ud)	// точки справа от [up-down]
	{
		if (p[mid_up].y>p[mid_down].y)
		{
			swap(mid_up, mid_down);
			dx_um=dx_ur;
		}
			
		fixed<>	dx_m=get_dx(p[mid_up], p[mid_down]),
					dx_md=get_dx(p[mid_down], p[down]);

		screen=fill_trapezium(screen, p[mid_up].y-p[up].y, x1, x2, dx_ud, dx_um);
		screen=fill_trapezium(screen, p[mid_down].y-p[mid_up].y, x1, x2, dx_ud, dx_m);
		fill_trapezium(screen, p[down].y-p[mid_down].y, x1, x2, dx_ud, dx_md);
	}
	else if (dx_ur<dx_ud)	// точки слева от [up-down]
	{
		if (p[mid_up].y>p[mid_down].y)
		{
			swap(mid_up, mid_down);
			dx_um=dx_ur;
		}
			
		fixed<>	dx_m=get_dx(p[mid_up], p[mid_down]),
					dx_md=get_dx(p[mid_down], p[down]);

		screen=fill_trapezium(screen, p[mid_up].y-p[up].y, x1, x2, dx_um, dx_ud);
		screen=fill_trapezium(screen, p[mid_down].y-p[mid_up].y, x1, x2, dx_m, dx_ud);
		fill_trapezium(screen, p[down].y-p[mid_down].y, x1, x2, dx_md, dx_ud);
	}
	else	// точки по разные стороны от [up-down]
	{
		fixed<>	dx_ld=get_dx(p[left], p[down]),
					dx_rd=get_dx(p[right], p[down]);

		if (p[left].y<p[right].y)
		{
			screen=fill_trapezium(screen, p[left].y-p[up].y, x1, x2, dx_ul, dx_ur);
			screen=fill_trapezium(screen, p[right].y-p[left].y, x1, x2, dx_ld, dx_ur);
			fill_trapezium(screen, p[down].y-p[right].y, x1, x2, dx_ld, dx_rd);
		}
		else if (p[left].y>p[right].y)
		{
			screen=fill_trapezium(screen, p[right].y-p[up].y, x1, x2, dx_ul, dx_ur);
			screen=fill_trapezium(screen, p[left].y-p[right].y, x1, x2, dx_ul, dx_rd);
			fill_trapezium(screen, p[down].y-p[left].y, x1, x2, dx_ld, dx_rd);
		}
		else
		{
			screen=fill_trapezium(screen, p[left].y-p[up].y, x1, x2, dx_ul, dx_ur);
			fill_trapezium(screen, p[down].y-p[left].y, x1, x2, dx_ld, dx_rd);
		}
	}
}


//=========================================================
color *model::fill_trapezium_fast(color *screen, Int16 h, fixed<> &x1, fixed<> &x2, const fixed<> &dx1, const fixed<> &dx2)
{
	if (!h)
	{
		x1+=dx1;
		x2+=dx2;
		return screen;
	}

	// целесообразно "увеличить" глубину цвета в 2 раза:
	// в первой половине один цвет, а во второй - другой (больший на 1)
	//	и после вывода строчки - циклический сдвиг цвета на 4 бита

	while (h--)
	{
		fill_line(screen, x1, (int)x2-(int)x1);
		screen+=BPP(SCREEN_WIDTH);
		
		x1+=dx1;
		x2+=dx2;
	}
	return screen;
}





//=========================================================
color *model::fill_trapezium_safe(color *screen, Int16 h, fixed<> &x1, fixed<> &x2, const fixed<> &dx1, const fixed<> &dx2)
{
	static	fixed<>	screen_width=SCREEN_WIDTH,
							zero=0;
	
	if (!h)
	{
		x1+=dx1;
		x2+=dx2;
		return screen;
	}

	Int16		l;

	while (h-- && screen<end_screen)
	{
		if (x1<screen_width && x2>zero)
		{
			l=(x2<screen_width ? (int)x2 : SCREEN_WIDTH);

			if (x1>zero)
				fill_line(screen, x1, l-(int)x1);
			else
				fill_line(screen, 0, l);
		}
		screen+=BPP(SCREEN_WIDTH);

		x1+=dx1;
		x2+=dx2;
	}
	return screen;
}