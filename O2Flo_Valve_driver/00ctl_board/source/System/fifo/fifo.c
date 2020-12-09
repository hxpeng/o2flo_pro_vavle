/*******************************************************************************
* 版本号   :  1.0
* 文件名   :  fifo.cpp
* 生成日期 :  
* 作者     :  
* 功能说明 :  数据发送缓冲区
*******************************************************************************/

/*******************************************************************************
*                                 头文件包含
*******************************************************************************/
#include "fifo.h"
#include "datatype.h"
/*******************************************************************************
*                                内部全局变量
*******************************************************************************/

/*******************************************************************************
*                                  外部函数定义
*******************************************************************************/

/*******************************************************************************
* 函数名称 : fifoInit
* 功能描述 : 初始化fifo
* 输入参数 : array:源地址 length:长度
* 输出参数 : 无
* 返回参数 : 写入结果,TRUE:正确,FALSE:异常
*******************************************************************************/
void fifoInit(FIFO_TYPE* ptr, UINT8* buffer, UINT16 lenght)
{
	ptr->gFifoBuffer = buffer;
	ptr->gFifoCnt = 0;
	ptr->gFifoLen = lenght;
	ptr->gFifoRdPtr = 0;
	ptr->gFifoWrPtr = 0;
}
/*******************************************************************************
* 函数名称 : fifoReset
* 功能描述 : 复位fifo，数据清空
* 输入参数 : 
* 输出参数 : 无
* 返回参数 : 无
*******************************************************************************/
void fifoReset(FIFO_TYPE* ptr)
{
	ptr->gFifoCnt = 0;
	ptr->gFifoRdPtr = 0;
	ptr->gFifoWrPtr = 0;
}
/*******************************************************************************
* 函数名称 : fifoIsFull
* 功能描述 : 判断fifo是否为满
* 输入参数 : 
* 输出参数 : 无
* 返回参数 : 满-TRUE
*******************************************************************************/
BOOL fifoIsFull(FIFO_TYPE* ptr)
{
	return (ptr->gFifoCnt >= ptr->gFifoLen);
}

/*******************************************************************************
* 函数名称 : fifoCnt
* 功能描述 : 获得fifo保存数据的个数
* 输入参数 : 
* 输出参数 : 无
* 返回参数 : 满-TRUE
*******************************************************************************/
UINT16 fifoCnt(FIFO_TYPE* ptr)
{
	return ptr->gFifoCnt;
}

/*******************************************************************************
* 函数名称 : fifoWriteData
* 功能描述 : 写入length字节的数据
* 输入参数 : array:源地址 length:长度
* 输出参数 : 无
* 返回参数 : 写入结果,TRUE:正确,FALSE:异常
*******************************************************************************/
BOOL fifoWriteData(FIFO_TYPE* ptr, UINT8 *array, UINT16 length)
{    
	UINT16 i;
	BOOL flg;
	
	if(ptr->gFifoCnt > ptr->gFifoLen - length)
	{		
		flg = FALSE;
	}
	else
	{
		for(i = 0; i < length; i++)
		{            
			if(ptr->gFifoWrPtr >= ptr->gFifoLen)
				ptr->gFifoWrPtr = 0;  

			ptr->gFifoBuffer[ptr->gFifoWrPtr] = array[i];
			ptr->gFifoWrPtr++;
		}
		ptr->gFifoCnt += length;

		flg = TRUE;	
	}   

	return flg;
}

