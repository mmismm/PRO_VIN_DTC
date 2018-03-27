/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	���峣�ù�ʽ���㺯���Ͱ汾��Ϣ��ʾ����
History:
	<author>	<time>		<desc>
************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <string.h>
#include "formula.h"
#include "../interface/protocol_define.h"

/*************************************************
Description:	һ���ֽڵļӼ��˳�����
Input:
	pcDsSource	ȡֵ��ʼ��ַ
	addnum		����
	subnum		����
	mulnum		����
	divnum		����
	pcFormat	��ӡ��ʽ

Output:	pcDataOut	�����������ַ
Return:	void
Others:
*************************************************/

void OneByteOperation( const byte *pcDsSource, double addnum, double subnum, double mulnum, double divnum,
                       const byte* pcFormat, byte *pcDataOut )
{
	int onebyte = *pcDsSource;
	double disnum = 0;

	if( divnum == 1 && mulnum == 1 )
		disnum = ( double )onebyte + addnum - subnum;
	else if( divnum != 1 && mulnum == 1 )
		disnum = ( double )onebyte / divnum + addnum - subnum;
	else if( mulnum != 1 && divnum == 1 )
		disnum = ( double )onebyte * mulnum + addnum - subnum;
	else
		disnum = ( double )onebyte * mulnum / divnum + addnum - subnum;

	/*sprintf_s( pcDataOut, 20, pcFormat, disnum );*/
	sprintf( pcDataOut, pcFormat, disnum );
}
/*************************************************
Description:	�����ֽڵļӼ��˳�����
Input:
	pcDsSource	ȡֵ��ʼ��ַ
	addnum		����
	subnum		����
	mulnum		����
	divnum		����
	Hfront		�ߵ�λ��־
					0:С�ˣ���λ�ڸߵ�ַ
					1:��ˣ���λ�ڵ͵�ַ
	pcFormat	��ӡ��ʽ

Output:	pcDataOut	�����������ַ
Return:	void
Others:
*************************************************/

void TwoByteOperation( const byte *pcDsSource, double addnum, double subnum, double mulnum, double divnum, byte Hfront,
                       const byte* pcFormat, byte *pcDataOut )
{
	uint16 twobyte = 0;
	double disnum = 0;

	if( Hfront == 0 ) //С�ˣ���λ�ڸߵ�ַ
	{
		twobyte = ( uint16 ) * ( ++pcDsSource ) << 8;
		twobyte |= ( uint8 ) * ( --pcDsSource );
	}
	else if( Hfront == 1 )
	{
		twobyte = ( uint16 ) * pcDsSource << 8;
		twobyte |= ( uint8 ) * ( ++pcDsSource );
	}

	if( divnum == 1 && mulnum == 1 )
		disnum = ( double )twobyte + addnum - subnum;
	else if( divnum != 1 && mulnum == 1 )
		disnum = ( double )twobyte / divnum + addnum - subnum;
	else if( mulnum != 1 && divnum == 1 )
		disnum = ( double )twobyte * mulnum + addnum - subnum;
	else
		disnum = ( double )twobyte * mulnum / divnum + addnum - subnum;

	/*sprintf_s( pcDataOut, 20, pcFormat, disnum );*/
	sprintf( pcDataOut, pcFormat, disnum );
}
/*************************************************
Description:	�ĸ��ֽڵļӼ��˳�����
Input:
	pcDsSource	ȡֵ��ʼ��ַ
	addnum		����
	subnum		����
	mulnum		����
	divnum		����
	Hfront		�ߵ�λ��־
					0:С�ˣ���λ�ڸߵ�ַ
					1:��ˣ���λ�ڵ͵�ַ
	pcFormat	��ӡ��ʽ

Output:	pcDataOut	�����������ַ
Return:	void
Others:	���������32λ eg.XYZW*2/65536
*************************************************/
void FourByteOperation( const byte *pcDsSource, double addnum, double subnum, double mulnum, double divnum, byte Hfront,
                        const byte* pcFormat, byte *pcDataOut )
{
	byte i;
	UNN_2WORD_4BYTE num;
	double dinum = 0;

	if( Hfront )
	{
		i = 4;

		while( i-- )
			num.u8Bit[i] = pcDsSource[i];
	}
	else
	{
		i = 0;

		while( i++ != 4 )
			num.u8Bit[i - 1] = pcDsSource[4 - i];
	}

	dinum = ( double )num.u32Bit;

	if( divnum == 1 && mulnum == 1 )
		dinum = dinum + addnum - subnum;
	else if( divnum != 1 && mulnum == 1 )
		dinum = dinum / divnum + addnum - subnum;
	else if( mulnum != 1 && divnum == 1 )
		dinum = dinum * mulnum + addnum - subnum;
	else
		dinum = dinum * mulnum / divnum + addnum - subnum;

	/*sprintf_s( pcDataOut, 20, pcFormat, dinum );*/
	sprintf( pcDataOut, pcFormat, dinum );
}
/*************************************************
Description:	�����ֽڵļӼ��˳�����
Input:
	pcDsSource	ȡֵ��ʼ��ַ
	addnum		����
	subnum		����
	mulnum		����
	divnum		����
	Hfront		�ߵ�λ��־
					0:С�ˣ���λ�ڸߵ�ַ
					1:��ˣ���λ�ڵ͵�ַ
	pcFormat	��ӡ��ʽ

Output:	pcDataOut	�����������ַ
Return:	void
Others:	���������32λ
*************************************************/
void ThreeByteOperationd( const byte *pcDsSource, double addnum, double subnum, double mulnum, double divnum, byte Hfront,
                          const byte* pcFormat, byte *pcDataOut )
{
	byte i;
	double dinum = 0;
	UNN_2WORD_4BYTE num;

	num.u32Bit = 0;

	if( Hfront )
	{
		i = 3;

		while( i-- )
			num.u8Bit[i] = pcDsSource[i];
	}
	else
	{
		i = 0;

		while( i < 3 )
		{
			num.u8Bit[i] = pcDsSource[2 - i];
			i++;
		}

	}

	dinum = ( double )num.u32Bit;

	if( divnum == 1 && mulnum == 1 )
		dinum = dinum + addnum - subnum;
	else if( divnum != 1 && mulnum == 1 )
		dinum = dinum / divnum + addnum - subnum;
	else if( mulnum != 1 && divnum == 1 )
		dinum = dinum * mulnum + addnum - subnum;
	else
		dinum = dinum * mulnum / divnum + addnum - subnum;

	/*sprintf_s( pcDataOut, 20, pcFormat, dinum );*/
	sprintf( pcDataOut, pcFormat, dinum );
}

