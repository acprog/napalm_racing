#include "car.h"
#include "cfg.h"
#include "flat3d.h"
#include "world.h"
#include "flat3drsc.h"

//-----------------------------------------------------------
// Путь
point<>	car::waypoints[MAX_WAYPOINTS];
car::way_params	car::way;
int	car::n_cars=0,
		car::n_laps=0,
		car::next_position=1;
res_image	car::final_lap(FinalLapBitmapFamily),
			car::rotate_back(RotateBackBitmapFamily);

//=========================================================
//	машина
//=========================================================
car::car(world *w, exterior *_ext, player_prefs *_prefs, bool human_player, float skill)
	:unit(w, _ext, Vector(), _prefs->Color)
	,top_accelerate	(skill*MAX_ACCELERATE)
	,back_accelerate	(top_accelerate)
	,top_speed			(skill*MAX_SPEED)
	,braking		(skill*MAX_BRAKING)
	,handing		(skill*MAX_HANDING)
	,comp			(!human_player)
	,target_point	(0)
	,lap		(1)
	,distance	(0)
	,prefs	(_prefs)
	,position	(_prefs->position)
	,ang_to_target	(0)
{
	if (w)
		w->cars.push_back(this);
	if (n_cars++==0)
	{
		next_position=1;
		rotate_back.draw_type=
		final_lap.draw_type=image::Or;
	}
	prefs->position=0;
}


//=========================================================
car::~car()
{
	n_cars--;
	if (prefs->position==0)
		prefs->position=position;
}


//=========================================================
void car::need_gear(const real &a)
{
	static	real zero=0;

	if (a>zero)
		accelerate=a*top_accelerate;
	else if (speed.x>0)
		accelerate=a*braking;
	else
		accelerate=a*back_accelerate;
}


//=========================================================
void car::process(const real &time, int f)
{
	if (z>0)	// машина была сброшена
	{
		z-=time*RESET_SPEED;
		if (z<0)
			z=0;
		else
			return;
	}

	// проверка попадания в контрольную область точки
	static	rect<real>	check_point(-CHECK_POINT_SIZE, -CHECK_POINT_SIZE, CHECK_POINT_SIZE*2, CHECK_POINT_SIZE*2);
	point<real>	to_target(waypoints[target_point].x, waypoints[target_point].y);
	
	if (hit(to_target, check_point))
		if (++target_point==way.n)
		{
			lap++;
			target_point=0;
			if (lap>n_laps)	// финиш
			{
				prefs->position=next_position++;
				//	выход с трассы:
				// приехал игрок или последняя машина
				if (!comp || prefs->position==n_cars)
					state=finish;
			}
		}

	distance=sqr((long)to_target.x)+sqr((long)to_target.y);
	ang_to_target=-angle+atan(to_target.x, to_target.y);

	int	old_d_angle=d_angle;
	// коэффициент переобразования руля
	real	angle_k=time*(100-10000/(100+sqr(speed.x/100)+sqr(speed.y/100)));

	if (comp)
		ai_control(time, angle_k, to_target);
	else
	{
		// перебор waypoint-ов вперед
		for (int i=1, n_wp=target_point; i<=MAX_SKIP_WAYPOINTS; i++)
		{
			if (++n_wp==way.n)
				n_wp=0;
			to_target.x=waypoints[n_wp].x;
			to_target.y=waypoints[n_wp].y;
	
			if (hit(to_target, check_point))
			{
				if (n_wp<target_point)	// перешел на следующий круг
					lap++;
				target_point=n_wp;
			}
		}

		d_angle=angle_k*d_angle;
		if (speed.x<0)
			d_angle=-d_angle;
	}
		
	// ограничение скорости
	if (speed.x>top_speed/(f*f) && accelerate>0)
		accelerate=0;
	unit::process(time, f);

	d_angle=old_d_angle;
}


//=========================================================
void car::ai_control(const real &time, real &angle_k, const point<real> &to_target)
{
	if (lap>n_laps)
	{
		// гонка закончена
		accelerate=(speed.x>0 ? -braking : 0);
		d_angle=0;
		return;
	}

	// ускорение на максимум
	accelerate=top_accelerate;
		
	// угол руля
	d_angle=ang_to_target;
	if (angle_k==0)
		d_angle=0;
	else
		d_angle=real(d_angle)/=angle_k;
	
	if (d_angle>handing)
	{
		d_angle=handing;
		if (speed.x>BRACKING_SPEED)
			accelerate=-braking;
		else	if (speed.y)
			accelerate=0;
	}
	if (d_angle<-handing)
	{
		d_angle=-handing;
		if (speed.x>BRACKING_SPEED)
			accelerate=-braking;
		else	if (speed.y)
			accelerate=0;
	}
	d_angle=angle_k*=d_angle;
}


//=========================================================
void car::friction(const real &f)
{
	speed.x=to_slow_down(speed.x, f*FRICTION_BACK);
	speed.y=to_slow_down(speed.y, f*FRICTION_CROSS);
}


