/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	���岿��ͨ�õĺ���
History:
	<author>	<time>		<desc>

************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include "../interface/protocol_define.h"
#include "public.h"
#include "../interface/protocol_interface.h"
#include "protocol_config.h"
#include "../protocol/iso_15765.h"
#include "../protocol/iso_14230.h"
#include "../command/command.h"
#include "../InitConfigFromXml/init_config_from_xml_lib.h"
#include "../SpecialFunction/special_function.h"
/*************************************************
Description:	���ݲ�ͬЭ�鷢�͡���������
Input:	piCmdIndex		����������ַ
Output:	none
Return:	int	�շ�����ʱ��״̬
Others:	����VCI�����е�ͨѶģʽ����Э������
*************************************************/
int send_and_receive_cmd( const int* piCmdIndex )
{
	int iReturnStatus = TIME_OUT;

// 	switch( g_stInitXmlGobalVariable.m_cProtocolConfigType ) //�������Э������ģ�����ͽ�������
// 	{
// 	case ISO15765_CONFIG://CAN
// 		iReturnStatus = send_and_receive_cmd_by_iso_15765( piCmdIndex );
// 		break;
// 
// 	case ISO14230_CONFIG://K��
// 		iReturnStatus = send_and_receive_cmd_by_iso_14230( piCmdIndex );
// 		break;
// 
// 	default:
// 		iReturnStatus = PROTOCOL_ERROR;
// 		break;
// 	}

	return iReturnStatus;
}
/*************************************************
Description:	�������ݵ������ַ
Input:	pstParamOut	������ݵ������ָ��
Output:	pOut		�����ַ
Return:	void
Others:
*************************************************/
void copy_data_to_out( void* pOut, const STRUCT_CHAIN_DATA_OUTPUT* pstParamOut )
{
	byte *pOutTemp = ( byte * )pOut;
	byte *pcSaveDataLen = NULL;
	uint16 u16DataLen = 0;
	UNN_2WORD_4BYTE DataLen;

	STRUCT_CHAIN_DATA_NODE *pTemp1 = pstParamOut->pstData;

	pcSaveDataLen = &pOutTemp[3];//�������ܳ�����ʼ��ַ

	memcpy( pOutTemp, pstParamOut->cKeyByte, 3 );

	pOutTemp += 3;
	u16DataLen += 3;

	pOutTemp += 4;//�����������ݳ��ȵ�4���ֽ�
	u16DataLen += 4;

	*pOutTemp = pstParamOut->stJump.cLenHighByte;
	pOutTemp++;
	*pOutTemp = pstParamOut->stJump.cLenLowByte;
	pOutTemp++;
	u16DataLen += 2;
	memcpy( pOutTemp, pstParamOut->stJump.pcData, ( pstParamOut->stJump.cLenLowByte + pstParamOut->stJump.cLenHighByte ) );
	pOutTemp += pstParamOut->stJump.cLenLowByte + pstParamOut->stJump.cLenHighByte;
	u16DataLen += pstParamOut->stJump.cLenLowByte + pstParamOut->stJump.cLenHighByte;

	*pOutTemp = pstParamOut->stTip.cLenHighByte;
	pOutTemp++;
	*pOutTemp = pstParamOut->stTip.cLenLowByte;
	pOutTemp++;
	u16DataLen += 2;
	memcpy( pOutTemp, pstParamOut->stTip.pcData, ( pstParamOut->stJump.cLenLowByte + pstParamOut->stJump.cLenHighByte ) );
	pOutTemp += pstParamOut->stJump.cLenLowByte + pstParamOut->stJump.cLenHighByte;
	u16DataLen += pstParamOut->stJump.cLenLowByte + pstParamOut->stJump.cLenHighByte;

	while( pTemp1 )
	{
		*pOutTemp = pTemp1->cLenHighByte;
		pOutTemp++;
		*pOutTemp = pTemp1->cLenLowByte;
		pOutTemp++;
		u16DataLen += 2;
		memcpy( pOutTemp, pTemp1->pcData, ( pTemp1->cLenHighByte + pTemp1->cLenLowByte ) );
		pOutTemp += pTemp1->cLenHighByte + pTemp1->cLenLowByte;
		u16DataLen += pTemp1->cLenHighByte + pTemp1->cLenLowByte;

		pTemp1 = pTemp1->pNextNode;
	}

	DataLen.u32Bit = ( uint32 )u16DataLen;

	pcSaveDataLen[0] = DataLen.u8Bit[3];
	pcSaveDataLen[1] = DataLen.u8Bit[2];
	pcSaveDataLen[2] = DataLen.u8Bit[1];
	pcSaveDataLen[3] = DataLen.u8Bit[0];

}
/*************************************************
Description:	�ͷŴ洢������������Ŀռ�
Input:	pstParamOut	������ݵ������ָ��
Output:	none
Return:	void
Others:
*************************************************/
void free_param_out_data_space( STRUCT_CHAIN_DATA_OUTPUT* pstParamOut )
{
	STRUCT_CHAIN_DATA_NODE *pTemp1 = pstParamOut->pstData;
	STRUCT_CHAIN_DATA_NODE *pTemp2 = NULL;

	while( pTemp1 )
	{
		pTemp2 = pTemp1->pNextNode;

		if( pTemp1->pcData != NULL )
		{
			free( pTemp1->pcData );
		}

		free( pTemp1 );

		pTemp1 = pTemp2;
	}
}
/*************************************************
Description:	������ʾ��Ϣ�ַ���
Input:	pcTipID	������ݵ������ָ��
Output:	pOut	�����ַ
Return:	int		��װ�ص����ݳ���
Others:	Ԥ����Ϊִ��ʧ�ܡ�������������Ҫ��ʾ����
*************************************************/
int general_load_tipID( const byte* pcTipID, void* pOut )
{
	byte *pOutTemp = ( byte* )pOut;
	pOutTemp[0] = 0;
	pOutTemp[1] = 0;
	pOutTemp[2] = 1;

	pOutTemp[3] = 0;
	pOutTemp[4] = 0;
	pOutTemp[5] = 0;
	pOutTemp[6] = 3 + 4 + 2 + 2 + ( byte )strlen( pcTipID );

	pOutTemp[7] = 0;//������
	pOutTemp[8] = 0;//������

	pOutTemp[9] = 0;
	pOutTemp[10] = ( pcTipID == NULL ) ? 0 : ( byte )strlen( pcTipID );

	memcpy( &pOutTemp[11], pcTipID, pOutTemp[10] );

	return( pOutTemp[6] );

}
/*************************************************
Description:	������Ϣ���ͷ�����ʾ����
Input:
	iStatus		��ʾ��Ϣ����
	pcSource	�����ʾ���ݵ�ַ
	cAppendLen	������ʾ���ݳ���

Output:	pOut	�����ַ
Return:	int		��װ�ص����ݳ���
Others:
*************************************************/
void general_return_status( const int iStatus, const byte* pcSource, const byte cAppendLen, void* pOut )
{
	byte *pOutTemp = ( byte* )pOut;
	int i = 0;
	int iLen = 0;

	switch( iStatus )
	{
	case SUCCESS:
	{
		general_load_tipID( g_cSuccess, pOut );
		pOutTemp[0]  = 1;//����ִ�гɹ�
	}
	break;

	case TIME_OUT:
	{
		general_load_tipID( g_cTime_out, pOut );
		pOutTemp[0]  = 2;//ͨѶ�ж�
	}
	break;

	case FRAME_TIME_OUT:
	{
		general_load_tipID( g_cFrame_time_out, pOut );
		pOutTemp[0]  = 2;//ͨѶ�ж�
	}
	break;

	case PROTOCOL_ERROR:
		general_load_tipID( g_cProtocol_error, pOut );
		break;

	case FAILE:
		general_load_tipID( g_cFaile, pOut );
		break;

	case FORMAT_ERORR:
		general_load_tipID( g_cFormat_error, pOut );
		break;

	case NULL_POINT:
		general_load_tipID( g_cNull_point, pOut );
		break;

	case NO_FREEZE_DS:
		general_load_tipID( g_cNo_freeze_DS, pOut );
		break;

	case NO_FREEZE_DTC:
		general_load_tipID( g_cNo_freeze_DTC, pOut );
		break;

	case NO_CURRENT_DTC:
		general_load_tipID( g_cNo_current_dtc, pOut );
		break;

	case NO_HISTORY_DTC:
		general_load_tipID( g_cNo_history_dtc, pOut );
		break;

	case RECEIVE_FRAME_TYPE_ERROR:
		general_load_tipID( g_cReceive_frame_type_error, pOut );
		break;

	case MESSAGE_TYPE_ERROR:
		general_load_tipID( g_cMessage_type_error, pOut );
		break;

	case INPUT_CHARACTER_FORMAT_ERROR:
		general_load_tipID( g_cInput_character_format_error, pOut );
		break;

	case INPUT_CHARACTER_NUMBER_ERROR:
		general_load_tipID( g_cInput_character_number_error, pOut );
		break;

	case NEGATIVE://������Ҫװ����������
	{
		iLen = general_load_tipID( g_cNegative, pOut );

		pOutTemp[2] = 2;//��ʾʱ����ʾ����

		pOutTemp[ iLen ] = 0; //���ȸ��ֽ�
		pOutTemp[ iLen + 1] = cAppendLen * 2; //���ȵ��ֽ�

		pOutTemp[6] += 2 + cAppendLen * 2; //�޸ĳ���

		pOutTemp += iLen + 2;

		for( ; i < cAppendLen; i++ )
		{
			/*sprintf_s( &pOutTemp[i * 2], ( cAppendLen * 2 + 1 - i * 2 ), "%02X", pcSource[i] );*/
			sprintf( &pOutTemp[i * 2], "%02X", pcSource[i] );
		}

	}
	break;

	case MATCH_ERROR:
	{
		iLen = general_load_tipID( g_cMatch_error, pOut );

		pOutTemp[2] = 2;//��ʾʱ����ʾ����

		pOutTemp[ iLen ] = 0; //���ȸ��ֽ�
		pOutTemp[ iLen + 1] = cAppendLen * 2; //���ȵ��ֽ�

		pOutTemp[6] += 2 + cAppendLen * 2; //�޸ĳ���

		pOutTemp += iLen + 2;

		for( ; i < cAppendLen; i++ )
		{
			/*sprintf_s( &pOutTemp[i * 2], ( cAppendLen * 2 + 1 - i * 2 ), "%02X", pcSource[i] );*/
			sprintf( &pOutTemp[i * 2], "%02X", pcSource[i] );
		}
	}
	break;

	default:
		break;
	}
}
/*************************************************
Description:	����ͨ�õ�ECU�ظ�����
Input:
	pcSource	�ظ������ŵ�ַ
	iValidLen	��Ч����

Output:	pstOut	�����ַ
Return:	void
Others:	�����ECU������Ӧ�����ݷŵ����������
*************************************************/
void process_general_response( const byte* pcSource, const int iValidLen, STRUCT_CHAIN_DATA_OUTPUT* pstOut )
{
	int i = 0;
	STRUCT_CHAIN_DATA_NODE* pstOutCache = ( STRUCT_CHAIN_DATA_NODE* )malloc( sizeof( STRUCT_CHAIN_DATA_NODE ) );

	pstOut->cKeyByte[0] = 1;
	pstOut->cKeyByte[1] = 0;
	pstOut->cKeyByte[2] = 0;
	pstOut->stJump.cLenHighByte = 0;
	pstOut->stJump.cLenLowByte = 0;
	pstOut->stTip.cLenHighByte = 0;
	pstOut->stTip.cLenLowByte = 0;

	pstOutCache->cLenHighByte = 0;
	pstOutCache->cLenLowByte = iValidLen * 2;
	pstOutCache->pcData = ( byte* )malloc( ( iValidLen * 2 + 1 ) * sizeof( byte ) );

	for( ; i < iValidLen; i++ )
	{
		/*sprintf_s( &( pstOutCache->pcData[i * 2] ), ( iValidLen * 2 + 1 - i * 2 ), "%02X", pcSource[i] );*/
		sprintf( &( pstOutCache->pcData[i * 2] ), "%02X", pcSource[i] );
	}

	pstOutCache->pNextNode = NULL;

	pstOut->pstData = pstOutCache;//���ӵ����������
}
/*************************************************
Description:	������������շ���û�к�������
Input:	iCmdOffset	����ƫ��
Output:	pOut	�����ַ
Return:	bool	���ش����״̬
Others:	����״̬���������ַpOut�У�
����Ĭ��Ϊ����ʾ
*************************************************/
bool process_single_cmd_without_subsequent_processing( const int iCmdOffset, void* pOut )
{
	int iReceiveResult = TIME_OUT;

	byte cBufferOffset = 0;//����ƫ��

	int iCmdIndex[] = {1, 0};

	iCmdIndex[1] = iCmdOffset;

	cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[iCmdOffset].cBufferOffset;

	iReceiveResult = send_and_receive_cmd( iCmdIndex );

	general_return_status( iReceiveResult, g_stBufferGroup[cBufferOffset].cBuffer, 3, pOut );

	if( iReceiveResult != SUCCESS )
		return false;

	return true;
}
/*************************************************
Description:	����У���
Input:
	pcSource	Ҫ����У��͵������׵�ַ
	iValidLen	Ҫ����У������ݵĳ���
Output:
Return:	byte	����У���
Others:
*************************************************/
byte calculate_Checksum( const byte* pcSource, const int iValidLen )
{
	byte cCheckNum = 0;
	int i = 0;

	for( ; i < iValidLen; i++ )
	{
		cCheckNum += pcSource[i];
	}

	return cCheckNum;
}

