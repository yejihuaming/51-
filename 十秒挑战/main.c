#include <REGX52.H>
char number[10] = {0X3F,0X06,0X5B,0X4F,0X66,0X6D,0X7D,0X07,0X7F,0X6F};//数码管段选
int flag = -1;//启动标志位
int i=0,j=0,k=0,l=0,m=0,n=0;//控制显示数字的大小l是分，i是秒的十位，j是秒的个位，k是毫秒的百分位，m是毫秒的十位，n是毫秒的个位
int key = -1;//按键标志位

//延时函数，用于消抖
void delay(int xms)	
{
	int i,j;
	for(i=0;i<xms;i++){
		for(j=0;j<120;j++);
	}
}

//初始化函数
void inittimer()
{
	//定时器初始化
	ET0 = 1;
	EA = 1;
	TR0 = 1;
	TR1 = 1;
	
	TMOD =0X21;
	SCON = 0X50;
	
	TH0 = 0XFC;//1ms中断一次
	TL0 = 0X66;
	
	TH1 = 0XFD;//波特率9600
	TL1 = 0XFD;
	
	//引脚初始化
	P2 = 0X00;
	P3 = 0X00;
	P0 = 0XFF;
	P1 = 0XFF;
}

//发送数据函数
void sendByte(unsigned char dat)
{
	SBUF = dat;
	while(!TI);
	TI = 0;
}

//发送两位数字
void sendNum2(int num)
{
	sendByte(num/10+'0');
	sendByte(num%10+'0');
}

//发送三位数字
void sendNum3(int num)
{
	sendByte(num/100+'0');
	sendByte(num%100/10+'0');
	sendByte(num%10+'0');
}

//发送字符串
void sendStr(char *str)
{
	while(*str != '\0'){  
		sendByte(*str);
		str++;            
	}
}

//发送调控函数
void sendTime()
{
	sendNum2(l); 							sendStr("分");
	sendNum2(10*i+j);					sendStr("秒");
	sendNum3(100*k+10*m+n);		sendStr("毫秒");
	sendByte(0x0D); //回车字符
  sendByte(0x0A); //换行字符
}

//按键检测函数
void button()
{
	while(P1==0XFF);
				delay(20);
			switch(P1)
			{
				case 0XFE:key = 1;break;
				case 0XFD:key = 2;break;
				case 0XFB:key = 3;break;
				case 0XF7:key = 4;break;
			}
			while(P1!=0XFF);
			switch(key)
			{
				case 1:flag = 1;break;													//1代表启动计时
				case 2:flag = -1;break;													//2代表暂停计时
				case 3:flag = -1;i=j=k=l=m=n=0;P0 = 0XFF;break;	//3代表清零
				case 4:sendTime();break;												//4代表记录当前值
			}
}

//主函数
void main()
{
	inittimer();
	while(1)
	{
		button();	
	}
}

//成绩判断函数
void judge(int j,int k)
{
	if((j==5&&k>4)||(j==6&&k<6))P0 = 0XFE;	//5.5~6.5秒亮一个LED
	if((j==6&&k>4)||(j==7&&k<6))P0 = 0XFC;	//6.5~7.5秒亮两个LED
	if((j==7&&k>4)||(j==8&&k<6))P0 = 0XF8;	//7.5~8.5秒亮三个LED
	if((j==8&&k>4)||(j==9&&k<6))P0 = 0XF0;	//8.5~9.5秒亮四个LED
	if((j==9&&k>4)||(j==10&&k<6))P0 = 0XE0;	//9.5~10.5秒亮五个LED
}

//计时增加函数
void increase()
{
	static unsigned int count = 0;
	if(flag==1)
	{
		count++;
		
		//将count百位十位个位依次分给k、m、n ，k是毫秒的百分位，m是毫秒的十位，n是毫秒的个位
		n = count%10;
    m = (count / 10) % 10;
		k = count/100;
		
		//秒分依次加			
		if(count>=1000){
			count = 0;
			j++;
			if(j>9)
			{
				i++;
				j=0;
				if(i>6)
				{
					i = 0;
					l++;
					if(l>9){
						l=0;
					}
				}
			}
		}
		judge(j,k);
	}
}

//数码管显示函数
void display()
{
		static unsigned char num = 0;
		
		//每次调用重置位码与段码，防止闪烁
		P3 = 0XFF;
		P2 = 0X00;
		
		//段选，用了138译码器
		switch(num){
			case 0:P2 = number[l];P3_2 = 0;P3_3 = 0;P3_4 = 0;break;
			case 1:P2 = 0X40;			P3_2 = 1;P3_3 = 0;P3_4 = 0;break;
			case 2:P2 = number[i];P3_2 = 0;P3_3 = 1;P3_4 = 0;break;
			case 3:P2 = number[j];P3_2 = 1;P3_3 = 1;P3_4 = 0;break;
			case 4:P2 = 0X40;			P3_2 = 0;P3_3 = 0;P3_4 = 1;break;
			case 5:P2 = number[k];P3_2 = 1;P3_3 = 0;P3_4 = 1;break;
			case 6:P2 = number[m];P3_2 = 0;P3_3 = 1;P3_4 = 1;break;
			case 7:P2 = number[n];P3_2 = 1;P3_3 = 1;P3_4 = 1;break;
		}
		num++;
		if(num>=8)
		{
			num=0;
		}
}

//定时器0控制数码管变化
void timer_isr0()	interrupt  1
{
	TH0 = 0XFC;
	TL0 = 0X66;
	
	display();
	
	if(flag==1)increase();
}