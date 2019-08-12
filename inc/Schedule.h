#ifndef _Schdule_
#define _Schdule_

#define  Motol_Left_Speed  TA0CCR3//range 0-100
#define  Motol_Right_Speed  TA0CCR4//range 0-100

#include "ADC.h"
#include "Motol_and_control.h"
#include "Timer.h"
#include "Uart.h"

void SysTimeFlash();
void System_Init(); //系统初始化
void Rode_port_init();//光电对管输入引脚初始化
void RodeDetect();//赛道检测，更新小车位置
void Encode_port_init();//拨码开关引脚初始化
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
 * @系统结构体
 * @FUNCTION: 存储系统状态变量以及状态信息
 */
typedef struct  {
    char RodeStatus[5]; //表示光电对管的状态
    int Mode;   //模式标志位
    int current;//采样检测电流值
    int Voltage;//采样检测的电压值
    float time_s;//系统时间--秒
    int time_m;//系统时间--分
    int time_h;//系统时间--小时
    float TimerTime;//系统定时器时间
    int Circle; //交接点标志位
    int PassCircleNum;//经过交接圆点次数(个数)

//mode 1 coffesition
    int start;      //断电开始标志位
    int end;        //断电结束标志位
    int Mode_1_Flag;//模式一继电器通断   计时参数
//mode 4 coffesition
    int begin;
    int off;
    int TimerCount;
    int TimerCountRestart;
    int SpeedDlcine;

}SystemFuture;




#endif
