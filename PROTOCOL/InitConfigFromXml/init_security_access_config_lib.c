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
#include "..\command\command.h"

STRUCT_INIT_CONFIG stInitSecurityAccessConfigGroup[] =
{
	{GENERAL_SECURITY_ACCESS_CONFIG, get_general_security_access_config_data},
};


/*************************************************
Description:	��ȡ��ʼ����ȫ�������ú���
Input:
	cConfigType		��������
Output:	����
Return:	pf_parammeter_is_point_to_uint32 ����ָ��
Others:
*************************************************/
pf_parammeter_is_point_to_uint32 get_init_security_access_config_fun( byte cConfigType )
{
	int i = 0;

	for( i = 0; i < sizeof( stInitSecurityAccessConfigGroup ) / sizeof( stInitSecurityAccessConfigGroup[0] ); i++ )
		if( cConfigType == stInitSecurityAccessConfigGroup[i].cConfigType )
			return stInitSecurityAccessConfigGroup[i].pFun;

	return 0;//���ڲ����
}

/*************************************************
Description:	��ȡ��ȫ��������
Input:
	PIn				������������
Output:	����
Return:	��
Others: ���ݲ�ͬ������ģ�����Ӧ�ķ�֧
*************************************************/
void get_security_access_config( void* pIn )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	uint32 u32ConfigTemp[50] = {0};

	byte cConfigType = 0;

	pf_parammeter_is_point_to_uint32 pFun = NULL;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	get_config_data( u32ConfigTemp, pstParam->pcData, NO_LENGTH_LIMIT );

	cConfigType	= ( byte )u32ConfigTemp[1];

	pFun = get_init_security_access_config_fun( cConfigType );

	assert( pFun );

	pFun( u32ConfigTemp );

}
/*************************************************
Description:	��ȡͨ�ð�ȫ������������
Input:
	pu32Source		������������
Output:	����
Return:	��
Others: �����Ὺ�ٿռ�����������
*************************************************/
void get_general_security_access_config_data( const uint32 * pu32Source )
{
	byte cConfigOffset = 0;

	cConfigOffset = ( byte )pu32Source[0];

	if( g_p_stSecurityAccessConfigGroup[cConfigOffset] == NULL )
		g_p_stSecurityAccessConfigGroup[cConfigOffset] = ( STRUCT_SECURITY_ACCSEE_CONFIG * )malloc( sizeof( STRUCT_SECURITY_ACCSEE_CONFIG ) );

	g_p_stSecurityAccessConfigGroup[cConfigOffset]->iDefaultSessionCmdID	= ( int )pu32Source[2];
	g_p_stSecurityAccessConfigGroup[cConfigOffset]->iExtendSessionCmdID		= ( int )pu32Source[3];
	g_p_stSecurityAccessConfigGroup[cConfigOffset]->iRequestSeedCmdID		= ( int )pu32Source[4];
	g_p_stSecurityAccessConfigGroup[cConfigOffset]->iSendKeyCmdID			= ( int )pu32Source[5];

}

/*************************************************
Description:	ѡ��ȫ��������
Input:
	iConfigOffset		����ƫ��
	cConfigType			��������
Output:	����
Return:	��
Others: �����������ͺ�����ƫ��ѡ����Ӧ�����ò�����
		��ʼ��
*************************************************/
void init_security_access_config( int iConfigOffset, const byte cConfigType )
{
	switch( cConfigType )
	{
	case GENERAL_SECURITY_ACCESS_CONFIG:
	{
		g_p_stSecurityAccessConfig = g_p_stSecurityAccessConfigGroup[iConfigOffset];

		g_iDefaultSessionCmdIndex[1]	= g_p_stSecurityAccessConfig->iDefaultSessionCmdID;
		g_iExtendSessionCmdIndex[1]		= g_p_stSecurityAccessConfig->iExtendSessionCmdID;
		g_iRequestSeedCmdIndex[1]		= g_p_stSecurityAccessConfig->iRequestSeedCmdID;
		g_iSendKeyCmdIndex[1]			= g_p_stSecurityAccessConfig->iSendKeyCmdID;
	}
	break;

	default:
		break;
	}
}

/*************************************************
Description:	�ͷŴ�Ű�ȫ�������õĿռ�
Input:	��
Output:	����
Return:	��
Others: ÿ���һ�����þ��ڸú����������Ӧ���ͷŴ���
����quit_system_lib.c��free_xml_config_space
�����е��øú�����
*************************************************/
void free_security_access_config_space( void )
{
	int i = 0;

	for( i = 0; i < sizeof( g_p_stSecurityAccessConfigGroup ) / sizeof( g_p_stSecurityAccessConfigGroup[0] ); i++ )
	{
		if( NULL != g_p_stSecurityAccessConfigGroup[i] )
			free( g_p_stSecurityAccessConfigGroup[i] );
	}

}
