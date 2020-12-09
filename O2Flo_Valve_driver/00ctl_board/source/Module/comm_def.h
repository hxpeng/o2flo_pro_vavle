#ifndef _COMM_DEF_H
#define _COMM_DEF_H
#include <stdint.h>

//                          |------------------------|
//                          |                        |  仅仅用于调试
//                          |          PC            |  
//                          |________________________|
//                                  |       /|\
//                                  |        |
//                                 \|/       |
//                          |------------------------|
//                          |                        |
//                          |        主板(MLB)       |
//                          |________________________|
//                                  |       /|\
//                        Get/Set   |        | Ack
//                                 \|/       |
//                          |------------------------|
//                          |                        |
//                          |         驱动板         |
//                          |________________________|
//                                  |       /|\
//                        Get/Set   |        | Ack
//                                 \|/       |
//                          |------------------------|
//                          |                        |
//                          |  比例阀板(空氧混合板)  |
//                          |________________________|
//
//

//**********************************START**通用信息***START************************************
//通信包的各部分信息的索引
//包头，包长，设备号，命令号
#define INDEX_HEAD0 					0
#define INDEX_HEAD1 					1
#define INDEX_LEN							2
#define INDEX_DEVICE_ID				3
#define INDEX_CMD_ID					4

//包头标记0xAA55
#define HEAD_MARK0          0xAA
#define HEAD_MARK1          0x55

//设备编号
#define DEVICE_ID_PC						0x00		//0-PC
#define DEVICE_ID_MLB				  	0x01		//1-主板(MLB,Main Logic Board)
#define DEVICE_ID_DRIVER				0x02		//2-驱动板
#define DEVICE_ID_VALVE_CTRL		0x03		//3-比例阀控制板
//***********************************END**通用信息***END**************************************

//************************************命令编号***主板*****************************************
#define CMD_MLB_GET_POWER_STATE    				0x01			//01-主板获取电源状态
#define CMD_MLB_GET_REAL_PARA			 				0x02			//02-主板获取参数
#define CMD_MLB_SETTING_PARA  		 				0x03			//03-主板设置 运行状态，总流量，氧流量
//#define CMD_MLB_SET_TEMP									0x04			//04-主板设置 患者端温度，加热盘温度
#define CMD_MLB_SET_RTC										0x05			//05-主板设置 RTC时钟
#define CMD_MLB_SET_BAUD_RATE			 				0x06			//06-主板设定波特率
#define CMD_MLB_GET_SOFTWARE_VER   				0x07			//07-主板获取软驱动板和blender的版本号
#define CMD_MLB_SET_REALTIME_TEMP  				0x08			//08-主板获取实时温度
#define CMD_MLB_GET_RELEASE_VER_CRC				0x09			//09-主板得到驱动板与比例阀板主版本号和程序CRC
#define	CMD_MLB_SET_SYS_RESET							0x0a			//0a-主板对驱动板与比例阀板系统复位
#define	CMD_MLB_GET_SYS_SELFCHECK					0x0b			//0b-主板对驱动板与比例阀板自检返回数据
//********************************************************************************************
//*************************************命令编号**驱动板****************************************
//驱动板与主控板通信命令
#define CMD_DRIVER_ACK_POWER              0x01			//01-驱动板应答主电源状态
#define CMD_DRIVER_ACK_REAL_PARA     		  0x02			//02-驱动板应答实时参数
#define CMD_DRIVER_ACK_SET_RUNSTATE  			0x03			//03-驱动板应答设置参数
#define CMD_DRIVER_ACK_SET_TEMP       		0x04			//04-驱动板应答设定患者端和出气口温度
#define CMD_DRIVER_ACK_SET_RTC        		0x05			//05-驱动板应答RTC时钟
#define CMD_DRIVER_ACK_SET_BAUD           0x06			//06-驱动板应答波特率
#define CMD_DRIVER_ACK_SOFTWARE_VER      	0x07			//07-驱动板应答软件版本号
#define CMD_DRIVER_ACK_SET_REALTIME_TEMP  0x08			//08-驱动板应答实时温度参数
#define CMD_DRIVER_ACK_RELEASE_VER_CRC		0x09			//09-驱动板应答驱动板与比例阀板主版本号及程序CRC
#define	CMD_DRIVER_ACK_SET_SYS_RESET			0x0a			//0a-驱动板应答驱动板与比例阀板系统复位
#define	CMD_DRIVER_ACK_SET_SYS_SELFCHECK	0x0b			//0b-驱动板应答驱动板与比例阀板自检
//驱动板与比例阀板通信命令
#define CMD_DRIVER_GET_PARA				   			0x21		//21-驱动板获取 实时氧流量，实时电压，比例阀电流，比例阀占空比
#define CMD_DRIVER_SET_RUNSTATE_FLOW 			0x22		//22-驱动板设置 运行状态，设定氧流量
#define CMD_DRIVER_SET_BAUD_RATE     			0x23		//23-驱动板设置波特率
#define CMD_DRIVER_GET_SOFTWARE_VER				0x24		//24-驱动板得到比例阀板软件版本号
#define CMD_DREVER_GET_RELEASE_VER_CRC		0x25		//25-驱动板得到比例阀板主版本号和固件CRC
#define CMD_DREVER_SET_SYS_RESET					0x26		//26-驱动板对比例阀板系统复位
#define CMD_DREVER_SET_SELFCHECK					0x27		//27-驱动板得到比例阀板自检数据
//*********************************************************************************************

