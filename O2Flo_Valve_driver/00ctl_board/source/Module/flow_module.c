/*******************************************************************************
** �ļ���: 		flow.c
** �汾��  		1.0
** ��������: 	RealView MDK-ARM 4.20
** ����: 		�����
** ��������: 	2011-04-10
** ����:		ģ������û������������˵�����̵Ĺ��ܣ�
** ����ļ�:	��
** �޸���־��	2011-04-10   �����ĵ�
*******************************************************************************/
/* ����ͷ�ļ� *****************************************************************/
#include "main.h"
#include "stm32f0xx_hal.h"
#include "cmsis_os.h"
#include "datatype.h"
#include "fifo.h"
#include "main.h"
#include "flow_module.h"
#include "sampling_module.h"
#include "pid.h"
#include "common.h"
#include "comm_module.h"
#include "comm_def.h"

#define FLOW_CONTROL_TASK_DELAY  100
#define PWM_DC_MAX  2400

//#define DEBUG_FOR_VALVE_CUREVE //���Ա�����������



/* �������� ------------------------------------------------------------------*/
extern void send_modify_PID_result(u8 result);
/* �궨�� --------------------------------------------------------------------*/

/* ���� ----------------------------------------------------------------------*/
extern TIM_HandleTypeDef htim3;
extern SAMPLING_MODULE sampling_module;
FLOW_MODULE flow_module;
FLOW_PID 	flow_pid; 
s16 flow_array_buf[FLOW_BUFFER_LEN] = {0};

static u32t duration_cnt = 0;
static BOOL b_set_pwm_dc=FALSE;
static int pwm_value=0;
static BOOL b_test_valve_curve=FALSE;
//uint8_t b_disable_pid_debug=1;
int p_max_val=0;
int i_max_val=0;
int i_min_val=0;
int pid_kp=0;					//����ϵ�� 
int pid_ki=0;					//����ϵ��
int pid_kd=0;					//΢��ϵ�� 

/* �������� ------------------------------------------------------------------*/

/* �������� ------------------------------------------------------------------*/
/******************************************************
*�ڲ���������
******************************************************/
/***************************************************
*�������ƣ�flow_module_init
*��������������ģ�������ʼ��
*�����������
*�����������
*����ֵ����
****************************************************/
static void flow_module_init(FLOW_MODULE * module)
{
	module->flow_lpm_set = 0;	//�趨����
	module->flow_lpm_rt = 0;	//ʵʱ����
	module->flow_slpm = 0;
	module->flow_slpm_rt = 0;
	module->pro_pwm = 0;			//����������PWM
}
//�򿪻�رձ�����24V��ѹ
void flow_module_valve_enalbe(u8 flg)
{
	if(flg)
	{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,GPIO_PIN_RESET);
	}
}
/***************************************************
*�������ƣ�_coeff_init
*�����������������Ʋ�����ʼ��
*�����������
*�����������
*����ֵ����
****************************************************/
static void _coeff_init(void)
{
	memset(&flow_pid,0,sizeof(flow_pid));		//��PID������ʼ����0
}
/***************************************************
*�������ƣ�flow_pid_calc
*������������������PID����
*����������趨������ʵʱ����
*�������������������PWM,ȡֵ0-2400
*����ֵ����
****************************************************/
	int value_ttt = 0;