/*************************************************
Description:	���������VCI����
Input:	none
Output:	none
Return:	bool	����ʱ��״̬
Others:	���Է���������ʧ�����FAIL
*************************************************/
bool package_and_send_vci_config( void )
{
	byte cCheckNum = 0;
	int i = 0;
	bool bSendStatus = false;
	byte cSendCache[128] = {0}; //����֡�Ļ���
	byte cReceiveCache[5] = {0}; //���ջ���

	cSendCache[0] = FRAME_HEAD_SET_ALL;
	cSendCache[1] = 0x00;
	cSendCache[2] = 1 + 2 + 7 + 3 + 3 + 4 + 1 + g_p_stVCI_params_config->cCanFilterIDGroupNum * 4 + 1;
	cSendCache[3] = g_p_stVCI_params_config->cCommunicationType;
	cSendCache[4] = g_p_stVCI_params_config->cVoltage;
	cSendCache[5] = g_p_stVCI_params_config->cLevel;
	cSendCache[6] = g_p_stVCI_params_config->cLogic;
	cSendCache[7] = g_p_stVCI_params_config->cReceivePin;
	cSendCache[8] = g_p_stVCI_params_config->cSendPin;
	cSendCache[9] = g_p_stVCI_params_config->cCANFrameMode;

	memcpy( &cSendCache[10], g_p_stVCI_params_config->cECUBaudRate, 3 );
	memcpy( &cSendCache[13], g_p_stVCI_params_config->cVCIBaudRate, 3 );
	memcpy( &cSendCache[16], g_p_stVCI_params_config->cCanSamplingConfig, 4 );

	cSendCache[20] = g_p_stVCI_params_config->cCanFilterIDGroupNum;

	memcpy( &cSendCache[21], g_p_stVCI_params_config->cCanFilterID, g_p_stVCI_params_config->cCanFilterIDGroupNum * 4 );

	for( ; i < cSendCache[2] - 1; i++ )
	{
		cCheckNum += cSendCache[i];
	}

	cSendCache[ cSendCache[2] - 1] = cCheckNum;

	send_cmd( cSendCache, cSendCache[2] );

	if( ( bool )receive_cmd( cReceiveCache, 5, 3000 ) )
	{
		if( cReceiveCache[3] == 0x00 )
		{
			bSendStatus = true;
		}
	}

	return bSendStatus;

}

