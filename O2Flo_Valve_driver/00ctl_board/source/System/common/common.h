/****************************************************
*版权所有：深圳永胜宏基医疗器械有限公司
*文件名：
*功能说明：
*作者：
*生成日期：
*修改记录：//修改了则写
*修改者/日期：//修改了则写
*****************************************************/
#ifndef  __COMMON_H_
#define  __COMMON_H_

/*****************************************************
*头文件包含
*****************************************************/
#include "datatype.h"

/******************************************************
*内部宏定义
******************************************************/

/******************************************************
*内部类型定义
******************************************************/
#ifndef u8
#define u8 UINT8
#endif

#ifndef s8
#define s8 INT8
#endif

#ifndef u16
#define u16 UINT16
#endif

#ifndef s16
#define s16 INT16
#endif

#ifndef u32
#define u32 UINT32
#endif

#ifndef s32
#define s32 INT32
#endif


/******************************************************
*外部函数声明
******************************************************/
void bubble_sort(u16* array, u8 len);
int com_atio(char *String);
s16 com_avg(s16* array, s16 len, s16 val);
void com_shift(s16* array, s16 len, s16 val);
s16 com_only_avg(s16* array, s16 len);
s16 com_shift_avg(s16* array, s16* temp_array, u8 len, u8 shift, s16 val);
#endif
