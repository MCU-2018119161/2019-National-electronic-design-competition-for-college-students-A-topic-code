#include <msp430.h>
#include "Schedule.h"

extern SystemFuture SystemStatus;

/*
 * ϵͳ��ʼ��
 */
void System_Init()
{
    ADC_init(); //ADC��ʼ��  ����ѹ ����
    PWM_init();     //PWM��������ʼ��
    Timer_init();   //��ʱ����ʼ��
    motol_init();   //����������ų�ʼ��
    Rode_port_init();   //���Թܳ�ʼ��  ����Ϊ����
    Encode_port_init(); //���뿪�س�ʼ��  �����趨ģʽ
    System_Mode_confirm();//ģʽ�ж�
    Uart0_init();
    __enable_interrupt();

//ϵͳ������ʼ��
    SystemStatus.TimerTime=0.050;  //50ms
    SystemStatus.PassCircleNum=0;
    SystemStatus.time_s=0;
    SystemStatus.time_m=0;
    SystemStatus.time_h=0;
    SystemStatus.current=0;
    SystemStatus.Voltage=0;
    SystemStatus.PassCircleNum=0;
    SystemStatus.Circle=0;
    SystemStatus.start=0;
    SystemStatus.end=0;
    SystemStatus.Mode_1_Flag=0;
    SystemStatus.begin=0;
    SystemStatus.off=0;
    SystemStatus.TimerCount=0;
    SystemStatus.TimerCountRestart=0;//ͣ�����ʱ����
    SystemStatus.SpeedDlcine=3; //ģʽ3  ����ʱ����������ٶ�
}

/*
 * ���뿪�ؽӿڳ�ʼ��
 */
void Encode_port_init()
{
    P8DIR &= ~BIT1;                //����P8.1��Ϊ����ģʽ
    P2DIR &= ~BIT2;                //����P2.2��Ϊ����ģʽ
    P2DIR &= ~BIT0;                //����P2.8��Ϊ����ģʽ

    P8OUT |= BIT1;
    P2OUT |= BIT2;
    P2OUT |= BIT0;

    P8REN |= BIT1;
    P2REN |= BIT2;
    P2REN |= BIT0;

 /**********/
    P2DIR |= BIT7;  // ���ָʾ��

    P1DIR |= BIT6;  //�̵���ͨ�ϵ�˿�
    P1OUT &= ~BIT6;
    P1DIR |= BIT0;
}

/*
 * ϵͳģʽ�ж�
 */
void System_Mode_confirm()
{
   int Mode=0;
   if(!(P2IN & BIT2))
       Mode+=1;
   if(!(P2IN & BIT0))
       Mode+=2;
   //Mode=(~(P2IN & BIT2))+2*(~(P2IN & BIT0));

   SystemStatus.Mode=Mode;
}

/*
 * ϵͳʱ�����
 */
void SysTimeFlash()
{
    //SystemStatus.time_s = 50;
   // SystemStatus.time_s += SystemStatus.TimerTime;    //ÿ�ν��жϼ��ϼ�ʱ�����ʱ�䣬ʵ�ָ���
    SystemStatus.time_s += 0.1;    //ÿ�ν��жϼ��ϼ�ʱ�����ʱ�䣬ʵ�ָ���
    if(SystemStatus.time_s>=60) //����
        {
            SystemStatus.time_m++;
            SystemStatus.time_s=0;


//ģʽ����ʱ���ٴ���
            Send_digital(SystemStatus.time_s);
           // if(SystemStatus.begin&&!SystemStatus.end) SystemStatus.TimerCount++;//��ʼ����
            if(SystemStatus.begin) SystemStatus.TimerCount++;//��ʼ����
           /* if(SystemStatus.current>=100)  SystemStatus.end=1;//��⵽���
            else SystemStatus.end=0;*/
            //if(SystemStatus.TimerCount>=2&&!SystemStatus.end) {
            if(SystemStatus.TimerCount>=2) {
                SystemStatus.begin=0;
            }
           // if(SystemStatus.end)  //ͣ�����

//
        }
    if(SystemStatus.time_m>=60)//����
        {
               SystemStatus.time_h++;
               SystemStatus.time_m=0;
        }


//ģʽ1�̵�����ʱ����
    if(SystemStatus.time_s>=30){
        if(SystemStatus.start&&!SystemStatus.end)      //��ʼģʽ1 �Ĵ����ϵ��ʱ
                    {
                        P1OUT &= ~BIT6;
                        //P1OUT &= ~BIT0;
                        SystemStatus.Mode_1_Flag++;
                    }

                    if(SystemStatus.Mode_1_Flag>=1) //����ͣ��ʱ��
                    {
                        SystemStatus.end=1; //ͣ������
                        P1OUT |= BIT6;
                       // P1OUT |= BIT0;
                    }
    }

//ģʽ������  ��ʱ����

}

/*
 * ���ƴ�����
 */
void Schedule()
{
    switch (SystemStatus.Mode)      //�ж�ģʽ,ѡ����
    {
        case 0:             //ģʽһ  ����Ϊ�㣬��������һ���ڵ�ͣ��
            Mode_1(0);          //��ת����Mode2
                break;

        case 1:             //ģʽ��
            Mode_1(1);          //ת����Mode2
                 break;

        case 2:             //ģʽ��
            Mode_3();
                 break;

        case 3:              //ģʽ��

                 break;
        default: break;     //�����˳�
    }
}
