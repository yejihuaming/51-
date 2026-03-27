#include <REGX52.H>
#include <intrins.h>

#define SPEED 714 //歌曲节奏快慢

//音高索引，L为低音，M为中音，H为高音，下滑线为升半音，音域为C3-C5
#define P	0
#define L1	1
#define L1_	2
#define L2	3
#define L2_	4
#define L3	5
#define L4	6
#define L4_	7
#define L5	8
#define L5_	9
#define L6	10
#define L6_	11
#define L7	12
#define M1	13
#define M1_	14
#define M2	15
#define M2_	16
#define M3	17
#define M4	18
#define M4_	19
#define M5	20
#define M5_	21
#define M6	22
#define M6_	23
#define M7	24
#define H1	25
#define H1_	26
#define H2	27
#define H2_	28
#define H3	29
#define H4	30
#define H4_	31
#define H5	32
#define H5_	33
#define H6	34
#define H6_	35
#define H7	36

//C4 - C6 音高频率的初值
unsigned int  freqtable[] = {
0,
63628,63731,63835,63928,64021,64103,64185,64260,64331,64400,64463,64528,
64580,64633,64684,64732,64777,64820,64860,64898,64934,64968,65000,65030,
65058,65085,65110,65134,65157,65178,65198,65217,65235,65252,65268,65283,
};

//歌曲音高及节奏索引
unsigned char code music1[] = {
// 远航星的告别 B大调

	
//第一段主歌
M2_,6,
M1_,1,
M2_,1+2,
0,1,
M1_,1,
M2_,2,
M4_,2,
	
// 2
M1_,6,
L7,2,
0,4,
M4_,2,
M4_,2,

// 3 4
L6_,6,
L7,2,
0,4,
M1_,2,
M1_,1,
L7,17,

//5
M2_,6,
M1_,1,
M2_,3,
0,1,
M1_,1,
M2_,2,
M5_,2,

// 6
M1_,2,
0,2,
M1_,2,
M4_,1,
M1_,1,
0,3,
L7,1,
L6_,2,
L7,2,

// 7
L6_,6,
L7,2,
M1_,3,
M2_,1,
0,2,
M1_,2,

// 8
L7,4,
L6_,4,
L5_,8,

// 9
M2_,6,
M1_,1,
M2_,3,
0,1,
M1_,1,
M2_,2,
M4_,2,

//10
M1_,6,
L7,1,
M1_,1,
0,3,
L7,1,
L6_,2,
L7,2,

//11
L6_,4,
L6_,2,
L7,2,
M1_,6,
M1_,2,

//12
L7,1,
L6_,2,
L7,1,
L6_,2,
L7,1,
L6_,1,
L6_,1,
L7,2,
L6_,1,
L7,2,
M1_,2,

//13
M2_,6,
M1_,1,
M2_,3,
0,1,
M1_,1,
M2_,2,
M5_,2,

//14
M1_,6,
M4_,1,
M1_,1,
0,3,
L7,1,
L6_,2,
L7,2,

//15
L6_,6,
L6_,1,
L7,1,
M1_,4,
M2_,4,

0XFF //标志结束
};

unsigned char  code music2[] = {

//16 副歌
M1_,2,
L7,2,
L7,4,
M7,4,
H1_,4,

//17
H2_,8,
H2_,2,
H1_,1,
H1_,2,
M7,3,

//18
M5_,8,
H2_,2,
H1_,1,
H1_,2,
M7,3,

//19
M4_,4,
M5_,2,
M2_,1,
M4_,5,
H1_,2,
M7,2,

//20
M6_,6,
M7,1,
M7,5,
0,4,

//21
H2_,8,
H2_,2,
H1_,1,
H1_,2,
M7,3,

//22
M5_,8,
H2_,2,
H1_,1,
H1_,2,
M7,3,

//23
M4_,6,
M4_,2,
M6_,3,
M6_,3,
M6_,2,

//24
M6_,3,
M7,3,
M6_,2,
H1_,4,
M6_,4,

0XFF 
};

