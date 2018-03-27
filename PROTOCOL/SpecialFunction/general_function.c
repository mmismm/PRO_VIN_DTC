/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	�������⹦�ܴ�������ͨ�õ���غ���
History:
	<author>	<time>		<desc>

************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*#define NDEBUG*/
#include "special_function.h"
#include "..\InitConfigFromXml\init_config_from_xml_lib.h"
#include "..\interface\protocol_interface.h"


/*************************************************
Description:	������⹦�ܵ���������
Input:
	pcSource	����������ݵĵ�ַ���ַ�����
	iMaxLen	�������ݳ��ȣ��ַ�����

Output:	pcCmdData	����������ݵ�ַ
Return:	int		�������ݸ���
Others:	ֱ�Ӵ�xml��ȡ������������string���͵ģ�
		�Ҹ�ʽ����"00,10,00,01,01,25,07,11"
*************************************************/
int get_string_type_data_to_byte( byte* pcCmdData, const byte* pcSource, const int iMaxLen )
{
	byte cTemp[15] = {0};
	int i = 0, k = 0, m = 0;
	int iScale = 16;

	if( 0 == iMaxLen )
		return iMaxLen;

	while( i != iMaxLen + 1 )
	{
		if( 0 == k )
			iScale = 10;

		if( isspace( pcSource[i] ) ) //����ո񡢻��С��Ʊ����
		{
			i += 1;
			continue;
		}

		if( ',' == pcSource[i] )
		{
			cTemp[k] = '\0';
			pcCmdData[m] = ( byte )strtol( cTemp, NULL, iScale );
			m += 1;
			k = 0;
			i += 1;
			continue;
		}
		else if( i == iMaxLen )
		{
			cTemp[k] = '\0';
			pcCmdData[m] = ( byte )strtol( cTemp, NULL, iScale );
			m += 1;
			break;
		}

		cTemp[k] = pcSource[i];

		if( 1 == k ) //��xml��0��x�м��пո���ʵ�Ǵ���ģ�������������ݴ�
		{
			if( cTemp[k] == 'x' || cTemp[k] == 'X' )
				iScale = 16;
		}

		k += 1;

		i += 1;
	}

	return m;
}

/*************************************************
Description:	�������Ϊstring����������
Input:
	pcSource	����������ݵĵ�ַ���ַ�����
	iMaxLen		�������ݳ��ȣ��ַ�����

Output:	pu32Dest	����������ݵ�ַ
Return:	int		�������ݸ���
Others:	ֱ�Ӵ�xml��ȡ������������string���͵ģ�
		�Ҹ�ʽ����"00,10,0x00,01,01,25,07,11"
		�ѽ��������uint32�͵�ַ��
*************************************************/
int get_string_type_data_to_uint32( uint32 *pu32Dest, const byte* pcSource, const int iMaxLen )
{
	byte cTemp[15] = {0};
	int i = 0, k = 0, m = 0;
	int iScale = 16;

	if( 0 == iMaxLen )
		return iMaxLen;

	while( i != iMaxLen + 1 )
	{
		if( 0 == k )
			iScale = 10;

		if( isspace( pcSource[i] ) ) //����ո񡢻��С��Ʊ����
		{
			i += 1;
			continue;
		}

		if( ',' == pcSource[i] )
		{
			cTemp[k] = '\0';
			pu32Dest[m] = strtol( cTemp, NULL, iScale );
			m += 1;
			k = 0;
			i += 1;
			continue;
		}
		else if( i == iMaxLen )
		{
			cTemp[k] = '\0';
			pu32Dest[m] = strtol( cTemp, NULL, iScale );
			m += 1;
			break;
		}

		cTemp[k] = pcSource[i];

		if( 1 == k ) //��xml��0��x�м��пո���ʵ�Ǵ���ģ�������������ݴ�
		{
			if( cTemp[k] == 'x' || cTemp[k] == 'X' )
				iScale = 16;
		}

		k += 1;

		i += 1;
	}

	return m;

}


