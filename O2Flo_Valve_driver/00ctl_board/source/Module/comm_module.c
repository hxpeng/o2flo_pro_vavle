
#include "comm_module.h"
#include "comm_def.h"



#include <string.h>
#include <stdlib.h>



//***************************��Ҫ˵��******************************
//���ļ�ּ�����ó�ͨ���ļ����û����ù������ʵ�֣�ֻ����ñ��ĵ��ṩ��
//6��API: void uart_Init(void),
//        void ProcessRcvedData(void),
//        void sendData(void),
//        Process_UART_IDLE_INTERRUPT(UART_HandleTypeDef* huart)��
//        SaveDataTo(DATA_SEND_CTRL* p_send_ctrl,uint8_t* pScr,int len)��
//        set_checkSum(uint8_t* pArray)
//
//ʹ�ø��ļ�ǰ��׼��������
//1.��CUBE�������Լ���Ҫ�Ĵ��ڣ�RX-���ó�circular,TX-���ó�Normal
//2.׼����������һ�����ڴ��������ݣ�һ�����ڴ��ڷ�����
//
//
//���ʹ����6��API:
//1.uart_Init()��Ҫ��ϵͳ�ĳ�ʼ���е��ã�����Ŀ��init_o2flo_system()�е��ã���ʼ������
//�����ã���ʼ�����Ϳ�����(��������,malloc��Ҫ�ķ���buffer)�����տ��������򿪴���DMA���գ���������IDLE�ж�
//2.ProcessRcvedData()�ڴ��ڽ��������е���
//3.sendData()�ڴ��ڷ����е���
//4.Process_UART_IDLE_INTERRUPT��USART3_IRQHandler()�ȴ����ж��е���
//5.SaveDataTo()��Ҫ���͵����ݰ����浽��Ӧ�Ĵ��ڷ��Ϳ�����
//6.set_checkSum()����checksum
//
//ͨ��ԭ�����(�Դ���1����)��
//��������:       
//g_UART1_recvBuffer                  ��ӦDMA��fifo    
//                                         |
//                                        \|/
//g_UART1_preProcess_Buffer  ͨ������getRcvData2PreProBuf����g_UART1_recvBuffer
//                           �е����ݿ�����g_UART1_preProcess_Buffer��
//                                         |
//                                        \|/
//UART1_send_ctrl            ��g_UART1_preProcess_Buffer�е����ݽ�������
//                           ����֡�����и�(�����ж�֡),�����ʼ��������
//                            malloc������buffer��
//                                         |
//                                        \|/
//��������                   ��UART1_send_ctrl�ķ���buffer�������(CmdID,DeviceID��)
//                           ������Ӧ����(���ͻ�������),���еķ��Ϳ�����Ҫ�ص�����
//                           HAL_UART_TxCpltCallback()��ϣ��ú�������߷�������÷����ĸ�buffer
//
//��ע��ParseAndSend()�������������extern,�ú�����ʵ�ֹ������û��Լ�ʵ�֣����ļ�ֻ��Ϊͨ���ļ�
//      ����CmdID������Ӧ���ݵȲ������ĵ�����Ҫ֪��������������ֲ
//
//Note:2019/07/31,Ŀǰʹ��HAL_UART_TxCpltCallback��������һ������buffer,�������ǻ�����Ӧ��Ҫ�����
//     �յ�������ֻ�����Լ��������������κλ�Ӧ����ô�Ͳ��ᴥ��HAL_UART_TxCpltCallback���Ӷ����׻���
//     �ͻ�ʧЧ����������������Ӷ���������һ�ַ���B����������ֻ�ܷ��ͣ�����ʱ��ͷ���
//����A���ŵ�-��Tx�ص��������ƣ�ȷ��ÿһ֡������֮������buffer��flg�����buffer���߼��Ϸǳ���
//       ȱ��-������յ�֡����Ҫ��Ӧ����ô�Ͳ��ᴥ��Tx�ص���������ƾͳ�������
//����B: �ŵ�-�˷��˷���A��ȱ��
//       ȱ��-�ڷ��ͺ����У�һ���뺯���ͽ�buffer��flg��0,��ʵ���ʱ��DMA���ڷ�����,������ʱ����յ�������
//            ��ô�ͻᴥ��saveDataTo�����������ڷ��͵�buffer������װ�����ˣ�������buffer��flg��־λ��������
//            ���������Ҫ�úÿ��Ʒ��ͣ�����ʱ��Ҫ�Լ����
//
//ע�⣺static �����в�Ҫдstatic����
//*****************************************************************

