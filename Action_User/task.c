#include "includes.h"
#include <app_cfg.h>
#include "misc.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "timer.h"
#include "gpio.h"
#include "usart.h"
#include "can.h"
#include "elmo.h"


/*
===============================================================
						信号量定义
===============================================================
*/
OS_EXT INT8U OSCPUUsage;
OS_EVENT *PeriodSem;

static OS_STK App_ConfigStk[Config_TASK_START_STK_SIZE];
static OS_STK PlayMusicTaskStk[PLAY_MUSIC_TASK_STK_SIZE ];

void App_Task()
{
	CPU_INT08U os_err;
	os_err = os_err; /*防止警告...*/

	/*创建信号量*/
	PeriodSem = OSSemCreate(0);

	/*创建任务*/
	os_err = OSTaskCreate((void (*)(void *))ConfigTask, /*初始化任务*/
						  (void *)0,
						  (OS_STK *)&App_ConfigStk[Config_TASK_START_STK_SIZE - 1],
						  (INT8U)Config_TASK_START_PRIO);

	os_err = OSTaskCreate((void (*)(void *))PlayMusicTask,
						  (void *)0,
						  (OS_STK *)&PlayMusicTaskStk[PLAY_MUSIC_TASK_STK_SIZE - 1],
						  (INT8U)PLAY_MUSIC_TASK_PRIO);
}

/*
   ===============================================================
   初始化任务
   ===============================================================
   */
void ConfigTask(void)
{
	CPU_INT08U os_err;
	os_err = os_err;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	USART2_Init(921600);
	USART3_Init(115200);//串口3和定位系统相连，接收定位系统的数据
	UART4_Init(921600);
	
	CAN_Config(CAN1, 500, GPIOB, GPIO_Pin_8, GPIO_Pin_9);
	
	TIM_Init(TIM2, 99, 839, 1, 0);
	
	delay_ms(2000);
	OSTaskSuspend(OS_PRIO_SELF);
}




int musicControlFlag = 0;
float soundDelay = 20,soundDelayCnt = 0;
int actualNote = EMPTY;

int Preview(int* cnt, int* n);//乐曲前奏
int APart(int* cnt, int* n);//乐曲A Part
int BPart(int* cnt, int* n);
int CPart(int* cnt, int* n);
int DPart(int* cnt, int* n);

int PlayWhole(int* cnt,int note);//一音符一拍
int PlayShort(int* cnt,int note);//短促音
int PlayLong(int* cnt,int note);//一音符两拍
int PlayChord(int note);//和音通道

void PlayMusicTask(void)
{

	CPU_INT08U os_err;
	os_err = os_err;
	OSSemSet(PeriodSem, 0, &os_err);
	int mode = 0;//乐曲行进记录
	int cntP = 0;//音符行进记录
	int	nP = 0;//节拍行进记录

	while (1)
	{	
		OSSemPend(PeriodSem, 0, &os_err);
			//乐曲行进
			switch(mode)
			{
				case 0:
					if(Preview(&cntP,&nP) == 1)//前奏
					{
						mode =1 ;
						nP = 0;
					}
					break;
				case 1:
					if(APart(&cntP,&nP) == 1)
					{
						mode =2;
						nP = 0;
					}
					break;
				case 2:
					if(BPart(&cntP,&nP) == 1)
					{
						mode =3;
						nP = 0;
					}
					break;
				case 3:
					if(CPart(&cntP,&nP) == 1)
					{
						mode =99;
						nP = 0;
					}					
					break;
				case 4:
										
					break;
				case 5:
				
					break;
				case 6:
						
					break;
			}
		}
	}

