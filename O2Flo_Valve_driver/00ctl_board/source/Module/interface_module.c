/****************************************************
*版权所有：
*文件名：interface_module.c
*功能说明：跟CubeMX生成的文件/库做接口，如main.c main.h 中断文件夹之类的
*作者：软件组
*生成日期：2017/02/27
*****************************************************/

/*****************************************************
*头文件包含
******************************************************/ 
#include "main.h"
#include "stm32f0xx_hal.h"
#include "cmsis_os.h"

#include "string.h"
#include "app.h" 
#include "interface_module.h"
#include "bsp_spi_flash.h"
#include "sampling_module.h"
/******************************************************
*局部变量定义
******************************************************/

/******************************************************
*全局变量定义
******************************************************/
uint16_t adc_buffer[ADC_TOTAL_LEN];

//引用CubeMX系统创建的句柄
extern ADC_HandleTypeDef hadc;
extern DMA_HandleTypeDef hdma_adc;
extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;
extern osThreadId defaultTaskHandle;
extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim3;
/******************************************************
*内部函数声明
******************************************************/


/******************************************************
*其他定义
******************************************************/

/******************************************************
*外部函数定义
******************************************************/

/************************************************************************************
* 软件接口
************************************************************************************/
/***************************************************
*函数名称：XXX
*功能描述：添加printf函数
*输入参数：无
*输出参数：无
*返回值：无
****************************************************/
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

/***************************************************
*函数名称：interface_module_init_sys
*功能描述：初始化系统软硬件,在main.c中调用
*输入参数：无
*输出参数：无
*返回值：无
****************************************************/
void interface_module_init_sys(void)
{
	//HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	//HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&adc_buffer, ADC_TOTAL_LEN);
	
	//硬件初始化
	bsp_InitSFlash();	/* 初始化串行Flash. 该函数会识别串行FLASH型号 */
	
	//启动TIM3
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);//设置占空比为0
	
	//启动ADC
	HAL_ADCEx_Calibration_Start(&hadc);

	HAL_ADC_Start(&hadc);
	HAL_ADC_Start_DMA(&hadc,(u32*)&adc_buffer,sizeof(ADC_TOTAL_LEN));
	/* 检测串行Flash OK */
	printf("检测到串行Flash, ID = %08X, 型号: %s \r\n", g_tSF.ChipID , g_tSF.ChipName);
	printf("    容量 : %dM字节, 扇区大小 : %d字节\r\n", g_tSF.TotalSize/(1024*1024), g_tSF.PageSize);
}

/***************************************************
*函数名称：interface_module_create_task
*功能描述：创建任务
*输入参数：无
*输出参数：无
*返回值：无
****************************************************/
void interface_module_create_task(void)
{
	create_task();
}

/************************************************************************************
* 硬件接口
************************************************************************************/

/*
*********************************************************************************************************
*	函 数 名: interface_module_spi_write_byte
*	功能说明: 向SPI总线发送一个字节。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void interface_module_spi_write_byte(uint8_t _ucByte)
{
	if(HAL_OK ==  HAL_SPI_Transmit(&hspi1, &_ucByte, 1, 1000))
	{
		
	}
}

/*
*********************************************************************************************************
*	函 数 名: interface_module_spi_read_byte
*	功能说明: 读SPI总线一个字节。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void interface_module_spi_read_byte(uint8_t* _ucByte)
{
	if(HAL_OK ==  HAL_SPI_Receive(&hspi1, _ucByte, 1, 1000))
	{
		
	}
}

/***************************************************
*函数名称：interface_module_get_16bits_adc_value
*功能描述：得到ADC的值  
*输入参数：data
*输出参数：无
*返回值：ADC值，16位，经过了过采样
****************************************************/
uint16_t interface_module_get_16bits_adc_value(ADC_CH ch)
{
	uint16_t  temp = 0, i;

	for(i = 0; i < ADC_OVERSAMPLING_LEN; i ++)
	{
		temp += adc_buffer[ADC_CH_MAX*i+ch];
	}

	return temp;
}

/***************************************************
*函数名称：interface_module_read_flash
*功能描述：读Flash 字
*输入参数：data
*输出参数：无
*返回值：无
****************************************************/
void interface_module_read_flash(uint32_t addr, uint32_t *pdata, uint16_t len)
{
	uint16_t i;
	
	for(i = 0; i < len; i ++)
		pdata[i] = *(__IO uint32_t*)(addr+i*4);
}

/***************************************************
*函数名称：interface_module_write_flash
*功能描述：写Flash 字
*输入参数：data
*输出参数：无
*返回值：无
****************************************************/
void interface_module_write_flash(uint32_t addr, uint32_t *pdata, uint16_t len)
{
	uint16_t i;
	
	//1、解锁FLASH
	HAL_FLASH_Unlock();

    //2、擦除FLASH
    //初始化FLASH_EraseInitTypeDef
    FLASH_EraseInitTypeDef f;
    f.TypeErase = FLASH_TYPEERASE_PAGES;
    f.PageAddress = addr;
    f.NbPages = 1;
    //设置PageError
    uint32_t PageError = 0;
    //调用擦除函数
    HAL_FLASHEx_Erase(&f, &PageError);

    //3、对FLASH烧写
	for(i = 0; i < len; i ++)
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, pdata[i]);

    //4、锁住FLASH
	HAL_FLASH_Lock();
}

/***************************************************
*函数名称：interface_module_receive
*功能描述：串口接收数据
*输入参数：data
*输出参数：无
*返回值：无
****************************************************/
void interface_module_receive(COM_CNT com, uint8_t* prx_buffer, uint16_t len)
{
	//COM1
//	if(com == COM1)
//	{
//		if(HAL_OK == HAL_UART_Receive_DMA(&huart1, prx_buffer, len))
//		{
//			
//		}
//		
//		return;
//	}
	
	//COM2
	if(HAL_OK == HAL_UART_Receive_DMA(&huart2, prx_buffer, len))
	{
		
	}
}

/***************************************************
*函数名称：interface_module_transmit
*功能描述：串口发送数据
*输入参数：data
*输出参数：无
*返回值：无
****************************************************/
void interface_module_transmit(COM_CNT com, uint8_t* prx_buffer, uint16_t len)
{
	#if 0
	//COM1
	if(com == COM1)
	{
		if(HAL_OK == HAL_UART_Transmit(&huart1, prx_buffer, len, 1000))
		{
			
		}
	}
	else	
	{
		//COM2
		if(HAL_OK == HAL_UART_Transmit(&huart2, prx_buffer, len, 1000))
		{
			
		}
	}
	#endif
	
	//COM2
	if(HAL_OK == HAL_UART_Transmit_DMA(&huart2, prx_buffer, len))
	{
		
	}
}
/*******************************************************************************
* 函数名称 : GetUartReceiverResidualCnt
* 功能描述 : 取串口pdc缓冲区剩余计数
* 输入参数 : 无
* 输出参数 : 无
* 函数返回 :串口pdc缓冲区剩余计数 
*******************************************************************************/
int GetUartReceiverResidualCnt(COM_CNT com)
{
	#if 0
	if(com == COM1)
		return __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
	else if(com == COM2)
		return __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
	else
	
		return 0;
	#endif
	
	return __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
}

/***************************************************
*函数名称：interface_module_receive
*功能描述：串口接收数据
*输入参数：data
*输出参数：无
*返回值：无
****************************************************/

