/* naPalm Racing

  Copyright (C) 2004

  Author: Alexander Semenov <acmain@gmail.com>
*/
#ifndef _MATRIX_H_
#define _MATRIX_H_

#include "Fixed.h"
#include "geometric.h"
#include "cfg.h"

typedef	fixed<FRAC_SHIFT>	m_fixed;

//===========================================================
//	Матрицы и все что с ними связано
//===========================================================
template <class T=m_fixed>
class matrix
{
public:
	T	t[4][4];

public:
	//========================================================
	inline void clear()
	{
		T	*p1=&t[0][0];
		for (int i=16; i--;)
			(*p1++)=0;
	}

	virtual	~matrix(){}

	//========================================================
	// единичная
	matrix()
	{
		clear();
		t[0][0]=t[1][1]=t[2][2]=t[3][3]=1;
	}

	//===========================================================
	// копия
	template <class H>
	matrix(const matrix<H> &m)
	{
		T	*p0=&t[0][0];
		const H		*p1=&m.t[0][0];
		for (int i=16; i--;)
			*(p0++)=*(p1++);
	}

	//========================================================
	// смещение
	matrix(const Vector &offset)
	{
		clear();

		t[0][0]=t[1][1]=t[2][2]=t[3][3]=1;
		t[0][3]=offset.x;
		t[1][3]=offset.y;
		t[2][3]=offset.z;
	}

	//========================================================
	// последовательные повороты вокруг X, Y, Z
	matrix(Angle angle_x, Angle angle_y=0, Angle angle_z=0)
	{
	// тут последовательность неправильная. потом заменить
		T	cosinus, sinus, zero=0;
		clear();

		if (angle_z)
		{
			cosinus=COS(angle_z);
			sinus=SIN(angle_z);
			
			t[2][2]=t[3][3]=1;
			t[0][0]=cosinus;
			t[0][1]=zero-sinus;
			t[1][0]=sinus;
			t[1][1]=cosinus;
	
			if (angle_x || angle_y)
				(*this)*=matrix(angle_x, angle_y);
		}
		else if (angle_y)
		{
			cosinus=COS(angle_y);
			sinus=SIN(angle_y);
			
			t[1][1]=t[3][3]=1;
			t[0][0]=cosinus;
			t[0][2]=sinus;
			t[2][0]=zero-sinus;
			t[2][2]=cosinus;
		
			if (angle_x)
				(*this)*=matrix(angle_x);
		}
		else if (angle_x)
		{	
			cosinus=COS(angle_x);
			sinus=SIN(angle_x);
			
			t[0][0]=t[3][3]=1;
			t[1][1]=cosinus;
			t[1][2]=sinus;
			t[2][1]=zero-sinus;
			t[2][2]=cosinus;
		} 
		else
			t[0][0]=t[1][1]=t[2][2]=t[3][3]=1;	// в единичную
	}
	
	//========================================================
	// быстрая матрица разворота вокруг точки x, y и оси Z
	matrix(const Vector &offset, Angle angle_z)
	{
		T	cosinus, sinus, zero=0;
		clear();

		t[2][2]=t[3][3]=1;

		t[0][3]=offset.x;
		t[1][3]=offset.y;
		t[2][3]=offset.z;

		if (angle_z)
		{
			cosinus=COS(angle_z);
			sinus=SIN(angle_z);
			
			t[0][0]=cosinus;
			t[0][1]=zero-sinus;
			t[1][0]=sinus;
			t[1][1]=cosinus;
		}
		else
			t[0][0]=t[1][1]=t[2][2];
	}
	

	//========================================================
	// масштаб
	matrix(volume<float> scale)
	{
		clear();
		t[0][0]=scale.width;
		t[1][1]=scale.height;
		t[2][2]=scale.length;
		t[3][3]=1;
	}


	//========================================================
	matrix(float perspective)
	{
		clear();
		t[0][0]=t[1][1]=t[2][2]=t[3][3]=1;
		t[3][2]=perspective;
	}


	//========================================================
	const matrix &operator*=(const matrix &m)
	{
		static	T	zero=0, one=1;
		matrix	res(*this);
	
		int	i, j, k;
		T		summ;
		for (i=0; i<4; i++)
			for (j=0; j<4; j++)
			{
				for (summ=0, k=0; k<4; k++)
					if (res.t[i][k]!=zero && m.t[k][j]!=zero)
						if (res.t[i][k]==one)
							summ+=m.t[k][j];
						else if (m.t[k][j]==one)
							summ+=res.t[i][k];
						else
							summ+=res.t[i][k]*m.t[k][j];
				t[i][j]=summ;
			}
		
		return *this;
	}


	//========================================================
	// перевод вершины модели в экранные координаты
	void transform(const Vertex &src, point<Int16> *dst) const
	{
		// координаты модели соответствуют текстуре
		long
			x=t[0][0].value*src.x + t[0][1].value*src.y + t[0][2].value*src.z + t[0][3].value,
			y=t[1][0].value*src.x + t[1][1].value*src.y + t[1][2].value*src.z + t[1][3].value,
			denom=t[3][0].value*src.x + t[3][1].value*src.y + t[3][2].value*src.z + t[3][3].value;

		dst->x=x/denom;
		dst->y=y/denom;
	}
};

#endif // _MATRIX_H_