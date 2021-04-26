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
class ground
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
		mud=grass,
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
		UInt8	id;	// номер

		struct
		{
			UInt8	internal	:1,	// булева: внутренняя/нет
					road		:1,	// булева: дорога/нет
					terrain	:2,	// номер территории
					n			:4;	// номер клетки
		}info;

		struct
		{
			UInt8	n		:4,	// номер территории
					cage	:4;	// номер клетки
		}terrain;

		// специальные территории - особый случай
		struct
		{
			UInt8	n			:2,	// номер группы
					terrain	:2,	// номер территории
					cage		:4;	// номер клетки
		}special;
	};

	static	texture	*map;		// карта поверхности (номера текстур)
	static	color2	*textures[256];	// все текстуры
	static	ground	*terrains[];		// указатели на пов-ти
	static	int	n_textures;	// сколько текстур загружено
	UInt8		first_id;	//	номер первой текстуры в группе
	int		friction;	// коэффициент трения поверхности

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
public:
	ground(int _friction);
	virtual	~ground();
	
	void set(BitmapType *bmp);	// установка указателей на текстуры
	static	texture	join_textures(texture current, texture left, texture right, texture down, texture up);
	static	bool	join_textures(texture &current, texture other);
	static	void	draw(register color2 *screen, const camera &cam);
	static	void 	make_minimap(image &minimap);
	static	void	unpack_ground(MemHandle h);
	static	void	unpack_road(MemHandle h);
	static	void	normalize_map();
	static	void	process(unit *u, const real &time);
	
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


//==========================================================
//	Неанимированая поверхность
//==========================================================
class static_ground : public ground
{
private:
	MemHandle	handle;
	BitmapType	*bmp;
	
public:
	static_ground(UInt16 id, int _friction);
	~static_ground();
};




//==========================================================
//	Анимированная поверхность
//==========================================================
class anim_ground : public ground
{
private:
	MemHandle	handle[N_FRAMES];
	BitmapType	*bmp[N_FRAMES];
	Int16			current;	// текущий кадр
	real			wait;		// сколько ждать до смены кадров
	
public:
	anim_ground(UInt16 id, int _friction);
	~anim_ground();
	void process(real time);	// прошло времени (сек)
};


#endif	// _GROUND_H_