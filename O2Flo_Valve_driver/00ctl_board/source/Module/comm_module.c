
#include "comm_module.h"
#include "comm_def.h"



#include <string.h>
#include <stdlib.h>



//***************************重要说明******************************
//该文件旨在配置成通用文件，用户不用关心如何实现，只需调用本文档提供的
//6个API: void uart_Init(void),
//        void ProcessRcvedData(void),
//        void sendData(void),
//        Process_UART_IDLE_INTERRUPT(UART_HandleTypeDef* huart)，
//        SaveDataTo(DATA_SEND_CTRL* p_send_ctrl,uint8_t* pScr,int len)，
//        set_checkSum(uint8_t* pArray)
//
//使用该文件前的准备工作：
//1.在CUBE中配置自己需要的串口，RX-配置成circular,TX-配置成Normal
//2.准备两个任务，一个用于串口收数据，一个用于串口发数据
//
//
//如何使用这6个API:
//1.uart_Init()需要在系统的初始化中调用，本项目在init_o2flo_system()中调用，初始化函数
//的作用：初始化发送控制器(根据配置,malloc需要的发送buffer)，接收控制器，打开串口DMA接收，开启串口IDLE中断
//2.ProcessRcvedData()在串口接收任务中调用
//3.sendData()在串口发送中调用
//4.Process_UART_IDLE_INTERRUPT在USART3_IRQHandler()等串口中断中调用
//5.SaveDataTo()将要发送的数据包保存到相应的串口发送控制器
//6.set_checkSum()设置checksum
//
//通信原理介绍(以串口1举例)：
//接收任务:       
//g_UART1_recvBuffer                  对应DMA的fifo    
//                                         |
//                                        \|/
//g_UART1_preProcess_Buffer  通过调用getRcvData2PreProBuf，将g_UART1_recvBuffer
//                           中的数据拷贝到g_UART1_preProcess_Buffer中
//                                         |
//                                        \|/
//UART1_send_ctrl            将g_UART1_preProcess_Buffer中的数据解析出来
//                           将多帧数据切割(假设有多帧),放入初始化函数中
//                            malloc出来的buffer中
//                                         |
//                                        \|/
//发送任务                   将UART1_send_ctrl的发送buffer逐个解析(CmdID,DeviceID等)
//                           进行相应操作(发送或者设置),其中的发送控制需要回调函数
//                           HAL_UART_TxCpltCallback()配合，该函数会告诉发送任务该发送哪个buffer
//
//备注：ParseAndSend()这个函数声明成extern,该函数的实现过程由用户自己实现，本文件只作为通用文件
//      解析CmdID发送相应数据等操作本文档不需要知道，这样方便移植
//
//Note:2019/07/31,目前使用HAL_UART_TxCpltCallback来控制下一个发送buffer,但是这是基于有应答要求，如果
//     收到的数据只接收自己处理，而不做出任何回应，那么就不会触发HAL_UART_TxCpltCallback，从而这套机制
//     就会失效，基于这种情况，从而做出另外一种方案B：发送任务只管发送，到了时间就发送
//方案A：优点-由Tx回调函数控制，确保每一帧发送完之后才清除buffer的flg，清除buffer，逻辑上非常好
//       缺点-如果接收的帧不需要回应，那么就不会触发Tx回调，这个机制就出问题了
//方案B: 优点-克服了方案A的缺点
//       缺点-在发送函数中，一进入函数就将buffer的flg清0,其实这个时候DMA还在发送中,如果这个时候接收到了数据
//            那么就会触发saveDataTo函数，将正在发送的buffer给重新装数据了，这是由buffer的flg标志位来决定的
//            这个方法需要好好控制发送，接收时序，要自己算好
//
//注意：static 函数中不要写static变量
//*****************************************************************

