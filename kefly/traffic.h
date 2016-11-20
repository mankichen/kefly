#include<reg52.h>
#include <intrins.h>

#define nop()			_nop_()
#define true			1
#define false			0
#define MMAXTIME		20
#define BMAXTIME		15
#define UCHAR			unsigned char
#define MR_BG			0				//����+֧��
#define MR_BY			1				//����+֧��
#define MG_BR			2				//����+֧��
#define MY_BR			3				//����+֧��
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
sbit OEE = P1 ^ 3;//������˸
sbit branchLedRed = P2 ^ 6;
sbit branchLedYel = P2 ^ 5;
sbit branchLedGre = P2 ^ 4;
sbit mainLedRed = P1 ^ 6;
sbit mainLedYel = P1 ^ 5;
sbit mainLedGre = P1 ^ 4;

UCHAR mode = 0;//0Ϊ��������1����ܲ�ɾ��2����
UCHAR mOrB = 0;//0���ɵ� and  1֧�ɵ�
UCHAR sec2 = 20 - 3;//�۳��˻Ƶ�3��
UCHAR sec = 20;
UCHAR tmp = 0;

void InitLed();

//*****************************************��������************************************
//��ĳ���������ʾ���֣�ѡλ����ʾ���֣��Ƿ���˸
void ShowDigital(UCHAR weLock, UCHAR num, status flash) {
	UCHAR temp = 0;
	if (sec>3) {
		if (mOrB)//��Ϊ֧�ɵ�
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
		if (mOrB) {//��Ϊ֧�ɵ�
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


	//*******************************************************λѡʮλ******************************************************

	if (flash&&tmp<100)//��˸,����ܱ仯�����ɵ���Ҫ����˸��ǰ���루��
	{
		for (temp = 0; temp <= 7; temp++) {
			if ((0x00 << temp) & 0x80) {// 0x80ѡ  0000 0000
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
	//***************��ѡ****************
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

//��ʾ���̵ƣ��ܹ���3��ģʽ��flash ������˸
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



//��ʼ��Led�ƣ�����Ϊ��ʼ״̬��ȫ��
void InitLed() {
	branchLedRed = 1;
	branchLedYel = 1;
	branchLedGre = 1;
	mainLedRed = 1;
	mainLedYel = 1;
	mainLedGre = 1;
}

//��ʱ��0
void T0_time() interrupt 1{
	TL0 = 0x78;
	TH0 = 0xEC;
	TF0 = 0;		//���TF0��־
	tmp++;
	if (tmp >= 200) {
		tmp = 0;
		sec--;
		sec2--;
		if (!(sec + 1)) {
			mOrB = ~mOrB;//��·������ܱ仯ת��
			if (mOrB) {//�ж�Ϊ֧·ʱ��
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

//�ⲿ�ж�
void INIT_key() interrupt 0{
	mode++;
	mode %= 3;
	IE0 = 0;//�������
}