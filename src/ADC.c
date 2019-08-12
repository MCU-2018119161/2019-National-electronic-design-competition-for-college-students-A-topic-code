#include <msp430.h>
#include "Schedule.h"

extern SystemFuture SystemStatus;

/*
 * 使用P6.0 AND P6.1 作为ADC输入通道
 * 读电压P6.0 读电流P6.1
 */
void ADC_init()
{
    P6DIR |= BIT3 + BIT4;

    REFCTL0 &= ~REFMSTR;

    ADC12CTL0 |= ADC12MSC+ADC12ON;                                                                              //自动循环采样转换                                                                                //启动ADC12模块
    ADC12CTL1 |= ADC12CONSEQ_3;                                                                     //选择单通道循环采样转换
    ADC12CTL1 |= ADC12SHP;
   // ADC12CTL1 |= ADC12MSC;
    //ADC12CTL1 |= ADC12ON;                                                                                   //启动ADC12模块

    ADC12MCTL0 |= ADC12INCH_0;
    ADC12MCTL1 |= ADC12INCH_1;
    ADC12CTL0 |= ADC12ENC;
}

float FilterBuff[5],FilterBuffVoltage[5];
/*
 * ADC检测电流
 */
void CurrentDetect()
{
    int i=4;
    ADC12CTL0 |= ADC12SC;
    ADC12CTL1 |= ADC12SC;

/***电流检测***/
    FilterBuff[0]=ADC12MEM1;    //5层滑动滤波
    while(i>0){
        FilterBuff[i]=FilterBuff[i-1];
        i--;
    }
    SystemStatus.current=(FilterBuff[0]+FilterBuff[1]+FilterBuff[2]+FilterBuff[3]+FilterBuff[4])/5;
/***电压检测***/
    i=4;
    FilterBuffVoltage[0]=ADC12MEM0;
    while(i>0){
        FilterBuffVoltage[i]=FilterBuffVoltage[i-1];
            i--;
        }
    SystemStatus.Voltage=(FilterBuffVoltage[0]+FilterBuffVoltage[1]+FilterBuffVoltage[2]+FilterBuffVoltage[3]+FilterBuffVoltage[4])/5;

 //detect end

    if(SystemStatus.current>=100)       //要求一  充电亮灯
        P2OUT &= ~BIT7;
    else
        P2OUT |= BIT7;

 }
