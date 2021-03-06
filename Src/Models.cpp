/* naPalm Racing

  Copyright (C) 2004

  Author: Alexander Semenov <acmain@gmail.com>
*/
//================================================================
//	???????? ???????
//================================================================
#include "model.h"


#undef MODEL
#define MODEL(name)	namespace data { namespace name {
#define VERTEXES		Vertex	vertexes[]=
#define POLYGONS		Polygon	polygons[]=
#define RENDERING		Int8		rendering[]=

#define END_MODEL(name)\
	}}\
	model	name(sizeof(data::name::vertexes)/sizeof(Vertex),\
		data::name::vertexes,\
		data::name::polygons,\
		data::name::rendering);

#define	END	-1	// ??? ??????


//============================================================
//	????? ??? ??????
//============================================================
MODEL	( car_low )
	VERTEXES
	{
		Vertex(-17,	10,	10	),	//	0
		Vertex(-17,	-10,	10	),	//	1
		Vertex(1,	0,	10	),	//	2
		Vertex(9,	12,	4	),	//	3
		Vertex(9,	-12,	4	),	//	4
		Vertex(-23,	12,	12	),	//	5
		Vertex(23,	12,	0	),	//	6
		Vertex(-23,	-12,	12	),	//	7
		Vertex(23,	-12,	0	),	//	8
		Vertex(-21,	-12,	0	),	//	9
		Vertex(-21,	12,	0	) 	//	10
	};
	POLYGONS
	{
		{0,		{	0,	1,	2,	-1	}},	//	0	?????
		{15,	{	2,	0,	3,	-1	}},	//	1	????? ????
		{13,	{	2,	3,	4,	-1	}},	//	2	??????? ??????
		{11,	{	1,	2,	4,	-1	}},	//	3	?????? ????
		{1,		{	5,	7,	6,	8	}},	//	4	?????
		{3,		{	7,	9,	8,	-1	}},	//	5	?????? ????
		{3,		{	5,	7,	9,	10	}},	//	6	???
		{3,		{	5,	6,	10,	-1	}} 	//	7	????? ????
	};
	RENDERING
	{
		4,	7,	3,	2,	1,	0,	-1,	//	0..45
		4,	7,	3,	2,	1,	0,	-1,	//	45..90
		6,	4,	1,	0,	7,	-1,	//	90..135
		7,	4,	1,	0,	6,	-1,	//	135..180
		5,	4,	3,	0,	6,	-1,	//	180..225
		6,	4,	3,	2,	5,	-1,	//	225..270
		4,	5,	1,	2,	3,	0,	-1,	//	270..315
		4,	5,	1,	2,	3,	0,	-1,	//	315..360
		6,	-1,
	};
END_MODEL ( car_low )


