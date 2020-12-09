/*******************************************************************************
* ��Ȩ���� :  �����а��ٿƼ����޹�˾
* ��Ŀ���� :  һ�廯��������
* ��Ŀ��� :  2705I
* �汾��   :  1.0
* �ļ���   :  CMD_receive.h
* �������� :  2009.06.23
* ����     :  ���촺
* ����˵�� :  ���������
* ����˵�� :  ��
* �޸ļ�¼ :  1.0   �����ļ�
*******************************************************************************/
#ifndef  __CMD_RECEIVE_H__
#define  __CMD_RECEIVE_H__
/*******************************************************************************
*                                 ͷ�ļ�����
*******************************************************************************/
#include <DataType.h>
#include "interface_module.h"

typedef enum
{
	BUF1_LENGTH = 64,
	BUF2_LENGTH = 64
}BUF_LENGTH;

typedef struct
{
	UINT8  m_Buf2[BUF2_LENGTH];                //��������ַ�������������
	INT32  m_Buf2Wptr;                         //��������дָ��
	INT32  m_Buf2Rptr;                         //���������ָ��
	INT32  m_Cnt;
	INT32  m_Position;

	UINT8  m_Buf1[BUF1_LENGTH];                //��������ַ���������  	  
}CMD_Receive;

void Init_Receive(CMD_Receive *p_RX);
void Buf2Write(CMD_Receive* p_RX, UINT8 data);
UINT8 Buf2Read(CMD_Receive* p_RX);
INT32 GetBuf2Length(CMD_Receive* p_RX);
void ReceiveData(CMD_Receive* p_RX, COM_CNT com);
#endif
/*************************************����*************************************/
