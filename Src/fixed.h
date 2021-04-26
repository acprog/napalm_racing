/* naPalm Racing

  Copyright (C) 2004

  Author: Alexander Semenov <acmain@gmail.com>
*/

#ifndef _FIXED_H_
#define _FIXED_H_

#define	FRACT	16

#define ANGLE_90        16384U          // angle of 90 degrees
#define ANGLE_PI			32768U          // angle of 180 degress
#define ANGLE_180       32768U          // angle of 180 degress
#define ANGLE_270       49152U          // angle of 270 degress

#ifndef M_PI
#define M_PI            3.14159265358979323846
#endif

typedef unsigned short  Angle;




//============================================================
//	Число с фиксированной точкой	(смещение целой части, тип)
//============================================================
template <int shift=FRACT, class T=long>
class fixed
{
public:
	T	value;
	
public:
	//========================================================
	inline	fixed()
		:value	(0)
	{}

	//--------------------------------------------------------
	inline	fixed(int v)
		:value	( ((T)v)<<shift )
	{}

	//--------------------------------------------------------
	inline	fixed(unsigned long v)
		:value	( ((T)v)<<shift )
	{}

	//--------------------------------------------------------
	inline	fixed(long v)
		:value	( ((T)v)<<shift )
	{}

	//--------------------------------------------------------
	inline	fixed(float v)
		:value	( (T)(v*(float)(1L<<shift)) )
	{}
	
	//--------------------------------------------------------
	template <int shift2, class T2>
	inline	fixed(const fixed<shift2, T2> &v)
		:value	( shift>shift2 ? ((T)v.value)<<(shift-shift2) : (value>>(shift2-shift)) )
	{}
	
	//========================================================
	inline	fixed(const fixed<shift> &v)	
		:value	( v.value )
	{}	

	//--------------------------------------------------------
	inline	const fixed &operator=(int v)
	{
		value=(((T)v)<<shift);
		return *this;
	}

	//--------------------------------------------------------
	inline	const fixed &operator=(unsigned int v)
	{
		value=(((T)v)<<shift);
		return *this;
	}

	//--------------------------------------------------------
	inline	const fixed &operator=(long v)
	{
		value=(v<<shift);
		return *this;
	}
	
	//--------------------------------------------------------
	inline	const fixed &operator=(const float &v)	
	{
		value=(T)(v*(float)(1L<<shift));
		return *this;
	}
	
	//--------------------------------------------------------
	template <int shift2, class T2>
	inline	const fixed &operator=(const fixed<shift2, T2> &v)
	{
		value=( shift>shift2 ? (((T)v.value)<<(shift-shift2)) : (v.value>>(shift2-shift)) );
		return *this;
	}
	
	//========================================================
	inline	const fixed &operator=(const fixed &v)	
	{
		value=v.value;
		return *this;
	}	
/*	
	//--------------------------------------------------------
	template <class H>
	operator H() const
	{
		return value>>shift;
	}
/**/
	//--------------------------------------------------------
	inline	operator float() const
	{
		return ((float)value)/(float)(1L<<shift);
	}

	//--------------------------------------------------------
	inline	operator long() const
	{
		return value>>shift;
	}

	//--------------------------------------------------------
	inline	operator int() const
	{
		return value>>shift;
	}

	//--------------------------------------------------------
	inline	operator unsigned char() const
	{
		return value>>shift;
	}

	//--------------------------------------------------------
	inline	operator unsigned short() const
	{
		return value>>shift;
	}

	//========================================================
	const fixed &operator*=(const fixed &v)
	{
		#define	ABS(x)	((x)>=0 ? (x) : -(x))
		//#define	HALF	( 1L<<(shift/2) )

		if ( ABS(value) > ABS(v.value) )
		//if ( value>HALF || value<-HALF )
		{
			value>>=(shift/2);
			value*=v.value;
		}
		else
			value*=(v.value>>(shift/2));
		value>>=(shift-shift/2);
	/**/
//		value*=v.value;
//		value>>=shift;
		return *this;

		#undef	ABS
		//#undef HALF
	}

	//========================================================
	const fixed &operator*=(int v)
	{
		value*=v;
		return *this;
	}

	//========================================================
	const fixed &operator*=(unsigned int v)
	{
		value*=v;
		return *this;
	}

	//========================================================
	const fixed &operator*=(long v)
	{
		value*=v;
		return *this;
	}

	//========================================================
	const fixed &operator*=(signed char v)
	{
		value*=v;
		return *this;
	}


	//========================================================
	const fixed &operator*=(float v)
	{
		value*=v;
		return *this;
	}


