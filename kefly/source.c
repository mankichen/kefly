#include"traffic.h"

//控制数码管的闪烁,主干道，支干道
void ControlDigial(status mFlash, status bFlash) {
	ShowDigital(weLockNum[0], mainRoad.shi, mFlash);
	ShowDigital(weLockNum[1], mainRoad.ge, mFlash);
	ShowDigital(weLockNum[2], branchRoad.shi, bFlash);
	ShowDigital(weLockNum[3], branchRoad.ge, bFlash);
}

void Show() {
	//OEE=0;
	if (sec>3) {

		if (mOrB) {//1为支干道
			ShowLed(MR_BG, 0);
		}
		else {
			ShowLed(MG_BR, 0);
		}
		ControlDigial(false, false);
		return;
	}

	switch (mode)
	{
	case 0:
		if (mOrB) {
			ShowLed(MR_BY, 0);
		}
		else {
			ShowLed(MY_BR, 0);
		}
		ControlDigial(false, false);
		break;
	case 1:
		if (mOrB) {
			ShowLed(MR_BY, 1);
		}
		else {
			ShowLed(MY_BR, 1);
		}
		ControlDigial(false, false);
		break;
	default:
		if (mOrB) {
			ShowLed(MR_BY, 1);
			ControlDigial(false, true);
		}
		else {
			ShowLed(MY_BR, 1);
			ControlDigial(true, false);
		}
		break;
	}
}

void Init() {
	//TMOD &= 0xF0;		//设置定时器模式
	EA = 1;
	TMOD &= 0xF0;
	TMOD |= 0x01;
	TL0 = 0x78;
	TH0 = 0xEC;
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;

	EX0 = 1;
	TCON |= 0x01;//外部中断0
	OEE = 0;
}

void main() {
	
	Init();
	while (true)
	{
		InitLed();//将灯初始化为全灭
		Show();
	}

}