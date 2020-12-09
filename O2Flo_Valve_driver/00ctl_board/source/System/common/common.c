/****************************************************
*版权所有：
*文件名：main.c
*功能说明：初始化系统时钟、创建任务等
*作者：软件组
*生成日期：2017/02/27
*****************************************************/

/*****************************************************
*头文件包含
******************************************************/
#include "common.h"


/******************************************************
*局部变量定义
******************************************************/

/******************************************************
*全局变量定义
******************************************************/

/******************************************************
*内部函数声明
******************************************************/

/******************************************************
*其他定义
******************************************************/

/******************************************************
*内部函数定义
******************************************************/

/***************************************************
*函数名称：bubble_sort
*功能描述：冒泡排序，
*输入参数：array，len
*输出参数：无
*返回值：无
****************************************************/
void bubble_sort(u16* array, u8 len)
{
	u16 i,j;
	u16 temp;
	
	if(NULL == array)
		return;

	for(j=1;j <= (len-1);j++)
	{
		for(i = 0;i <= (len-1- j);i++ )
		{
			if(array[i] > array[i+1])
			{
				temp = array[i];
				array[i] = array[i+1];
				array[i+1] = temp;
			}
		}
	}
}
/***************************************************
*函数名称：com_atio
*功能描述：str->int，
*输入参数：String
*输出参数：无
*返回值：无
****************************************************/
int com_atio(char *String)
{
	int num = 0;
	int flag = 1;

	if(*String == '-')//判断正负号
	{
		flag = -1;
		String++;
	}

	while(*String != '\0' && *String >= '0' && *String <= '9')//判断是否为0到9
	{
		num = num * 10 + (*String - '0');//每次乘十就是向前一位再加上数字（用‘3’ - ‘0’算出所代表的数字）
		String++;
	}

	return num * flag;
}

/***************************************************
*函数名称：com_avg
*功能描述：做平均
*输入参数：String
*输出参数：无
*返回值：无
****************************************************/
s16 com_avg(s16* array, s16 len, s16 val)
{
	int sum = 0;
	int i;
	
	for(i = len-1; i > 0; i --)
	{
		array[i] = array[i-1];
		
		if(array[i] == 0)
			array[i] = val;
		
		sum += array[i];
	}
	
	array[0] = val;
	sum += val;

	return sum/len;
}

/***************************************************
*函数名称：com_only_avg
*功能描述：仅做平均
*输入参数：String
*输出参数：无
*返回值：无
****************************************************/
s16 com_only_avg(s16* array, s16 len)
{
	int sum = 0;
	int i;
	
	for(i = 0; i < len; i ++)
	{
		sum += array[i];
	}

	return sum/len;
}

/***************************************************
*函数名称：com_shift
*功能描述：将数据val按顺序加入缓存
*输入参数：String
*输出参数：无
*返回值：无
****************************************************/
void com_shift(s16* array, s16 len, s16 val)
{
	int sum = 0;
	int i;
	
	for(i = len-1; i > 0; i --)
	{
		array[i] = array[i-1];
		
		if(array[i] == 0)
			array[i] = val;
	}
	
	array[0] = val;
}

/***************************************************
*函数名称：com_shift_ave
*功能描述：移位平均，滤掉最大值、最小值
*输入参数：String
*输出参数：无
*返回值：无
****************************************************/
s16 com_shift_avg(s16* array, s16* temp_array, u8 len, u8 shift, s16 val)
{
	s16 temp;
	int i;
		
		//存入缓存
	com_shift(array, len, val);
	//赋值
	for(i = 0; i < len; i++)
		temp_array[i] = array[i];
	//排序
	bubble_sort((u16*)temp_array, len);
	//做平均
	temp = com_only_avg(&temp_array[2], (len-(shift<<1)) -2);
		
	return temp;
}
