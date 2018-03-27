/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	�������������ݴ�����
History:
	<author>	<time>		<desc>
************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ds_lib.h"
#include "../formula/formula_comply.h"
#include <assert.h>
#include "../command/command.h"
#include "../public/public.h"
#include "../public/protocol_config.h"
#include "../InitConfigFromXml/init_config_from_xml_lib.h"
#include "../formula_parse/interface.h"

//����������ID��������Žṹ�����飬�ֲ�����
STRUCT_CMD_ID_ASSOCIATE_BUFFER_OFFSET s_stDSCmdIDAssociateBufferOffsetGroup[20] =
{
	{0, 0,},
	{0, 0,},
	{0, 0,},
	{0, 0,},
	{0, 0,},
	{0, 0,},
	{0, 0,},
	{0, 0,},
	{0, 0,},
	{0, 0,},
	{0, 0,},
	{0, 0,},
	{0, 0,},
	{0, 0,},
	{0, 0,},
	{0, 0,},
	{0, 0,},
	{0, 0,},
	{0, 0,},
	{0, 0,},
};
//��ʱ�������������漰״̬�����飬�ֲ�����
STRUCT_DS_STORAGE_BUFFER s_stDSBufferTempGroup[20] =
{
	{false, 0, 0,},
	{false, 0, 0,},
	{false, 0, 0,},
	{false, 0, 0,},
	{false, 0, 0,},
	{false, 0, 0,},
	{false, 0, 0,},
	{false, 0, 0,},
	{false, 0, 0,},
	{false, 0, 0,},
	{false, 0, 0,},
	{false, 0, 0,},
	{false, 0, 0,},
	{false, 0, 0,},
	{false, 0, 0,},
	{false, 0, 0,},
	{false, 0, 0,},
	{false, 0, 0,},
	{false, 0, 0,},
	{false, 0, 0,},
};

STRUCT_SELECT_FUN stReadDataStreamFunGroup[] =
{
	{READ_GENERAL_DATA_STREAM, process_read_general_data_stream},
};

/*************************************************
Description:	��ȡ�����ȡ����������
Input:
	cType		��������
Output:	����
Return:	pf_general_function ����ָ��
Others:
*************************************************/
pf_general_function get_read_data_stream_fun( byte cType )
{
	int i = 0;

	for( i = 0; i < sizeof( stReadDataStreamFunGroup ) / sizeof( stReadDataStreamFunGroup[0] ); i++ )
		if( cType == stReadDataStreamFunGroup[i].cType )
			return stReadDataStreamFunGroup[i].pFun;

	return 0;
}
/*************************************************
Description:	��������
Input:	pIn		�������
Output:	pOut	������ݵ�ַ
Return:	����
Others:
*************************************************/
void process_read_data_stream( void* pIn, void* pOut )
{
	pf_general_function pFun = NULL;

	pFun = get_read_data_stream_fun( g_p_stProcessFunConfig->cDSFunOffset );

	assert( pFun );

	pFun( pIn, pOut );

}
/*************************************************
Description:	�����ȡ��ͨ��������
Input:	pIn		�������
Output:	pOut	������ݵ�ַ
Return:	����
Others:
*************************************************/
void process_read_general_data_stream( void* pIn, void* pOut )
{
	STRUCT_CHAIN_DATA_OUTPUT* pstDataStreamOut = ( STRUCT_CHAIN_DATA_OUTPUT* )malloc( sizeof( STRUCT_CHAIN_DATA_OUTPUT ) );

	int iReceiveResult = TIME_OUT;
	int * piCmdIndex = NULL;
	byte cStartOffset = 0;
	int i = 0;
	byte cItemSum = 0;
	int iDSCmdIndexTemp[2] = {1, 0};
	byte cBufferOffset = 0;
	byte CmdSum = 0;

	//��ȡ��ǰ��������������
	cItemSum = get_current_screen_DS_config( pIn );

	piCmdIndex = ( int * )malloc( sizeof( int ) * ( cItemSum + 1 ) );

	for( i = 0; i < cItemSum; i++ )
	{
		piCmdIndex[i + 1] = g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[i].iDSCmdID;
	}

	piCmdIndex[0] = remove_duplicate_int_element( piCmdIndex + 1, cItemSum );


	for( i = 0; i < piCmdIndex[0]; i++ )
	{
		s_stDSCmdIDAssociateBufferOffsetGroup[i].iCmdID = piCmdIndex[i + 1];
		s_stDSCmdIDAssociateBufferOffsetGroup[i].cBufferOffset = i;
	}

	if( piCmdIndex[0] > 20 ) /* ��ֹ��λ��������������ʹ����Խ�� */
	{
		CmdSum = 20;
	}
	else
	{
		CmdSum = piCmdIndex[0];
	}

	for( i = 0; i < CmdSum; i++ )
	{
		iDSCmdIndexTemp[1] = piCmdIndex[i + 1];

		iReceiveResult = send_and_receive_cmd( iDSCmdIndexTemp );

		if( ( iReceiveResult != SUCCESS ) && ( iReceiveResult != NEGATIVE ) )
		{
			free( pstDataStreamOut );

			general_return_status( iReceiveResult, NULL, 0, pOut );

			return;
		}

		cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[iDSCmdIndexTemp[1]].cBufferOffset;
		//�ѵ�ǰ�������Ļ������ݱ��浽��ʱ�Ļ�����
		s_stDSBufferTempGroup[i].iValidLen = g_stBufferGroup[cBufferOffset].iValidLen;

		memcpy( s_stDSBufferTempGroup[i].cBuffer, g_stBufferGroup[cBufferOffset].cBuffer,
		        g_stBufferGroup[cBufferOffset].iValidLen );

		if( iReceiveResult == NEGATIVE )
		{
			s_stDSBufferTempGroup[i].bNegativeFlag = true;
		}
		else
		{
			s_stDSBufferTempGroup[i].bNegativeFlag = false;
		}

	}

	process_general_data_stream_data( ( STRUCT_DS_FORMULA_GROUP_CONFIG* )&g_stGeneralDSFormulaGroupConfig, cItemSum,
	                                  pstDataStreamOut );
	copy_data_to_out( pOut, pstDataStreamOut );
	free_param_out_data_space( pstDataStreamOut );

	free( pstDataStreamOut );

}



