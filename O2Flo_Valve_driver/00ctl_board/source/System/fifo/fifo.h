/*******************************************************************************
* �ļ���   :  fifo.h
* �������� :  
* ����     :  
* ����˵�� :  ���ݷ��ͻ�����
*******************************************************************************/
#ifndef  __FIFO_H__
#define  __FIFO_H__

/*******************************************************************************
*                                 ͷ�ļ�����
*******************************************************************************/
#include "datatype.h"

typedef struct {
	UINT16 gFifoWrPtr;
	UINT16 gFifoRdPtr;
	UINT16 gFifoCnt;
	UINT16 gFifoLen;
	UINT8* gFifoBuffer;
}FIFO_TYPE;

void fifoInit(FIFO_TYPE* ptr, UINT8* buffer, UINT16 lenght);
void fifoReset(FIFO_TYPE* ptr);
UINT16 fifoCnt(FIFO_TYPE* ptr);
BOOL fifoIsFull(FIFO_TYPE* ptr);
BOOL fifoWriteData(FIFO_TYPE* ptr, UINT8 *array, UINT16 length); //����д
UINT16 fifoForceWriteData(FIFO_TYPE* ptr, UINT8 *array, UINT16 length); //ǿ��д��
UINT16 fifoReadData(FIFO_TYPE* ptr, UINT8 *array, UINT16 length);
UINT16 fifoOnlyReadData(FIFO_TYPE* ptr, UINT8 *array, UINT16 length);
UINT16 fifoOnlyReadData_to_2Byte(FIFO_TYPE* ptr, UINT16 *array, UINT16 length);
BOOL fifoWriteData_to_2Byte(FIFO_TYPE* ptr, UINT16 *array, UINT16 length);
#endif
/*************************************����*************************************/
