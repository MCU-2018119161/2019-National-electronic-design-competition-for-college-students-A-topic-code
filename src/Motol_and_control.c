#include <msp430.h>
#include "Schedule.h"

extern SystemFuture SystemStatus;

/*
 * 电机初始化。电机pwm P1.4(P1.2) P1.5(P1.3) __在PWM初始化中已经初始化好__此处不需要初始化PWM引脚
 * 正反转P1.2(P1.4)右    P1.3(P1.5)左
 */
void motol_init()
{
    P1DIR |= BIT2;                //设置P1.2口为输出模式
    P1DIR |= BIT3;                //设置P1.3口为输出模式

    P1OUT &= ~BIT3;             //初始化 防止反转
    P1OUT |= BIT2;
}

/*
 * 光电对管端口初始化
 */
void Rode_port_init()
{
    int i;
    P8DIR &= ~BIT2;                //设置P8.2口为输入模式
    P3DIR &= ~BIT7;                //设置P3.7口为输入模式
    P2DIR &= ~BIT4;                //设置P2.4口为输入模式
    P2DIR &= ~BIT5;                //设置P3.5口为输入模式

    for(i=1;i<5;i++)
    SystemStatus.RodeStatus[i]=0;       //状态清零
}

/*
 * 小车位置检测，更新小车位置
 */
void RodeDetect()
{
    static int LastCircleStutus=0; //记录上次是否为交接点处标志位
    SystemStatus.RodeStatus[1]=!(!(P8IN & BIT2));
    SystemStatus.RodeStatus[2]=!(P3IN & BIT7);
    SystemStatus.RodeStatus[3]=!(P2IN & BIT4);
    SystemStatus.RodeStatus[4]=!(!(P2IN & BIT5));

    if(SystemStatus.RodeStatus[1]&&!SystemStatus.RodeStatus[2]&&!SystemStatus.RodeStatus[3]&&SystemStatus.RodeStatus[4])
        SystemStatus.Circle=1;      //到达交接点
    else
        SystemStatus.Circle=0;

    if((LastCircleStutus!=SystemStatus.Circle)&&LastCircleStutus)   //上次为交接点处且这次不为交接点处，此时 视为经过一个交接点
        SystemStatus.PassCircleNum++;
    LastCircleStutus=SystemStatus.Circle;

    //模式3 检测圆圈减速一段时间
    if(SystemStatus.Circle)
        SystemStatus.start=1;
}

/*
 * 小车向前走
 */
void Direct_Forward(int Speed_Left,int Speed_Right)
{
    P1OUT &= ~BIT3;
    P1OUT |= BIT2;

    Motol_Right_Speed = 50-Speed_Left;
    Motol_Left_Speed = 50-Speed_Right;
}

/*
 * 小车向前走
 */
void Direct_Backward(int Speed_Left,int Speed_Right)
{
    P1OUT &= ~BIT2;
    P1OUT |= BIT3;

    Motol_Right_Speed = 50-Speed_Left;
    Motol_Left_Speed = 50-Speed_Right;
}

/*
 * 小车向左转
 */
void Direct_Left(int Speed_Left,int Speed_Right)
{
    P1OUT |= BIT3;
    P1OUT |= BIT2;

    Motol_Right_Speed = 50-Speed_Left;
    Motol_Left_Speed = 50-Speed_Right;
}

/*
 * 小车向右转
 */
void Direct_Right(int Speed_Left,int Speed_Right)
{
    P1OUT &= ~BIT2;
    P1OUT &= ~BIT3;

    Motol_Right_Speed = 50-Speed_Left;
    Motol_Left_Speed = 50-Speed_Right;
}

/*
 * 寻迹函数
 */
void FollowLine()       //49最快  1最慢
{
    int L1=!SystemStatus.RodeStatus[1],\
        L2=!SystemStatus.RodeStatus[2],\
        R1=!SystemStatus.RodeStatus[3],\
        R2=!SystemStatus.RodeStatus[4];

    if(L1+L2+R1+R2)  //存在压线情况       PWM越小越快 模式7
    {
        if(L1&&!L2&&!R1&&!R2)   //1000  左转L
            Direct_Forward(3,15);
        if(!L1&&L2&&!R1&&!R2)   //0100  左转S
            Direct_Forward(6,15);
        if(!L1&&!L2&&!R1&&R2)   //0001  右转L
            Direct_Forward(16,4);
        if(!L1&&!L2&&R1&&!R2)   //0010  右转S
            Direct_Forward(16,7);
    }
    else Direct_Forward(14,8);
   // Direct_Forward(40,10);
}

/*
 * 模式三寻迹函数
 */
void FollowLineMode3(int SpeedDecline)       //49最快  1最慢
{
    int L1=!SystemStatus.RodeStatus[1],\
        L2=!SystemStatus.RodeStatus[2],\
        R1=!SystemStatus.RodeStatus[3],\
        R2=!SystemStatus.RodeStatus[4];

    if(L1+L2+R1+R2)  //存在压线情况       PWM越小越快 模式7
    {
        if(L1&&!L2&&!R1&&!R2)   //1000  左转L
            Direct_Forward(5,13);
        if(!L1&&L2&&!R1&&!R2)   //0100  左转S
            Direct_Forward(5,12);
        if(!L1&&!L2&&!R1&&R2)   //0001  右转L
            Direct_Forward(13,5);
        if(!L1&&!L2&&R1&&!R2)   //0010  右转S
            Direct_Forward(12,5);
    }
    else Direct_Forward(10,7);
   // Direct_Forward(40,10);
}

/*
 * 功能一函数
 */
void Mode_1(int ToMode2)
{
    if(SystemStatus.current<=20&&SystemStatus.end==0)
    {
        SystemStatus.start=1;
    }
    if(SystemStatus.end)
        {
       // FollowLineMode3(SystemStatus.SpeedDlcine);     //减速5 检测电流是否能充电
            //Direct_Forward(49,49);
            FollowLine();
            //SendSring("start!\n");
        }

    //if((SystemStatus.PassCircleNum)&&(SystemStatus.Circle)&&(SystemStatus.time_s>=10&&SystemStatus.current<=500))//到达第二个交接点，停止

    //Send_digital(10*SystemStatus.PassCircleNum+SystemStatus.Circle);
//mode1 和 mode2 区别在于是否停车,用于区分
    if(!ToMode2)   //是否转换成Mode2?  0否  1是
   // if((SystemStatus.PassCircleNum)&&(SystemStatus.Circle))
        if((SystemStatus.Circle))
    {
        Direct_Forward(0,0);
        __disable_interrupt();
        //SystemStatus.start=0;
        while(1);       //结束
    }
}

/*
 * 功能2函数
 * 由于和mode1差不多，于是暂停使用
 */
void Mode_2()
{
    FollowLine();
}

/*
 * 功能3函数
 * 发挥部分
 */
void Mode_3()
{
    static Full_Time_Start=0;       //时间计时 寻迹开始标志位
    if(SystemStatus.time_m>=16&&!Full_Time_Start)
    {
        Full_Time_Start=1;
        P1OUT |= BIT6;
    }
    else if(!Full_Time_Start)    P1OUT &= ~BIT6;


//时间到，开始寻迹
    if(Full_Time_Start&&!SystemStatus.begin){
        FollowLine();
    }

    if(SystemStatus.begin)
    {
        FollowLineMode3(SystemStatus.SpeedDlcine);     //减速5 检测电流是否能充电
    }

    /*if(SystemStatus.end)        //在时间到之前经过充电线圈时  停车
        Direct_Forward(0,0);
*/

}