//函数调用关系
// USART3_IRQHandler()
// {
//   Process_UART_IDLE_INTERRUPT(&huart3); 
//   {
//     if(RESET != __HAL_UART_GET_FLAG(p_huart, UART_FLAG_IDLE))   //判断是否是空闲中断
//	   {
//		   __HAL_UART_CLEAR_IDLEFLAG(p_huart);                       //清除空闲中断标志（否则会一直不断进入中断）
//
//		   USAR_UART_IDLECallback(p_huart);                          //调用中断处理函数
//       {
//         UART3_rcv_Ctrl.recv_flg=1;                              //置标志位
//       }
//     }
//   }
// }
//
//串口接收任务，20ms一次
//void DATA_receive_task(const void* arg)
//{
//	//处理接收到的数据
//	ProcessRcvedData();
//  {
//    if(UART3_rcv_Ctrl.recv_flg)
//		{
//			UART3_rcv_Ctrl.recv_flg=0;
//			//1.将DMA的接收buffer中的数据放入预处理buffer中
//			getRcvData2PreProBuf(&huart3,g_UART3_preProcess_Buffer,sizeof(g_UART3_preProcess_Buffer));
//			//2.将预处理buffer中的数据整理(可能断帧，多帧)，放入发送控制器中
//			processData2Ctrollor(&UART3_send_ctrl,g_UART3_preProcess_Buffer);    //处理数据放入发送控制器中
//			{
//        ...
//
//				SaveDataTo(p_Sndctrl,&p_start[i],pack_len);   //将数据保存到控制器的队列buffer中
//        
//        ...
//			}
//			//3.清空预处理buffer,准备下次的数据接收
//			memset(g_UART3_preProcess_Buffer,0,sizeof(g_UART3_preProcess_Buffer));
//		}
//  }
//}
//
//
////串口发送任务,20ms一次
//void DATA_send_task(const void* arg)
//{
//	sendData();
//	{
//		if(!UART3_rcv_Ctrl.recv_flg)   //在数据接收完成之后才啊发送
//		{
//			send_data_by_controllor(&UART3_send_ctrl,&huart3);
//		}
//	}
//}


//该函数剥离出来放到通信任务的文件中，让用户自己定制
extern void ParseAndSend(UART_HandleTypeDef* p_huart,uint8_t* pArray);

#ifdef UART1_ENABLE
//串口3，定义串口接收和发送数据buffer的长度
extern DMA_HandleTypeDef hdma_usart1_rx;
extern UART_HandleTypeDef huart1;
uint8_t g_UART1_recvBuffer[USART1_BUFFER_SIZE]={0};
uint8_t g_UART1_preProcess_Buffer[USART1_BUFFER_SIZE]={0};
UART_RCV_CTRL UART1_rcv_Ctrl={0};
DATA_SEND_CTRL UART1_send_ctrl;
#endif

#ifdef UART2_ENABLE
//串口3，定义串口接收和发送数据buffer的长度
extern DMA_HandleTypeDef hdma_usart2_rx;
extern UART_HandleTypeDef huart2;
uint8_t g_UART2_recvBuffer[USART2_BUFFER_SIZE]={0};
uint8_t g_UART2_preProcess_Buffer[USART2_BUFFER_SIZE]={0};
UART_RCV_CTRL UART2_rcv_Ctrl={0};
DATA_SEND_CTRL UART2_send_ctrl;
#endif


#ifdef UART3_ENABLE
//串口3，定义串口接收和发送数据buffer的长度
extern DMA_HandleTypeDef hdma_usart3_rx;
extern UART_HandleTypeDef huart3;
uint8_t g_UART3_recvBuffer[USART3_BUFFER_SIZE]={0};
uint8_t g_UART3_preProcess_Buffer[USART3_BUFFER_SIZE]={0};
UART_RCV_CTRL UART3_rcv_Ctrl={0};
DATA_SEND_CTRL UART3_send_ctrl;
#endif

#ifdef UART4_ENABLE
//串口4，定义串口接收和发送数据buffer的长度
extern DMA_HandleTypeDef hdma_uart4_rx;
extern UART_HandleTypeDef huart4;
uint8_t g_UART4_recvBuffer[USART4_BUFFER_SIZE]={0};
uint8_t g_UART4_preProcess_Buffer[USART4_BUFFER_SIZE]={0};
UART_RCV_CTRL g_UART4_Ctrl={0};
DATA_SEND_CTRL UART4_send_ctrl;
#endif



//CRC表
const uint16_t crc_table[256] ={                                                                   //Crc table
0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b, 
0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6, 
0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de, 0x2462, 0x3443, 0x0420, 0x1401, 
0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d, 
0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719, 0x8738, 
0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823, 
0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b, 0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 
0x1a71, 0x0a50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a, 
0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd, 
0xad2a, 0xbd0b, 0x8d68, 0x9d49, 0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70, 
0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78, 0x9188, 0x81a9, 0xb1ca, 0xa1eb, 
0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067, 
0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e, 0x02b1, 0x1290, 0x22f3, 0x32d2, 
0x4235, 0x5214, 0x6277, 0x7256, 0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d, 
0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 0xa7db, 0xb7fa, 0x8799, 0x97b8, 
0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634, 
0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827, 
0x18c0, 0x08e1, 0x3882, 0x28a3, 0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 
0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92, 0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 
0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1, 
0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 0x4e55, 0x5e74, 
0x2e93, 0x3eb2, 0x0ed1, 0x1ef0};

