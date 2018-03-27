/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	���嶳��֡����������������ݴ�����
History:
	<author>	<time>		<desc>

************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freeze_lib.h"
#include "../public/protocol_config.h"
#include "../command/command.h"
#include "../formula/formula_comply.h"
#include "../public/public.h"
#include "../interface/protocol_define.h"
#include <assert.h>
#include "../InitConfigFromXml/init_config_from_xml_lib.h"
#include "../formula_parse/interface.h"
#include "ds_lib.h"
#include "..\SpecialFunction\special_function.h"

int s_iUDSReadFreezeDTCCmdIndex = 0;//UDS��ȡ����֡�����������������ֲ����������ļ���Ч��Ϊʹ��xml�����

STRUCT_SELECT_FUN stReadFreezeDTCFunGroup[] =
{
	{READ_FREEZE_DTC_BY_ISO14229, process_read_freeze_frame_DTC_by_ISO14229},
};

/*************************************************
Description:	��ȡ������֡���뺯��
Input:
	cType		��������
Output:	����
Return:	pf_general_function ����ָ��
Others:
*************************************************/
pf_general_function get_read_freeze_DTC_fun( byte cType )
{
	int i = 0;

	for( i = 0; i < sizeof( stReadFreezeDTCFunGroup ) / sizeof( stReadFreezeDTCFunGroup[0] ); i++ )
		if( cType == stReadFreezeDTCFunGroup[i].cType )
			return stReadFreezeDTCFunGroup[i].pFun;

	return 0;
}

/*************************************************
Description:	����֡����
Input:	pIn		���������������
Output:	pOut	������ݵ�ַ
Return:	����
Others:
*************************************************/
void process_read_freeze_frame_DTC( void* pIn, void* pOut )
{
	pf_general_function pFun = NULL;

	pFun = get_read_freeze_DTC_fun( g_p_stProcessFunConfig->cFreezeDTCFunOffset );

	assert( pFun );

	pFun( pIn, pOut );
}

STRUCT_SELECT_FUN stReadFreezeDSFunGroup[] =
{
	{READ_FREEZE_DS_BY_ISO14229,					process_read_freeze_frame_DS_by_ISO14229},
	{READ_ONE_MEMORY_REQUEST_ALL_DATA_BY_ISO14230,	process_read_one_memory_request_all_data_DS_by_ISO14230},
	{READ_FREEZE_ACCORD_XML,						process_read_freeze_frame_DS_by_xml},
};
/*************************************************
Description:	��ȡ������֡����������
Input:
	cType		��������
Output:	����
Return:	pf_general_function ����ָ��
Others:
*************************************************/
pf_general_function get_read_freeze_DS_fun( byte cType )
{
	int i = 0;

	for( i = 0; i < sizeof( stReadFreezeDSFunGroup ) / sizeof( stReadFreezeDSFunGroup[0] ); i++ )
		if( cType == stReadFreezeDSFunGroup[i].cType )
			return stReadFreezeDSFunGroup[i].pFun;

	return 0;
}

/*************************************************
Description:	����֡��������
Input:	pIn		���������������
Output:	pOut	������ݵ�ַ
Return:	����
Others:
*************************************************/
void process_read_freeze_frame_DS( void* pIn, void* pOut )
{
	pf_general_function pFun = NULL;

	pFun = get_read_freeze_DS_fun( g_p_stProcessFunConfig->cFreezeDSFunOffset );

	assert( pFun );

	pFun( pIn, pOut );
}
/*************************************************
Description:	������֡������
Input:	pIn		���������������
Output:	pOut	������ݵ�ַ
Return:	����
Others:
*************************************************/
void process_read_freeze_frame_DTC_by_ISO14229( void* pIn, void* pOut )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	STRUCT_CHAIN_DATA_OUTPUT* pstDtcOut = ( STRUCT_CHAIN_DATA_OUTPUT* )malloc( sizeof( STRUCT_CHAIN_DATA_OUTPUT ) );
	int iReceiveResult = TIME_OUT;
	int iReceiveValidLen = 0;//���յ�����Ч�ֽڳ���
	int iProcessStatus = 0;//����״̬

	byte cBufferOffset = 0;
	byte cConfigType = 0;
	int iConfigOffset = 0;
	int iReadFreezeFrameDtcCmdIndex[3] = {1, 0, 1};
	int iCmdSum = 0;
	uint32 u32CmdTemp[6] = {0};
	int i = 0;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	iConfigOffset = atoi( pstParam->pcData ); //��ö�����֡����������ID

	pstParam = pstParam->pNextNode;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	cConfigType = ( byte )atoi( pstParam->pcData ); //��ö�����֡����������ģ���

	select_freeze_dtc_config( iConfigOffset, cConfigType );

	pstParam = pstParam->pNextNode;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	//��ȡ����������������������
	iCmdSum = get_string_type_data_to_uint32( u32CmdTemp, pstParam->pcData, pstParam->iLen );

	iReadFreezeFrameDtcCmdIndex[0] = iCmdSum;

	for( i = 0; i < iCmdSum; i++ )  //��ö�����֡��������������
	{
		iReadFreezeFrameDtcCmdIndex[i + 1] = ( int )u32CmdTemp[i];
	}

	s_iUDSReadFreezeDTCCmdIndex = iReadFreezeFrameDtcCmdIndex[iCmdSum];

	iReceiveResult = send_and_receive_cmd( iReadFreezeFrameDtcCmdIndex );

	cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[ iReadFreezeFrameDtcCmdIndex[iCmdSum] ].cBufferOffset;

	iReceiveValidLen = g_stBufferGroup[cBufferOffset].iValidLen;

	switch( iReceiveResult )
	{
	case SUCCESS:
	{
		iProcessStatus = process_freeze_DTC_data_by_ISO14229( g_stBufferGroup[cBufferOffset].cBuffer,
		                 iReceiveValidLen, pstDtcOut );

		if( NO_FREEZE_DTC == iProcessStatus )
		{
			general_return_status( NO_FREEZE_DTC, NULL, 0, pOut );
		}
		else
		{
			copy_data_to_out( pOut, pstDtcOut );
			free_param_out_data_space( pstDtcOut );
		}

	}
	break;

	case NEGATIVE:
	case FRAME_TIME_OUT:
	case TIME_OUT:
	default:
		general_return_status( iReceiveResult, g_stBufferGroup[cBufferOffset].cBuffer, 3, pOut );
		break;
	}

	free( pstDtcOut );

}