static void flow_pid_calc(u16 set_flow,u16 rt_flow) 
{
 	int value_sum = 0;
//	int p_max_val;
//	int i_max_val;
//	int i_min_val;
//	int pid_kp;					//����ϵ�� 
// 	int pid_ki;					//����ϵ��
// 	int pid_kd;					//΢��ϵ�� 	

//	pid_kp = 3;
//	pid_ki = 5;
//	pid_kd = 1;
//	
//	p_max_val = 1000;			//���Ƶ�������ֵ
//	i_max_val = 48000;		//���Ƶ��������ֵ
//	i_min_val = 2000;			//���Ƶ�������Сֵ 

	if(set_flow == 0){
		flow_pid.Uk = 0;
		return;
	}
	if(rt_flow == 0){
		return;
	}
	flow_pid.Ek = set_flow-rt_flow;
	if(flow_pid.Ek > p_max_val)//10��Ϊ���� 
	{
		flow_pid.Ek = p_max_val;
	}
	if(flow_pid.Ek < 0-p_max_val)
	{
		flow_pid.Ek = 0-p_max_val;
	}	
 	flow_pid.Sum_error += flow_pid.Ek;//�������
 	if(flow_pid.Sum_error > i_max_val) //0.1���������250һ����
 	{
 		flow_pid.Sum_error = i_max_val;
 	}
 	if(flow_pid.Sum_error < i_min_val)
 	{
 		flow_pid.Sum_error = i_min_val;
 	} 	

	value_sum = pid_kp * flow_pid.Ek/100;//���� ���100*30=3000;  ���*����ϵ��(�ٶ����5�ȼ�50*25=1250)
	if(value_sum<PWM_DC_MAX*20/100)  //��Ӧ�̽����20%��ʼ(2400*20%=480),ʵ�ʲ���30%(2400*30%=720)Ҳok
	{
		value_sum=PWM_DC_MAX*20/100;
	}
	
	value_sum += flow_pid.Sum_error * pid_ki/100 ; //���� ���3000*2/100=60,һ������������250��  250*5/15*3=250;
 	value_sum += (flow_pid.Ek - (2 * flow_pid.Ek1) + flow_pid.Ek2) * pid_kd/100;//΢�� 

	if(value_sum > 5000){
		value_ttt = value_sum;
	}
	if((value_sum >= 0)&&(value_sum <= PWM_DC_MAX))
		flow_pid.Uk = value_sum;
	
	if(flow_pid.Uk > PWM_DC_MAX)
	{
		flow_pid.Uk = PWM_DC_MAX;
	}
////	if(flow_pid.Uk <= 0)
////	{
////		flow_pid.Uk = 0;
////	}
//	if(flow_pid.Uk <= 480)  //��Ӧ�̽����20%��ʼ(2400*20%=480)
//	{
//		flow_pid.Uk = 480;
//	}
	//if(flow_pid.Uk <= PWM_DC_MAX)
	if(value_sum <= PWM_DC_MAX)   //2020-6-12
	{
		flow_pid.Uk1 = flow_pid.Uk;       // �����µ�K-1�����ֵ
	 	flow_pid.Ek2 = flow_pid.Ek1;        // �����µ�K-1������ֵ
	 	flow_pid.Ek1 = flow_pid.Ek;
	}

}

	
//��������  0.01 slpm 0~80slpm
void flow_module_detect_flow(FLOW_MODULE *module)
{
	s16 flow_buf[FLOW_BUFFER_LEN] = {0};

	module->flow_lpm_rt = com_shift_avg((s16 *)flow_array_buf,(s16 *)flow_buf,FLOW_BUFFER_LEN,1,sampling_module.flow);
}

/***************************************************
*�������ƣ�interface_module_set_valve_putout
*�������������ñ����� 
*���������putout ���ǿ�ȣ�[0~2399]  //�����TIM3�������޸�  PWM 20khz
*�����������
*����ֵ����
****************************************************/
void interface_module_set_valve_putout(int16_t putout)
{
	//����
	if(putout >= PWM_DC_MAX)
		putout = PWM_DC_MAX;
	else if(putout < 0)
		putout = 0;
	
	//���ö�ʱ��
	//putout = 2400;
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, putout);//����ռ�ձ�Ϊ0
}

static u16 start_flow=0;
static u16 end_flow=0;
static u16 step=0;
static u16 duration=0;
static u8 Is_only_set_flow=0;

void set_flow()
{
	start_flow=0;
	end_flow=0;
	step=0;
	duration=0;
	
	Is_only_set_flow=1;
}

void init_PID_parameters()
{
	pid_kp = 2;
	pid_ki = 1;
	pid_kd = 1;
	
	p_max_val = 1200;			
//	i_max_val = 80000; //kp=3,ki=3;��ŵ��㷨:(kp*1000+ki*sum_error)/100=2400,2400��PWM�����ֵ(100%ռ�ձ�)
	i_max_val = 240000;
	i_min_val = 2000;			 
}

