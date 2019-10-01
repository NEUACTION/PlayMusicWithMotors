#include "music.h"
#include "svpwm.h"

MusicType music;


//八分音符（低八度、标准八度、高八度）
static float freq[] = {0,130.81,146.83,164.81,174.61,196,220,246.9,\
                       261.63,293.66,329.63,349.23,392.00,440,493.88,\
                       523.25,587.33,659.26,698.46,783.99,880,987.77};
void PlayChord(void)
{
	static int chordCnt = 0;
	PlayMusicWithCurrent(freq[music.actualNote],freq[music.actualNote2],0);
}


void PlayMusicWithCurrent(float f1,float f2, float f3)
{
	 float T1 = 1 / f1, T2 = 1/f2, T3 = 1 / f3;
	 float cntGoal1 = T1 * 1e6 / DEFAULT_PWM_PERIOD_US,cntGoal2 = T2 * 1e6 / DEFAULT_PWM_PERIOD_US, cntGoal3 = T3 * 1e6 / DEFAULT_PWM_PERIOD_US; 
	 float aVol = 0.f, bVol = 0.f,cVol = 0.f; 
	 static int cnt1 = 0, cnt2 = 0, cnt3 = 0;
	 static int posCnt = 0,pos = 0;
	 static float vol = 0.5f;
	 cnt1 ++;
	 cnt2 ++;
	 cnt3 ++;
		posCnt++;
	{
		
		 if(cnt1 >= cntGoal1)
		 {
				aVol = 4.5;
			 	cnt1 = 0;
		 }
		 else
		 {
				aVol = 0;
		 }
		 
		 if(cnt2 >= cntGoal2)
		 {
				bVol = 2.5;
			 	cnt2 = 0;
		 }
		 else
		 {
				bVol = 0;
		 }
		 
		 if(cnt3 >= cntGoal3)
		 {
				cVol = 4.5;
			 	cnt3 = 0;
		 }
		 else
		 {
				cVol = 0;
		 }
	 }
	
	 if(posCnt >= 5)
	 {
			pos += 1.0f;
			posCnt = 0;
	 }

	SvpwmCommutation(aVol + bVol + cVol,vol,pos);
}