//获取CRC值
static uint16_t crc_16(uint8_t *Cdata, uint16_t len)
{
	uint16_t crc16 = 0 ;    
	uint16_t crc_h8, crc_l8;  
	uint8_t index=0;
	
	while( len-->(uint16_t)0 ) 
	{		
		crc_h8 = (crc16 >> 8);                                                                          //High byte		
		crc_l8 = (crc16 << 8);                                                                          //Low byte
		crc16 = crc_l8 ^ crc_table[crc_h8 ^ Cdata[index]];
		index++;			
	}
	return crc16;
}


//给指定的Array填充checksum
void set_checkSum(uint8_t* pArray)
{
	if(pArray[0]==HEAD_MARK0&&pArray[1]==HEAD_MARK1)   //如果头是0xAA55
	{
		uint16_t len=pArray[INDEX_LEN];              //获取帧的长度
		uint16_t crc16=crc_16(pArray+INDEX_LEN,len-2);
		pArray[len]=crc16%256;               //CheckSum1          
		pArray[len+1]=crc16/256;             //CheckSum2
	}
}
//初始化串口3，串口4收发buffer的设定
void uart_Init()
{
	#ifdef UART1_ENABLE
	//UART1 发送控制初始化
	UART1_send_ctrl.ready_2_send=1;             //1表示可以发送
	UART1_send_ctrl.cur_flgAdressCtrl_No=0;     //当前flgAdress控制器的编号，目前就3个，0，1，2

	for(int i=0;i<SEND_BUFFER_QTY;i++)  //在堆里分配3个buffer，每个buffer50字节
	{
		UART1_send_ctrl.flg_address_ctrl[i].flg=0;
		UART1_send_ctrl.flg_address_ctrl[i].p_Adress=malloc(SEND_BUFFER_SIZE);
	}
	
	//UART1 接收控制
	UART1_rcv_Ctrl.cur_Index=0;
	UART1_rcv_Ctrl.prev_Index=0;
	UART1_rcv_Ctrl.recv_flg=0;
	UART1_rcv_Ctrl.recv_length=0;
	
	HAL_UART_Receive_DMA(&huart1,g_UART1_recvBuffer,sizeof(g_UART1_recvBuffer));
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);              //开启串口1的IDLE中断
	#endif
	
	#ifdef UART2_ENABLE
	//UART2 发送控制初始化
	UART2_send_ctrl.ready_2_send=1;             //1表示可以发送
	UART2_send_ctrl.cur_flgAdressCtrl_No=0;     //当前flgAdress控制器的编号，目前就3个，0，1，2

	for(int i=0;i<SEND_BUFFER_QTY;i++)  //在堆里分配3个buffer，每个buffer50字节
	{
		UART2_send_ctrl.flg_address_ctrl[i].flg=0;
		UART2_send_ctrl.flg_address_ctrl[i].p_Adress=malloc(SEND_BUFFER_SIZE);
	}
	
	//UART2 接收控制
	UART2_rcv_Ctrl.cur_Index=0;
	UART2_rcv_Ctrl.prev_Index=0;
	UART2_rcv_Ctrl.recv_flg=0;
	UART2_rcv_Ctrl.recv_length=0;
	
	HAL_UART_Receive_DMA(&huart2,g_UART2_recvBuffer,sizeof(g_UART2_recvBuffer));
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);              //开启串口1的IDLE中断
	#endif
	
	
	#ifdef UART3_ENABLE
	//UART3 发送控制初始化
	UART3_send_ctrl.ready_2_send=1;             //1表示可以发送
	UART3_send_ctrl.cur_flgAdressCtrl_No=0;     //当前flgAdress控制器的编号，目前就3个，0，1，2

	for(int i=0;i<SEND_BUFFER_QTY;i++)  //在堆里分配3个buffer，每个buffer50字节
	{
		UART3_send_ctrl.flg_address_ctrl[i].flg=0;
		UART3_send_ctrl.flg_address_ctrl[i].p_Adress=malloc(SEND_BUFFER_SIZE);
	}
	
	//UART3 接收控制
	UART3_rcv_Ctrl.cur_Index=0;
	UART3_rcv_Ctrl.prev_Index=0;
	UART3_rcv_Ctrl.recv_flg=0;
	UART3_rcv_Ctrl.recv_length=0;
	
	HAL_UART_Receive_DMA(&huart3,g_UART3_recvBuffer,sizeof(g_UART3_recvBuffer));
	__HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);              //开启串口3的IDLE中断
	#endif
	
	#ifdef UART4_ENABLE
	//UART4 发送控制初始化  
	UART4_send_ctrl.ready_2_send=1;             //1表示可以发送
	UART4_send_ctrl.cur_flgAdressCtrl_No=0;     //当前flgAdress控制器的编号，目前就3个，0，1，2

	for(int i=0;i<SEND_BUFFER_QTY;i++)  //在堆里分配3个buffer，每个buffer50字节
	{
		UART4_send_ctrl.flg_address_ctrl[i].flg=0;
		UART4_send_ctrl.flg_address_ctrl[i].p_Adress=malloc(SEND_BUFFER_SIZE);
	} 
	
	//UART4 接收控制
	g_UART4_Ctrl.cur_Index=0;
	g_UART4_Ctrl.prev_Index=0;
	g_UART4_Ctrl.recv_flg=0;
	g_UART4_Ctrl.recv_length=0;
	
	HAL_UART_Receive_DMA(&huart4,g_UART4_recvBuffer,sizeof(g_UART4_recvBuffer));
	__HAL_UART_ENABLE_IT(&huart4, UART_IT_IDLE);              //开启串口4的IDLE中断
	#endif
		
}


