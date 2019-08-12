#include <msp430.h>
#include "Schedule.h"
#include <stdio.h>
#include <stdlib.h>
/*
 * 串口0初始化
 */
void Uart0_init()
{
    P3SEL |= BIT3+BIT4; // P3.3,4 = USCI_A0 TXD/RXD

    UCA0CTL1 |= UCSWRST; // **Put state machine in reset**
    UCA0CTL1 |= UCSSEL_2; // SMCLK
    UCA0BR0 = 9; // 1MHz 115200 (see User's Guide)
    UCA0BR1 = 0; // 1MHz 115200
    UCA0MCTL |= UCBRS_1 + UCBRF_0; // Modulation UCBRSx=1, UCBRFx=0
    UCA0IE |= UCRXIE+UCTXIE; // Enable USCI_A0 RX interrupt
   // UCA0IE |= UCTXIE; // Enable USCI_A0 TX interrupt
    UCA0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**
}

/*
 *串口发送字符串
 */
void SendSring(char SendBuff[])
{
    //TA0CTL &= ~TAIE;

    while(*SendBuff!='\0'){

        UCA0TXBUF=*SendBuff++;

        while (!(UCA0IFG&UCTXIFG));
             }

    //TA0CTL |= TAIE;
}

/*
 * Send_digital
 * 发送数字，最大为4位，不能使用itoa 只能拆解发送了
 */
void Send_digital(int Num)
{
    char string[4];
    int i_=0;

    string[3] = Num%10+48;
    string[0] = Num/1000+48;
    string[1] = Num/100%10+48;
    string[2] = Num/10%10+48;

    while(i_<4)
    {
        if(string[i_]>57||string[i_]<48)
            break;
            i_++;
    }
    while(i_<4)
    {
        string[i_]=0+48;
        i_++;
    }
   // itoa(Num,string,10);
    SendSring(string);
}
