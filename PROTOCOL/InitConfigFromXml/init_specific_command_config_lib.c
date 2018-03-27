/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	�����ȡ�����������ô�����
History:
	<author>	<time>		<desc>

***********************************************************************/
#include "..\interface\protocol_define.h"
#include "..\public\protocol_config.h"
#include "init_config_from_xml_lib.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/*************************************************
Description:	��ȡ����������������
Input:	PIn		������������
Output:	����
Return:	int		��������
Others:
*************************************************/
int get_specific_command_config_data( void* pIn )
{
	free_specific_command_config_space();

	g_stInitXmlGobalVariable.m_iSpecificCmdGetFromXmlSum =
	    get_command_config_data( pIn, &( g_stInitXmlGobalVariable.m_p_stCmdList ) );

	return( g_stInitXmlGobalVariable.m_iSpecificCmdGetFromXmlSum );
}
/*************************************************
Description:	��ȡ������������
Input:	PIn		������������
Output:	ppstCmd	����ṹ��ָ���ָ��
Return:	int		��������
Others:
*************************************************/
int get_command_config_data( void* pIn, STRUCT_CMD ** ppstCmd )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	byte cCmdTemp[256] = {0};
	byte cTemp[15] = {0};
	byte * pcTemp = NULL;
	int iCmdSum = 0;
	int i = 0;
	int iLen = 0;
	uint32 u32CmdLen = 0;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	pcTemp = pstParam->pcData;

	memcpy( cTemp, pcTemp, 4 );
	cTemp[4] = '\0';

	pcTemp += 4;

	iLen = strtol( cTemp, NULL, 16 );

	memcpy( cTemp, pcTemp, iLen );
	cTemp[iLen] = '\0';

	iCmdSum = strtol( cTemp, NULL, 16 ); //��ȡ����������

	if( 0 == iCmdSum )
	{
		return iCmdSum;
	}

	//�����ڴ�
	*ppstCmd = ( STRUCT_CMD * )malloc( sizeof( STRUCT_CMD ) * iCmdSum );

	pcTemp += iLen;

	for( i = 0; i < iCmdSum; i++ )
	{
		iLen = get_command_one_block_config_data( cCmdTemp, pcTemp );
		( *ppstCmd )[i].cBufferOffset = cCmdTemp[0]; //����ƫ��

		pcTemp += iLen;

		iLen = get_command_one_block_config_data( cCmdTemp, pcTemp );
		( *ppstCmd )[i].cReserved = cCmdTemp[0]; //�����ֽ�

		pcTemp += iLen;

		memcpy( cTemp, pcTemp, 4 );
		cTemp[4] = '\0';

		iLen = 4;

		iLen += strtol( cTemp, NULL, 16 );

		get_config_data( &u32CmdLen, pcTemp, iLen );

		( *ppstCmd )[i].iCmdLen = ( int )u32CmdLen; //��ȡ����ĳ���

		pcTemp += iLen;

		iLen = get_command_one_block_config_data( cCmdTemp, pcTemp );

		( *ppstCmd )[i].pcCmd = ( byte* )malloc( sizeof( byte ) * u32CmdLen  + 1 );

		memcpy( ( *ppstCmd )[i].pcCmd, cCmdTemp, u32CmdLen ); //������������

		pcTemp += iLen;
	}

	return iCmdSum;
}



/*************************************************
Description:	�ͷŴ�ž����������õĿռ�
Input:	��
Output:	����
Return:	��
Others: ÿ���һ�����þ��ڸú����������Ӧ���ͷŴ���
����quit_system_lib.c��free_xml_config_space
�����е��øú�����
*************************************************/
void free_specific_command_config_space( void )
{
	int i = 0;

	if( NULL != g_stInitXmlGobalVariable.m_p_stCmdList )
	{
		for( i = 0; i < g_stInitXmlGobalVariable.m_iSpecificCmdGetFromXmlSum; i++ )
		{
			if( NULL != g_stInitXmlGobalVariable.m_p_stCmdList[i].pcCmd )
				free( g_stInitXmlGobalVariable.m_p_stCmdList[i].pcCmd );
		}

		free( g_stInitXmlGobalVariable.m_p_stCmdList );

		g_stInitXmlGobalVariable.m_p_stCmdList = NULL;
	}

	g_stInitXmlGobalVariable.m_iSpecificCmdGetFromXmlSum = 0;
}