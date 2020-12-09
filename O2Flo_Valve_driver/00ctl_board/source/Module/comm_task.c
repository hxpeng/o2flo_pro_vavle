#include "comm_task.h"
#include "main.h"
#include "stm32f0xx_hal.h"
#include "cmsis_os.h"

#include "string.h"

#include "CMD_Receive.h"
#include "datatype.h"
#include "fifo.h"
#include "main.h"
#include "comm_module.h"
#include "comm_def.h"
#include "flow_module.h"
#include "sampling_module.h"
#include "flow_module.h" 
#include "cmsis_armcc.h"


/* ���� ----------------------------------------------------------------------*/
static CMD_Receive _com2_CmdReceive;  // ������տ��ƶ���

FIFO_TYPE com2_tx_fifo;
extern FLOW_MODULE flow_module;
static UINT8 _com2_fifo_buffer[TX_FIFO_LEN];
/* �������� ------------------------------------------------------------------*/

/* �������� ------------------------------------------------------------------*/
/******************************************************
*�ڲ���������
******************************************************/

//--------------------------��������-----------------------------//
static BOOL ModuleUnPackFrame(void);
static BOOL ModuleProcessPacket(UINT8 *pData);
//static UINT8 CheckCheckSum(UINT8* pData, UINT8 nLen);

extern SAMPLING_MODULE sampling_module;

void send_fio2_flow_data(void);

//���巢�ͼ��200ms
#define SEND_INTERVAL  200
static uint8_t send_loop_cnt=0;
VALVE_PARA valve_para={0};
STE_PARA set_runState_Flow={0};
RUN_STATE runing_state={0};
SET_BAUD_RATE set_baud_rate={0};
uint32_t band_rate = 115200;
const uint16_t soft_versions = 2;//2020-8-31
uint16_t hardware_versions = 1;
extern uint8_t out_flash;



uint8_t ackerror = 0;

uint8_t uart_flag = 0;
void send_valve_data_2_PC();



//��ʼ��
void comm_module_init(void)
{
	uart_Init();
}

void CalcCheckSum(UINT8* pPacket)
{
	set_checkSum(pPacket);
}

void send_modify_PID_result(u8 result)
{
//	u8 buffer[7];    
//	u16 tx_len;
//	u8 com2_txbuffer[TX_FIFO_LEN];	
//	
//	buffer[0] = PACK_HEAD_BYTE;
//	buffer[1] = 7-2;
//	buffer[2] = MODULE_CMD_TYPE;     //0x00
//	buffer[3] = SLAVER_SEND_MODIFY_PID_SUCCESS;    //0xAB
//	

//	buffer[4] = result;      //result=0����ʾdisable�ɹ�(��ԭ��PID);result=1,��ʾ�޸�PID�ɹ�
//	 
//	CalcCheckSum(buffer);
//	SaveDataTo(&UART2_send_ctrl,buffer,
	
	int FIXED_LEN1=5;//5=AA+55+len+deviceID+cmdID(һ��5���ֽ�)
	int FIXED_LEN2=7;//7=FIXED_LEN1+2(checksum)

	int pack_len=FIXED_LEN2+1;       
	uint8_t buffer[pack_len];
	buffer[INDEX_HEAD0]=HEAD_MARK0;
	buffer[INDEX_HEAD1]=HEAD_MARK1;
	
	buffer[INDEX_LEN]=FIXED_LEN1+1;   
	buffer[INDEX_DEVICE_ID]=DEVICE_ID_VALVE_CTRL;
	buffer[INDEX_CMD_ID]=SLAVER_SEND_MODIFY_PID_SUCCESS;  //����״̬&����
	
	//�������
	buffer[5]    =result;

	set_checkSum(buffer);  //����checksum
	SaveDataTo(&UART2_send_ctrl,buffer,pack_len);
		
}

