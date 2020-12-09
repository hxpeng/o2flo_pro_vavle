/*******************************************************************************
* �汾��   :  1.0
* �ļ���   :  fifo.cpp
* �������� :  
* ����     :  
* ����˵�� :  ���ݷ��ͻ�����
*******************************************************************************/

/*******************************************************************************
*                                 ͷ�ļ�����
*******************************************************************************/
#include "fifo.h"
#include "datatype.h"
/*******************************************************************************
*                                �ڲ�ȫ�ֱ���
*******************************************************************************/

/*******************************************************************************
*                                  �ⲿ��������
*******************************************************************************/

/*******************************************************************************
* �������� : fifoInit
* �������� : ��ʼ��fifo
* ������� : array:Դ��ַ length:����
* ������� : ��
* ���ز��� : д����,TRUE:��ȷ,FALSE:�쳣
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
* �������� : fifoReset
* �������� : ��λfifo���������
* ������� : 
* ������� : ��
* ���ز��� : ��
*******************************************************************************/
void fifoReset(FIFO_TYPE* ptr)
{
	ptr->gFifoCnt = 0;
	ptr->gFifoRdPtr = 0;
	ptr->gFifoWrPtr = 0;
}
/*******************************************************************************
* �������� : fifoIsFull
* �������� : �ж�fifo�Ƿ�Ϊ��
* ������� : 
* ������� : ��
* ���ز��� : ��-TRUE
*******************************************************************************/
BOOL fifoIsFull(FIFO_TYPE* ptr)
{
	return (ptr->gFifoCnt >= ptr->gFifoLen);
}

/*******************************************************************************
* �������� : fifoCnt
* �������� : ���fifo�������ݵĸ���
* ������� : 
* ������� : ��
* ���ز��� : ��-TRUE
*******************************************************************************/
UINT16 fifoCnt(FIFO_TYPE* ptr)
{
	return ptr->gFifoCnt;
}

/*******************************************************************************
* �������� : fifoWriteData
* �������� : д��length�ֽڵ�����
* ������� : array:Դ��ַ length:����
* ������� : ��
* ���ز��� : д����,TRUE:��ȷ,FALSE:�쳣
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
* �������� : fifoReadData
* �������� : ��ȡlength�ֽڵ�����
* ������� : array:Ŀ���ַ length:����
* ������� : ��
* ���ز��� : д����,TRUE:��ȷ,FALSE:�쳣
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
* �������� : fifoWriteData
* �������� : д��length�ֽڵ�����
* ������� : array:Դ��ַ length:����
* ������� : ��
* ���ز��� : д����,TRUE:��ȷ,FALSE:�쳣
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
* �������� : fifoReadData
* �������� : ��ȡlength�ֽڵ�����
* ������� : array:Ŀ���ַ length:����
* ������� : ��
* ���ز��� : д����,TRUE:��ȷ,FALSE:�쳣
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
* �������� : fifoOnlyReadData
* �������� : ����length�ֽڵ����ݣ�����ȡ��
* ������� : array:Ŀ���ַ length:����
* ������� : ��
* ���ز��� : д����,TRUE:��ȷ,FALSE:�쳣
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
* �������� : fifoOnlyReadData_to_2Byte
* �������� : ����length�ֽڵ����ݣ�����ȡ��,����16bit������
* ������� : array:Ŀ���ַ length:����
* ������� : ��
* ���ز��� : д����,TRUE:��ȷ,FALSE:�쳣
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
* �������� : fifoWriteData
* �������� : ǿ��д��length�ֽڵ����ݣ��������������
* ������� : array:Դ��ַ length:����
* ������� : ��
* ���ز��� : д����,TRUE:��ȷ,FALSE:�쳣
*******************************************************************************/
#if 1
UINT16 fifoForceWriteData(FIFO_TYPE* ptr, UINT8 *array, UINT16 length)
{
	INT16 diff_cnt;
	
	if(length > ptr->gFifoLen)
		return 0;
	
	//��ֵ������Ҫ�����ĸ���
	diff_cnt = length - (ptr->gFifoLen - ptr->gFifoCnt);
	if(diff_cnt < 0)
		diff_cnt = 0;
 
	fifoReadData(ptr, array, diff_cnt);
	fifoWriteData(ptr, array, length);

	return length;
}
#else
/*******************************************************************************
* �������� : fifoWriteData
* �������� : д��length�ֽڵ�����
* ������� : array:Դ��ַ length:����
* ������� : ��
* ���ز��� : д����,TRUE:��ȷ,FALSE:�쳣
*******************************************************************************/
UINT16 fifoForceWriteData(FIFO_TYPE* ptr, UINT8 *array, UINT16 length)
{    
	UINT16 i,len;
	BOOL flg;
	
	//������Ҫɾ�������ݸ���
	
	
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
* �������� : fifoWriteData
* �������� : ǿ��д��length�ֽڵ����ݣ��������������
* ������� : array:Դ��ַ length:����
* ������� : ��
* ���ز��� : д����,TRUE:��ȷ,FALSE:�쳣
*******************************************************************************/
UINT16 fifoForceWriteData_to_2Byte(FIFO_TYPE* ptr, UINT16 *array, UINT16 length)
{
	INT16 diff_cnt;
	
	if(length > ptr->gFifoLen)
		return 0;
	
	//��ֵ������Ҫ�����ĸ���
	diff_cnt = length - (ptr->gFifoLen - ptr->gFifoCnt);
	if(diff_cnt < 0)
		diff_cnt = 0;
 
	fifoReadData_to_2Byte(ptr, array, diff_cnt);
	fifoWriteData_to_2Byte(ptr, array, length);

	return length;
}
/*************************************����*************************************/
