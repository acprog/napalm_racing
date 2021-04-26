#ifndef	_CAMERA_H_
#define	_CAMERA_H_

#include "object.h"
#include "matrix.h"
#include <list>
using std::list;


//=========================================================
//	����� ����������
//=========================================================
class	camera : public object
{
protected:
	Angle	look_angle;
	int		distance;	// ���������� �� �������
	object	*look_to;
	matrix<float>	projection;	// ������� ��������
public:
	bool	mount_to_obj;	// ������������ �� ������
	int		d_angle;	// ������������ ���������� ���� ������
	
	//----------------------------------------------------------
private:
	void make_projection();

	//==========================================================
public:
	// ������ ������������ �� ������
	camera(object *_look_to=NULL, int _z=0, int _distance=0, Angle _look_angle=0);
	// ����������� ������. � ������ - ������
	camera(object *_look_to, const Vector &loc);
	// ��������� ������
	camera(const Vector &loc, Angle _look_angle=0);
	// ��� ����� ������� ���������� ������, �� ���-������ �����
	virtual ~camera(){}

	void mount(object *_look_to, int _z, int _distance, Angle _look_angle=0);
//	void center(object *_lock_to);

	//-----------------------------------------------------------	
	void process(const real &time);
	void draw(color *screen, list<object*> &objects);
	
	friend class world;
	friend class ground;
};




//=========================================================
//	������������
//=========================================================
class	preview : public camera
{
private:
	matrix<>	m_fast;
	real		real_angle;

public:
	preview(object *_look_to, int _z, int _distance, Angle _d_angle=20000);
	virtual	~preview(){}
	void draw(color *screen);
	void process(const real &time);
};




#endif	//	_CAMERA_H_