//************************************命令编号**比例阀控制板***********************************
#define CMD_VALVECTRL_ACK_REAL_PARA       0x21		//21-比例阀控制板 应答获取实时参数
#define CMD_VALVECTRL_ACK_SET_PARA        0x22		//22-比例阀控制板 应答设定参数
#define CMD_VALVECTRL_ACK_SET_BAUD_RATE   0x23		//23-比例阀控制板 回应驱动板的波特率设定
#define CMD_VALVECTRL_ACK_SOFTWARE_VER		0x24		//24-比例阀控制板 回应驱动板的软件版本号
#define	CMD_VALVECTRL_ACK_RELEASE_VER_CRC	0x25		//25-比例阀控制板 回应软件主版本号及CRC校验
#define CMD_VALVECTRL_ACK_SYS_RESET				0x26		//26-比例阀控制板 回应系统复位
#define CMD_VALVECTRL_ACK_SELFCHECK				0x27		//27-比例阀控制板 回应自检数据
//*********************************************************************************************



//*************************************主板/驱动板***主电源状态0x1*************************************
#define INDEX_PS_DC_ON_OFF           5
#define INDEX_PS_PWR_STATE           6
#define INDEX_PS_PWR_CAPACITY_L      7
#define INDEX_PS_PWR_CAPACITY_H      8
#define INDEX_PS_RTC_YEAR            9
#define INDEX_PS_RTC_MONTH           10
#define INDEX_PS_RTC_DAY             11
#define INDEX_PS_RTC_WEEK            12
#define INDEX_PS_RTC_HOUR            13
#define INDEX_PS_RTC_MIN             14
#define INDEX_PS_RTC_SEC             15
#define INDEX_PS_RTC_RESERVE         16