/***************************************************
*�������ƣ�set_flow
*����������������λ�����͵��趨��������,���趨����
*���������pData,���յ�����λ�������
*�����������
*����ֵ����
****************************************************/
void set_flow_parameter(UINT8 *pData)
{
	//��ȡ��λ�����͵���������ֵ
	duration_cnt=0;
	
	if(pData[5]==0x01)  //�����λ��������ֻ��������ֵ
	{
		start_flow=0;
		end_flow=0;
		step=0;
		duration=0;
		
		Is_only_set_flow=1;
		
		flow_module.flow_lpm_rt=0;
		flow_module.flow_slpm_rt=0;
		flow_module.flow_slpm=0;
		flow_module.flow_lpm_set=pData[14]*256+pData[15]; 
	}
	else
	{
		Is_only_set_flow=0;
		
		start_flow=pData[6]*256+pData[7];
		end_flow=pData[8]*256+pData[9];
		step=pData[10]*256+pData[11];
		duration=pData[12]*256+pData[13];
//		flow_module.flow_lpm_set=pData[13]*256+pData[14]; 
		
		flow_module.flow_lpm_set=start_flow;
	}
	
//	init_PID_parameters();  //��ԭPID����
//	_coeff_init();   //�������Ʋ�����ʼ��
}

//void send_modify_PID_result(u8 result)
//{
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
//	
//	fifoWriteData(&com2_tx_fifo, buffer, buffer[1]+2);
////	vTaskDelay(20 /portTICK_RATE_MS);
////	
////	tx_len = fifoReadData(&com2_tx_fifo, com2_txbuffer, TX_FIFO_LEN);  //��com2_tx_fifo����com2_txbuffer��
////		if(tx_len > 0)
////			interface_module_transmit(COM2, com2_txbuffer,tx_len);      //ʹ��DMA��������
//}

/***************************************************
*�������ƣ�set_pid
*����������������λ������PID��������λ����pid����
*���������pData,���յ�����λ�������
*�����������
*����ֵ����
****************************************************/
extern FIFO_TYPE com2_tx_fifo;
void set_pid(UINT8 *pData)
{
	if(pData[5]==0x01)
	{
//		b_disable_pid_debug=1;
		init_PID_parameters();  //��ԭPID����
//		_coeff_init();   //�������Ʋ�����ʼ��
	}
	else
	{
//		b_disable_pid_debug=0;
		 
		pid_kp=(pData[6]<<24)+((pData[7]<<16))+(pData[8]<<8)+pData[9];
		pid_ki=(pData[10]<<24)+((pData[11]<<16))+(pData[12]<<8)+pData[13];
		pid_kd=(pData[14]<<24)+((pData[15]<<16))+(pData[16]<<8)+pData[17];
		p_max_val=(pData[18]<<24)+((pData[19]<<16))+(pData[20]<<8)+pData[21];
		i_max_val=(pData[22]<<24)+((pData[23]<<16))+(pData[24]<<8)+pData[25];
		i_min_val=(pData[26]<<24)+((pData[27]<<16))+(pData[28]<<8)+pData[29];
	}
//	b_test_valve_curve=FALSE;
	send_modify_PID_result(pData[5]);
}