/*************************************************
Description:	������֡������
Input:	pIn		�б���DTC��ID
Output:	pOut	������ݵ�ַ
Return:	����
Others:	����ISO14229Э��
*************************************************/
void process_read_freeze_frame_DS_by_ISO14229( void* pIn, void* pOut )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	int iReceiveResult = TIME_OUT;
	int iReceiveValidLen = 0;//���յ�����Ч�ֽڳ���
	int iDtcID = 0;

	byte cReadFreezeDsMode			= 0;
	byte cFreezeDtcStartOffset		= 0;
	byte cFreezeRecordNumberOffset	= 0;
	byte cDtcBytesInCmd				= 0;
	byte cSaveRecordNumberOffset	= 0;
	byte cRecordNumberBytes			= 0;
	byte cSaveDtcOffset				= 0;
	byte cDtcBytes					= 0;

	byte cBufferOffset = 0;
	int iProcessStatus;//����֡����������״̬
	byte cErrorDID[] = {0, 0};
	byte cConfigType = 0;
	int iConfigOffset = 0;
	int iReadFreezeFrameDSCmdIndex[2] = {1, 0};

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	iDtcID = atoi( pstParam->pcData ); //����б���DTC��ID

	pstParam = pstParam->pNextNode;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	iConfigOffset = atoi( pstParam->pcData ); //��ö�����֡����������ID

	pstParam = pstParam->pNextNode;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	cConfigType = ( byte )atoi( pstParam->pcData ); //��ö�����֡����������ģ���

	select_freeze_ds_config( iConfigOffset, cConfigType );

	cReadFreezeDsMode			= g_p_stUDSFreezeDtcConfig->cReadFreezeDsMode;//��ȡ����֡������ģʽ0�����ݹ����� 1�����ݴ洢��
	cFreezeDtcStartOffset		= g_p_stUDSFreezeDtcConfig->cFreezeDtcStartOffset;//�ظ���DTC��ʼƫ�ƣ���SID�ظ���ʼ
	cFreezeRecordNumberOffset	= g_p_stUDSFreezeDtcConfig->cFreezeRecordNumberOffset;
	cDtcBytesInCmd				= g_p_stUDSFreezeDtcConfig->cDtcBytesInCmd;//�����м����ֽڱ�ʾһ��������
	cSaveRecordNumberOffset		= g_p_stUDSFreezeDtcConfig->cSaveRecordNumberOffset;
	cRecordNumberBytes			= g_p_stUDSFreezeDtcConfig->cRecordNumberBytes;
	cSaveDtcOffset				= g_p_stUDSFreezeDtcConfig->cSaveDtcOffset;
	cDtcBytes					= g_p_stUDSFreezeDtcConfig->cDtcBytes;

	pstParam = pstParam->pNextNode;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	iReadFreezeFrameDSCmdIndex[1] = atoi( pstParam->pcData ); //��ö�����֡��������������

	//��ô洢����֡�������洢�ŵĻ���ƫ��
	cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[ s_iUDSReadFreezeDTCCmdIndex ].cBufferOffset;

	if( cReadFreezeDsMode == 0 ) //�ж϶�ȡ����֡��������ʽ ���ݹ�����Ż�洢��
	{
		memcpy( ( g_stInitXmlGobalVariable.m_p_stCmdList[ iReadFreezeFrameDSCmdIndex[1] ].pcCmd + cSaveDtcOffset ),
		        ( g_stBufferGroup[cBufferOffset].cBuffer + cFreezeDtcStartOffset + iDtcID * cDtcBytesInCmd ), cDtcBytes );
	}
	else if( cReadFreezeDsMode == 1 )
	{
		memcpy( ( g_stInitXmlGobalVariable.m_p_stCmdList[ iReadFreezeFrameDSCmdIndex[1] ].pcCmd + cSaveRecordNumberOffset ),
		        ( g_stBufferGroup[cBufferOffset].cBuffer + cFreezeRecordNumberOffset + iDtcID * cDtcBytesInCmd ), cRecordNumberBytes );
	}

	//��ô洢����֡�������Ļ���ƫ��
	cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[ iReadFreezeFrameDSCmdIndex[1] ].cBufferOffset;

	iReceiveResult = send_and_receive_cmd( iReadFreezeFrameDSCmdIndex );

	iReceiveValidLen = g_stBufferGroup[cBufferOffset].iValidLen;

	switch( iReceiveResult )
	{
	case SUCCESS:
	{
		iProcessStatus = process_freeze_data_stream_by_ISO14229( g_stBufferGroup[cBufferOffset].cBuffer,
		                 iReceiveValidLen, pOut );

		if( iProcessStatus == MATCH_ERROR )
		{
			cErrorDID[0]	= *( byte* )pOut;
			cErrorDID[1]	= *( ( byte* )pOut + 1 );
			general_return_status( MATCH_ERROR, cErrorDID, 2, pOut );
		}
		else if( iProcessStatus == NO_FREEZE_DS )
		{
			general_return_status( NO_FREEZE_DS, NULL, 0, pOut );
		}

	}
	break;

	case NEGATIVE:
	case FRAME_TIME_OUT:
	case TIME_OUT:
	default:
		general_return_status( iReceiveResult, g_stBufferGroup[cBufferOffset].cBuffer, 3, pOut );
		break;
	}
}

