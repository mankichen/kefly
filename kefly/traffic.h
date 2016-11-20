#include<reg52.h>
#include <intrins.h>

#define nop()			_nop_()
#define true			1
#define false			0
#define MMAXTIME		20
#define BMAXTIME		15
#define UCHAR			unsigned char
#define MR_BG			0				//主红+支绿
#define MR_BY			1				//主红+支黄
#define MG_BR			2				//主绿+支红
#define MY_BR			3				//主黄+支红
#define status			unsigned char

typedef struct {
	unsigned char shi;
	unsigned char ge;
}TIME;

TIME mainRoad = { 2,0 };
TIME branchRoad = { 0,0 };

UCHAR code digital[] =
{ ~0x3f,~0x06,~0x5b,~0x4f,
~0x66,~0x6d,~0x7d,~0x07,
~0x7f,~0x6f };

UCHAR code weLockNum[] =
{ 0x08 ,0x04 ,0x02 ,0x01 };

sbit key = P3 ^ 2;

sbit DS = P1 ^ 0;
sbit SHCP = P1 ^ 1;
sbit STCP = P1 ^ 2;
sbit OEE = P1 ^ 3;//控制闪烁
sbit branchLedRed = P2 ^ 6;
sbit branchLedYel = P2 ^ 5;
sbit branchLedGre = P2 ^ 4;
sbit mainLedRed = P1 ^ 6;
sbit mainLedYel = P1 ^ 5;
sbit mainLedGre = P1 ^ 4;

UCHAR mode = 0;//0为都不闪；1数码管不删；2都闪
UCHAR mOrB = 0;//0主干道 and  1支干道
UCHAR sec2 = 20 - 3;//扣除了黄灯3秒
UCHAR sec = 20;
UCHAR tmp = 0;

void InitLed();

//*****************************************函数部分************************************
//在某个数码管显示数字，选位，显示数字，是否闪烁
void ShowDigital(UCHAR weLock, UCHAR num, status flash) {
	UCHAR temp = 0;
	if (sec>3) {
		if (mOrB)//真为支干道
		{
			branchRoad.ge = sec2 % 10;
			branchRoad.shi = sec2 / 10;
			mainRoad.ge = sec % 10;
			mainRoad.shi = sec / 10;
		}
		else {
			mainRoad.ge = sec2 % 10;
			mainRoad.shi = sec2 / 10;
			branchRoad.ge = sec % 10;
			branchRoad.shi = sec / 10;
		}
	}
	else {
		if (mOrB) {//真为支干道
			branchRoad.ge = sec % 10;
			branchRoad.shi = sec / 10;
			mainRoad.ge = sec % 10;
			mainRoad.shi = sec / 10;
		}
		else {
			mainRoad.ge = sec % 10;
			mainRoad.shi = sec / 10;
			branchRoad.ge = sec % 10;
			branchRoad.shi = sec / 10;
		}
	}


	//*******************************************************位选十位******************************************************

	if (flash&&tmp<100)//闪烁,数码管变化在主干道，要求闪烁，前半秒（灭）
	{
		for (temp = 0; temp <= 7; temp++) {
			if ((0x00 << temp) & 0x80) {// 0x80选  0000 0000
				DS = 1;
			}
			else {
				DS = 0;
			}
			SHCP = 0;
			nop();
			nop();
			SHCP = 1;
		}
		//OEE=1;
	}
	else {
		//OEE=0;
		for (temp = 0; temp <= 7; temp++) {
			if ((weLock << temp) & 0x80) {
				DS = 1;
			}
			else {
				DS = 0;
			}
			SHCP = 0;
			nop();
			nop();
			SHCP = 1;
		}
	}
	//***************段选****************
	for (temp = 0; temp <= 7; temp++) {
		if ((digital[num] << temp) & 0x80) {
			DS = 1;
		}
		else {
			DS = 0;
		}
		SHCP = 0;
		nop();
		nop();
		SHCP = 1;
	}
	STCP = 0;
	nop();
	nop();
	STCP = 1;
}

//显示红绿灯，总共有3总模式，flash 控制闪烁
void ShowLed(UCHAR mode, status flash) {
	switch (mode)
	{
	case MG_BR:
		mainLedGre = 0;
		branchLedRed = 0;
		break;
	case MY_BR:
		if (tmp >= 100 && flash) {
			mainLedYel = 0;
			branchLedRed = 0;
		}
		else {
			if (!flash) {
				mainLedYel = 0;
				branchLedRed = 0;
			}
			else {
				mainLedYel = 1;
				branchLedRed = 0;
			}
		}
		break;
	case MR_BG:
		mainLedRed = 0;
		branchLedGre = 0;
		break;
	case MR_BY:
		if (tmp >= 100 && flash) {
			mainLedRed = 0;
			branchLedYel = 0;
		}
		else {
			if (!flash) {
				mainLedRed = 0;
				branchLedYel = 0;
			}
			else {
				mainLedRed = 0;
				branchLedYel = 1;
			}
		}
		break;
	}
}



//初始化Led灯，重置为初始状态，全灭
void InitLed() {
	branchLedRed = 1;
	branchLedYel = 1;
	branchLedGre = 1;
	mainLedRed = 1;
	mainLedYel = 1;
	mainLedGre = 1;
}

//定时器0
void T0_time() interrupt 1{
	TL0 = 0x78;
	TH0 = 0xEC;
	TF0 = 0;		//清除TF0标志
	tmp++;
	if (tmp >= 200) {
		tmp = 0;
		sec--;
		sec2--;
		if (!(sec + 1)) {
			mOrB = ~mOrB;//两路的数码管变化转化
			if (mOrB) {//判定为支路时间
				sec = BMAXTIME;
				sec2 = BMAXTIME - 3;
			}
			else {
				sec = MMAXTIME;
				sec2 = MMAXTIME - 3;
			}
		}
	}
}

//外部中断
void INIT_key() interrupt 0{
	mode++;
	mode %= 3;
	IE0 = 0;//软件清零
}