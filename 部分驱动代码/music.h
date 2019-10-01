#ifndef __PLAY_MUSIC_H
#define __PLAY_MUSIC_H

typedef enum{
	
	EMPTY = 0,
	LDO,
	LRI,
	LMI,
	LFA,
	LSO,
	LLA,
	LSI,
	DO,
	RI,
	MI,
	FA,
	SO,
	LA,
	SI,
	HDO,
	HRI,
	HMI,
	HFA,
	HSO,
	HLA,
	HSI,
}MuseNote;

typedef struct
{
	int actualNote;
	int actualNote2;
	
}MusicType;

extern MusicType music;
void PlayMusic(float f,int* cnt);
void PlayMusicAllRank(void);
void PlayChord(void);
void PlayMusicWithCurrent(float f1, float f2,float f3);
#endif


