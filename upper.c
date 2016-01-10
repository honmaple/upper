/**************************************************************************
  Copyright © 2015 JiangLin. All rights reserved.
  File Name: upper.c
Author:JiangLin
Mail:xiyang0807@gmail.com
Created Time: 2016-01-01 00:46:02
 **************************************************************************/
#include<8052.h>
#include<stdio.h>
/*------------------------------------------------
  硬件端口定义
  ------------------------------------------------*/
/*__sbit __at (0xA2) P2_2 ;*/
/*__sbit __at (0xA5) P2_5 ;*/
/*__sbit __at (0x90) P1_0 ;*/
/*__sbit __at (0x91) P1_1 ;*/
/*__sbit __at (0xA6) P2_6 ;*/
/*__sbit __at (0xA7) P2_7 ;*/
__sbit __at 0xA2 DS; //温度传感器
__sbit __at 0xB4 lcden;
__sbit __at 0xB5 rs;
__sbit __at 0xB6 rw;
__sbit __at 0xA6 dula;
__sbit __at 0xA7 wela;
__sbit __at 0xA3 feng;
unsigned char __idata sendBuf[40];
unsigned char __idata RecBuf[40];
unsigned char Range_up[] = "1500";
unsigned char Range_down[] = "0010";
unsigned char Report_up[] = "0600";
unsigned char xuehao[] = "1111111111";
unsigned char Report_down[] = "0018";
unsigned char Address[] = "0812";
unsigned char Temp[] = "0030.50";
unsigned char sendLen=0;
unsigned char lencount = 0;
unsigned char send_flag;
unsigned char rec_flag;
unsigned char LRC;


/*温度查询*/
void Delay_us(unsigned int us)
{
    while(us--);
}
/*ds18b20初始化*/
void ds_init()
{
    __bit i;
    DS = 1;
    Delay_us(0);
    DS = 0;
    Delay_us(75); //拉低总线499.45us 挂接在总线上的18B20将会全部被复位
    DS = 1;          //释放总线
    Delay_us(4);  //延时37.95us 等待18B20发回存在信号
    i = DS;
    Delay_us(20); //141.95us
    /*DS = 1;*/
    /*Delay_us(0);*/
    /*return (i);*/
}
/*读一个字节*/
unsigned char read_byte()
{
    unsigned char i,dat;
    dat = 0;
    for(i=8;i>0;i--)
        　　{
            　　DS = 0;                  //将总线拉低，要在1us之后释放总线
            　　//单片机要在此下降沿后的15us内读数据才会有效。
                　　Delay_us(0);                 //至少维持了1us,表示读时序开始
            　　dat >>= 1;               //让从总线上读到的位数据，依次从高位移动到低位。
            　　DS = 1;                  //释放总线，此后DS18B20会控制总线,把数据传输到总线上
            　　Delay_us(1);                 //延时7us,此处参照推荐的读时序图，尽量把控制器采样时间放到读时序后的15us内的最后部分
            　　if(DS)                   //控制器进行采样
                　　{
                    　　dat |= 0x80;            //若总线为1,即DQ为1,那就把dat的最高位置1;若为0,则不进行处理,保持为0
                    　　}
            　　Delay_us(10);               //此延时不能少，确保读时序的长度60us。
        }
    return (dat);
}

/*for(i=0;i<8;i++)*/
/*{*/
/*DS = 0;*/
/*Delay_us(0);//产生读时序*/
/*DS = 1;*/
/*Delay_us(0);//释放总线*/
/*j = DS;*/
/*Delay_us(10);//76.95us*/
/*DS = 1;*/
/*Delay_us(0);*/
/*dat = (j<<7)|(dat>>1);*/
/*}*/
/*return (dat);*/
/*}*/
/*写一个字节*/
void write_byte(unsigned char dat)
{
    unsigned char i;
    for(i=0;i<8;i++)
    {
        DS = 0;
        Delay_us(0);//产生些时序
        DS = dat&0x01;
        Delay_us(10);//76.95us
        DS = 1;         //释放总线准备下一次数据写入
        dat >>= 1;
        /*Delay_us(0);*/
        /*dat >>= 1;*/
    }
}