/*************************************************
Description:	�л��շ��ܽ�����
Input:	cReceivePin ���չܽ�
		cSendPin	���͹ܽ�
Output:	none
Return:	bool	�л�ʱ��״̬
Others:
*************************************************/
bool switch_channel( byte cReceivePin, byte cSendPin )
{
	g_p_stVCI_params_config->cReceivePin = cReceivePin;
	g_p_stVCI_params_config->cSendPin    = cSendPin;

	return( package_and_send_vci_config() );
}

/*************************************************
Description:	�޸�ͨѶ������
Input:	uuBandRate Ҫ�޸ĵĲ�����
Output:	none
Return:	bool	�޸�ʱ��״̬
Others:
*************************************************/
bool change_communicate_with_ECU_baudrate( uint32 uuBandRate )
{
	UNN_2WORD_4BYTE uBandRateTemp;

	uBandRateTemp.u32Bit = uuBandRate;

	g_p_stVCI_params_config->cECUBaudRate[0] = uBandRateTemp.u8Bit[2];
	g_p_stVCI_params_config->cECUBaudRate[1] = uBandRateTemp.u8Bit[1];
	g_p_stVCI_params_config->cECUBaudRate[2] = uBandRateTemp.u8Bit[0];

	return( package_and_send_vci_config() );
}

