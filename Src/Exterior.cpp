/* naPalm Racing

  Copyright (C) 2004

  Author: Alexander Semenov <acmain@gmail.com>
*/
#include "exterior.h"

projected_point exterior::immovable[MAX_VERTEXES];
Int8	exterior::immov_sector;
model	*exterior::immov_model;

//=========================================================
//	Внешние параметры объекта
//=========================================================
exterior::exterior(model *_high, model *_medium, model *_low, solid *_Solid)
	:high	(_high)
	,medium	(_medium)
	,low	(_low)
	,Solid	(_Solid)
{
}