/*************************************************
Description:	������֡������
Input:	pIn
Output:	pOut	������ݵ�ַ
Return:	����
Others: ����ISO14230Э�飬�ô洢�Ŷ�ȡһ��洢��
���������ݡ�
*************************************************/
void process_read_one_memory_request_all_data_DS_by_ISO14230( void* pIn, void* pOut )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	int iReceiveResult = TIME_OUT;
	int iReceiveValidLen = 0;//���յ�����Ч�ֽڳ���
	int iDtcID = 0;

	byte cBufferOffset = 0;
	int iProcessStatus;//����֡����������״̬
	byte cErrorDID[] = {0, 0};
	byte cConfigType = 0;
	int iConfigOffset = 0;
	int iReadFreezeFrameDSCmdIndex[2] = {1, 0};

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	iConfigOffset = atoi( pstParam->pcData ); //��ö�����֡����������ID

	pstParam = pstParam->pNextNode;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	cConfigType = ( byte )atoi( pstParam->pcData ); //��ö�����֡����������ģ���

	select_freeze_ds_config( iConfigOffset, cConfigType );

	pstParam = pstParam->pNextNode;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	iReadFreezeFrameDSCmdIndex[1] = atoi( pstParam->pcData ); //��ö�����֡��������������

	//��ô洢����֡�������Ļ���ƫ��
	cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[ iReadFreezeFrameDSCmdIndex[1] ].cBufferOffset;

	iReceiveResult = send_and_receive_cmd( iReadFreezeFrameDSCmdIndex );

	iReceiveValidLen = g_stBufferGroup[cBufferOffset].iValidLen;

	switch( iReceiveResult )
	{
	case SUCCESS:
	case NEGATIVE://�п����ڲ�֧�ֶ���֡������ʱECU�ظ�������Ӧ
	{
		iProcessStatus =
		    process_one_memory_request_all_data_freeze_data_stream_by_ISO14230( g_stBufferGroup[cBufferOffset].cBuffer,
		            iReceiveValidLen, pOut );

		if( iProcessStatus == MATCH_ERROR )
		{
			cErrorDID[0]	= *( byte* )pOut;
			cErrorDID[1]	= *( ( byte* )pOut + 1 );
			general_return_status( MATCH_ERROR, cErrorDID, 2, pOut );
		}
		else if( iProcessStatus == NO_FREEZE_DS )
		{
			general_return_status( NO_FREEZE_DS, NULL, 0, pOut );
		}

	}
	break;

	case FRAME_TIME_OUT:
	case TIME_OUT:
	default:
		general_return_status( iReceiveResult, g_stBufferGroup[cBufferOffset].cBuffer, 3, pOut );
		break;
	}
}