void send_pid_2_PC(UINT8 *pData)
{
//	u8 i;
//	u8 buffer[30];      
//	
//	//TODO,�������ݴ��޸�
//	buffer[0] = PACK_HEAD_BYTE;
//	buffer[1] = 30-2;
//	buffer[2] = MODULE_CMD_TYPE;     //0x00
//	buffer[3] = SLAVER_SEND_PID_GET_RESULT;    //0xAD
//	
//	//kp
//	buffer[4] = (u8)((((pid_kp>>16)&0xffff)>>8)&0xff);
//	buffer[5] = (u8)((pid_kp>>16)&0xffff);
//	buffer[6] = (u8)(((pid_kp&0xffff)>>8)&0xff);
//	buffer[7] = (u8)(pid_kp&0xffff);
//	
//	//ki
//	buffer[8] = (u8)((((pid_ki>>16)&0xffff)>>8)&0xff);
//	buffer[9] = (u8)((pid_ki>>16)&0xffff);
//	buffer[10] = (u8)(((pid_ki&0xffff)>>8)&0xff);
//	buffer[11] = (u8)(pid_ki&0xffff);
//	
//	//kd
//	buffer[12] = (u8)((((pid_kd>>16)&0xffff)>>8)&0xff);
//	buffer[13] = (u8)((pid_kd>>16)&0xffff);
//	buffer[14] = (u8)(((pid_kd&0xffff)>>8)&0xff);
//	buffer[15] = (u8)(pid_kd&0xffff);

//	//P Max
//	buffer[16] = (u8)((((p_max_val>>16)&0xffff)>>8)&0xff);
//	buffer[17] = (u8)((p_max_val>>16)&0xffff);
//	buffer[18] = (u8)(((p_max_val&0xffff)>>8)&0xff);
//	buffer[19] = (u8)(p_max_val&0xffff);

//	//I Max
//	buffer[20] = (u8)((((i_max_val>>16)&0xffff)>>8)&0xff);
//	buffer[21] = (u8)((i_max_val>>16)&0xffff);
//	buffer[22] = (u8)(((i_max_val&0xffff)>>8)&0xff);
//	buffer[23] = (u8)(i_max_val&0xffff);
//	
//	//I Min
//	buffer[24] = (u8)((((i_min_val>>16)&0xffff)>>8)&0xff);
//	buffer[25] = (u8)((i_min_val>>16)&0xffff);
//	buffer[26] = (u8)(((i_min_val&0xffff)>>8)&0xff);
//	buffer[27] = (u8)(i_min_val&0xffff);

//	CalcCheckSum(buffer);
//	
//	fifoWriteData(&com2_tx_fifo, buffer, buffer[1]+2);
	
	
	
	int FIXED_LEN1=5;//5=AA+55+len+deviceID+cmdID(һ��5���ֽ�)
	int FIXED_LEN2=7;//7=FIXED_LEN1+2(checksum)

	int pack_len=FIXED_LEN2+24;       
	uint8_t buffer[pack_len];
	buffer[INDEX_HEAD0]=HEAD_MARK0;
	buffer[INDEX_HEAD1]=HEAD_MARK1;
	
	buffer[INDEX_LEN]=FIXED_LEN1+24;   
	buffer[INDEX_DEVICE_ID]=DEVICE_ID_VALVE_CTRL;
	buffer[INDEX_CMD_ID]=SLAVER_SEND_PID_GET_RESULT; 
	
	//�������
	//kp
	buffer[5] = (u8)((((pid_kp>>16)&0xffff)>>8)&0xff);
	buffer[6] = (u8)((pid_kp>>16)&0xffff);
	buffer[7] = (u8)(((pid_kp&0xffff)>>8)&0xff);
	buffer[8] = (u8)(pid_kp&0xffff);
	
	//ki
	buffer[9] = (u8)((((pid_ki>>16)&0xffff)>>8)&0xff);
	buffer[10] = (u8)((pid_ki>>16)&0xffff);
	buffer[11] = (u8)(((pid_ki&0xffff)>>8)&0xff);
	buffer[12] = (u8)(pid_ki&0xffff);
	
	//kd
	buffer[13] = (u8)((((pid_kd>>16)&0xffff)>>8)&0xff);
	buffer[14] = (u8)((pid_kd>>16)&0xffff);
	buffer[15] = (u8)(((pid_kd&0xffff)>>8)&0xff);
	buffer[16] = (u8)(pid_kd&0xffff);

	//P Max
	buffer[17] = (u8)((((p_max_val>>16)&0xffff)>>8)&0xff);
	buffer[18] = (u8)((p_max_val>>16)&0xffff);
	buffer[19] = (u8)(((p_max_val&0xffff)>>8)&0xff);
	buffer[20] = (u8)(p_max_val&0xffff);

	//I Max
	buffer[21] = (u8)((((i_max_val>>16)&0xffff)>>8)&0xff);
	buffer[22] = (u8)((i_max_val>>16)&0xffff);
	buffer[23] = (u8)(((i_max_val&0xffff)>>8)&0xff);
	buffer[24] = (u8)(i_max_val&0xffff);
	
	//I Min
	buffer[25] = (u8)((((i_min_val>>16)&0xffff)>>8)&0xff);
	buffer[26] = (u8)((i_min_val>>16)&0xffff);
	buffer[27] = (u8)(((i_min_val&0xffff)>>8)&0xff);
	buffer[28] = (u8)(i_min_val&0xffff);

	set_checkSum(buffer);  //����checksum
	SaveDataTo(&UART2_send_ctrl,buffer,pack_len);
	
}

