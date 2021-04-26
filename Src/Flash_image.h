#ifndef _FLASH_IMAGE_H_
#define _FLASH_IMAGE_H_

#include "image.h"
#include "fixed.h"

//=========================================================
//	Вспыхивающая картинка
//=========================================================
class flash_image : public res_image
{
private:
	color	lo,
			hi;
public:
	flash_image(UInt16 id)
		:res_image(id)
		,lo	(0)
		,hi	(0)
	{}
	
	//----------------------------------------------------
	void process(const real &time)
	{
	#ifdef COLOR_GRAY
		lo=time*15;
		if ((signed char)lo<1)
			lo=1;
		else if (lo>15)
			lo=15;
		hi=lo<<4;
	#endif
	#ifdef COLOR_8
		if (time<2)
			lo=time*3;
		else
			lo=5;
		lo+=0x72;
	#endif
	}
	
	//----------------------------------------------------
	void draw(color *screen, const point<> &v)
	{
		int 	x, y;
		color	*p=bits;

		screen+=SCREEN_OFFSET(v.x, v.y);
		for (y=height; y--; screen+=BPP(SCREEN_WIDTH-width), p+=BPP(offset))
			for (x=BPP(width); x--; screen++, p++)
			#ifdef COLOR_GRAY
			{
				if ( (*p) & 0xf)
					(*screen)|=lo;
				if ( (*p) & 0xf0)
					(*screen)|=hi;
			}
			#endif

			#ifdef COLOR_8
			if (*p)
			{
				(*screen)=0;
				(*screen)|=lo;
			}
			#endif

			#ifdef COLOR_16
			;
			#endif
	}
};


#endif	// _FLASH_IMAGE_H_