/*************************************************
Description:	����̬����������
Input:
	iDsId		��������ID
	pcDsSource	ȡֵ��ַ

Output:	pstDataStreamOut	�������ָ��
Return:	void
Others:
*************************************************/

void process_general_data_stream_data( STRUCT_DS_FORMULA_GROUP_CONFIG * pstDSGroupConfig, byte cItemSum,
                                       STRUCT_CHAIN_DATA_OUTPUT* pstDataStreamOut )

{
	int i = 0;
	int j = 0;
	byte cContentCache[128] = {0};

	STRUCT_CHAIN_DATA_NODE *phead  = NULL;
	STRUCT_CHAIN_DATA_NODE *ptemp1 = NULL;
	STRUCT_CHAIN_DATA_NODE *ptemp2 = NULL;
	STRUCT_CHAIN_DATA_NODE *ptemp3 = NULL;

	int	 iDisplayID = 0;
	byte cStartOffset = 0;
	byte cValidByteLen = 0;
	int  iDSCmdID = 0;
	byte cBufferOffset = 0;//����ƫ��

	byte cNegtiveDisplay[20] = {"Neg.$"};

	pstDataStreamOut->cKeyByte[0] = 1;
	pstDataStreamOut->cKeyByte[1] = 0;
	pstDataStreamOut->cKeyByte[2] = 0;
	pstDataStreamOut->stJump.cLenHighByte = 0;
	pstDataStreamOut->stJump.cLenLowByte = 0;
	pstDataStreamOut->stTip.cLenHighByte = 0;
	pstDataStreamOut->stTip.cLenLowByte = 0;

	for( i = 0; i < cItemSum; i++ )
	{
		iDisplayID			= pstDSGroupConfig->pstDSFormulaConfig[i].iDSID;
		cStartOffset		= pstDSGroupConfig->pstDSFormulaConfig[i].cValidByteOffset;
		cValidByteLen		= pstDSGroupConfig->pstDSFormulaConfig[i].cValidByteNumber;
		iDSCmdID			= pstDSGroupConfig->pstDSFormulaConfig[i].iDSCmdID;
		//�õ�����ƫ��
		cBufferOffset		= get_DS_buffer_offset( iDSCmdID );

		assert( cBufferOffset != 255 );

		ptemp2 = ( STRUCT_CHAIN_DATA_NODE * )malloc( sizeof( STRUCT_CHAIN_DATA_NODE ) );
		//һ���ֽڶ�Ӧ���ַ�����Ϊ2
		ptemp2->cLenHighByte = 0;
		ptemp2->cLenLowByte = 4;
		ptemp2->pcData = ( byte * )malloc( ( 4 + 1 ) * sizeof( byte ) );

		sprintf( ptemp2->pcData, "%04d", iDisplayID ); //���ʹ�ӡ������ID

		ptemp3 = ( STRUCT_CHAIN_DATA_NODE * )malloc( sizeof( STRUCT_CHAIN_DATA_NODE ) );

		memset( cContentCache, 0, sizeof( cContentCache ) );

		if( !s_stDSBufferTempGroup[cBufferOffset].bNegativeFlag )
		{
			if( PROTOCOL_PARSER == pstDSGroupConfig->pstDSFormulaConfig[i].cFormulaType )
			{
				//����protocol�ļ��㹫ʽ
				process_normal_ds_calculate(
				    pstDSGroupConfig->pstDSFormulaConfig[i].iDSID,
				    s_stDSBufferTempGroup[cBufferOffset].cBuffer + cStartOffset,
				    cContentCache
				);
			}
			else
			{
				//���ù�ʽ������
				calculate(
				    s_stDSBufferTempGroup[cBufferOffset].cBuffer + cStartOffset,
				    pstDSGroupConfig->pstDSFormulaConfig[i].cValidByteNumber,
				    pstDSGroupConfig->pstDSFormulaConfig[i].pcFormula,
				    pstDSGroupConfig->pstDSFormulaConfig[i].iFormulaLen,
				    pstDSGroupConfig->pstDSFormulaConfig[i].pStrFormat,
				    cContentCache
				);
			}

			ptemp3->cLenHighByte = 0;
			ptemp3->cLenLowByte = ( byte )strlen( cContentCache );
			ptemp3->pcData = ( byte * )malloc( strlen( cContentCache ) * sizeof( byte ) );
			memcpy( ptemp3->pcData, cContentCache, strlen( cContentCache ) );
		}
		else
		{
			for( j = 0; j < 3; j++ )
			{
				sprintf( &( cNegtiveDisplay[j * 2 + 5] ), "%02X", s_stDSBufferTempGroup[cBufferOffset].cBuffer[j] );
			}

			cNegtiveDisplay[11] = '\0';

			ptemp3->cLenHighByte = 0;
			ptemp3->cLenLowByte = ( byte )strlen( cNegtiveDisplay );
			ptemp3->pcData = ( byte * )malloc( strlen( cNegtiveDisplay ) * sizeof( byte ) );
			memcpy( ptemp3->pcData, cNegtiveDisplay, strlen( cNegtiveDisplay ) );
		}

		ptemp2->pNextNode = ptemp3;

		if( i == 0 )
		{
			phead = ptemp2;
			ptemp1 = ptemp3;
		}
		else
		{
			ptemp1->pNextNode = ptemp2;
			ptemp1 = ptemp3;
		}

	}

	ptemp1->pNextNode = NULL;

	pstDataStreamOut->pstData = phead;
}