typedef struct
{
	uint8_t  DC_on_off;      //0-NA  1-有DC 2-无DC
	uint8_t  pwr_state;    	//1-无电池 2-充电中 3-充满
	uint16_t pwr_capacity;  //电量
	
	uint8_t RTC_YEAR;         //RTC
	uint8_t RTC_MONTH;
	uint8_t RTC_DAY;
	uint8_t RTC_WEEK;
	uint8_t RTC_HOUR;
	uint8_t RTC_MIN;
	uint8_t RTC_SEC;
	uint8_t RTC_RESERVE;
	
}POWER_STATE;
//***************************************************************************************************
//***************************************主板/驱动板***实时参数0x02*************************************************
#define INDEX_RP_TOTAL_FLOW_L             5
#define INDEX_RP_TOTAL_FLOW_H             6
#define INDEX_RP_O2_FLOW_L                7
#define INDEX_RP_O2_FLOW_H                8
#define INDEX_RP_RESERVED_1 						  9    //病人温度
#define INDEX_RP_RESERVED_2  							10
#define INDEX_RP_RESERVED_3  						  11   //出气口温度
#define INDEX_RP_RESERVED_4   					  12
#define INDEX_RP_TEMP_HP_L                13
#define INDEX_RP_TEMP_HP_H                14
#define INDEX_RP_RESERVED_5    						15   //环境温度
#define INDEX_RP_RESERVED_6    						16
#define INDEX_RP_TEMP_MAINFUN_L           17
#define INDEX_RP_TEMP_MAINFUN_H           18
#define INDEX_RP_TEMP_DrvBrd_L            19
#define INDEX_RP_TEMP_DrvBrd_H            20
#define INDEX_RP_VOLT_DrvBrd24V_L         21
#define INDEX_RP_VOLT_DrvBrd24V_H         22
#define INDEX_RP_VOLT_DrvBrd12V           23
#define INDEX_RP_VOLT_DrvBrd5V            24
#define INDEX_RP_VOLT_DrvBrd_BAT          25
#define INDEX_RP_VOLT_ValveBrd_12V        26
#define INDEX_RP_VOLT_ValveBrd_5V         27
#define INDEX_RP_FUN_ErrCode              28
#define INDEX_RP_FUN_SPEED_L              29
#define INDEX_RP_FUN_SPEED_H              30
#define INDEX_RP_HW_CURRENT_L             31
#define INDEX_RP_HW_CURRENT_H             32
#define INDEX_RP_HW_PWM_DC_L              33
#define INDEX_RP_HW_PWM_DC_H              34
#define INDEX_RP_TempCtrl_STATE           35
#define INDEX_RP_HP_STATE                 36
#define INDEX_RP_HP_PWM_DC_L              37
#define INDEX_RP_HP_PWM_DC_H              38
#define INDEX_RP_VALVE_CURRENT_L          39
#define INDEX_RP_VALVE_CURRENT_H          40
#define INDEX_RP_VALVE_PWM_DC_L           41
#define INDEX_RP_VALVE_PWM_DC_H           42
#define INDEX_RP_VOLT_110_220V            43
#define INDEX_RP_6PIN_STYLE               44
#define INDEX_RP_BLENDER_EXISTED          45
//2019-10-28 新增
#define INDEX_RP_O2_CON_L                 46
#define INDEX_RP_O2_CON_H                 47
#define INDEX_RP_O2_TEMP_L                48
#define INDEX_RP_O2_TEMP_H                49
#define INDEX_RP_PRESSURE_VAL_L						50
#define INDEX_RP_PRESSURE_VAL_H						51
//2019-10-31新增
#define INDEX_WATER_LEVEL_VAL_L           52
#define INDEX_WATER_LEVEL_VAL_H           53
#define INDEX_HUMIDIFYING_CAN_L           54
#define INDEX_HUMIDIFYING_CAN_H           55

