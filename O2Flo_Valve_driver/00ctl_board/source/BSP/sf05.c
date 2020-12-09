//==============================================================================
//    S E N S I R I O N   AG,  Laubisruetistr. 50, CH-8712 Staefa, Switzerland
//==============================================================================
// Project   :  SF05 Sample Code (V1.0)
// File      :  sf05.c (V1.0)
// Author    :  RFU
// Date      :  07-Nov-2012
// Controller:  STM32F100RB
// IDE       :  礦ision V4.60.0.0
// Compiler  :  Armcc
// Brief     :  Sensor Layer: Implementation of functions for sensor access.
//==============================================================================

//-- Includes ------------------------------------------------------------------
#include "sf05.h"
#include "cmsis_os.h"
//-- Global Variables ----------------------------------------------------------
u16t currentCommand_ch1 = 0x0000;
u16t currentCommand_ch2 = 0x0000;
u16t currentCommand_ch3 = 0x0000;

etCommands  FLOW_MEASUREMENT = 0x1000; // Sensirion command: flow measurement
etCommands	READ_STATUS      = 0x1010;// command: read status
etCommands  READ_ID          = 0x7700; // command: read ID
etCommands  SOFT_RESET       = 0X2000;  // command: soft reset

extern etI2cHeader I2C_ADR;
extern etI2cHeader I2C_WRITE; // write bit in header
extern etI2cHeader I2C_READ; // read bit in header
extern etI2cHeader I2C_RW_MASK; // bit position of read/write bit in header

//==============================================================================
void SF05_Init(u8 channel, SENSOR_TYPE sensor){
//==============================================================================
		if(sensor == SENIRION){
		FLOW_MEASUREMENT = 0x1000; // Sensirion command: flow measurement
		READ_STATUS      = 0x1010;// command: read status
		READ_ID          = 0x7700; // command: read ID
		SOFT_RESET       = 0X2000;  // command: soft reset
		
		I2C_ADR = 64;
	}
	else if(sensor == MEMSIC)
	{
		FLOW_MEASUREMENT = 0xC1; // Sensirion command: flow measurement
		READ_STATUS      = 0x00;// command: read status
		READ_ID          = 0x7700; // command: read ID
		SOFT_RESET       = 0XFE;  // command: soft reset
		
		I2C_ADR = 0x31;			//0x70   0x31
		#if 0
		if(channel == EEPROM_CH)
		{
			I2C_ADR = 0x70;
		}
		else if(channel == SPEAKER_CH)
		{
			I2C_ADR = 0x31;//0x10;
		}
		else
		{
			I2C_ADR = 0x31;//0x09;
		}
		#endif
	}
}
#if SENIRION_TYPE
//==============================================================================
etError SF05_WriteCommand(u8 channel,etCommands cmd, SENSOR_TYPE sensor){
//==============================================================================
	etError error; // error code

	
	SF05_Init(channel, sensor);

	if(sensor == SENIRION)
	{
		// write command to sensor
		I2c_Start(channel);
		error  = I2c_WriteByte(channel,I2C_ADR << 1 | I2C_WRITE);
		error |= I2c_WriteByte(channel,cmd >> 8);
		error |= I2c_WriteByte(channel,cmd & 0xFF);
		I2c_Stop(channel);
		
		// if no error, store current command
		if(error == NO_ERROR)
		{
			if(channel==1)
				currentCommand_ch1 = cmd;
			else if(channel==2)
				currentCommand_ch2 = cmd;
			else if(channel==3)
				currentCommand_ch3 = cmd;
		}
		
	}
	else if(sensor == MEMSIC)
	{
 
		// write command to sensor
		I2c_Start(channel);
		error  = I2c_WriteByte(channel,I2C_ADR << 1 | I2C_WRITE);
		//error |= I2c_WriteByte(channel,cmd >> 8);
		error |= I2c_WriteByte(channel,cmd&0xFF);
		I2c_Stop(channel);
		
		// if no error, store current command
		if(error == NO_ERROR)
		{
			if(channel==1)
				currentCommand_ch1 = cmd;
			else if(channel==2)
				currentCommand_ch2 = cmd;
			else if(channel==3)
				currentCommand_ch3 = cmd;
		}
		
	}

	return error;
}