void set_pwm_dc(UINT8 *pData,int CMD)
{
	if(CMD==HOST_SEND_MANUAL_SET_PWM_DC)
	{
		b_set_pwm_dc=pData[5];    //�ֶ�����PWM��flagֵ
		if(b_set_pwm_dc==TRUE)
		{
			pwm_value=pData[6]*256+pData[7];
		}
		else
		{
			pwm_value=0;
		}
	}
	else if(CMD==HOST_SEND_TEST_VALVE_CURVE)
	{
		b_test_valve_curve=pData[5];    //���Ա�������flagֵ
		if(b_test_valve_curve==TRUE)
		{
			pwm_value=pData[6]*256+pData[7];
		}
		else
		{
			pwm_value=0;
		}
	}
	
}

void re_cfg_PWM_Frequency(uint32_t frequency)
{
	TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  htim3.Instance = TIM3;
//  htim3.Init.Prescaler = 20-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = PWM_DC_MAX-1;
	htim3.Init.Prescaler = 48000000/frequency/htim3.Init.Period-1;
//	htim3.Init.Prescaler = 10-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 400;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_TIM_MspPostInit(&htim3);
	
	//������ʱ��
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
}



typedef enum 
{
	FREQUENCY_1,
	FREQUENCY_2
}FREQUENCY_CHANGE_STATE;

FREQUENCY_CHANGE_STATE freq_state=FREQUENCY_1;

#if 0
void Motor_PWM_Freq_Dudy_Set(UINT8 PWM_NUMBER, UINT16 Freq,UINT16 Duty)			//PWM1-2-3,FREQ,DUFY
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	UINT32 i;	
	
	if((Freq >=1) && (Freq <=50000)// Frequency  1 - 255Hz
		&& (Duty <= 100))//Duty cycle 10 - 90
	{
		TIM_TimeBaseStructure.TIM_Period = 48000/Freq - 1;       //
		TIM_TimeBaseStructure.TIM_Prescaler = 1000-1;	    //1000Ԥ��Ƶ
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;	//����ʱ�ӷ�Ƶϵ��������Ƶ(�����ò���)
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //���ϼ���ģʽ
		//TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);	
		
		i = TIM_TimeBaseStructure.TIM_Period + 1;
		i *= Duty;
		i /= 100;
		
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //����ΪPWMģʽ1
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//OUT ENABLE
		TIM_OCInitStructure.TIM_OutputNState=TIM_OutputNState_Enable;  //@�޸�PWM2
		TIM_OCInitStructure.TIM_Pulse = i;
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //��ЧΪ�ߵ�ƽ���
		//  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
		TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;  //@�޸�PWM2
		TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
		TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
		
//		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //����ΪPWMģʽ1
//		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
//		TIM_OCInitStructure.TIM_Pulse = i;//0;	
//		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //����ʱ������ֵС��CCR1_ValʱΪ�͵�ƽ
		if(PWM_NUMBER == 1)
		{
			TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);	
			TIM_OC1Init(TIM3, &TIM_OCInitStructure);	 //ʹ��TIM3CH1
			TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);	//		
		}
//		else if(PWM_NUMBER == 2)
//		{
//			TIM_TimeBaseInit(TIM17, &TIM_TimeBaseStructure);	
//			TIM_OC1Init(TIM17, &TIM_OCInitStructure);	 //ʹ��TIM3CH1
//			TIM_OC1PreloadConfig(TIM17, TIM_OCPreload_Enable);	//		
//		}
		else if(PWM_NUMBER == 2)
		{
			TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);	
			TIM_OC1Init(TIM14, &TIM_OCInitStructure);	 
			TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable);		
		}		
		//TIM_SetCompare1(TIM3, Wire_Temp);
	}
}
#endif

