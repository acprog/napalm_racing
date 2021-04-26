#include "image.h"

//==============================================================
//	Изображение
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
//	Изображение из ресурса
//==============================================================
res_image::res_image(UInt16 id)
	:handle	(DmGetResource(bitmapRsc, id))
	,bitmapP((BitmapType*)MemHandleLock(handle))
{
	// т.к. BmpGetDimensions() не везде работает:
	struct BitmapSize
	{
		Int16	width;
		Int16	height;
	}*s=(BitmapSize*)bitmapP;

	width=s->width;
	height=s->height;
	bits=(color*)BmpGetBits(bitmapP);
}



//==============================================================
res_image::~res_image()
{
	bits=NULL;
	MemHandleUnlock(handle);
	DmReleaseResource(handle);
}

	

//==============================================================
//	Шрифт
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

