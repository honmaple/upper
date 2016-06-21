/* #include <reg52.h> */
#include<8052.h>
#define uchar unsigned char
#define uint  unsigned int 
__sbit __at 0xB5 lcdrs ;	//Һ�����Ŷ���
__sbit __at 0xB6 led_cs;
__sbit __at 0xB4 lcden ;
__sbit __at 0xA7 dx;//��������Ŷ���
__sbit __at 0xA6 wx;
__sbit __at 0xB0 key1;
__sbit __at 0xB1 key2;
__sbit __at 0xB2 key3;
__sbit __at 0xB3 keyE;

/* sbit lcdrs  = P3^5;	//Һ�����Ŷ��� */
/* sbit led_cs = P3^6; */
/* sbit lcden  = P3^4; */
/* sbit dx = P2^7;//��������Ŷ��� */
/* sbit wx = P2^6; */
/* sbit key1 = P3^0; */
/* sbit key2 = P3^1; */
/* sbit key3 = P3^2; */
/* sbit keyE = P3^3; */
uchar num,key1num,count;
uint sec,min,hour;
uint year,month,day;
uchar __code cg_1602[]={ 0x08,0x0f,0x12,0x0f,0x0a,0x1f,0x02,0x02,
                         0x0f,0x09,0x0f,0x09,0x0f,0x09,0x11,0x00,
                         0x0f,0x09,0x09,0x0f,0x09,0x09,0x0f,0x00};//"������"
/*��ʱ����*/

void delay(uint xms)
{
    uint i,j; 
    for(i=xms;i>0;i--)
        for(j=114;j>0;j--);
}
/*1602Һ��*/
void write_com(uchar com)//д����
{
    lcdrs=0;
    lcden=0;
    P0=com;
    delay(5);
    lcden=1;
    delay(5);
    lcden=0;
}
void write_data(uchar date)//д����
{
    lcdrs=1;
    lcden=0;
    P0=date;
    delay(5);
    lcden=1;
    delay(5);
    lcden=0;
}
void write_by_xy(uchar x,uchar y)//���궨λ
{
    uchar address;
    if(y==0) {
        address=0x80+x;//y=0,д��һ��
    } else {
        address=0xc0+x;//y=1,д�ڶ���
    }
    write_com(address); 
}
/*void write_char(uchar x,uchar y,uchar date)//���궨λ��ʾ
  {
  write_by_xy(x,y);//�ȶ�λ 
  write_data(date);//��д��
  }*/
void write_string(uchar x,uchar y,uchar *s)//д�ַ��������ָ�뻹���󶮰���
{
    write_by_xy(x,y);//�趨��ʾλ�� 
    while(*s)//write character
    {
        P0=*s;
        write_data(*s); 
        s++;
    }
}

void write_year(uchar add,uint date)//дʱ���뺯��
{
    uchar qian,bai,shi,ge;
    if (date > 999){
        qian = date/1000;
        bai = date/100%10;
        shi = date/10%100;
        ge = date - 1000*qian - 100*bai - 10*shi;
        write_com(0x80+add);//������ʾλ��
        write_data(0x30+qian);//��ȥҺ����ʾǧλ
        write_data(0x30+bai);//��ȥҺ����ʾ��λ
        write_data(0x30+shi);//��ȥҺ����ʾʮλ
        write_data(0x30+ge); //��ȥҺ����ʾ��λ
    }else{
        shi=date/10;//�ֽ�һ��2λ����ʮλ�͸�λ
        ge=date%10;
        /* write_com(0x80+0x40+add);//������ʾλ�� */
        write_com(0x80+add);//������ʾλ��
        write_data(0x30+shi);//��ȥҺ����ʾʮλ
        write_data(0x30+ge); //��ȥҺ����ʾ��λ
    }
}