//uint32_t last_remain=USART4_BUFFER_SIZE;
//uint32_t current_remain=0;
//串口IDLE中断的回调函数
static void USAR_UART_IDLECallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART1)
	{
		#ifdef UART1_ENABLE
		UART1_rcv_Ctrl.recv_flg=1;
		#endif
	}
	else if(huart->Instance==USART2)
	{
		#ifdef UART2_ENABLE
		UART2_rcv_Ctrl.recv_flg=1;
		#endif
	}
	else
	{
		//do nothing
	}
	
}

//串口IDLE中断处理函数
void Process_UART_IDLE_INTERRUPT(UART_HandleTypeDef* huart)
{
	UART_HandleTypeDef* p_huart=huart;
	if(p_huart->Instance==USART1)
	{
		#ifdef UART1_ENABLE
		p_huart=&huart1;
		#endif
	}
	else if(p_huart->Instance==USART2)
	{
		#ifdef UART2_ENABLE
		p_huart=&huart2;
		#endif
	}
	else
	{
		//do nothing
	}

	if(RESET != __HAL_UART_GET_FLAG(p_huart, UART_FLAG_IDLE))   //判断是否是空闲中断
	{
		__HAL_UART_CLEAR_IDLEFLAG(p_huart);                       //清除空闲中断标志（否则会一直不断进入中断）

		USAR_UART_IDLECallback(p_huart);                          //调用中断处理函数
	}
}

static void Move_to_Next_buffer(DATA_SEND_CTRL* p_send_ctrl)
{
	p_send_ctrl->cur_flgAdressCtrl_No++;
	if(p_send_ctrl->cur_flgAdressCtrl_No==SEND_BUFFER_QTY)
	{
		p_send_ctrl->cur_flgAdressCtrl_No=0;
	}
}


