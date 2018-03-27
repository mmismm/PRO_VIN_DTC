/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	�������ù�ʽ���㺯���Ͱ汾��Ϣ��ʾ����
History:
	<author>	<time>		<desc>
************************************************************************/
#ifndef _FORMULA_H
#define _FORMULA_H
#include "../interface/protocol_define.h"

/*����ļ����ṹ�壬��������ַ�����Ŀ������ַ������*/
typedef struct _STRUCT_STRING_UNIT
{
	byte cCompareData;		//Ҫ�Ƚϵ�����
	byte* pcOutputString;	//Ҫ������ַ���
} STRUCT_STRING_UNIT;

typedef struct _STRUCT_DIS_STRING
{
	byte cCompareDataSum;					//��Ҫ�Ƚ����ݸ���
	byte* pcDefaultOutputString;			//������Ҫ��ʱ������ַ���
	STRUCT_STRING_UNIT *pstStringUnitGroup;	//����������
} STRUCT_DIS_STRING;

void OneByteOperation( const byte *pcDsSource, double addnum, double subnum, double mulnum, double divnum, const byte* pcFormat, byte *pcDataOut );
void TwoByteOperation( const byte *pcDsSource, double addnum, double subnum, double mulnum, double divnum, byte Hfront, const byte* pcFormat, byte *pcDataOut );
void FourByteOperation( const byte *pcDsSource, double addnum, double subnum, double mulnum, double divnum, byte Hfront, const byte* pcFormat, byte *pcDataOut );
void ThreeByteOperationd( const byte *pcDsSource, double addnum, double subnum, double mulnum, double divnum, byte Hfront, const byte* pcFormat, byte *pcDataOut );
void DisplayString( const byte *pcDsSource, STRUCT_DIS_STRING* pstDisString, byte cDisStringOffset, byte cAndNum, byte cMoveToRightTimes, byte *pcDataOut );

void DisASCII( const byte *pcDsSource, int iValidLen, byte *pcDataOut );
void DisHex( const byte *pcDsSource, int iValidLen, byte *pcDataOut );


#endif

