/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	�����ȡЭ�����ô�����
History:
	<author>	<time>		<desc>

***********************************************************************/
#include "..\interface\protocol_define.h"
#include "..\public\protocol_config.h"
#include "init_config_from_xml_lib.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

STRUCT_INIT_CONFIG stInitProtocolConfigGroup[] =
{
	{ISO15765_CONFIG, get_ISO15765_config_data },
	{ISO14230_CONFIG, get_ISO14230_config_data },
};

/*************************************************
Description:	��ȡ��ʼ��Э�����ú���
Input:
	cConfigType		��������
Output:	����
Return:	pf_parammeter_is_point_to_uint32 ����ָ��
Others:
*************************************************/
pf_parammeter_is_point_to_uint32 get_init_protocol_config_fun( byte cConfigType )
{
	int i = 0;

	for( i = 0; i < sizeof( stInitProtocolConfigGroup ) / sizeof( stInitProtocolConfigGroup[0] ); i++ )
		if( cConfigType == stInitProtocolConfigGroup[i].cConfigType )
			return stInitProtocolConfigGroup[i].pFun;

	return 0;//���ڲ����
}
/*************************************************
Description:	��ȡЭ������
Input:
	PIn				������������
Output:	����
Return:	��
Others: ���ݲ�ͬ������ģ�����Ӧ�ķ�֧
*************************************************/

void get_protocol_config_data( void* pIn )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	uint32 u32ConfigTemp[50] = {0};

	byte cProtocolConfigType = 0;
	pf_parammeter_is_point_to_uint32 pFun = NULL;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	get_config_data( u32ConfigTemp, pstParam->pcData, NO_LENGTH_LIMIT );

	cProtocolConfigType	= ( byte )u32ConfigTemp[1];

	pFun = get_init_protocol_config_fun( cProtocolConfigType );

	assert( pFun );

	pFun( u32ConfigTemp );

}

/*************************************************
Description:	��ȡISO15765��������
Input:
	pu32Source		������������
Output:	����
Return:	��
Others: �����Ὺ�ٿռ�����������
*************************************************/
void get_ISO15765_config_data( const uint32 * pu32Source )
{
	byte cConfigOffset = 0;
	UNN_2WORD_4BYTE unnTemp;
	unnTemp.u32Bit = 0;

	cConfigOffset = ( byte )pu32Source[0];

	if( g_p_stISO15765ConfigGroup[cConfigOffset] == NULL )
		g_p_stISO15765ConfigGroup[cConfigOffset] = ( STRUCT_ISO15765_CONFIG * )malloc( sizeof( STRUCT_ISO15765_CONFIG ) );

	g_p_stISO15765ConfigGroup[cConfigOffset]->u16Code7F78Timeout	= ( uint16 )pu32Source[2];
	g_p_stISO15765ConfigGroup[cConfigOffset]->u16ECUResTimeout		= ( uint16 )pu32Source[3];
	g_p_stISO15765ConfigGroup[cConfigOffset]->u16TimeBetweenFrames	= ( uint16 )pu32Source[4];
	g_p_stISO15765ConfigGroup[cConfigOffset]->cRetransTime			= ( byte )pu32Source[5];
	g_p_stISO15765ConfigGroup[cConfigOffset]->cReserved				= ( byte )pu32Source[6];
	g_p_stISO15765ConfigGroup[cConfigOffset]->cMultiframestime	    = ( byte )pu32Source[7];

}

/*************************************************
Description:	��ȡISO14230��������
Input:
	pu32Source		������������
Output:	����
Return:	��
Others: �����Ὺ�ٿռ�����������
*************************************************/
void get_ISO14230_config_data( const uint32 * pu32Source )
{
	byte cConfigOffset = 0;
	UNN_2WORD_4BYTE unnTemp;
	unnTemp.u32Bit = 0;

	cConfigOffset = ( byte )pu32Source[0];

	if( g_p_stISO14230ConfigGroup[cConfigOffset] == NULL )
		g_p_stISO14230ConfigGroup[cConfigOffset] = ( STRUCT_ISO14230_CONFIG * )malloc( sizeof( STRUCT_ISO14230_CONFIG ) );

	g_p_stISO14230ConfigGroup[cConfigOffset]->u16Code7F78Timeout	= ( uint16 )pu32Source[2];
	g_p_stISO14230ConfigGroup[cConfigOffset]->u16ECUResTimeout		= ( uint16 )pu32Source[3];
	g_p_stISO14230ConfigGroup[cConfigOffset]->u16TimeBetweenFrames	= ( uint16 )pu32Source[4];
	g_p_stISO14230ConfigGroup[cConfigOffset]->cRetransTime			= ( byte )pu32Source[5];
	g_p_stISO14230ConfigGroup[cConfigOffset]->cTimeBetweenBytes		= ( byte )pu32Source[6];
}


/*************************************************
Description:	ѡ��Э������
Input:
	iProtocolConfigOffset	����ƫ��
	cProtocolConfigType		Э����������
Output:	����
Return:	��
Others: ����Э���������ͺ�����ƫ��ѡ����Ӧ������
*************************************************/
void select_protocol_config( int iProtocolConfigOffset, const byte cProtocolConfigType )
{
	switch( cProtocolConfigType )
	{
	case ISO15765_CONFIG:
		g_p_stISO15765Config = g_p_stISO15765ConfigGroup[iProtocolConfigOffset];
		break;

	case ISO14230_CONFIG:
		g_p_stISO14230Config = g_p_stISO14230ConfigGroup[iProtocolConfigOffset];
		break;

	default:
		break;
	}
}

/*************************************************
Description:	�ͷŴ��Э�����õĿռ�
Input:	��
Output:	����
Return:	��
Others: ÿ���һ�����þ��ڸú����������Ӧ���ͷŴ���
����quit_system_lib.c��free_xml_config_space
�����е��øú�����
*************************************************/
void free_protocol_config_space( void )
{
	int i = 0;

	for( i = 0; i < sizeof( g_p_stISO15765ConfigGroup ) / sizeof( g_p_stISO15765ConfigGroup[0] ); i++ )
	{
		if( NULL != g_p_stISO15765ConfigGroup[i] )
			free( g_p_stISO15765ConfigGroup[i] );

	}

	for( i = 0; i < sizeof( g_p_stISO14230ConfigGroup ) / sizeof( g_p_stISO14230ConfigGroup[0] ); i++ )
	{
		if( NULL != g_p_stISO14230ConfigGroup[i] )
			free( g_p_stISO14230ConfigGroup[i] );

	}
}