static void Process_PC_Msg(UART_HandleTypeDef* p_huart,uint8_t* pArray)
{
//	#ifdef DEBUG_FOR_VALVE_CUREVE
	uint8_t cmdID=pArray[INDEX_CMD_ID];
	switch(cmdID)
	{
		case HOST_SEND_SETTING_FLOW:
			set_flow_parameter(pArray);
			break;
		case HOST_SEND_MODIFY_PID:
			set_pid(pArray);
			break;
		case HOSET_SEND_GET_PID:
			send_pid_2_PC(pArray);
			break;
		case HOST_SEND_MANUAL_SET_PWM_DC:
			set_pwm_dc(pArray,HOST_SEND_MANUAL_SET_PWM_DC);
			break;
		case HOST_SEND_TEST_VALVE_CURVE:
			set_pwm_dc(pArray,HOST_SEND_TEST_VALVE_CURVE);
			break;
		case HOST_GET_DATA:
			send_valve_data_2_PC();
			break;
		default:
			break;
	}
//	#endif
}

static void Process_MLB_Msg(UART_HandleTypeDef* p_huart,uint8_t* pArray)
{
	//do nothing
}

static uint8_t usart_buff[20];
void saveRealPara2SndBuffer(VALVE_PARA* p_valve_para,UART_HandleTypeDef* p_huart)
{
	if(p_huart==&huart2)
	{
		int FIXED_LEN1=5;//5=AA+55+len+deviceID+cmdID(һ��5���ֽ�)
		int FIXED_LEN2=7;//7=FIXED_LEN1+2(checksum)
		
		//��"��ȡ����״̬"֡�����浽���Ϳ������ķ���buffer��
		int pack_len=FIXED_LEN2+10;       //0��ʾû���������,�û�������Ҫ�Լ�����
		uint8_t buffer[pack_len];
		buffer[INDEX_HEAD0]=HEAD_MARK0;
		buffer[INDEX_HEAD1]=HEAD_MARK1;
		
		buffer[INDEX_LEN]=FIXED_LEN1+10;   //0��ʾû���������,�û�������Ҫ�Լ�����
		buffer[INDEX_DEVICE_ID]=DEVICE_ID_VALVE_CTRL;
		buffer[INDEX_CMD_ID]=CMD_VALVECTRL_ACK_REAL_PARA;
		
		//������
		buffer[INDEX_ValvePara_O2_FLOW_L]  =p_valve_para->o2_flow%256;  //ʵʱ��������ȡ����ʱ��Ϊ0xfffe
		buffer[INDEX_ValvePara_O2_FLOW_H]  =p_valve_para->o2_flow/256;
		buffer[INDEX_ValvePara_VOLT_12V]   =p_valve_para->valveBoard_12V;
		buffer[INDEX_ValvePara_VOLT_5V]    =p_valve_para->valveBoard_5V;
		buffer[INDEX_ValvePara_CURRENT_L]  =p_valve_para->valve_current%256;
		buffer[INDEX_ValvePara_CURRENT_H]  =p_valve_para->valve_current/256;
		buffer[INDEX_ValvePara_PWM_DC_L]   =p_valve_para->valve_PWM_DC%256;
		buffer[INDEX_ValvePara_PWM_DC_H]   =p_valve_para->valve_PWM_DC/256;
		buffer[INDEX_ValvePara_TEMP_L]   =p_valve_para->temp%256;
		buffer[INDEX_ValvePara_TEMP_H]   =p_valve_para->temp/256;

		if(ackerror == 1)
		{
			buffer[INDEX_ValvePara_O2_FLOW_L]  = 0xfe;
			buffer[INDEX_ValvePara_O2_FLOW_H]  = 0xff;
		}
		
		set_checkSum(buffer);  //����checksum
		memcpy(usart_buff,buffer,pack_len);
		SaveDataTo(&UART2_send_ctrl,buffer,pack_len);
	}
}

void saveAck2SndBuffer( )
{
	int FIXED_LEN1=5;//5=AA+55+len+deviceID+cmdID(һ��5���ֽ�)
	int FIXED_LEN2=7;//7=FIXED_LEN1+2(checksum)
	
	int pack_len=FIXED_LEN2+1;       //0��ʾû���������,�û�������Ҫ�Լ�����
	uint8_t buffer[pack_len];
	buffer[INDEX_HEAD0]=HEAD_MARK0;
	buffer[INDEX_HEAD1]=HEAD_MARK1;
	
	buffer[INDEX_LEN]=FIXED_LEN1+1;   //0��ʾû���������,�û�������Ҫ�Լ�����
	buffer[INDEX_DEVICE_ID]=DEVICE_ID_VALVE_CTRL;
	buffer[INDEX_CMD_ID]=CMD_VALVECTRL_ACK_SET_PARA;
	

	//������
	buffer[5]  =1;   //��ӦACK

	set_checkSum(buffer);  //����checksum
	SaveDataTo(&UART2_send_ctrl,buffer,pack_len);
}