/***************************************************
*�������ƣ�flow_task
*���������������������� 
*�����������
*�����������
*����ֵ����
****************************************************/
void flow_control_task(void* pvParamemters)
{
	//��Ƶ����,��������Ƶ��
	static uint8_t freq_cnt=0;
	//static int freq_arry[4]={1000,800,4000,8000};
	static int freq_arry[4]={1400,800,4000,8000};
	static int index=0;
	static int freq_1_cnt=0;   //��Ƶ
	static int freq_2_cnt=0;   //����Ƶ��
	static int temp=0;   
	
	flow_module_init(&flow_module);
  init_PID_parameters();
	_coeff_init();
	re_cfg_PWM_Frequency(freq_arry[0]);
	while(1)
	{
		
//		if(freq_cnt==1)   //ÿ��4�θ���һ��Ƶ��
//		{
//			freq_cnt=0;   //����
//			
//			//ѭ��ȡfreq_arry��ֵ,��������Ƶ��Ϊ500,1000,2000,500....һֱѭ��
//			if(index==1)   
//			{
//				index=0;
//			}
//			re_cfg_PWM_Frequency(freq_arry[index++]);
//		}
//		else
//		{
//			freq_cnt++;
//		}
		
		
		
#if 0
		//��Ƶ��������2��(2*120ms)��Ƶ��1
		if(freq_state==FREQUENCY_1)
		{
			if(freq_1_cnt==2)
			{
				freq_1_cnt=0;
				freq_state=FREQUENCY_2;
			}
			else
			{
				freq_1_cnt++;
				re_cfg_PWM_Frequency(freq_arry[0]);
			}
		}
		//Ƶ��1������֮��,����Ƶ��2,Ȼ��������Ƶ��1...
		if(freq_state==FREQUENCY_2)
		{
			if(freq_2_cnt==1)
			{
				freq_2_cnt=0;
				freq_state=FREQUENCY_1;
			}
			else
			{
				freq_2_cnt++;
				re_cfg_PWM_Frequency(freq_arry[1]);
			}
		}
#endif		

		#ifdef DEBUG_FOR_VALVE_CUREVE
		if(b_test_valve_curve)
		{
			flow_pid_calc(flow_module.flow_lpm_set,flow_module.flow_lpm_rt);	//PID����
			flow_module.pro_pwm = flow_pid.Uk;

			interface_module_set_valve_putout(pwm_value);
		}
		else
		{
//			if(flow_module.flow_lpm_set == 0)	//����趨����Ϊ0ʱ����رձ�����
//			{
//				flow_module.pro_pwm = 0;
//				interface_module_set_valve_putout(flow_module.pro_pwm);
//			}
//			else			//������Ϊ0ʱ������������
			{
				if(Is_only_set_flow==0)
				{
					if(duration_cnt*FLOW_CONTROL_TASK_DELAY >= 1000*duration)                    //����duration��󣬽����ж�
					{
						duration_cnt = 0;
						if(flow_module.flow_lpm_set>=end_flow)  //����end_flow֮�����´�start_flow��ʼ
						{
							flow_module.flow_lpm_set=start_flow;
						}
						else
						{
							flow_module.flow_lpm_set+=step;       //�𲽵���step
						}
					}
				}

				flow_pid_calc(flow_module.flow_lpm_set,flow_module.flow_lpm_rt);	//PID����
				flow_module.pro_pwm = flow_pid.Uk;
	//			interface_module_set_valve_putout(flow_module.pro_pwm);						//��PID����ֵ���Ʊ�����
	
				if((flow_module.flow_lpm_set < 1000)&&(flow_module.pro_pwm > 1440)){
					flow_module.pro_pwm == 1440;
				}else if(flow_module.flow_lpm_set < 7000){
					temp = ((flow_module.flow_lpm_set-1000)*7/1000+62)*24;
					if(flow_module.pro_pwm > temp){
						flow_module.pro_pwm = temp;
					}
				}
				
				if(b_set_pwm_dc)
				{
					interface_module_set_valve_putout(pwm_value);
				}
				else
				{
					interface_module_set_valve_putout(flow_module.pro_pwm);	
				}
			}
		}
		
		
		flow_module_detect_flow(&flow_module);		//�������ݵ����ư�
		duration_cnt++;
		#else
		flow_pid_calc(flow_module.flow_lpm_set,flow_module.flow_lpm_rt);	//PID����
		flow_module.pro_pwm = flow_pid.Uk;

		#if 1
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
		#endif
		//flow_module.flow_lpm_set = 8000;
		interface_module_set_valve_putout(flow_module.pro_pwm);						//��PID����ֵ���Ʊ�����

		flow_module_detect_flow(&flow_module);		//�������ݵ����ư�
		#endif
		

		vTaskDelay(FLOW_CONTROL_TASK_DELAY /portTICK_RATE_MS);
	}
}