/*************************************************
Description:	��������ͼ�������
Input:	none
Output:	none
Return:	bool	����ʱ��״̬
Others:	���Է���������ʧ�����FAIL
*************************************************/
bool package_and_send_active_config( void )
{
	bool bSendStatus = false;
	STRUCT_CMD	stActiveModeTemp;

	byte cActiveTemp[30] = {0};
	byte *pcTemp = cActiveTemp;

	byte cTimeBetweenBytes	= g_p_stGeneralActiveEcuConfig->cTimeBetweenBytes;	//�ֽ����ֽڼ�ʱ��
	byte cActiveMode		= g_p_stGeneralActiveEcuConfig->cActiveMode;		//���ʽ
	byte cDetectBandRateFlag = g_p_stGeneralActiveEcuConfig->cDetectBandRateFlag;

	stActiveModeTemp.cBufferOffset = 0;
	stActiveModeTemp.cReserved = 0;
	stActiveModeTemp.iCmdLen = 1 + 1 + 3 + 1 + g_p_stGeneralActiveEcuConfig->cActiveAddress[0];

	memcpy( pcTemp, &cActiveMode, 1 ); //װ�ؼ���ģʽ

	pcTemp++;

	memcpy( pcTemp, &cDetectBandRateFlag, 1 ); //װ���Լ��־

	pcTemp++;

	memcpy( pcTemp, g_p_stGeneralActiveEcuConfig->cBandRate, 3 ); //װ�ز�����

	pcTemp += 3;

	memcpy( pcTemp, &g_p_stGeneralActiveEcuConfig->cActiveAddress[0], g_p_stGeneralActiveEcuConfig->cActiveAddress[0] + 1 ); //װ�ؼ����ַ����+��ַ

	stActiveModeTemp.pcCmd = cActiveTemp;

	//���ͼ�����������
	bSendStatus = package_and_send_frame( FRAME_HEAD_ACTIVE_ECU, ( STRUCT_CMD* )&stActiveModeTemp, cTimeBetweenBytes );

	if( !bSendStatus )
	{
		return false;
	}

	return true;

}

