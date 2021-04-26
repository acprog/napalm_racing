/* naPalm Racing

  Copyright (C) 2004

  Author: Alexander Semenov <acmain@gmail.com>
*/
/***********************************************************

  Здесь содержатся классы точек, прямоугольных областей

***********************************************************/

#ifndef GEOMETRIC_HEADER
#define GEOMETRIC_HEADER

namespace	geometric
{

//#include <property.h>

//========================================================================================
//	точка на плоскости
//========================================================================================
template <class X=int, class Y=X>
class	point
{
public:
	//=============================================
	X	x;
	Y	y;
	
	//----------------------------------------------
	// маски расположения точки относительно этой. (можно применять и для rect)
	enum	position
	{
		inside=0x0,		//	0000
		//outside=0xf,	//	1111
		right=0x1,		//	0001
		right_top=0x3,	//	0011
		top=0x2,		//	0010
		left_top=0x6,	//	0110
		left=0x4,		//	0100
		left_bottom=0xc,//	1100
		bottom=0x8,		//	1000
		right_bottom=0x9//	1001
	};

	//=============================================
	inline	point()
		:x(0)
		,y(0)
	{
	}

	//-------------------------------------
	inline	point(X _x, Y _y)
		:x(_x)
		,y(_y)
	{
	}

	//-------------------------------------
	inline	point(const point &_point)
		:x(_point.x)
		,y(_point.y)
	{
	}

	//-------------------------------------
	inline	const point<X, Y> &operator()(X _x, Y _y)
	{
		x=_x;
		y=_y;
		return *this;
	}

	//-------------------------------------
	inline	const point<X, Y> &operator+=(const point &p)
	{
		x+=p.x;
		y+=p.y;
		return *this;
	}

	//-------------------------------------
	inline	const point<X, Y> &operator-=(const point &p)
	{
		x-=p.x;
		y-=p.y;
		return *this;
	}

	//-------------------------------------
	inline	point<X, Y> operator+(const point &p)
	{
		return point<X, Y>(x+p.x, y+p.y);
	}


	//-------------------------------------
	inline	point<X, Y> operator-(const point &p)
	{
		return point<X, Y>(x-p.x, y-p.y);
	}

	//-------------------------------------
	inline	point<X, Y>::position order(const point<X, Y> &p) const
	{
		return	(position)(	( p.x<x ? left : ( p.x>x ? right : inside ))|
									( p.y<y ? top : ( p.y>y ? bottom : inside )));
	}

	//-------------------------------------
	template <class T>
	inline	const point<X, Y> &operator*=(T mult)
	{
		x*=mult;
		y*=mult;
		return *this;
	}

	//-------------------------------------
	template <class T>
	inline	const point<X, Y> &operator/=(T divd)
	{
		x/=divd;
		y/=divd;
		return *this;
	}

	//-------------------------------------
	template <class T>
	inline	point<X, Y> operator*(T mult)
	{
		return point<X, Y>(x*mult, y*mult);
	}

	//-------------------------------------
	template <class T>
	inline	point<X, Y> operator/(T divd)
	{
		return point<X, Y>(x/divd, y/divd);
	}
};	// point










//========================================================================================
//	размер на плоскости
//========================================================================================
template <class X=int, class Y=X>
class	size
{
public:
	X	width;
	Y	height;
	
	//-------------------------------------
	inline	size()
		:width(0)
		,height(0)
	{
	}

	//-------------------------------------
	inline	size(X _width, Y _height)
		:width(_width)
		,height(_height)
	{
	}

	//-------------------------------------
	inline	size(const size &s)
		:width(s.width)
		,height(s.height)
	{
	}
};	// size










//========================================================================================
//	прямоугольная область
//========================================================================================
template <class X=int, class Y=X>
class	rect : public point<X, Y>, public size<X, Y>
{
public:
	//-------------------------------------------------------
	inline	rect()
	{
	}

	//-------------------------------------------------------
	inline	rect(const rect &_rect)
		:point(_rect)
		,size(_rect)
	{
	}

	//-------------------------------------------------------
	inline	rect(X _left_pos, Y _up_pos, X _width, Y _height)
		:point(_left_pos, _up_pos)
		,size(_width, _height)
	{
	}

	//-------------------------------------------------------
	inline	rect(const point<X, Y> &_left_up, X _width, Y _height)
		:point(_left_up)
		,size(_width, _height)
	{
	}

	//-------------------------------------------------------
	inline	rect(const point<X, Y> &_left_up, size<X, Y> _size)
		:point(_left_up)
		,size(_size)
	{
	}