/*************************************************
Description:	������֡���������������ǹ̶��ģ���ͨ��ID���ң�ֱ����ʾXML�е������
Input:	pIn		�б���DTC��ID
Output:	pOut	������ݵ�ַ
Return:	����
Others:	����ISO14229Э��
*************************************************/
void process_read_freeze_frame_DS_by_xml( void* pIn, void* pOut )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	int iReceiveResult = TIME_OUT;
	int iReceiveValidLen = 0;//���յ�����Ч�ֽڳ���
	int iDtcID = 0;

	byte cReadFreezeDsMode			= 0;
	byte cFreezeDtcStartOffset		= 0;
	byte cFreezeRecordNumberOffset	= 0;
	byte cDtcBytesInCmd				= 0;
	byte cSaveRecordNumberOffset	= 0;
	byte cRecordNumberBytes			= 0;
	byte cSaveDtcOffset				= 0;
	byte cDtcBytes					= 0;

	byte cBufferOffset = 0;
	int iProcessStatus;//����֡����������״̬
	byte cErrorDID[] = {0, 0};
	byte cConfigType = 0;
	int iConfigOffset = 0;
	int iReadFreezeFrameDSCmdIndex[2] = {1, 0};

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	iDtcID = atoi( pstParam->pcData ); //����б���DTC��ID

	pstParam = pstParam->pNextNode;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	iConfigOffset = atoi( pstParam->pcData ); //��ö�����֡����������ID

	pstParam = pstParam->pNextNode;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	cConfigType = ( byte )atoi( pstParam->pcData ); //��ö�����֡����������ģ���

	select_freeze_ds_config( iConfigOffset, cConfigType );

	cReadFreezeDsMode			= g_p_stUDSFreezeDtcConfig->cReadFreezeDsMode;//��ȡ����֡������ģʽ0�����ݹ����� 1�����ݴ洢��
	cFreezeDtcStartOffset		= g_p_stUDSFreezeDtcConfig->cFreezeDtcStartOffset;//�ظ���DTC��ʼƫ�ƣ���SID�ظ���ʼ
	cFreezeRecordNumberOffset	= g_p_stUDSFreezeDtcConfig->cFreezeRecordNumberOffset;
	cDtcBytesInCmd				= g_p_stUDSFreezeDtcConfig->cDtcBytesInCmd;//�����м����ֽڱ�ʾһ��������
	cSaveRecordNumberOffset		= g_p_stUDSFreezeDtcConfig->cSaveRecordNumberOffset;
	cRecordNumberBytes			= g_p_stUDSFreezeDtcConfig->cRecordNumberBytes;
	cSaveDtcOffset				= g_p_stUDSFreezeDtcConfig->cSaveDtcOffset;
	cDtcBytes					= g_p_stUDSFreezeDtcConfig->cDtcBytes;

	pstParam = pstParam->pNextNode;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	iReadFreezeFrameDSCmdIndex[1] = atoi( pstParam->pcData ); //��ö�����֡��������������

	//��ô洢����֡�������洢�ŵĻ���ƫ��
	cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[ s_iUDSReadFreezeDTCCmdIndex ].cBufferOffset;

	if( cReadFreezeDsMode == 0 ) //�ж϶�ȡ����֡��������ʽ ���ݹ�����Ż�洢��
	{
		memcpy( ( g_stInitXmlGobalVariable.m_p_stCmdList[ iReadFreezeFrameDSCmdIndex[1] ].pcCmd + cSaveDtcOffset ),
		        ( g_stBufferGroup[cBufferOffset].cBuffer + cFreezeDtcStartOffset + iDtcID * cDtcBytesInCmd ), cDtcBytes );
	}
	else if( cReadFreezeDsMode == 1 )
	{
		memcpy( ( g_stInitXmlGobalVariable.m_p_stCmdList[ iReadFreezeFrameDSCmdIndex[1] ].pcCmd + cSaveRecordNumberOffset ),
		        ( g_stBufferGroup[cBufferOffset].cBuffer + cFreezeRecordNumberOffset + iDtcID * cDtcBytesInCmd ), cRecordNumberBytes );
	}

	//��ô洢����֡�������Ļ���ƫ��
	cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[ iReadFreezeFrameDSCmdIndex[1] ].cBufferOffset;

	iReceiveResult = send_and_receive_cmd( iReadFreezeFrameDSCmdIndex );

	iReceiveValidLen = g_stBufferGroup[cBufferOffset].iValidLen;

	switch( iReceiveResult )
	{
	case SUCCESS:
	{
		iProcessStatus = process_freeze_data_stream_by_xml( g_stBufferGroup[cBufferOffset].cBuffer,
		                 iReceiveValidLen, pOut );

		if( iProcessStatus == MATCH_ERROR )
		{
			cErrorDID[0]	= *( byte* )pOut;
			cErrorDID[1]	= *( ( byte* )pOut + 1 );
			general_return_status( MATCH_ERROR, cErrorDID, 2, pOut );
		}
		else if( iProcessStatus == NO_FREEZE_DS )
		{
			general_return_status( NO_FREEZE_DS, NULL, 0, pOut );
		}

	}
	break;

	case NEGATIVE:
	case FRAME_TIME_OUT:
	case TIME_OUT:
	default:
		general_return_status( iReceiveResult, g_stBufferGroup[cBufferOffset].cBuffer, 3, pOut );
		break;
	}
}
/*
typedef struct _STRUCT_UDS_FREEZE_DTC_CONFIG
{
byte cFreezeDtcStartOffset;	//��Ч�ظ��б������DTC��ʼƫ�ƣ���SID�ظ���ʼ
byte cDtcBytesInCmd;	//�����м����ֽڱ�ʾһ��������
byte cDtcBytesInDisplay;//һ����������Ҫ��ʾ�����ֽ�
byte cReadFreezeDsMode; //��ȡ����֡������ģʽ 0�����ݹ����� 1�����ݴ洢��

}STRUCT_UDS_FREEZE_DTC_CONFIG;
*/