void write_sfm(uchar add,uchar date)//дʱ���뺯��
{
    uchar hour,ge;
    hour=date/10;//�ֽ�һ��2λ����ʮλ�͸�λ
    ge=date%10;
    /* write_com(0x80+0x40+add);//������ʾλ�� */
    write_com(0xc0+add);//������ʾλ��
    write_data(0x30+hour);//��ȥҺ����ʾʮλ
    write_data(0x30+ge); //��ȥҺ����ʾ��λ
}
void write_cg()//д��̶����ֺͷ���
{
    /* write_com(0x40); //�趨CGRAM��ַ  */
    /* for(num=0;num<24;num++) //���Զ���������д��CGRAM��  */
    /* {  */
    /* 	write_data(cg_1602[num]);  */
    /* } */
    write_com(0x84+3);//�� 
    { 
        /* write_data(0);  */
        write_data(0x2d); 
    } 
    write_com(0x87+3);//�� 
    { 
        write_data(0x2d); 
        /* write_data(1);  */
    }
    /* write_com(0x8a);//��  */
    /* {  */
    /*     write_data(0x2d);  */
    /*     /\* write_data(2);  *\/ */
    /* } */
    write_string(2,1,":");
    write_string(5,1,":");
    write_string(8,1,"JiangLin");
}
void init_1602()//��ʼ��
{
    dx=0;wx=0;led_cs=0;
    hour=12;min=30;sec=30;
    year=2016;month=6;day=18;
    key1num=0;count=0;
    write_com(0x38);
    write_com(0x0c);
    write_com(0x06);
    write_com(0x01);
    /* write_string(3,0,year); */
    /* write_string(8,0,month); */
    /* write_string(11,0,day); */
    write_year(3,year);
    write_year(8,month);
    write_year(11,day);
    write_sfm(0,hour);
    write_sfm(3,min);
    write_sfm(6,sec);
    write_cg();
    TMOD=0x01;//���ö�ʱ��0����ģʽ1
    TH0=(65536-50000)/256;//��ʱ��װ��ֵ
    TL0=(65536-50000)%256;
    EA=1; //�����ж�
    ET0=1; //����ʱ��0�ж�
    TR0=1; //������ʱ��0
}
/*����ɨ�躯��*/
void keyscan()//����ɨ�躯��
{
    /* keyE =0; */
    if(keyE == 0){
        delay(5);
        if(keyE == 0){
            key1num=0;//��¼����������
            write_com(0x0c);//ȡ�������˸
            TR0=1; //������ʱ��ʹʱ�ӿ�ʼ��
        }
    }
    if(key1==0) 
    {
        delay(5);
        if(key1==0)//ȷ�Ϲ��ܼ�������
        { 	
            key1num++;//���ܼ����´�����¼
            while(!key1);//�ͷ�ȷ��
            if(key1num==1)//��һ�α�����ʱ
            {
                TR0=0; //�رն�ʱ��
                write_com(0x80+0x40+7);//��궨λ����λ��
                write_com(0x0f); //��꿪ʼ��˸
            }
            if(key1num==2)//�ڶ��ΰ��¹����˸��λ������λ��
            {
                write_com(0x80+0x40+4);
            }
            if(key1num==3)//�����ΰ��¹����˸��λ��Сʱλ��
            {
                write_com(0x80+0x40+1);
            }
            if(key1num==4)//�����ΰ��¹����˸��λ��Сʱλ��
            {
                write_com(0x80+12);
            }
            if(key1num==5)//�����ΰ��¹����˸��λ��Сʱλ��
            {
                write_com(0x80+9);
            }
            if(key1num==6)//�����ΰ��¹����˸��λ��Сʱλ��
            {
                write_com(0x80+6);
            }
            if(key1num==7)//���Ĵΰ���
            {
                key1num=0;//��¼����������
                write_com(0x0c);//ȡ�������˸
                TR0=1; //������ʱ��ʹʱ�ӿ�ʼ��
            } 
        }
    }
    if(key1num!=0)//ֻ�й��ܼ������º����Ӻͼ�С������Ч
    {
        if(key2==0)
        {
            delay(5);
            if(key2==0)//���Ӽ�ȷ�ϱ�����
            {
                while(!key2);//�����ͷ�
                if(key1num==1)//�����ܼ���һ�ΰ���
                {
                    sec++; //��������1
                    if(sec==60)//����60������
                        sec=0;
                    write_sfm(6,sec);//ÿ����һ����Һ����ʾһ��
                    write_com(0x80+0x40+7);//��ʾλ�����»ص����ڴ�
                }
                if(key1num==2)//�����ܼ��ڶ��ΰ���
                {
                    min++;//��������Ӽ�1
                    if(min==60)//����60������
                        min=0;
                    write_sfm(3,min);//ÿ����һ����Һ����ʾһ��
                    write_com(0x80+0x40+4);//��ʾλ�����»ص����ڴ�
                }
                if(key1num==3)//�����ܼ������ΰ���
                {
                    hour++;//�����Сʱ��1
                    if(hour==24)//����24������
                        hour=0;
                    write_sfm(0,hour);;//ÿ����һ����Һ����ʾһ��
                    write_com(0x80+0x40+1);//��ʾλ�����»ص����ڴ�
                }
                if(key1num==4)//�����ܼ������ΰ���
                {
                    day++;//�����Сʱ��1
                    if(day==31)//����30������
                        day=1;
                    write_year(11,day);;//ÿ����һ����Һ����ʾһ��
                    write_com(0x80+12);//��ʾλ�����»ص����ڴ�
                }
                if(key1num==5)//�����ܼ������ΰ���
                {
                    month++;//�����Сʱ��1
                    if(month==13)//����24������
                        month=1;
                    write_year(8,month);;//ÿ����һ����Һ����ʾһ��
                    write_com(0x80+9);//��ʾλ�����»ص����ڴ�
                }
                if(key1num==6)//�����ܼ������ΰ���
                {
                    year++;//�����Сʱ��1
                    write_year(3,year);;//ÿ����һ����Һ����ʾһ��
                    write_com(0x80+6);//��ʾλ�����»ص����ڴ�
                }
            }
        }
        if(key3==0)
        {
            delay(5);
            if(key3==0)//ȷ�ϼ�С��������
            {
                while(!key3);//�����ͷ�
                if(key1num==1)//�����ܼ���һ�ΰ���
                {
                    sec--;//��������1
                    if(sec==-1)//����������������������Ϊ59
                        sec=59;
                    write_sfm(6,sec);//ÿ����һ����Һ����ʾһ��
                    write_com(0x80+0x40+7);//��ʾλ�����»ص����ڴ�
                }
                if(key1num==2)//�����ܼ��ڶ��ΰ���
                {
                    min--;//��������Ӽ�1
                    if(min==-1)//����������������������Ϊ59
                        min=59;
                    write_sfm(3,min);//ÿ����һ����Һ����ʾһ��
                    write_com(0x80+0x40+4);//��ʾλ�����»ص����ڴ�
                }
                if(key1num==3)//�����ܼ��ڶ��ΰ���
                {
                    hour--;//�����Сʱ��1
                    if(hour==-1)//����������������������Ϊ23
                        hour=23;
                    write_sfm(0,hour);//ÿ����һ����Һ����ʾһ��
                    write_com(0x80+0x40+1);//��ʾλ�����»ص����ڴ�
                }
                if(key1num==4)//�����ܼ������ΰ���
                {
                    day--;//�����Сʱ��1
                    if(day==0)//����30������
                        day=30;
                    write_year(11,day);;//ÿ����һ����Һ����ʾһ��
                    write_com(0x80+12);//��ʾλ�����»ص����ڴ�
                }
                if(key1num==5)//�����ܼ������ΰ���
                {
                    month--;//�����Сʱ��1
                    if(month==0)//����24������
                        month=12;
                    write_year(8,month);;//ÿ����һ����Һ����ʾһ��
                    write_com(0x80+9);//��ʾλ�����»ص����ڴ�
                }
                if(key1num==6)//�����ܼ������ΰ���
                {
                    year--;//�����Сʱ��1
                    if(year==-1)//����30������
                        year=2016;
                    write_year(3,year);;//ÿ����һ����Һ����ʾһ��
                    write_com(0x80+6);//��ʾλ�����»ص����ڴ�
                }
            }
        }
    }
}
/*������*/
void main()
{
    dx=0;wx=0;led_cs=0;
    init_1602();
    while(1)
    {
        keyscan();
    } 
}
/*�жϺ���*/
void timer0() __interrupt 1//��ʱ��0�жϷ������
{
    TH0=(65536-50000)/256;//�ٴ�װ��ʱ����ֵ
    TL0=(65536-50000)%256;
    count++; //�жϴ����ۼ�
    if(count==20) //20��50����Ϊ1��
    {
        count=0;
        sec++;
        if(sec==60)//��ӵ�60���λ����
        {
            sec=0;//ͬʱ��������
            min++;
            if(min==60)//���Ӽӵ�60���λСʱ
            {
                min=0;//ͬʱ����������
                hour++;
                if(hour==24)//Сʱ�ӵ�24��Сʱ����
                {
                    hour=0;
                    day++;
                    if(day == 31){
                        day = 1;
                        month++;
                        if(month==13){
                            month = 1;
                            year++;
                            write_year(3,year);//�����仯������д��
                        }
                        write_year(8,month);//�����仯������д��
                    }
                    write_year(11,day);//�����仯������д��
                }
                write_sfm(0,hour);//Сʱ���仯������д��
            }
            write_sfm(3,min);//�������仯������д��
        }
        write_sfm(6,sec);//�����仯������д�� 
    } 
}