/*************************************************
Description:	��ȡ��������������
Input:
	pstDSConfig		ָ�����������õ�ָ��
	pIn				��������

Output:	��
Return:	int ��ǰ������ռ�ռ��С
Others: ע��:�ڵ��ñ�����ʱ��ʹ��calloc��������
		STRUCT_DS_FORMULA_CONFIG���ͱ������ڴ棬
		��ʹ����������ʱ������ʼ����
*************************************************/
int get_ds_config( STRUCT_DS_FORMULA_CONFIG * pstDSConfig, const byte * pcSource )
{

	byte cTemp[15] = {0};
	const byte * pcTemp = NULL;
	const byte * pcHead = NULL;
	byte * pcFormulaType = NULL;
	int iCmdSum = 0;
	int iLen = 0;
	uint32 u32Temp = 0;

	if( NULL != pstDSConfig->pcFormula )
		free( pstDSConfig->pcFormula );

	if( NULL != pstDSConfig->pStrFormat )
		free( pstDSConfig->pStrFormat );

	pcTemp = pcSource;
	pcHead = pcTemp;

	memcpy( cTemp, pcTemp, 4 );
	cTemp[4] = '\0';

	iLen = 4;

	iLen += strtol( cTemp, NULL, 16 );

	get_config_data( &u32Temp, pcTemp, iLen );

	pstDSConfig->iDSID = ( int )u32Temp; //���DS��ID

	pcTemp += iLen;

	memcpy( cTemp, pcTemp, 4 );
	cTemp[4] = '\0';

	iLen = 4;

	iLen += strtol( cTemp, NULL, 16 );

	get_config_data( &u32Temp, pcTemp, iLen );

	pstDSConfig->iDSCmdID = ( int )u32Temp; //�������ƫ��

	pcTemp += iLen;

	iLen = get_command_one_block_config_data( cTemp, pcTemp );

	pstDSConfig->cValidByteOffset = cTemp[0]; //�����Ч�ֽ���ʼƫ��

	pcTemp += iLen;

	iLen = get_command_one_block_config_data( cTemp, pcTemp );

	pstDSConfig->cValidByteNumber = cTemp[0]; //�����Ч�ֽڸ���

	pcTemp += iLen;

	memcpy( cTemp, pcTemp, 4 );
	cTemp[4] = '\0';

	iLen = 4;

	iLen += strtol( cTemp, NULL, 16 );

	pcFormulaType = ( byte * )malloc( sizeof( byte ) * ( iLen - 4 + 1 ) );

	memcpy( pcFormulaType, pcTemp + 4, iLen - 4 ); //��ù�ʽ����

	pcFormulaType[iLen - 4] = '\0';

	pstDSConfig->cFormulaType = FORMULA_PARSER;//Ĭ��Ϊ����������

	if( 0 == strcmp( pcFormulaType, "FORMULA_PARSER" ) )
		pstDSConfig->cFormulaType = FORMULA_PARSER;

	if( 0 == strcmp( pcFormulaType, "PROTOCOL_PARSER" ) )
		pstDSConfig->cFormulaType = PROTOCOL_PARSER;

	free( pcFormulaType );
	pcTemp += iLen;

	memcpy( cTemp, pcTemp, 4 );
	cTemp[4] = '\0';

	iLen = 4;

	iLen += strtol( cTemp, NULL, 16 );

	pstDSConfig->pcFormula = ( byte * )malloc( sizeof( byte ) * ( iLen - 4 + 1 ) );

	memcpy( pstDSConfig->pcFormula, pcTemp + 4, iLen - 4 ); //��ù�ʽ����
	pstDSConfig->iFormulaLen = iLen - 4;//��ù�ʽ���ݵĳ���

	pstDSConfig->pcFormula[iLen - 4] = '\0';

	pcTemp += iLen;

	memcpy( cTemp, pcTemp, 4 );
	cTemp[4] = '\0';

	iLen = 4;

	iLen += strtol( cTemp, NULL, 16 );

	pstDSConfig->pStrFormat = ( byte * )malloc( sizeof( byte ) * ( iLen - 4 + 1 ) );

	memcpy( pstDSConfig->pStrFormat, pcTemp + 4, iLen - 4 ); //�����ʾ����

	pstDSConfig->pStrFormat[iLen - 4] = '\0';

	pcTemp += iLen;

	return ( int )( pcTemp - pcHead );

}

