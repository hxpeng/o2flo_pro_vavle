/****************************************************
*��Ȩ���У�
*�ļ�����interface_module.c
*����˵������CubeMX���ɵ��ļ�/�����ӿڣ���main.c main.h �ж��ļ���֮���
*���ߣ������
*�������ڣ�2017/02/27
*****************************************************/

/*****************************************************
*ͷ�ļ�����
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
*�ֲ���������
******************************************************/

/******************************************************
*ȫ�ֱ�������
******************************************************/
uint16_t adc_buffer[ADC_TOTAL_LEN];

//����CubeMXϵͳ�����ľ��
extern ADC_HandleTypeDef hadc;
extern DMA_HandleTypeDef hdma_adc;
extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;
extern osThreadId defaultTaskHandle;
extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim3;
/******************************************************
*�ڲ���������
******************************************************/


/******************************************************
*��������
******************************************************/

/******************************************************
*�ⲿ��������
******************************************************/

/************************************************************************************
* ����ӿ�
************************************************************************************/
/***************************************************
*�������ƣ�XXX
*�������������printf����
*�����������
*�����������
*����ֵ����
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
*�������ƣ�interface_module_init_sys
*������������ʼ��ϵͳ��Ӳ��,��main.c�е���
*�����������
*�����������
*����ֵ����
****************************************************/
void interface_module_init_sys(void)
{
	//HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	//HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&adc_buffer, ADC_TOTAL_LEN);
	
	//Ӳ����ʼ��
	bsp_InitSFlash();	/* ��ʼ������Flash. �ú�����ʶ����FLASH�ͺ� */
	
	//����TIM3
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);//����ռ�ձ�Ϊ0
	
	//����ADC
	HAL_ADCEx_Calibration_Start(&hadc);

	HAL_ADC_Start(&hadc);
	HAL_ADC_Start_DMA(&hadc,(u32*)&adc_buffer,sizeof(ADC_TOTAL_LEN));
	/* ��⴮��Flash OK */
	printf("��⵽����Flash, ID = %08X, �ͺ�: %s \r\n", g_tSF.ChipID , g_tSF.ChipName);
	printf("    ���� : %dM�ֽ�, ������С : %d�ֽ�\r\n", g_tSF.TotalSize/(1024*1024), g_tSF.PageSize);
}

/***************************************************
*�������ƣ�interface_module_create_task
*������������������
*�����������
*�����������
*����ֵ����
****************************************************/
void interface_module_create_task(void)
{
	create_task();
}

/************************************************************************************
* Ӳ���ӿ�
************************************************************************************/

/*
*********************************************************************************************************
*	�� �� ��: interface_module_spi_write_byte
*	����˵��: ��SPI���߷���һ���ֽڡ�
*	��    ��:  ��
*	�� �� ֵ: ��
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
*	�� �� ��: interface_module_spi_read_byte
*	����˵��: ��SPI����һ���ֽڡ�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void interface_module_spi_read_byte(uint8_t* _ucByte)
{
	if(HAL_OK ==  HAL_SPI_Receive(&hspi1, _ucByte, 1, 1000))
	{
		
	}
}

/***************************************************
*�������ƣ�interface_module_get_16bits_adc_value
*�����������õ�ADC��ֵ  
*���������data
*�����������
*����ֵ��ADCֵ��16λ�������˹�����
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
*�������ƣ�interface_module_read_flash
*������������Flash ��
*���������data
*�����������
*����ֵ����
****************************************************/
void interface_module_read_flash(uint32_t addr, uint32_t *pdata, uint16_t len)
{
	uint16_t i;
	
	for(i = 0; i < len; i ++)
		pdata[i] = *(__IO uint32_t*)(addr+i*4);
}

/***************************************************
*�������ƣ�interface_module_write_flash
*����������дFlash ��
*���������data
*�����������
*����ֵ����
****************************************************/
void interface_module_write_flash(uint32_t addr, uint32_t *pdata, uint16_t len)
{
	uint16_t i;
	
	//1������FLASH
	HAL_FLASH_Unlock();

    //2������FLASH
    //��ʼ��FLASH_EraseInitTypeDef
    FLASH_EraseInitTypeDef f;
    f.TypeErase = FLASH_TYPEERASE_PAGES;
    f.PageAddress = addr;
    f.NbPages = 1;
    //����PageError
    uint32_t PageError = 0;
    //���ò�������
    HAL_FLASHEx_Erase(&f, &PageError);

    //3����FLASH��д
	for(i = 0; i < len; i ++)
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, pdata[i]);

    //4����סFLASH
	HAL_FLASH_Lock();
}

/***************************************************
*�������ƣ�interface_module_receive
*�������������ڽ�������
*���������data
*�����������
*����ֵ����
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
*�������ƣ�interface_module_transmit
*�������������ڷ�������
*���������data
*�����������
*����ֵ����
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
* �������� : GetUartReceiverResidualCnt
* �������� : ȡ����pdc������ʣ�����
* ������� : ��
* ������� : ��
* �������� :����pdc������ʣ����� 
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
*�������ƣ�interface_module_receive
*�������������ڽ�������
*���������data
*�����������
*����ֵ����
****************************************************/