/**
* @brief  前奏
* @param  cnt  乐符进行  
	        n    小节进行
* @author ACTION
*/
int Preview(int* cnt, int* n)
{
	int noteNum = 30;
	int flag = 0;
	int note[]=  //手动输入前奏音符
	{
		MI,FA,SO,\
		LA,EMPTY,EMPTY,HDO,SI,LA,\
		SO,EMPTY,EMPTY,SO,HDO,HDO,\
		SO,FA,MI,FA,SO,LA,\
		HDO,SI,LA,SI,HDO,HMI,\
		HRI,HRI,HSO,HSO//30
	};
	
	if(*n == 26 || *n == 27 || *n == 28 || *n == 29)//判断需要使用长音的音符
	{
		flag = PlayLong(cnt,note[*n]);
	}
	else
	{
		flag = PlayWhole(cnt,note[*n]);	
	}
	if(flag == 1)//手动配置和音
	{
		(*n)++;
	}	
	if(*n < 9)
	{
		PlayChord(DO);
	}
	else if(*n < 13)
	{
		PlayChord(LSO);
	}
	else if(*n < 17)
	{
		PlayChord(FA);
	}
	else if(*n < 21)
	{
		PlayChord(LSO);
	}	
	else if(*n < 28)
	{
		PlayChord(LDO);
	}
	else
	{
		PlayChord(RI);
	}
	
	if((*n) >= noteNum)	
	{
		PlayChord(EMPTY);
		return 1;
	}
	return 0;
}

int APart(int* cnt, int* n)
{
	int noteNum = 40;
	int flag = 0;
	int note[]=
	{
		//0-5
		SO,LA,SO,FA,MI,RI,\
		//6-11
		DO,EMPTY,EMPTY,LSO,EMPTY,EMPTY,
		//12-17
		DO,MI,HDO,SI,LA,MI,\
		//18，19
		SO,SO,\
		//20-25
		LA,SI,LA,SO,FA,MI,\
		//26-31
		RI,EMPTY,EMPTY,LLA,EMPTY,EMPTY,\
		//32-37
		LSI,LLA,LSO,SO,DO,RI,\
		//38，39
		MI,MI,\
	};
	

  if(*n == 18 || *n == 19 || *n == 38 || *n == 39)
	{
		flag = PlayLong(cnt,note[*n]);	
	}
	else
	{
		flag = PlayWhole(cnt,note[*n]);	
	}
	
	if(flag == 1)
	{
		(*n)++;
	}	
	if(*n < 6)
	{
		PlayChord(DO);
	}
	else if(*n < 12)
	{
		PlayChord(LSO);
	}
	else if(*n < 18)
	{
		PlayChord(DO);
	}	
	else if(*n < 24)
	{
		PlayChord(MI);
	}		
	else if(*n < 30)
	{
		PlayChord(DO);
	}	
	else if(*n < 33)
	{
		PlayChord(DO);
	}	
	else
	{
		PlayChord(LDO);
	}
	if(*n >= noteNum)	
	{
		PlayChord(EMPTY);
		return 1;
	}
	return 0;
}

int BPart(int* cnt, int* n)
{
	int noteNum = 40;
	int flag = 0;	
	int note[]=
	{
	//0-5
		SO,LA,SO,FA,MI,RI,\
		//6-11
		DO,EMPTY,EMPTY,LSO,EMPTY,EMPTY,\
		//12-17
		DO,MI,HDO,SI,HRI,HDO,\
		//18,19
		LA,LA,\
		//20-25
		HDO,SI,LA,SO,EMPTY,EMPTY,\
		//26-31
		LA,SO,FA,MI,EMPTY,EMPTY,\
		//32-37
		LSI,LSI,LLA,LSO,LSO,RI,
		//38,39
		DO,DO\
		
	};
	 if(*n == 18 || *n == 19 || *n == 38 || *n == 39)
	{
		flag = PlayLong(cnt,note[*n]);	
	}
	else
	{
		flag = PlayWhole(cnt,note[*n]);	
	}
	
	if(flag == 1)
	{
		(*n)++;
	}	
		if(*n < 6)
	{
		PlayChord(DO);
	}
	else if(*n < 12)
	{
		PlayChord(LSO);
	}
	else if(*n < 18)
	{
		PlayChord(DO);
	}	
	else if(*n < 24)
	{
		PlayChord(MI);
	}		
	else if(*n < 30)
	{
		PlayChord(DO);
	}	
	else if(*n < 33)
	{
		PlayChord(DO);
	}	
	else
	{
		PlayChord(LDO);
	}
	if(*n >= noteNum)	
	{
		PlayChord(EMPTY);
		return 1;
	}
	return 0;	
}