//============================================================
MODEL	( car_med )
	VERTEXES
	{
		Vertex(-16,	10,	10	),	//	0
		Vertex(10,	12,	6	),	//	1
		Vertex(2,	10,	10	),	//	2
		Vertex(20,	-12,	0	),	//	3
		Vertex(-24,	-12,	10	),	//	4
		Vertex(-20,	-12,	0	),	//	5
		Vertex(14,	-14,	6	),	//	6
		Vertex(-16,	-10,	10	),	//	7
		Vertex(2,	-10,	10	),	//	8
		Vertex(10,	-12,	6	),	//	9
		Vertex(-10,	12,	0	),	//	10
		Vertex(-8,	12,	4	),	//	11
		Vertex(-16,	12,	0	),	//	12
		Vertex(-18,	12,	6	),	//	13
		Vertex(14,	12,	0	),	//	14
		Vertex(16,	12,	2	),	//	15
		Vertex(8,	12,	0	),	//	16
		Vertex(6,	12,	4	),	//	17
		Vertex(8,	-12,	0	),	//	18
		Vertex(6,	-12,	4	),	//	19
		Vertex(14,	-12,	0	),	//	20
		Vertex(16,	-12,	2	),	//	21
		Vertex(-16,	-12,	0	),	//	22
		Vertex(-18,	-12,	6	),	//	23
		Vertex(-10,	-12,	0	),	//	24
		Vertex(-8,	-12,	4	),	//	25
		Vertex(-20,	12,	0	),	//	26
		Vertex(-24,	12,	10	),	//	27
		Vertex(14,	14,	6	),	//	28
		Vertex(20,	12,	0	) 	//	29
	};
	POLYGONS
	{
		{15,	{	0,	2,	1,	-1	}},	//	0	????? ????
		{4,		{	4,	3,	5,	-1	}},	//	1	?????? ????
		{2,		{	4,	6,	3,	-1	}},	//	2	?????? ????
		{11,	{	7,	8,	9,	-1	}},	//	3	?????? ????
		{0,		{	2,	8,	0,	7	}},	//	4	?????
		{13,	{	2,	8,	1,	9	}},	//	5	??????? ??????
		{15,	{	13,	11,	12,	10	}},	//	6	?????? ????? ??????
		{15,	{	17,	15,	16,	14	}},	//	7	???????? ????? ??????
		{15,	{	19,	21,	20,	18	}},	//	8	???????? ?????? ??????
		{15,	{	23,	25,	24,	22	}},	//	9	?????? ?????? ??????
		{3,		{	27,	4,	5,	26	}},	//	10	???
		{1,		{	4,	27,	28,	6	}},	//	11	?????
		{2,		{	6,	28,	29,	3	}},	//	12	?????
		{2,		{	27,	28,	29,	-1	}},	//	13	????? ????
		{4,		{	27,	29,	26,	-1	}}, //	14	????? ????
		{0,		{	8,	9,	-1,	-1	}}, //	15	?????? ??????
		{0,		{	2,	1,	-1,	-1	}} 	//	16	????? ??????
	};
	RENDERING
	{
		13,	14,	7,	6,	11,	12,	0,	5,	15,	16,	4,	-1,	//	0..45
		11,	5,	12,	14,	0,	16,	13,	6,	7,	4,	-1,	//	45..90
		10,	12,	11,	13,	0,	14,	5,	7,	6,	4,	-1,	//	90..135
		13,	14,	6,	7,	11,	0,	10,	4,	-1,	//	135..180
		2,	1,	8,	9,	11,	3,	10,	4,	-1,	//	180..225
		10,	12,	11,	1,	2,	8,	9,	3,	5,	4,	-1,	//	225..270
		11,	5,	12,	1,	3,	15,	2,	8,	9,	4,	-1,	//	270..315
		1,	2,	8,	9,	11,	12,	3,	5,	16,	15,	4,	-1, 	//	315..360
		11,	10,	4,	-1,	// 180
	};
END_MODEL ( car_med )


