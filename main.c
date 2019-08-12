#include <msp430.h>
#include "Schedule.h"

SystemFuture SystemStatus;

/*
 * Main function
 */
int main(void)
{
    System_Init();  //ϵͳ��ʼ��
    while(1);
}

/************************��ʱ���жϺ���********************************/
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A (void)

{
    SysTimeFlash();     //ϵͳʱ�����
    CurrentDetect();    //��������ѹ���
    RodeDetect();       //С��λ�ü��
    Schedule();         //50ms ִ��һ�ο�������

    //Send_digital(SystemStatus.current);
}

/************************�����жϺ���********************************/
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
