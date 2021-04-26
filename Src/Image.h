/* naPalm Racing

  Copyright (C) 2004

  Author: Alexander Semenov <acmain@gmail.com>
*/
#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <PalmOS.h>
#include "geometric.h"
#include "cfg.h"

// ���������� ��� animation
#include "fixed.h"
#include "database.h"

//==============================================================
//	�����������
//==============================================================
class image : public size<>	// ������ � ������
{
protected:
	color	*bits;
	bool	free_bits;	// �������� ����� bits
	int		offset;	// �������� �� ����� ���������� ������ �� ������ ���������
					// � ������

	//-----------------------------------------------------------
	void add(color *screen, const point<> &p);	// ���������� �����
	void put(color *screen, const point<> &p);	// ���������
	void or_(color *screen, const point<> &p);

public:
	image();
	image(const size<> &s, color *_bits=NULL, int _offset=0);
	virtual	~image();

	void copy(const image &img);	// ����������� �����������

	// "��������" �� ������� �������
	void	stick_on(image *img, color transparent);

	//----------------------------------------------------------
	void clear(color fill)
	{
		UInt32	*p=(UInt32*)bits,
			f=fill;
		f|=(f<<4);
		f|=(f<<8);
		f|=(f<<16);
		for (int i=BPP(width*height)>>2; i--;)
			*(p++)=f;
	}

	//------------------------------------------------------------
	inline	color &operator[](int i)
	{
		return bits[i];
	}

	//------------------------------------------------------------
	inline	color &operator[](const point<> &p)
	{
		return bits[BPP( p.x + (p.y*(width+offset)) )];
	}


	//------------------------------------------------------------
	inline	void put_pixel(const point<> &p, color c)
	{
	#if (COLOR_DEPTH==4)
		color	*pix=&((*this)[p]);
		if (p.x & 1)	// ��������
		{
			*pix&=0xf0;
			*pix|=c;
		}
		else
		{
			*pix&=0xf;
			*pix|=(c<<4);			
		}
	#else
		(*this)[p]=c;
	#endif
	}

	//------------------------------------------------------------
	inline	operator color*()
	{
		return bits;
	}

	//============================================================
	// ���� ���������
	enum	draw_types
	{
		Put,
		Add,
		Or
	}draw_type;
	
	//------------------------------------------------------------
	inline	void draw(color *screen, const point<> &p)
	{
		switch(draw_type)
		{
		case Add:
			add(screen, p);
			break;

		case Or:
			or_(screen, p);
			break;

		case Put:
			put(screen, p);
			break;
		}
	}

	
	friend class segment_image;
};


//==============================================================
//	����������� �� �������
//==============================================================
class res_image : public image
{
private:
	BitmapType	*bitmapP;

public:
	res_image(UInt16 id, database *db=NULL);
	virtual	~res_image();

	//----------------------------------------------------
	inline	void WinDrawBitmap(const point<> &p)
	{
		::WinDrawBitmap(bitmapP, p.x, p.y);
	}	
};



//==============================================================
//	����� �����������
//==============================================================
class segment_image : public image
{
public:
	//----------------------------------------------------------
	segment_image(res_image *parent, const rect<> &r)
		:image(r, parent->bits+BPP(r.x+r.y*parent->width), parent->width-r.width)
	{
		draw_type=parent->draw_type;
	}
	
	//----------------------------------------------------------
	virtual	~segment_image()
	{
		bits=NULL;	// ���-�� �� ������ �������
	}

	friend class font;
};




//==============================================================
//	�����
//==============================================================
class font : public res_image
{
private:
	segment_image	one_char;	// ��� �������� ��������� ����
	UInt8	lower_char,
			step;	// ��� ����� �������

public:
	font(UInt16 id, char first, char last, int _step);	// ����� � ������ �� ��������� ������������
	virtual	~font(){}
	void draw(color *screen, point<> p, const char *str);
};



//==============================================================
//	��������
// ����: ������ ��������� ���:
//	 1. �������� �� ���� (���� ������� �������)
//	 2. �������� ������� (������� �/� ������ ����)
//==============================================================
class animation
{
private:
	segment_image	*window;// ���� ��� ��������� �� ������� �������.
	int			n_frames,	// ���-�� ������
				current;	// �������
	real		wait,		// ������� �������� ����� �� ����� ������
				delay;		// ����� ����� �������

protected:
	res_image	**frames;	// �����
				
	//--------------------------------------------------------
public:
	animation(UInt16 id, database *db=NULL, res_image *background=NULL);
	virtual	~animation();
	void anim(const real &time);
	virtual	void set_frame(int n);
};


#endif	// _IMAGE_H_