//�������ù�ϵ
// USART3_IRQHandler()
// {
//   Process_UART_IDLE_INTERRUPT(&huart3); 
//   {
//     if(RESET != __HAL_UART_GET_FLAG(p_huart, UART_FLAG_IDLE))   //�ж��Ƿ��ǿ����ж�
//	   {
//		   __HAL_UART_CLEAR_IDLEFLAG(p_huart);                       //��������жϱ�־�������һֱ���Ͻ����жϣ�
//
//		   USAR_UART_IDLECallback(p_huart);                          //�����жϴ�����
//       {
//         UART3_rcv_Ctrl.recv_flg=1;                              //�ñ�־λ
//       }
//     }
//   }
// }
//
//���ڽ�������20msһ��
//void DATA_receive_task(const void* arg)
//{
//	//������յ�������
//	ProcessRcvedData();
//  {
//    if(UART3_rcv_Ctrl.recv_flg)
//		{
//			UART3_rcv_Ctrl.recv_flg=0;
//			//1.��DMA�Ľ���buffer�е����ݷ���Ԥ����buffer��
//			getRcvData2PreProBuf(&huart3,g_UART3_preProcess_Buffer,sizeof(g_UART3_preProcess_Buffer));
//			//2.��Ԥ����buffer�е���������(���ܶ�֡����֡)�����뷢�Ϳ�������
//			processData2Ctrollor(&UART3_send_ctrl,g_UART3_preProcess_Buffer);    //�������ݷ��뷢�Ϳ�������
//			{
//        ...
//
//				SaveDataTo(p_Sndctrl,&p_start[i],pack_len);   //�����ݱ��浽�������Ķ���buffer��
//        
//        ...
//			}
//			//3.���Ԥ����buffer,׼���´ε����ݽ���
//			memset(g_UART3_preProcess_Buffer,0,sizeof(g_UART3_preProcess_Buffer));
//		}
//  }
//}
//
//
////���ڷ�������,20msһ��
//void DATA_send_task(const void* arg)
//{
//	sendData();
//	{
//		if(!UART3_rcv_Ctrl.recv_flg)   //�����ݽ������֮��Ű�����
//		{
//			send_data_by_controllor(&UART3_send_ctrl,&huart3);
//		}
//	}
//}


//�ú�����������ŵ�ͨ��������ļ��У����û��Լ�����
extern void ParseAndSend(UART_HandleTypeDef* p_huart,uint8_t* pArray);

#ifdef UART1_ENABLE
//����3�����崮�ڽ��պͷ�������buffer�ĳ���
extern DMA_HandleTypeDef hdma_usart1_rx;
extern UART_HandleTypeDef huart1;
uint8_t g_UART1_recvBuffer[USART1_BUFFER_SIZE]={0};
uint8_t g_UART1_preProcess_Buffer[USART1_BUFFER_SIZE]={0};
UART_RCV_CTRL UART1_rcv_Ctrl={0};
DATA_SEND_CTRL UART1_send_ctrl;
#endif

#ifdef UART2_ENABLE
//����3�����崮�ڽ��պͷ�������buffer�ĳ���
extern DMA_HandleTypeDef hdma_usart2_rx;
extern UART_HandleTypeDef huart2;
uint8_t g_UART2_recvBuffer[USART2_BUFFER_SIZE]={0};
uint8_t g_UART2_preProcess_Buffer[USART2_BUFFER_SIZE]={0};
UART_RCV_CTRL UART2_rcv_Ctrl={0};
DATA_SEND_CTRL UART2_send_ctrl;
#endif


