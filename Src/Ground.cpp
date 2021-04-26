/* naPalm Racing

  Copyright (C) 2004

  Author: Alexander Semenov <acmain@gmail.com>
*/
#include "ground.h"
#include "world.h"
#include "unit.h"
#include "flat3d.h"

fixed<>	*ground::distTable,
			*ground::CSinTable,
			*ground::CCosTable;
ground::texture	*ground::map;	// карта поверхности
color2	*ground::textures[256];	// все текстуры
int		ground::n_grounds=0;	// сколько текстур загружено
ground	*ground::terrains[16];	// указатели на пов-ти


//========================================================================
// сглаживание изгибов дороги с барьерами с 0-45 на 30-30

// для краткости
#define	MN(n)	{grass, 	true, false,n}	// основная группа текстур (дорога с барьерами)
#define	TR(n)	{specials,	true, false,n}	// треугольники
#define	QA(n)	{grass, 	true, true,	n}	// квадрат

#define	SM(lo, hi, mn, tr_lo, tr_hi, qa)	{	lo, hi,	{MN(mn), TR(tr_lo), TR(tr_hi), QA(qa)}	}

// таблица сглаживаемых клеток дороги
ground::smooth_cage_info	ground::smooth_cages[8]=
{
	SM(3, 1,	0x5, 0x8, 0xa, 0x0),
	SM(0, 2,	0x6, 0xb, 0x0, 0x1),
	SM(1, 4,	0x7, 0x1, 0x9, 0x3),
	SM(5, 0,	0x9, 0x2, 0xc, 0x4),
	SM(2, 7,	0xb, 0xd, 0x3, 0x7),
	SM(3, 6,	0xd, 0x6, 0xe, 0xc),
	SM(5, 7,	0xe, 0xf, 0x4, 0xd),
	SM(6, 4,	0xf, 0x5, 0x7, 0xf)
};
#undef MN
#undef TR
#undef QA
#undef SM
//========================================================================













//==========================================================
//	Поверхность
//==========================================================
ground::ground(UInt16 id, int _friction)
	:animation	(id)
	,first_id	(n_grounds*16)
	,friction	(_friction)
{
	if (n_grounds==0)	// первый вызов
		init_globals();
	terrains[n_grounds++]=this;
	
	if (id)
		set_frame(0);
}


//==========================================================
void ground::init_globals()
{
	map=new texture[MAP_SIZE*MAP_SIZE];
	distTable=new fixed<>[BPP(SCREEN_WIDTH)];
	CSinTable=new fixed<>[1024];
	CCosTable=new fixed<>[1024];

	// заполнение таблиц
	for ( int i=0; i<BPP(SCREEN_WIDTH); i++ )
	{
		distTable[i]=D0/( (i+1)*DELTA );
		distTable[i].value>>=8;
	}

	for ( int i = 0; i < 1024; i++ )
	{
		CSinTable[i]=sinTable[i]*C;
		CCosTable[i]=cosTable[i]*C;
	}
}

//==========================================================
ground::~ground()
{
	if (--n_grounds==0)	// последний вызов
	{
		delete[]	map;	
		delete[]	distTable;
		delete[]	CSinTable;
		delete[]	CCosTable;
	}
}


//==========================================================
// установка указателей на текстуры
void ground::set_frame(int n)
{
	color	*bits=(color*)*frames[n];
	color	**p=(color**)textures+first_id;

	int	h, w;
	for (h=0; h<4; h++)
		for (w=0; w<4; w++)
			*(p++)=bits+BPP(TEXTURE_SIZE*w+(TEXTURE_SIZE*4*TEXTURE_SIZE)*h);
}