//==============================================================================
etError SF05_ReadCommandResult(u8 channel,u16t *result, SENSOR_TYPE sensor){
//==============================================================================
	etError error;    // error code
	SF05_Init(channel,sensor);
	
	
	if(sensor == SENIRION)
	{
		
		u8t     checksum; // checksum byte
		u8t     data[2];  // read data array
	 
		// read command result & checksum from sensor
		I2c_Start(channel);
		error    = I2c_WriteByte(channel,I2C_ADR << 1 | I2C_READ);
		data[0]  = I2c_ReadByte(channel,ACK);
		data[1]  = I2c_ReadByte(channel,ACK);
		checksum = I2c_ReadByte(channel,ACK);
		I2c_Stop(channel);
		
		// checksum verification
		error  |= SF05_CheckCrc (data, 2, checksum);
		
		// if no error, combine 16-bit result from the read data array
		if(error == NO_ERROR)
			*result = (data[0] << 8) | data[1];
		
		if(*result<OFFSET_FLOW) //20160804 add, avoid minus value for flow calculate.
			*result=OFFSET_FLOW;
		
	}
	else if(sensor == MEMSIC)
	{
		//  u8t     checksum; // checksum byte
		u8t     data[2];  // read data array
	 
		// read command result & checksum from sensor
		I2c_Start(channel);
		error    = I2c_WriteByte(channel,I2C_ADR << 1 | I2C_READ);
		data[0]  = I2c_ReadByte(channel,ACK);
		//data[1]  = I2c_ReadByte(channel,ACK);
		data[1]  = I2c_ReadByte(channel,NO_ACK);
		//  checksum = I2c_ReadByte(channel,ACK);
		I2c_Stop(channel);
		
		// checksum verification
		//error  |= SF05_CheckCrc (data, 2, checksum);
		
		// if no error, combine 16-bit result from the read data array
		if(error == NO_ERROR)
			*result = (data[0] << 8) | data[1];
	 /* 
		if(*result<OFFSET_FLOW) //20160804 add, avoid minus value for flow calculate.
			*result=OFFSET_FLOW;
	 */ 
		
	}
	
	return error;
}
#else
//MEMSIC
etError SF05_WriteCommand(u8 channel,etCommands cmd){
//==============================================================================
  etError error; // error code
 
  // write command to sensor
  I2c_Start(channel);
  error  = I2c_WriteByte(channel,I2C_ADR << 1 | I2C_WRITE);
  //error |= I2c_WriteByte(channel,cmd >> 8);
  error |= I2c_WriteByte(channel,cmd&0xFF);
  I2c_Stop(channel);
  
  // if no error, store current command
  if(error == NO_ERROR)
  {
    if(channel==1)
      currentCommand_ch1 = cmd;
    else if(channel==2)
      currentCommand_ch2 = cmd;
    else if(channel==3)
      currentCommand_ch3 = cmd;
  }
  
  return error;
}
//MEMSIC
//==============================================================================
etError SF05_ReadCommandResult(u8 channel,u16t *result){
//==============================================================================
  etError error;    // error code
//  u8t     checksum; // checksum byte
  u8t     data[2];  // read data array
 
  // read command result & checksum from sensor
  I2c_Start(channel);
  error    = I2c_WriteByte(channel,I2C_ADR << 1 | I2C_READ);
  data[0]  = I2c_ReadByte(channel,ACK);
  //data[1]  = I2c_ReadByte(channel,ACK);
  data[1]  = I2c_ReadByte(channel,NO_ACK);
//  checksum = I2c_ReadByte(channel,ACK);
  I2c_Stop(channel);
  
  // checksum verification
  //error  |= SF05_CheckCrc (data, 2, checksum);
  
  // if no error, combine 16-bit result from the read data array
  if(error == NO_ERROR)
    *result = (data[0] << 8) | data[1];
 /* 
  if(*result<OFFSET_FLOW) //20160804 add, avoid minus value for flow calculate.
    *result=OFFSET_FLOW;
 */ 
  return error;
}
#endif
//==============================================================================
etError SF05_ReadCommandResultWithTimeout(u8 channel,u8t maxRetries, u16t *result, SENSOR_TYPE sensor){
//==============================================================================
	etError error; //variable for error code
  
  while(maxRetries--)
  {
    // try to read command result
    error = SF05_ReadCommandResult(channel,result,sensor); 
    
    // if read command result was successful -> exit loop
    // it will only be successful if a new valid measurement was performed
    if(error == NO_ERROR) break;
    
    // if it was not successful -> wait a short time and then try it again
    //delay_us(1000);  //delay_us(10000);
		if(sensor == MEMSIC)
			vTaskDelay(10/portTICK_RATE_MS ); 
		else
			vTaskDelay(10/portTICK_RATE_MS ); 
  }

  return error;
}
 
