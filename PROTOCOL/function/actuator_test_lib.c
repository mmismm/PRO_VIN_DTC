/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	定义执行器测试处理函数
History:
	<author>	<time>		<desc>
************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*#define NDEBUG*/
#include <assert.h>

#include "actuator_test_lib.h"
#include "../public/public.h"
#include "../command/command.h"
#include "../formula/formula_comply.h"
#include "../SpecialFunction/special_function.h"
#include "../InitConfigFromXml/init_config_from_xml_lib.h"

STRUCT_SELECT_FUN stActuatorTestFunGroup[] =
{
	{GENERAL_ACTUATOR_TEST, process_general_actuator_test},
};

/*************************************************
Description:	获取处理读取数据流函数
Input:
	cType		配置类型
Output:	保留
Return:	pf_general_function 函数指针
Others:
*************************************************/
pf_general_function get_actuator_test_fun( byte cType )
{
	int i = 0;

	for( i = 0; i < sizeof( stActuatorTestFunGroup ) / sizeof( stActuatorTestFunGroup[0] ); i++ )
		if( cType == stActuatorTestFunGroup[i].cType )
			return stActuatorTestFunGroup[i].pFun;

	return 0;
}

/*************************************************
Description:	执行器测试处理函数
Input:
	pIn		输入从xml获取的执行器测试配置命令
Output:	pOut	输出数据地址
Return:	void
Others:	从UI获取的输入数据是字符串型
*************************************************/
void process_actuator_test( void* pIn, void* pOut )
{
	pf_general_function pFun = NULL;

	pFun = get_actuator_test_fun( g_p_stProcessFunConfig->cActuatorTestFunOffset );

	assert( pFun );

	pFun( pIn, pOut );
}
/*************************************************
Description:	执行器测试处理函数
Input:
	pIn		输入从xml获取的执行器测试配置命令
Output:	pOut	输出数据地址
Return:	void
Others:	从UI获取的输入数据是字符串型
*************************************************/
void process_general_actuator_test( void* pIn, void* pOut )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	int iReceiveResult = TIME_OUT;

	byte cBufferOffset = 0;//缓存偏移
	bool bProcessStatus = false;
	byte *pTempOut = pOut;
	int iInputValue = 0;
	UNN_2WORD_4BYTE uuValueTemp;
	byte cValueCache[4] = {0};
	uint32 u32ActuatorTestCmdData[50] = {0};//执行器测试命令数据
	int i = 0;
	int * piCmdIndex = NULL;
	int iCmdSum = 0;

	byte cActuatorTestMode		= 0xff;
	byte cEndian				= 0;
	byte cProcessBytes			= 0;
	byte cSaveDataOffset		= 0;

	int iGetPIDCmdOffset = 0;
	int iSavePIDCmdOffset = 0;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	get_string_type_data_to_uint32( u32ActuatorTestCmdData, pstParam->pcData, pstParam->iLen );

	cActuatorTestMode = ( byte )u32ActuatorTestCmdData[0];

	switch( cActuatorTestMode )
	{
	case 0x00://直接执行型
		if( u32ActuatorTestCmdData[1] != 0 )
		{
			bProcessStatus = process_single_cmd_without_subsequent_processing( g_iExtendSessionCmdIndex[1], pOut );

			if( !bProcessStatus )
			{
				*( byte* )pOut = 1; //置功能执行成功，保证退出成功

				return;
			}

			bProcessStatus = process_security_access_algorithm( 0, pOut );

			if( !bProcessStatus )
			{
				*( byte* )pOut = 1; //置功能执行成功，保证退出成功

				return;
			}
		}

		pstParam = pstParam->pNextNode;//获取命令偏移

		assert( pstParam->pcData );
		assert( pstParam->iLen != 0 );

		iCmdSum = get_string_type_data_to_uint32( u32ActuatorTestCmdData, pstParam->pcData, pstParam->iLen );

		piCmdIndex = ( int * )malloc( sizeof( int ) * ( iCmdSum + 1 ) );

		piCmdIndex[0] = iCmdSum;

		for( i = 0; i < iCmdSum; i++ )
		{
			piCmdIndex[i + 1] = ( int )u32ActuatorTestCmdData[i];
		}

		cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[ piCmdIndex[1] ].cBufferOffset;//当发送多条命令时需把所有命令缓存号统一

		iReceiveResult = send_and_receive_cmd( piCmdIndex );

		free( piCmdIndex );

		if( iReceiveResult != SUCCESS )
		{
			general_return_status( iReceiveResult, g_stBufferGroup[cBufferOffset].cBuffer, 3, pOut );

			return;
		}

		break;

	case 0x01://带输入型
		pstParam = pstParam->pNextNode;//获取命令偏移

		assert( pstParam->pcData );
		assert( pstParam->iLen != 0 );

		piCmdIndex = ( int * )malloc( sizeof( int ) * 2 );

		piCmdIndex[0] = 1;
		piCmdIndex[1] = atoi( pstParam->pcData );

		pstParam = pstParam->pNextNode;//获取输入内容

		if( ( pstParam == NULL ) || ( pstParam->iLen == 0 ) )
		{
			general_return_status( INPUT_CHARACTER_NUMBER_ERROR, NULL, 0, pOut ); //提示无效输入

			free( piCmdIndex );

			return;
		}

		assert( pstParam->pcData );
		assert( pstParam->iLen != 0 );

		iInputValue = atoi( pstParam->pcData );


		//判断输入数据的范围
		switch(u32ActuatorTestCmdData[9])
		{
		case 0:
			for( i = 0; i < pstParam->iLen; ++i )
			{
				if(( pstParam->pcData[i] >= '0' && pstParam->pcData[i] <= '9' )||(pstParam->pcData[0] == '-'))
				{
				}
				else
				{
					general_return_status( INPUT_CHARACTER_FORMAT_ERROR, NULL, 0, pOut );

					free( piCmdIndex );

					return;
				}
			}
			
			//判断最小值最大值
			if ((iInputValue < ( int )u32ActuatorTestCmdData[10])||(iInputValue > ( int )u32ActuatorTestCmdData[11]))
			{
				general_return_status( INPUT_CHARACTER_FORMAT_ERROR, NULL, 0, pOut );
			
				free( piCmdIndex );

				return;
			}
			break;
		case 1:
			break;
		default:
			break;
		}

		iInputValue *= ( int )u32ActuatorTestCmdData[3];
		iInputValue /= ( int )u32ActuatorTestCmdData[4];
		iInputValue += ( int )u32ActuatorTestCmdData[1];
		iInputValue -= ( int )u32ActuatorTestCmdData[2];

		cEndian			= u32ActuatorTestCmdData[5];
		cProcessBytes	= u32ActuatorTestCmdData[6];
		cSaveDataOffset = u32ActuatorTestCmdData[7];

		uuValueTemp.u32Bit = ( uint32 )iInputValue;

		memcpy( cValueCache, uuValueTemp.u8Bit, 4 );

		if( ( cEndian == 1 ) && ( cProcessBytes > 1 ) ) //如果是大端且字节数大于1
		{
			cValueCache[0] = uuValueTemp.u8Bit[3];
			cValueCache[1] = uuValueTemp.u8Bit[2];
			cValueCache[2] = uuValueTemp.u8Bit[1];
			cValueCache[3] = uuValueTemp.u8Bit[0];

			memcpy( ( g_stInitXmlGobalVariable.m_p_stCmdList[ piCmdIndex[1] ].pcCmd + cSaveDataOffset )
			        , &cValueCache[4 - cProcessBytes], cProcessBytes );
		}
		else
		{
			memcpy( ( g_stInitXmlGobalVariable.m_p_stCmdList[ piCmdIndex[1] ].pcCmd + cSaveDataOffset )
			        , &cValueCache[0], cProcessBytes );
		}

		if( u32ActuatorTestCmdData[8] != 0 ) //判断有没有安全进入
		{
			bProcessStatus = process_single_cmd_without_subsequent_processing( g_iExtendSessionCmdIndex[1], pOut );

			if( !bProcessStatus )
			{
				*( byte* )pOut = 1; //置功能执行成功，保证退出成功

				free( piCmdIndex );

				return;
			}

			bProcessStatus = process_security_access_algorithm( 0, pOut );

			if( !bProcessStatus )
			{
				*( byte* )pOut = 1; //置功能执行成功，保证退出成功

				free( piCmdIndex );

				return;
			}

		}

		cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[ piCmdIndex[1] ].cBufferOffset;//

		iReceiveResult = send_and_receive_cmd( piCmdIndex );

		free( piCmdIndex );

		if( iReceiveResult != SUCCESS )
		{
			general_return_status( iReceiveResult, g_stBufferGroup[cBufferOffset].cBuffer, 3, pOut );

			return;
		}

		break;

	case 0x02://执行器测试退出，需取PID
		if( u32ActuatorTestCmdData[1] != 0x00 ) //如果有退 标志不为0x00
		{
			pstParam = pstParam->pNextNode;

			assert( pstParam->pcData );
			assert( pstParam->iLen != 0 );

			iGetPIDCmdOffset = atoi( pstParam->pcData );//获取 获取PID的命令偏移

			pstParam = pstParam->pNextNode;

			assert( pstParam->pcData );
			assert( pstParam->iLen != 0 );

			iSavePIDCmdOffset = atoi( pstParam->pcData );//获取 保存PID的命令偏移，即要发送的命令

			memcpy(
			    g_stInitXmlGobalVariable.m_p_stCmdList[ iSavePIDCmdOffset].pcCmd + u32ActuatorTestCmdData[3],
			    g_stInitXmlGobalVariable.m_p_stCmdList[ iGetPIDCmdOffset ].pcCmd + u32ActuatorTestCmdData[2],
			    u32ActuatorTestCmdData[4]
			);

			bProcessStatus = process_single_cmd_without_subsequent_processing( iSavePIDCmdOffset, pOut );

			if( !bProcessStatus )
			{
				process_single_cmd_without_subsequent_processing( g_iDefaultSessionCmdIndex[1], pOut );
				*( byte* )pOut = 1; //置功能执行成功，保证退出成功

				return;
			}

		}

		bProcessStatus = process_single_cmd_without_subsequent_processing( g_iDefaultSessionCmdIndex[1], pOut );
		*( byte* )pOut = 1; //置功能执行成功，保证退出成功
		pTempOut[2] = 0; //不提示
		return;

		break;

	default:
		break;
	}

	//提示成功
	special_return_status( PROCESS_OK | HAVE_JUMP | NO_TIP, "test_success", NULL, NULL, 0, 0, NULL, pOut );
}

