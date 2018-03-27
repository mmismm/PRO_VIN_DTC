/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	�����ȡ���������ô�����
History:
	<author>	<time>		<desc>

***********************************************************************/
#include "..\interface\protocol_define.h"
#include "..\public\protocol_config.h"
#include "init_config_from_xml_lib.h"
#include <assert.h>
#include <stdlib.h>

STRUCT_INIT_CONFIG stInitProcessFunConfigGroup[] =
{
	{GENERAL_PROCESS_FUN_CONFIG, get_general_process_fun_config_data},
};


/*************************************************
Description:	��ȡ��ʼ�����������ú���
Input:
	cConfigType		��������
Output:	����
Return:	pf_parammeter_is_point_to_uint32 ����ָ��
Others:
*************************************************/
pf_parammeter_is_point_to_uint32 get_init_process_fun_config_fun( byte cConfigType )
{
	int i = 0;

	for( i = 0; i < sizeof( stInitProcessFunConfigGroup ) / sizeof( stInitProcessFunConfigGroup[0] ); i++ )
		if( cConfigType == stInitProcessFunConfigGroup[i].cConfigType )
			return stInitProcessFunConfigGroup[i].pFun;

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
void get_process_fun_config_data( void* pIn )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	uint32 u32ConfigTemp[50] = {0};

	byte cConfigType = 0;

	pf_parammeter_is_point_to_uint32 pFun = NULL;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	get_config_data( u32ConfigTemp, pstParam->pcData, NO_LENGTH_LIMIT );

	cConfigType	= ( byte )u32ConfigTemp[1];

	pFun = get_init_process_fun_config_fun( cConfigType );

	assert( pFun );

	pFun( u32ConfigTemp );

}
/*************************************************
Description:	��ȡͨ�ô�������������
Input:
	pu32Source		������������
Output:	����
Return:	��
Others: �����Ὺ�ٿռ�����������
*************************************************/
void get_general_process_fun_config_data( const uint32 * pu32Source )
{
	byte cConfigOffset = 0;

	cConfigOffset = ( byte )pu32Source[0];

	if( g_p_stProcessFunConfigGroup[cConfigOffset] == NULL )
		g_p_stProcessFunConfigGroup[cConfigOffset] = ( STRUCT_PROCESS_FUN_CONFIG * )malloc( sizeof( STRUCT_PROCESS_FUN_CONFIG ) );

	g_p_stProcessFunConfigGroup[cConfigOffset]->cActiveECUFunOffset		= ( byte )pu32Source[2];
	g_p_stProcessFunConfigGroup[cConfigOffset]->cActuatorTestFunOffset	= ( byte )pu32Source[3];
	g_p_stProcessFunConfigGroup[cConfigOffset]->cClearDTCFunOffset		= ( byte )pu32Source[4];
	g_p_stProcessFunConfigGroup[cConfigOffset]->cDSFunOffset			= ( byte )pu32Source[5];
	g_p_stProcessFunConfigGroup[cConfigOffset]->cCurrentDTCFunOffset	= ( byte )pu32Source[6];
	g_p_stProcessFunConfigGroup[cConfigOffset]->cHistoryDTCFunOffset	= ( byte )pu32Source[7];
	g_p_stProcessFunConfigGroup[cConfigOffset]->cFreezeDTCFunOffset		= ( byte )pu32Source[8];
	g_p_stProcessFunConfigGroup[cConfigOffset]->cFreezeDSFunOffset		= ( byte )pu32Source[9];
	g_p_stProcessFunConfigGroup[cConfigOffset]->cInforFunOffset			= ( byte )pu32Source[10];
	g_p_stProcessFunConfigGroup[cConfigOffset]->cQuitSystemFunOffset	= ( byte )pu32Source[11];
}

/*************************************************
Description:	ѡ����������
Input:
	iConfigOffset		����ƫ��
	cConfigType			��������
Output:	����
Return:	��
Others: ���ݼ����������ͺ�����ƫ��ѡ����Ӧ������
*************************************************/
void select_process_fun_config( int iConfigOffset, const byte cConfigType )
{
	switch( cConfigType )
	{
	case GENERAL_ACTIVE_ECU_CONFIG:
		g_p_stProcessFunConfig = g_p_stProcessFunConfigGroup[iConfigOffset];
		break;

	default:
		break;
	}
}

/*************************************************
Description:	�ͷŴ�Ŵ��������õĿռ�
Input:	��
Output:	����
Return:	��
Others: ÿ���һ�����þ��ڸú����������Ӧ���ͷŴ���
����quit_system_lib.c��free_xml_config_space
�����е��øú�����
*************************************************/
void free_process_fun_config_space( void )
{
	int i = 0;

	for( i = 0; i < sizeof( g_p_stProcessFunConfigGroup ) / sizeof( g_p_stProcessFunConfigGroup[0] ); i++ )
	{
		if( NULL != g_p_stProcessFunConfigGroup[i] )
			free( g_p_stProcessFunConfigGroup[i] );
	}

}
