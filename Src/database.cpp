/* naPalm Racing

  Copyright (C) 2004

  Author: Alexander Semenov <acmain@gmail.com>
*/
#include "database.h"
#include "flat3d.h"

//============================================================
//	Интерфейс к базам данных
//============================================================
database::database(UInt32 type, UInt32 creator)
	:temporarily	(false)
{
	db=DmOpenDatabaseByTypeCreator(type, creator, dmModeReadWrite);
	if (!db)
		ERROR("Can't find program database");
}


//============================================================
// закрыть базу. Если она была временная, удалить
database::~database()
{	
	DmCloseDatabase(db);
}


//============================================================
MemHandle database::detach(DmResType resType, DmResID resID, MemHandle handle)
{
	MemHandle	h;

	switch (DmDetachResource(db, DmFindResource(db, resType, resID, handle), &h))
	{
	case dmErrReadOnly:
		ERROR("ReadOnly");
	case dmErrIndexOutOfRange:
		ERROR("IndexOutOfRange");
	case dmErrCorruptDatabase:
		ERROR("CorruptDatabase");
	case memErrChunkLocked:
		ERROR("ChunkLocked");
	case memErrInvalidParam:
		ERROR("InvalidParam");
	}
	return h;
}


//============================================================
void database::attach(UInt16 id, DmResType resType, MemHandle h)
{
	DmAttachResource(db, h, resType, id);
}