/*******************************************************************************
* 函数名称 : fifoReadData
* 功能描述 : 读取length字节的数据
* 输入参数 : array:目标地址 length:长度
* 输出参数 : 无
* 返回参数 : 写入结果,TRUE:正确,FALSE:异常
*******************************************************************************/
UINT16 fifoReadData(FIFO_TYPE* ptr, UINT8 *array, UINT16 length)
{
	UINT16 i;

	if(ptr->gFifoCnt <= length)
	{
			length = ptr->gFifoCnt; 
	}

	for(i = 0; i < length; i++)    
	{
			if(ptr->gFifoRdPtr >= ptr->gFifoLen)
			{
					ptr->gFifoRdPtr = 0;
			}      
			
			array[i] = ptr->gFifoBuffer[ptr->gFifoRdPtr];
			ptr->gFifoRdPtr++;
	}
	ptr->gFifoCnt -= length;	

	return length;
}
/*******************************************************************************
* 函数名称 : fifoWriteData
* 功能描述 : 写入length字节的数据
* 输入参数 : array:源地址 length:长度
* 输出参数 : 无
* 返回参数 : 写入结果,TRUE:正确,FALSE:异常
*******************************************************************************/
BOOL fifoWriteData_to_2Byte(FIFO_TYPE* ptr, UINT16 *array, UINT16 length)
{    
	UINT16 i;
	BOOL flg;
	
	if(ptr->gFifoCnt > ptr->gFifoLen - length)
	{		
		flg = FALSE;
	}
	else
	{
		for(i = 0; i < length; i++)
		{            
			if(ptr->gFifoWrPtr >= ptr->gFifoLen)
				ptr->gFifoWrPtr = 0;  

			ptr->gFifoBuffer[ptr->gFifoWrPtr] = (UINT8)(array[i]>>4);
			ptr->gFifoWrPtr++;
		}
		ptr->gFifoCnt += length;

		flg = TRUE;	
	}   

	return flg;
}

/*******************************************************************************
* 函数名称 : fifoReadData
* 功能描述 : 读取length字节的数据
* 输入参数 : array:目标地址 length:长度
* 输出参数 : 无
* 返回参数 : 写入结果,TRUE:正确,FALSE:异常
*******************************************************************************/
UINT16 fifoReadData_to_2Byte(FIFO_TYPE* ptr, UINT16 *array, UINT16 length)
{
	UINT16 i;

	if(ptr->gFifoCnt <= length)
	{
			length = ptr->gFifoCnt; 
	}

	for(i = 0; i < length; i++)    
	{
			if(ptr->gFifoRdPtr >= ptr->gFifoLen)
			{
					ptr->gFifoRdPtr = 0;
			}      
			
			array[i] = ptr->gFifoBuffer[ptr->gFifoRdPtr];
			ptr->gFifoRdPtr++;
	}
	ptr->gFifoCnt -= length;	

	return length;
}
/*******************************************************************************
* 函数名称 : fifoOnlyReadData
* 功能描述 : 仅读length字节的数据，而不取出
* 输入参数 : array:目标地址 length:长度
* 输出参数 : 无
* 返回参数 : 写入结果,TRUE:正确,FALSE:异常
*******************************************************************************/
UINT16 fifoOnlyReadData(FIFO_TYPE* ptr, UINT8 *array, UINT16 length)
{
	UINT16 i;
	UINT16 gFifoRdPtr;
	UINT16 gFifoCnt;
	
	gFifoRdPtr = ptr->gFifoRdPtr;
	gFifoCnt = ptr->gFifoCnt;

	if(ptr->gFifoCnt <= length)
	{
			length = ptr->gFifoCnt; 
	}

	for(i = 0; i < length; i++)    
	{
			if(ptr->gFifoRdPtr >= ptr->gFifoLen)
			{
					ptr->gFifoRdPtr = 0;
			}      
			
			array[i] = ptr->gFifoBuffer[ptr->gFifoRdPtr];
			ptr->gFifoRdPtr++;
	}
	ptr->gFifoCnt -= length;	
	
	ptr->gFifoRdPtr = gFifoRdPtr;
	ptr->gFifoCnt = gFifoCnt;

	return length;
}