/*************************************************
Description:	����״̬��Ϣ����
Input:
	cTipMode		״̬��Ϣ���ͣ���ȡ����3��ֵ�Ļ�
						����ִ�еĽ��:
								COMM_INTERRUPT(0X02)
								PROCESS_OK(0X01)
								PROCESS_FAIL(0X00)
						�Ƿ�����:
								HAVE_JUMP(0X04)
								NO_JUMP(0X00)
						�Ƿ񵯳���ʾ��:
								HAVE_TIP_DATA(0X10)
								HAVE_TIP(0X08)
								NO_TIP(0X00)

	pcLable			Ҫ��ת��lable
	pcTipContent	��ʾ��Ϣ����
	pcData			���ݴ�ŵ�ַ
	cDataLen		���ݳ���
	iUploadDataType	�ϴ���������(HEX_PRINT,DEC_PRINT,ORIGINAL)
	pcControlsID	�ؼ�ID

Output:	pOut	������ݵ�ַ
Return:	int		�����ַ�д�����ݳ���
Others:�ú���Ŀǰ֧���ϴ�һ�����ݺ���֮��صĿؼ�ID
*************************************************/
int special_return_status( const byte cTipMode, const byte* pcLable, const byte* pcTipContent, const byte* pcData,
                           const byte cDataLen, const int iUploadDataType, const byte* pcControlsID, void* pOut )
{
	byte *pOutTemp = ( byte* )pOut;
	byte cProcessValue	= ( cTipMode >> 0 ) & 0x03;
	byte cJumpValue		= ( cTipMode >> 2 ) & 0x01;
	byte cTipValue		= ( cTipMode >> 3 ) & 0x03;

	byte cTipConentOffset	= 0;//��ʾ���ݱ���ƫ��
	byte cAppendDataOffset	= 0;//�������ݱ���ƫ��
	byte GetLogArr[] = {0xA0, 0x00, 0x05, 0x80, 0x25}; //��VDI����log��������

	int i = 0;
	int iDataLen = 0;

	if( cProcessValue == PROCESS_FAIL )
	{
		send_cmd( GetLogArr, 5 );
	}

	pOutTemp[0] = cProcessValue;
	pOutTemp[1] = cJumpValue;
	pOutTemp[2] = cTipValue;

	pOutTemp[3] = 0;
	pOutTemp[4] = 0;
	pOutTemp[5] = 0;
	pOutTemp[6] = 0;

	//װ��Lable
	pOutTemp[7] = 0;//��תLable���ȸ��ֽ�
	pOutTemp[8] = ( pcLable == NULL ) ? 0 : ( byte )strlen( pcLable ); //��תLable���ȵ��ֽ�

	memcpy( &pOutTemp[9], pcLable, pOutTemp[8] );

	//װ����ʾ��Ϣ
	cTipConentOffset = 9 + pOutTemp[8];

	pOutTemp[ cTipConentOffset ] = 0;//��ʾ��Ϣ���ȸ��ֽ�
	pOutTemp[ cTipConentOffset  + 1] = ( pcTipContent == NULL ) ? 0 : ( byte )strlen( pcTipContent ); //��ʾ��Ϣ���ȵ��ֽ�

	memcpy( &pOutTemp[ cTipConentOffset + 2 ], pcTipContent, pOutTemp[ cTipConentOffset + 1] );


	//װ�ظ�������
	cAppendDataOffset = cTipConentOffset + 2 + pOutTemp[cTipConentOffset + 1];

	iDataLen = add_data_and_controlsID( cAppendDataOffset, pcData, cDataLen,
	                                    iUploadDataType, pcControlsID, pOut );

	return iDataLen;

}
/*************************************************
Description:	׷�����ݺͿؼ�ID
Input:
	iAppendDataOffset	��Ÿ������ݵ�ƫ�ƣ�
							������ܳ�����ͬ
	pcData				���ݴ�ŵ�ַ
	cDataLen			���ݳ���
	iUploadDataType		�ϴ���������(HEX_PRINT,DEC_PRINT,ORIGINAL)
	pcControlsID		�ؼ�ID

Output:	pOut	������ݵ�ַ
Return:	int		�����ַ�д�����ݳ���
Others:��һ��ʹ�ñ�����ǰ���ȵ���special_return_status
���������������޸�������������ֽ�
*************************************************/
int add_data_and_controlsID( const int iAppendDataOffset, const byte* pcData, const byte cDataLen,
                             const int iUploadDataType, const byte* pcControlsID, void* pOut )
{
	byte *pOutTemp = ( byte* )pOut;
	int i = 0;

	int iControlsIDOffset	= 0;//�ؼ�ID����ƫ��
	byte cDecDataLen		= 0;//10�������ݳ���
	uint32 iTemp = 0;

	UNN_2WORD_4BYTE uuControlsIDLen;

	UNN_2WORD_4BYTE DataLen;

	DataLen.u32Bit = 0;
	uuControlsIDLen.u32Bit = 0;

	DataLen.u32Bit += iAppendDataOffset;//��ȡ֮ǰ����ܳ���

	switch( iUploadDataType )
	{
	case HEX_PRINT://��16���ƴ�ӡ���ַ���
	{
		pOutTemp[iAppendDataOffset] = 0;
		pOutTemp[iAppendDataOffset + 1] = cDataLen * 2;

		for( i = 0; i < cDataLen; i++ )
		{
			/*sprintf_s( &pOutTemp[iAppendDataOffset + 1 + i * 2], ( cDataLen * 2 + 1 - i * 2 ), "%02X", pcData[i] );*/
			sprintf( &pOutTemp[iAppendDataOffset + 2 + i * 2], "%02X", pcData[i] );
		}
	}
	break;

	case DEC_COMBIN://���ֽ����ʮ������ʾ������ĸ�
	{
		for( i = 0; i < cDataLen; ++i )
		{
			iTemp |= pcData[i];

			if( i != cDataLen - 1 )
			{
				iTemp <<= 8;
			}
		}

		pOutTemp[iAppendDataOffset] = 0;
		pOutTemp[iAppendDataOffset + 1] = sprintf( &pOutTemp[iAppendDataOffset + 2], "%d", iTemp );
	}
	break;

	case DEC_PRINT://��10���ƴ�ӡ���ַ���
	{
		cDecDataLen = 0;

		for( i = 0; i < cDataLen; i++ )
		{
			cDecDataLen += sprintf( &pOutTemp[iAppendDataOffset + 2 + cDecDataLen], "%d", pcData[i] );
		}

		pOutTemp[iAppendDataOffset] = 0;
		pOutTemp[iAppendDataOffset + 1] = cDecDataLen;
	}
	break;

	case ORIGINAL:
	default://���������ԭ������
	{
		pOutTemp[iAppendDataOffset] = 0;
		pOutTemp[iAppendDataOffset + 1] = cDataLen;

		memcpy( &pOutTemp[iAppendDataOffset + 2], pcData, cDataLen );
	}
	break;
	}

	DataLen.u32Bit += 2 + pOutTemp[iAppendDataOffset + 1];

	//װ��controlsID
	iControlsIDOffset = iAppendDataOffset + 2 + pOutTemp[iAppendDataOffset + 1];

	uuControlsIDLen.u32Bit = ( pcControlsID == NULL ) ? 0 : ( uint32 )strlen( pcControlsID );

	pOutTemp[iControlsIDOffset] = uuControlsIDLen.u8Bit[1];
	pOutTemp[iControlsIDOffset + 1] = uuControlsIDLen.u8Bit[0];

	memcpy( &pOutTemp[iControlsIDOffset + 2], pcControlsID, uuControlsIDLen.u32Bit );

	DataLen.u32Bit += 2 + uuControlsIDLen.u32Bit;

	//װ������
	pOutTemp[3] = DataLen.u8Bit[3];
	pOutTemp[4] = DataLen.u8Bit[2];
	pOutTemp[5] = DataLen.u8Bit[1];
	pOutTemp[6] = DataLen.u8Bit[0];

	return ( int )DataLen.u32Bit;
}
