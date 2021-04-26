/* naPalm Racing

  Copyright (C) 2004

  Author: Alexander Semenov <acmain@gmail.com>
*/
#include "image.h"
#include "flat3d.h"
#include <algorithm>
using namespace std;
#include "flat3drsc.h"

//==============================================================
//	»зображение
//==============================================================
image::image()
	:bits		(NULL)
	,offset		(0)
	,free_bits	(false)
#ifdef COLOR_GRAY
	,draw_type	(Add)
#endif
#ifdef COLOR_8
	,draw_type	(Or)
#endif
{
}


//==============================================================
image::image(const size<> &s, color *_bits, int _offset)
	:size		(s)
	,bits		(_bits)
	,offset		(_offset)
	,free_bits	(bits==NULL)
#ifdef COLOR_GRAY
	,draw_type	(Add)
#endif
#ifdef COLOR_8
	,draw_type	(Or)
#endif
{
	if (!bits)
		bits=new color[width*height];
}


//==============================================================
image::~image()
{
	if (free_bits)
		delete[] bits;
}


//==============================================================
void image::add(color *screen, const point<> &v)
{
	int 	x, y;
	color	*p=bits;

	screen+=SCREEN_OFFSET(v.x, v.y);
	for (y=height; y--; screen+=BPP(SCREEN_WIDTH-width), p+=BPP(offset))
		for (x=BPP(width)>>2; x--;)
			*( ((UInt32*)screen)++ ) += *( ((UInt32*)p)++ );
}


//==============================================================
void image::put(color *screen, const point<> &v)
{
	int 	x, y;
	color	*p=bits;

	screen+=SCREEN_OFFSET(v.x, v.y);
	for (y=height; y--; screen+=BPP(SCREEN_WIDTH-width), p+=BPP(offset))
		for (x=BPP(width)>>2; x--;)
			*( ((UInt32*)screen)++ ) = *( ((UInt32*)p)++ );
}


//==============================================================
void image::or_(color *screen, const point<> &v)
{
	int 	x, y;
	color	*p=bits;

	screen+=SCREEN_OFFSET(v.x, v.y);
	for (y=height; y--; screen+=BPP(SCREEN_WIDTH-width), p+=BPP(offset))
		for (x=BPP(width)>>2; x--;)
			*( ((UInt32*)screen)++ ) |= *( ((UInt32*)p)++ );
}


//==============================================================
// копирование изображени€ img на текущее
void image::copy(const image &img)
{
	const	UInt32	*src=(UInt32*)img.bits;
			UInt32	*dst=(UInt32*)bits;
			
	int	x, y;
	for (y=height; y--; src+=BPP(img.offset)>>2, dst+=BPP(offset)>>2)
		for (x=BPP(width)>>2; x--;)
			*(dst++)=*(src++);
}


//==============================================================
// "наклеить" на фоновый рисунок
void image::stick_on(image *img, color transparent)
{
	int		x, y;
	color	*background=(color*)*img,
			*p=bits;

	#ifdef COLOR_4
		ERROR("Not <res_image::stick_on> function for COLOR_4");
	#else
		for (y=height; y--; background+=img->offset, p+=offset)
			for (x=width; x--; p++, background++)
				if (*p==transparent)
					(*p)=*(background);
	#endif
}

//==============================================================
//	»зображение из ресурса
//==============================================================
res_image::res_image(UInt16 id, database *db)
	:bitmapP	(NULL)
{
	MemHandle	handle=DmGetResource(bitmapRsc, id);

	// принцип такой: распаковываем графику (если сжата) и
	// дописываем к базе (но сохран€ем указатели)
	// ≈сли база временна€, то удал€ем упакованную графику.
	// в деструкторе удал€ем из базы то, что в нее добавили,
	// если база не временна€. ≈сли база временна€, то 
	// все потом удалит деструктор database
	if (handle)
	{
		if (db)
			handle=db->detach(bitmapRsc, id, handle);
		bitmapP=(BitmapType*)MemHandleLock(handle);
	}
	else
	{
		ERROR("Can't find Tbmp");
/*
		#ifdef COLOR_16	// jpeg
			open_jpeg();
			if (true)
			{
				create_new(sizeof(open_jpeg));
				unpack_jpeg(to_new);
				release(opened_jpeg);
			}
		#else	// zlib
			open_zip();
			if (true)
			{
				create_new(sizeof(open_zip));
				unpack_zip(to_new);
				release(opened_zip);
			}
		#endif
/**/
	}

	DmReleaseResource(handle);

	struct	Head
	{
		Int16 width, height;
	}*head=(Head*)bitmapP;
	width=head->width;
	height=head->height;

	bits=(color*)BmpGetBits(bitmapP);
}



//==============================================================
res_image::~res_image()
{
}




	

//==============================================================
//	Ўрифт
//==============================================================
font::font(UInt16 id, char first, char last, int _step)
	:res_image	(id)
	,one_char	(this, rect<>(0, 0, width/(last-first+1), height) )
	,lower_char	(first)
	,step		(_step)
{
}


//==============================================================
void font::draw(color *screen, point<> p, const char *str)
{
	one_char.draw_type=draw_type;
	for (; *str; str++, p.x+=step)
	{
		one_char.bits=bits+BPP(((int)(*str-lower_char))*one_char.width);
		one_char.draw(screen, p);
	}
}






//==============================================================
//	јнимаци€
//==============================================================
animation::animation(UInt16 id, database *db, res_image *background)
	:window		(NULL)
	,n_frames	(ResLoadConstant(id+95))
	,current	(0)
	,wait		(0)
	,delay		((float)ResLoadConstant(id+99)/100.0f)
	,frames		(NULL)
{
	if (!id)	// пуста€ анимаци€ (в основном нужно дл€ пропуска групп поверхностей)
		return;

	int	i;
	MemHandle	handle;

	if (n_frames==0)
		n_frames=1;
	
	frames=new res_image*[n_frames];

	for (i=0; i<n_frames; i++)
		frames[i]=new res_image(id+i);
		
	//	если считать, что изображени€ хран€тс€ в запакованном
	// виде, то так мы будем "портить" только их распакованные
	// копии

	// наклейка изображени€ на фон
	if (handle=DmGetResource(constantRscType, id+98))
	{
		DmReleaseResource(handle);
		color	transparent=ResLoadConstant(id+98);

		if (background)
		{
			window=new segment_image(background, 
				rect<>(
					ResLoadConstant(id+96),
					ResLoadConstant(id+97),
					frames[0]->width,
					frames[0]->height
				));
			for (int i=0; i<n_frames; i++)
				frames[i]->stick_on(window, transparent);
		}
		else	// фон - первый кадр
			for (int i=1; i<n_frames; i++)
				frames[i]->stick_on(frames[0], transparent);
	}
/**/
}


//==============================================================
animation::~animation()
{
	if (window)
		delete window;
	
	if (frames)
	{
		for (int i=0; i<n_frames; i++)
			delete frames[i];
		delete[] frames;
	}
}


//==============================================================
void animation::anim(const real &time)
{
	if (n_frames<2)
		return;

	wait-=time;
	if (wait<0)
	{
		wait+=delay;
		if (++current==n_frames)
			current=0;
		set_frame(current);
	}
}


//==============================================================
void animation::set_frame(int n)
{
	window->copy(*frames[n]);
}