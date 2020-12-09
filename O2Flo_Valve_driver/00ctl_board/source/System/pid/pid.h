
/****************************************************
*��Ȩ���У�
*�ļ�����app.c
*����˵��������ϵͳ�����
*���ߣ������
*�������ڣ�2017/02/27

*ʹ��˵����
1. ����ṹ������
	* PID_TYPE��
	* ADJUST_TYPE��
2. ��ʼ��PIDAdjustInit ���� PIDAdjustReset��
3. ����PID����PIDParaSet��
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
	INCREMENTAL, //����ʽ
	POSITION     //λ��ʽ
}PID_MODE;

typedef struct{
	int 					SetPoint; //ϵͳ����Ŀ��ֵ 
	long long 		SumError; //����ۼ� 

	float 	Kp; //�������� Proportional Const 
	float		Ki; //���ֳ��� Integral Const 
	float 	Kd; //΢�ֳ��� Derivative Const
	
	int 		LastError; //��һ�����
	int 		PrevError; //����һ�����
	
	int input_posi_limit;//�޷�
	int input_nega_limit;
	int output_posi_limit;
	int output_nega_limit;
	long long integral_posi_limit;
	long long integral_nega_limit;
}PID_TYPE;

typedef struct{
	PID_TYPE* _pid;
	
	int uk;					//PID���ֵ
	int uk_1;				//PID�ϴ����ֵ
	int delta_uk;
}ADJUST_TYPE;

//��������
void PIDAdjustInit(ADJUST_TYPE* sptr, PID_TYPE* pid_sptr);
void PIDParaSet(ADJUST_TYPE* sptr, PID_COEF pid_coef, PID_LIMIT limit);
int PIDCalc(PID_TYPE* sptr, int ThisError, PID_MODE mode);

//��չ����
void PIDAdjustReset(ADJUST_TYPE* sptr, PID_TYPE* pid_sptr);

#endif

