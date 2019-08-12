#include <msp430.h>
#include "Schedule.h"

SystemFuture SystemStatus;

/*
 * Main function
 */
int main(void)
{
    System_Init();  //系统初始化
    while(1);
}

/************************定时器中断函数********************************/
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A (void)

{
    SysTimeFlash();     //系统时间更新
    CurrentDetect();    //电流，电压检测
    RodeDetect();       //小车位置检测
    Schedule();         //50ms 执行一次控制周期

    //Send_digital(SystemStatus.current);
}

/************************串口中断函数********************************/
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
    switch(__even_in_range(UCA0IV,4))
    {
        case 0:break; // Vector 0 - no interrupt
        case 1:break;
        case 2: // Vector 2 - RXIFG
            while (!(UCA0IFG&UCTXIFG)); // USCI_A0 TX buffer ready?
            UCA0TXBUF = UCA0RXBUF; // TX -> RXed character
            break;
        case 4:break; // Vector 4 - TXIFG
        default: break;
    }
}
