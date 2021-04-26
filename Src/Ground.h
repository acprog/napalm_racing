/* naPalm Racing

  Copyright (C) 2004

  Author: Alexander Semenov <acmain@gmail.com>
*/
#ifndef _GROUND_H_
#define _GROUND_H_

#include <PalmOS.h>
#include "object.h"
#include "cfg.h"
#include "image.h"


class unit;


//============================================================
//	Упакованные биты
struct	packed_4bit
{
	UInt16	_00	:4,
				_01	:4,
				_10	:4,
				_11	:4;
};

//--------------------------------------
struct	packed_2bit
{
	UInt16	_00	:2,
				_01	:2,
				_10	:2,
				_11	:2,
				_20	:2,
				_21	:2,
				_30	:2,
				_31	:2;
};

//--------------------------------------
struct	packed_1bit
{
	UInt16	_00	:1,
				_01	:1,
				_10	:1,
				_11	:1,
				_20	:1,
				_21	:1,
				_30	:1,
				_31	:1,
				_40	:1,
				_41	:1,
				_50	:1,
				_51	:1,
				_60	:1,
				_61	:1,
				_70	:1,
				_71	:1;
};



//==========================================================
//	Поверхность
//==========================================================
class ground : public animation
{
protected:
	static	fixed<>	*distTable,
					*CSinTable,
					*CCosTable;
							
	//-----------------------------------------------------
	//	Номера групп текстур
	enum groups
	{
		water=0,
		grass=1,
		sand=2,
		specials=3
	};
		
	// номера клеток в группе
	enum cages_n
	{
		center_n=0xa,
		block_n=0
	};
	
	//----------------------------------------------------
	// тукстуры поверхности
	union texture
	{
		struct
		{
			UInt8	terrain		:2,	// номер территории
					road		:1,	// булева: относится к дороге/нет
					internal	:1,	// булева: внутренняя/нет
					n			:4;	// номер клетки
		}info;

		struct
		{
			UInt8	n		:4,	// номер территории
					cage	:4;	// номер клетки
		}terrain;

		UInt8	id;	// номер
	};
	
	//-----------------------------------------------------------------	
	// таблица сглаживаемых клеток дороги (используется при генерации карты)
	static	struct smooth_cage_info
	{
		Int8	lo_join,	// в зависимости от присутствия рядом
				hi_join;	// <- этих двух (индексы smooth), пулучим
		texture	variant[4];	// 4 варианта одной клетки
	}smooth_cages[8];

	
	//-----------------------------------------------------------------
	static	texture	*map;		// карта поверхности (номера текстур)
	static	color2	*textures[256];	// все текстуры
	static	ground	*terrains[];		// указатели на пов-ти
	static	int	n_grounds;	// сколько текстур загружено


	//-----------------------------------------------------------------
	// частные данные одной поверхности
	UInt8		first_id;	// номер первой текстуры в группе
	int			friction;	// коэффициент трения поверхности

	//=====================================================================
	// разложение параметра u
	union ground_u
	{
		fixed<> fix;
		struct
		{
			UInt32	out_map	:(32-FRACT-MAP_SHIFT-TEXTURE_SHIFT),
						map		:MAP_SHIFT,
						tex		:TEXTURE_SHIFT-1,
						fraction	:FRACT+1;
		}coord;
	};
	
	//---------------------------------------------------------------------
	// разложение параметра v
	union ground_v
	{
		fixed<>	fix;
		struct
		{
			UInt32	out_map	:(32-FRACT-MAP_SHIFT-TEXTURE_SHIFT),
						map		:MAP_SHIFT,
						tex		:TEXTURE_SHIFT,
						fraction	:FRACT;
		}coord;
	};

	//-------------------------------------------------------------------------
	static	inline color2 get_color(ground_u &u, ground_v &v)
	{
		static	UInt32 um=0, vm=0;
		static	color2	*p;

	#define	MAP_MASK	(((1L<<MAP_SHIFT)-1L)<<FRACT+TEXTURE_SHIFT)
		if ( (u.fix.value&MAP_MASK)!=um || (v.fix.value&MAP_MASK)!=vm)
		{
			um=u.fix.value&MAP_MASK;
			vm=v.fix.value&MAP_MASK;
			p=textures[map[(um>>(FRACT+TEXTURE_SHIFT)) | (vm>>(FRACT+TEXTURE_SHIFT-MAP_SHIFT))].id];
		}
	#undef MAP_MASK
		return p[ u.coord.tex | (v.coord.tex<<(GROUP_TEXTURES_SHIFT-1)) ];
	}
	
	//------------------------------------------------------------
	static	texture	join_textures(texture current, texture left, texture right, texture down, texture up);
	static	bool	join_textures(texture &current, texture other);

	static	void normalize_road();
	static	void normalize_road_conner(texture *p);
	static	void draw_road_mark(texture *p);	// рисует белые линии на дороге
	
	static	texture	smooth_cage(Int8 current, Int8 left, Int8 right, Int8 down, Int8 up);
	static	Int8	search_cage(texture current);
	
	static	void init_globals();
	
	void set_frame(int n);
	
	//------------------------------------------------------------
public:
	ground(UInt16 id=NULL, int _friction=10);
	virtual	~ground();
	
	static	void	draw(register color2 *screen, const camera &cam);
	static	void 	make_minimap(image &minimap);
	static	void	unpack_ground(MemHandle h);
	static	void	unpack_road(MemHandle h);
	static	void	normalize_map();
	static	void	process(unit *u, const real &time);
	static	void	animate(const real &time);
	
	//========================================================
	static inline	texture get_texture(const point<> &v)
	{
		return map[ v.x | (v.y<<MAP_SHIFT) ];
	}

	//--------------------------------------------------------
	static inline	texture get_texture(const point<real> &v)
	{
		return get_texture(point<>( ((int)v.x)>>TEXTURE_SHIFT, ((int)v.y)>>TEXTURE_SHIFT ));
	}
};

#endif	// _GROUND_H_