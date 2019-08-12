#include <msp430.h>
#include "Schedule.h"

extern SystemFuture SystemStatus;

/*
 * �����ʼ�������pwm P1.4(P1.2) P1.5(P1.3) __��PWM��ʼ�����Ѿ���ʼ����__�˴�����Ҫ��ʼ��PWM����
 * ����תP1.2(P1.4)��    P1.3(P1.5)��
 */
void motol_init()
{
    P1DIR |= BIT2;                //����P1.2��Ϊ���ģʽ
    P1DIR |= BIT3;                //����P1.3��Ϊ���ģʽ

    P1OUT &= ~BIT3;             //��ʼ�� ��ֹ��ת
    P1OUT |= BIT2;
}

/*
 * ���Թܶ˿ڳ�ʼ��
 */
void Rode_port_init()
{
    int i;
    P8DIR &= ~BIT2;                //����P8.2��Ϊ����ģʽ
    P3DIR &= ~BIT7;                //����P3.7��Ϊ����ģʽ
    P2DIR &= ~BIT4;                //����P2.4��Ϊ����ģʽ
    P2DIR &= ~BIT5;                //����P3.5��Ϊ����ģʽ

    for(i=1;i<5;i++)
    SystemStatus.RodeStatus[i]=0;       //״̬����
}

/*
 * С��λ�ü�⣬����С��λ��
 */
void RodeDetect()
{
    static int LastCircleStutus=0; //��¼�ϴ��Ƿ�Ϊ���ӵ㴦��־λ
    SystemStatus.RodeStatus[1]=!(!(P8IN & BIT2));
    SystemStatus.RodeStatus[2]=!(P3IN & BIT7);
    SystemStatus.RodeStatus[3]=!(P2IN & BIT4);
    SystemStatus.RodeStatus[4]=!(!(P2IN & BIT5));

    if(SystemStatus.RodeStatus[1]&&!SystemStatus.RodeStatus[2]&&!SystemStatus.RodeStatus[3]&&SystemStatus.RodeStatus[4])
        SystemStatus.Circle=1;      //���ｻ�ӵ�
    else
        SystemStatus.Circle=0;

    if((LastCircleStutus!=SystemStatus.Circle)&&LastCircleStutus)   //�ϴ�Ϊ���ӵ㴦����β�Ϊ���ӵ㴦����ʱ ��Ϊ����һ�����ӵ�
        SystemStatus.PassCircleNum++;
    LastCircleStutus=SystemStatus.Circle;

    //ģʽ3 ���ԲȦ����һ��ʱ��
    if(SystemStatus.Circle)
        SystemStatus.start=1;
}

/*
 * С����ǰ��
 */
void Direct_Forward(int Speed_Left,int Speed_Right)
{
    P1OUT &= ~BIT3;
    P1OUT |= BIT2;

    Motol_Right_Speed = 50-Speed_Left;
    Motol_Left_Speed = 50-Speed_Right;
}

/*
 * С����ǰ��
 */
void Direct_Backward(int Speed_Left,int Speed_Right)
{
    P1OUT &= ~BIT2;
    P1OUT |= BIT3;

    Motol_Right_Speed = 50-Speed_Left;
    Motol_Left_Speed = 50-Speed_Right;
}

/*
 * С������ת
 */
void Direct_Left(int Speed_Left,int Speed_Right)
{
    P1OUT |= BIT3;
    P1OUT |= BIT2;

    Motol_Right_Speed = 50-Speed_Left;
    Motol_Left_Speed = 50-Speed_Right;
}

/*
 * С������ת
 */
void Direct_Right(int Speed_Left,int Speed_Right)
{
    P1OUT &= ~BIT2;
    P1OUT &= ~BIT3;

    Motol_Right_Speed = 50-Speed_Left;
    Motol_Left_Speed = 50-Speed_Right;
}

/*
 * Ѱ������
 */