//=========================================================
void car::draw_info(color *screen, font *f, bool selected)
{
#ifdef COLOR_8
	UInt8	color_sheme[]=
	{
		0x00,	//	белая
		0x00,	//	сине-белая
		0x12,	//	изумрудная
		0x5f,	//	синяя
		0xc0,	//	зеленая
		0xdf,	//	серебрянная
		0x7c,	//	красная
		0x8c,	//	охра
		0x73,	//	золотистая
		0x73,	//	черно-желтая
		0xdb	//	черная
	};
#endif

	int		shift=TEXTURE_SHIFT+MAP_SHIFT-MINIMAP_SHIFT;
	color	*p=screen+BPP(MINIMAP_LEFT+((long)x>>shift)+(((long)y>>shift)+MINIMAP_TOP)*SCREEN_WIDTH );

#ifdef COLOR_GRAY
	if (!selected)
		*p&=0xf0;
	else
	{
		*p=0;
		p[SCREEN_WIDTH/2]=0;
#endif

#ifdef COLOR_8
	if (!selected)
		*p=color_sheme[Color];
	else
	{
		p[0]=
		p[1]=
		p[SCREEN_WIDTH]=
		p[SCREEN_WIDTH+1]=
		color_sheme[Color];
#endif

#ifdef COLOR_16
	if (!selected)
		*p=0;//color_sheme[color];
	else
	{
		p[0]=
		p[1]=
		p[SCREEN_WIDTH]=
		p[SCREEN_WIDTH+1]=
		0;//color_sheme[color];
#endif

		static	char	str[20];
		
		// скорость
		int		s=speed.x*(g_prefs.kmh_speed ? TO_KMH_SPEED : TO_MH_SPEED);
		if (speed.x>0)
			StrPrintF(str, "%d.%d", s/10, s%10);
		else
			StrPrintF(str, "%d.%d", s/10, -s%10);
		f->draw(screen, point<>(4, 11), str);
		
		// круг
		if (lap>=n_laps)
			final_lap.draw(screen, point<>(4, 40));
		else
		{
			StrPrintF(str, "%d/%d", lap, n_laps);
			f->draw(screen, point<>(4, 40), str);
		}

		// позиция
		StrPrintF(str, "%d/%d", lap<=n_laps ? position : prefs->position, n_cars);
		f->draw(screen, point<>(4, 70), str);
		
		// повернись назад
		if (ang_to_target>(ANGLE_PI)-ANGLE_PI/4 && ang_to_target<ANGLE_PI+ANGLE_PI/4)
			rotate_back.draw(screen, point<>(72, 30));
	}
}



//=========================================================
void car::sound(const real &time)
{
	SndCommandType		sndCmd;
	sndCmd.cmd=sndCmdFrqOn;

#define	ABS(x)	((x)>0 ? (x) : -(x))
	//	частота
	if (speed.x==0 && speed.y==0)	// холостой ход
		sndCmd.param1=24;
	else	if (ABS(speed.x)>ABS(speed.y))	// двигатель
		sndCmd.param1=(2+ABS(speed.x)/1000)*12;
	else	// занос
		sndCmd.param1=2000;

	sndCmd.param3=PrefGetPreference(prefGameSoundVolume);
	sndCmd.param2=time*2000;	//	duration
	SndDoCmd(0, &sndCmd, true);
}


//=========================================================
void car::load_way(MemHandle h, point<real> &p, Angle &a)
{
	MemPtr	ptr=MemHandleLock(h);

	MemMove(&way, ptr, sizeof(way));
	MemMove(waypoints, (char*)ptr+sizeof(way), sizeof(point<>)*way.n);
	
	MemHandleUnlock(h);

	// перевод в мировые координаты
	for (int i=0; i<way.n; i++)
	{
		waypoints[i].x<<=TEXTURE_SHIFT;
		waypoints[i].y<<=TEXTURE_SHIFT;
	}
	
	if (way.n)
	{
		p.x=waypoints[0].x;
		p.y=waypoints[0].y;
		if (way.n>1)
			a=atan(waypoints[0].x-waypoints[way.n-1].x, waypoints[0].y-waypoints[way.n-1].y);
		waypoints[way.n]=waypoints[0];
	}
}


//=========================================================
void car::place(point<real> &p, Angle a)
{
	angle=a;
	x=p.x-=COS(a)*CARS_STEP;
	y=p.y-=SIN(a)*CARS_STEP;
	speed.x=speed.y=0;
	accelerate=0;
	target_point=0;
	lap=1;
}



//=========================================================
void car::reset()
{
	int	set_to=(target_point>0 ? target_point : way.n)-1;

	x=waypoints[set_to].x;
	y=waypoints[set_to].y;
	z=RESET_Z;
	speed.x=speed.y=0;
	ricochet_move=false;
	angle=atan(
		waypoints[target_point].x-waypoints[set_to].x, 
		waypoints[target_point].y-waypoints[set_to].y);
}


//=========================================================
void car::ricochet()
{
	if (comp)
		reset();
	else
		unit::ricochet();
}