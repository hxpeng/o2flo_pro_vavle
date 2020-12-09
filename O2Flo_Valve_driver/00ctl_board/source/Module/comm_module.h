/****************************************************
*版权所有：深圳XXX有限公司
*文件名：comm_module.h
*功能说明：
*作者：
*生成日期：
*****************************************************/
#ifndef  __COMM_MODULE_H_
#define  __COMM_MODULE_H_

/*****************************************************
*头文件包含
******************************************************/
#include "datatype.h"
#include "fifo.h"
#include "common.h"

#ifndef _COMM_MODULE_H
#define _COMM_MODULE_H
#include "comm_task.h"
//#include "o2flo_system_cfg.h"
#include "stm32f0xx_hal.h"
/******************************************************
*内部宏定义
******************************************************/
#define   CMD_BUFFER_LENGTH         	100  //宏定义命令数据包的长度
#define	  PACK_HEAD_BYTE				0xFF //头文件标志
#define   MODULE_CMD_TYPE				0


#define  TX_FIFO_LEN  100

//--------------------模块发送----------------//
#define MODULE_SEND_FIO2_FLOW_RT_ID   	0x01 //50HZ
#define MODULE_SEND_FIO2_FLOW_PRESS_TEMP_AVG_ID   	0x02 //1HZ
#define MODULE_SEND_CALI_PATA_ID  0x10 //1HZ 作为校准用

//调试
#define MODULE_SEND_FIO2_TIME_ID  0x20
#define MODULE_SEND_TEST_ID  0x80
//--------------------主机发送----------------//
#define HOST_SEND_TEST_ID  0x80
#define HOST_SEND_SETTING_FLOW 0x31
#define SLAVER_SEND_DATA			 0x31

#define HOST_SEND_MODIFY_PID    						 0x32
#define SLAVER_SEND_MODIFY_PID_SUCCESS 			 0x32
#define HOSET_SEND_GET_PID									 0x33
#define SLAVER_SEND_PID_GET_RESULT					 0x33      
#define HOST_SEND_MANUAL_SET_PWM_DC					 0x34      //手动设置比例阀PWM的D.C
#define HOST_SEND_TEST_VALVE_CURVE				   0x35      //测试比例阀的特性曲线
#define HOST_GET_DATA				   0x36

/******************************************************
*内部类型定义
******************************************************/


/******************************************************
*外部变量声明
******************************************************/

/******************************************************
*外部函数声明
******************************************************/
void comm_task(void* pvParamemters);










//***************以下需要用户自己配置***************************************************

//定义"发送数据由回调函数触发，默认对应方案A，如果屏蔽就变成了方案B
//#define SEND_TRIGGERED_BY_TXCALLBACK 

//ProcessRcvedData()可以在中断中使用，也可以在接收任务中使用
//#define PROCESS_RCV_IN_TASK

//#define UART1_ENABLE 
#define UART2_ENABLE
//#define UART3_ENABLE 
//#define UART4_ENABLE

//定义串口接收的buffer长度
#define USART1_BUFFER_SIZE  100
#define USART2_BUFFER_SIZE  100
#define USART3_BUFFER_SIZE  100
#define USART4_BUFFER_SIZE  100

//send buffer的控制配置,3表示有3个发送buffer,50表示每个buffer有50字节
//这里UART3和UART4不在做分别定义了，全部通用成3和50
#define SEND_BUFFER_QTY     4
#define SEND_BUFFER_SIZE    100

//*****************************************************************************************

//目前就支持两种方案A和B，后续如果有就添加
//A-由HAL_UART_TxCpltCallback控制下一帧的发送
//B-由发送任务自己控制，到时间就发送，不用管HAL_UART_TxCpltCallback
#ifdef SEND_TRIGGERED_BY_TXCALLBACK
#define UART_SEND_PLAN_A
#else
#define UART_SEND_PLAN_B
#endif


typedef struct
{
	uint8_t flg;
	uint8_t* p_Adress;
}FLG_ADRESS_CTRL;

typedef struct
{
	uint8_t ready_2_send;   //发送标志位 1-可以发送; 0-不能发送
	uint8_t cur_flgAdressCtrl_No;     //当前buffer flg的编号
	FLG_ADRESS_CTRL flg_address_ctrl[SEND_BUFFER_QTY];
}DATA_SEND_CTRL;

typedef struct 
{
	uint16_t recv_flg;
	uint16_t recv_length; 
	uint32_t prev_Index;
	uint32_t cur_Index;
}UART_RCV_CTRL;


////串口1，定义串口接收和发送数据buffer的长度
//extern DMA_HandleTypeDef hdma_usart1_rx;
//extern UART_HandleTypeDef huart1;
//extern uint8_t g_UART1_recvBuffer[USART1_BUFFER_SIZE];
//extern uint8_t g_UART1_preProcess_Buffer[USART1_BUFFER_SIZE];
//extern UART_RCV_CTRL UART1_rcv_Ctrl;
//extern DATA_SEND_CTRL UART1_send_ctrl;

//串口2，定义串口接收和发送数据buffer的长度
extern DMA_HandleTypeDef hdma_uart2_rx;
extern UART_HandleTypeDef huart2;
extern uint8_t g_UART2_recvBuffer[USART2_BUFFER_SIZE];
extern uint8_t g_UART2_preProcess_Buffer[USART2_BUFFER_SIZE];
extern UART_RCV_CTRL g_UART2_Ctrl;
extern DATA_SEND_CTRL UART2_send_ctrl;

////串口3，定义串口接收和发送数据buffer的长度
//extern DMA_HandleTypeDef hdma_usart3_rx;
//extern UART_HandleTypeDef huart3;
//extern uint8_t g_UART3_recvBuffer[USART3_BUFFER_SIZE];
//extern uint8_t g_UART3_preProcess_Buffer[USART3_BUFFER_SIZE];
//extern UART_RCV_CTRL UART3_rcv_Ctrl;
//extern DATA_SEND_CTRL UART3_send_ctrl;

////串口4，定义串口接收和发送数据buffer的长度
//extern DMA_HandleTypeDef hdma_uart4_rx;
//extern UART_HandleTypeDef huart4;
//extern uint8_t g_UART4_recvBuffer[USART4_BUFFER_SIZE];
//extern uint8_t g_UART4_preProcess_Buffer[USART4_BUFFER_SIZE];
//extern UART_RCV_CTRL g_UART4_Ctrl;
//extern DATA_SEND_CTRL UART4_send_ctrl;

//对外接口
void uart_Init(void);
void ProcessRcvedData(void);
void sendData(void);
void Process_UART_IDLE_INTERRUPT(UART_HandleTypeDef* huart);
uint8_t* SaveDataTo(DATA_SEND_CTRL* p_send_ctrl,uint8_t* pScr,int len);
void set_checkSum(uint8_t* pArray);
#endif //_COMM_MODULE_H

#endif
