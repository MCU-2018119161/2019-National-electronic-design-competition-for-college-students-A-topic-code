#include <msp430.h>
#include "Schedule.h"

extern SystemFuture SystemStatus;

/*
 * ʹ��P6.0 AND P6.1 ��ΪADC����ͨ��
 * ����ѹP6.0 ������P6.1
 */
void ADC_init()
{
    P6DIR |= BIT3 + BIT4;

    REFCTL0 &= ~REFMSTR;

    ADC12CTL0 |= ADC12MSC+ADC12ON;                                                                              //�Զ�ѭ������ת��                                                                                //����ADC12ģ��
    ADC12CTL1 |= ADC12CONSEQ_3;                                                                     //ѡ��ͨ��ѭ������ת��
    ADC12CTL1 |= ADC12SHP;
   // ADC12CTL1 |= ADC12MSC;
    //ADC12CTL1 |= ADC12ON;                                                                                   //����ADC12ģ��

    ADC12MCTL0 |= ADC12INCH_0;
    ADC12MCTL1 |= ADC12INCH_1;
    ADC12CTL0 |= ADC12ENC;
}

float FilterBuff[5],FilterBuffVoltage[5];
/*
 * ADC������
 */
void CurrentDetect()
{
    int i=4;
    ADC12CTL0 |= ADC12SC;
    ADC12CTL1 |= ADC12SC;

/***�������***/
    FilterBuff[0]=ADC12MEM1;    //5�㻬���˲�
    while(i>0){
        FilterBuff[i]=FilterBuff[i-1];
        i--;
    }
    SystemStatus.current=(FilterBuff[0]+FilterBuff[1]+FilterBuff[2]+FilterBuff[3]+FilterBuff[4])/5;
/***��ѹ���***/
    i=4;
    FilterBuffVoltage[0]=ADC12MEM0;
    while(i>0){
        FilterBuffVoltage[i]=FilterBuffVoltage[i-1];
            i--;
        }
    SystemStatus.Voltage=(FilterBuffVoltage[0]+FilterBuffVoltage[1]+FilterBuffVoltage[2]+FilterBuffVoltage[3]+FilterBuffVoltage[4])/5;

 //detect end

    if(SystemStatus.current>=100)       //Ҫ��һ  �������
        P2OUT &= ~BIT7;
    else
        P2OUT |= BIT7;

 }
