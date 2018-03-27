/************************************************************************
* Copyright (c) 2014,����һ����Ϣ�Ƽ����޹�˾
*
* �ļ����ƣ�clear_dtc_lib.h
* �ļ���ʶ��
* ժ    Ҫ��
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�
*
* ȡ���汾��
* ԭ����  ��
* ������ڣ�
**************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include "clear_dtc_lib.h"
#include "..\public\public.h"
#include "..\command\command.h"
#include "..\interface\protocol_define.h"
#include "..\public\protocol_config.h"
#include <assert.h>
#include "..\InitConfigFromXml\init_config_from_xml_lib.h"
#include "..\SpecialFunction\special_function.h"

STRUCT_SELECT_FUN stClearDTCFunGroup[] =
{
	{GENERAL_CLEAR_DTC, process_general_clear_Dtc},
};

/*************************************************
Description:	��ȡ�������뺯��
Input      :	cType		��������
Output     :	����
Return     :	pf_general_function ����ָ��
Time       :
*************************************************/
pf_general_function get_clear_DTC_fun( byte cType )
{
	int i = 0;

	for( i = 0; i < sizeof( stClearDTCFunGroup ) / sizeof( stClearDTCFunGroup[0] ); i++ )
		if( cType == stClearDTCFunGroup[i].cType )
			return stClearDTCFunGroup[i].pFun;

	return 0;
}

/*************************************************
Description:	���������
Input:	pIn		���������������
Output:	pOut	������ݵ�ַ
Return:	����
Others:
*************************************************/
void process_clear_Dtc( void* pIn, void* pOut )
{
	pf_general_function pFun = NULL;

	pFun = get_clear_DTC_fun( g_p_stProcessFunConfig->cClearDTCFunOffset );

	assert( pFun );

	pFun( pIn, pOut );
}
/*************************************************
Description:	��ͨ��������뺯��
Input:	pIn		���������������
Output:	pOut	������ݵ�ַ
Return:	����
Others:
*************************************************/
void process_general_clear_Dtc( void* pIn, void* pOut )
{
	STRUCT_CHAIN_DATA_OUTPUT* pstClearDtcOut = ( STRUCT_CHAIN_DATA_OUTPUT* )malloc( sizeof( STRUCT_CHAIN_DATA_OUTPUT ) );

	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	byte *pcOutTemp = ( byte * )pOut;

	int iReceiveResult = TIME_OUT;
	int iReceiveValidLen = 0;//���յ�����Ч�ֽڳ���

	int * piCmdIndex = NULL;
	uint32 u32CmdData[50] = {0};//��������
	int iCmdSum = 0;
	int i = 0;

	byte cBufferOffset  = 0;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	iCmdSum = get_string_type_data_to_uint32( u32CmdData, pstParam->pcData, pstParam->iLen );

	if( 0 == iCmdSum )
	{
		general_return_status( SUCCESS, NULL, 0, pcOutTemp );
		pcOutTemp[2] = 0;//����ʾ

		free( pstClearDtcOut );

		return ;
	}

	piCmdIndex = ( int * )malloc( sizeof( int ) * ( iCmdSum + 1 ) );

	piCmdIndex[0] = iCmdSum;

	for( i = 0; i < iCmdSum; i++ )
	{
		piCmdIndex[i + 1] = ( int )u32CmdData[i];
	}

	cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[ piCmdIndex[1] ].cBufferOffset;

	iReceiveResult = send_and_receive_cmd( piCmdIndex );

	iReceiveValidLen = g_stBufferGroup[cBufferOffset].iValidLen;

	switch( iReceiveResult )
	{
	case SUCCESS:
	{
		process_general_response( g_stBufferGroup[cBufferOffset].cBuffer, iReceiveValidLen, pstClearDtcOut );
		copy_data_to_out( pOut, pstClearDtcOut );
		free_param_out_data_space( pstClearDtcOut );
	}
	break;

	case NEGATIVE:
	case FRAME_TIME_OUT:
	case TIME_OUT:
	default:
		general_return_status( iReceiveResult, g_stBufferGroup[cBufferOffset].cBuffer, 3, pOut );
		break;

	}

	/*free(piCmdIndex);*/
	free( pstClearDtcOut );

}