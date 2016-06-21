/**************************************************************************
 Copyright © 2016 jianglin
 File Name: mine.c
 Author: jianglin
 Email: xiyang0807@gmail.com
 Created: 2016-06-20 21:11:18 (CST)
 Last Update:星期二 2016-6-21 13:58:37 (CST)
           By:
 Description:
**************************************************************************/
#include<stdio.h>
#include<string.h>
#include<8052.h>
#define uchar unsigned char
#define uint unsigned int
__sbit __at 0xB4 lcden;
__sbit __at 0xB5 rs;
__sbit __at 0xB6 rw;
__sbit __at 0xA6 dula;
__sbit __at 0xA7 wela;

/* sbit lcden=P3^4; */
/* sbit rs=P3^5; */
/* sbit rw=P3^6;         */
/* sbit dula=P2^6; */
/* sbit wela=P2^7; */

uchar __idata sendBuf[32]=" HELLO HONMAPLE";
uchar __idata RecBuf[32] = " This is My World";
unsigned char sendLen=0;
uchar send_flag;
uchar rec_flag;
uchar lencount = 0;
uchar rec_len = 0;
uchar table1[]=" HELLO HONMAPLE ";
uchar table2[]="This is my world";
void delay(uint x)
{
        uint a,b;
        for(a=x;a>0;a--)
                for(b=10;b>0;b--);
}

void delay1(uint x)
{
        uint a,b;
        for(a=x;a>0;a--)
                for(b=100;b>0;b--);
}

void write_com(uchar com)
{
        P0=com;
        rs=0;
        lcden=0;
        delay(10);
        lcden=1;
        delay(10);
        lcden=0;

}

void write_date(uchar date)
{
        P0=date;
        rs=1;
        lcden=0;
        delay(10);
        lcden=1;
        delay(10);
        lcden=0;
}


void lcd_init()
{
        rw=0;
        dula=0;
        wela=0;
        write_com(0x38);
        delay(20);
        write_com(0x0c);
        delay(20);
        write_com(0x06);
        delay(20);
        write_com(0x01);
        delay(20);
}
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

void (*reset)(void)=0x0000;

void main()
{

        int i;
        unsigned char student[] = "honmaple";
        init();
        lcd_init();

        while(1)
        {
                sendLen = strlen(RecBuf);
                rec_len = 0;
                if(send_flag==1) //发送数据标志位
                {
                        send_flag=0;
                        for(i = 0; i < sendLen ;i++)
                        {
                                ES = 0;
                                SBUF = sendBuf[i];
                                while(!TI);//发送
                                TI = 0;
                                ES = 1;
                        }
                }
                for (i =1;i<sendLen;i++)
                {
                        if (RecBuf[i] == 0x40){
                                break;
                        }
                        rec_len += 1;
                }
                write_com(0x80+17);
                delay(20);
                for(i=1;i<=rec_len;i++)
                {
                        write_date(RecBuf[i]);
                        delay(20);
                }
                for(i=0;i<rec_len;i++)
                {
                        write_com(0x18); //����
                        delay1(150);
                }
        }
}


void ser() __interrupt 4
{
        unsigned char Buff;
        while(RI) //接收标志
        {

                RI=0;
                Buff = SBUF;
                if(Buff == 0x23)
                {
                        rec_flag = 1;
                }
                if(rec_flag == 1)
                {
                        RecBuf[lencount] = Buff;
                        lencount++;
                        if(Buff == 0x40) {
                                send_flag = 1;
                                rec_flag=0;
                                lencount=0;
                                lcd_init();
                        }
                }
        }
}