/*************************************************
Description:	�ַ�����ʾ����
Input:
	pcDsSource			ȡֵ��ʼ��ַ
	pstDisString		��ʾ�ַ���������
	cDisStringOffset	��ʾ�ַ���ƫ��
	cAndNum				������
	cMoveToRightTimes	���ƴ���

Output:	pcDataOut	�����������ַ
Return:	void
Others:
*************************************************/
void DisplayString( const byte *pcDsSource, STRUCT_DIS_STRING* pstDisString, byte cDisStringOffset, byte cAndNum,
                    byte cMoveToRightTimes, byte *pcDataOut )
{
	STRUCT_STRING_UNIT *pstStringUnitTemp;
	byte i = 0;
	byte cNeedCompareData = 0;
	bool bFind = false;

	pstStringUnitTemp = pstDisString[cDisStringOffset].pstStringUnitGroup;
	cNeedCompareData = *pcDsSource & cAndNum;
	cNeedCompareData >>= cMoveToRightTimes;

	for( i = 0; i < pstDisString[cDisStringOffset].cCompareDataSum; i++ )
	{
		if( cNeedCompareData == pstStringUnitTemp->cCompareData )
		{
			bFind = true;
			break;
		}

		pstStringUnitTemp++;
	}

	if( bFind ) //�ж��Ƿ��ҵ�
	{
		memcpy( pcDataOut, pstStringUnitTemp->pcOutputString, strlen( pstStringUnitTemp->pcOutputString ) );

		pcDataOut[ strlen( pstStringUnitTemp->pcOutputString ) ] = '\0';
	}
	else
	{
		memcpy(	pcDataOut,
		        pstDisString[cDisStringOffset].pcDefaultOutputString,
		        strlen( pstDisString[cDisStringOffset].pcDefaultOutputString )
		      );

		pcDataOut[ strlen( pstDisString[cDisStringOffset].pcDefaultOutputString ) ] = '\0';
	}

}

/*************************************************
Description:	��ASII�뷽ʽ��ʾ
Input:
	pcDsSource	ȡֵ��ʼ��ַ
	iValidLen	��Ч���ȣ����ʽ���ĳ��ȣ�

Output:	pcDataOut	��������ַ
Return:	void
Others:
*************************************************/
void DisASCII( const byte *pcDsSource, int iValidLen, byte *pcDataOut )
{
	memcpy( pcDataOut, pcDsSource, iValidLen );
	pcDataOut[iValidLen] = '\0';
}
/*************************************************
Description:	��DisHex�뷽ʽ��ʾ
Input:
	pcDsSource	ȡֵ��ʼ��ַ
	iValidLen	��Ч���ȣ����ʽ���ĳ��ȣ�

Output:	pcDataOut	��������ַ
Return:	void
Others:
*************************************************/

void DisHex( const byte *pcDsSource, int iValidLen, byte *pcDataOut )
{
	int i = 0;

	for( ; i < iValidLen; i++ )
	{
		/*sprintf_s( &pcDataOut[i * 2], ( iValidLen * 2 + 1 - i * 2 ), "%02X", pcDsSource[i] );*/
		sprintf( &pcDataOut[i * 2], "%02X", pcDsSource[i] );
	}
}