typedef struct
{
	uint16_t total_flow;        				//实时总流量
	uint16_t O2_flow;           				//实时氧流量
	 
	int16_t NOT_USED_temp_patient;      //患者端温度
	int16_t NOT_USED_temp_outlet;       //出气口温度
	int16_t temp_heatingPlate; 					//加热盘温度
	int16_t NOT_USED_temp_environment;  //环境温度
	int16_t temp_mainFun;      					//主风机温度
	int16_t temp_driverBoard;  					//驱动板温度
	
	uint16_t volt_drvBrd_24V;    				//驱动板电压24V
	uint8_t volt_drvBrd_12V;  					//驱动板电压12V
	uint8_t volt_drvBrd5V;   		  			//驱动板电压5V
	uint8_t volt_drvBrd_Bat;					  //驱动板电池
	uint8_t volt_valveBrd_12V;   				//空氧混合板12V
	uint8_t volt_valveBrd_5V;     			//空氧混合板5V
	
	uint8_t fun_error_code;        			//风机状态-故障码 1-正常 2-过热 3-过流
	uint16_t fun_speed;            			//风机转速 0-40000RPM
	uint16_t heatingWire_current;  			//发热线电流
	uint16_t heartingWire_PWM_DC;  			//发热线PWM  0-3000
	uint8_t tempCtrl_state;        			//温控器状态 1-正常 2-开路
	uint8_t heatingPlate_state;    			//发热盘状态 1-正常 2-开路
	uint16_t heatingPlate_PWM_DC;  			//发热盘占空比 0-250
	uint16_t valve_current;        			//比例阀电流
	uint16_t valve_PWM_DC;         			//比例阀占空比
	uint8_t volt_110V_220V;        			//市电电压检测 1-110V 2-220V
	uint8_t tube_type;           				//6pin回路类型 1-非6pin回路 2-6Pin 3-加密验证ok
	uint8_t blender_existed;       			//空氧模块是否存在
	//2019-10-28 新增
	uint16_t O2_Con;        		  			//氧浓度
	int16_t  temp_O2;             			//氧气温度
	uint16_t pressure_val;       				//压力数据
	
	uint16_t water_level_val;        		//水位ADC值
	uint16_t humidifyingCan_ADC_val; 		//湿化罐ADC值
	
}REAL_PARA;
//***************************************************************************************************************




//*********************************主板/驱动板***设定运行命令&流量0x03******************************************************
#define INDEX_SetPara_RUN_CMD            5
#define INDEX_SetPara_TOTAL_FLOW_L       6
#define INDEX_SetPara_TOTAL_FLOW_H       7
#define INDEX_SetPara_O2_FLOW_L          8
#define INDEX_SetPara_O2_FLOW_H          9

#define INDEX_SetTemp_PATIENT_L      		 10
#define INDEX_SetTemp_PATIENT_H     		 11
#define INDEX_SetTemp_HP_L               12
#define INDEX_SetTemp_HP_H               13


typedef struct
{
	uint8_t running_cmd;  					//运行命令 1-暂停 2-运行 3-关机
	uint16_t set_total_flow;				//设定总流量 0-8000
	uint16_t set_O2_flow;   				//设定氧流量 0-8000
	int16_t set_temp_patient;   	  //设定患者端温度
	int16_t set_temp_heatingPalte;	//设定加热盘温度
}STE_PARA;
//*****************************************************************************************************************


//**********************************主板****设定温度0x04**************************************************************
//合并到0x03中

//#define INDEX_SetTemp_PATIENT_L      5
//#define INDEX_SetTemp_PATIENT_H      6
//#define INDEX_SetTemp_HP_L           7
//#define INDEX_SetTemp_HP_H           8

//typedef struct
//{
//	int16_t set_temp_patient;     //设定患者端温度
//	int16_t set_temp_heatingPalte;//设定加热盘温度
//}SET_TEMP;
//*******************************************************************************************************



//**********************************主板***设定RTC时钟0x05*****************************************************
//这条命令废弃，由MLB提供时钟
#define INDEX_SetRTC_YEAR          5
#define INDEX_SetRTC_MONTH         6
#define INDEX_SetRTC_DAY           7
#define INDEX_SetRTC_WEEK          8
#define INDEX_SetRTC_HOUR          9
#define INDEX_SetRTC_MIN           10
#define INDEX_SetRTC_SEC           11
#define INDEX_SetRTC_RESERVE       12

