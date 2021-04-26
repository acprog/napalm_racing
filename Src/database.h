/* naPalm Racing

  Copyright (C) 2004

  Author: Alexander Semenov <acmain@gmail.com>
*/
#ifndef	_DATABASE_H_
#define	_DATABASE_H_

//============================================================
//	»нтерфейс к базам данных
//============================================================
class database
{
private:
	DmOpenRef 	db;
	bool		temporarily;	// временна€ база

public:
	// базы открываютс€(пока) в read-only
	// дл€ открыти€ баз лучше будет сделать 2-а разных конструктора
	database(UInt32 type, UInt32 creator);
	~database();	// закрыть базу. ≈сли она была временна€, удалить
	
	MemHandle detach(DmResType resType, DmResID resID, MemHandle handle);
	void attach(UInt16 id, DmResType resType, MemHandle h);
};

#endif	//	_DATABASE_H_