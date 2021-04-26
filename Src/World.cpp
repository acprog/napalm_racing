/* naPalm Racing

  Copyright (C) 2004

  Author: Alexander Semenov <acmain@gmail.com>
*/
#include "world.h"
#include "Flat3DRsc.h"
#include "Flat3D.h"
#include "model.h"

enum world_state	state;

//=====================================================
//	Плоский мир
//=====================================================
world::world(db_info *track, int n_laps, player_prefs *players, int n_players, int opponents_skill, bool demo)
	:world_base	(new database(WORLD_TYPE, appFileCreator))
	,water_join	(WaterBitmapFamily, 30)
	,bridge_join(BridgeBitmapFamily, 14)
	,grass_join	(GrassJoinBitmapFamily, 22)
	,grass		(GrassBitmapFamily, 22)
	,road_join0	(RoadBitmapFamily, 10)
	,road_marks0(RoadMarks0BitmapFamily, 10)
	,sand		(SandBitmapFamily, 18)
	,road_join1	(BrRoadBitmapFamily, 12)
	//special_join,
	,special	(SpecialBitmapFamily, 1)
	,road_join2	(Road3BitmapFamily, 10)
	,road_join3	(Road4BitmapFamily, 10)
	,minimap	(size<>(MINIMAP_SIZE, MINIMAP_SIZE))
	,sky	(SkyBitmapFamily, world_base)
	,devil	(&car_high, &car_med, &car_low, &car_solid)
	,numbers	(NumbersBitmapFamily, '-', '9', 8)
	,small_numbers	(SmallNumbersBitmapFamily, '-', '9', 8)
	,fps		(FPSBitmapFamily)
	,speed	(SpeedBitmapFamily)
	,lap		(LapBitmapFamily)
	,position(PositionBitmapFamily)
	,active_font	(g_prefs.small_font ? &small_numbers : &numbers)
	,big_three	(ThreeBitmapFamily)
	,big_two	(TwoBitmapFamily)
	,big_one	(OneBitmapFamily)
	,big_go		(GoBitmapFamily)
	,big_finish	(FinishBitmapFamily)
{
	delete	world_base;
	sky.draw_type=image::Put;

	big_three.draw_type=
	big_two.draw_type=
	big_one.draw_type=
	big_go.draw_type=image::Or;

	small_numbers.draw_type=image::Or;
	if (!g_prefs.minimap.transparent)
		minimap.draw_type=image::Put;
	
	float	skills[]={0.8f, 1.0f, 1.2f};
	for (; n_players--; players++)
		if (players->id!=0)
			new car(this, &devil, players, false,
				skills[opponents_skill]);
		else
			cam.mount(
				player=new car(this, &devil, players, !demo),
				CAMERA_H, 87);

	car::n_laps=n_laps;
	load_track(track);
}


//=====================================================
world::~world()
{
	for (obj_iter i=objects.begin(); i!=objects.end(); i++)
		delete (*i);
}


//=====================================================
void world::load_track(db_info *track)
{
	DmOpenRef db=DmOpenDatabase(track->card, DmFindDatabase(track->card, track->name), dmModeReadOnly);

	MemHandle	h;
	UInt16		index=DmNumRecords(db);
	
	// поверхность
	h=DmGetRecord(db, --index);
	ground::unpack_ground(h);
	DmReleaseRecord(db, index, true);

	// дорога
	h=DmGetRecord(db, --index);
	ground::unpack_road(h);
	DmReleaseRecord(db, index, true);

	// путь
	h=DmGetRecord(db, --index);
	point<real>	p;
	Angle	inital_angle;
	car::load_way(h, p, inital_angle);
	DmReleaseRecord(db, index, true);

	DmCloseDatabase(db);
	
	ground::normalize_map();
	ground::make_minimap(minimap);

	// расстановка машин	
	for (car_iter i=cars.begin(); i!=cars.end(); i++)
		(*i)->place(p, inital_angle);

	state=move_to_car;
	cam.mount_to_obj=false;
}