//=====================================================================
void	ground::unpack_ground(MemHandle h)
{
	texture		*dst=map;
	packed_2bit	*buffer=new packed_2bit[MAP_SIZE*MAP_SIZE/8],
					*src=buffer;

	MemMove(buffer, MemHandleLock(h), MAP_SIZE*MAP_SIZE/4);
	MemHandleUnlock(h);

	// очистка
	for (int i=0; i<MAP_SIZE*MAP_SIZE; i++)
		dst[i].id=0;

	for (int i=MAP_SIZE*MAP_SIZE/8; i--; src++, dst+=8)
	{
		dst[0].info.terrain=(*src)._00;
		dst[1].info.terrain=(*src)._01;
		dst[2].info.terrain=(*src)._10;
		dst[3].info.terrain=(*src)._11;
		dst[4].info.terrain=(*src)._20;
		dst[5].info.terrain=(*src)._21;
		dst[6].info.terrain=(*src)._30;
		dst[7].info.terrain=(*src)._31;
	}

	delete[] buffer;
}


//=====================================================================
void	ground::unpack_road(MemHandle h)
{
	texture		*dst=map;
	packed_1bit	*buffer=new packed_1bit[MAP_SIZE*MAP_SIZE/16],
					*src=buffer;
	
	MemMove(buffer, MemHandleLock(h), MAP_SIZE*MAP_SIZE/8);
	MemHandleUnlock(h);
	
	for (int i=MAP_SIZE*MAP_SIZE/16; i--; src++, dst+=16)
	{
		dst[0].info.road=(*src)._00;
		dst[1].info.road=(*src)._01;
		dst[2].info.road=(*src)._10;
		dst[3].info.road=(*src)._11;
		dst[4].info.road=(*src)._20;
		dst[5].info.road=(*src)._21;
		dst[6].info.road=(*src)._30;
		dst[7].info.road=(*src)._31;
		dst[8].info.road=(*src)._40;
		dst[9].info.road=(*src)._41;
		dst[10].info.road=(*src)._50;
		dst[11].info.road=(*src)._51;
		dst[12].info.road=(*src)._60;
		dst[13].info.road=(*src)._61;
		dst[14].info.road=(*src)._70;
		dst[15].info.road=(*src)._71;
	}

	delete[] buffer;
}


//==========================================================
bool	ground::join_textures(texture &current, texture other)
{
	if (current.info.terrain==sand && !current.info.road)
		return true;

	if (other.info.terrain==specials && other.info.internal)
		return true;
		
	if (current.info.road && other.info.road)
		return true;
		
	if (current.info.terrain==water && !current.info.road && other.info.terrain==water)
		return true;
		
	return current.terrain.n==other.terrain.n;
}


//==========================================================
ground::texture	ground::join_textures(texture current, texture left, texture right, texture down, texture up)
{
	// для сопряжения земель
	// битовая маска (от младшего к старшему):
	//	left, right, down, up (true - аналогичная пов-ть)
	// соответствие номеров, получаемых битовой маской, номерам текстур
	static	UInt8	join[16]=
	{
		0x0, 0x3, 0x1, 0x2,
		0x4, 0x7, 0x5, 0x6,
		0xc, 0xf, 0xd, 0xe,
		0x8, 0xb, 0x9, 0xa
	};

	// специальные сопрягаются со всеми
	if (current.info.terrain==specials)
		return current;

	current.info.n=join[
		join_textures(current, left) |
		(join_textures(current, right)<<1) |
		(join_textures(current, down)<<2) |
		(join_textures(current, up)<<3)
		];
		
	return current;
}