/*************************************************
Description:	�޸�����ȿռ�
Input:	iCmdOffset	���޸ĵ�����ƫ��
		iNewCmdLen	����Ҫ�޸ĳɵĳ���

Output:	none
Return:	none
Others:	ֱ���޸�g_stInitXmlGobalVariable.m_p_stCmdList
		�е�����,����ǰ�����С���趨�ĳ������д
		��ǰ����ĳ��Ȳ����·����ڴ棬�����޸ġ�
		�����䡣
*************************************************/
void change_cmd_space( const int iCmdOffset, const int iNewCmdLen )
{
	if( g_stInitXmlGobalVariable.m_p_stCmdList[iCmdOffset].iCmdLen < iNewCmdLen )
	{
		if( NULL != g_stInitXmlGobalVariable.m_p_stCmdList[iCmdOffset].pcCmd )
			free( g_stInitXmlGobalVariable.m_p_stCmdList[iCmdOffset].pcCmd );

		g_stInitXmlGobalVariable.m_p_stCmdList[iCmdOffset].pcCmd = ( byte * )malloc( sizeof( byte ) * iNewCmdLen );
	}

	g_stInitXmlGobalVariable.m_p_stCmdList[iCmdOffset].iCmdLen = iNewCmdLen;
}

/*************************************************
Description:	����VCI
Input:	none
Output:	none
Return:	bool	����VCI״̬
Others:	��Ϊ����package_and_send_frame��������ʵ��
����ʱ��������ǰ��һ�������ֽڵġ�
*************************************************/
bool reset_VCI( void )
{
	bool bSendStatus = false;
	STRUCT_CMD stSendCmd = {0};
	byte cSetContent[] = "RESET";

	stSendCmd.iCmdLen = sizeof( cSetContent ) - 1;

	stSendCmd.pcCmd = ( byte* )malloc( sizeof( byte ) * stSendCmd.iCmdLen );

	memcpy( stSendCmd.pcCmd, cSetContent, stSendCmd.iCmdLen );

	bSendStatus = package_and_send_frame( FRAME_HEAD_RESET_VCI, ( STRUCT_CMD* )&stSendCmd, 0xaa );

	free( stSendCmd.pcCmd );

	return bSendStatus;
}

/*************************************************
Description:	�ȽϺ���
Input:
		a		����1
		b		����2
Output:	none
Return:	int 	 ��ֵ�ȽϽ��
Others:	��qsort������
*************************************************/
int compare( const void *a, const void * b )
{
	return *( int * )a - *( int * )b;
}
/*************************************************
Description:	ɾ���������ظ�Ԫ��
Input:
		pi		�����������ָ��
		num		������Ԫ�ظ���
Output:	none
Return:	int		�������ЧԪ�ظ���
Others:	������ֱ�ӷ���piָ���������
*************************************************/
int remove_duplicate_int_element( int * pi, byte num )
{
	int i = 0;
	int k = 0;

	qsort( pi, num, sizeof( int ), compare );

	for( i = 1; i < num; i++ )
	{
		//����Ƚ�����Ԫ�ؼ���
		if( pi[i] != pi[k] )
		{
			k += 1;

			pi[k] = pi[i];
		}
	}

	return k + 1;
}