//=================================================================
MODEL	( car_high )
	VERTEXES
	{
		Vertex(15,	-12,	0	),	//	0
		Vertex(16,	-13,	3	),	//	1
		Vertex(13,	-13,	0	),	//	2
		Vertex(7,	-13,	4	),	//	3
		Vertex(9,	-12,	0	),	//	4
		Vertex(-7,	-13,	4	),	//	5
		Vertex(-17,	-14,	6	),	//	6
		Vertex(-15,	-14,	0	),	//	7
		Vertex(-15,	-12,	0	),	//	8
		Vertex(-9,	-12,	0	),	//	9
		Vertex(-19,	-14,	7	),	//	10
		Vertex(-19,	-12,	0	),	//	11
		Vertex(21,	-12,	0	),	//	12
		Vertex(21,	-12,	2	),	//	13
		Vertex(13,	-14,	6	),	//	14
		Vertex(-19,	-11,	10	),	//	15
		Vertex(-22,	-10,	7	),	//	16
		Vertex(-15,	14,	0	),	//	17
		Vertex(-17,	14,	6	),	//	18
		Vertex(-15,	12,	0	),	//	19
		Vertex(-7,	13,	4	),	//	20
		Vertex(-9,	12,	0	),	//	21
		Vertex(13,	13,	0	),	//	22
		Vertex(7,	13,	4	),	//	23
		Vertex(9,	12,	0	),	//	24
		Vertex(16,	13,	3	),	//	25
		Vertex(15,	12,	0	),	//	26
		Vertex(21,	12,	0	),	//	27
		Vertex(-19,	14,	7	),	//	28
		Vertex(-19,	12,	0	),	//	29
		Vertex(21,	12,	2	),	//	30
		Vertex(13,	14,	6	),	//	31
		Vertex(-19,	11,	10	),	//	32
		Vertex(-22,	10,	7	),	//	33
		Vertex(-15,	10,	10	),	//	34
		Vertex(11,	12,	6	),	//	35
		Vertex(3,	10,	10	),	//	36
		Vertex(13,	0,	6	),	//	37
		Vertex(3,	-10,	10	),	//	38
		Vertex(11,	-12,	6	),	//	39
		Vertex(-15,	-10,	10	) 	//	40
	};
	POLYGONS
	{
		{15,	{	1,	0,	2,	-1	}},	//	0	?????? ???????? ??????
		{15,	{	3,	1,	2,	-1	}},	//	1	--	?????
		{15,	{	3,	2,	4,	-1	}},	//	2	--	???
		{15,	{	6,	8,	7,	-1	}},	//	3	?????? ?????? ??????
		{15,	{	6,	5,	7,	-1	}},	//	4	--	?????
		{15,	{	5,	7,	9,	-1	}},	//	5	--	?????
		{5,		{	10,	11,	12,	-1	}},	//	6	?????? ????
		{4,		{	10,	13,	12,	-1	}},	//	7	--
		{3,		{	10,	14,	13,	-1	}},	//	8	--
		{2,		{	15,	10,	14,	-1	}},	//	9	--	????
		{4,		{	10,	16,	11,	-1	}},	//	10	?????? ???
		{8,		{	15,	16,	10,	-1	}},	//	11	?????? ????
		{15,	{	18,	17,	19,	-1	}},	//	12	????? ?????? ??????
		{15,	{	18,	20,	17,	-1	}},	//	13	--	?????
		{15,	{	20,	21,	17,	-1	}},	//	14	--	?????
		{15,	{	23,	22,	24,	-1	}},	//	15	????? ???????? ??????
		{15,	{	23,	25,	22,	-1	}},	//	16	--	?????
		{15,	{	25,	26,	22,	-1	}},	//	17	--	?????
		{5,		{	28,	27,	29,	-1	}},	//	18	????? ????
		{4,		{	28,	30,	27,	-1	}},	//	19	-- 
		{3,		{	28,	31,	30,	-1	}},	//	20	--
		{2,		{	32,	28,	31,	-1	}},	//	21	-- 	????
		{4,		{	28,	33,	29,	-1	}},	//	22	????? ???
		{8,		{	32,	28,	33,	-1	}},	//	23	????? ????
		{15,	{	34,	36,	35,	-1	}},	//	24	????? ????
		{13,	{	38,	36,	37,	-1	}},	//	25	??????? ??????
		{14,	{	36,	37,	35,	-1	}},	//	26	--	?????
		{12,	{	38,	39,	37,	-1	}},	//	27	--	??????
		{11,	{	40,	38,	39,	-1	}},	//	28	?????? ????
		{0,		{	34,	36,	40,	38	}},	//	29	?????
		{3,		{	33,	16,	11,	29	}},	//	30	???
		{0,		{	32,	15,	16,	33	}},	//	31	?????
		{1,		{	15,	32,	31,	14	}},	//	32	?????
		{2,		{	14,	31,	30,	13	}},	//	33	?????
		{4,		{	30,	13,	12,	27	}},	//	34	?????
		{0,		{	36,	35,	-1,	-1	}},	//	35	????? ??????
		{0,		{	38,	39,	-1,	-1	}}	//	36	?????? ??????
	};
	RENDERING
	{
		18,	19,	20,		16,	17,	13,	14,		21,	32,	33,	34,		24,	25, 26, 27,	35,	36,	29,	-1,	//	0..45
		34,		22,	23,	18,	19,	20,	21,		15,	16,	17,	13,	14,		32,	33,		24,	25, 26, 27,	35, 29,	-1,	//	45..90
		30,	31,	22,	23,		18,	19,	20,	21,		14,	15,	16,	17,	12,	13,		33,	32,		25, 26, 27, 24,	35,	29,	-1,	//	90..135
		18,	19,	20,		15,	16,	12,	13,		21,	9,	10,	11,	32,	22,	23,	30,	31,		24,	29,	-1,	//	135..180
		6,	7,	8,	9,		1,	2,	3,	4,		21,	22,	23,	30,	31,	32,	10,	11,		28,	29,	-1,	//	180..225
		30,	31,	10,	11,		6,	7,	8,	9,		0,	1,	2,	3,	4,	5,		33,	32,		28,	25, 26, 27, 36,	29,	-1,	//	225..270
		10,	11,		6,	7,	8,	9,		0,	1,	2,	4,	5,		32,	34,	33,		28,	25, 26, 27, 36,	29,	-1,	//	270..315
		6,	7,	8,		0,	1,	4,	5,		9,	32,	34,	33,		28,	25, 26, 27, 36,	35,	29,	-1,	//	315..360
		9,		21,	22,	23,	30,	31,	32,	10,	11,		29,	-1,	//	?????????? ??? ???? ?????
	};
END_MODEL ( car_high )




//============================================================
//	????? ??? ????
//============================================================
// ???? ??????
solid::atom	car_atoms[]=
{
	//	d,	r
	{	0,	15},
	{	-8,	15},
	{	7,	15}
};

solid	car_solid=
{
	car_atoms,
	3,	//	n
	23	//	r
};

/*
solid::atom	car_atoms[]=
{
	//	d,	r
	{	-8,	15},
	{	7,	15}
};

solid	car_solid=
{
	car_atoms,
	2,	//	n
	23	//	r
};
/**/