unsigned char freqselect ;//存放频率
char number[10] = {0X3F,0X06,0X5B,0X4F,0X66,0X6D,0X7D,0X07,0X7F,0X6F};//数码管段选
int start = 0;//启动标志
unsigned int i = 0,j = 0,k = 0; //时间变量，i为分，j为秒的十位，k为秒的个位

//延时函数
void delay(int xms)	
{
	int i,j;
	for(i=0;i<xms;i++){
		for(j=0;j<120;j++);
	}
}

//初始化函数
void init()
{
	
	EA = 1;
	
	TR0 = 1;
	ET0 = 1;
	
	TR1 = 0;
	ET1 = 0;
	
	
	TMOD =0X11;
	
	TH0 = 0xFF;     
  TL0 = 0x06; 
	
	TH1 = 0xF4;
  TL1 = 0x48;
	
	
	P3 = 0XFF;
	P2 = 0XFF;
}

//演奏函数
void perform(){
    static int song_flag = 1; //歌曲段数
    static unsigned char index = 0; //索引
    unsigned char *p;//存放歌曲数组地址
		
		//根据段数更换p指向的地址
    if(song_flag == 1)
        p = music1;
    else
        p = music2;
		
		//演奏
    if(p[index] != 0XFF&&start==1){
        freqselect = p[index];  //给音高
        index++;
				delay(SPEED/4 * p[index]);//给节奏
        index++;

        delay(5);//音的更换
		}
    else if(p[index] == 0XFF){
        
        if(song_flag == 1){
            song_flag = 2;//换歌曲段数，因为发现太长了会被截断，用code也不行
            index = 0;      
        }
        else{
            TR0 = 0;//结束
            while(1);
        }
    }
}

//按键识别函数
void button(){
unsigned char key = 0;//按键标志
	while(P1==0XFF)perform();
				delay(20);
			switch(P1)
			{
				case 0XFE:key = 1;break;//按下识别
				case 0XFD:key = 2;break;
			}
			while(P1!=0XFF);
			switch(key)
			{
				case 1:start = 1;break;													//1代表启动
				case 2:start = 0;break;													//2代表暂停											
			}
}

//主函数
void main()
{
	init();
	while(1){
		
		perform();
		button();
		
	}
}

//数码管显示函数
void display()
{
		static unsigned char num = 0;
		
		//每次调用重置位码与段码，防止闪烁
		P2 = 0XFF;
		P0 = 0X00;
		
		//段选与位选，动态扫描
		switch(num){
			case 0:P0 = number[i];P2 = 0XFE;break;
			case 1:P0 = 0X40;			P2 = 0XFD;break;
			case 2:P0 = number[j];P2 = 0XFB;break;
			case 3:P0 = number[k];P2 = 0XF7;break;
		}
		num++;
		if(num>3)
		{
			num=0;
		}
}

//计时增加函数
void increase()
{
	static unsigned int count = 0;
	if(start==1)
	{
		count++;
		
		//秒分依次加			
		if(count>=100){
			count = 0;
			k++;
			if(k>9)
			{
				j++;
				k=0;
				if(j>5)
				{
					j = 0;
					i++;
					if(i>9){
						i=0;
					}
				}
			}
		}
	}
}

//定时器0给音高
void Timer0_ISR(void) interrupt 1
{
	//给音高初值
	if(freqtable[freqselect]!=0&&start==1){
		TH0 = freqtable[freqselect]/256;     
		TL0 = freqtable[freqselect]%256;
		
		P3_3 = !P3_3;//给蜂鸣器频率
	}
}

//定时器1给数码管计时
void Timer1_ISR(void) interrupt 3
{
    TH1 = 0xD8;//10ms一次中断
    TL1 = 0xF0;
		
		
		display();
	
	if(start==1)increase();
}