typedef struct
{
	uint8_t set_RTC_YEAR;
	uint8_t set_RTC_MONTH;
	uint8_t set_RTC_DAY;
	uint8_t set_RTC_WEEK;
	uint8_t set_RTC_HOUR;
	uint8_t set_RTC_MIN;
	uint8_t set_RTC_SEC;
	uint8_t set_RTC_RESERVE;
}SET_RTC;
//**********************************************************************************************************




//************************************主板/驱动板/比例阀板****设定波特率0x06/0x23************************************
#define INDEX_SET_BAUD_RATE_L        5
#define INDEX_SET_BAUD_RATE_H        6

typedef struct
{
	uint16_t set_baud_rate;   
}SET_BAUD_RATE;
//***********************************************************************************************************

//************************************主板/驱动板****获取版本号0x07************************************
#define INDEX_DRIVER_BORAD_SW_VER_L        	5
#define INDEX_DRIVER_BORAD_SW_VER_H        	6
#define INDEX_DRIVER_BORAD_HW_VER_L		   		7
#define INDEX_DRIVER_BORAD_HW_VER_H        	8
#define INDEX_VALVECTRL_SW_VER_L   			    9
#define INDEX_VALVECTRL_SW_VER_H            10
#define INDEX_VALVECTRL_HW_VER_L          	11
#define INDEX_VALVECTRL_HW_VER_H          	12

typedef struct
{
	uint16_t driver_board_SW_ver;     //驱动板版本
	uint16_t driver_board_HW_ver;
	
	uint16_t valveBoard_SW_ver;          //blender的版本
	uint16_t valveBoard_HW_ver;
  
}DRIVER_VALVECTRL_VER;
//***********************************************************************************************************


//************************************主板/驱动板****实时温度0x08************************************
#define INDEX_REALTIME_TEMP_PATIENT_L        5
#define INDEX_REALTIME_TEMP_PATIENT_H        6

#define INDEX_REALTIME_TEMP_OUTLET_L         7
#define INDEX_REALTIME_TEMP_OUTLET_H         8

#define INDEX_REALTIME_TEMP_AMBIENT_L   		 9
#define INDEX_REALTIME_TEMP_AMBIENT_H   		 10

typedef struct
{
	uint16_t temp_patient_realtime;          //实时患者端温度
	uint16_t temp_outlet_realtime;           //实时出气口温度 
	uint16_t temp_ambient_realtime;          //实时环境温度
}REALTIME_TEMP;
//***********************************************************************************************************

//************************************主板/驱动板****得到主版本号和固件CRC 0x09******************************
#define INDEX_DRIVER_RELEASE_VER_H						5
#define INDEX_DRIVER_RELEASE_VER_M						6
#define INDEX_DRIVER_RELEASE_VER_L						7
#define INDEX_DRIVER_RELEASE_FIRMWARE_LEN_1		8				//低位字节
#define INDEX_DRIVER_RELEASE_FIRMWARE_LEN_2		9
#define INDEX_DRIVER_RELEASE_FIRMWARE_LEN_3		10
#define INDEX_DRIVER_RELEASE_FIRMWARE_LEN_4		11			//高位字节
#define INDEX_DRIVER_RELEASE_FIRMWARE_CRC_L		12
#define INDEX_DRIVER_RELEASE_FIRMWARE_CRC_H		13
#define INDEX_DRIVER_RELEASE_RESERVE1					14
#define INDEX_DRIVER_RELEASE_RESERVE2					15
#define INDEX_VALVE_RELEASE_VER_H							16
#define INDEX_VALVE_RELEASE_VER_M							17
#define INDEX_VALVE_RELEASE_VER_L							18
#define INDEX_VALVE_RELEASE_FIRMWARE_LEN_1		19			//低位字节
#define INDEX_VALVE_RELEASE_FIRMWARE_LEN_2		20
#define INDEX_VALVE_RELEASE_FIRMWARE_LEN_3		21
#define INDEX_VALVE_RELEASE_FIRMWARE_LEN_4		22			//高位字节
#define INDEX_VALVE_RELEASE_FIRMWARE_CRC_L		23
#define INDEX_VALVE_RELEASE_FIRMWARE_CRC_H		24
#define INDEX_VALVE_RELEASE_RESERVE1					25
#define INDEX_VALVE_RELEASE_RESERVE2					26