/*************************************************
Description:	����֧�ֶ���֡�Ĺ�����������洢��
Input:
	pcDctData	������ظ�����洢��ַ
	iValidLen	��Ч����

Output:	pstDtc	�������ָ��
Return:	int ����״̬
Others:	����������IS014229Э�鴦����ȡ����
		Ϊ��19 02/03;
*************************************************/
int process_freeze_DTC_data_by_ISO14229( byte* pcDctData, int iValidLen, STRUCT_CHAIN_DATA_OUTPUT* pstDtc )
{
	int i = 0;
	int j = 0;
	int iDtcNum = 0;
	byte *pcDtcStart = NULL;//��������ʼ��ŵ�ַ

	byte cFreezeDtcStartOffset	= g_p_stUDSFreezeDtcConfig->cFreezeDtcStartOffset;	//��Ч�ظ��б������DTC��ʼƫ�ƣ���SID�ظ���ʼ
	byte cDtcBytesInCmd			= g_p_stUDSFreezeDtcConfig->cDtcBytesInCmd;	//�����м����ֽڱ�ʾһ��������
	byte cDtcBytesInDisplay		= g_p_stUDSFreezeDtcConfig->cDtcBytesInDisplay;//һ����������Ҫ��ʾ�����ֽ�
	byte cSaveRecordNumberOffset = g_p_stUDSFreezeDtcConfig->cSaveRecordNumberOffset;

	STRUCT_CHAIN_DATA_NODE *phead  = NULL;
	STRUCT_CHAIN_DATA_NODE *ptemp1 = NULL;
	STRUCT_CHAIN_DATA_NODE *ptemp2 = NULL;
	STRUCT_CHAIN_DATA_NODE *ptemp3 = NULL;

	iDtcNum = ( iValidLen -  cFreezeDtcStartOffset ) / cDtcBytesInCmd;
	pcDtcStart = pcDctData + cFreezeDtcStartOffset;

	pstDtc->cKeyByte[0] = 1;
	pstDtc->cKeyByte[1] = 0;
	pstDtc->cKeyByte[2] = 0;
	pstDtc->stJump.cLenHighByte = 0;
	pstDtc->stJump.cLenLowByte = 0;
	pstDtc->stTip.cLenHighByte = 0;
	pstDtc->stTip.cLenLowByte = 0;
	pstDtc->pstData = NULL;

	if( 0 == iDtcNum ) //û�ж���֡������
		return NO_FREEZE_DTC;

	for( i = 0; i < iDtcNum; i++ )
	{
		ptemp2 = ( STRUCT_CHAIN_DATA_NODE * )malloc( sizeof( STRUCT_CHAIN_DATA_NODE ) ); //dtc id
		ptemp2->cLenHighByte = 0;
		ptemp2->cLenLowByte = cDtcBytesInDisplay * 2;//һ���ֽڶ�Ӧ���ַ�����Ϊ2
		ptemp2->pcData = ( byte * )malloc( ( cDtcBytesInDisplay * 2 + 1 ) * sizeof( byte ) );

		for( j = 0; j < cDtcBytesInDisplay; j++ )
		{
			/*sprintf_s( &( ptemp2->pcData[j * 2] ), ( cDtcBytesInDisplay * 2 + 1 - j * 2 ), "%02X", pcDtcStart[j + i * cDtcBytesInCmd] );*/
			sprintf( &( ptemp2->pcData[j * 2] ), "%02X", pcDtcStart[j + i * cDtcBytesInCmd] );
		}

		ptemp3 = ( STRUCT_CHAIN_DATA_NODE * )malloc( sizeof( STRUCT_CHAIN_DATA_NODE ) ); //dtc status

		ptemp3->cLenHighByte = 0;	//����֡������û��״̬
		ptemp3->cLenLowByte = 0;
		ptemp3->pcData = NULL;

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

	pstDtc->pstData = phead;

	return SUCCESS;
}
/*************************************************
Description:	������֡��������
Input:
	pcDctData	������ظ�����洢��ַ
	iValidLen	��Ч����

Output:	pOut	������ݵ�ַ������ƥ�����ʱ
				��ų���ʱ��DID��
Return:	void
Others:	����������IS014229Э�鴦����ȡ����
		Ϊ��19 04/05
*************************************************/
int process_freeze_data_stream_by_ISO14229( byte* pcDsData, int iValidLen,  void* pOut )
{
	byte *pcOutTemp = ( byte* )pOut;
	byte *pcHead = pcOutTemp;
	byte i = 0;
	byte j = 0;
	byte k = 0;
	byte m = 0;
	byte cDataValueCache[128] = {0};
	byte *pcFreezeDsData = NULL;
	byte cFreezeDsStartOffset = g_p_stUDSFreezeDtcConfig->cFreezeDsStartOffset;
	byte cDtcSupportDIDSum = pcDsData[cFreezeDsStartOffset];//�õ���DTC����֧֡�ֵ�DID����
	byte cSysSupportDIDSum  = g_stInitXmlGobalVariable.m_cFreezeDSDIDSum;//�õ���ϵͳ����֧֡�ֵ�DID����
	STRUCT_UDS_FREEZE_DS_CONFIG *pstFreezeConfig = NULL;
	int iAddressOffset = 0;
	int iDsID = 0;
	bool bMatchIDStatus = false;

	UNN_2WORD_4BYTE uDataLen;
	uDataLen.u32Bit = 0;

	pcFreezeDsData = pcDsData + cFreezeDsStartOffset + 1;//ָ���һ��DID

	pcOutTemp[0] = 1;
	pcOutTemp[1] = 0;
	pcOutTemp[2] = 0;

	pcOutTemp[3] = 0;
	pcOutTemp[4] = 0;
	pcOutTemp[5] = 0;
	pcOutTemp[6] = 0;
	pcOutTemp[7] = 0;//������
	pcOutTemp[8] = 0;//������
	pcOutTemp[9] = 0;//����ʾ
	pcOutTemp[10] = 0;//����ʾ

	uDataLen.u32Bit += 11;

	pcOutTemp = &pcOutTemp[11];//���⿪ʼ���������ID���ȡ�������ID��������ֵ���ȡ�������ֵ

	if( iValidLen <= 6 ) //����Ч����С�ڵ���6����Ϊ�޶���֡������,�μ�ISO14229��ز���
		return NO_FREEZE_DS;

	for( i = 0; i < cDtcSupportDIDSum;i++)
	{
		bMatchIDStatus = false;

		for( j = 0; j < cSysSupportDIDSum; j++ )
		{
			if( pcFreezeDsData[0] == g_p_stUDSFreezeDSConfig[j].cDIDHighByte
			        && pcFreezeDsData[1] == g_p_stUDSFreezeDSConfig[j].cDIDLowByte )
			{
				pstFreezeConfig = &g_p_stUDSFreezeDSConfig[j];
				bMatchIDStatus = true;
				break;
			}
		}

		if( !bMatchIDStatus )
		{
			pcHead[0] = pcFreezeDsData[0];//���浱ǰ���յ�DID
			pcHead[1] = pcFreezeDsData[1];

			return MATCH_ERROR;//ƥ��ʧ�ܣ�����
		}

		iDsID = 0;

		for( k = 0; k < pstFreezeConfig->cDSItemSum; k++ )
		{
			iDsID = pstFreezeConfig->pcSpecificDIDRule[0 + k * 4];
			iDsID <<= 8;
			iDsID += pstFreezeConfig->pcSpecificDIDRule[1 + k * 4];
			/*iAddressOffset = sprintf_s( pcOutTemp + 1, 10, "%04d", iDsID ); //���ʹ�ӡ������ID*/
			iAddressOffset = sprintf( pcOutTemp + 2, "%04d", iDsID ); //���ʹ�ӡ������ID
			*pcOutTemp = 0;
			pcOutTemp++;
			*pcOutTemp = iAddressOffset;
			pcOutTemp += iAddressOffset + 1;
			uDataLen.u32Bit += iAddressOffset + 2;

			memset( cDataValueCache, 0, sizeof( cDataValueCache ) );

			for( m = 0; m < g_p_stFreezeDSFormulaConfig->cItemSum; m++ )
			{
				if( iDsID == g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[m].iDSID )  //�ж϶���֡������ID
				{
					if( FORMULA_PARSER == g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[m].cFormulaType )
					{
						//���ù�ʽ������
						calculate(	pcFreezeDsData + 2 + pstFreezeConfig->pcSpecificDIDRule[2 + k * 4],
						            g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[m].cValidByteNumber,
						            g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[m].pcFormula,
						            g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[m].iFormulaLen,
						            g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[m].pStrFormat,
						            cDataValueCache
						         );
					}
					else
					{
						process_freeze_ds_calculate( iDsID, pcFreezeDsData + 2 + pstFreezeConfig->pcSpecificDIDRule[2 + k * 4],
						                             cDataValueCache );
					}

					break;

				}
			}

			*pcOutTemp = 0;
			pcOutTemp++;
			*pcOutTemp = ( byte )strlen( cDataValueCache );
			pcOutTemp++;
			uDataLen.u32Bit += 2;
			memcpy( pcOutTemp, cDataValueCache, strlen( cDataValueCache ) ); //��ż�����ֵ
			pcOutTemp += ( byte )strlen( cDataValueCache );
			uDataLen.u32Bit += ( byte )strlen( cDataValueCache );
		}

		pcFreezeDsData += 2 + pstFreezeConfig->cNeedByteSum;
	}

	pcHead[3] = uDataLen.u8Bit[3];
	pcHead[4] = uDataLen.u8Bit[2];
	pcHead[5] = uDataLen.u8Bit[1];
	pcHead[6] = uDataLen.u8Bit[0];

	return SUCCESS;
}
/*************************************************
Description:	������֡�����������������ǹ̶��ģ���ͨ��ID���ң�ֱ����ʾXML�е������
Input:
pcDctData	������ظ�����洢��ַ
iValidLen	��Ч����

Output:	pOut	������ݵ�ַ������ƥ�����ʱ
��ų���ʱ��DID��
Return:	void
*************************************************/
int process_freeze_data_stream_by_xml( byte* pcDsData, int iValidLen,  void* pOut )
{
	byte *pcOutTemp = ( byte* )pOut;
	byte *pcHead = pcOutTemp;
	byte i = 0;
	byte j = 0;
	byte k = 0;
	byte m = 0;
	byte cDataValueCache[128] = {0};
	byte *pcFreezeDsData = NULL;
	byte cFreezeDsStartOffset = g_p_stUDSFreezeDtcConfig->cFreezeDsStartOffset;
	byte cDtcSupportDIDSum = pcDsData[cFreezeDsStartOffset];//�õ���DTC����֧֡�ֵ�DID����
	byte cSysSupportDIDSum  = g_stInitXmlGobalVariable.m_cFreezeDSDIDSum;//�õ���ϵͳ����֧֡�ֵ�DID����
	STRUCT_UDS_FREEZE_DS_CONFIG *pstFreezeConfig = NULL;
	int iAddressOffset = 0;
	int iDsID = 0;
	bool bMatchIDStatus = false;

	UNN_2WORD_4BYTE uDataLen;
	uDataLen.u32Bit = 0;

	pcFreezeDsData = pcDsData;//ָ���һ��DID

	pcOutTemp[0] = 1;
	pcOutTemp[1] = 0;
	pcOutTemp[2] = 0;

	pcOutTemp[3] = 0;
	pcOutTemp[4] = 0;
	pcOutTemp[5] = 0;
	pcOutTemp[6] = 0;
	pcOutTemp[7] = 0;//������
	pcOutTemp[8] = 0;//������
	pcOutTemp[9] = 0;//����ʾ
	pcOutTemp[10] = 0;//����ʾ

	uDataLen.u32Bit += 11;

	pcOutTemp = &pcOutTemp[11];//���⿪ʼ���������ID���ȡ�������ID��������ֵ���ȡ�������ֵ

	if( iValidLen <= 6 ) //����Ч����С�ڵ���6����Ϊ�޶���֡������,�μ�ISO14229��ز���
		return NO_FREEZE_DS;

	for( i = 0; i < g_p_stFreezeDSFormulaConfig->cItemSum; i++ )
	{
		bMatchIDStatus = false;

		for( j = 0; j < cSysSupportDIDSum; j++ )
		{
			if( 0 == g_p_stUDSFreezeDSConfig[j].cDIDHighByte
			        && i == g_p_stUDSFreezeDSConfig[j].cDIDLowByte )
			{
				pstFreezeConfig = &g_p_stUDSFreezeDSConfig[j];
				bMatchIDStatus = true;
				break;
			}
		}

		if( !bMatchIDStatus )
		{
			pcHead[0] = pcFreezeDsData[0];//���浱ǰ���յ�DID
			pcHead[1] = pcFreezeDsData[1];

			return MATCH_ERROR;//ƥ��ʧ�ܣ�����
		}

		for( k = 0; k < pstFreezeConfig->cDSItemSum; k++ )
		{
			iDsID = pstFreezeConfig->pcSpecificDIDRule[0 + k * 4];
			iDsID <<= 8;
			iDsID += pstFreezeConfig->pcSpecificDIDRule[1 + k * 4];
			/*iAddressOffset = sprintf_s( pcOutTemp + 1, 10, "%04d", iDsID ); //���ʹ�ӡ������ID*/
			iAddressOffset = sprintf( pcOutTemp + 2, "%04d", iDsID ); //���ʹ�ӡ������ID
			*pcOutTemp = 0;
			pcOutTemp++;
			*pcOutTemp = iAddressOffset;
			pcOutTemp += iAddressOffset + 1;
			uDataLen.u32Bit += iAddressOffset + 2;

			memset( cDataValueCache, 0, sizeof( cDataValueCache ) );

			for( m = 0; m < g_p_stFreezeDSFormulaConfig->cItemSum; m++ )
			{
				if( iDsID == g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[m].iDSID )  //�ж϶���֡������ID
				{
					if( FORMULA_PARSER == g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[m].cFormulaType )
					{
						//���ù�ʽ������
						calculate(	pcFreezeDsData + pstFreezeConfig->pcSpecificDIDRule[2 + k * 4],
						            g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[m].cValidByteNumber,
						            g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[m].pcFormula,
						            g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[m].iFormulaLen,
						            g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[m].pStrFormat,
						            cDataValueCache
						         );
					}
					else
					{
						process_freeze_ds_calculate( iDsID, pcFreezeDsData + 2 + pstFreezeConfig->pcSpecificDIDRule[2 + k * 4],
						                             cDataValueCache );
					}

					break;
				}
			}

			*pcOutTemp = 0;
			pcOutTemp++;
			*pcOutTemp = ( byte )strlen( cDataValueCache );
			pcOutTemp++;
			uDataLen.u32Bit += 2;
			memcpy( pcOutTemp, cDataValueCache, strlen( cDataValueCache ) ); //��ż�����ֵ
			pcOutTemp += ( byte )strlen( cDataValueCache );
			uDataLen.u32Bit += ( byte )strlen( cDataValueCache );
		}
	}

	pcHead[3] = uDataLen.u8Bit[3];
	pcHead[4] = uDataLen.u8Bit[2];
	pcHead[5] = uDataLen.u8Bit[1];
	pcHead[6] = uDataLen.u8Bit[0];

	return SUCCESS;
}

/*************************************************
Description:	������֡��������
Input:
	pcDctData	������ظ�����洢��ַ
	iValidLen	��Ч����

Output:	pOut	������ݵ�ַ������ƥ�����ʱ
				��ų���ʱ��DID��
Return:	void
Others:	����������IS014230Э�鴦����ȡ����
		Ϊ��12 xx 00
*************************************************/
int process_one_memory_request_all_data_freeze_data_stream_by_ISO14230( byte* pcDsData, int iValidLen,  void* pOut )
{
	byte *pcOutTemp = ( byte* )pOut;
	byte *pcHead = pcOutTemp;
	byte i = 0;
	byte j = 0;
	byte k = 0;
	byte m = 0;
	byte cDataValueCache[128] = {0};
	byte *pcFreezeDsData = NULL;
	byte cFreezeDsStartOffset = 2;
	byte cGroupValidDataSum = ( byte )( iValidLen - 2 - 1 ); //�õ�������Ч���ݵ���������ID+�ؼ��ֽ�
	byte cSysSupportDIDSum  = g_stInitXmlGobalVariable.m_cFreezeDSDIDSum;//�õ���ϵͳ����֧֡�ֵ�DID����
	STRUCT_UDS_FREEZE_DS_CONFIG *pstFreezeConfig = NULL;
	int iAddressOffset = 0;
	int iDsID = 0;
	bool bMatchIDStatus = false;

	UNN_2WORD_4BYTE uDataLen;
	uDataLen.u32Bit = 0;

	pcFreezeDsData = pcDsData + cFreezeDsStartOffset;//ָ���һ��DID

	pcOutTemp[0] = 1;
	pcOutTemp[1] = 0;
	pcOutTemp[2] = 0;

	pcOutTemp[3] = 0;
	pcOutTemp[4] = 0;
	pcOutTemp[5] = 0;
	pcOutTemp[6] = 0;
	pcOutTemp[7] = 0;//������
	pcOutTemp[8] = 0;//������
	pcOutTemp[9] = 0;//����ʾ
	pcOutTemp[10] = 0;//����ʾ

	uDataLen.u32Bit += 11;

	pcOutTemp = &pcOutTemp[11];//���⿪ʼ���������ID���ȡ�������ID��������ֵ���ȡ�������ֵ

	//����Ч����С�ڵ���3����Ϊ�޶���֡������,�μ�ISO14230��ز���,���������������Ӧ�����
	if( iValidLen <= 3 )
		return NO_FREEZE_DS;

	for( i = 0; i < cGroupValidDataSum; )
	{
		bMatchIDStatus = false;

		for( j = 0; j < cSysSupportDIDSum; j++ )
		{
			if( pcFreezeDsData[0] == g_p_stUDSFreezeDSConfig[j].cDIDHighByte
			        && pcFreezeDsData[1] == g_p_stUDSFreezeDSConfig[j].cDIDLowByte )
			{
				pstFreezeConfig = &g_p_stUDSFreezeDSConfig[j];
				bMatchIDStatus = true;
				break;
			}
		}

		if( !bMatchIDStatus )
		{
			pcHead[0] = pcFreezeDsData[0];//���浱ǰ���յ�DID
			pcHead[1] = pcFreezeDsData[1];

			return MATCH_ERROR;//ƥ��ʧ�ܣ�����
		}

		iDsID = 0;

		for( k = 0; k < pstFreezeConfig->cDSItemSum; k++ )
		{
			iDsID = pstFreezeConfig->pcSpecificDIDRule[0 + k * 4];
			iDsID <<= 8;
			iDsID += pstFreezeConfig->pcSpecificDIDRule[1 + k * 4];
			/*iAddressOffset = sprintf_s( pcOutTemp + 1, 10, "%04d", iDsID ); //���ʹ�ӡ������ID*/
			iAddressOffset = sprintf( pcOutTemp + 2, "%04d", iDsID ); //���ʹ�ӡ������ID
			*pcOutTemp = 0;
			pcOutTemp++;
			*pcOutTemp = iAddressOffset;
			pcOutTemp += iAddressOffset + 1;
			uDataLen.u32Bit += iAddressOffset + 2;

			memset( cDataValueCache, 0, sizeof( cDataValueCache ) );

			for( m = 0; m < g_p_stFreezeDSFormulaConfig->cItemSum; m++ )
			{
				if( iDsID == g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[m].iDSID )  //�ж϶���֡������ID
				{
					if( FORMULA_PARSER == g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[m].cFormulaType )
					{
						//���ù�ʽ������
						calculate(	pcFreezeDsData + 2 + pstFreezeConfig->pcSpecificDIDRule[2 + k * 4],
						            g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[m].cValidByteNumber,
						            g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[m].pcFormula,
						            g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[m].iFormulaLen,
						            g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[m].pStrFormat,
						            cDataValueCache
						         );
					}
					else
					{
						process_freeze_ds_calculate( iDsID, pcFreezeDsData + 2 + pstFreezeConfig->pcSpecificDIDRule[2 + k * 4],
						                             cDataValueCache );
					}

					break;
				}
			}

			*pcOutTemp = 0;
			pcOutTemp++;
			*pcOutTemp = ( byte )strlen( cDataValueCache );
			pcOutTemp++;
			uDataLen.u32Bit += 2;
			memcpy( pcOutTemp, cDataValueCache, strlen( cDataValueCache ) ); //��ż�����ֵ
			pcOutTemp += ( byte )strlen( cDataValueCache );
			uDataLen.u32Bit += ( byte )strlen( cDataValueCache );
		}

		pcFreezeDsData += 2 + pstFreezeConfig->cNeedByteSum;

		i += 2 + pstFreezeConfig->cNeedByteSum;

	}

	pcHead[3] = uDataLen.u8Bit[3];
	pcHead[4] = uDataLen.u8Bit[2];
	pcHead[5] = uDataLen.u8Bit[1];
	pcHead[6] = uDataLen.u8Bit[0];

	return SUCCESS;
}