	//-------------------------------------------------------
	inline	rect(const point<X, Y> &left_up, const point<X, Y> &right_down)
		:point(_left_up)
		,size(_right_down.x-x, _right_down.y-y)
	{
	}

	//-------------------------------------------------------
	inline	void set_left_up(const point<X, Y> &p)
	{
		width+=x-p.x;
		height+=y-p.y;
		(*this)=p;
	}

	//-------------------------------------------------------
	inline	void set_left_down(const point<X, Y> &p)
	{
		width+=x-p.x;
		height=p.y-y;
		x=p.x;
	}

	//-------------------------------------------------------
	inline	void set_right_up(const point<X, Y> &p)
	{
		width=p.x-x;
		height+=y-p.y;
		y=p.y;
	}

	//-------------------------------------------------------
	inline	void set_right_down(const point<X, Y> &p)
	{
		width=p.x-x;
		height=p.y-y;
	}

	//-------------------------------------------------------
	inline	int left_pos()
	{
		return x;
	}

	//-------------------------------------------------------
	inline	int right_pos()
	{
		return x+width;
	}

	//-------------------------------------------------------
	inline	int up_pos()
	{
		return y;
	}

	//-------------------------------------------------------
	inline	int down_pos()
	{
		return y+height;
	}

	//-------------------------------------
	inline	point<X, Y>::position order(const point<X, Y> &p) const
	{
		return	(position)(	( p.x<x ? left : ( p.x>x+width ? right : inside ))|
									( p.y<y ? top : ( p.y>y+height ? bottom : inside )));
	}

	//-------------------------------------
	inline	point<X, Y>::position order(const rect<X, Y> &r) const
	{
		return	(position)(	( r.x<x ? left : inside )|
									( r.x+r.width>x+width ? right : inside )|
									( r.y<y ? top : inside)|
									( r.y+r.height>y+height ? bottom : inside ));
	}
};	// rect






//========================================================================================
//	точка в пространстве
//========================================================================================
template <class X=float, class Y=X, class Z=Y>
class vector : public point<X, Y>
{
public:
	Z	z;
	
	//-------------------------------------------------------------------------
	inline	vector()
		:z(0)
	{
	}

	//-------------------------------------------------------------------------
	inline	vector(const X *xyz)
		:point(xyz[0], (Y)xyz[1])
		,z	((Z)xyz[2])
	{
	}

	//-------------------------------------------------------------------------
	inline	vector(X _x, Y _y, Z _z=0)
		:point(_x, _y)
		,z(_z)
	{
	}

	//-------------------------------------------------------------------------
	inline	vector(const vector &v)
		:point(v)
		,z(v.z)
	{
	}

	//-------------------------------------------------------------------------
	inline	const vector<X, Y, Z> &set(X _x, Y _y, Z _z=0)
	{
		x=_x;
		y=_y;
		z=_z;
		return *this;
	}

	//-------------------------------------------------------------------------
	inline	operator const X*() const
	{
		return (const X*)&x;
	}

	//-------------------------------------------------------------------------
	inline	const vector<X, Y, Z> &operator=(X *xyz)
	{
		x=xyz[0];
		y=(Y)xyz[1];
		z=(Z)xyz[2];
		return *this;
	}

	//-------------------------------------------------------------------------
	inline	const vector<X, Y, Z> &operator-=(const vector<X, Y, Z> &v)
	{
		x-=v.x;
		y-=v.y;
		z-=v.z;
		return *this;
	}

	//-------------------------------------------------------------------------
	template<class T>
	inline	const vector<X, Y, Z> &operator*=(T mult)
	{
		x*=mult;
		y*=mult;
		z*=mult;
		return *this;
	}
	
	/*
	point<T> xy(T x_c=1, T y_c=1);
	point<T> yx(T x_c=1, T y_c=1);
	point<T> xz(T x_c=1, T y_c=1);
	point<T> zx(T x_c=1, T y_c=1);
	point<T> yz(T x_c=1, T y_c=1);
	point<T> zy(T x_c=1, T y_c=1);
	/**/
};








//========================================================================================
//	размер в пространстве
//========================================================================================
template <class X=int, class Y=X, class Z=Y>
class	volume : public size<X, Y>
{
public:
	Z	length;
	
	//-------------------------------------
	inline	volume()
		:length	(0)
	{
	}

	//-------------------------------------
	inline	volume(X _width, Y _height, Z _length=0)
		:size		(_width, _height)
		,length	(_length)
	{
	}

	//-------------------------------------
	inline	volume(const volume &s)
		:size		(s)
		,length	(s.length)
	{
	}
};	// volume




}

using namespace geometric;
#endif GEOMETRIC_HEADER