#include <msp430.h>
#include "Schedule.h"

extern SystemFuture SystemStatus;

/*
 * 系统初始化
 */
void System_Init()
{
    ADC_init(); //ADC初始化  检测电压 电流
    PWM_init();     //PWM脉宽调解初始化
    Timer_init();   //定时器初始化
    motol_init();   //电机控制引脚初始化
    Rode_port_init();   //光电对管初始化  设置为输入
    Encode_port_init(); //拨码开关初始化  用于设定模式
    System_Mode_confirm();//模式判断
    Uart0_init();
    __enable_interrupt();

//系统参数初始化
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
    SystemStatus.TimerCountRestart=0;//停车后计时启动
    SystemStatus.SpeedDlcine=3; //模式3  减速时间内需减的速度
}

/*
 * 拨码开关接口初始化
 */
void Encode_port_init()
{
    P8DIR &= ~BIT1;                //设置P8.1口为输入模式
    P2DIR &= ~BIT2;                //设置P2.2口为输入模式
    P2DIR &= ~BIT0;                //设置P2.8口为输入模式

    P8OUT |= BIT1;
    P2OUT |= BIT2;
    P2OUT |= BIT0;

    P8REN |= BIT1;
    P2REN |= BIT2;
    P2REN |= BIT0;

 /**********/
    P2DIR |= BIT7;  // 充电指示灯

    P1DIR |= BIT6;  //继电器通断电端口
    P1OUT &= ~BIT6;
    P1DIR |= BIT0;
}

/*
 * 系统模式判断
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
 * 系统时间更新
 */
void SysTimeFlash()
{
    //SystemStatus.time_s = 50;
   // SystemStatus.time_s += SystemStatus.TimerTime;    //每次进中断加上计时器溢出时间，实现更新
    SystemStatus.time_s += 0.1;    //每次进中断加上计时器溢出时间，实现更新
    if(SystemStatus.time_s>=60) //秒满
        {
            SystemStatus.time_m++;
            SystemStatus.time_s=0;


//模式三定时减速处理
            Send_digital(SystemStatus.time_s);
           // if(SystemStatus.begin&&!SystemStatus.end) SystemStatus.TimerCount++;//开始减速
            if(SystemStatus.begin) SystemStatus.TimerCount++;//开始减速
           /* if(SystemStatus.current>=100)  SystemStatus.end=1;//检测到充电
            else SystemStatus.end=0;*/
            //if(SystemStatus.TimerCount>=2&&!SystemStatus.end) {
            if(SystemStatus.TimerCount>=2) {
                SystemStatus.begin=0;
            }
           // if(SystemStatus.end)  //停车充电

//
        }
    if(SystemStatus.time_m>=60)//分满
        {
               SystemStatus.time_h++;
               SystemStatus.time_m=0;
        }


//模式1继电器定时处理
    if(SystemStatus.time_s>=30){
        if(SystemStatus.start&&!SystemStatus.end)      //开始模式1 寄存器断电计时
                    {
                        P1OUT &= ~BIT6;
                        //P1OUT &= ~BIT0;
                        SystemStatus.Mode_1_Flag++;
                    }

                    if(SystemStatus.Mode_1_Flag>=1) //给定停车时间
                    {
                        SystemStatus.end=1; //停车结束
                        P1OUT |= BIT6;
                       // P1OUT |= BIT0;
                    }
    }

//模式三处理  定时减速

}

/*
 * 控制处理函数
 */
void Schedule()
{
    switch (SystemStatus.Mode)      //判断模式,选择功能
    {
        case 0:             //模式一  电流为零，启动，第一个节点停车
            Mode_1(0);          //不转换成Mode2
                break;

        case 1:             //模式二
            Mode_1(1);          //转换成Mode2
                 break;

        case 2:             //模式三
            Mode_3();
                 break;

        case 3:              //模式四

                 break;
        default: break;     //否则退出
    }
}