//=====================================================================
void world::process(const real &time)
{
	//--------------------------------------------------------
	//	Управление по кнопкам
	if (!g_prefs.analog_control)
	{
		UInt32	press=KeyCurrentState();

		//------------------------------------------------------
		//	газ/тормоз
		if (press & GEAR_UP)
			analog_gear(1);
		else	if (press & GEAR_DOWN)
			analog_gear(-0.5f);
		else
			free_gear();

		//----------------------------------------------------
		//	руль			
		static	real	rudder=0,
							need_rudder=0,
							d_rudder[]={1, 0.5f, 0.25f, 0.2f, 0.1f, 0.05f, 0.025f, 0.02f, 0.01f};

		if (press & RUDDER_LEFT)
			need_rudder=-1;
		else	if (press & RUDDER_RIGHT)
			need_rudder=1;
		else
			need_rudder=0;

		if (rudder>need_rudder)
			analog_rudder(rudder-=d_rudder[g_prefs.rudder_speed]);
		else	if (rudder<need_rudder)
			analog_rudder(rudder+=d_rudder[g_prefs.rudder_speed]);

		//--------------------------------------------------------
		// другие кноки

		// сброс
		if (press & RESET_KEY && state==racing && player->z==0)
			player->reset();

		// смотреть назад
		static	bool	look_back=false;
		if (press & LOOK_BACK)
		{
			if (!look_back)
			{
				cam.mount_to_obj=false;
				cam.look_angle=ANGLE_PI;
				look_back=true;
			}
		}
		else	if (look_back)
		{
			cam.mount_to_obj=true;
			cam.look_angle=0;
			look_back=false;
		}
	}

	//--------------------------------------------------------
	// обработка состояния
	static	real	t_wait=0;
	switch (state)
	{
	case finish:
		big_finish.process(t_wait);
	
	case move_out_car:
		cam.mount_to_obj=false;
		cam.z=CAMERA_H-(int)(t_wait*4);
		cam.look_angle=t_wait*(ANGLE_PI/3);
		t_wait+=time;
		if (t_wait>3)
		{
//			state=pause;
			t_wait=0;
			FrmGotoForm(RaceResultsForm);
		}

	case racing:
		for (unit_iter i=units.begin(); i!=units.end(); i++)
			ground::process(*i, time);

		for (unit_iter i=units.begin(); i!=units.end(); i++)
			for (unit_iter j=i; ++j!=units.end();)
				(*i)->impact(*j);
		break;

	case move_to_car:
		cam.z=CAMERA_H+20-(int)(t_wait*10);
		t_wait+=time;
		if (cam.z<CAMERA_H)
		{
			t_wait=0;
			state=wait_3;
			cam.mount_to_obj=true;
			cam.z=CAMERA_H;
		}
		break;

	case wait_3:
		big_three.process(t_wait);
		t_wait+=time;
		if (t_wait>1)
		{
			t_wait=0;
			state=wait_2;
		}
		break;

	case wait_2:
		big_two.process(t_wait);
		t_wait+=time;
		if (t_wait>1)
		{
			t_wait=0;
			state=wait_1;
		}
		break;
		
	case wait_1:
		big_one.process(t_wait);
		t_wait+=time;
		if (t_wait>1)
		{
			t_wait=0;
			state=wait_go;
		}
		break;

	case wait_go:
		big_go.process(t_wait);
		t_wait+=time;
		if (t_wait>1)
		{
			t_wait=0;
			state=racing;
		}
		break;
	}

	cam.process(time);

	ground::animate(time);//water_join.process(time);
	sky.animate(time);
	
	//--------------------------------------------------
	if (!g_prefs.mute_sound)
		player->sound(time);
	
	//--------------------------------------------------
	// расстановка по местам
	static	int	refresh_position=0;
	if (refresh_position--==0)
	{
		refresh_position=REFRESH_POSITIONS;
		cars.sort(car::compare_positions);
		int pos=0;
		for (car_iter i=cars.begin(); i!=cars.end(); i++)
			(*i)->position=++pos;
	}
}



//=====================================================================
// замер FPS
void world::draw_fps(color *screen)
{
	static	UInt32	ticks=0;
	static	int		frames=0;
	static	char		str[20]="";
	
	if (TimGetTicks()>=ticks)
	{
		ticks=TimGetTicks()+SysTicksPerSecond()*10;
		StrPrintF(str, "%d.%d", frames/10, frames%10);
		frames=0;
	}
	frames++;
	fps.draw(screen, point<>(120, 80));
	active_font->draw(screen, point<>(120, 90), str);
}


