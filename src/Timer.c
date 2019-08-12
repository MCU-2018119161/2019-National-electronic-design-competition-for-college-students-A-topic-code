#include <msp430.h>
#include "Timer.h"
#include "Schedule.h"

/*
 * Ӳ��PWM��ʼ����ʹ��PWM����˿� P1.4 and P1.5
 */
void PWM_init()
{
    P1SEL|=BIT5;P1DIR|=BIT5;  //TA0.4 PWM��� 4�� P0.4
    P1SEL|=BIT4;P1DIR|=BIT4;  //TA0.3 PWM��� 5�� P0.3

    TA0CCTL4 = OUTMOD_7;
    TA0CCTL3 = OUTMOD_7;

    TA0CCR0=50;     //1MHZƵ��   ������ֵΪ50 ���Ӧ�ıȽ�Ƶ��Ϊ20KHZ �����������

    TA0CCR3=48;     //ͨ��3 �Ƚ���ֵ
    TA0CCR4=48;         //ͨ��4�Ƚ���ֵ

    TA0CTL = TASSEL_2 + MC_1 + TACLR;
}

/*
 * �жϳ�ʼ�������ж�
 */
void Timer_init()
{
    WDTCTL = WDTPW | WDTHOLD;                // �رտ��Ź�
    TA0CTL |= MC_1 + TASSEL_2 + TACLR;      //ʱ��ΪSMCLK,�Ƚ�ģʽ����ʼʱ���������
    TA0CCTL0 = CCIE;                        //�Ƚ����ж�ʹ��
    //TA0CCR0  = 100;                         //�Ƚ�ֵ��Ϊ50000���൱��50ms��ʱ����s
}
