
/****************************************************
*版权所有：
*文件名：app.c
*功能说明：创建系统任务等
*作者：软件组
*生成日期：2017/02/27

*使用说明：
1. 定义结构体类型
	* PID_TYPE；
	* ADJUST_TYPE；
2. 初始化PIDAdjustInit 或者 PIDAdjustReset；
3. 设置PID参数PIDParaSet；
4. 
*****************************************************/

#ifndef __PID_PID_H
#define __PID_PID_H


#include "datatype.h"

typedef struct{
	float kp;
	float ki;
	float kd;
}PID_COEF;

typedef struct{
	int input_posi_limit;
	int input_nega_limit;
	int output_posi_limit;
	int output_nega_limit;
	long long integral_posi_limit;
	long long integral_nega_limit;
}PID_LIMIT;

typedef enum{
	INCREMENTAL, //增量式
	POSITION     //位置式
}PID_MODE;

typedef struct{
	int 					SetPoint; //系统调节目标值 
	long long 		SumError; //误差累计 

	float 	Kp; //比例常数 Proportional Const 
	float		Ki; //积分常数 Integral Const 
	float 	Kd; //微分常数 Derivative Const
	
	int 		LastError; //上一次误差
	int 		PrevError; //上上一次误差
	
	int input_posi_limit;//限幅
	int input_nega_limit;
	int output_posi_limit;
	int output_nega_limit;
	long long integral_posi_limit;
	long long integral_nega_limit;
}PID_TYPE;

typedef struct{
	PID_TYPE* _pid;
	
	int uk;					//PID输出值
	int uk_1;				//PID上次输出值
	int delta_uk;
}ADJUST_TYPE;

//基本方法
void PIDAdjustInit(ADJUST_TYPE* sptr, PID_TYPE* pid_sptr);
void PIDParaSet(ADJUST_TYPE* sptr, PID_COEF pid_coef, PID_LIMIT limit);
int PIDCalc(PID_TYPE* sptr, int ThisError, PID_MODE mode);

//扩展方法
void PIDAdjustReset(ADJUST_TYPE* sptr, PID_TYPE* pid_sptr);

#endif