typedef struct
{
	uint32_t d_release_firmware_len;
	uint32_t v_release_firmware_len;
	uint16_t d_release_firmware_crc;
	uint16_t v_release_firmware_crc;
	uint8_t d_release_ver_h;
	uint8_t d_release_ver_m;
	uint8_t d_release_ver_l;
	uint8_t v_release_ver_h;
	uint8_t v_release_ver_m;
	uint8_t v_release_ver_l;
}RELEASE_VER_CRC;
//***********************************************************************************************************


//************************************主板对驱动板及比例阀板进行复位 0x0a************************************
//只有命令没有数据
//***********************************************************************************************************
#define INDEX_ERROR_CODE_GROUP_0		5
#define INDEX_ERROR_CODE_GROUP_1		6
#define INDEX_ERROR_CODE_GROUP_2		7
#define INDEX_ERROR_CODE_GROUP_3    8

//************************************驱动板进行自检并返回错误代码 0x0b**********************
typedef struct
{
	uint8_t error_code_group_0;
	uint8_t error_code_group_1;
	uint8_t error_code_group_2;
	uint8_t error_code_group_3;
}SELF_CHECK_ERROR_CODE;
//***********************************************************************************************************

//***********************************驱动板/比例阀板***获取参数(获取氧流量,实时电压...)0x21***********************
#define INDEX_ValvePara_O2_FLOW_L      5
#define INDEX_ValvePara_O2_FLOW_H      6
#define INDEX_ValvePara_VOLT_12V       7
#define INDEX_ValvePara_VOLT_5V        8
#define INDEX_ValvePara_CURRENT_L      9 
#define INDEX_ValvePara_CURRENT_H      10
#define INDEX_ValvePara_PWM_DC_L       11
#define INDEX_ValvePara_PWM_DC_H       12
#define INDEX_ValvePara_TEMP_L       13
#define INDEX_ValvePara_TEMP_H       14


typedef struct
{
	uint16_t o2_flow;
	uint8_t valveBoard_12V;
	uint8_t valveBoard_5V;
	uint16_t valve_current;
	uint16_t valve_PWM_DC;
	int16_t temp;       //温度
}VALVE_PARA;
//**********************************************************************************************************



//***********************************驱动板/比例阀板***设置运行状态&氧流量0x22***********************************
#define INDEX_RUN_STATE        5
#define INDEX_SET_O2_FLOW_L    6
#define INDEX_SET_O2_FLOW_H    7

typedef struct
{
	uint8_t runing_state;
	uint16_t set_o2_flow;
}RUN_STATE;

//******************************************************************************************************


////***********************************驱动板/比例阀板***设置波特率0x23***********************************
//#define INDEX_SET_BAUD_RATE_L        5
//#define INDEX_SET_BAUD_RATE_H        6

//typedef struct
//{
//	uint16_t set_baud_rate;
//}BAUD_RATE_DRIVER_BLENDER;
////******************************************************************************************************


//************************************驱动板****获取比例阀板软件版本号0x24************************************
#define INDEX_VALVECTRL_SW_VERSION_L   			    5
#define INDEX_VALVECTRL_SW_VERSION_H             6
#define INDEX_VALVECTRL_RESERVE_1         		7
#define INDEX_VALVECTRL_RESERVE_2             8

typedef struct
{	
	uint16_t valveBoard_ver;          //比例阀板的软件版本
	uint16_t valveBoard_reserve;
  
}VALVECTRL_SW_VER;
//***********************************************************************************************************


