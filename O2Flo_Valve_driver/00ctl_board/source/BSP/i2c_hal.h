//==============================================================================
//    S E N S I R I O N   AG,  Laubisruetistr. 50, CH-8712 Staefa, Switzerland
//==============================================================================
// Project   :  SF05 Sample Code (V1.0)
// File      :  i2c_hal.h (V1.0)
// Author    :  RFU
// Date      :  07-Nov-2012
// Controller:  STM32F100RB
// IDE       :  礦ision V4.60.0.0
// Compiler  :  Armcc
// Brief     :  I2C hardware abstraction layer 
//==============================================================================

#ifndef I2C_HAL_H
#define I2C_HAL_H

#include "datatype.h"
#include "cmsis_os.h"

//-- Enumerations --------------------------------------------------------------
// Error codes
typedef enum{
  NO_ERROR       = 0x00, // no error
  ACK_ERROR      = 0x01, // no acknowledgment error
  CHECKSUM_ERROR = 0x02  // checksum mismatch error
}etError;

typedef enum{
  IO_INPUT       = 0, // no error
	IO_OUTPUT
}IO_TYPE;

typedef unsigned char   u8t;      ///< range: 0 .. 255
//typedef signed char     i8t;      ///< range: -128 .. +127                                     
typedef unsigned short  u16t;     ///< range: 0 .. 65535
//typedef signed short    i16t;     ///< range: -32768 .. +32767                                      
typedef unsigned long   u32t;     ///< range: 0 .. 4'294'967'295
//typedef signed long     i32t;     ///< range: -2'147'483'648 .. +2'147'483'647                                     
typedef float           ft;       ///< range: +-1.18E-38 .. +-3.39E+38
//typedef double          dt;      ///< range:            .. +-1.79E+308

//IO port define:
// PB6  IIC1_SCL	Oxygen flow detect
// PB7  IIC1_SDA	
// PB8  IIC2_SCL	Air flow detect
// PB9  IIC2_SDA	
// PB10 IIC3_SCL	Option flow sensor I2C
// PB11 IIC3_SDA	

//IO操作函数	 
//#define IIC1_SCL    PDout(4) //SCL
//#define IIC1_SDA    PDout(5) //SDA	 
//#define READ1_SDA   PDin(5)  //输入SDA 

#define IIC1_SCL_H    HAL_GPIO_WritePin(FLOW_SCL_GPIO_Port, FLOW_SCL_Pin, GPIO_PIN_SET);
#define IIC1_SCL_L   	HAL_GPIO_WritePin(FLOW_SCL_GPIO_Port, FLOW_SCL_Pin, GPIO_PIN_RESET);
#define IIC1_SDA_H   	HAL_GPIO_WritePin(FLOW_SDA_GPIO_Port, FLOW_SDA_Pin, GPIO_PIN_SET);
#define IIC1_SDA_L   	HAL_GPIO_WritePin(FLOW_SDA_GPIO_Port, FLOW_SDA_Pin, GPIO_PIN_RESET);

#define READ1_SDA   HAL_GPIO_ReadPin(FLOW_SDA_GPIO_Port, FLOW_SDA_Pin)

////IO操作函数	 
//#define IIC2_SCL    PBout(8) //SCL
//#define IIC2_SDA    PBout(9) //SDA	 
//#define READ2_SDA   PBin(9)  //输入SDA 

////IO操作函数	 
//#define IIC3_SCL    PBout(10) //SCL
//#define IIC3_SDA    PBout(11) //SDA	 
//#define READ3_SDA   PBin(11)  //输入SDA 
//-- Defines -------------------------------------------------------------------
// I2C IO-Pins
// SDA on port B, bit 11
// #define SDA_LOW()  (GPIOB->BSRR = 0x08000000) // set SDA to low -//IIC3_SDA=0;
// #define SDA_OPEN() (GPIOB->BSRR = 0x00000800) // set SDA to open-drain -//IIC3_SDA=1;
// #define SDA_READ   (GPIOB->IDR & 0x0800) // read SDA -//READ3_SDA     // read SDA
// SCL on port B, bit 10
// #define SCL_LOW()  (GPIOB->BSRR = 0x04000000)  //IIC3_SCL=0;
// #define SCL_OPEN() (GPIOB->BSRR = 0x00000400)  //IIC3_SCL=1;
// #define SCL_READ   (GPIOB->IDR  & 0x0400)     // read SCL

//-- Enumerations --------------------------------------------------------------
// I2C header

//typedef enum{
 // I2C_ADR     = 64, //64,   //sensirion   // default sensor I2C address  //0x31, //Memsic  //
 // I2C_WRITE   = 0x00, // write bit in header
  //I2C_READ    = 0x01, // read bit in header
  //I2C_RW_MASK = 0x01  // bit position of read/write bit in header
//}etI2cHeader;

typedef u8 etI2cHeader;

// I2C acknowledge
typedef enum{
  ACK    = 0,
  NO_ACK = 1,
}etI2cAck;


void i2c_io_change(IO_TYPE flg);
//==============================================================================
void I2c_Init(void);
//==============================================================================
// Initializes the ports for I2C interface.
//------------------------------------------------------------------------------

//==============================================================================
void I2c_Start(u8 channel);
//==============================================================================
// Writes a start condition on I2C-Bus.
//------------------------------------------------------------------------------
// remark: Timing (delay) may have to be changed for different microcontroller.
//       _____
// SDA:       |_____
//       _______
// SCL:         |___

//==============================================================================
void I2c_Stop(u8 channel);
//==============================================================================
// Writes a stop condition on I2C-Bus.
//------------------------------------------------------------------------------
// remark: Timing (delay) may have to be changed for different microcontroller.
//              _____
// SDA:   _____|
//            _______
// SCL:   ___|

//==============================================================================
etError I2c_WriteByte(u8 channel,u8t txByte);
//==============================================================================
// Writes a byte to I2C-Bus and checks acknowledge.
//------------------------------------------------------------------------------
// input:  txByte       transmit byte
//
// return: error:       ACK_ERROR = no acknowledgment from sensor
//                      NO_ERROR  = no error
//
// remark: Timing (delay) may have to be changed for different microcontroller.

//==============================================================================
u8t I2c_ReadByte(u8 channel,etI2cAck ack);
//==============================================================================
// Reads a byte on I2C-Bus.
//------------------------------------------------------------------------------
// input:  ack          Acknowledge: ACK or NO_ACK
//
// return: rxByte
//
// remark: Timing (delay) may have to be changed for different microcontroller.

#endif