/*************************************************
Description:	�ж�����ֵ��״̬
Input:
iRangeLen	�涨��Χ�ַ�������
pcRange		�涨��Χ�ַ���ָ��

��ʽ:  ����λ ��Сֵ ����λ ���ֵ
����:    1     100      0    100   ��Χ[-100~100]

����λ:  0��ʾ ����   ��0 ��ʾ����

iValueLen	�����ַ�������
pcValue		�����ַ���ָ��
return  int		�ж�����ֵ�Ľ������ֵ��Ϊ��
				INPUT_FORMAT_ERROR
				INPUT_OUT_OF_RANGE
				INPUT_WITHIN_THE_RANGE
Others:	����ֵ����������,��һλ������'+'��'-' ��
******************************************************/

int judge_input_value( const int iRangeLen, const byte* pcRange, const int iValueLen, const byte* pcValue )
{
	uint32 u32InputValueRange[4] = {0};
	int iInputValue = 0;
	int i = 0;

	assert( iRangeLen != 0 && pcRange );

	assert( iValueLen != 0 && pcValue );

	for( i = 0; i < iValueLen; i++ )
	{
		if( !isdigit( pcValue[i] ) )
		{
			if( 0 == i )
			{
				if( pcValue[i] == '-' || pcValue[i] == '+' )
					continue;
			}

			return INPUT_FORMAT_ERROR;

		}
	}

	iInputValue = atoi( pcValue );
	//��ȡ�ο���Χ������
	get_string_type_data_to_uint32( u32InputValueRange, pcRange, iRangeLen );

	if( 0 == u32InputValueRange[0] ) //0���� ��0�Ǹ�
	{
		u32InputValueRange[1] = 0 - u32InputValueRange[1] ; //��Сֵ
	}

	if( 0 == u32InputValueRange[2] ) //0���� ��0�Ǹ�
	{
		u32InputValueRange[3] = 0 - u32InputValueRange[3] ; //���ֵ
	}

	if( ( iInputValue < ( int )u32InputValueRange[1] ) || ( iInputValue > ( int )u32InputValueRange[3] ) )
	{
		return INPUT_OUT_OF_RANGE;
	}

	return INPUT_WITHIN_THE_RANGE;

}

/*************************************************
Description:	����FC֡
Input:
Output:
Return:
Others:
*************************************************/

void process_SET_CONFIG_FC_CMD_CAN( void* pIn, void* pOut )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	byte *pcOutTemp = ( byte * )pOut;
	STRUCT_CMD * FcCmdData = NULL;

	bool bSetStatus = false;
	int FcFrameNumber = 0;
	int i = 0;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	FcFrameNumber = get_command_config_data( pstParam, &FcCmdData );

	if( 0 == FcFrameNumber )
	{
		//�ͷ��ڴ�
		if( NULL != FcCmdData )
		{
			free( FcCmdData );
			FcCmdData = NULL;
		}

		general_return_status( SUCCESS, NULL, 0, pcOutTemp );
		pcOutTemp[2] = 0;//����ʾ
		return ;
	}

	bSetStatus = bSend_FC_Frame_Cmd( FcCmdData, u32Config_fuc[0], u32Config_fuc[1], u32Config_fuc[2] );

	if( bSetStatus ) //������ճɹ������óɹ��򷵻�SUCCESS
	{
		general_return_status( SUCCESS, NULL, 0, pcOutTemp );
		pcOutTemp[2] = 0;//����ʾ
	}
	else
	{
		general_return_status( FAILE, NULL, 0, pcOutTemp );
		pcOutTemp[2] = 0;//����ʾ
	}

	//�ͷ��ڴ�
	if( NULL != FcCmdData )
	{
		if( NULL != FcCmdData->pcCmd )
		{
			free( FcCmdData->pcCmd );
			FcCmdData->pcCmd = NULL;
		}

		free( FcCmdData );
		FcCmdData = NULL;
	}

}



/*************************************************
Description:	����FC֡
Input:
		FcCmdData		��������ĵ�ַ
		FcFrameTime		30֡���͵���ʱʱ��
        WhichByte       ���0x10�ֽ�λ��
        MultiByte      �жϵĶ�֡�ֽ�
Output:	bool
Return:
Others:
*************************************************/
bool bSend_FC_Frame_Cmd( const STRUCT_CMD * FcCmdData, const uint32 FcFrameTime, const uint32 WhichByte, const uint32 MultiByte )
{
	STRUCT_CMD  SendFcCmdData = {0};
	bool  bSetStatus = false;
	SendFcCmdData.iCmdLen = FcCmdData->iCmdLen + 2;
	SendFcCmdData.pcCmd = ( byte* )malloc( sizeof( byte ) * SendFcCmdData.iCmdLen );

	SendFcCmdData.pcCmd[0] = ( byte )WhichByte;
	SendFcCmdData.pcCmd[1] = ( byte )MultiByte;
	memcpy( ( SendFcCmdData.pcCmd ) + 2, FcCmdData->pcCmd, FcCmdData->iCmdLen );
	bSetStatus = package_and_send_frame( FRAME_HEAD_RESET_CAN_FC_FRAME, ( STRUCT_CMD * )&SendFcCmdData, ( byte )FcFrameTime );

	if( NULL != SendFcCmdData.pcCmd )
	{
		free( SendFcCmdData.pcCmd );
		SendFcCmdData.pcCmd = NULL;
	}

	return bSetStatus;
}