//==============================================================================
etError SF05_GetFlow(u8 channel,u16 offset, u16 scale, u16 *flow, SENSOR_TYPE sensor){
//==============================================================================
	SF05_Init(channel,sensor);
	if(sensor == SENIRION)
	{
  etError error = NO_ERROR; // error code
  u16t    result;           // read result from sensor
  u16t    currentCommand;
	u8t 		maxRetries = 20;

    if(channel==1)
      currentCommand = currentCommand_ch1;
    else if(channel==2)
      currentCommand = currentCommand_ch2;
    else if(channel==3)
      currentCommand = currentCommand_ch3;  
  // write command if it is not already set     
//  if(currentCommand != FLOW_MEASUREMENT)
    error = SF05_WriteCommand(channel,FLOW_MEASUREMENT,sensor);
  vTaskDelay(10);
  // if no error, read command result
  if(error == NO_ERROR)
    error = SF05_ReadCommandResultWithTimeout(channel,20, &result,sensor);

  // if no error, compute the flow
  if(error == NO_ERROR)     //*flow = ((ft)result - offset) / scale;
    *flow = ((u16t)(result - offset)*1000) / scale; //flow = real flow volume x100，转化为单位10ml
    //*flow = (result - offset)*320 / scale; //flow = real flow volume x32
		return error;
	}
	else
	{
	etError error = NO_ERROR; // error code
  u16t    result;           // read result from sensor
	u32			temp = 0;
  u16t    currentCommand;
  
    if(channel==1)
      currentCommand = currentCommand_ch1;
    else if(channel==2)
      currentCommand = currentCommand_ch2;
    else if(channel==3)
      currentCommand = currentCommand_ch3;
    
  // write command if it is not already set     
  //if(currentCommand != FLOW_MEASUREMENT)
    //error = SF05_WriteCommand(channel,FLOW_MEASUREMENT);
			//delay_us(20000);
    error = SF05_WriteCommand(channel,READ_STATUS,sensor);
  // if no error, read command result
  if(error == NO_ERROR)
    error = SF05_ReadCommandResultWithTimeout(channel,1, &result,sensor);

//   if(error) //161025. write once more.
//   {
//     error |= SF05_SoftReset(channel); // perform a soft reset on the sensor 
//     error = SF05_ReadCommandResultWithTimeout(channel,20, &result);
//   }
  // if no error, compute the flow
  if(error == NO_ERROR)     //*flow = ((ft)result - offset) / scale;
      {
				*flow = (result)/ 10;  //转化为单位10ml
				temp = *flow;
				
				if(temp>3200)
					temp = 0;
				
				#if 0
				if(channel == EEPROM_CH)
					temp = temp*250/100;
				else
					temp = temp*125/100;
				#else
					
				temp = temp*250/100;
//				temp = temp*3125/1000;
				#endif
				
				
				*flow = temp;
			}
	
		return error;
	}
}

etError SF05_SetMeasurement(u8 channel, SENSOR_TYPE sensor)
{
	SF05_Init(channel,sensor);
	
	SF05_WriteCommand(channel,FLOW_MEASUREMENT,sensor);
	
	return NO_ERROR;
}

