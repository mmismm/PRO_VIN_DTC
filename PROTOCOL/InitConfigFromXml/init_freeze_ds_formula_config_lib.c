/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	�����xml��ȡ����֡��������ʽ���ô�����
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
Description:	��ȡ����֡��������ʽ����
Input:
	PIn				������������
Output:	����
Return:	��
Others: ���ݲ�ͬ������ģ�����Ӧ�ķ�֧
*************************************************/
void get_freeze_DS_formula_config( void* pIn )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	byte cTemp	= 0;
	byte * pcTemp = NULL;

	byte j = 0;

	int iLen = 0;

	byte cGroupSum = 0;
	byte cGroupID = 0;
	byte cItemSum = 0;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	pcTemp = pstParam->pcData;

	iLen = get_command_one_block_config_data( &cTemp, pcTemp );
	cGroupSum = cTemp;//�������������ڶ���ֻ֡��һ��
	pcTemp += iLen;

	g_p_stFreezeDSFormulaConfig = ( STRUCT_DS_FORMULA_GROUP_CONFIG * )malloc( sizeof( STRUCT_DS_FORMULA_GROUP_CONFIG ) );
	g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig = NULL;

	iLen = get_command_one_block_config_data( &cTemp, pcTemp );
	cGroupID = cTemp;//��ñ�������
	pcTemp += iLen;

	iLen = get_command_one_block_config_data( &cTemp, pcTemp );
	cItemSum = cTemp;//��ñ���������������
	pcTemp += iLen;

	g_p_stFreezeDSFormulaConfig->cItemSum = cItemSum;

	g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig =
	    ( STRUCT_DS_FORMULA_CONFIG * )calloc( cItemSum, sizeof( STRUCT_DS_FORMULA_CONFIG ) );

	for( j = 0; j < cItemSum; j++ )
	{
		iLen = get_ds_config( ( STRUCT_DS_FORMULA_CONFIG * ) & ( g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[j] ), pcTemp );
		pcTemp += iLen;
	}

}

/*************************************************
Description:	�ͷŴ�Ŷ���֡��������ʽ���õĿռ�
Input:	��
Output:	����
Return:	��
Others: ÿ���һ�����þ��ڸú����������Ӧ���ͷŴ���
		����quit_system_lib.c��free_xml_config_space
		�����е��øú�����
*************************************************/
void free_freeze_DS_formula_config_space( void )
{
	byte j = 0;

	if( NULL != g_p_stFreezeDSFormulaConfig )
	{
		if( NULL != g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig )
		{
			for( j = 0; j < g_p_stFreezeDSFormulaConfig->cItemSum; j++ )
			{
				if( NULL != g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[j].pcFormula )
					free( g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[j].pcFormula );

				if( NULL != g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[j].pStrFormat )
					free( g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[j].pStrFormat );
			}

			free( g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig );
		}

		free( g_p_stFreezeDSFormulaConfig );
	}

}