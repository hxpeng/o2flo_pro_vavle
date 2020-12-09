#include "bsp_iic2_flow.h"
#include "main.h"
#include "stm32f0xx_hal.h"
#include "cmsis_os.h"
#include "datatype.h"

//#define	 I2C_TIMEOUT  200

extern I2C_HandleTypeDef hi2c2;

uint8_t iic2_timeout = 0xf0;

BOOL IIC2_write(void)
{
//	iic2_timeout = I2C_TIMEOUT;
//		if(HAL_OK == HAL_I2C_Master_Receive(&hi2c2,0x18,&iic2_timeout,1,200))

	if(HAL_OK == HAL_I2C_Master_Transmit(&hi2c2,0x18,&iic2_timeout,1,200))
		return TRUE;
	else
		return FALSE;
}