/*************************************************
Description:	����ECU�Ƿ�ظ�
Input:
Output:
Return:
Others:
*************************************************/
void process_SET_ECU_REPLY( void* pIn, void* pOut )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	byte *pcOutTemp = ( byte * )pOut;
	byte * pcTemp = NULL;
	bool bSetStatus = false;
	int i = 0;

	//assert(pstParam->pcData);
	//assert(pstParam->iLen != 0);
	pcTemp = pstParam->pcData;

	if( ( pstParam->iLen == 0 ) )
	{
		general_return_status( SUCCESS, NULL, 0, pcOutTemp );
		pcOutTemp[2] = 0;//����ʾ
		return;
	}

	for( i = 0; i < ( int )strlen( pcTemp ) ; i++ )
	{
		pcTemp[i] = toupper( pcTemp[i] ); //ת���д
	}

	if( 0 != strcmp( pcTemp, "TRUE" ) )
	{
		general_return_status( SUCCESS, NULL, 0, pcOutTemp );
		pcOutTemp[2] = 0;//����ʾ
		return;
	}

	bSetStatus = bSend_Ecu_Reply_Cmd( ( int )u32Config_fuc[0], ( int )u32Config_fuc[1] );

	if( bSetStatus ) //������ճɹ������óɹ��򷵻�SUCCESS
	{
		general_return_status( SUCCESS, NULL, 0, pcOutTemp );
		pcOutTemp[2] = 0;//����ʾ
	}
	else
	{
		general_return_status( FAILE, NULL, 0, pcOutTemp );
		pcOutTemp[2] = 0;//����ʾ
	}

}

/*************************************************
Description:	����ECU �Ƿ�ظ�����
Input:
		ECUReplyFlag		ECU�Ƿ�ظ�  0: ���ظ�   1:  �ظ�
		ECUReplyTimeOut		ECU�ظ�����ĳ�ʱʱ�䣬��λms  ��Χ (0 - 65535 )

Output:	bool
Return:
Others:
*************************************************/

bool bSend_Ecu_Reply_Cmd( int ECUReplyFlag, int ECUReplyTimeOut )
{
	STRUCT_CMD  SendECUReplyData = {0};
	bool  bSetStatus = false;
	SendECUReplyData.iCmdLen = 3;
	SendECUReplyData.pcCmd = ( byte* )malloc( sizeof( byte ) * 3 );

	SendECUReplyData.pcCmd[0] = ( byte )ECUReplyFlag;
	SendECUReplyData.pcCmd[1] = ( byte )( ECUReplyTimeOut / 256 );
	SendECUReplyData.pcCmd[2] = ( byte )( ECUReplyTimeOut % 256 );
	bSetStatus = package_and_send_frame( FRAME_HEAD_ECU_REPLAY, ( STRUCT_CMD * )&SendECUReplyData, 0xaa );	 //0xaa �Ǳ����ֽ�VDIû���õ�

	if( NULL != SendECUReplyData.pcCmd )
	{
		free( SendECUReplyData.pcCmd );
		SendECUReplyData.pcCmd = NULL;
	}

	return bSetStatus;

}


/*************************************************
Description:	���ռ�֡�������һ֡����
Input:
Output:
Return:
Others:
*************************************************/

void get_accord_ecu_cmdnum_send_cmdconfig_data( void* pIn, void* pOut )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	byte *pcOutTemp = ( byte * )pOut;
	STRUCT_CMD *SetCmdData = NULL;

	bool bSetStatus = false;
	int SetFrameNumber = 0;
	int i = 0;
	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	SetFrameNumber = get_command_config_data( pstParam, &SetCmdData );

	if( 0 == SetFrameNumber )
	{
		//�ͷ��ڴ�
		if( NULL != SetCmdData )
		{
			for( i = 0; i < SetFrameNumber; i++ )
			{
				if( NULL != SetCmdData[i].pcCmd )
				{
					free( SetCmdData[i].pcCmd );
					SetCmdData[i].pcCmd = NULL;
				}
			}

			free( SetCmdData );

			SetCmdData = NULL;
		}

		general_return_status( SUCCESS, NULL, 0, pcOutTemp );
		pcOutTemp[2] = 0;//����ʾ
		return ;
	}

	bSetStatus = bSend_Receive_config_Frame_Cmd( SetCmdData, SetFrameNumber, u32Config_fuc );

	if( bSetStatus ) //������ճɹ������óɹ��򷵻�SUCCESS
	{
		general_return_status( SUCCESS, NULL, 0, pcOutTemp );
		pcOutTemp[2] = 0;//����ʾ
	}
	else
	{
		general_return_status( FAILE, NULL, 0, pcOutTemp );
		pcOutTemp[2] = 0;//����ʾ
	}

	//�ͷ��ڴ�
	if( NULL != SetCmdData )
	{
		for( i = 0; i < SetFrameNumber; i++ )
		{
			if( NULL != SetCmdData[i].pcCmd )
			{
				free( SetCmdData[i].pcCmd );
				SetCmdData[i].pcCmd = NULL;
			}
		}

		free( SetCmdData );

		SetCmdData = NULL;
	}

}