/*
//==============================================================================
etError SF05_GetFlow(u8 channel,ft offset, ft scale, ft *flow){
//==============================================================================
  etError error = NO_ERROR; // error code
  u16t    result;           // read result from sensor
  u16t    currentCommand;
  
    if(channel==1)
      currentCommand = currentCommand_ch1;
    else if(channel==2)
      currentCommand = currentCommand_ch2;
    else if(channel==3)
      currentCommand = currentCommand_ch3;
    
  // write command if it is not already set     
  if(currentCommand != FLOW_MEASUREMENT)
    error = SF05_WriteCommand(channel,FLOW_MEASUREMENT);
  
  // if no error, read command result
  if(error == NO_ERROR)
    SF05_ReadCommandResultWithTimeout(channel,20, &result);

  // if no error, compute the flow
  if(error == NO_ERROR)
    *flow = ((ft)result - offset) / scale;
  
  return error;
}
*/
// //==============================================================================
// etError SF05_GetTemp(ft offset, ft scale, ft *temperature){
// //==============================================================================
//   etError error = NO_ERROR; // error code
//   u16t    result;           // read result from sensor
//   
//   // write command if it is not already set 
//   if(currentCommand != TEMP_MEASUREMENT)
//     error = SF05_WriteCommand(TEMP_MEASUREMENT);
//   
//   // if no error, read command result
//   if(error == NO_ERROR)
//     SF05_ReadCommandResultWithTimeout(20, &result);
//   
//   // if no error, compute the temperature
//   if(error == NO_ERROR)
//     *temperature = ((ft)result - offset) / scale;
//   
//   return error;
// }

//==============================================================================
etError SF05_GetStatus(u8 channel,u16t *status, SENSOR_TYPE sensor){
//=============================================================================
	etError error = NO_ERROR; // error code
  u16t    result;           // read result from sensor
  u16t    currentCommand;
  
    if(channel==1)
      currentCommand = currentCommand_ch1;
    else if(channel==2)
      currentCommand = currentCommand_ch2;
    else if(channel==3)
      currentCommand = currentCommand_ch3;
    
  // write command if it is not already set 
  if(currentCommand != READ_STATUS)
    error = SF05_WriteCommand(channel,READ_STATUS,sensor);
  if(error) //add 161018. write once more.
  {
    error |= SF05_SoftReset(channel,sensor); // perform a soft reset on the sensor 
    error = SF05_WriteCommand(channel,READ_STATUS,sensor);
  }
  
  // if no error, read command result
  if(error == NO_ERROR)
    error = SF05_ReadCommandResult(channel,&result,sensor);
  
  // if no error, extract status from the lower 14 bits
  if(error == NO_ERROR)
    *status = result & MASK_STATUS;
  
  return error;
}
#ifdef SENIRION_TYPE
//==============================================================================
etError SF05_GetId(u8 channel,u16t *id){
//==============================================================================
  etError error = NO_ERROR; // error code
  u16t result;              // read result from sensor
  u16t    currentCommand;
  
    if(channel==1)
      currentCommand = currentCommand_ch1;
    else if(channel==2)
      currentCommand = currentCommand_ch2;
    else if(channel==3)
      currentCommand = currentCommand_ch3;
    
  // write command if it is not already set 
  if(currentCommand != READ_ID)
    error = SF05_WriteCommand(channel,READ_ID,SENIRION);
  
  // if no error, read command result
  if(error == NO_ERROR)
    error = SF05_ReadCommandResult(channel,&result,SENIRION);
  
  // if no error, extract id from the lower 12 bits
  if(error == NO_ERROR)
    *id = result & MASK_ID;
  
  return error;
}
#endif
//==============================================================================
etError SF05_SoftReset(u8 channel, SENSOR_TYPE sensor){
//==============================================================================
	etError error; // error code
  
  error = SF05_WriteCommand(channel,SOFT_RESET,sensor);
  
  return error;
}

//==============================================================================
etError SF05_CheckCrc(u8t data[], u8t nbrOfBytes, u8t checksum){
//==============================================================================
  u8t bit;     // bit mask
  u8t crc = 0; // calculated checksum
  u8t byteCtr; // byte counter
  
  // calculates 8-Bit checksum with given polynomial
  for(byteCtr = 0; byteCtr < nbrOfBytes; byteCtr++)
  {
    crc ^= (data[byteCtr]);
    for(bit = 8; bit > 0; --bit)
    {
      if(crc & 0x80) crc = (crc << 1) ^ POLYNOMIAL;
      else           crc = (crc << 1);
    }
  }
  
  // verify checksum
  if(crc != checksum) return CHECKSUM_ERROR;
  else                return NO_ERROR;
}

