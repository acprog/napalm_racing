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
int		ground::n_textures=0;	// сколько текстур загружено
ground	*ground::terrains[16];	// указатели на пов-ти

//==========================================================
//	Поверхность
//==========================================================
ground::ground(int _friction)
	:first_id	(n_textures)
	,friction	(_friction)
{
	terrains[n_textures/16]=this;
	if (n_textures==0)	// первый вызов
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
	n_textures+=16;
}


//==========================================================
ground::~ground()
{
	n_textures-=16;
	if (n_textures==0)	// последний вызов
	{
		delete[] map;	
		delete[]	distTable;
		delete[]	CSinTable;
		delete[]	CCosTable;
	}
}


//==========================================================
// установка указателей на текстуры
void ground::set(BitmapType *bmp)
{
	color	*bits=(color*)BmpGetBits(bmp);
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

	for (int i=MAP_SIZE*MAP_SIZE/8; i--; src++, dst+=8)
	{
		dst[0].terrain.n=(*src)._00;
		dst[1].terrain.n=(*src)._01;
		dst[2].terrain.n=(*src)._10;
		dst[3].terrain.n=(*src)._11;
		dst[4].terrain.n=(*src)._20;
		dst[5].terrain.n=(*src)._21;
		dst[6].terrain.n=(*src)._30;
		dst[7].terrain.n=(*src)._31;
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
	if (current.terrain.n==sand)
		return true;

	if (other.info.terrain==specials)
		return true;
		
	if (current.info.road && other.info.road)
		return true;
		
	if (current.terrain.n==water && other.info.terrain==water)
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
			map[i].info.n=block_n;
			map[i].info.road=false;
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
		if (map[i].info.n==center_n && (map[i].terrain.n==grass || map[i].terrain.n==sand) && SysRandom(0)%10==0)
		{
			if (map[i].terrain.n!=sand)
				map[i].info.internal=true;
			map[i].info.n=SysRandom(0)%16;
		}
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
	texture	t=get_texture(*u);

	if (t.info.terrain!=specials)
		u->ricochet_move=false;
	else
		u->ricochet();
	u->process(time, terrains[t.terrain.n]->friction);
}



//==========================================================
//	Неанимированная поверхность
//==========================================================
static_ground::static_ground(UInt16 id, int _friction)
	:ground	(_friction)
	,handle	(DmGetResource(bitmapRsc, id))
	,bmp		((BitmapType*)MemHandleLock(handle))
{
	set(bmp);
}

	
//==========================================================
static_ground::~static_ground()
{
	MemHandleUnlock(handle);
	DmReleaseResource(handle);
}




//==========================================================
//	Анимированная поверхность
//==========================================================
anim_ground::anim_ground(UInt16 id, int _friction)
	:ground	(_friction)
	,current	(0)
	,wait		(0)
{
	for (int i=0; i<N_FRAMES; i++)
	{
		handle[i]=DmGetResource(bitmapRsc, id+i);
		bmp[i]	=(BitmapType*)MemHandleLock(handle[i]);
	}
	set(bmp[0]);
}


//==========================================================
anim_ground::~anim_ground()
{
	for (int i=0; i<N_FRAMES; i++)
	{
		MemHandleUnlock(handle[i]);
		DmReleaseResource(handle[i]);
	}
}


//==========================================================
void anim_ground::process(real time)
{
	wait-=time;
	if (wait<0)
	{
		wait+=ANIM_GROUND_WAIT;
		if (++current==N_FRAMES)
			current=0;
		set(bmp[current]);
	}
}
