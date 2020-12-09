/****************************************************
*��Ȩ���У�����XXX���޹�˾
*�ļ�����flow_module.h
*����˵����
*���ߣ�
*�������ڣ�
*****************************************************/
#ifndef  __FLOW_MODULE_H_
#define  __FLOW_MODULE_H_

/*****************************************************
*ͷ�ļ�����
******************************************************/
#include "datatype.h"
#include "fifo.h"
#include "common.h"
#include "sf05.h"
#include <string.h>
/******************************************************
*�ڲ��궨��
******************************************************/
#define FLOW_BUFFER_LEN  5
/******************************************************
*�ڲ����Ͷ���
******************************************************/
typedef struct
{
	u16 flow_slpm;		//�������� 0.01 slpm
	u16 flow_lpm_set;			//������� 0.01 slpm
	u16 flow_slpm_rt;
	u16 flow_lpm_rt;
	u16 pro_pwm;			//����������PWM

}FLOW_MODULE;


typedef struct   
{
		 	int Uk; //�ܵĿ�����
		 	int Uk1;//�ϴε��ܿ�����
		 	long Sum_error;//�������
		 	int Ek;//��ǰ�������
		 	int Ek1;//ǰһ�ε������
		 	int Ek2;//ǰ���ε������  
}FLOW_PID;

/******************************************************
*�ⲿ��������
******************************************************/
void sampling_module_detect_flow(FLOW_MODULE* module); 
/******************************************************
*�ⲿ��������
******************************************************/
void flow_control_task(void* pvParamemters);
void set_flow_parameter(UINT8 *pData);
void set_pid(UINT8 *pData);
void send_pid_2_PC(UINT8 *pData);
void set_pwm_dc(UINT8 *pData,int CMD);
void set_flow();

#endif