/*************************************************
Description:	���ռ�֡�������һ֡����
Input:
		SetCmdData		�洢������
		SetFrameNumber	����ĸ���
        u32Config_fuc   ���������׵�ַ

Output:	bool
Return:
Others:
*************************************************/

bool bSend_Receive_config_Frame_Cmd( const STRUCT_CMD * SetCmdData, const int SetFrameNumber,
                                     const uint32 * u32Config_fuc )
{
	UNN_2WORD_4BYTE uFrameLen;
	bool bReceiveStatus = false;
	bool bReturnStatus = false;
	byte *pcSendCache = NULL;
	byte cReceiveCache[5] = {0};//���յĻ���
	byte cCheckNum = 0;
	int i = 0;
	int j = 0;

	uFrameLen.u32Bit = 0;

	for( i = 0; i < SetFrameNumber; i++ )
	{
		uFrameLen.u32Bit += SetCmdData[i].iCmdLen ;
	}


	uFrameLen.u32Bit += 1 + 2 + 11 + 1 + SetFrameNumber + 1;

	pcSendCache = ( byte * )malloc( ( uFrameLen.u32Bit ) * sizeof( byte ) ); //����֡�Ļ���

	pcSendCache[0] = FRAME_HEAD_SEND_RECEIVE;
	pcSendCache[1] = uFrameLen.u8Bit[1];
	pcSendCache[2] = uFrameLen.u8Bit[0];

	pcSendCache[3] = ( byte )u32Config_fuc[0]; //�ֽڼ���
	pcSendCache[4] = 0xff; //�����ֽ�
	pcSendCache[5] = 0xff; //�����ֽ�
	pcSendCache[6] = ( byte )u32Config_fuc[1]; //���÷�������Ĵ�/�ر�״̬
	pcSendCache[7] = ( byte )u32Config_fuc[2]; //��֡����Ŀ��Ʒ�ʽ
	pcSendCache[8] = u32Config_fuc[3] / 256; //���Ͳ�֡�����������
	pcSendCache[9] = u32Config_fuc[3] % 256; //���Ͳ�֡�����������
	pcSendCache[10] = u32Config_fuc[4] / 256; //Ҫ���͵��������һ֡ʱ��֡��֮֡��ļ��������֡����ķ��Ͳ���Ҫ���ã�
	pcSendCache[11] = u32Config_fuc[4] % 256; //Ҫ���͵��������һ֡ʱ��֡��֮֡��ļ��������֡����ķ��Ͳ���Ҫ���ã�
	pcSendCache[12] = ( byte )u32Config_fuc[5]; //���͵������Ƿ��лظ�
	pcSendCache[13] = ( byte )u32Config_fuc[6]; //��������Ļظ��Ƿ��ϴ�

	pcSendCache[14] = ( byte )SetFrameNumber; //��������

	for( i = 0 ; i < SetFrameNumber; i++ )
	{
		pcSendCache[14 + j + 1 ] = SetCmdData[i].iCmdLen;
		memcpy( &pcSendCache[14 + j + 2], SetCmdData[i].pcCmd, SetCmdData[i].iCmdLen );
		j += SetCmdData[i].iCmdLen + 1;
	}

	for( i = 0; ( i < ( int )uFrameLen.u32Bit - 1 ); i++ )
	{
		cCheckNum += pcSendCache[i];
	}

	pcSendCache[ uFrameLen.u32Bit - 1] = cCheckNum;

	send_cmd( pcSendCache, uFrameLen.u32Bit );

	if( ( bool )receive_cmd( cReceiveCache, 5, 3000 ) )
	{
		if( cReceiveCache[3] == 0x00 )
		{
			bReturnStatus = true;
		}
	}

	free( pcSendCache );

	return bReturnStatus;
}

