/* naPalm Racing

  Copyright (C) 2004

  Author: Alexander Semenov <acmain@gmail.com>
*/
#ifndef	_DATABASE_H_
#define	_DATABASE_H_

//============================================================
//	��������� � ����� ������
//============================================================
class database
{
private:
	DmOpenRef 	db;
	bool		temporarily;	// ��������� ����

public:
	// ���� �����������(����) � read-only
	// ��� �������� ��� ����� ����� ������� 2-� ������ ������������
	database(UInt32 type, UInt32 creator);
	~database();	// ������� ����. ���� ��� ���� ���������, �������
	
	MemHandle detach(DmResType resType, DmResID resID, MemHandle handle);
	void attach(UInt16 id, DmResType resType, MemHandle h);
};

#endif	//	_DATABASE_H_