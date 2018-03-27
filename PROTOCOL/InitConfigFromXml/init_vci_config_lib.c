/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	�����ȡVCI���ô�����
History:
	<author>	<time>		<desc>

***********************************************************************/
#include "init_config_from_xml_lib.h"
#include "..\public\protocol_config.h"
#include <assert.h>
#include <stdlib.h>
#include "..\interface\protocol_define.h"
/*************************************************
Description:	��ȡVCI����
Input:
	PIn				������������
Output:	����
Return:	��
Others: �����Ὺ�ٿռ����������ݣ��ռ�ָ�븳��
		g_p_stVciParamsGroup����Ӧ��
*************************************************/
void get_vci_config_data( void* pIn )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	uint32 u32VciConfigTemp[50] = {0};
	byte cConfigOffset = 0;
	UNN_2WORD_4BYTE unnTemp;
	byte i = 0;

	unnTemp.u32Bit = 0;
	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	get_config_data( u32VciConfigTemp, pstParam->pcData, NO_LENGTH_LIMIT );

	cConfigOffset = ( byte )u32VciConfigTemp[0];

	//���Ϊ�����¿��ٿռ���򸲸���ǰ��
	if( g_p_stVciParamsGroup[cConfigOffset] == NULL )
		g_p_stVciParamsGroup[cConfigOffset] = ( STRUCT_VCI_PARAMS * )malloc( sizeof( STRUCT_VCI_PARAMS ) );

	g_p_stVciParamsGroup[cConfigOffset]->cCommunicationType		= ( byte )u32VciConfigTemp[2];
	g_p_stVciParamsGroup[cConfigOffset]->cVoltage				= ( byte )u32VciConfigTemp[3];
	g_p_stVciParamsGroup[cConfigOffset]->cLevel					= ( byte )u32VciConfigTemp[4];
	g_p_stVciParamsGroup[cConfigOffset]->cLogic					= ( byte )u32VciConfigTemp[5];
	g_p_stVciParamsGroup[cConfigOffset]->cReceivePin			= ( byte )u32VciConfigTemp[6];
	g_p_stVciParamsGroup[cConfigOffset]->cSendPin				= ( byte )u32VciConfigTemp[7];
	g_p_stVciParamsGroup[cConfigOffset]->cCANFrameMode			= ( byte )u32VciConfigTemp[8];

	unnTemp.u32Bit = u32VciConfigTemp[9];
	g_p_stVciParamsGroup[cConfigOffset]->cECUBaudRate[0]		= unnTemp.u8Bit[2];
	g_p_stVciParamsGroup[cConfigOffset]->cECUBaudRate[1]		= unnTemp.u8Bit[1];
	g_p_stVciParamsGroup[cConfigOffset]->cECUBaudRate[2]		= unnTemp.u8Bit[0];

	unnTemp.u32Bit = u32VciConfigTemp[10];
	g_p_stVciParamsGroup[cConfigOffset]->cVCIBaudRate[0]		= unnTemp.u8Bit[2];
	g_p_stVciParamsGroup[cConfigOffset]->cVCIBaudRate[1]		= unnTemp.u8Bit[1];
	g_p_stVciParamsGroup[cConfigOffset]->cVCIBaudRate[2]		= unnTemp.u8Bit[0];

	g_p_stVciParamsGroup[cConfigOffset]->cCanSamplingConfig[0]	= ( byte )u32VciConfigTemp[11];
	g_p_stVciParamsGroup[cConfigOffset]->cCanSamplingConfig[1]	= ( byte )u32VciConfigTemp[12];
	g_p_stVciParamsGroup[cConfigOffset]->cCanSamplingConfig[2]	= ( byte )u32VciConfigTemp[13];
	g_p_stVciParamsGroup[cConfigOffset]->cCanSamplingConfig[3]	= ( byte )u32VciConfigTemp[14];

	g_p_stVciParamsGroup[cConfigOffset]->cCanFilterIDGroupNum	= ( byte )u32VciConfigTemp[15];

	for( i = 0; i < g_p_stVciParamsGroup[cConfigOffset]->cCanFilterIDGroupNum; i++ )
	{
		unnTemp.u32Bit = u32VciConfigTemp[16 + i];
		g_p_stVciParamsGroup[cConfigOffset]->cCanFilterID[4 * i + 0]		= unnTemp.u8Bit[3];
		g_p_stVciParamsGroup[cConfigOffset]->cCanFilterID[4 * i + 1]		= unnTemp.u8Bit[2];
		g_p_stVciParamsGroup[cConfigOffset]->cCanFilterID[4 * i + 2]		= unnTemp.u8Bit[1];
		g_p_stVciParamsGroup[cConfigOffset]->cCanFilterID[4 * i + 3]		= unnTemp.u8Bit[0];
	}


}
/*************************************************
Description:	�ͷŴ��VCI���õĿռ�
Input:	��
Output:	����
Return:	��
Others: ÿ���һ�����þ��ڸú����������Ӧ���ͷŴ���
����quit_system_lib.c��free_xml_config_space
�����е��øú�����
*************************************************/
void free_vci_config_space( void )
{
	int i = 0;

	for( i = 0; i < sizeof( g_p_stVciParamsGroup ) / sizeof( g_p_stVciParamsGroup[0] ); i++ )
	{
		if( NULL != g_p_stVciParamsGroup[i] )
			free( g_p_stVciParamsGroup[i] );

	}

}