#include "sound.h"

bool	sound::audio_driver=false;
int		sound::n_samples=0;
UInt16	sound::port=0;


//==================================================================
//	Звук
//==================================================================
sound::sound(UInt16 id)
{
	if (n_samples++==0)
	{
	//	audio_driver=!SrmOpen('auDi', 1280L, &port);
		if (audio_driver)
		{
			// Set Default
			UInt32	value32=sndMaxAmp;
			UInt16	vlen=sizeof(UInt32);
			SrmControl(port, AUdrvOpCodeVolume, &value32, &vlen);
			value32=8000L;
			vlen=sizeof(UInt32);
			SrmControl(port, AUdrvOpCodeSampling, &value32, &vlen);
		}
	}
	
	if (audio_driver)
	{	// загрузка данных
		//wave=DmGetResource('tWAV', id);
	}
}


//===============================================================
sound::~sound()
{
	if (audio_driver)
	{
		// выгрузка данных
		
		if (--n_samples==0)
		{
			// STOP Playing
			UInt32	value32 = 0L;
			UInt16	vlen = sizeof(UInt32);
			SrmControl(port, AUdrvOpCodeStop, &value32, &vlen);
	
			// Close Audio Driver
			SrmClose(port);
		}
	}
}


//===============================================================
void sound::play()
{
	if (audio_driver)
	{
		UInt32	size = MemHandleSize(wave);
		MemPtr	wp = MemHandleLock(wave);
		Err		err;
		UInt32	sent = SrmSend(port, wp, size, &err);

		MemHandleUnlock(wave);

		UInt32	value32=2400L;
		UInt16	vlen=sizeof(UInt32);
		SrmControl(port, AUdrvOpCodePlay, &value32, &vlen); // Start Playing
	}
}