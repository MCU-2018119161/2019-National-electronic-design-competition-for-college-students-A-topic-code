#ifndef _Schdule_
#define _Schdule_

#define  Motol_Left_Speed  TA0CCR3//range 0-100
#define  Motol_Right_Speed  TA0CCR4//range 0-100

#include "ADC.h"
#include "Motol_and_control.h"
#include "Timer.h"
#include "Uart.h"

void SysTimeFlash();
void System_Init(); //ϵͳ��ʼ��
void Rode_port_init();//���Թ��������ų�ʼ��
void RodeDetect();//������⣬����С��λ��
void Encode_port_init();//���뿪�����ų�ʼ��
void System_Mode_confirm();
void Schedule();
void Mode_1(int ToMode2);
void Mode_2();
void Mode_3();
void Uart0_init();

void SendSring(char SendBuff[]);

void FollowLine();

void FollowLineMode3(int SpeedDecline);
/*
 * @ϵͳ�ṹ��
 * @FUNCTION: �洢ϵͳ״̬�����Լ�״̬��Ϣ
 */
typedef struct  {
    char RodeStatus[5]; //��ʾ���Թܵ�״̬
    int Mode;   //ģʽ��־λ
    int current;//����������ֵ
    int Voltage;//�������ĵ�ѹֵ
    float time_s;//ϵͳʱ��--��
    int time_m;//ϵͳʱ��--��
    int time_h;//ϵͳʱ��--Сʱ
    float TimerTime;//ϵͳ��ʱ��ʱ��
    int Circle; //���ӵ��־λ
    int PassCircleNum;//��������Բ�����(����)

//mode 1 coffesition
    int start;      //�ϵ翪ʼ��־λ
    int end;        //�ϵ������־λ
    int Mode_1_Flag;//ģʽһ�̵���ͨ��   ��ʱ����
//mode 4 coffesition
    int begin;
    int off;
    int TimerCount;
    int TimerCountRestart;
    int SpeedDlcine;

}SystemFuture;




#endif