#ifdef UART3_ENABLE
//����3�����崮�ڽ��պͷ�������buffer�ĳ���
extern DMA_HandleTypeDef hdma_usart3_rx;
extern UART_HandleTypeDef huart3;
uint8_t g_UART3_recvBuffer[USART3_BUFFER_SIZE]={0};
uint8_t g_UART3_preProcess_Buffer[USART3_BUFFER_SIZE]={0};
UART_RCV_CTRL UART3_rcv_Ctrl={0};
DATA_SEND_CTRL UART3_send_ctrl;
#endif

#ifdef UART4_ENABLE
//����4�����崮�ڽ��պͷ�������buffer�ĳ���
extern DMA_HandleTypeDef hdma_uart4_rx;
extern UART_HandleTypeDef huart4;
uint8_t g_UART4_recvBuffer[USART4_BUFFER_SIZE]={0};
uint8_t g_UART4_preProcess_Buffer[USART4_BUFFER_SIZE]={0};
UART_RCV_CTRL g_UART4_Ctrl={0};
DATA_SEND_CTRL UART4_send_ctrl;
#endif



//CRC��
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

//��ȡCRCֵ
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


//��ָ����Array���checksum
void set_checkSum(uint8_t* pArray)
{
	if(pArray[0]==HEAD_MARK0&&pArray[1]==HEAD_MARK1)   //���ͷ��0xAA55
	{
		uint16_t len=pArray[INDEX_LEN];              //��ȡ֡�ĳ���
		uint16_t crc16=crc_16(pArray+INDEX_LEN,len-2);
		pArray[len]=crc16%256;               //CheckSum1          
		pArray[len+1]=crc16/256;             //CheckSum2
	}
}
//��ʼ������3������4�շ�buffer���趨
void uart_Init()
{
	#ifdef UART1_ENABLE
	//UART1 ���Ϳ��Ƴ�ʼ��
	UART1_send_ctrl.ready_2_send=1;             //1��ʾ���Է���
	UART1_send_ctrl.cur_flgAdressCtrl_No=0;     //��ǰflgAdress�������ı�ţ�Ŀǰ��3����0��1��2

	for(int i=0;i<SEND_BUFFER_QTY;i++)  //�ڶ������3��buffer��ÿ��buffer50�ֽ�
	{
		UART1_send_ctrl.flg_address_ctrl[i].flg=0;
		UART1_send_ctrl.flg_address_ctrl[i].p_Adress=malloc(SEND_BUFFER_SIZE);
	}
	
	//UART1 ���տ���
	UART1_rcv_Ctrl.cur_Index=0;
	UART1_rcv_Ctrl.prev_Index=0;
	UART1_rcv_Ctrl.recv_flg=0;
	UART1_rcv_Ctrl.recv_length=0;
	
	HAL_UART_Receive_DMA(&huart1,g_UART1_recvBuffer,sizeof(g_UART1_recvBuffer));
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);              //��������1��IDLE�ж�
	#endif
	
	#ifdef UART2_ENABLE
	//UART2 ���Ϳ��Ƴ�ʼ��
	UART2_send_ctrl.ready_2_send=1;             //1��ʾ���Է���
	UART2_send_ctrl.cur_flgAdressCtrl_No=0;     //��ǰflgAdress�������ı�ţ�Ŀǰ��3����0��1��2

	for(int i=0;i<SEND_BUFFER_QTY;i++)  //�ڶ������3��buffer��ÿ��buffer50�ֽ�
	{
		UART2_send_ctrl.flg_address_ctrl[i].flg=0;
		UART2_send_ctrl.flg_address_ctrl[i].p_Adress=malloc(SEND_BUFFER_SIZE);
	}
	
	//UART2 ���տ���
	UART2_rcv_Ctrl.cur_Index=0;
	UART2_rcv_Ctrl.prev_Index=0;
	UART2_rcv_Ctrl.recv_flg=0;
	UART2_rcv_Ctrl.recv_length=0;
	
	HAL_UART_Receive_DMA(&huart2,g_UART2_recvBuffer,sizeof(g_UART2_recvBuffer));
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);              //��������1��IDLE�ж�
	#endif
	
	
	#ifdef UART3_ENABLE
	//UART3 ���Ϳ��Ƴ�ʼ��
	UART3_send_ctrl.ready_2_send=1;             //1��ʾ���Է���
	UART3_send_ctrl.cur_flgAdressCtrl_No=0;     //��ǰflgAdress�������ı�ţ�Ŀǰ��3����0��1��2

	for(int i=0;i<SEND_BUFFER_QTY;i++)  //�ڶ������3��buffer��ÿ��buffer50�ֽ�
	{
		UART3_send_ctrl.flg_address_ctrl[i].flg=0;
		UART3_send_ctrl.flg_address_ctrl[i].p_Adress=malloc(SEND_BUFFER_SIZE);
	}
	
	//UART3 ���տ���
	UART3_rcv_Ctrl.cur_Index=0;
	UART3_rcv_Ctrl.prev_Index=0;
	UART3_rcv_Ctrl.recv_flg=0;
	UART3_rcv_Ctrl.recv_length=0;
	
	HAL_UART_Receive_DMA(&huart3,g_UART3_recvBuffer,sizeof(g_UART3_recvBuffer));
	__HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);              //��������3��IDLE�ж�
	#endif
	
	#ifdef UART4_ENABLE
	//UART4 ���Ϳ��Ƴ�ʼ��  
	UART4_send_ctrl.ready_2_send=1;             //1��ʾ���Է���
	UART4_send_ctrl.cur_flgAdressCtrl_No=0;     //��ǰflgAdress�������ı�ţ�Ŀǰ��3����0��1��2

	for(int i=0;i<SEND_BUFFER_QTY;i++)  //�ڶ������3��buffer��ÿ��buffer50�ֽ�
	{
		UART4_send_ctrl.flg_address_ctrl[i].flg=0;
		UART4_send_ctrl.flg_address_ctrl[i].p_Adress=malloc(SEND_BUFFER_SIZE);
	} 
	
	//UART4 ���տ���
	g_UART4_Ctrl.cur_Index=0;
	g_UART4_Ctrl.prev_Index=0;
	g_UART4_Ctrl.recv_flg=0;
	g_UART4_Ctrl.recv_length=0;
	
	HAL_UART_Receive_DMA(&huart4,g_UART4_recvBuffer,sizeof(g_UART4_recvBuffer));
	__HAL_UART_ENABLE_IT(&huart4, UART_IT_IDLE);              //��������4��IDLE�ж�
	#endif
		
}