void FollowLine()       //49���  1����
{
    int L1=!SystemStatus.RodeStatus[1],\
        L2=!SystemStatus.RodeStatus[2],\
        R1=!SystemStatus.RodeStatus[3],\
        R2=!SystemStatus.RodeStatus[4];

    if(L1+L2+R1+R2)  //����ѹ�����       PWMԽСԽ�� ģʽ7
    {
        if(L1&&!L2&&!R1&&!R2)   //1000  ��תL
            Direct_Forward(3,15);
        if(!L1&&L2&&!R1&&!R2)   //0100  ��תS
            Direct_Forward(6,15);
        if(!L1&&!L2&&!R1&&R2)   //0001  ��תL
            Direct_Forward(16,4);
        if(!L1&&!L2&&R1&&!R2)   //0010  ��תS
            Direct_Forward(16,7);
    }
    else Direct_Forward(14,8);
   // Direct_Forward(40,10);
}

/*
 * ģʽ��Ѱ������
 */
void FollowLineMode3(int SpeedDecline)       //49���  1����
{
    int L1=!SystemStatus.RodeStatus[1],\
        L2=!SystemStatus.RodeStatus[2],\
        R1=!SystemStatus.RodeStatus[3],\
        R2=!SystemStatus.RodeStatus[4];

    if(L1+L2+R1+R2)  //����ѹ�����       PWMԽСԽ�� ģʽ7
    {
        if(L1&&!L2&&!R1&&!R2)   //1000  ��תL
            Direct_Forward(5,13);
        if(!L1&&L2&&!R1&&!R2)   //0100  ��תS
            Direct_Forward(5,12);
        if(!L1&&!L2&&!R1&&R2)   //0001  ��תL
            Direct_Forward(13,5);
        if(!L1&&!L2&&R1&&!R2)   //0010  ��תS
            Direct_Forward(12,5);
    }
    else Direct_Forward(10,7);
   // Direct_Forward(40,10);
}

/*
 * ����һ����
 */
void Mode_1(int ToMode2)
{
    if(SystemStatus.current<=20&&SystemStatus.end==0)
    {
        SystemStatus.start=1;
    }
    if(SystemStatus.end)
        {
       // FollowLineMode3(SystemStatus.SpeedDlcine);     //����5 �������Ƿ��ܳ��
            //Direct_Forward(49,49);
            FollowLine();
            //SendSring("start!\n");
        }

    //if((SystemStatus.PassCircleNum)&&(SystemStatus.Circle)&&(SystemStatus.time_s>=10&&SystemStatus.current<=500))//����ڶ������ӵ㣬ֹͣ

    //Send_digital(10*SystemStatus.PassCircleNum+SystemStatus.Circle);
//mode1 �� mode2 ���������Ƿ�ͣ��,��������
    if(!ToMode2)   //�Ƿ�ת����Mode2?  0��  1��
   // if((SystemStatus.PassCircleNum)&&(SystemStatus.Circle))
        if((SystemStatus.Circle))
    {
        Direct_Forward(0,0);
        __disable_interrupt();
        //SystemStatus.start=0;
        while(1);       //����
    }
}

/*
 * ����2����
 * ���ں�mode1��࣬������ͣʹ��
 */
void Mode_2()
{
    FollowLine();
}

/*
 * ����3����
 * ���Ӳ���
 */
void Mode_3()
{
    static Full_Time_Start=0;       //ʱ���ʱ Ѱ����ʼ��־λ
    if(SystemStatus.time_m>=16&&!Full_Time_Start)
    {
        Full_Time_Start=1;
        P1OUT |= BIT6;
    }
    else if(!Full_Time_Start)    P1OUT &= ~BIT6;


//ʱ�䵽����ʼѰ��
    if(Full_Time_Start&&!SystemStatus.begin){
        FollowLine();
    }

    if(SystemStatus.begin)
    {
        FollowLineMode3(SystemStatus.SpeedDlcine);     //����5 �������Ƿ��ܳ��
    }

    /*if(SystemStatus.end)        //��ʱ�䵽֮ǰ���������Ȧʱ  ͣ��
        Direct_Forward(0,0);
*/

}