/*******************************************************************************
* 函数名称 : fifoOnlyReadData_to_2Byte
* 功能描述 : 仅读length字节的数据，而不取出,存于16bit的数据
* 输入参数 : array:目标地址 length:长度
* 输出参数 : 无
* 返回参数 : 写入结果,TRUE:正确,FALSE:异常
*******************************************************************************/
UINT16 fifoOnlyReadData_to_2Byte(FIFO_TYPE* ptr, UINT16 *array, UINT16 length)
{
	UINT16 i;
	UINT16 gFifoRdPtr;
	UINT16 gFifoCnt;
	
	gFifoRdPtr = ptr->gFifoRdPtr;
	gFifoCnt = ptr->gFifoCnt;

	if(ptr->gFifoCnt <= length)
	{
			length = ptr->gFifoCnt; 
	}

	for(i = 0; i < length; i++)    
	{
			if(ptr->gFifoRdPtr >= ptr->gFifoLen)
			{
					ptr->gFifoRdPtr = 0;
			}      
			
			array[i] = (UINT16)(ptr->gFifoBuffer[ptr->gFifoRdPtr]&0x00ff);
			ptr->gFifoRdPtr++;
	}
	ptr->gFifoCnt -= length;	
	
	ptr->gFifoRdPtr = gFifoRdPtr;
	ptr->gFifoCnt = gFifoCnt;

	return length;
}

/*******************************************************************************
* 函数名称 : fifoWriteData
* 功能描述 : 强制写入length字节的数据，丢掉较早的数据
* 输入参数 : array:源地址 length:长度
* 输出参数 : 无
* 返回参数 : 写入结果,TRUE:正确,FALSE:异常
*******************************************************************************/
#if 1
UINT16 fifoForceWriteData(FIFO_TYPE* ptr, UINT8 *array, UINT16 length)
{
	INT16 diff_cnt;
	
	if(length > ptr->gFifoLen)
		return 0;
	
	//差值，即需要读出的个数
	diff_cnt = length - (ptr->gFifoLen - ptr->gFifoCnt);
	if(diff_cnt < 0)
		diff_cnt = 0;
 
	fifoReadData(ptr, array, diff_cnt);
	fifoWriteData(ptr, array, length);

	return length;
}
#else
/*******************************************************************************
* 函数名称 : fifoWriteData
* 功能描述 : 写入length字节的数据
* 输入参数 : array:源地址 length:长度
* 输出参数 : 无
* 返回参数 : 写入结果,TRUE:正确,FALSE:异常
*******************************************************************************/
UINT16 fifoForceWriteData(FIFO_TYPE* ptr, UINT8 *array, UINT16 length)
{    
	UINT16 i,len;
	BOOL flg;
	
	//计算需要删除的数据个数
	
	
	if(ptr->gFifoCnt > ptr->gFifoLen - length)
	{		
		flg = FALSE;
	}
	else
	{
		for(i = 0; i < length; i++)
		{            
			if(ptr->gFifoWrPtr >= ptr->gFifoLen)
				ptr->gFifoWrPtr = 0;  

			ptr->gFifoBuffer[ptr->gFifoWrPtr] = array[i];
			ptr->gFifoWrPtr++;
		}
		ptr->gFifoCnt += length;

		flg = TRUE;	
	}   

	return flg;
}
#endif
/*******************************************************************************
* 函数名称 : fifoWriteData
* 功能描述 : 强制写入length字节的数据，丢掉较早的数据
* 输入参数 : array:源地址 length:长度
* 输出参数 : 无
* 返回参数 : 写入结果,TRUE:正确,FALSE:异常
*******************************************************************************/
UINT16 fifoForceWriteData_to_2Byte(FIFO_TYPE* ptr, UINT16 *array, UINT16 length)
{
	INT16 diff_cnt;
	
	if(length > ptr->gFifoLen)
		return 0;
	
	//差值，即需要读出的个数
	diff_cnt = length - (ptr->gFifoLen - ptr->gFifoCnt);
	if(diff_cnt < 0)
		diff_cnt = 0;
 
	fifoReadData_to_2Byte(ptr, array, diff_cnt);
	fifoWriteData_to_2Byte(ptr, array, length);

	return length;
}
/*************************************结束*************************************/
