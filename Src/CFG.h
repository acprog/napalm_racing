/* naPalm Racing

  Copyright (C) 2004

  Author: Alexander Semenov <acmain@gmail.com>
*/
#ifndef _CFG_H_
#define _CFG_H_

#include "GraphicsRsrc.h"

// частота процессора
enum	cpu_type
{
	mhz_16,
	mhz_33
};

extern cpu_type	current_cpu;


// Область граффити
#define	GRAFFITI_LEFT		27
#define	GRAFFITI_TOP		164
#define	GRAFFITI_WIDTH		106
#define	GRAFFITI_HEIGHT	56
#define	GRAFFITI_RIGHT		(GRAFFITI_LEFT+GRAFFITI_WIDTH)

//	кнопки управления
#define	RUDDER_LEFT		keyBitHard1
#define	RUDDER_RIGHT	keyBitHard2
#define	GEAR_UP			keyBitHard3
#define	GEAR_DOWN		keyBitHard4
#define	RESET_KEY		keyBitPageUp
#define	LOOK_BACK		keyBitPageDown
#define	KEYS_MASK		(RUDDER_LEFT|RUDDER_RIGHT|GEAR_UP|GEAR_DOWN)

// параметры экрана
#ifdef COLOR_GRAY
	typedef	unsigned char	color;	// тип цвета
	typedef	unsigned char	color2;	// тип сдвоенный цвет
	#define	BPP(x)	((x)>>1)	// 4бита
	#define	COLOR_SIZE		4
#endif

#ifdef COLOR_8
	typedef	unsigned char	color;	// тип цвета
	typedef	unsigned short	color2;	// тип сдвоенный цвет
	#define	BPP(x)	(x)	// 8 бит
	#define	COLOR_SIZE		8
#endif

#ifdef COLOR_16
	typedef	unsigned short	color;	// тип цвета
	typedef	unsigned long	color2;	// тип сдвоенный цвет
	#define	BPP(x)	(x)	// 16 бит
	#define	COLOR_SIZE		16
#endif

#define	SCREEN_OFFSET(x, y)	BPP( (x)+(y)*SCREEN_WIDTH )
#define 	SCREEN_WIDTH	160
#define 	SCREEN_HEIGHT	160

// параметры мира
#define	MAP_SHIFT		7	// (128)
#define	MAP_SIZE			(1<<MAP_SHIFT)
#define	MAX_OBJECTS		256

// размер мини-карты (бит)
#define	MINIMAP_SHIFT	(6-g_prefs.minimap.small)	//	(64)
#define	MINIMAP_SIZE	(1<<MINIMAP_SHIFT)
#define	MINIMAP_LEFT	(SCREEN_WIDTH-MINIMAP_SIZE-4)
#define	MINIMAP_TOP		4

// небо
#define	SKY_WIDTH	512
#define	SKY_HEIGHT	110

// параметры самеры
#define	SKIP_UP		5	
#define	DELTA			1.0f		// note : DELTA * NumLines == H
#define	C				0.01f
#define	D0				100.0f
#define	CAMERA_H		38

// размер граней текстуры (бит)
#define	TEXTURE_SHIFT	5	// (32)
#define	TEXTURE_SIZE	(1<<TEXTURE_SHIFT)
// группа текстур 4x4
#define	GROUP_TEXTURES_SHIFT	(TEXTURE_SHIFT+2)
#define	GROUP_TEXTURES_SIZE	(1<<GROUP_TEXTURES_SHIFT)

// анимация поверхности
#define	N_FRAMES	5
#define	ANIM_GROUND_WAIT	0.2f	// сек

// параметры машины
#define	MAX_SPEED		1500000L
#define	MAX_ACCELERATE	1500
#define	MAX_BRAKING		3000
#define	MAX_HANDING		(g_prefs.handing*10)//120//(current_cpu==mhz_33 ? 170 : 120)	//120
#define	MAX_COORD		((1L<<(TEXTURE_SHIFT+MAP_SHIFT))-1L)
#define	RESET_Z			20
#define	RESET_SPEED		30
#define	BRACKING_SPEED	6000L	// параметр AI - скорость по X
											// выше которой нужно тормозить
											// при реском повороте
// коэффициенты трения
#define	FRICTION_BACK	25	//20
#define	FRICTION_CROSS	350	//300

// коэффициенты удара
#define	PUSH_X		5
#define	PUSH_Y		5
#define	PUSH_ANGLE	11
/*
#define	PUSH_X		7
#define	PUSH_Y		7
#define	PUSH_ANGLE	15
/**/

// параметры трассы
#define	HARD_OF_BORDER	3
#define	CHECK_POINT_SIZE	(5<<TEXTURE_SHIFT)
#define	MAX_SKIP_WAYPOINTS	3	// макс. количество точек пути которое можно пропустить
#define	CARS_STEP	(2<<TEXTURE_SHIFT)	// шаг расстаноки машин на трассе
#define	MAX_WP_SHIFT	7
#define	MAX_WAYPOINTS	(1l<<MAX_WP_SHIFT)
#define	MIN_LAPS	2

// коэффициенты преобразования величин
#define	TO_KMH_SPEED	0.36f
#define	TO_MH_SPEED		0.224f


// параметры моделей
#define	MAX_VERTEXES	127
#define	MAX_POLYGONS	127

#ifdef COLOR_GRAY
#define	REPLACE_TO_COLOR	0x55
#define	MAX_ADD_COLOR	(0xf-(REPLACE_TO_COLOR&0xf))
#endif

#ifdef COLOR_8
#define	REPLACE_TO_COLOR	0x5
#define	MAX_ADD_COLOR	(0xf-REPLACE_TO_COLOR)
#endif

#ifdef COLOR_16
#define	REPLACE_TO_COLOR	0x5
#define	MAX_ADD_COLOR	(0xf-REPLACE_TO_COLOR)
#endif

// точность матриц
#define	FRAC_SHIFT		14

// область отображения
#define	VISIBLE_DISTANCE	200000L
#define	VISIBLE_LOW			90000L
#define	VISIBLE_MED			35000L
#define	VISIBLE_HIGHT		6000L
#define	VISIBLE_ANGLE		7000


// период обновление позиций (через каждые .. кадров)
#define	REFRESH_POSITIONS	10

// навык компьютера
#define	COMP_NOVICE		0
#define	COMP_AMATEUR	1
#define	COMP_MASTER		2	

#define	MAX_PLAYERS	9

#endif	// _CFG_H_