/*************************************************
Description:	��ȡ��ǰ����������������
Input:
	pIn				��������

Output:	��
Return:	byte		��ǰ������������
Others: ��������Ϊ����������+�������������á�
ע������g_stGeneralDSFormulaGroupConfig.cItemSum
����������ֵ���������ÿ�ε�������
*************************************************/
byte get_current_screen_DS_config( void* pIn )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	byte cTemp	= 0;
	byte * pcTemp = NULL;

	byte j = 0;

	int iLen = 0;

	byte cItemSum = 0;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	pcTemp = pstParam->pcData;

	iLen = get_command_one_block_config_data( &cTemp, pcTemp );
	cItemSum = cTemp;//��õ�ǰ������������
	pcTemp += iLen;

	if( g_stGeneralDSFormulaGroupConfig.cItemSum < cItemSum ) //�����ǰ����������С����Ҫ�������·���
	{
		free_general_DS_formula_config_space();//�ͷ��ѷ����

		g_stGeneralDSFormulaGroupConfig.cItemSum = cItemSum;

		g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig =
		    ( STRUCT_DS_FORMULA_CONFIG * )calloc( cItemSum, sizeof( STRUCT_DS_FORMULA_CONFIG ) );
	}

	for( j = 0; j < cItemSum; j++ )
	{
		iLen = get_ds_config( ( STRUCT_DS_FORMULA_CONFIG * ) & ( g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[j] ), pcTemp );
		pcTemp += iLen;
	}

	return cItemSum;
}

/*************************************************
Description:	�ͷŴ����ͨ��������ʽ���õĿռ�
Input:	��
Output:	����
Return:	��
Others: ÿ���һ�����þ��ڸú����������Ӧ���ͷŴ���
		����quit_system_lib.c��free_xml_config_space
		�����е��øú�����
*************************************************/
void free_general_DS_formula_config_space( void )
{
	byte j = 0;

	if( NULL != g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig )
	{
		for( j = 0; j < g_stGeneralDSFormulaGroupConfig.cItemSum; j++ )
		{
			if( NULL != g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[j].pcFormula )
				free( g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[j].pcFormula );

			if( NULL != g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[j].pStrFormat )
				free( g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[j].pStrFormat );
		}

		free( g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig );
	}

}
/*************************************************
Description:	��ȡ����������ƫ��
Input:	iDSCmdID ����������ID
Output:	����
Return:	byte	����ƫ��
Others:
*************************************************/
byte get_DS_buffer_offset( int iDSCmdID )
{
	int i = 0;

	for( i = 0; i < sizeof( s_stDSCmdIDAssociateBufferOffsetGroup ) / sizeof( s_stDSCmdIDAssociateBufferOffsetGroup[0] ); i++ )
		if( iDSCmdID == s_stDSCmdIDAssociateBufferOffsetGroup[i].iCmdID )
			return s_stDSCmdIDAssociateBufferOffsetGroup[i].cBufferOffset;

	return 255;
}