	//========================================================
	inline	const fixed &operator/=(const fixed &v)
	{
		value/=v.value;
		value<<=shift;
		return *this;
	}

	//========================================================
	inline	const fixed &operator/=(int v)
	{
		value/=v;
		return *this;
	}

	//========================================================
	inline	const fixed &operator/=(long v)
	{
		value/=v;
		return *this;
	}


	//========================================================
	inline	const fixed &operator/=(unsigned int v)
	{
		value/=v;
		return *this;
	}


	//========================================================
	template <class H>
	inline	fixed operator/(H v) const
	{
		return fixed(*this)/=v;
	}


	//========================================================
	inline	const	fixed &operator+=(const fixed &v)
	{
		value+=v.value;
		return *this;
	}

	
	//========================================================
	inline	const fixed &operator-=(const fixed &v)
	{
		value-=v.value;
		return *this;
	}

	
	//========================================================
	inline	const fixed &operator-=(int v)
	{
		value-=((long)v)<<shift;
		return *this;
	}

/*
	//========================================================
	template <class H>
	inline	fixed operator*(const H &v)
	{
		return fixed(*this)*=v;
	}
/**/

	//========================================================
	template <class H>
	inline	fixed operator+(const H &v) const
	{
		return fixed(*this)+=v;
	}


	//========================================================
	template <class H>
	inline	fixed operator-(const H &v) const
	{
		return fixed(*this)-=v;
	}

	//========================================================
	inline	const	fixed &operator>>=(int s)
	{
		value>>=s;
		return *this;
	}


	//========================================================
	inline	const	fixed &operator>>(int s) const
	{
		return fixed(*this)>>=s;
	}



	/*
	//========================================================
	template <class H>
	inline	fixed operator-(const H &v)
	{
		return fixed(*this)-=v;
	}
/*
	//========================================================
	inline	bool operator>(long v)
	{
		return value>(v<<shift);
	}
/**/

	//========================================================
	inline	bool operator==(long v) const
	{
		return value==(v<<shift);
	}

	//========================================================
	inline	bool operator==(int v) const
	{
		return (value>>shift)==v;
	}


	//========================================================
	inline	bool operator==(const fixed &v) const
	{
		return value==v.value;
	}

/*
	//========================================================
	inline	bool operator!=(const fixed &v)
	{
		return value!=v.value;
	}

/*
	//========================================================
	inline	bool operator!=(long v)
	{
		return value!=(v<<shift);
	}
/**/
};


//========================================================
template <int shift, class T, class H>
inline	fixed<shift, T> operator*(const fixed<shift, T> &v1, const H &v2) const
{
	return fixed<shift, T>(v1)*=v2;
}
/**/
//========================================================
template <int shift, class T>
inline	bool operator<(const fixed<shift, T> &v1, const fixed<shift, T> &v2) const
{
	return v1.value<v2.value;
}


//========================================================
template <int shift, class T>
inline	bool operator<(const fixed<shift, T> &v1, int v2) const
{
	return v1.value<(((long)v2)<<shift);
}


//========================================================
template <int shift, class T>
inline	bool operator>(const fixed<shift, T> &v1, const fixed<shift, T> &v2) const
{
	return v1.value>v2.value;
}


//========================================================
template <int shift, class T>
inline	bool operator>(const fixed<shift, T> &v1, int v2) const
{
	return (v1.value>>shift)>v2;
}


//========================================================
template <int shift, class T>
inline	bool operator>(const fixed<shift, T> &v1, long v2) const
{
	return v1.value>(v2<<shift);
}


//========================================================
template <int shift, class T>
inline	bool operator!=(const fixed<shift, T> &v1, const fixed<shift, T> &v2) const
{
	return v1.value!=v2.value;
}


//========================================================
template <int shift, class T>
inline	bool operator==(const fixed<shift, T> &v1, const fixed<shift, T> &v2) const
{
	return v1.value==v2.value;
}



/*
//========================================================
template <int shift, class T>
inline	fixed<shift, T> operator-(const fixed<shift, T> &v1, const fixed<shift, T> &v2) const
{
	return fixed<shift, T>(v1)-=v2;
}
/**/


//================================================================

float sin(float x);
float cos(float x);

extern  fixed<> *sinTable;
extern  fixed<> *cosTable;

#define SIN(x)	sinTable[(x)>>6]
#define COS(x)	cosTable[(x)>>6]

Angle atan(fixed<> x, fixed<> y);

//================================================================

void    initFixMath ();
void    destroyFixMath ();

//================================================================

#include "geometric.h"
typedef fixed<>	real;	// в последствии fixed будет заменено на float
typedef vector<real>	Vector;
typedef point<real>	Point;

#endif	// _FIXED_H_
