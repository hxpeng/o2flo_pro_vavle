//==============================================================================
//    S E N S I R I O N   AG,  Laubisruetistr. 50, CH-8712 Staefa, Switzerland
//==============================================================================
// Project   :  SF05 Sample Code (V1.0)
// File      :  sf05.h (V1.0)
// Author    :  RFU
// Date      :  07-Nov-2012
// Controller:  STM32F100RB
// IDE       :  礦ision V4.60.0.0
// Compiler  :  Armcc
// Brief     :  Sensor Layer: Definitions of commands and functions for sensor
//                            access.
//==============================================================================

#ifndef SF05_H
#define SF05_H

//-- Includes ------------------------------------------------------------------
#include "i2c_hal.h"
#include "datatype.h"
//-- Defines -------------------------------------------------------------------
// CRC
#define POLYNOMIAL  0x131    // P(x) = x^8 + x^5 + x^4 + 1 = 100110001
#define SENIRION_TYPE 1
// masks
#define MASK_STATUS 0x3FFF   // bitmask for the status register
#define MASK_ID     0x0FFF   // bitmask for the id register

//-- Defines -------------------------------------------------------------------
// Offset and scale factors from datasheet (SFM3000).
#define OFFSET_FLOW 32000   //.0F   // offset flow
//#define OFFSET_TEMP 20000.0F   // offset temperature
#define SCALE_FLOW    1400  //140.0F   // scale factor flow --for air and N2    x10 for integer calculate.
#define SCALE_FLOW_O2  1400//1428   //  142.8F   // scale factor flow --for O2    x10 for integer calculate.
//#define SCALE_TEMP    100.0F   // scale factor temperature

//-- Enumerations --------------------------------------------------------------
//Sensirion Sensor Commands

typedef u8 etI2cHeader;
typedef u16 etCommands;

typedef enum{
	NO_SENSOR,
	SENIRION,
	MEMSIC
}SENSOR_TYPE;

//typedef enum{
//  NO_ERROR       = 0x00, // no error
//  ACK_ERROR      = 0x01, // no acknowledgment error
//  CHECKSUM_ERROR = 0x02  // checksum mismatch error
//}etError;
//==============================================================================
void SF05_Init(u8 channel,SENSOR_TYPE sensor);
//==============================================================================
// Initializes the I2C bus for communication with the sensor.
//------------------------------------------------------------------------------

//==============================================================================
etError SF05_WriteCommand(u8 channel,etCommands cmd, SENSOR_TYPE sensor);
//==============================================================================
// Writes command to the sensor.
//------------------------------------------------------------------------------
// input:  cmd          command which is to be written to the sensor
//
// return: error:       ACK_ERROR = no acknowledgment from sensor
//                      NO_ERROR  = no error

//==============================================================================
etError SF05_ReadCommandResult(u8 channel,u16t *result, SENSOR_TYPE sensor);
//==============================================================================
// Reads command results from sensor.
//------------------------------------------------------------------------------
// input:  *result      pointer to an integer where the result will be stored
//
// return: errror:      ACK_ERROR      = no acknowledgment from sensor
//                      CHECKSUM_ERROR = checksum mismatch
//                      NO_ERROR       = no error

//==============================================================================
etError SF05_ReadCommandResultWithTimeout(u8 channel,u8t maxRetries, u16t *result, SENSOR_TYPE sensor);
//==============================================================================
// Reads command results from sensor. If an error occurs, then the read will be
// repeated after a short wait (approx. 10ms).
//------------------------------------------------------------------------------
// input:  maxRetries   maximum number of retries
//         *result      pointer to an integer where the result will be stored  
//
// return: errror:      ACK_ERROR      = no acknowledgment from sensor
//                      CHECKSUM_ERROR = checksum mismatch
//                      NO_ERROR       = no error
//
// remark: This function is usefull for reading measurement results. If not yet
//         a new valid measurement was performed, an acknowledge error occurs
//         and the read will be automatical repeated until a valid measurement
//         could be read.

//==============================================================================
etError SF05_GetFlow(u8 channel,u16 offset, u16 scale, u16 *flow, SENSOR_TYPE sensor);
//==============================================================================
// Gets the flow from the sensor in a predefined unit. The "flow measurement"
// command will be automatical written to the sensor, if it is not already set.
//------------------------------------------------------------------------------
// input:  offset       offset flow
//         scale        scale factor flow
//         *flow        pointer to a floating point value, where the calculated
//                      flow will be stored
//
// return: errror:      ACK_ERROR      = no acknowledgment from sensor
//                      CHECKSUM_ERROR = checksum mismatch
//                      NO_ERROR       = no error
//
// remark: The result will be converted according to the following formula:
//         flow in predefined unit = (measurement_result - offset) / scale

//设置测量模式
etError SF05_SetMeasurement(u8 channel, SENSOR_TYPE sensor);
//==============================================================================
//etError SF05_GetTemp(ft offset, ft scale, ft *temperature);
//==============================================================================
// Gets the temperature from sensor in 癈. The "temperature measurement" command
// will be automatical written to the sensor, if it is not already set.
//------------------------------------------------------------------------------
// input:  offset       offset temperature
//         scale        scale factor temperature
//         *temperature pointer to a floating point value, where the calculated
//                      temperature will be stored
//
// return: errror:      ACK_ERROR      = no acknowledgment from sensor
//                      CHECKSUM_ERROR = checksum mismatch
//                      NO_ERROR       = no error
//
// remark: The result will be converted according to the following formula:
//         temperature in 癈 = (measurement_result - offset) / scale

//==============================================================================
etError SF05_GetStatus(u8 channel,u16t *status, SENSOR_TYPE sensor);
//==============================================================================
// Gets the status from the sensor. The "read status" command will be
// automatical written to the sensor, if it is not already set.
//------------------------------------------------------------------------------
// input:  *status      pointer to a integer, where the status will be stored
//
// return: errror:      ACK_ERROR      = no acknowledgment from sensor
//                      CHECKSUM_ERROR = checksum mismatch
//                      NO_ERROR       = no error

//==============================================================================
etError SF05_GetId(u8 channel,u16t *id);
//==============================================================================
// Gets the ID from the sensor. The "read id" command will be automatical 
// written to the sensor, if it is not already set.
//------------------------------------------------------------------------------
// input:  *id          pointer to a integer, where the id will be stored
//
// return: error:       ACK_ERROR      = no acknowledgment from sensor
//                      CHECKSUM_ERROR = checksum mismatch
//                      NO_ERROR       = no error

//==============================================================================
etError SF05_SoftReset(u8 channel, SENSOR_TYPE sensor);
//==============================================================================
// Forces a sensor reset without switching the power off and on again.
//------------------------------------------------------------------------------
// return: error:       ACK_ERROR      = no acknowledgment from sensor
//                      NO_ERROR       = no error

//==============================================================================
etError SF05_CheckCrc(u8t data[], u8t nbrOfBytes, u8t checksum);
//==============================================================================
// Calculates checksum for n bytes of data and compares it with expected
// checksum.
//------------------------------------------------------------------------------
// input:  data[]       checksum is built based on this data
//         nbrOfBytes   checksum is built for n bytes of data
//         checksum     expected checksum
//
// return: error:       CHECKSUM_ERROR = checksum does not match
//                      NO_ERROR       = checksum matches

#endif



