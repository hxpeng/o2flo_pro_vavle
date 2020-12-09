//==============================================================================
//    S E N S I R I O N   AG,  Laubisruetistr. 50, CH-8712 Staefa, Switzerland
//==============================================================================
// Project   :  SF05 Sample Code (V1.0)
// File      :  i2c_hal.c (V1.0)
// Author    :  RFU
// Date      :  07-Nov-2012
// Controller:  STM32F100RB
// IDE       :  礦ision V4.60.0.0
// Compiler  :  Armcc
// Brief     :  I2C hardware abstraction layer
//==============================================================================

//-- Includes ------------------------------------------------------------------
#include "i2c_hal.h"
#include "main.h"

etI2cHeader I2C_ADR     = 64; //64,   //sensirion   // default sensor I2C address  //0x31, //Memsic  //
etI2cHeader I2C_WRITE   = 0x00; // write bit in header
etI2cHeader I2C_READ    = 0x01; // read bit in header
etI2cHeader I2C_RW_MASK = 0x01; // bit position of read/write bit in header

//==============================================================================
void I2c_Init(void)
{

}
//对I2C的SDA口做输入输出切换
void i2c_io_change(IO_TYPE flg)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
		/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOF_CLK_ENABLE();
	if(flg)
	{
		/*Configure GPIO pin Output Level */
		HAL_GPIO_WritePin(GPIOF, FLOW_SDA_Pin, GPIO_PIN_SET);

		GPIO_InitStruct.Pin = FLOW_SDA_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	}
	else
	{
		GPIO_InitStruct.Pin = FLOW_SDA_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	}
}

void delay_us(u16 dly)
{
	u8 i;
	while(dly--)
	{
		for(i=0; i<5; i++);
	}
}

//==============================================================================
void I2c_Start(u8 channel){
//==============================================================================
        IIC1_SDA_H;;
        delay_us(2);
        IIC1_SCL_H;
        delay_us(2);
        IIC1_SDA_L;;
        delay_us(5);  // hold time start condition (t_HD;STA)
        IIC1_SCL_L;;
        delay_us(5);
}

//==============================================================================
void I2c_Stop(u8 channel){
//==============================================================================
      IIC1_SCL_L;;
      delay_us(2);
      IIC1_SDA_L;;
      delay_us(2);
      IIC1_SCL_H;
      delay_us(5);  // set-up time stop condition (t_SU;STO)
      IIC1_SDA_H;;
      delay_us(5);
}
//==============================================================================
etError I2c_WriteByte(u8 channel,u8t txByte){
//==============================================================================
  u8t     mask;
	static u8 flag = 0;
  etError error = NO_ERROR;
        for(mask = 0x80; mask > 0; mask >>= 1)// shift bit for masking (8 times)
        {
          if((mask & txByte) == 0)
          {
            IIC1_SDA_L; // masking txByte, write bit to SDA-Line
          }
          else
          {
            IIC1_SDA_H;   
          }
          delay_us(2);               // data set-up time (t_SU;DAT)
          IIC1_SCL_H;                         // generate clock pulse on SCL
          delay_us(5);               // SCL high time (t_HIGH)
          IIC1_SCL_L;
          delay_us(1);               // data hold time(t_HD;DAT)
        }
        IIC1_SDA_H;                           // release SDA-line
        IIC1_SCL_H;                           // clk #9 for ack
				i2c_io_change(IO_INPUT);
        delay_us(10);//delay_us(1);                 // data set-up time (t_SU;DAT)
				
        if(READ1_SDA) 
				{
					flag = 2;
					error = ACK_ERROR;       // check ack from i2c slave
				}
				else
				{
					flag = 3;
				}
				
        IIC1_SCL_L;
				i2c_io_change(IO_OUTPUT);
  delay_us(10);                // wait to see byte package on scope
  return error;                         // return error code
}

//==============================================================================
u8t I2c_ReadByte(u8 channel,etI2cAck ack){
//==============================================================================
  u8t mask;
  u8t rxByte = NO_ERROR;
        IIC1_SDA_H;                            // release SDA-line
		i2c_io_change(IO_INPUT);
        for(mask = 0x80; mask > 0; mask >>= 1) // shift bit for masking (8 times)
        { 
          IIC1_SCL_H;                          // start clock on SCL-line
          delay_us(10);//delay_us(3);                // SCL high time (t_HIGH)
          if(READ1_SDA) rxByte = rxByte | mask; // read bit
          IIC1_SCL_L;
          delay_us(2);                // data hold time(t_HD;DAT)
        }
			i2c_io_change(IO_OUTPUT);	
        if(ack == ACK)
        {
          IIC1_SDA_L;              // send acknowledge if necessary
        }
        else
        {
          IIC1_SDA_H;
        }
        delay_us(2);                  // data set-up time (t_SU;DAT)
        IIC1_SCL_H;                            // clk #9 for ack
        delay_us(5);                  // SCL high time (t_HIGH)
        IIC1_SCL_L;
        IIC1_SDA_H;       // release SDA-line               
  delay_us(10);                 // wait to see byte package on scope
  return rxByte;                         // return error code
}
