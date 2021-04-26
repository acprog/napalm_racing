/* naPalm Racing

  Copyright (C) 2004

  Author: Alexander Semenov <acmain@gmail.com>
*/
#include <PalmOS.h>
#include "Flat3DRsc.h"

#include "fixed.h"

fixed<> *sinTable;
fixed<> *cosTable;
Angle *atanTable;


//============================================================
float sin(float x)
{
	float s=0, d=x;

	int i=1;
	while (i<10)
	{
		s+=d;
		d*=x/(++i);
		d*=x/(++i);
		s-=d;
		d*=x/(++i);
		d*=x/(++i);
	}

	return s;
}


//============================================================
float cos(float x)
{
	float s=0, d=1;

	int i=0;
	while (i<10)
	{
		s+=d;
		d*=x/(++i);
		d*=x/(++i);
		s-=d;
		d*=x/(++i);
		d*=x/(++i);
	}

	return s;
}


//============================================================
Angle atan(fixed<> x, fixed<> y)
{
	// таблица для atan(-1 : 1) => [-pi/4 : pi/4]
	if (x>y)
		if (x.value>-y.value)
			return atanTable[y.value/(x.value>>9)+512];
		else
			return atanTable[-x.value/(y.value>>9)+512]+3*(ANGLE_PI/2);
	else
		if (-x.value>y.value)
			return atanTable[y.value/(x.value>>9)+512]+ANGLE_PI;
		else
			return atanTable[-x.value/(y.value>>9)+512]+ANGLE_PI/2;
}


//============================================================
void initFixMath()
{
	long 	PI=512;
	
	sinTable=new fixed<>[1024];
	cosTable=new fixed<>[1024];
	atanTable=new Angle[1025];

	for (int i=0; i<1025; i++)
		atanTable[i]=0;

	for (int i=0; i<PI/2; i++)
   {
		float	x  = i * 2 * M_PI / (1024.0),
				sx = sin ( x ),
				cx = cos ( x );
		fixed<>	zero=0;

		sinTable[i]=sx;
		sinTable[PI-i-1]=sinTable[i];
		sinTable[PI+i]=zero-sinTable[i];
		sinTable[2*PI-i-1]=zero-sinTable[i];

		cosTable[i]=cx;
		cosTable[2*PI-i-1]=cosTable[i];
		cosTable[PI+i]=zero-cosTable[i];
		cosTable[PI-i-1]=zero-cosTable[i];
		
		if (i<=PI/4)
			atanTable[ 512+(int)(512.0*sx/cx) ]=(i<<6);
	}
	
	// заполнение пробелов в таблице atan
	int	first=512,
			last=first,
			d;
	while(first<1024)
	{
		// ищем следующее значение
		while (!atanTable[++last]);
		// заполняем таблицу
		d=(atanTable[last]-atanTable[first])/(last-first);
		while (++first<last)
			atanTable[first]=atanTable[first-1]+d;
	}
	atanTable[0]=57343;
	atanTable[1024]=8192;

	for (int i=0; i<512; i++)
		atanTable[i]=0xffff-atanTable[1024-i];
}


//============================================================
void    destroyFixMath()
{
	delete[] sinTable;
	delete[] cosTable;
	delete[] atanTable;
}