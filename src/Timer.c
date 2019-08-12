#include <msp430.h>
#include "Timer.h"
#include "Schedule.h"

/*
 * 硬件PWM初始化，使能PWM输出端口 P1.4 and P1.5
 */
void PWM_init()
{
    P1SEL|=BIT5;P1DIR|=BIT5;  //TA0.4 PWM输出 4号 P0.4
    P1SEL|=BIT4;P1DIR|=BIT4;  //TA0.3 PWM输出 5号 P0.3

    TA0CCTL4 = OUTMOD_7;
    TA0CCTL3 = OUTMOD_7;

    TA0CCR0=50;     //1MHZ频率   给定初值为50 后对应的比较频率为20KHZ 用于驱动电机

    TA0CCR3=48;     //通道3 比较数值
    TA0CCR4=48;         //通道4比较数值

    TA0CTL = TASSEL_2 + MC_1 + TACLR;
}

/*
 * 中断初始化，开中断
 */
void Timer_init()
{
    WDTCTL = WDTPW | WDTHOLD;                // 关闭看门狗
    TA0CTL |= MC_1 + TASSEL_2 + TACLR;      //时钟为SMCLK,比较模式，开始时清零计数器
    TA0CCTL0 = CCIE;                        //比较器中断使能
    //TA0CCR0  = 100;                         //比较值设为50000，相当于50ms的时间间隔s
}