//************************************驱动板****获取比例阀板主版本号和固件CRC 0x25************************************
#define INDEX_VALVECTRL_RELEASE_FIRMWARE_VERSION_H   			    5
#define INDEX_VALVECTRL_RELEASE_FIRMWARE_VERSION_M   			    6
#define INDEX_VALVECTRL_RELEASE_FIRMWARE_VERSION_L   			    7
#define INDEX_VALVECTRL_RELEASE_FIRMWARE_LEN_1   			    8
#define INDEX_VALVECTRL_RELEASE_FIRMWARE_LEN_2   			    9
#define INDEX_VALVECTRL_RELEASE_FIRMWARE_LEN_3   			    10
#define INDEX_VALVECTRL_RELEASE_FIRMWARE_LEN_4   			    11
#define INDEX_VALVECTRL_RELEASE_FIRMWARE_CRC_L   			    12
#define INDEX_VALVECTRL_RELEASE_FIRMWARE_CRC_H   			    13
#define INDEX_VALVECTRL_RELEASE_RESERVE_1               			    14
#define INDEX_VALVECTRL_RELEASE_RESERVE_2               			    15

typedef struct
{	
	uint8_t valveBoard_release_firmware_ver1;          //比例阀板固件版本
	uint8_t valveBoard_release_firmware_ver2;
	uint8_t valveBoard_release_firmware_ver3;
	uint32_t valveBoard_relase_firmware_len;
	uint32_t valveBoard_relase_firmware_crc;
}VALVECTRL_RELEASE_FIRMWARE_VER;
//********************************************************************************************************************

//************************************驱动板****复位比例阀板 0x26*****************************************************
#define INDEX_VALVECTRL_RESET       5


//********************************************************************************************************************

//************************************驱动板****自检比例阀板 0x27*****************************************************
#define INDEX_VALVECTRL_SELFCHECK       5


//********************************************************************************************************************

//全局的参数
typedef struct
{
	POWER_STATE         			power_state;
	REAL_PARA           			real_para;
	STE_PARA        	  			set_para;
//	SET_TEMP                  set_temp;
	SET_RTC                   set_RTC;
	SET_BAUD_RATE             set_baud_rate;
	VALVE_PARA                valve_para;
	RUN_STATE                 run_state;
	REALTIME_TEMP             realtime_temp;
	SELF_CHECK_ERROR_CODE			selfcheck_errCode;
	int16_t										dew_point_rt;	
	int16_t										dew_point_set;		
	uint32_t									run_time;
	uint16_t									water_h;
	uint16_t									water_l;
//	uint16_t									o2_rt;
	uint16_t									o2_set;
	uint16_t 									o2_high_limit_set;
	uint16_t									o2_low_limit_set;
	uint16_t									press_rt;
	uint8_t										patient_type;
	uint16_t									alarm_mute;
}_G_PARA;

typedef enum{
	PATIENT_ADULT,
	PATIENT_CHILD
}PATIENT_TYPE;


//系统运行命令
typedef enum{
	SYS_CMD_NONE			= 0,
//	SYS_CMD_STARTUP   = 1,
	SYS_CMD_PAUSE,
	SYS_CMD_RUNNING,
	SYS_CMD_SHUTDOWN,
	SYS_CMD_SEVICE,
	SYS_CMD_SELFCHECK,
}SYS_RUN_CMD;

//系统运行状态
typedef enum
{
	SYS_STATE_NONE     	= 0,
	SYS_STATE_PAUSE			= 1,
	SYS_STATE_RUNNING,
	SYS_STATE_SHUTDOWN,
	SYS_STATE_SEVICE,
	SYS_STATE_SELFCHECK,
}SYS_RUN_STATE;
//市电电压类型
typedef enum
{
 AC_220V_VOL,
 AC_110V_VOL,
}AC_TYPE;
//电池状态
typedef enum
{
 BATTERY_NONE,
 BATTERY_CHARGE,
 BATTERY_FULL,
}BATTERY_STATE;

#endif //_COMM_DEF_H