//=====================================================================
void	ground::normalize_map()
{
	int			w, h, i;

	// очистка
	for (i=0; i<MAP_SIZE*MAP_SIZE; i++)
		if (map[i].info.terrain!=specials)
			map[i].info.n=center_n;
		else
		{
			map[i].info.n=block_n;	// или маркер (что несущественно)
			map[i].info.internal=true;
		}

	for (i=MAP_SIZE+1, h=1; h<MAP_SIZE-1; h++, i+=2)
		for (w=1; w<MAP_SIZE-1; w++, i++)
			map[i]=join_textures(
				map[i],
				map[i-1],
				map[i+1],
				map[i+MAP_SIZE],
				map[i-MAP_SIZE]);
	
	// разнообразие поверхностей
	for (i=MAP_SIZE*MAP_SIZE; i--;)
		if (map[i].info.n==center_n 
		&& !map[i].info.road
		&& (map[i].info.terrain==grass || map[i].info.terrain==sand)
		&& SysRandom(0)%10==0)
		{
			if (map[i].info.terrain!=sand)
				map[i].info.internal=true;
			map[i].info.n=SysRandom(0)%16;
		}
		
	normalize_road();
}



//=====================================================================
ground::texture	ground::smooth_cage(Int8 current, Int8 left, Int8 right, Int8 down, Int8 up)
{
	Int8	lo=smooth_cages[current].lo_join,
			hi=smooth_cages[current].hi_join;

	return smooth_cages[current].variant[
		(lo==left || lo==right || lo==down || lo==up)
	|  ((hi==left || hi==right || hi==down || hi==up)<<1)
	];
}


//=====================================================================
Int8	ground::search_cage(texture current)
{
	int	i, j;
	
	for (i=0; i<8; i++)
		for (j=0; j<4; j++)
			if (smooth_cages[i].variant[j].id==current.id)
				return i;
	return -1;
}


//=====================================================================
void	ground::normalize_road()
{
	int		w, h, i, k;
	Int8	id;

	//---------------------------------------------------

	for (i=MAP_SIZE+1, h=1; h<MAP_SIZE-1; h++, i+=2)
		for (w=1; w<MAP_SIZE-1; w++, i++)
			if (map[i].info.road)
			{
				// рисуем белые линии на дороге
				if (map[i].info.terrain==specials)
					draw_road_mark(map+i);
			
				id=search_cage(map[i]);
				if (id!=-1)
					map[i]=smooth_cage(
						id,
						search_cage(map[i-1]),
						search_cage(map[i+1]),
						search_cage(map[i+MAP_SIZE]),
						search_cage(map[i-MAP_SIZE])
					);

				// "затыкаем" углы
				normalize_road_conner(map+i);
			}
}


//=====================================================================
void ground::normalize_road_conner(texture *p)
{
	static	texture
		center_grass={grass, true, false, center_n},
		center_sand={sand, true, false, center_n},
		corner_up_left={grass, true, true, 0xa},
		corner_up_right={grass, true, true, 0x9},
		corner_down_left={grass, true, true, 0x6},
		corner_down_right={grass, true, true, 0x5};

	if (p->id==center_grass.id || p->id==center_sand.id)
		if (!p[-MAP_SIZE-1].info.road)
			*p=corner_up_left;
		else if (!p[-MAP_SIZE+1].info.road)
			*p=corner_up_right;
		else if (!p[MAP_SIZE-1].info.road)
			*p=corner_down_left;
		else if (!p[MAP_SIZE+1].info.road)
			*p=corner_down_right;
}


//=====================================================================
void ground::draw_road_mark(texture *p)
{
	// здесь берется битовая маска со прилежащих клеток
	// и ищется соответствующий ей номер текстуры по таблице

	// сокращение
	#define	BITS(one, two)	((1<<one) | (1<<two))
	// битовые маски, соответствующие номерам текстур в группе	
	static	UInt8	marks_mask[]=
	{
		BITS(1, 6),
		BITS(3, 4),
		BITS(0, 7),
		BITS(2, 5),
		BITS(2, 6),
		BITS(0, 6),
		BITS(4, 5),
		BITS(3, 7),
		BITS(1, 7),
		BITS(1, 5),
		BITS(0, 4),
		BITS(2, 3)
	};
	#undef BITS	

	UInt8	mask=0;
	
	// сокращение
	#define	MSK(offset)	(mask<<=1)|=(p[offset].terrain.n==15)

	MSK(MAP_SIZE+1);
	MSK(MAP_SIZE);
	MSK(MAP_SIZE-1);
	MSK(1);
	MSK(-1);
	MSK(-MAP_SIZE+1);
	MSK(-MAP_SIZE);
	MSK(-MAP_SIZE-1);
	
	#undef	MSK	
	
	for (int i=0; i<12; i++)
		if (mask==marks_mask[i])
		{
			p->info.n=i;
			return;
		}
		
	p->info.n=15;
}