void self_checking()
{
	int FIXED_LEN1=5;//5=AA+55+len+deviceID+cmdID(һ��5���ֽ�)
	int FIXED_LEN2=7;//7=FIXED_LEN1+2(checksum)
	
	//��"��ȡ����״̬"֡�����浽���Ϳ������ķ���buffer��
	int pack_len=FIXED_LEN2+8;       //0��ʾû���������,�û�������Ҫ�Լ�����
	uint8_t buffer[pack_len];
	buffer[INDEX_HEAD0]=HEAD_MARK0;
	buffer[INDEX_HEAD1]=HEAD_MARK1;
	
	buffer[INDEX_LEN]=FIXED_LEN1+8;   //0��ʾû���������,�û�������Ҫ�Լ�����
	buffer[INDEX_DEVICE_ID]=DEVICE_ID_VALVE_CTRL;
	buffer[INDEX_CMD_ID]=CMD_VALVECTRL_ACK_SELFCHECK;
	
	//������,�����ݣ������
	buffer[5]  =1;
	buffer[6]  =out_flash;
	buffer[7]  =0;   //2
	buffer[8]  =0;   //3
	buffer[9]  =0;   //4
	buffer[10]  =0;   //5
	buffer[11]  =0;   //6
	buffer[12]  =0;   //7

	set_checkSum(buffer);  //����checksum
	SaveDataTo(&UART2_send_ctrl,buffer,pack_len);
}

void saveBaudSet2SndBuffer(SET_BAUD_RATE* p_set_baud_rate,uint8_t* pArray)
{
	#if 0
	if(p_huart!=&huart2)
	{
		return;
	}
	#endif

	int FIXED_LEN1=5;//5=AA+55+len+deviceID+cmdID(һ��5���ֽ�)
	int FIXED_LEN2=7;//7=FIXED_LEN1+2(checksum)
	
	int pack_len=FIXED_LEN2+1;       //0��ʾû���������,�û�������Ҫ�Լ�����
	uint8_t buffer[pack_len];
	buffer[INDEX_HEAD0]=HEAD_MARK0;
	buffer[INDEX_HEAD1]=HEAD_MARK1;
	
	buffer[INDEX_LEN]=FIXED_LEN1+1;   //0��ʾû���������,�û�������Ҫ�Լ�����
	buffer[INDEX_DEVICE_ID]=DEVICE_ID_DRIVER;
	buffer[INDEX_CMD_ID]=CMD_VALVECTRL_ACK_SET_BAUD_RATE;
	

	//������
	buffer[5]  =1;

	p_set_baud_rate->set_baud_rate = pArray[INDEX_SET_BAUD_RATE_H]*256 + pArray[INDEX_SET_BAUD_RATE_L];
	band_rate = pArray[INDEX_SET_BAUD_RATE_H]*256 + pArray[INDEX_SET_BAUD_RATE_L];

	set_checkSum(buffer);  //����checksum
	SaveDataTo(&UART2_send_ctrl,buffer,pack_len);
}

static void getSetRunstateFlow(RUN_STATE* p_RunState_Flow,uint8_t* pArray)
{
	p_RunState_Flow->runing_state=pArray[INDEX_RUN_STATE];
	p_RunState_Flow->set_o2_flow=pArray[INDEX_SET_O2_FLOW_L]+256*pArray[INDEX_SET_O2_FLOW_H];
	
	flow_module.flow_lpm_set=p_RunState_Flow->set_o2_flow;

	//set_flow();
	
	flow_module.flow_lpm_rt=0;
	flow_module.flow_slpm_rt=0;
	flow_module.flow_slpm=0;

}

