#include <PalmOS.h>

#include "Flat3D.h"
#include "Flat3DRsc.h"

#include "image.h"
#include "ground.h"

//=================================================================
db_info		*tracks_names[1024];
int			n_tracks=0;
static	ListType	*tracks=NULL;


//=================================================================
db_info *select_track(int &tr, UInt16 label)
{
	if (tr>=n_tracks)
		tr=0;
	
	db_info	*base=tracks_names[tr];

	FrmCopyLabel(FrmGetActiveForm(), label, base->name);

	DmOpenRef db=DmOpenDatabase(base->card, DmFindDatabase(base->card, base->name), dmModeReadOnly);

	MemHandle	h;
	UInt16		index=DmNumRecords(db);
	
	// поверхность
	--index;

	// дорога
	h=DmGetRecord(db, --index);

	packed_1bit	*buffer=new packed_1bit[MAP_SIZE*MAP_SIZE/8];

	image		*minimap_img=NULL;
	BitmapType	*minimap_bmp=NULL;
	if (!g_prefs.safe_mode)
	{
		minimap_img=new image(size<>(64, 64));
		minimap_img->draw_type=image::Put;
	}
	else
	{
		Err	error;
		minimap_bmp=BmpCreate(64, 64, COLOR_SIZE, NULL, &error);
	}

	packed_1bit	*src=buffer;
	MemMove(buffer, MemHandleLock(h), MAP_SIZE*MAP_SIZE/8);
	MemHandleUnlock(h);

	int	x, y;
#ifdef COLOR_GRAY
	#define COLOR(n)	(1<<(src[0]._##n##0+src[0]._##n##1+src[MAP_SIZE/16]._##n##0+src[MAP_SIZE/16]._##n##1))-1;

	packed_4bit	*dst=(packed_4bit*)(g_prefs.safe_mode
		?	BmpGetBits(minimap_bmp)
		:	(color*)*minimap_img);

	for (int y=0; y<MAP_SIZE/2; y++, src+=MAP_SIZE/16)
		for (int x=0; x<MAP_SIZE/16; src++, dst+=2, x++)
		{
			dst[0]._00=COLOR(0);
			dst[0]._01=COLOR(1);
			dst[0]._10=COLOR(2);
			dst[0]._11=COLOR(3);
			dst[1]._00=COLOR(4);
			dst[1]._01=COLOR(5);
			dst[1]._10=COLOR(6);
			dst[1]._11=COLOR(7);
		}
#endif

#ifdef COLOR_8
	color	colors[]={0x00, 0x25, 0x38, 0x5d, 0x64};
//	#define COLOR(n)	(0xe0-2*(src[0]._##n##0+src[0]._##n##1+src[MAP_SIZE/16]._##n##0+src[MAP_SIZE/16]._##n##1));
	#define COLOR(n)	colors[src[0]._##n##0+src[0]._##n##1+src[MAP_SIZE/16]._##n##0+src[MAP_SIZE/16]._##n##1];
	color	*dst=(color*)(g_prefs.safe_mode
		?	BmpGetBits(minimap_bmp)
		:	*minimap_img);

	for (int y=0; y<MAP_SIZE/2; y++, src+=MAP_SIZE/16)
		for (int x=0; x<MAP_SIZE/16; src++, dst+=8, x++)
		{
			dst[0]=COLOR(0);
			dst[1]=COLOR(1);
			dst[2]=COLOR(2);
			dst[3]=COLOR(3);
			dst[4]=COLOR(4);
			dst[5]=COLOR(5);
			dst[6]=COLOR(6);
			dst[7]=COLOR(7);
		}

#endif

	#undef COLOR

	DmReleaseRecord(db, index, true);
	DmCloseDatabase(db);
	
	delete[] buffer;

	if (!g_prefs.safe_mode)
	{
		color	*screen=(color*)BmpGetBits(WinGetBitmap(WinGetDisplayWindow()));
		minimap_img->draw(screen, point<>(96, 40));
		delete minimap_img;
	}
	else
	{
		WinDrawBitmap(minimap_bmp, 96, 40);
		BmpDelete(minimap_bmp);
	}
	
	return base;
}


//=================================================================
static	void insert_name(db_info *base)
{
	*(tracks_names[n_tracks++]=new db_info)=*base;
//	StrCopy(tracks_names[n_tracks++], name);
	tracks_names[n_tracks]=NULL;
}



//=================================================================
static void FormInit(FormType *frmP)
{
	tracks=GetObjectPtr<ListType>(TrackNamesList, frmP);
	LstSetListChoices(tracks, (char**)tracks_names, n_tracks);
	LstSetSelection(tracks, 0);
	
	//------------------------------------------
	char str[3];
	StrPrintF(str, "%d", g_prefs.single_race.n_laps);
	FieldType	*fldP=GetObjectPtr<FieldType>(TrackLapsField, frmP);
	FldDelete(fldP, 0, FldGetMaxChars(fldP));
	FldInsert(fldP, str, StrLen(str));
}


//=================================================================
// Заполнение списка трасс
void make_tracks_list()
{
	n_tracks=0;

//	UInt16	cardNo=0;
	LocalID	dbID;
	bool	newSearch=true;
//	char	name[256];
	db_info base;
	UInt16	version;
	DmSearchStateType stateInfo;

	while (!DmGetNextDatabaseByTypeCreator(newSearch, &stateInfo,
		TRACK_TYPE, TRACK_CREATOR, false, &base.card, &dbID))
	{
		DmDatabaseInfo(base.card, dbID, base.name,
					NULL, &version, NULL,
					NULL, NULL,
					NULL, NULL,
					NULL, NULL,
					NULL);
		if (base.name[0]!='*')
			insert_name(&base);
		newSearch=false;
	}
}



//=================================================================
static void FormDestroy(FormType * /*frmP*/)
{
	g_prefs.single_race.n_laps=StrAToI(FldGetTextPtr(GetObjectPtr<FieldType>(TrackLapsField)));
	if (g_prefs.single_race.n_laps<MIN_LAPS)
		g_prefs.single_race.n_laps=MIN_LAPS;
}


//=================================================================
// Уничтожение списка трасс
void destroy_tracks_list()
{
	while (n_tracks--)
		delete[] tracks_names[n_tracks];
}



//=================================================================
static bool select_control(UInt16 id)
{
	switch (id)
	{
	case TrackBackButton:
		g_prefs.single_race.curr_track=LstGetSelection(tracks);
		FrmGotoForm(SingleRaceForm);
		return true;
	}

	return false;
}



//=================================================================
Boolean TrackFormHandleEvent(EventType *eventP)
{
    Boolean handled = false;
	FormType *frmP=NULL;

    switch (eventP->eType) 
        {
		case frmOpenEvent:
			frmP = FrmGetActiveForm();
			FormInit(frmP);
			int tr=0;
			select_track(tr, TrackNameLabel);
			FrmDrawForm(frmP);
            handled = true;
            break;

		case frmCloseEvent:
			FormDestroy(frmP);
			break;
            
        case lstSelectEvent:
        	tr=eventP->data.lstSelect.selection;
        	select_track(tr, TrackNameLabel);
        	return true;

            
        case frmUpdateEvent:
            // To do any custom drawing here, first call 
            // FrmDrawForm(), then do your drawing, and 
            // then set handled to true.
            break;
        
        case ctlSelectEvent:
            return select_control(eventP->data.ctlSelect.controlID);
		}
    
    return handled;
}