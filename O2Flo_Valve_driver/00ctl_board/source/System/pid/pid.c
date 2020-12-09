/****************************************************
*版权所有：
*文件名：pid.c
*功能说明：创建系统任务等
*作者：软件组
*生成日期：2017/02/27
*****************************************************/

#include "pid.h"
#include "datatype.h"

//初始化PID控制参数
void PIDInit(PID_TYPE* sptr) 
{ 
	if(NULL == sptr)
	{
		return;
	}
	
	sptr->Kp = 0; //比例常数 Proportional Const 
	sptr->Ki = 0; //积分常数Integral Const 
	sptr->Kd = 0; //微分常数 Derivative Const 
	
	sptr->LastError = 0; //Error[-1]
	sptr->PrevError = 0; //Error[-2] 
	
	sptr->SetPoint = 0;
	
	sptr->SumError = 0;//累加误差
}

//PID计算
int PIDCalc(PID_TYPE* sptr, int ThisError, PID_MODE mode)
{
	int pError, iError, dError, uk_temp;
	long ThisIncVal, pIncVal, iIncVal, dIncVal; 
	int err_limit_val = 0;

	if(NULL == sptr)
	{
		return -1;
	}
	
		//限制输入
	if(sptr->SumError > sptr->input_posi_limit)
		sptr->SumError = sptr->input_posi_limit;
	else if(sptr->SumError < sptr->input_nega_limit)
		sptr->SumError = sptr->input_nega_limit;
	
	//--------------------------增量式（1）-----------------------------------//
	if(INCREMENTAL == mode)
	{
		//增量式
		//获取偏差
		pError = ThisError - sptr->LastError; 

		iError = ThisError;

		dError = ThisError - 2 * (sptr->LastError) + sptr->PrevError;
		
		//存储误差用于下次运算
		sptr->PrevError = sptr->LastError;
		sptr->LastError = ThisError;

		//增量计算
		pIncVal = (sptr->Kp * pError);
		iIncVal = (sptr->Ki * iError);
		dIncVal = (sptr->Kd * dError);
		
		//得到PID输出
		uk_temp = pIncVal + iIncVal + dIncVal;
		
		//输出限幅
	}
	//--------------------------位置式（2）-----------------------------------//
	else
	{	
		//增量计算 
		pIncVal = (sptr->Kp * ThisError);
		iIncVal = (sptr->Ki * sptr->SumError);
		dIncVal = (sptr->Kd * (ThisError - sptr->LastError));
		
		//得到PID输出
		uk_temp = pIncVal + iIncVal + dIncVal;
		
		//累加值
		sptr->SumError += ThisError;
		sptr->LastError = ThisError;
		
		//积分饱和处理
		if(sptr->SumError > sptr->integral_posi_limit)
			sptr->SumError = sptr->integral_posi_limit;
		else if(sptr->SumError < sptr->integral_nega_limit)
			sptr->SumError = sptr->integral_nega_limit;
	}
	
	//输出限幅
	if(uk_temp < sptr->output_nega_limit)
		uk_temp = sptr->output_nega_limit;
	else if(uk_temp > sptr->output_posi_limit)
		uk_temp = sptr->output_posi_limit;
	
	return uk_temp; 
}

//初始化调节器
void PIDAdjustInit(ADJUST_TYPE* sptr, PID_TYPE* pid_sptr)
{
	if(NULL == sptr)
	{
		return;
	}
	
	sptr->delta_uk = 0;
	sptr->uk = 0;
	sptr->uk_1 = 0;
		
	PIDInit(pid_sptr);
	
	sptr->_pid = pid_sptr;
}

//复位调节器
void PIDAdjustReset(ADJUST_TYPE* sptr, PID_TYPE* pid_sptr)
{
	if(NULL == sptr)
	{
		return;
	}
	
	sptr->delta_uk = 0;//增量
	sptr->uk = 0; //
	sptr->uk_1 = 0;

	PIDInit(pid_sptr);
	
	sptr->_pid = pid_sptr;
}


//设置PID参数
void PIDParaSet(ADJUST_TYPE* sptr, PID_COEF pid_coef, PID_LIMIT limit)
{	
	if(NULL == sptr)
	{
		return;
	}
	
	//设置pid参数
	sptr->_pid->Kp = pid_coef.kp;
	sptr->_pid->Ki = pid_coef.ki;
	sptr->_pid->Kd = pid_coef.kd;
	
	//设置限幅
	sptr->_pid->input_posi_limit = limit.input_posi_limit;
	sptr->_pid->input_nega_limit = limit.input_nega_limit;
	sptr->_pid->output_posi_limit = limit.output_posi_limit;
	sptr->_pid->output_nega_limit = limit.output_nega_limit;
	sptr->_pid->integral_posi_limit = limit.integral_posi_limit;
	sptr->_pid->integral_nega_limit = limit.integral_nega_limit;
}

