//在DMA数据发送成功，回调，设置发送控制器
static void set_controllor_after_sendCmplt(DATA_SEND_CTRL* p_send_ctrl ,UART_HandleTypeDef *p_huart)
{
	//如果定了方案A，发送需要回调函数来控制
	#ifdef UART_SEND_PLAN_A
//	if((p_send_ctrl==&UART3_send_ctrl&&p_huart==&huart3)   //确保传入的参数是正确的
//		||(p_send_ctrl==&UART4_send_ctrl&&p_huart==&huart4))
	{
		DATA_SEND_CTRL* p_ctrl;
		
		if(p_huart->Instance==USART1)
		{
			#ifdef UART1_ENABLE
			p_ctrl=&UART1_send_ctrl;
			#endif
		}
		else if(p_huart->Instance==USART2)
		{
			#ifdef UART2_ENABLE
			p_ctrl=&UART2_send_ctrl;
			#endif
		}
		else if(p_huart->Instance==USART3)
		{
			#ifdef UART3_ENABLE
			p_ctrl=&UART3_send_ctrl;
			#endif
		}
		else if(p_huart->Instance==UART4)
		{
			#ifdef UART4_ENABLE
			p_ctrl=&UART4_send_ctrl;
			#endif
		}
		else
		{
			//do nothing
		}
		
		//1.将ready_2_send置为1
		p_ctrl->ready_2_send=1;
		//2.发送buffer对应的flg置为0，表示该buffer数据已经发送成功，buffer要清空
		int index=p_ctrl->cur_flgAdressCtrl_No;
		if(p_ctrl->flg_address_ctrl[index].flg==1)  
		{
			p_ctrl->flg_address_ctrl[index].flg=0; //将当前buffer对应的flg置为0，表示该buffer是空的
			memset(p_ctrl->flg_address_ctrl[index].p_Adress,0,SEND_BUFFER_SIZE);  //清空buffer
		}
		
		//3.设置下一个发送buffer的编号
		p_ctrl->cur_flgAdressCtrl_No++;  //下一个
		if(p_ctrl->cur_flgAdressCtrl_No==SEND_BUFFER_QTY)
		{
			p_ctrl->cur_flgAdressCtrl_No=0;
		}
		
		for(int i=0;i<SEND_BUFFER_QTY;i++)
		{
			if(p_ctrl->flg_address_ctrl[p_ctrl->cur_flgAdressCtrl_No].flg==0)
			{
				p_ctrl->cur_flgAdressCtrl_No++;
				if(p_ctrl->cur_flgAdressCtrl_No==SEND_BUFFER_QTY)
				{
					p_ctrl->cur_flgAdressCtrl_No=0;
				}
			}
			else
			{
				break;
			}
		}
	}
	#endif
	
	#ifdef UART_SEND_PLAN_B
	//如果定义了方案B,那就不需要在回调中做任何事情
	#endif
}


//串口发送完毕的回调函数
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	UART_HandleTypeDef* p_huart;
	DATA_SEND_CTRL* p_ctrl;
	if(huart->Instance==USART1)
	{
		#ifdef UART1_ENABLE
		p_huart=&huart1;
		p_ctrl=&UART1_send_ctrl;
		#endif
	}
	else if(huart->Instance==USART2)
	{
		#ifdef UART2_ENABLE
		p_huart=&huart2;
		p_ctrl=&UART2_send_ctrl;
		#endif
	}

	else
	{
		//do nothing
	}
	
	set_controllor_after_sendCmplt(p_ctrl,p_huart);
}


//将UART收到的数据放入预处理buffer中
static void getRcvData2PreProBuf(UART_HandleTypeDef* huart,uint8_t* preProBuf,uint32_t buffer_size)
{
	uint32_t* p_prev_Index;
	uint32_t* p_cur_Index;
	uint16_t* p_rcv_length;
	uint8_t* p_rcvBuffer;
	
	//为了通用性，将if..else if..else 拆成了if...if...
	
	#ifdef UART1_ENABLE
	if(huart==&huart1)
	{
		p_prev_Index=&UART1_rcv_Ctrl.prev_Index;
		p_cur_Index=&UART1_rcv_Ctrl.cur_Index;
		p_rcv_length=&UART1_rcv_Ctrl.recv_length;
		p_rcvBuffer=g_UART1_recvBuffer;
		UART1_rcv_Ctrl.cur_Index=USART1_BUFFER_SIZE-__HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
	}
	#endif
		
	#ifdef UART2_ENABLE
	if(huart==&huart2)
	{
		p_prev_Index=&UART2_rcv_Ctrl.prev_Index;
		p_cur_Index=&UART2_rcv_Ctrl.cur_Index;
		p_rcv_length=&UART2_rcv_Ctrl.recv_length;
		p_rcvBuffer=g_UART2_recvBuffer;
		UART2_rcv_Ctrl.cur_Index=USART2_BUFFER_SIZE-__HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
	}
	#endif
		
	#ifdef UART3_ENABLE
	if(huart==&huart3)
	{
		
		p_prev_Index=&UART3_rcv_Ctrl.prev_Index;
		p_cur_Index=&UART3_rcv_Ctrl.cur_Index;
		p_rcv_length=&UART3_rcv_Ctrl.recv_length;
		p_rcvBuffer=g_UART3_recvBuffer;
		UART3_rcv_Ctrl.cur_Index=USART3_BUFFER_SIZE-__HAL_DMA_GET_COUNTER(&hdma_usart3_rx);
	}
	#endif
	
	#ifdef UART4_ENABLE
	if(huart==&huart4)
	{
		
		p_prev_Index=&g_UART4_Ctrl.prev_Index;
		p_cur_Index=&g_UART4_Ctrl.cur_Index;
		p_rcv_length=&g_UART4_Ctrl.recv_length;
		p_rcvBuffer=g_UART4_recvBuffer;
		g_UART4_Ctrl.cur_Index=USART4_BUFFER_SIZE-__HAL_DMA_GET_COUNTER(&hdma_uart4_rx);
	}
	#endif
	
	if(*p_prev_Index<*p_cur_Index)
	{
		*p_rcv_length=*p_cur_Index-*p_prev_Index;  //获取长度
		for(int i=*p_prev_Index;i<*p_cur_Index;i++)
		{
			preProBuf[i-*p_prev_Index]=p_rcvBuffer[i];
		}
	}
	else
	{
		*p_rcv_length=buffer_size-(*p_prev_Index-*p_cur_Index);
		for(int i=*p_prev_Index;i<=buffer_size;i++)
		{
			if(i==buffer_size)  //循环到头了
			{
				for(int j=0;j<*p_cur_Index;j++)
				{
					preProBuf[j+buffer_size-*p_prev_Index]=p_rcvBuffer[j];
				}
			}
			else
			{
				preProBuf[i-*p_prev_Index]=p_rcvBuffer[i];
			}
		}
	}
	*p_prev_Index=*p_cur_Index;
}


