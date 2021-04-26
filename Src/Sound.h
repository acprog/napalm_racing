#include "aDrvPublic.h"
#include "PalmOS.h"

//==================================================================
//	Звук
//==================================================================
class sound
{
private:
	static	bool	audio_driver;
	static	int		n_samples;
	static	UInt16	port;

	MemHandle 	wave;

public:
	sound(UInt16 id);
	~sound();
	void play();
};