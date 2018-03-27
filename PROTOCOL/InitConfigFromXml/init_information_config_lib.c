/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	�����xml��ȡ�汾��Ϣ���ô�����
History:
	<author>	<time>		<desc>

***********************************************************************/
#include "..\interface\protocol_define.h"
#include "..\public\protocol_config.h"
#include "init_config_from_xml_lib.h"
#include <assert.h>
#include <stdlib.h>
#include "..\function\ds_lib.h"


/*************************************************
Description:	��ȡ�汾��Ϣ����
Input:
	PIn				������������
Output:	����
Return:	��
Others: ���ݲ�ͬ������ģ�����Ӧ�ķ�֧
*************************************************/
void get_information_config( void* pIn )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	byte cTemp	= 0;
	byte * pcTemp = NULL;

	byte i = 0;
	byte j = 0;

	int iLen = 0;

	byte cGroupSum = 0;
	byte cGroupID = 0;
	byte cItemSum = 0;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	pcTemp = pstParam->pcData;

	iLen = get_command_one_block_config_data( &cTemp, pcTemp );
	cGroupSum = cTemp;//����������
	pcTemp += iLen;

	for( i = 0; i < cGroupSum; i++ )
	{
		iLen = get_command_one_block_config_data( &cTemp, pcTemp );
		cGroupID = cTemp;//��ñ�������
		pcTemp += iLen;

		g_p_stInformationGroupConfigGroup[cGroupID] = ( STRUCT_DS_FORMULA_GROUP_CONFIG * )malloc( sizeof( STRUCT_DS_FORMULA_GROUP_CONFIG ) );
		g_p_stInformationGroupConfigGroup[cGroupID]->pstDSFormulaConfig = NULL;

		iLen = get_command_one_block_config_data( &cTemp, pcTemp );
		cItemSum = cTemp;//��ñ����а汾��Ϣ����
		pcTemp += iLen;

		g_p_stInformationGroupConfigGroup[cGroupID]->cItemSum = cItemSum;
		g_p_stInformationGroupConfigGroup[cGroupID]->pstDSFormulaConfig =
		    ( STRUCT_DS_FORMULA_CONFIG * )calloc( cItemSum, sizeof( STRUCT_DS_FORMULA_CONFIG ) );

		for( j = 0; j < cItemSum; j++ )
		{
			iLen = get_ds_config( ( STRUCT_DS_FORMULA_CONFIG * ) & ( g_p_stInformationGroupConfigGroup[cGroupID]->pstDSFormulaConfig[j] ),
			                      pcTemp );
			pcTemp += iLen;
		}

	}

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
void free_information_config_space( void )
{
	byte i = 0;
	byte j = 0;

	for( i = 0; i < sizeof( g_p_stInformationGroupConfigGroup ) / sizeof( g_p_stInformationGroupConfigGroup[0] ); i++ )
	{
		if( NULL != g_p_stInformationGroupConfigGroup[i] )
		{
			if( NULL != g_p_stInformationGroupConfigGroup[i]->pstDSFormulaConfig )
			{
				for( j = 0; j < g_p_stInformationGroupConfigGroup[i]->cItemSum; j++ )
				{
					if( NULL != g_p_stInformationGroupConfigGroup[i]->pstDSFormulaConfig[j].pcFormula )
						free( g_p_stInformationGroupConfigGroup[i]->pstDSFormulaConfig[j].pcFormula );

					if( NULL != g_p_stInformationGroupConfigGroup[i]->pstDSFormulaConfig[j].pStrFormat )
						free( g_p_stInformationGroupConfigGroup[i]->pstDSFormulaConfig[j].pStrFormat );
				}

				free( g_p_stInformationGroupConfigGroup[i]->pstDSFormulaConfig );
			}

			free( g_p_stInformationGroupConfigGroup[i] );

		}
	}

}