//将数据通过send控制器发送
//p_data_ctrl,控制器
//huart,UART
//其中p_data_ctrl和UART必须配对
static void send_data_by_controllor(DATA_SEND_CTRL* p_send_ctrl,UART_HandleTypeDef* p_huart)
{
	//方案A，发送需要Tx的回调函数配合一起使用
	#ifdef UART_SEND_PLAN_A
//	if((p_send_ctrl==&UART3_send_ctrl&&p_huart==&huart3)  
//		||(p_send_ctrl==&UART4_send_ctrl&&p_huart==&huart4))
	{
		if(p_send_ctrl->ready_2_send)               //如果已经准备ok，则表示可以发送数据，这个是由发送完成的回调函数设置的
		{
			#if 0
//			for(int i=0;i<SEND_BUFFER_QTY;i++)
//			{
//				if(p_send_ctrl->cur_flgAdressCtrl_No==i&&p_send_ctrl->flg_address_ctrl[i].flg==1) //查找当前要发送的数据是哪一个buffer
//				{
//					uint8_t* snd_buffer=p_send_ctrl->flg_address_ctrl[i].p_Adress;
//					uint16_t len=snd_buffer[2]+2;
//					
//					p_send_ctrl->ready_2_send=0;  //ready_2_send置为0，等待发送cmplt callback将ready_2_send置为1
//					
//					//解析发送数据，后续添加的话，在这个函数里面添加
//					ParseAndSend(p_huart,snd_buffer);
//					return;       
//				}
//			}
			#endif
			int index=p_send_ctrl->cur_flgAdressCtrl_No;
			if(p_send_ctrl->flg_address_ctrl[index].flg==1) //查找当前要发送的数据是哪一个buffer
			{
				uint8_t* snd_buffer=p_send_ctrl->flg_address_ctrl[index].p_Adress;
				uint16_t len=snd_buffer[2]+2;
				
				p_send_ctrl->ready_2_send=0;  //ready_2_send置为0，等待发送cmplt callback将ready_2_send置为1
				
				//解析发送数据，后续添加的话，在这个函数里面添加
				ParseAndSend(p_huart,snd_buffer);    
			}
		}
	}
	#endif
	
	//方案B，自己控制轮询发送
	#ifdef UART_SEND_PLAN_B
	int index=p_send_ctrl->cur_flgAdressCtrl_No;
	for(int i=0;i<SEND_BUFFER_QTY;i++)
	{
		if(p_send_ctrl->flg_address_ctrl[index].flg==1)
		{
			uint8_t* snd_buffer=p_send_ctrl->flg_address_ctrl[index].p_Adress;
			uint16_t len=snd_buffer[INDEX_LEN]+2;
			
		//p_send_ctrl->ready_2_send=0;  //方案B不需要发送控制位
			
			//数据还没发就清空标值位，是因为是在循环任务里，Xms之后进来到这个函数都认为发送完成了
			//如果不清呢，用永远认为刚刚发送的buffer仍然有数据
			//如果这个时候有人调用了saveDataTo呢？
			p_send_ctrl->flg_address_ctrl[index].flg=0; 
			
			//下一个buffer
			Move_to_Next_buffer(p_send_ctrl);
			//解析发送数据，后续添加的话，在这个函数里面添加
			ParseAndSend(p_huart,snd_buffer);   
			break;
		}
		else
		{
			//清除上一个发送buffer,防止数据污染
			uint8_t last_flgAddressCtrl_No;
			if(p_send_ctrl->cur_flgAdressCtrl_No==0)
			{
				last_flgAddressCtrl_No=SEND_BUFFER_QTY-1;
			}
			else
			{
				last_flgAddressCtrl_No=p_send_ctrl->cur_flgAdressCtrl_No-1;
			}
			if(p_send_ctrl->flg_address_ctrl[last_flgAddressCtrl_No].flg==0)   //如果上一个buffe中没数据就清一下
			{
				uint8_t* pAddresss=p_send_ctrl->flg_address_ctrl[last_flgAddressCtrl_No].p_Adress;
				memset(pAddresss,0,SEND_BUFFER_SIZE);
			}
			//下一个buffer
			Move_to_Next_buffer(p_send_ctrl);
		}
	}
	#endif
}