void saveVersion2SndBuffer( )
{
	int FIXED_LEN1=5;//5=AA+55+len+deviceID+cmdID(һ��5���ֽ�)
	int FIXED_LEN2=7;//7=FIXED_LEN1+2(checksum)
	
	int pack_len=FIXED_LEN2+4;       //0��ʾû���������,�û�������Ҫ�Լ�����
	uint8_t buffer[pack_len];
	buffer[INDEX_HEAD0]=HEAD_MARK0;
	buffer[INDEX_HEAD1]=HEAD_MARK1;
	
	buffer[INDEX_LEN]=FIXED_LEN1+4;   //0��ʾû���������,�û�������Ҫ�Լ�����
	buffer[INDEX_DEVICE_ID]=DEVICE_ID_VALVE_CTRL;
	buffer[INDEX_CMD_ID]=CMD_VALVECTRL_ACK_SOFTWARE_VER;
	

	//������
	buffer[5] = soft_versions%256;
	buffer[6] = soft_versions/256;
	buffer[7] = hardware_versions%256;
	buffer[8] = hardware_versions/256;

	set_checkSum(buffer);  //����checksum
	SaveDataTo(&UART2_send_ctrl,buffer,pack_len);
}

void saveVersionAndCRC2SndBuffer( )
{
	int FIXED_LEN1=5;//5=AA+55+len+deviceID+cmdID(һ��5���ֽ�)
	int FIXED_LEN2=7;//7=FIXED_LEN1+2(checksum)
	
	int pack_len=FIXED_LEN2+9;       //0��ʾû���������,�û�������Ҫ�Լ�����
	uint8_t buffer[pack_len];
	buffer[INDEX_HEAD0]=HEAD_MARK0;
	buffer[INDEX_HEAD1]=HEAD_MARK1;
	
	buffer[INDEX_LEN]=FIXED_LEN1+1;   //0��ʾû���������,�û�������Ҫ�Լ�����
	buffer[INDEX_DEVICE_ID]=DEVICE_ID_VALVE_CTRL;
	buffer[INDEX_CMD_ID]=CMD_VALVECTRL_ACK_RELEASE_VER_CRC;
	

	//������
	buffer[5] = 0;
	buffer[6] = 0;
	buffer[7] = 0;
	
	buffer[8] = 0;
	buffer[9] = 0;
	buffer[10] = 0;
	buffer[11] = 0;
	
	buffer[12] = 0;
	buffer[13] = 0;
	buffer[14] = 0;

	set_checkSum(buffer);  //����checksum
	SaveDataTo(&UART2_send_ctrl,buffer,pack_len);
}


void SoftReset()
{	
	//__set_FAULTMASK(1);
	NVIC_SystemReset();
}

static void Process_DrvBrd_Msg(UART_HandleTypeDef* p_huart,uint8_t* pArray)
{
	uint8_t cmdID=pArray[INDEX_CMD_ID];        //��ȡCMD ID
	
	switch(cmdID)
	{
		case CMD_DRIVER_GET_PARA:
			saveRealPara2SndBuffer(&valve_para,p_huart);
			break;
		case CMD_DRIVER_SET_RUNSTATE_FLOW:
			getSetRunstateFlow(&runing_state,pArray);   //1.��ȡ"��������״̬&����"����
			saveAck2SndBuffer();
			break;
		case CMD_DRIVER_SET_BAUD_RATE:
			saveBaudSet2SndBuffer(&set_baud_rate,pArray);
			uart_flag = 1;
			break;
		case CMD_DRIVER_GET_SOFTWARE_VER:
			saveVersion2SndBuffer();
			break;
		case CMD_DREVER_GET_RELEASE_VER_CRC:
			saveVersionAndCRC2SndBuffer();
			break;
		case CMD_DREVER_SET_SYS_RESET:
			SoftReset();
			break;
		case CMD_DREVER_SET_SELFCHECK:
			self_checking();		//��ʼ�Լ�
			break;
		default:
			break;
	}
}
//uint8_t tempArray[50]={0};
static void Process_ValveCtrl_Msg(UART_HandleTypeDef* p_huart,uint8_t* pArray)
{
	uint8_t cmdID=pArray[INDEX_CMD_ID];
	//memcpy(tttt,pArray,pArray[INDEX_LEN]+2);
	switch(cmdID)
	{
		case CMD_VALVECTRL_ACK_REAL_PARA:
			HAL_UART_Transmit_DMA(p_huart,pArray,pArray[INDEX_LEN]+2);  
			break;
		case CMD_VALVECTRL_ACK_SET_PARA:
			HAL_UART_Transmit_DMA(p_huart,pArray,pArray[INDEX_LEN]+2);
			break;
		case CMD_VALVECTRL_ACK_SET_BAUD_RATE:
			HAL_UART_Transmit_DMA(p_huart,pArray,pArray[INDEX_LEN]+2);
			break;
		case SLAVER_SEND_PID_GET_RESULT:
			HAL_UART_Transmit_DMA(p_huart,pArray,pArray[INDEX_LEN]+2);
			break;
		case HOST_SEND_SETTING_FLOW:
			HAL_UART_Transmit_DMA(p_huart,pArray,pArray[INDEX_LEN]+2);
			break;
		case CMD_VALVECTRL_ACK_SOFTWARE_VER:
			HAL_UART_Transmit_DMA(p_huart,pArray,pArray[INDEX_LEN]+2);
			break;
		case CMD_VALVECTRL_ACK_RELEASE_VER_CRC:
			HAL_UART_Transmit_DMA(p_huart,pArray,pArray[INDEX_LEN]+2);
			break;
		case CMD_VALVECTRL_ACK_SYS_RESET:
			HAL_UART_Transmit_DMA(p_huart,pArray,pArray[INDEX_LEN]+2);
			break;
		case CMD_VALVECTRL_ACK_SELFCHECK:
			HAL_UART_Transmit_DMA(p_huart,pArray,pArray[INDEX_LEN]+2);
			break;
		default:
			HAL_UART_Transmit_DMA(p_huart,pArray,pArray[INDEX_LEN]+2);
			break;
	}
}