/*得到温度值*/
unsigned int Get_temp()
{
    unsigned int i;
    unsigned char L, M;
    ds_init();//初始化DS18B20
    write_byte(0xcc);//发送跳跃ROM指令
    write_byte(0x44);//发送温度转换指令
    ds_init();//初始化DS18B20
    write_byte(0xcc);//发送跳跃ROM指令
    write_byte(0xbe);//读取DS18B20暂存器值
    L = read_byte();
    M = read_byte();
    i = M;
    i <<= 8;
    i = i|L;
    /*i |= L;*/
    i = i * 0.625 + 0.5;
    return (i);
}
/*lcd显示*/
void delay(unsigned int x)
{
    unsigned int a,b;
    for(a=x;a>0;a--)
        for(b=10;b>0;b--);
}

void write_com(unsigned char com)
{
    P0=com;
    rs=0;
    lcden=0;
    delay(10);
    lcden=1;
    delay(10);
    lcden=0;
}

void write_date(unsigned char date)
{
    P0=date;
    rs=1;
    lcden=0;
    delay(10);
    lcden=1;
    delay(10);
    lcden=0;

}
/*LCD初始化*/
void Lcd_init()
{
    rw=0;
    dula=0;
    wela=0;
    write_com(0x38);   //显示模式设置：16×2显示，5×7点阵，8位数据接口
    delay(20);
    write_com(0x0c);   //显示模式设置
    delay(20);
    write_com(0x06);   //显示模式设置：光标右移，字符不移
    delay(20);
    write_com(0x01);   //清屏幕指令，将以前的显示内容清除
    delay(20);
}
/*显示LCD*/
void show(unsigned int data)
{
    int i;
    unsigned char student[] = "honmaple";
    unsigned char temperture[] = "Temp:";
    unsigned char num[6];
    num[0] = data/100000;
    num[1] = data/10000%10;
    num[2] = data/1000%10;
    num[3] = data/100%10;
    num[4] = data/10%10;
    num[5] = data%10;
    write_com(0x80);	 //将第一个字符写在向右偏移17个字符处，为后面的由右向左划入做准备。
    delay(20);
    for(i=0;i<15;i++)
    {
        write_date(student[i]);
        delay(20);
    }

    write_com(0xc0);
    delay(50);
    for(i=0;i<5;i++)
    {
        write_date(temperture[i]);
        delay(20);
    }
    for(i=0;i<4;i++)
    {
        write_date(num[i] + 0x30);
        delay(20);
    }
    write_date(0x2e);
    delay(20);
    for(i=4;i<6;i++)
    {
        write_date(num[i] + 0x30);
        delay(20);
    }
}
/*设置地址*/
void Set_addr()
{
    int i;
    unsigned char lrc_test;
    LRC = 0;
    sendBuf[0] = 0x23;

    for(i = 0; i <= 3; i++)
        Address[i] = RecBuf[i + 9];

    for (i = 1; i <= 4; i++) {
        sendBuf[i] = Address[i - 1];
    }
    /*功能码*/
    sendBuf[5] = 0x35;
    sendBuf[6] = 0x30;
    /*数据长度4位，地址*/
    sendBuf[7] = 0x30;
    sendBuf[8] = 0x31;
    sendBuf[9] = 0x30;
    for(i = 1; i <= 9; i++)
        LRC =LRC + sendBuf[i];
    lrc_test =255 - LRC +1;
    sendBuf[10] = lrc_test >> 4;
    sendBuf[11] = lrc_test & 0x0f;
    if(sendBuf[10]<10)
    {
        sendBuf[10]+=0x30;
    }
    else
    {
        sendBuf[10]+=87;
    }
    if(sendBuf[11]<10)
    {
        sendBuf[11]+=0x30;
    }
    else
    {
        sendBuf[11]+=87;
    }
    /*结束符*/
    sendBuf[12] = 0x40;
    sendBuf[13] = 0x24;
    sendLen = 14;
}
/*设置量程上限*/
void Set_range_up()
{
    int i;
    unsigned char lrc_test;
    LRC = 0;
    sendBuf[0] = 0x23;

    for(i = 0; i <= 3; i++)
        Range_up[i] = RecBuf[i + 9];

    for (i = 1; i <= 4; i++) {
        sendBuf[i] = Address[i - 1];
    }
    /*功能码*/
    sendBuf[5] = 0x35;
    sendBuf[6] = 0x31;
    /*数据长度4位，地址*/
    sendBuf[7] = 0x30;
    sendBuf[8] = 0x31;
    sendBuf[9] = 0x30;
    for(i = 1; i <= 9; i++)
        LRC =LRC + sendBuf[i];
    lrc_test =255 - LRC +1;
    sendBuf[10] = lrc_test >> 4;
    sendBuf[11] = lrc_test & 0x0f;
    if(sendBuf[10]<10)
    {
        sendBuf[10]+=0x30;
    }
    else
    {
        sendBuf[10]+=87;
    }
    if(sendBuf[11]<10)
    {
        sendBuf[11]+=0x30;
    }
    else
    {
        sendBuf[11]+=87;
    }
    /*结束符*/
    sendBuf[12] = 0x40;
    sendBuf[13] = 0x24;
    sendLen = 14;
}
/*设置量程下限*/
void Set_range_down()
{
    int i;
    unsigned char lrc_test;
    LRC = 0;
    sendBuf[0] = 0x23;

    for(i = 0; i <= 3; i++)
        Range_down[i] = RecBuf[i + 9];

    for (i = 1; i <= 4; i++) {
        sendBuf[i] = Address[i - 1];
    }
    /*功能码*/
    sendBuf[5] = 0x35;
    sendBuf[6] = 0x32;
    /*数据长度4位，地址*/
    sendBuf[7] = 0x30;
    sendBuf[8] = 0x31;
    sendBuf[9] = 0x30;
    for(i = 1; i <= 9; i++)
        LRC =LRC + sendBuf[i];
    lrc_test =255 - LRC +1;
    sendBuf[10] = lrc_test >> 4;
    sendBuf[11] = lrc_test & 0x0f;
    if(sendBuf[10]<10)
    {
        sendBuf[10]+=0x30;
    }
    else
    {
        sendBuf[10]+=87;
    }
    if(sendBuf[11]<10)
    {
        sendBuf[11]+=0x30;
    }
    else
    {
        sendBuf[11]+=87;
    }
    /*结束符*/
    sendBuf[12] = 0x40;
    sendBuf[13] = 0x24;
    sendLen = 14;
}
/*设置报警上限*/
void Set_report_up()
{
    int i;
    unsigned char lrc_test;
    LRC = 0;
    sendBuf[0] = 0x23;

    for(i = 0; i <= 3; i++)
        Report_up[i] = RecBuf[i + 9];

    for (i = 1; i <= 4; i++) {
        sendBuf[i] = Address[i - 1];
    }
    /*功能码*/
    sendBuf[5] = 0x35;
    sendBuf[6] = 0x33;
    /*数据长度4位，地址*/
    sendBuf[7] = 0x30;
    sendBuf[8] = 0x31;
    sendBuf[9] = 0x30;
    for(i = 1; i <= 9; i++)
        LRC =LRC + sendBuf[i];
    lrc_test =255 - LRC +1;
    sendBuf[10] = lrc_test >> 4;
    sendBuf[11] = lrc_test & 0x0f;
    if(sendBuf[10]<10)
    {
        sendBuf[10]+=0x30;
    }
    else
    {
        sendBuf[10]+=87;
    }
    if(sendBuf[11]<10)
    {
        sendBuf[11]+=0x30;
    }
    else
    {
        sendBuf[11]+=87;
    }
    /*结束符*/
    sendBuf[12] = 0x40;
    sendBuf[13] = 0x24;
    sendLen = 14;
}
/*设置报警下限*/
void Set_report_down()
{
    int i;
    unsigned char lrc_test;
    LRC = 0;
    sendBuf[0] = 0x23;

    for(i = 0; i <= 3; i++)
        Report_down[i] = RecBuf[i + 9];

    for (i = 1; i <= 4; i++) {
        sendBuf[i] = Address[i - 1];
    }
    /*功能码*/
    sendBuf[5] = 0x35;
    sendBuf[6] = 0x34;
    /*数据长度4位，地址*/
    sendBuf[7] = 0x30;
    sendBuf[8] = 0x31;
    sendBuf[9] = 0x30;
    for(i = 1; i <= 9; i++)
        LRC =LRC + sendBuf[i];
    lrc_test =255 - LRC +1;
    sendBuf[10] = lrc_test >> 4;
    sendBuf[11] = lrc_test & 0x0f;
    if(sendBuf[10]<10)
    {
        sendBuf[10]+=0x30;
    }
    else
    {
        sendBuf[10]+=87;
    }
    if(sendBuf[11]<10)
    {
        sendBuf[11]+=0x30;
    }
    else
    {
        sendBuf[11]+=87;
    }
    /*结束符*/
    sendBuf[12] = 0x40;
    sendBuf[13] = 0x24;
    sendLen = 14;
}
/*查询地址*/
void Query_addr()
{
    int i;
    unsigned char lrc_test;
    LRC = 0;
    sendBuf[0] = 0x23;
    for (i = 1; i <= 4; i++) {
        sendBuf[i] = Address[i - 1];
    }
    /*功能码*/
    sendBuf[5] = 0x30;
    sendBuf[6] = 0x30;
    /*数据长度4位，地址*/
    sendBuf[7] = 0x30;
    sendBuf[8] = 0x34;
    sendBuf[9] = Address[0];
    sendBuf[10] = Address[1];
    sendBuf[11] = Address[2];
    sendBuf[12] = Address[3];
    for(i = 1; i <= 12; i++)
        LRC =LRC + sendBuf[i];
    lrc_test =255 - LRC +1;
    sendBuf[13] = lrc_test >> 4;
    sendBuf[14] = lrc_test & 0x0f;
    if(sendBuf[13]<10)
    {
        sendBuf[13]+=0x30;
    }
    else
    {
        sendBuf[13]+=87;
    }
    if(sendBuf[14]<10)
    {
        sendBuf[14]+=0x30;
    }
    else
    {
        sendBuf[14]+=87;
    }
    /*结束符*/
    sendBuf[15] = 0x40;
    sendBuf[16] = 0x24;
    sendLen = 17;
}
/*查询量程下限*/
void Query_range_up()
{
    int i;
    unsigned char lrc_test;
    LRC = 0;
    sendBuf[0] = 0x23;
    for (i = 1; i <= 4; i++) {
        sendBuf[i] = Address[i - 1];
    }
    /*功能码*/
    sendBuf[5] = 0x30;
    sendBuf[6] = 0x31;
    /*数据长度4位，地址*/
    sendBuf[7] = 0x30;
    sendBuf[8] = 0x34;
    sendBuf[9] = Range_up[0];
    sendBuf[10] = Range_up[1];
    sendBuf[11] = Range_up[2];
    sendBuf[12] = Range_up[3];
    for(i = 1; i <= 12; i++)
        LRC =LRC + sendBuf[i];
    lrc_test =255 - LRC +1;
    sendBuf[13] = lrc_test >> 4;
    sendBuf[14] = lrc_test & 0x0f;
    if(sendBuf[13]<10)
    {
        sendBuf[13]+=0x30;
    }
    else
    {
        sendBuf[13]+=87;
    }
    if(sendBuf[14]<10)
    {
        sendBuf[14]+=0x30;
    }
    else
    {
        sendBuf[14]+=87;
    }
    /*结束符*/
    sendBuf[15] = 0x40;
    sendBuf[16] = 0x24;
    sendLen = 17;
}
/*查询量程下限*/
void Query_range_down()
{
    int i;
    unsigned char lrc_test;
    LRC = 0;
    sendBuf[0] = 0x23;
    for (i = 1; i <= 4; i++) {
        sendBuf[i] = Address[i - 1];
    }
    /*功能码*/
    sendBuf[5] = 0x30;
    sendBuf[6] = 0x32;
    /*数据长度4位，地址*/
    sendBuf[7] = 0x30;
    sendBuf[8] = 0x34;
    sendBuf[9] = Range_down[0];
    sendBuf[10] = Range_down[1];
    sendBuf[11] = Range_down[2];
    sendBuf[12] = Range_down[3];
    for(i = 1; i <= 12; i++)
        LRC =LRC + sendBuf[i];
    lrc_test =255 - LRC +1;
    sendBuf[13] = lrc_test >> 4;
    sendBuf[14] = lrc_test & 0x0f;
    if(sendBuf[13]<10)
    {
        sendBuf[13]+=0x30;
    }
    else
    {
        sendBuf[13]+=87;
    }
    if(sendBuf[14]<10)
    {
        sendBuf[14]+=0x30;
    }
    else
    {
        sendBuf[14]+=87;
    }
    /*结束符*/
    sendBuf[15] = 0x40;
    sendBuf[16] = 0x24;
    sendLen = 17;
}
/*查询报警上限*/
void Query_report_up()
{
    int i;
    unsigned char lrc_test;
    LRC = 0;
    sendBuf[0] = 0x23;
    for (i = 1; i <= 4; i++) {
        sendBuf[i] = Address[i - 1];
    }
    /*功能码*/
    sendBuf[5] = 0x30;
    sendBuf[6] = 0x33;
    /*数据长度4位，地址*/
    sendBuf[7] = 0x30;
    sendBuf[8] = 0x34;
    sendBuf[9] = Report_up[0];
    sendBuf[10] = Report_up[1];
    sendBuf[11] = Report_up[2];
    sendBuf[12] = Report_up[3];
    for(i = 1; i <= 12; i++)
        LRC =LRC + sendBuf[i];
    lrc_test =255 - LRC +1;
    sendBuf[13] = lrc_test >> 4;
    sendBuf[14] = lrc_test & 0x0f;
    if(sendBuf[13]<10)
    {
        sendBuf[13]+=0x30;
    }
    else
    {
        sendBuf[13]+=87;
    }
    if(sendBuf[14]<10)
    {
        sendBuf[14]+=0x30;
    }
    else
    {
        sendBuf[14]+=87;
    }
    /*结束符*/
    sendBuf[15] = 0x40;
    sendBuf[16] = 0x24;
    sendLen = 17;
}
/*查询报警下限*/
void Query_report_down()
{
    int i;
    unsigned char lrc_test;
    LRC = 0;
    sendBuf[0] = 0x23;
    for (i = 1; i <= 4; i++) {
        sendBuf[i] = Address[i - 1];
    }
    /*功能码*/
    sendBuf[5] = 0x30;
    sendBuf[6] = 0x34;
    /*数据长度4位，地址*/
    sendBuf[7] = 0x30;
    sendBuf[8] = 0x34;
    sendBuf[9] = Report_down[0];
    sendBuf[10] = Report_down[1];
    sendBuf[11] = Report_down[2];
    sendBuf[12] = Report_down[3];
    for(i = 1; i <= 12; i++)
        LRC =LRC + sendBuf[i];
    lrc_test =255 - LRC +1;
    sendBuf[13] = lrc_test >> 4;
    sendBuf[14] = lrc_test & 0x0f;
    if(sendBuf[13]<10)
    {
        sendBuf[13]+=0x30;
    }
    else
    {
        sendBuf[13]+=87;
    }
    if(sendBuf[14]<10)
    {
        sendBuf[14]+=0x30;
    }
    else
    {
        sendBuf[14]+=87;
    }
    /*结束符*/
    sendBuf[15] = 0x40;
    sendBuf[16] = 0x24;
    sendLen = 17;
}
/*查询温度值*/
void Query_temp()
{
    unsigned char lrc_test;
    int i;
    LRC = 0;
    sendBuf[0] = 0x23;
    for(i = 1; i <= 4; i++)
        sendBuf[i] = Address[i - 1];
    sendBuf[5] = 0x30;
    sendBuf[6] = 0x35;
    sendBuf[7] = 0x30;
    sendBuf[8] = 0x37;
    sendBuf[9] = Temp[0];
    sendBuf[10] = Temp[1];
    sendBuf[11] = Temp[2];
    sendBuf[12] = Temp[3];
    sendBuf[13] = Temp[4];
    sendBuf[14] = Temp[5];
    sendBuf[15] = Temp[6];
    for(i = 1; i <= 15; i++)
        LRC =LRC + sendBuf[i];
    lrc_test =255 - LRC +1;
    sendBuf[16] = lrc_test >> 4;
    sendBuf[17] = lrc_test & 0x0f;
    if(sendBuf[16]<10)
    {
        sendBuf[16]+=0x30;
    }
    else
    {
        sendBuf[16]+=87;
    }
    if(sendBuf[17]<10)
    {
        sendBuf[17]+=0x30;
    }
    else
    {
        sendBuf[17]+=87;
    }
    sendBuf[18] = 0x40;
    sendBuf[19] = 0x24;
    sendLen = 20;
}
/*查询报警状态*/
void Query_report_status()
{
    int i;
    unsigned char lrc_test;
    float temp;
    float up,down;
    up = 0;
    down = 0;
    temp = 0;
    LRC = 0;
    temp = (Temp[0] - 0x30) * 1000 + (Temp[1] - 0x30) * 100 + (Temp[2] - 0x30) * 10 + (Temp[3] - 0x30) ;
    temp +=(Temp[5] - 0x30) * 0.10;
    temp +=(Temp[6] - 0x30) * 0.01;  //计算温度值
    sendBuf[0] = 0x23;
    for(i = 1; i <= 4; i++)
        sendBuf[i] = Address[i - 1];
    sendBuf[5] = 0x30;
    sendBuf[6] = 0x36;
    sendBuf[7] = 0x30;
    sendBuf[8] = 0x31;
    for(i = 0;i < 4;i ++)
    {
        down += (Report_down[i] - 0x30) * 10^(3 - i);
    }
    up = (Report_up[0] - 0x30) * 1000 + (Report_up[1] - 0x30) * 100 + (Report_up[2] - 0x30) * 10 + (Report_up[3] - 0x30);
    down = (Report_down[0] - 0x30) * 1000 + (Report_down[1] - 0x30) * 100 + (Report_down[2] - 0x30) * 10 + (Report_down[3] - 0x30);

    if( temp > up || temp < down )
        sendBuf[9] = 0x36;
    else
        sendBuf[9] = 0x35;
    for(i = 1; i <= 9; i++)
        LRC =LRC + sendBuf[i];
    lrc_test = 255- LRC + 1;
    sendBuf[10] = lrc_test >> 4 ;
    sendBuf[11] = lrc_test & 0x0f;
    if(sendBuf[10]<10)
    {
        sendBuf[10]+=0x30;
    }
    else
    {
        sendBuf[10]+=87;
    }
    if(sendBuf[11]<10)
    {
        sendBuf[11]+=0x30;
    }
    else
    {
        sendBuf[11]+=87;
    }
    sendBuf[12] = 0x40;
    sendBuf[13] = 0x24;
    sendLen = 14;
}
/*查询全部*/
void Query_all()
{
    int i;
    unsigned char lrc_test;
    LRC = 0;
    sendBuf[0] = 0x23;
    for (i = 1; i <= 4; i++) {
        sendBuf[i] = Address[i - 1];
    }
    /*功能码*/
    sendBuf[5] = 0x30;
    sendBuf[6] = 0x37;
    /*数据长度4位，地址*/
    sendBuf[7] = 0x30;
    sendBuf[8] = 0x46;
    sendBuf[9] = Range_up[0];
    sendBuf[10] = Range_up[1];
    sendBuf[11] = Range_up[2];
    sendBuf[12] = Range_up[3];
    sendBuf[13] = Range_down[0];
    sendBuf[14] = Range_down[1];
    sendBuf[15] = Range_down[2];
    sendBuf[16] = Range_down[3];
    sendBuf[17] = Report_up[0];
    sendBuf[18] = Report_up[1];
    sendBuf[19] = Report_up[2];
    sendBuf[20] = Report_up[3];
    sendBuf[21] = Report_down[0];
    sendBuf[22] = Report_down[1];
    sendBuf[23] = Report_down[2];
    sendBuf[24] = Report_down[3];
    for(i = 1; i <= 24; i++)
        LRC =LRC + sendBuf[i];
    lrc_test =255 - LRC +1;
    sendBuf[25] = lrc_test >> 4;
    sendBuf[26] = lrc_test & 0x0f;
    if(sendBuf[25]<10)
    {
        sendBuf[25]+=0x30;
    }
    else
    {
        sendBuf[25]+=87;
    }
    if(sendBuf[26]<10)
    {
        sendBuf[26]+=0x30;
    }
    else
    {
        sendBuf[26]+=87;
    }
    /*结束符*/
    sendBuf[27] = 0x40;
    sendBuf[28] = 0x24;
    sendLen = 29;
}
void Query_other()
{
    int i;
    unsigned char lrc_test;
    LRC = 0;
    sendBuf[0] = 0x23;
    for (i = 1; i <= 4; i++) {
        sendBuf[i] = Address[i - 1];
    }
    sendBuf[5] = 0x30;
    sendBuf[6] = 0x38;
    sendBuf[7] = 0x31;
    sendBuf[8] = 0x30;
    sendBuf[9] = xuehao[0];
    sendBuf[10] = xuehao[1];
    sendBuf[11] = xuehao[2];
    sendBuf[12] = xuehao[3];
    sendBuf[13] = xuehao[4];
    sendBuf[14] = xuehao[5];
    sendBuf[15] = xuehao[6];
    sendBuf[16] = xuehao[7];
    sendBuf[17] = xuehao[8];
    sendBuf[18] = xuehao[9];
    for(i = 1; i <= 18; i++)
        LRC =LRC + sendBuf[i];
    lrc_test =255 - LRC +1;
    sendBuf[19] = lrc_test >> 4;
    sendBuf[20] = lrc_test & 0x0f;
    if(sendBuf[19]<10)
    {
        sendBuf[19]+=0x30;
    }
    else
    {
        sendBuf[19]+=87;
    }
    if(sendBuf[20]<10)
    {
        sendBuf[20]+=0x30;
    }
    else
    {
        sendBuf[20]+=87;
    }
    /*结束符*/
    sendBuf[21] = 0x40;
    sendBuf[22] = 0x24;
    sendLen = 23;
}
void Set_other()
{
    int i;
    unsigned char lrc_test;
    LRC = 0;
    sendBuf[0] = 0x23;
    for (i = 1; i <= 4; i++) {
        sendBuf[i] = Address[i - 1];
    }
    for (i = 0; i <10; i++) {
        xuehao[i] = RecBuf[i + 9];
    }
    sendBuf[5] = 0x35;
    sendBuf[6] = 0x38;
    sendBuf[7] = 0x31;
    sendBuf[8] = 0x30;
    sendBuf[9] = xuehao[0];
    sendBuf[10] = xuehao[1];
    sendBuf[11] = xuehao[2];
    sendBuf[12] = xuehao[3];
    sendBuf[13] = xuehao[4];
    sendBuf[14] = xuehao[5];
    sendBuf[15] = xuehao[6];
    sendBuf[16] = xuehao[7];
    sendBuf[17] = xuehao[8];
    sendBuf[18] = xuehao[9];
    for(i = 1; i <= 18; i++)
        LRC =LRC + sendBuf[i];
    lrc_test =255 - LRC +1;
    sendBuf[19] = lrc_test >> 4;
    sendBuf[20] = lrc_test & 0x0f;
    if(sendBuf[19]<10)
    {
        sendBuf[19]+=0x30;
    }
    else
    {
        sendBuf[19]+=87;
    }
    if(sendBuf[20]<10)
    {
        sendBuf[20]+=0x30;
    }
    else
    {
        sendBuf[20]+=87;
    }
    /*结束符*/
    sendBuf[21] = 0x40;
    sendBuf[22] = 0x24;
    sendLen = 23;
}
unsigned int Address_check()
{
    int i=0;
    unsigned char Address_count[4];
    if(RecBuf[1] == 0x30&&RecBuf[2]==0x30&&RecBuf[3]==0x30&&RecBuf[4]==0x30)
    {
        i = 1;
    }
    Address_count[0] = Address[0];
    Address_count[1] = Address[1];
    Address_count[2] = Address[2];
    Address_count[3] = Address[3];
    if(RecBuf[1]==Address_count[0]&&RecBuf[2]==Address_count[1]&&RecBuf[3]==Address_count[2]&&RecBuf[4]==Address_count[3])
    {
        i = 2;
    }
    return i;
}
/*初始化*/
void init()
{
    TMOD = 0x20;               // TMOD: timer 1, mode 2, 8-bit 重装
    TH1 = 0xfd;         // TH1:  重装值 9600 波特率 晶振 11.0592MHz
    TL1 = 0xfd;
    TR1 = 1;                  // TR1:  timer 1 打开
    REN = 1;
    SM0 = 0;
    SM1 = 1;
    EA  = 1;                  //打开总中断
    ES  = 1;                  //打开串口中断
    dula = 0;
    wela = 0;
}
/*主函数*/
void main ()
{
    int i;
    unsigned int TempBuff;
    init();
    Lcd_init();
    /*ds_init();*/
    while (1)
    {
        if(send_flag==1) //发送数据标志位
        {
            send_flag=0;
            if(RecBuf[5] == 0x30 && RecBuf[6] == 0x30)
                Query_addr();
            else if(RecBuf[5] == 0x30 && RecBuf[6] == 0x31)
                Query_range_up();
            else if(RecBuf[5] == 0x30 && RecBuf[6] == 0x32)
                Query_range_down();
            else if(RecBuf[5] == 0x30 && RecBuf[6] == 0x33)
                Query_report_up();
            else if(RecBuf[5] == 0x30 && RecBuf[6] == 0x34)
                Query_report_down();
            else if(RecBuf[5] == 0x30 && RecBuf[6] == 0x35)
                Query_temp();
            else if(RecBuf[5] == 0x30 && RecBuf[6] == 0x36)
                Query_report_status();
            else if(RecBuf[5] == 0x30 && RecBuf[6] == 0x37)
                Query_all();
            else if(RecBuf[5] == 0x30 && RecBuf[6] == 0x38)
                Query_other();
            else if(RecBuf[5] == 0x35 && RecBuf[6] == 0x38)
                Set_other();

            else if(RecBuf[5] == 0x35 && RecBuf[6] == 0x30)
                Set_addr();
            else if(RecBuf[5] == 0x35 && RecBuf[6] == 0x31)
                Set_range_up();
            else if(RecBuf[5] == 0x35 && RecBuf[6] == 0x32)
                Set_range_down();
            else if(RecBuf[5] == 0x35 && RecBuf[6] == 0x33)
                Set_report_up();
            else if(RecBuf[5] == 0x35 && RecBuf[6] == 0x34)
                Set_report_down();

            for(i = 0; i < sendLen ;i++)
            {
                ES = 0;
                SBUF = sendBuf[i];
                while(!TI);//发送
                TI = 0;
                ES = 1;
            }
        }
        TempBuff = Get_temp();
        Temp[0] = TempBuff/100000 + 0x30;
        Temp[1] = TempBuff/10000%10+ 0x30;
        Temp[2] = TempBuff/1000%10+ 0x30;
        Temp[3] = TempBuff/100%10+ 0x30;
        Temp[5] = TempBuff/10%10+ 0x30;
        Temp[6] = TempBuff%10+ 0x30;
        show(TempBuff);
    }
}
void ser() __interrupt 4
{
    /*int i;*/
    unsigned int j;
    /*unsigned char a,b;*/
    /*unsigned char lrc_value;*/
    /*unsigned char lrc = 0;*/
    unsigned char Buff;
    while(RI) //接收标志
    {
        RI=0;
        Buff = SBUF;
        /*开始符*/
        if(Buff == 0x23)
            rec_flag = 1;
        if(rec_flag)
        {
            RecBuf[lencount] = SBUF;
            lencount++;
            /*结束符@*/
            if(Buff == 0x24)
            {
                j = Address_check();
                if(j>0)
                {
                    send_flag = 1;
                }
                /*if(RecBuf[1]==0&&RecBuf[2]==0&&RecBuf[3]==0&&RecBuf[4]==0)*/
                /*if (RecBuf[1]==Address[0]&&RecBuf[2]==Address[1]&&RecBuf[3]==Address[2]&&RecBuf[4]==Address[3])*/
                /*{*/
                    /*j = 1;*/
                /*}*/
                /*if(j)*/
                /*{*/
                    /*send_flag = 1;*/
                /*}*/
                /*send_flag = 1;*/
                rec_flag=0;
                lencount=0;
            }
        }
    }
}