//保存数据,将地址为pScr，长度为len的数据放入发送控制器中
uint8_t* SaveDataTo(DATA_SEND_CTRL* p_send_ctrl,uint8_t* pScr,int len)
{
	int index=p_send_ctrl->cur_flgAdressCtrl_No;
	for(int i=0;i<SEND_BUFFER_QTY;i++)
	{
		//根据buffer的flg来判断能否存入buffer中，0表示buffer没有数据，可以使用
		//如果每个buffer的flg都是1,那么数据都无法放入
		//放入成功后立即返回
		uint8_t test[50]={0};
		if(p_send_ctrl->flg_address_ctrl[index].flg==0)   
		{
			p_send_ctrl->flg_address_ctrl[index].flg=1;                      //放入成功之后flg置为1
			memcpy(p_send_ctrl->flg_address_ctrl[index].p_Adress,pScr,len);  //将数据拷入第i个发送buffer中
			
			memcpy(test,pScr,len);
			return p_send_ctrl->flg_address_ctrl[index].p_Adress;
		}
		index++;
		if(index==SEND_BUFFER_QTY)
		{
			index=0;
		}
	}
}

static uint8_t IsCheckSumOK(uint8_t* p_Array,int len)
{
	//crc校验 LEN+DEVICE ID+Command ID+Data  ,不包含头AA 55
	uint16_t result=0;
	result=crc_16(p_Array+2,len-4);
	if((p_Array[len-1]<<8)+p_Array[len-2]==result)  //CheckSum的高位在后，低位在前
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


static void processData2Ctrollor(DATA_SEND_CTRL* p_Sndctrl,uint8_t* p_preProcessBuf)
{
	//解析接收到的数据
	//1.可能的问题，发送的数据不到完整的一帧，处理方法：丢弃
	//2.接收的数据位一帧或多帧，加残帧(不到一帧)
	//3.接收到的数据为残帧(帧头都没有的)+完成的一帧或多帧
	//完成以上容错之后，将数据放到发送控制器buffer中
	//为了通用性，将if..else if..else 拆成了if...if...
	uint8_t* p_start=p_preProcessBuf;
	UART_RCV_CTRL* pUARTRcvCtrl;
	int i=0;
	#ifdef UART1_ENABLE
	if(p_Sndctrl==&UART1_send_ctrl)
	{
		pUARTRcvCtrl=&UART1_rcv_Ctrl;
	}
	#endif
	
	#ifdef UART2_ENABLE
	if(p_Sndctrl==&UART2_send_ctrl)
	{
		pUARTRcvCtrl=&UART2_rcv_Ctrl;
	}
	#endif
	
	#ifdef UART3_ENABLE
	if(p_Sndctrl==&UART3_send_ctrl)
	{
		pUARTRcvCtrl=&UART3_rcv_Ctrl;
	}
	#endif
	
	#ifdef UART4_ENABLE
	if(p_Sndctrl==&UART4_send_ctrl)
	{
		pUARTRcvCtrl=&g_UART4_Ctrl;
	}
	#endif

		
	if(pUARTRcvCtrl->recv_length>5)  
	{
		while(1)
		{
			if(p_start[i]==0xAA&&p_start[i+1]==0x55)  //如果是AA55,表示找到了头
			{
				//开始处理数据
				int pack_len=p_start[i+2]+2;        //获取整个包的长度
				
				if(pUARTRcvCtrl->recv_length-i<pack_len)  //如果该帧不完整，不用解析了，数据不对
				{
					break;
				}
				
				//如果校验成功则保存数据到发送控制器中
				if(IsCheckSumOK(&p_start[i],pack_len))
				{
					//保存数据到send控制器中
					SaveDataTo(p_Sndctrl,&p_start[i],pack_len);
				}
				
				i+=pack_len;
			}
			else
			{
				i++;            												//如果不是AA55那么就继续找下一个字节的数据
				if(i>pUARTRcvCtrl->recv_length-2)   						//如果i接近了UART3_rcv_Ctrl.recv_length的长度，那就不用找了，数据肯定不对
				{
					break;
				}
			}
		}
	}
}


void ProcessRcvedData()
{
	#ifdef UART1_ENABLE
	if(UART1_rcv_Ctrl.recv_flg)
	{
		UART1_rcv_Ctrl.recv_flg=0;
		//1.将DMA的接收buffer中的数据放入预处理buffer中
		getRcvData2PreProBuf(&huart1,g_UART1_preProcess_Buffer,sizeof(g_UART1_preProcess_Buffer));
		//2.将预处理buffer中的数据整理(可能断帧，多帧)，放入发送控制器中
		processData2Ctrollor(&UART1_send_ctrl,g_UART1_preProcess_Buffer);    //处理数据放入发送控制器中
		//3.清空预处理buffer,准备下次的数据接收
		memset(g_UART1_preProcess_Buffer,0,sizeof(g_UART1_preProcess_Buffer));
	}
	#endif
	
	#ifdef UART2_ENABLE
	if(UART2_rcv_Ctrl.recv_flg)
	{
		UART2_rcv_Ctrl.recv_flg=0;
		//1.将DMA的接收buffer中的数据放入预处理buffer中
		getRcvData2PreProBuf(&huart2,g_UART2_preProcess_Buffer,sizeof(g_UART2_preProcess_Buffer));
		//2.将预处理buffer中的数据整理(可能断帧，多帧)，放入发送控制器中
		processData2Ctrollor(&UART2_send_ctrl,g_UART2_preProcess_Buffer);    //处理数据放入发送控制器中
		//3.清空预处理buffer,准备下次的数据接收
		memset(g_UART2_preProcess_Buffer,0,sizeof(g_UART2_preProcess_Buffer));
	}
	#endif
	
	#ifdef UART3_ENABLE
	if(UART3_rcv_Ctrl.recv_flg)
	{
		UART3_rcv_Ctrl.recv_flg=0;
		//1.将DMA的接收buffer中的数据放入预处理buffer中
		getRcvData2PreProBuf(&huart3,g_UART3_preProcess_Buffer,sizeof(g_UART3_preProcess_Buffer));
		//2.将预处理buffer中的数据整理(可能断帧，多帧)，放入发送控制器中
		processData2Ctrollor(&UART3_send_ctrl,g_UART3_preProcess_Buffer);    //处理数据放入发送控制器中
		//3.清空预处理buffer,准备下次的数据接收
		memset(g_UART3_preProcess_Buffer,0,sizeof(g_UART3_preProcess_Buffer));
	}
	#endif
	
	#ifdef UART4_ENABLE
	if(g_UART4_Ctrl.recv_flg)
	{
		g_UART4_Ctrl.recv_flg=0;
		//1.将DMA的接收buffer中的数据放入预处理buffer中
		getRcvData2PreProBuf(&huart4,g_UART4_preProcess_Buffer,sizeof(g_UART4_preProcess_Buffer));
		//2.将预处理buffer中的数据整理(可能断帧，多帧)，放入发送控制器中
		processData2Ctrollor(&UART4_send_ctrl,g_UART4_preProcess_Buffer);    //处理数据放入发送控制器中
		//3.清空预处理buffer,准备下次的数据接收
		memset(g_UART4_preProcess_Buffer,0,sizeof(g_UART4_preProcess_Buffer));
	}
	#endif
}

void sendData()
{
	#ifdef UART1_ENABLE
	if(!UART1_rcv_Ctrl.recv_flg)   //在数据接收完成之后才啊发送
	{
		send_data_by_controllor(&UART1_send_ctrl,&huart1);
	}
	#endif
	
	#ifdef UART2_ENABLE
	if(!UART2_rcv_Ctrl.recv_flg)   //在数据接收完成之后才啊发送
	{
		send_data_by_controllor(&UART2_send_ctrl,&huart2);
	}
	#endif
	
	#ifdef UART3_ENABLE
	if(!UART3_rcv_Ctrl.recv_flg)   //在数据接收完成之后才啊发送
	{
		send_data_by_controllor(&UART3_send_ctrl,&huart3);
	}
	#endif

	#ifdef UART4_ENABLE
	if(!g_UART4_Ctrl.recv_flg)
	{
		send_data_by_controllor(&UART4_send_ctrl,&huart4);
	}
	#endif
}





