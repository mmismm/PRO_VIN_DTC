/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	�����xml��ȡ���������ô�����
History:
	<author>	<time>		<desc>

***********************************************************************/
#include "..\interface\protocol_define.h"
#include "..\public\protocol_config.h"
#include "init_config_from_xml_lib.h"
#include <assert.h>
#include <stdlib.h>

STRUCT_INIT_DTC_CONFIG stInitDTCConfigGroup[] =
{
	{GENERAL_DTC_CONFIG, get_general_dtc_config_data},
};

/*************************************************
Description:	��ȡ���������ú���
Input:
	cConfigType		��������
Output:	����
Return:	pf_parammeter_is_point_to_byte ����ָ��
Others:
*************************************************/
pf_parammeter_is_point_to_byte get_init_DTC_config_fun( byte cConfigType )
{
	int i = 0;

	for( i = 0; i < sizeof( stInitDTCConfigGroup ) / sizeof( stInitDTCConfigGroup[0] ); i++ )
		if( cConfigType == stInitDTCConfigGroup[i].cDTCConfigType )
			return stInitDTCConfigGroup[i].pFun;

	return 0;//���ڲ����
}

/*************************************************
Description:	��ȡ����������
Input:
	PIn				������������
Output:	����
Return:	��
Others: ���ݲ�ͬ������ģ�����Ӧ�ķ�֧
*************************************************/
void get_dtc_config_data( void* pIn )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	byte cConfigTemp[50] = {0};

	byte cDtcConfigType = 0;
	pf_parammeter_is_point_to_byte pFun = NULL;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	get_cmd_config_content_data( cConfigTemp, pstParam->pcData, NO_LENGTH_LIMIT );

	cDtcConfigType	= cConfigTemp[1];

	pFun = get_init_DTC_config_fun( cDtcConfigType );

	assert( pFun );

	pFun( cConfigTemp );

}

/*************************************************
Description:	��ȡ����������
Input:
	PIn				������������
Output:	����
Return:	��
Others:
*************************************************/
void get_general_dtc_config_data( const byte * pcSource )
{
	byte cConfigOffset = 0;

	cConfigOffset = pcSource[0];

	if( g_p_stGeneralReadDtcConfigGroup[cConfigOffset] == NULL )
		g_p_stGeneralReadDtcConfigGroup[cConfigOffset] = ( STRUCT_READ_DTC_CONFIG * )malloc( sizeof( STRUCT_READ_DTC_CONFIG ) );

	g_p_stGeneralReadDtcConfigGroup[cConfigOffset]->cDtcStartOffset		= pcSource[2];
	g_p_stGeneralReadDtcConfigGroup[cConfigOffset]->cDtcBytesInCmd		= pcSource[3];
	g_p_stGeneralReadDtcConfigGroup[cConfigOffset]->cStatusOffset		= pcSource[4];
	g_p_stGeneralReadDtcConfigGroup[cConfigOffset]->cDtcBytesInDisplay	= pcSource[5];
	g_p_stGeneralReadDtcConfigGroup[cConfigOffset]->cCurrentDtcMask		= pcSource[6];
	g_p_stGeneralReadDtcConfigGroup[cConfigOffset]->cHistoryDtcMask		= pcSource[7];
}

/*************************************************
Description:	ѡ�����������
Input:
	iConfigOffset		����ƫ��
	cConfigType			��������������
Output:	����
Return:	��
Others: ���ݼ����������ͺ�����ƫ��ѡ����Ӧ������
*************************************************/
void select_dtc_config( int iConfigOffset, const byte cConfigType )
{
	switch( cConfigType )
	{
	case GENERAL_DTC_CONFIG:
		g_p_stGeneralReadDtcConfig = g_p_stGeneralReadDtcConfigGroup[iConfigOffset];
		break;

	default:
		break;
	}
}

/*************************************************
Description:	�ͷŴ�Ŷ������õĿռ�
Input:	��
Output:	����
Return:	��
Others: ÿ���һ�����þ��ڸú����������Ӧ���ͷŴ���
����quit_system_lib.c��free_xml_config_space
�����е��øú�����
*************************************************/
void free_read_dtc_config_space( void )
{
	int i = 0;

	for( i = 0; i < sizeof( g_p_stGeneralReadDtcConfigGroup ) / sizeof( g_p_stGeneralReadDtcConfigGroup[0] ); i++ )
	{
		if( NULL != g_p_stGeneralReadDtcConfigGroup[i] )
			free( g_p_stGeneralReadDtcConfigGroup[i] );
	}

}