int CPart(int* cnt, int* n)
{
	int noteNum = 41;
	int flag = 0;	
	int note[]=
	{
	//0-5
		HDO,HRI,HMI,HMI,HRI,LA,\
		//6-8
		SI,LA,MI,\
		//9,10
		SO,SO,
		//11-16
		HDO,HRI,HMI,HMI,HRI,LA,\
		//17-19
		SI,SO,MI,\
		//20
		LA,
		//21-26
		SO,FA,MI,RI,EMPTY,EMPTY,\
		//27-32
		LSI,LLA,LSO,MI,EMPTY,EMPTY,\
		//33-38
		FA,EMPTY,EMPTY,RI,EMPTY,DO,\
		//39,40
		DO,DO\
	};
	

  if(*n == 9 || *n == 10 ||  *n == 20 || *n == 39 || *n == 40)
	{
		flag = PlayLong(cnt,note[*n]);	
	}
	else
	{
		flag = PlayWhole(cnt,note[*n]);	
	}
	
	if(flag == 1)
	{
		(*n)++;
	}	
	
	if(*n < 3)
	{
		PlayChord(DO);
	}
	else if(*n < 6)
	{
		PlayChord(LSO);
	}
	else if(*n < 12)
	{
		PlayChord(DO);
	}	
	else if(*n < 18)
	{
		PlayChord(LDO);
	}		
	else if(*n < 21)
	{
		PlayChord(LSO);
	}	
	else if(*n < 27)
	{
		PlayChord(DO);
	}	
	else if(*n < 35)
	{
		PlayChord(LSO);
	}	
	else if(*n < 38)
	{
		PlayChord(EMPTY);
	}	
	else
	{
		PlayChord(DO);
	}
	
	if(*n >= noteNum)	
	{
		PlayChord(EMPTY);
		return 1;
	}
	return 0;	
}



/**
* @brief  音符演奏
* @param  cnt  乐符进行  
	        note    小节进行
* @author ACTION
*/
int PlayWhole(int* cnt,int note)
{
	static int slice = 30; //单个乐符行进速度为30个周期(300ms)
	PlayMusic(CAN1,9,note);
	PlayMusic(CAN1,8,note);
	(*cnt)++;
	if(*cnt >= slice)
	{
		PlayMusic(CAN1,9,EMPTY);
		*cnt = 0;
		return 1;
	}
	return 0;
}
/**
* @brief  促音符演奏
* @param  cnt  乐符进行  
	        note    小节进行
* @author ACTION
*/
int PlayShort(int* cnt,int note)
{
	static int slice = 10;
	(*cnt)++;

	if(*cnt <= slice / 2)
	{
		PlayMusic(CAN1,9,note);
		PlayMusic(CAN1,8,note);
	}
	else if(*cnt <= slice)
	{
		PlayMusic(CAN1,9,EMPTY);
		PlayMusic(CAN1,8,EMPTY);
		
	}	
	else
	{
		(*cnt) = 0;	
		return 1;
	}
	return 0;
}
/**
* @brief  延音符演奏
* @param  cnt  乐符进行  
	        note    小节进行
* @author ACTION
*/
int PlayLong(int* cnt,int note)
{
	static int slice = 90;
	PlayMusic(CAN1,9,note);
	PlayMusic(CAN1,8,note);
	(*cnt)++;
	if(*cnt >= slice)
	{
		PlayMusic(CAN1,9,EMPTY);
		PlayMusic(CAN1,8,EMPTY);
		*cnt = 0;
		return 1;
	}
	return 0;
}
/**
* @brief  和音通道
* @param  note    小节进行
* @author ACTION
*/
int PlayChord(int note)
{
	PlayMusicChannel2(CAN1,9,note);
	PlayMusicChannel2(CAN1,8,note);
	return 0;
}