//=====================================================================
void world::draw(color *screen)
{
	sky.draw(screen, cam.angle);
	ground::draw((color2*)(screen+BPP(SKY_HEIGHT*SCREEN_WIDTH)), cam);
	cam.draw(screen, objects);

	speed.draw(screen, point<>(0, 1));
	position.draw(screen, point<>(0, 60));
	lap.draw(screen, point<>(0, 30));
	minimap.draw(screen, point<>(MINIMAP_LEFT, MINIMAP_TOP));
	// рисуем позиции на карте
	for (unit_iter i=units.begin(); i!=units.end(); i++)
		(*i)->draw_info(screen, active_font, (*i)==player);

	if (g_prefs.show_fps)
		draw_fps(screen);


	static	point<>	msg(52, 80);
	switch (state)
	{
	case racing:
		break;
		
	case wait_3:
		big_three.draw(screen, msg);
		break;

	case wait_2:
		big_two.draw(screen, msg);
		break;
		
	case wait_1:
		big_one.draw(screen, msg);
		break;

	case wait_go:
		big_go.draw(screen, point<>(32, 80));
		break;

	case finish:
		big_finish.draw(screen, point<>(28, 80));
		break;
	}
}


//=================================================================
// изменение управляемости у всех машин (в разы)
void world::change_handing(float dh)
{
	for (car_iter i=cars.begin(); i!=cars.end(); i++)
		(*i)->handing=dh*(float)(*i)->handing;
}





//=====================================================================
//	Небо+анимация на нем
//=====================================================================
world::sky_image::sky_image(UInt16 id, database *db)
	:res_image	(id, db)
	,anims		(NULL)
	,n_anims	(ResLoadConstant(id+1))
{
	anims=new animation*[n_anims];
	for (int i=0; i<n_anims; i++)
	{
		anims[i]=new animation(id+1100, db, this);
		anims[i]->set_frame(0);
	}
}


//=====================================================================
world::sky_image::~sky_image()
{
	for (int i=0; i<n_anims; i++)
		delete anims[i];
	delete[] anims;
//	db->attach(10000, 'Tbmp', );
}


//=====================================================================
void world::sky_image::animate(const real &time)
{
	for (int i=0; i<n_anims; i++)
		anims[i]->anim(time);
}


//=====================================================================
void world::sky_image::draw(color *screen, Angle angle)
{
	// миним. шаг картинки - 4 пикс.
	UInt32	*s=(UInt32*)screen;
	UInt32	*b=(UInt32*)bits;
	Int32	y, x,
			o=(-angle>>6)&(SKY_WIDTH-1);

	((UInt16*)b)+=BPP(SKY_WIDTH-o)/2;
	for (y=SKY_HEIGHT; y--; b+=BPP(SKY_WIDTH-SCREEN_WIDTH)/4)
		if (o<SCREEN_WIDTH)	// два куска
		{
			for (x=BPP(o)/4; x--;)
				(*s++)=*(b++);
			b-=BPP(SKY_WIDTH)/4;
			for (x=BPP(SCREEN_WIDTH)/4-BPP(o)/4; x--;)
				(*s++)=*(b++);
			b+=BPP(SKY_WIDTH)/4;
		}
		else	// один
			for (x=BPP(SCREEN_WIDTH)/4; x--;)
				(*s++)=*(b++);
/**/
/*
	// миним. шаг картинки - 4 пикс.
	UInt32	*s=(UInt32*)screen;
	UInt32	*b=(UInt32*)bits;
	Int32	y, x,
			o=(-angle>>6)&(SKY_WIDTH-1);

	((UInt16*)b)+=(SKY_WIDTH-o)/4;
	for (y=SKY_HEIGHT; y--; b+=(SKY_WIDTH-SCREEN_WIDTH)/8)
		if (o<SCREEN_WIDTH)	// два куска
		{
			for (x=o/8; x--;)
				(*s++)=*(b++);
			b-=SKY_WIDTH/8;
			for (x=SCREEN_WIDTH/8-o/8; x--;)
				(*s++)=*(b++);
			b+=SKY_WIDTH/8;
		}
		else	// один
			for (x=SCREEN_WIDTH/8; x--;)
				(*s++)=*(b++);
/**/
}


