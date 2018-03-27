/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	定义退出系统处理函数
History:


***********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "quit_system_lib.h"
#include "..\command\command.h"
#include "..\public\public.h"
#include "..\interface\protocol_define.h"
#include "..\InitConfigFromXml\init_config_from_xml_lib.h"
#include "..\public\protocol_config.h"
#include <assert.h>
#include "..\SpecialFunction\special_function.h"
#include "idle_link_lib.h"
#include "ds_lib.h"
#include "active_ecu_lib.h"

STRUCT_SELECT_FUN stQuitSystemFunGroup[] =
{
	{GENERAL_QUIT_SYSTEM, process_general_quit_system},
};

/*************************************************
Description:	获取处理退出系统函数
Input:
	cType		配置类型
Output:	保留
Return:	pf_general_function 函数指针
Others:
*************************************************/
pf_general_function get_quit_system_fun( byte cType )
{
	int i = 0;

	for( i = 0; i < sizeof( stQuitSystemFunGroup ) / sizeof( stQuitSystemFunGroup[0] ); i++ )
		if( cType == stQuitSystemFunGroup[i].cType )
			return stQuitSystemFunGroup[i].pFun;

	return 0;
}

/*************************************************
Description:	退出系统
Input:	pIn		输入参数（保留）
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
void process_quit_system( void* pIn, void* pOut )
{
	pf_general_function pFun = NULL;

	if( g_iActiveECUStatus == ACTIVE_ECU_SUCCESS ) //激活成功后才执行退出动作
	{
		set_idle_link( IDLE_LINK_STOP );
		pFun = get_quit_system_fun( g_p_stProcessFunConfig->cQuitSystemFunOffset );

		assert( pFun );

		pFun( pIn, pOut );
	}

	free_xml_config_space();
}
/*************************************************
Description:	普通退出系统处理
Input:	pIn		输入参数（保留）
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
void process_general_quit_system( void* pIn, void* pOut )
{

	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	byte *pcOutTemp = ( byte * )pOut;
	int iReceiveValidLen = 0;//接收到的有效字节长度

	int * piQuitSysCmdIndex = NULL;
	uint32 u32CmdData[50] = {0};//命令数据
	int iCmdSum = 0;
	int i = 0;
	uint16 ISO15765TimeOut, ISO14230TimeOut;
	byte *pOutTemp = ( byte* )pOut;

	byte cBufferOffset  = 0;

	if( g_p_stISO15765Config != NULL )
	{
		ISO15765TimeOut = g_p_stISO15765Config->u16ECUResTimeout;
		g_p_stISO15765Config->u16ECUResTimeout = 1000;
	}

	if( g_p_stISO14230Config != NULL )
	{
		ISO14230TimeOut = g_p_stISO14230Config->u16ECUResTimeout;
		g_p_stISO14230Config->u16ECUResTimeout = 1000;
	}


// 	assert(pstParam->pcData);
// 	assert(pstParam->iLen != 0);

	iCmdSum = get_string_type_data_to_uint32( u32CmdData, pstParam->pcData, pstParam->iLen );

	if( 0 == iCmdSum )
	{
		general_return_status( SUCCESS, NULL, 0, pcOutTemp );
		return ;
	}

	piQuitSysCmdIndex = ( int * )malloc( sizeof( int ) * ( iCmdSum + 1 ) );

	piQuitSysCmdIndex[0] = iCmdSum;

	for( i = 0; i < iCmdSum; i++ )
	{
		piQuitSysCmdIndex[i + 1] = ( int )u32CmdData[i];
	}

	send_and_receive_cmd( piQuitSysCmdIndex );

	general_return_status( SUCCESS, NULL, 0, pOut );

	if( g_p_stISO15765Config != NULL )
	{
		g_p_stISO15765Config->u16ECUResTimeout = ISO15765TimeOut;
	}

	if( g_p_stISO14230Config != NULL )
	{
		g_p_stISO14230Config->u16ECUResTimeout = ISO14230TimeOut;
	}


	free( piQuitSysCmdIndex );
}

/*************************************************
Description:	释放保存从xml获取配置的空间
Input:	无
Output:	无
Return:	保留
Others:
*************************************************/
void free_xml_config_space( void )
{
	free_vci_config_space();
	free_active_ecu_config_space();
	free_protocol_config_space();
	free_specific_command_config_space();
	free_read_dtc_config_space();
	free_freeze_dtc_config_space();
	free_freeze_ds_config_space();
	free_process_fun_config_space();
	free_security_access_config_space();
	free_information_config_space();
	free_freeze_DS_formula_config_space();
	free_idle_link_command_config_space();
	free_idle_link_config_space();
	free_general_DS_formula_config_space();
}
