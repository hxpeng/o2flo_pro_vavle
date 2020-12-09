/****************************************************
*版权所有：
*文件名：app.c
*功能说明：创建系统任务等
*作者：软件组
*生成日期：2017/02/27
*****************************************************/

/*****************************************************
*头文件包含
******************************************************/
#include "main.h"
#include "stm32f0xx_hal.h"
#include "cmsis_os.h"

#include "app.h"  
#include "sampling_module.h"
#include "flow_module.h"
#include "comm_module.h"
/******************************************************
*局部变量定义
******************************************************/

/******************************************************
*全局变量定义
******************************************************/
//采样任务
#define SAMPLING_TASK_STACK_DEPTH  128
#define SAMPLING_TASK_PRIORITY   2

//通信任务
#define COMM_TASK_STACK_DEPTH  128
#define COMM_TASK_PRIORITY   3

//流量任务
#define FLOW_TASK_STACK_DEPTH  128
#define FLOW_TASK_PRIORITY   4
/******************************************************
*内部函数声明
******************************************************/


/******************************************************
*其他定义
******************************************************/

/******************************************************
*外部函数定义
******************************************************/

/***************************************************
*函数名称：create_task
*功能描述：创建任务
*输入参数：无
*输出参数：无
*返回值：无
****************************************************/
void create_task(void)
{
	//采样任务
	xTaskCreate(sampling_task, "sampling task", SAMPLING_TASK_STACK_DEPTH, NULL, SAMPLING_TASK_PRIORITY, NULL);
	
	//通信任务
	xTaskCreate(comm_task, "comm task", COMM_TASK_STACK_DEPTH, NULL, COMM_TASK_PRIORITY, NULL);
	
	//流量任务
	xTaskCreate(flow_control_task, "flow control task", FLOW_TASK_STACK_DEPTH, NULL, FLOW_TASK_PRIORITY, NULL);
	
}


 
	 