//����ʵʱ���ݵ�PC
void send_valve_data_2_PC()
{
	int FIXED_LEN1=5;//5=AA+55+len+deviceID+cmdID(һ��5���ֽ�)
	int FIXED_LEN2=7;//7=FIXED_LEN1+2(checksum)

	int pack_len=FIXED_LEN2+34;       //5��ʾ5������
	uint8_t buffer[pack_len];
	buffer[INDEX_HEAD0]=HEAD_MARK0;
	buffer[INDEX_HEAD1]=HEAD_MARK1;
	
	buffer[INDEX_LEN]=FIXED_LEN1+34;   //5��ʾ5������
	buffer[INDEX_DEVICE_ID]=DEVICE_ID_VALVE_CTRL;
	buffer[INDEX_CMD_ID]=HOST_SEND_SETTING_FLOW;  //����״̬&����
	
	//�������
	
	//flow_module��ֻ����flow_slpm��flow_lpm_set��flow_slpm
	buffer[5] = (u8)((flow_module.flow_slpm >> 8) & 0xff);      //flow_slpm
	buffer[6] = (u8)flow_module.flow_slpm;
	
	buffer[7] = (u8)((flow_module.flow_lpm_set >> 8) & 0xff);   //flow_lpm_set
	buffer[8] = (u8)flow_module.flow_lpm_set;
	
	buffer[9] = (u8)((flow_module.pro_pwm >> 8) & 0xff);        //pro_pwm
	buffer[10] = (u8)flow_module.pro_pwm;

	//sampling_module�з���
	buffer[11] = (u8)((sampling_module.avg_flow >> 8) & 0xff); 	//avg_flow
	buffer[12] = (u8)sampling_module.avg_flow;
	
	buffer[13] = (u8)((sampling_module.flow >> 8) & 0xff);    	//flow
	buffer[14] = (u8)sampling_module.flow;                   
	
	buffer[15] = (u8)sampling_module.sensor_type;           		//sensor_type
	buffer[16]=(u8)sampling_module.sensor_err;             			//sensor_err
	
	buffer[17]=(u8)((sampling_module.cur_adc>>8)&0xff);   		  //cur_adc
	buffer[18]=(u8)sampling_module.cur_adc;
	
	buffer[19]=(u8)sampling_module.IS_valve_exist;              //Is valve exist
	buffer[20]=(u8)sampling_module.IS_5V_ok;                    //Is 5V ok
	buffer[21]=(u8)sampling_module.IS_12V_ok;                   //Is 12V ok
	 
	//������
	for(int i=0;i<17;i++)
	{
		buffer[22+i]=0x11;
	}
	set_checkSum(buffer);  //����checksum
	SaveDataTo(&UART2_send_ctrl,buffer,pack_len);
}