//=====================================================================
void ground::make_minimap(image &minimap)
{
#ifdef COLOR_GRAY
	UInt8	transparent[]={1, 3, 2, 0, 6},
			opaque[]={5, 10, 7, 0, 15};
#endif

#ifdef COLOR_8
	UInt8	transparent[]={0, 0, 0, 0, 0xff},
			opaque[]={0x62, 0xc3, 0x1f, 0x6c, 0xff};
#endif

#ifdef COLOR_16
	color	transparent[]={0, 0, 0, 0, 0xff},
			opaque[]={0x62, 0xc3, 0x1f, 0x6c, 0xff};
#endif

	color	*colors=g_prefs.minimap.transparent ? transparent : opaque;
	int	d=MAP_SIZE/MINIMAP_SIZE;
	point<>	src,
			dst;
				
#define	COLOR(c)	(c.info.road ? colors[4] : colors[c.info.terrain])

#ifdef COLOR_GRAY
	for (dst.y=0; dst.y<MINIMAP_SIZE; dst.y++, src.y+=d)
		for (src.x=0, dst.x=0; dst.x<MINIMAP_SIZE; dst.x+=2, src.x+=2*d)
			minimap[dst]=COLOR(get_texture(src+point<>(d, 0))) | (COLOR(get_texture(src))<<4);
#endif

#ifdef COLOR_8
	for (dst.y=0; dst.y<MINIMAP_SIZE; dst.y++, src.y+=d)
		for (src.x=0, dst.x=0; dst.x<MINIMAP_SIZE; dst.x++, src.x+=d)
			minimap[dst]=COLOR(get_texture(src));
#endif
}



//=====================================================================
void ground::draw(register color2 *screen, const camera &cam)
{
	fixed<>	dist, u0, v0;
	register	fixed<>	du, dv;
	register	Int8		col;
	register	ground_u	u;
	register	ground_v v;

	for ( int row = SKIP_UP; row < (SCREEN_HEIGHT-SKY_HEIGHT+SKIP_UP); row++ )
	{
		dist.value= ( (cam.z.value>>8) * distTable[row].value ) >> 8;
		u0.value = ( cam.x.value + dist.value * (COS(cam.angle).value >> 8 ) );
		v0.value = ( cam.y.value + dist.value * (SIN(cam.angle).value >> 8 ) );
		du.value = (  dist.value * CSinTable[cam.angle>>6].value ) >> 7;
		dv.value = ( -dist.value * CCosTable[cam.angle>>6].value ) >> 7;

		screen+=40;
		for (u.fix=u0, v.fix=v0, col=40; col--;)
		{
			*(--screen)=get_color(u, v);

			u.fix+=du;
			v.fix+=dv;
		}

		screen+=40;
		for (u.fix=u0, v.fix=v0, col=40; col--;)
		{
			u.fix-=du;
			v.fix-=dv;

			*(screen++)=get_color(u, v);
		}
	}
}



//==========================================================
void	ground::process(unit *u, const real &time)
{
	static	texture	block={specials, false, true, block_n};
	texture	t=get_texture(*u);

	if (t.id!=block.id)
		u->ricochet_move=false;
	else
		u->ricochet();
	u->process(time, terrains[t.terrain.n]->friction);
}


//==========================================================
void	ground::animate(const real &time)
{
	for (int i=0; i<n_grounds; i++)
		terrains[i]->anim(time);
}