//uint32_t last_remain=USART4_BUFFER_SIZE;
//uint32_t current_remain=0;
//����IDLE�жϵĻص�����
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

//����IDLE�жϴ�����
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

	if(RESET != __HAL_UART_GET_FLAG(p_huart, UART_FLAG_IDLE))   //�ж��Ƿ��ǿ����ж�
	{
		__HAL_UART_CLEAR_IDLEFLAG(p_huart);                       //��������жϱ�־�������һֱ���Ͻ����жϣ�

		USAR_UART_IDLECallback(p_huart);                          //�����жϴ�����
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


//��DMA���ݷ��ͳɹ����ص������÷��Ϳ�����
static void set_controllor_after_sendCmplt(DATA_SEND_CTRL* p_send_ctrl ,UART_HandleTypeDef *p_huart)
{
	//������˷���A��������Ҫ�ص�����������
	#ifdef UART_SEND_PLAN_A
//	if((p_send_ctrl==&UART3_send_ctrl&&p_huart==&huart3)   //ȷ������Ĳ�������ȷ��
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
		
		//1.��ready_2_send��Ϊ1
		p_ctrl->ready_2_send=1;
		//2.����buffer��Ӧ��flg��Ϊ0����ʾ��buffer�����Ѿ����ͳɹ���bufferҪ���
		int index=p_ctrl->cur_flgAdressCtrl_No;
		if(p_ctrl->flg_address_ctrl[index].flg==1)  
		{
			p_ctrl->flg_address_ctrl[index].flg=0; //����ǰbuffer��Ӧ��flg��Ϊ0����ʾ��buffer�ǿյ�
			memset(p_ctrl->flg_address_ctrl[index].p_Adress,0,SEND_BUFFER_SIZE);  //���buffer
		}
		
		//3.������һ������buffer�ı��
		p_ctrl->cur_flgAdressCtrl_No++;  //��һ��
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
	//��������˷���B,�ǾͲ���Ҫ�ڻص������κ�����
	#endif
}


//���ڷ�����ϵĻص�����
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


//��UART�յ������ݷ���Ԥ����buffer��
static void getRcvData2PreProBuf(UART_HandleTypeDef* huart,uint8_t* preProBuf,uint32_t buffer_size)
{
	uint32_t* p_prev_Index;
	uint32_t* p_cur_Index;
	uint16_t* p_rcv_length;
	uint8_t* p_rcvBuffer;
	
	//Ϊ��ͨ���ԣ���if..else if..else �����if...if...
	
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
		*p_rcv_length=*p_cur_Index-*p_prev_Index;  //��ȡ����
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
			if(i==buffer_size)  //ѭ����ͷ��
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


//������ͨ��send����������
//p_data_ctrl,������
//huart,UART
//����p_data_ctrl��UART�������
static void send_data_by_controllor(DATA_SEND_CTRL* p_send_ctrl,UART_HandleTypeDef* p_huart)
{
	//����A��������ҪTx�Ļص��������һ��ʹ��
	#ifdef UART_SEND_PLAN_A
//	if((p_send_ctrl==&UART3_send_ctrl&&p_huart==&huart3)  
//		||(p_send_ctrl==&UART4_send_ctrl&&p_huart==&huart4))
	{
		if(p_send_ctrl->ready_2_send)               //����Ѿ�׼��ok�����ʾ���Է������ݣ�������ɷ�����ɵĻص��������õ�
		{
			#if 0
//			for(int i=0;i<SEND_BUFFER_QTY;i++)
//			{
//				if(p_send_ctrl->cur_flgAdressCtrl_No==i&&p_send_ctrl->flg_address_ctrl[i].flg==1) //���ҵ�ǰҪ���͵���������һ��buffer
//				{
//					uint8_t* snd_buffer=p_send_ctrl->flg_address_ctrl[i].p_Adress;
//					uint16_t len=snd_buffer[2]+2;
//					
//					p_send_ctrl->ready_2_send=0;  //ready_2_send��Ϊ0���ȴ�����cmplt callback��ready_2_send��Ϊ1
//					
//					//�����������ݣ�������ӵĻ�������������������
//					ParseAndSend(p_huart,snd_buffer);
//					return;       
//				}
//			}
			#endif
			int index=p_send_ctrl->cur_flgAdressCtrl_No;
			if(p_send_ctrl->flg_address_ctrl[index].flg==1) //���ҵ�ǰҪ���͵���������һ��buffer
			{
				uint8_t* snd_buffer=p_send_ctrl->flg_address_ctrl[index].p_Adress;
				uint16_t len=snd_buffer[2]+2;
				
				p_send_ctrl->ready_2_send=0;  //ready_2_send��Ϊ0���ȴ�����cmplt callback��ready_2_send��Ϊ1
				
				//�����������ݣ�������ӵĻ�������������������
				ParseAndSend(p_huart,snd_buffer);    
			}
		}
	}
	#endif
	
	//����B���Լ�������ѯ����
	#ifdef UART_SEND_PLAN_B
	int index=p_send_ctrl->cur_flgAdressCtrl_No;
	for(int i=0;i<SEND_BUFFER_QTY;i++)
	{
		if(p_send_ctrl->flg_address_ctrl[index].flg==1)
		{
			uint8_t* snd_buffer=p_send_ctrl->flg_address_ctrl[index].p_Adress;
			uint16_t len=snd_buffer[INDEX_LEN]+2;
			
		//p_send_ctrl->ready_2_send=0;  //����B����Ҫ���Ϳ���λ
			
			//���ݻ�û������ձ�ֵλ������Ϊ����ѭ�������Xms֮������������������Ϊ���������
			//��������أ�����Զ��Ϊ�ոշ��͵�buffer��Ȼ������
			//������ʱ�����˵�����saveDataTo�أ�
			p_send_ctrl->flg_address_ctrl[index].flg=0; 
			
			//��һ��buffer
			Move_to_Next_buffer(p_send_ctrl);
			//�����������ݣ�������ӵĻ�������������������
			ParseAndSend(p_huart,snd_buffer);   
			break;
		}
		else
		{
			//�����һ������buffer,��ֹ������Ⱦ
			uint8_t last_flgAddressCtrl_No;
			if(p_send_ctrl->cur_flgAdressCtrl_No==0)
			{
				last_flgAddressCtrl_No=SEND_BUFFER_QTY-1;
			}
			else
			{
				last_flgAddressCtrl_No=p_send_ctrl->cur_flgAdressCtrl_No-1;
			}
			if(p_send_ctrl->flg_address_ctrl[last_flgAddressCtrl_No].flg==0)   //�����һ��buffe��û���ݾ���һ��
			{
				uint8_t* pAddresss=p_send_ctrl->flg_address_ctrl[last_flgAddressCtrl_No].p_Adress;
				memset(pAddresss,0,SEND_BUFFER_SIZE);
			}
			//��һ��buffer
			Move_to_Next_buffer(p_send_ctrl);
		}
	}
	#endif
}


//��������,����ַΪpScr������Ϊlen�����ݷ��뷢�Ϳ�������
uint8_t* SaveDataTo(DATA_SEND_CTRL* p_send_ctrl,uint8_t* pScr,int len)
{
	int index=p_send_ctrl->cur_flgAdressCtrl_No;
	for(int i=0;i<SEND_BUFFER_QTY;i++)
	{
		//����buffer��flg���ж��ܷ����buffer�У�0��ʾbufferû�����ݣ�����ʹ��
		//���ÿ��buffer��flg����1,��ô���ݶ��޷�����
		//����ɹ�����������
		uint8_t test[50]={0};
		if(p_send_ctrl->flg_address_ctrl[index].flg==0)   
		{
			p_send_ctrl->flg_address_ctrl[index].flg=1;                      //����ɹ�֮��flg��Ϊ1
			memcpy(p_send_ctrl->flg_address_ctrl[index].p_Adress,pScr,len);  //�����ݿ����i������buffer��
			
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
	//crcУ�� LEN+DEVICE ID+Command ID+Data  ,������ͷAA 55
	uint16_t result=0;
	result=crc_16(p_Array+2,len-4);
	if((p_Array[len-1]<<8)+p_Array[len-2]==result)  //CheckSum�ĸ�λ�ں󣬵�λ��ǰ
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
	//�������յ�������
	//1.���ܵ����⣬���͵����ݲ���������һ֡��������������
	//2.���յ�����λһ֡���֡���Ӳ�֡(����һ֡)
	//3.���յ�������Ϊ��֡(֡ͷ��û�е�)+��ɵ�һ֡���֡
	//��������ݴ�֮�󣬽����ݷŵ����Ϳ�����buffer��
	//Ϊ��ͨ���ԣ���if..else if..else �����if...if...
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
			if(p_start[i]==0xAA&&p_start[i+1]==0x55)  //�����AA55,��ʾ�ҵ���ͷ
			{
				//��ʼ��������
				int pack_len=p_start[i+2]+2;        //��ȡ�������ĳ���
				
				if(pUARTRcvCtrl->recv_length-i<pack_len)  //�����֡�����������ý����ˣ����ݲ���
				{
					break;
				}
				
				//���У��ɹ��򱣴����ݵ����Ϳ�������
				if(IsCheckSumOK(&p_start[i],pack_len))
				{
					//�������ݵ�send��������
					SaveDataTo(p_Sndctrl,&p_start[i],pack_len);
				}
				
				i+=pack_len;
			}
			else
			{
				i++;            												//�������AA55��ô�ͼ�������һ���ֽڵ�����
				if(i>pUARTRcvCtrl->recv_length-2)   						//���i�ӽ���UART3_rcv_Ctrl.recv_length�ĳ��ȣ��ǾͲ������ˣ����ݿ϶�����
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
		//1.��DMA�Ľ���buffer�е����ݷ���Ԥ����buffer��
		getRcvData2PreProBuf(&huart1,g_UART1_preProcess_Buffer,sizeof(g_UART1_preProcess_Buffer));
		//2.��Ԥ����buffer�е���������(���ܶ�֡����֡)�����뷢�Ϳ�������
		processData2Ctrollor(&UART1_send_ctrl,g_UART1_preProcess_Buffer);    //�������ݷ��뷢�Ϳ�������
		//3.���Ԥ����buffer,׼���´ε����ݽ���
		memset(g_UART1_preProcess_Buffer,0,sizeof(g_UART1_preProcess_Buffer));
	}
	#endif
	
	#ifdef UART2_ENABLE
	if(UART2_rcv_Ctrl.recv_flg)
	{
		UART2_rcv_Ctrl.recv_flg=0;
		//1.��DMA�Ľ���buffer�е����ݷ���Ԥ����buffer��
		getRcvData2PreProBuf(&huart2,g_UART2_preProcess_Buffer,sizeof(g_UART2_preProcess_Buffer));
		//2.��Ԥ����buffer�е���������(���ܶ�֡����֡)�����뷢�Ϳ�������
		processData2Ctrollor(&UART2_send_ctrl,g_UART2_preProcess_Buffer);    //�������ݷ��뷢�Ϳ�������
		//3.���Ԥ����buffer,׼���´ε����ݽ���
		memset(g_UART2_preProcess_Buffer,0,sizeof(g_UART2_preProcess_Buffer));
	}
	#endif
	
	#ifdef UART3_ENABLE
	if(UART3_rcv_Ctrl.recv_flg)
	{
		UART3_rcv_Ctrl.recv_flg=0;
		//1.��DMA�Ľ���buffer�е����ݷ���Ԥ����buffer��
		getRcvData2PreProBuf(&huart3,g_UART3_preProcess_Buffer,sizeof(g_UART3_preProcess_Buffer));
		//2.��Ԥ����buffer�е���������(���ܶ�֡����֡)�����뷢�Ϳ�������
		processData2Ctrollor(&UART3_send_ctrl,g_UART3_preProcess_Buffer);    //�������ݷ��뷢�Ϳ�������
		//3.���Ԥ����buffer,׼���´ε����ݽ���
		memset(g_UART3_preProcess_Buffer,0,sizeof(g_UART3_preProcess_Buffer));
	}
	#endif
	
	#ifdef UART4_ENABLE
	if(g_UART4_Ctrl.recv_flg)
	{
		g_UART4_Ctrl.recv_flg=0;
		//1.��DMA�Ľ���buffer�е����ݷ���Ԥ����buffer��
		getRcvData2PreProBuf(&huart4,g_UART4_preProcess_Buffer,sizeof(g_UART4_preProcess_Buffer));
		//2.��Ԥ����buffer�е���������(���ܶ�֡����֡)�����뷢�Ϳ�������
		processData2Ctrollor(&UART4_send_ctrl,g_UART4_preProcess_Buffer);    //�������ݷ��뷢�Ϳ�������
		//3.���Ԥ����buffer,׼���´ε����ݽ���
		memset(g_UART4_preProcess_Buffer,0,sizeof(g_UART4_preProcess_Buffer));
	}
	#endif
}

void sendData()
{
	#ifdef UART1_ENABLE
	if(!UART1_rcv_Ctrl.recv_flg)   //�����ݽ������֮��Ű�����
	{
		send_data_by_controllor(&UART1_send_ctrl,&huart1);
	}
	#endif
	
	#ifdef UART2_ENABLE
	if(!UART2_rcv_Ctrl.recv_flg)   //�����ݽ������֮��Ű�����
	{
		send_data_by_controllor(&UART2_send_ctrl,&huart2);
	}
	#endif
	
	#ifdef UART3_ENABLE
	if(!UART3_rcv_Ctrl.recv_flg)   //�����ݽ������֮��Ű�����
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





