/****************************************************
*��Ȩ���У�
*�ļ�����pid.c
*����˵��������ϵͳ�����
*���ߣ������
*�������ڣ�2017/02/27
*****************************************************/

#include "pid.h"
#include "datatype.h"

//��ʼ��PID���Ʋ���
void PIDInit(PID_TYPE* sptr) 
{ 
	if(NULL == sptr)
	{
		return;
	}
	
	sptr->Kp = 0; //�������� Proportional Const 
	sptr->Ki = 0; //���ֳ���Integral Const 
	sptr->Kd = 0; //΢�ֳ��� Derivative Const 
	
	sptr->LastError = 0; //Error[-1]
	sptr->PrevError = 0; //Error[-2] 
	
	sptr->SetPoint = 0;
	
	sptr->SumError = 0;//�ۼ����
}

//PID����
int PIDCalc(PID_TYPE* sptr, int ThisError, PID_MODE mode)
{
	int pError, iError, dError, uk_temp;
	long ThisIncVal, pIncVal, iIncVal, dIncVal; 
	int err_limit_val = 0;

	if(NULL == sptr)
	{
		return -1;
	}
	
		//��������
	if(sptr->SumError > sptr->input_posi_limit)
		sptr->SumError = sptr->input_posi_limit;
	else if(sptr->SumError < sptr->input_nega_limit)
		sptr->SumError = sptr->input_nega_limit;
	
	//--------------------------����ʽ��1��-----------------------------------//
	if(INCREMENTAL == mode)
	{
		//����ʽ
		//��ȡƫ��
		pError = ThisError - sptr->LastError; 

		iError = ThisError;

		dError = ThisError - 2 * (sptr->LastError) + sptr->PrevError;
		
		//�洢��������´�����
		sptr->PrevError = sptr->LastError;
		sptr->LastError = ThisError;

		//��������
		pIncVal = (sptr->Kp * pError);
		iIncVal = (sptr->Ki * iError);
		dIncVal = (sptr->Kd * dError);
		
		//�õ�PID���
		uk_temp = pIncVal + iIncVal + dIncVal;
		
		//����޷�
	}
	//--------------------------λ��ʽ��2��-----------------------------------//
	else
	{	
		//�������� 
		pIncVal = (sptr->Kp * ThisError);
		iIncVal = (sptr->Ki * sptr->SumError);
		dIncVal = (sptr->Kd * (ThisError - sptr->LastError));
		
		//�õ�PID���
		uk_temp = pIncVal + iIncVal + dIncVal;
		
		//�ۼ�ֵ
		sptr->SumError += ThisError;
		sptr->LastError = ThisError;
		
		//���ֱ��ʹ���
		if(sptr->SumError > sptr->integral_posi_limit)
			sptr->SumError = sptr->integral_posi_limit;
		else if(sptr->SumError < sptr->integral_nega_limit)
			sptr->SumError = sptr->integral_nega_limit;
	}
	
	//����޷�
	if(uk_temp < sptr->output_nega_limit)
		uk_temp = sptr->output_nega_limit;
	else if(uk_temp > sptr->output_posi_limit)
		uk_temp = sptr->output_posi_limit;
	
	return uk_temp; 
}

//��ʼ��������
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

//��λ������
void PIDAdjustReset(ADJUST_TYPE* sptr, PID_TYPE* pid_sptr)
{
	if(NULL == sptr)
	{
		return;
	}
	
	sptr->delta_uk = 0;//����
	sptr->uk = 0; //
	sptr->uk_1 = 0;

	PIDInit(pid_sptr);
	
	sptr->_pid = pid_sptr;
}


//����PID����
void PIDParaSet(ADJUST_TYPE* sptr, PID_COEF pid_coef, PID_LIMIT limit)
{	
	if(NULL == sptr)
	{
		return;
	}
	
	//����pid����
	sptr->_pid->Kp = pid_coef.kp;
	sptr->_pid->Ki = pid_coef.ki;
	sptr->_pid->Kd = pid_coef.kd;
	
	//�����޷�
	sptr->_pid->input_posi_limit = limit.input_posi_limit;
	sptr->_pid->input_nega_limit = limit.input_nega_limit;
	sptr->_pid->output_posi_limit = limit.output_posi_limit;
	sptr->_pid->output_nega_limit = limit.output_nega_limit;
	sptr->_pid->integral_posi_limit = limit.integral_posi_limit;
	sptr->_pid->integral_nega_limit = limit.integral_nega_limit;
}

