//������Ĵ˺���������
//ͨ��ʶ��Device ID:
//1.������PC��ص���Ϣ
//2.������MLB��ص���Ϣ
//3.��������������ص���Ϣ
//4.�����������ϰ����Ϣ
void ParseAndSend(UART_HandleTypeDef* p_huart,uint8_t* pArray)
{
	//��ȡ�豸��
	uint8_t deviceID=pArray[INDEX_DEVICE_ID];

	//�����豸���Լ�ȥ����������
	switch(deviceID)
	{
		case DEVICE_ID_PC:
			Process_PC_Msg(p_huart,pArray);                                          //������PC��ص���Ϣ
			break;
		case DEVICE_ID_MLB:
			//����MLB��Ϣ,�����Ϣֻ��DEVICE_ID_MLB�й�
			Process_MLB_Msg(p_huart,pArray);                                         //������MLB��ص���Ϣ
			break;
		case DEVICE_ID_DRIVER:
			//���д����������Ĵ��룬���������Լ���Ӵ�����
			Process_DrvBrd_Msg(p_huart,pArray);                                      //��������������ص���Ϣ
			#ifdef MASTER_MLB
			time_out_cnt=0;    //��������е������ʾ��������Ӧ��time_out_cnt��0
			#endif
			break;
		case DEVICE_ID_VALVE_CTRL:
			//���д���ǿ�����ϰ�Ĵ��룬����������Ӵ�����
			Process_ValveCtrl_Msg(p_huart,pArray);                                   //�����������ϰ����Ϣ
			break;
		default:
			break;
	}
}




//���������������
void comm_task(void* pvParamemters)
{
	u16 re_delay = 0;
	u16 tx_len;
	u8 com2_txbuffer[TX_FIFO_LEN];
	static u32t sum_flow=0;
	static int temp=0;
	
	comm_module_init();
	vTaskDelay(10/portTICK_RATE_MS );
	
	uint8_t step=5;
	
	while(1)
	{	
		#ifdef PROCESS_RCV_IN_TASK
		#else
		ProcessRcvedData();
		#endif
		
		
		//100msһ�Σ���������䵽valve_para�У������Ҫ���Խ����ݷ��͵�PC(��������)
		if(send_loop_cnt>=SEND_INTERVAL/10-1)
		{
			#ifdef DEBUG_FOR_VALVE_CUREVE
			send_valve_data_2_PC();   //��PC���ݣ����ڵ���
			#else
			//if(pc_lock){
				//send_valve_data_2_PC();
			//}
			//��ȡ����
			//sampling_module.cur_adc = sampling_module.cur_adc*0.35;
			if((flow_module.flow_lpm_set < 1000)&&(flow_module.pro_pwm > 1440)){
				flow_module.pro_pwm = 1440;
			}else if(flow_module.flow_lpm_set < 7000){
				temp = ((flow_module.flow_lpm_set-1000)*7/1000+62)*24;
				if(flow_module.pro_pwm > temp){
					flow_module.pro_pwm = temp;
				}
			}
			if(flow_module.pro_pwm > 2400){
				flow_module.pro_pwm = 2400;
			}
			if(sampling_module.avg_flow < 10000){
				valve_para.o2_flow=sampling_module.avg_flow;
			}
			//valve_para.o2_flow=sampling_module.avg_flow;
			valve_para.valveBoard_12V=sampling_module.IS_12V_ok;
			valve_para.valveBoard_5V=sampling_module.IS_5V_ok;
			valve_para.valve_current=sampling_module.cur_adc*0.366;
			valve_para.valve_PWM_DC=flow_module.pro_pwm;
			valve_para.temp = sampling_module.temp;
			#endif
			sampling_module.avg_flow=sum_flow/send_loop_cnt;   //flow��ƽ��ֵ

			send_loop_cnt=0;
			sum_flow=0;
			if(uart_flag != 0){
				uart_flag++;
				if(uart_flag == 5){
					uart_flag = 0;
					MX_USART2_UART_Init();
				}
			}
		}
		else
		{
			sum_flow+=sampling_module.flow;
			send_loop_cnt++;
			//��������
		}

	
		sendData();
		vTaskDelay(25/portTICK_RATE_MS );
	}
}
