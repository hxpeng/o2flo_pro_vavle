/****************************************************
*版权所有：深圳XXX有限公司
*文件名：flow_module.h
*功能说明：
*作者：
*生成日期：
*****************************************************/
#ifndef  __FLOW_MODULE_H_
#define  __FLOW_MODULE_H_

/*****************************************************
*头文件包含
******************************************************/
#include "datatype.h"
#include "fifo.h"
#include "common.h"
#include "sf05.h"
#include <string.h>
/******************************************************
*内部宏定义
******************************************************/
#define FLOW_BUFFER_LEN  5
/******************************************************
*内部类型定义
******************************************************/
typedef struct
{
	u16 flow_slpm;		//质量流量 0.01 slpm
	u16 flow_lpm_set;			//体积流量 0.01 slpm
	u16 flow_slpm_rt;
	u16 flow_lpm_rt;
	u16 pro_pwm;			//比例阀控制PWM

}FLOW_MODULE;


typedef struct   
{
		 	int Uk; //总的控制量
		 	int Uk1;//上次的总控制量
		 	long Sum_error;//误差总量
		 	int Ek;//当前的误差量
		 	int Ek1;//前一次的误差量
		 	int Ek2;//前二次的误差量  
}FLOW_PID;

/******************************************************
*外部变量声明
******************************************************/
void sampling_module_detect_flow(FLOW_MODULE* module); 
/******************************************************
*外部函数声明
******************************************************/
void flow_control_task(void* pvParamemters);
void set_flow_parameter(UINT8 *pData);
void set_pid(UINT8 *pData);
void send_pid_2_PC(UINT8 *pData);
void set_pwm_dc(UINT8 *pData,int CMD);
void set_flow();

#endif
