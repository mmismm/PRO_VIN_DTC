/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	定义部分通用的函数
History:
	<author>	<time>		<desc>

************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include "../interface/protocol_define.h"
#include "public.h"
#include "../interface/protocol_interface.h"
#include "protocol_config.h"
#include "../protocol/iso_15765.h"
#include "../protocol/iso_14230.h"
#include "../command/command.h"
#include "../InitConfigFromXml/init_config_from_xml_lib.h"
#include "../SpecialFunction/special_function.h"
/*************************************************
Description:	根据不同协议发送、接收命令
Input:	piCmdIndex		命令索引地址
Output:	none
Return:	int	收发处理时的状态
Others:	根据VCI设置中的通讯模式区分协议类型
*************************************************/
int send_and_receive_cmd( const int* piCmdIndex )
{
	int iReturnStatus = TIME_OUT;

// 	switch( g_stInitXmlGobalVariable.m_cProtocolConfigType ) //这里根据协议配置模板类型进行区分
// 	{
// 	case ISO15765_CONFIG://CAN
// 		iReturnStatus = send_and_receive_cmd_by_iso_15765( piCmdIndex );
// 		break;
// 
// 	case ISO14230_CONFIG://K线
// 		iReturnStatus = send_and_receive_cmd_by_iso_14230( piCmdIndex );
// 		break;
// 
// 	default:
// 		iReturnStatus = PROTOCOL_ERROR;
// 		break;
// 	}

	return iReturnStatus;
}
/*************************************************
Description:	拷贝数据到输出地址
Input:	pstParamOut	存放数据的链表的指针
Output:	pOut		输出地址
Return:	void
Others:
*************************************************/
void copy_data_to_out( void* pOut, const STRUCT_CHAIN_DATA_OUTPUT* pstParamOut )
{
	byte *pOutTemp = ( byte * )pOut;
	byte *pcSaveDataLen = NULL;
	uint16 u16DataLen = 0;
	UNN_2WORD_4BYTE DataLen;

	STRUCT_CHAIN_DATA_NODE *pTemp1 = pstParamOut->pstData;

	pcSaveDataLen = &pOutTemp[3];//保存存放总长的起始地址

	memcpy( pOutTemp, pstParamOut->cKeyByte, 3 );

	pOutTemp += 3;
	u16DataLen += 3;

	pOutTemp += 4;//跳过保存数据长度的4个字节
	u16DataLen += 4;

	*pOutTemp = pstParamOut->stJump.cLenHighByte;
	pOutTemp++;
	*pOutTemp = pstParamOut->stJump.cLenLowByte;
	pOutTemp++;
	u16DataLen += 2;
	memcpy( pOutTemp, pstParamOut->stJump.pcData, ( pstParamOut->stJump.cLenLowByte + pstParamOut->stJump.cLenHighByte ) );
	pOutTemp += pstParamOut->stJump.cLenLowByte + pstParamOut->stJump.cLenHighByte;
	u16DataLen += pstParamOut->stJump.cLenLowByte + pstParamOut->stJump.cLenHighByte;

	*pOutTemp = pstParamOut->stTip.cLenHighByte;
	pOutTemp++;
	*pOutTemp = pstParamOut->stTip.cLenLowByte;
	pOutTemp++;
	u16DataLen += 2;
	memcpy( pOutTemp, pstParamOut->stTip.pcData, ( pstParamOut->stJump.cLenLowByte + pstParamOut->stJump.cLenHighByte ) );
	pOutTemp += pstParamOut->stJump.cLenLowByte + pstParamOut->stJump.cLenHighByte;
	u16DataLen += pstParamOut->stJump.cLenLowByte + pstParamOut->stJump.cLenHighByte;

	while( pTemp1 )
	{
		*pOutTemp = pTemp1->cLenHighByte;
		pOutTemp++;
		*pOutTemp = pTemp1->cLenLowByte;
		pOutTemp++;
		u16DataLen += 2;
		memcpy( pOutTemp, pTemp1->pcData, ( pTemp1->cLenHighByte + pTemp1->cLenLowByte ) );
		pOutTemp += pTemp1->cLenHighByte + pTemp1->cLenLowByte;
		u16DataLen += pTemp1->cLenHighByte + pTemp1->cLenLowByte;

		pTemp1 = pTemp1->pNextNode;
	}

	DataLen.u32Bit = ( uint32 )u16DataLen;

	pcSaveDataLen[0] = DataLen.u8Bit[3];
	pcSaveDataLen[1] = DataLen.u8Bit[2];
	pcSaveDataLen[2] = DataLen.u8Bit[1];
	pcSaveDataLen[3] = DataLen.u8Bit[0];

}
/*************************************************
Description:	释放存储数据链表申请的空间
Input:	pstParamOut	存放数据的链表的指针
Output:	none
Return:	void
Others:
*************************************************/
void free_param_out_data_space( STRUCT_CHAIN_DATA_OUTPUT* pstParamOut )
{
	STRUCT_CHAIN_DATA_NODE *pTemp1 = pstParamOut->pstData;
	STRUCT_CHAIN_DATA_NODE *pTemp2 = NULL;

	while( pTemp1 )
	{
		pTemp2 = pTemp1->pNextNode;

		if( pTemp1->pcData != NULL )
		{
			free( pTemp1->pcData );
		}

		free( pTemp1 );

		pTemp1 = pTemp2;
	}
}
/*************************************************
Description:	加载提示信息字符串
Input:	pcTipID	存放数据的链表的指针
Output:	pOut	输出地址
Return:	int		已装载的数据长度
Others:	预处理为执行失败、无跳步、不需要提示数据
*************************************************/
int general_load_tipID( const byte* pcTipID, void* pOut )
{
	byte *pOutTemp = ( byte* )pOut;
	pOutTemp[0] = 0;
	pOutTemp[1] = 0;
	pOutTemp[2] = 1;

	pOutTemp[3] = 0;
	pOutTemp[4] = 0;
	pOutTemp[5] = 0;
	pOutTemp[6] = 3 + 4 + 2 + 2 + ( byte )strlen( pcTipID );

	pOutTemp[7] = 0;//无跳步
	pOutTemp[8] = 0;//无跳步

	pOutTemp[9] = 0;
	pOutTemp[10] = ( pcTipID == NULL ) ? 0 : ( byte )strlen( pcTipID );

	memcpy( &pOutTemp[11], pcTipID, pOutTemp[10] );

	return( pOutTemp[6] );

}
/*************************************************
Description:	根据信息类型返回提示内容
Input:
	iStatus		提示信息类型
	pcSource	存放提示数据地址
	cAppendLen	附加提示数据长度

Output:	pOut	输出地址
Return:	int		已装载的数据长度
Others:
*************************************************/
void general_return_status( const int iStatus, const byte* pcSource, const byte cAppendLen, void* pOut )
{
	byte *pOutTemp = ( byte* )pOut;
	int i = 0;
	int iLen = 0;

	switch( iStatus )
	{
	case SUCCESS:
	{
		general_load_tipID( g_cSuccess, pOut );
		pOutTemp[0]  = 1;//功能执行成功
	}
	break;

	case TIME_OUT:
	{
		general_load_tipID( g_cTime_out, pOut );
		pOutTemp[0]  = 2;//通讯中断
	}
	break;

	case FRAME_TIME_OUT:
	{
		general_load_tipID( g_cFrame_time_out, pOut );
		pOutTemp[0]  = 2;//通讯中断
	}
	break;

	case PROTOCOL_ERROR:
		general_load_tipID( g_cProtocol_error, pOut );
		break;

	case FAILE:
		general_load_tipID( g_cFaile, pOut );
		break;

	case FORMAT_ERORR:
		general_load_tipID( g_cFormat_error, pOut );
		break;

	case NULL_POINT:
		general_load_tipID( g_cNull_point, pOut );
		break;

	case NO_FREEZE_DS:
		general_load_tipID( g_cNo_freeze_DS, pOut );
		break;

	case NO_FREEZE_DTC:
		general_load_tipID( g_cNo_freeze_DTC, pOut );
		break;

	case NO_CURRENT_DTC:
		general_load_tipID( g_cNo_current_dtc, pOut );
		break;

	case NO_HISTORY_DTC:
		general_load_tipID( g_cNo_history_dtc, pOut );
		break;

	case RECEIVE_FRAME_TYPE_ERROR:
		general_load_tipID( g_cReceive_frame_type_error, pOut );
		break;

	case MESSAGE_TYPE_ERROR:
		general_load_tipID( g_cMessage_type_error, pOut );
		break;

	case INPUT_CHARACTER_FORMAT_ERROR:
		general_load_tipID( g_cInput_character_format_error, pOut );
		break;

	case INPUT_CHARACTER_NUMBER_ERROR:
		general_load_tipID( g_cInput_character_number_error, pOut );
		break;

	case NEGATIVE://这里需要装载消极代码
	{
		iLen = general_load_tipID( g_cNegative, pOut );

		pOutTemp[2] = 2;//提示时需显示数据

		pOutTemp[ iLen ] = 0; //长度高字节
		pOutTemp[ iLen + 1] = cAppendLen * 2; //长度低字节

		pOutTemp[6] += 2 + cAppendLen * 2; //修改长度

		pOutTemp += iLen + 2;

		for( ; i < cAppendLen; i++ )
		{
			/*sprintf_s( &pOutTemp[i * 2], ( cAppendLen * 2 + 1 - i * 2 ), "%02X", pcSource[i] );*/
			sprintf( &pOutTemp[i * 2], "%02X", pcSource[i] );
		}

	}
	break;

	case MATCH_ERROR:
	{
		iLen = general_load_tipID( g_cMatch_error, pOut );

		pOutTemp[2] = 2;//提示时需显示数据

		pOutTemp[ iLen ] = 0; //长度高字节
		pOutTemp[ iLen + 1] = cAppendLen * 2; //长度低字节

		pOutTemp[6] += 2 + cAppendLen * 2; //修改长度

		pOutTemp += iLen + 2;

		for( ; i < cAppendLen; i++ )
		{
			/*sprintf_s( &pOutTemp[i * 2], ( cAppendLen * 2 + 1 - i * 2 ), "%02X", pcSource[i] );*/
			sprintf( &pOutTemp[i * 2], "%02X", pcSource[i] );
		}
	}
	break;

	default:
		break;
	}
}
/*************************************************
Description:	处理通用的ECU回复数据
Input:
	pcSource	回复命令存放地址
	iValidLen	有效长度

Output:	pstOut	输出地址
Return:	void
Others:	这里把ECU积极响应的数据放到输出链表中
*************************************************/
void process_general_response( const byte* pcSource, const int iValidLen, STRUCT_CHAIN_DATA_OUTPUT* pstOut )
{
	int i = 0;
	STRUCT_CHAIN_DATA_NODE* pstOutCache = ( STRUCT_CHAIN_DATA_NODE* )malloc( sizeof( STRUCT_CHAIN_DATA_NODE ) );

	pstOut->cKeyByte[0] = 1;
	pstOut->cKeyByte[1] = 0;
	pstOut->cKeyByte[2] = 0;
	pstOut->stJump.cLenHighByte = 0;
	pstOut->stJump.cLenLowByte = 0;
	pstOut->stTip.cLenHighByte = 0;
	pstOut->stTip.cLenLowByte = 0;

	pstOutCache->cLenHighByte = 0;
	pstOutCache->cLenLowByte = iValidLen * 2;
	pstOutCache->pcData = ( byte* )malloc( ( iValidLen * 2 + 1 ) * sizeof( byte ) );

	for( ; i < iValidLen; i++ )
	{
		/*sprintf_s( &( pstOutCache->pcData[i * 2] ), ( iValidLen * 2 + 1 - i * 2 ), "%02X", pcSource[i] );*/
		sprintf( &( pstOutCache->pcData[i * 2] ), "%02X", pcSource[i] );
	}

	pstOutCache->pNextNode = NULL;

	pstOut->pstData = pstOutCache;//链接到输出链表中
}
/*************************************************
Description:	处理单条命令的收发，没有后续处理
Input:	iCmdOffset	命令偏移
Output:	pOut	输出地址
Return:	bool	返回处理的状态
Others:	返回状态放在输出地址pOut中，
部分默认为有提示
*************************************************/
bool process_single_cmd_without_subsequent_processing( const int iCmdOffset, void* pOut )
{
	int iReceiveResult = TIME_OUT;

	byte cBufferOffset = 0;//缓存偏移

	int iCmdIndex[] = {1, 0};

	iCmdIndex[1] = iCmdOffset;

	cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[iCmdOffset].cBufferOffset;

	iReceiveResult = send_and_receive_cmd( iCmdIndex );

	general_return_status( iReceiveResult, g_stBufferGroup[cBufferOffset].cBuffer, 3, pOut );

	if( iReceiveResult != SUCCESS )
		return false;

	return true;
}
/*************************************************
Description:	计算校验和
Input:
	pcSource	要计算校验和的数据首地址
	iValidLen	要计算校验和数据的长度
Output:
Return:	byte	返回校验和
Others:
*************************************************/
byte calculate_Checksum( const byte* pcSource, const int iValidLen )
{
	byte cCheckNum = 0;
	int i = 0;

	for( ; i < iValidLen; i++ )
	{
		cCheckNum += pcSource[i];
	}

	return cCheckNum;
}

/*************************************************
Description:	打包并发送VCI配置
Input:	none
Output:	none
Return:	bool	发送时的状态
Others:	尝试发送三次若失败输出FAIL
*************************************************/
bool package_and_send_vci_config( void )
{
	byte cCheckNum = 0;
	int i = 0;
	bool bSendStatus = false;
	byte cSendCache[128] = {0}; //发送帧的缓存
	byte cReceiveCache[5] = {0}; //接收缓存

	cSendCache[0] = FRAME_HEAD_SET_ALL;
	cSendCache[1] = 0x00;
	cSendCache[2] = 1 + 2 + 7 + 3 + 3 + 4 + 1 + g_p_stVCI_params_config->cCanFilterIDGroupNum * 4 + 1;
	cSendCache[3] = g_p_stVCI_params_config->cCommunicationType;
	cSendCache[4] = g_p_stVCI_params_config->cVoltage;
	cSendCache[5] = g_p_stVCI_params_config->cLevel;
	cSendCache[6] = g_p_stVCI_params_config->cLogic;
	cSendCache[7] = g_p_stVCI_params_config->cReceivePin;
	cSendCache[8] = g_p_stVCI_params_config->cSendPin;
	cSendCache[9] = g_p_stVCI_params_config->cCANFrameMode;

	memcpy( &cSendCache[10], g_p_stVCI_params_config->cECUBaudRate, 3 );
	memcpy( &cSendCache[13], g_p_stVCI_params_config->cVCIBaudRate, 3 );
	memcpy( &cSendCache[16], g_p_stVCI_params_config->cCanSamplingConfig, 4 );

	cSendCache[20] = g_p_stVCI_params_config->cCanFilterIDGroupNum;

	memcpy( &cSendCache[21], g_p_stVCI_params_config->cCanFilterID, g_p_stVCI_params_config->cCanFilterIDGroupNum * 4 );

	for( ; i < cSendCache[2] - 1; i++ )
	{
		cCheckNum += cSendCache[i];
	}

	cSendCache[ cSendCache[2] - 1] = cCheckNum;

	send_cmd( cSendCache, cSendCache[2] );

	if( ( bool )receive_cmd( cReceiveCache, 5, 3000 ) )
	{
		if( cReceiveCache[3] == 0x00 )
		{
			bSendStatus = true;
		}
	}

	return bSendStatus;

}

/*************************************************
Description:	切换收发管脚配置
Input:	cReceivePin 接收管脚
		cSendPin	发送管脚
Output:	none
Return:	bool	切换时的状态
Others:
*************************************************/
bool switch_channel( byte cReceivePin, byte cSendPin )
{
	g_p_stVCI_params_config->cReceivePin = cReceivePin;
	g_p_stVCI_params_config->cSendPin    = cSendPin;

	return( package_and_send_vci_config() );
}

/*************************************************
Description:	修改通讯波特率
Input:	uuBandRate 要修改的波特率
Output:	none
Return:	bool	修改时的状态
Others:
*************************************************/
bool change_communicate_with_ECU_baudrate( uint32 uuBandRate )
{
	UNN_2WORD_4BYTE uBandRateTemp;

	uBandRateTemp.u32Bit = uuBandRate;

	g_p_stVCI_params_config->cECUBaudRate[0] = uBandRateTemp.u8Bit[2];
	g_p_stVCI_params_config->cECUBaudRate[1] = uBandRateTemp.u8Bit[1];
	g_p_stVCI_params_config->cECUBaudRate[2] = uBandRateTemp.u8Bit[0];

	return( package_and_send_vci_config() );
}

/*************************************************
Description:	打包并发送激活配置
Input:	none
Output:	none
Return:	bool	发送时的状态
Others:	尝试发送三次若失败输出FAIL
*************************************************/
bool package_and_send_active_config( void )
{
	bool bSendStatus = false;
	STRUCT_CMD	stActiveModeTemp;

	byte cActiveTemp[30] = {0};
	byte *pcTemp = cActiveTemp;

	byte cTimeBetweenBytes	= g_p_stGeneralActiveEcuConfig->cTimeBetweenBytes;	//字节与字节间时间
	byte cActiveMode		= g_p_stGeneralActiveEcuConfig->cActiveMode;		//激活方式
	byte cDetectBandRateFlag = g_p_stGeneralActiveEcuConfig->cDetectBandRateFlag;

	stActiveModeTemp.cBufferOffset = 0;
	stActiveModeTemp.cReserved = 0;
	stActiveModeTemp.iCmdLen = 1 + 1 + 3 + 1 + g_p_stGeneralActiveEcuConfig->cActiveAddress[0];

	memcpy( pcTemp, &cActiveMode, 1 ); //装载激活模式

	pcTemp++;

	memcpy( pcTemp, &cDetectBandRateFlag, 1 ); //装载自检标志

	pcTemp++;

	memcpy( pcTemp, g_p_stGeneralActiveEcuConfig->cBandRate, 3 ); //装载波特率

	pcTemp += 3;

	memcpy( pcTemp, &g_p_stGeneralActiveEcuConfig->cActiveAddress[0], g_p_stGeneralActiveEcuConfig->cActiveAddress[0] + 1 ); //装载激活地址总数+地址

	stActiveModeTemp.pcCmd = cActiveTemp;

	//发送激活设置命令
	bSendStatus = package_and_send_frame( FRAME_HEAD_ACTIVE_ECU, ( STRUCT_CMD* )&stActiveModeTemp, cTimeBetweenBytes );

	if( !bSendStatus )
	{
		return false;
	}

	return true;

}

/*************************************************
Description:	修改命令长度空间
Input:	iCmdOffset	待修改的命令偏移
		iNewCmdLen	命令要修改成的长度

Output:	none
Return:	none
Others:	直接修改g_stInitXmlGobalVariable.m_p_stCmdList
		中的内容,若当前命令长度小于设定的长度则改写
		当前命令的长度并重新分配内存，否则不修改、
		不分配。
*************************************************/
void change_cmd_space( const int iCmdOffset, const int iNewCmdLen )
{
	if( g_stInitXmlGobalVariable.m_p_stCmdList[iCmdOffset].iCmdLen < iNewCmdLen )
	{
		if( NULL != g_stInitXmlGobalVariable.m_p_stCmdList[iCmdOffset].pcCmd )
			free( g_stInitXmlGobalVariable.m_p_stCmdList[iCmdOffset].pcCmd );

		g_stInitXmlGobalVariable.m_p_stCmdList[iCmdOffset].pcCmd = ( byte * )malloc( sizeof( byte ) * iNewCmdLen );
	}

	g_stInitXmlGobalVariable.m_p_stCmdList[iCmdOffset].iCmdLen = iNewCmdLen;
}

/*************************************************
Description:	重启VCI
Input:	none
Output:	none
Return:	bool	重启VCI状态
Others:	因为调用package_and_send_frame函数所以实际
发送时会在内容前多一个保留字节的。
*************************************************/
bool reset_VCI( void )
{
	bool bSendStatus = false;
	STRUCT_CMD stSendCmd = {0};
	byte cSetContent[] = "RESET";

	stSendCmd.iCmdLen = sizeof( cSetContent ) - 1;

	stSendCmd.pcCmd = ( byte* )malloc( sizeof( byte ) * stSendCmd.iCmdLen );

	memcpy( stSendCmd.pcCmd, cSetContent, stSendCmd.iCmdLen );

	bSendStatus = package_and_send_frame( FRAME_HEAD_RESET_VCI, ( STRUCT_CMD* )&stSendCmd, 0xaa );

	free( stSendCmd.pcCmd );

	return bSendStatus;
}

/*************************************************
Description:	比较函数
Input:
		a		参数1
		b		参数2
Output:	none
Return:	int 	 数值比较结果
Others:	由qsort函数用
*************************************************/
int compare( const void *a, const void * b )
{
	return *( int * )a - *( int * )b;
}
/*************************************************
Description:	删除数组中重复元素
Input:
		pi		待处理的数组指针
		num		数组中元素个数
Output:	none
Return:	int		处理后有效元素个数
Others:	处理结果直接放在pi指向的数组中
*************************************************/
int remove_duplicate_int_element( int * pi, byte num )
{
	int i = 0;
	int k = 0;

	qsort( pi, num, sizeof( int ), compare );

	for( i = 1; i < num; i++ )
	{
		//这里比较相邻元素即可
		if( pi[i] != pi[k] )
		{
			k += 1;

			pi[k] = pi[i];
		}
	}

	return k + 1;
}

/*************************************************
Description:	判断输入值的状态
Input:
iRangeLen	规定范围字符串长度
pcRange		规定范围字符串指针

格式:  符号位 最小值 符号位 最大值
例如:    1     100      0    100   范围[-100~100]

符号位:  0表示 正数   非0 表示负数

iValueLen	输入字符串长度
pcValue		输入字符串指针
return  int		判断输入值的结果，其值可为：
				INPUT_FORMAT_ERROR
				INPUT_OUT_OF_RANGE
				INPUT_WITHIN_THE_RANGE
Others:	输入值必须是数字,第一位可以是'+'或'-' 号
******************************************************/

int judge_input_value( const int iRangeLen, const byte* pcRange, const int iValueLen, const byte* pcValue )
{
	uint32 u32InputValueRange[4] = {0};
	int iInputValue = 0;
	int i = 0;

	assert( iRangeLen != 0 && pcRange );

	assert( iValueLen != 0 && pcValue );

	for( i = 0; i < iValueLen; i++ )
	{
		if( !isdigit( pcValue[i] ) )
		{
			if( 0 == i )
			{
				if( pcValue[i] == '-' || pcValue[i] == '+' )
					continue;
			}

			return INPUT_FORMAT_ERROR;

		}
	}

	iInputValue = atoi( pcValue );
	//获取参考范围的内容
	get_string_type_data_to_uint32( u32InputValueRange, pcRange, iRangeLen );

	if( 0 == u32InputValueRange[0] ) //0是正 非0是负
	{
		u32InputValueRange[1] = 0 - u32InputValueRange[1] ; //最小值
	}

	if( 0 == u32InputValueRange[2] ) //0是正 非0是负
	{
		u32InputValueRange[3] = 0 - u32InputValueRange[3] ; //最大值
	}

	if( ( iInputValue < ( int )u32InputValueRange[1] ) || ( iInputValue > ( int )u32InputValueRange[3] ) )
	{
		return INPUT_OUT_OF_RANGE;
	}

	return INPUT_WITHIN_THE_RANGE;

}

/*************************************************
Description:	设置FC帧
Input:
Output:
Return:
Others:
*************************************************/

void process_SET_CONFIG_FC_CMD_CAN( void* pIn, void* pOut )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	byte *pcOutTemp = ( byte * )pOut;
	STRUCT_CMD * FcCmdData = NULL;

	bool bSetStatus = false;
	int FcFrameNumber = 0;
	int i = 0;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	FcFrameNumber = get_command_config_data( pstParam, &FcCmdData );

	if( 0 == FcFrameNumber )
	{
		//释放内存
		if( NULL != FcCmdData )
		{
			free( FcCmdData );
			FcCmdData = NULL;
		}

		general_return_status( SUCCESS, NULL, 0, pcOutTemp );
		pcOutTemp[2] = 0;//不提示
		return ;
	}

	bSetStatus = bSend_FC_Frame_Cmd( FcCmdData, u32Config_fuc[0], u32Config_fuc[1], u32Config_fuc[2] );

	if( bSetStatus ) //如果接收成功且设置成功则返回SUCCESS
	{
		general_return_status( SUCCESS, NULL, 0, pcOutTemp );
		pcOutTemp[2] = 0;//不提示
	}
	else
	{
		general_return_status( FAILE, NULL, 0, pcOutTemp );
		pcOutTemp[2] = 0;//不提示
	}

	//释放内存
	if( NULL != FcCmdData )
	{
		if( NULL != FcCmdData->pcCmd )
		{
			free( FcCmdData->pcCmd );
			FcCmdData->pcCmd = NULL;
		}

		free( FcCmdData );
		FcCmdData = NULL;
	}

}



/*************************************************
Description:	发送FC帧
Input:
		FcCmdData		发送命令的地址
		FcFrameTime		30帧发送的延时时间
        WhichByte       获得0x10字节位置
        MultiByte      判断的多帧字节
Output:	bool
Return:
Others:
*************************************************/
bool bSend_FC_Frame_Cmd( const STRUCT_CMD * FcCmdData, const uint32 FcFrameTime, const uint32 WhichByte, const uint32 MultiByte )
{
	STRUCT_CMD  SendFcCmdData = {0};
	bool  bSetStatus = false;
	SendFcCmdData.iCmdLen = FcCmdData->iCmdLen + 2;
	SendFcCmdData.pcCmd = ( byte* )malloc( sizeof( byte ) * SendFcCmdData.iCmdLen );

	SendFcCmdData.pcCmd[0] = ( byte )WhichByte;
	SendFcCmdData.pcCmd[1] = ( byte )MultiByte;
	memcpy( ( SendFcCmdData.pcCmd ) + 2, FcCmdData->pcCmd, FcCmdData->iCmdLen );
	bSetStatus = package_and_send_frame( FRAME_HEAD_RESET_CAN_FC_FRAME, ( STRUCT_CMD * )&SendFcCmdData, ( byte )FcFrameTime );

	if( NULL != SendFcCmdData.pcCmd )
	{
		free( SendFcCmdData.pcCmd );
		SendFcCmdData.pcCmd = NULL;
	}

	return bSetStatus;
}

/*************************************************
Description:	设置ECU是否回复
Input:
Output:
Return:
Others:
*************************************************/
void process_SET_ECU_REPLY( void* pIn, void* pOut )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	byte *pcOutTemp = ( byte * )pOut;
	byte * pcTemp = NULL;
	bool bSetStatus = false;
	int i = 0;

	//assert(pstParam->pcData);
	//assert(pstParam->iLen != 0);
	pcTemp = pstParam->pcData;

	if( ( pstParam->iLen == 0 ) )
	{
		general_return_status( SUCCESS, NULL, 0, pcOutTemp );
		pcOutTemp[2] = 0;//不提示
		return;
	}

	for( i = 0; i < ( int )strlen( pcTemp ) ; i++ )
	{
		pcTemp[i] = toupper( pcTemp[i] ); //转变大写
	}

	if( 0 != strcmp( pcTemp, "TRUE" ) )
	{
		general_return_status( SUCCESS, NULL, 0, pcOutTemp );
		pcOutTemp[2] = 0;//不提示
		return;
	}

	bSetStatus = bSend_Ecu_Reply_Cmd( ( int )u32Config_fuc[0], ( int )u32Config_fuc[1] );

	if( bSetStatus ) //如果接收成功且设置成功则返回SUCCESS
	{
		general_return_status( SUCCESS, NULL, 0, pcOutTemp );
		pcOutTemp[2] = 0;//不提示
	}
	else
	{
		general_return_status( FAILE, NULL, 0, pcOutTemp );
		pcOutTemp[2] = 0;//不提示
	}

}

/*************************************************
Description:	发送ECU 是否回复命令
Input:
		ECUReplyFlag		ECU是否回复  0: 不回复   1:  回复
		ECUReplyTimeOut		ECU回复命令的超时时间，单位ms  范围 (0 - 65535 )

Output:	bool
Return:
Others:
*************************************************/

bool bSend_Ecu_Reply_Cmd( int ECUReplyFlag, int ECUReplyTimeOut )
{
	STRUCT_CMD  SendECUReplyData = {0};
	bool  bSetStatus = false;
	SendECUReplyData.iCmdLen = 3;
	SendECUReplyData.pcCmd = ( byte* )malloc( sizeof( byte ) * 3 );

	SendECUReplyData.pcCmd[0] = ( byte )ECUReplyFlag;
	SendECUReplyData.pcCmd[1] = ( byte )( ECUReplyTimeOut / 256 );
	SendECUReplyData.pcCmd[2] = ( byte )( ECUReplyTimeOut % 256 );
	bSetStatus = package_and_send_frame( FRAME_HEAD_ECU_REPLAY, ( STRUCT_CMD * )&SendECUReplyData, 0xaa );	 //0xaa 是保留字节VDI没有用到

	if( NULL != SendECUReplyData.pcCmd )
	{
		free( SendECUReplyData.pcCmd );
		SendECUReplyData.pcCmd = NULL;
	}

	return bSetStatus;

}


/*************************************************
Description:	接收几帧命令后发送一帧命令
Input:
Output:
Return:
Others:
*************************************************/

void get_accord_ecu_cmdnum_send_cmdconfig_data( void* pIn, void* pOut )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	byte *pcOutTemp = ( byte * )pOut;
	STRUCT_CMD *SetCmdData = NULL;

	bool bSetStatus = false;
	int SetFrameNumber = 0;
	int i = 0;
	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	SetFrameNumber = get_command_config_data( pstParam, &SetCmdData );

	if( 0 == SetFrameNumber )
	{
		//释放内存
		if( NULL != SetCmdData )
		{
			for( i = 0; i < SetFrameNumber; i++ )
			{
				if( NULL != SetCmdData[i].pcCmd )
				{
					free( SetCmdData[i].pcCmd );
					SetCmdData[i].pcCmd = NULL;
				}
			}

			free( SetCmdData );

			SetCmdData = NULL;
		}

		general_return_status( SUCCESS, NULL, 0, pcOutTemp );
		pcOutTemp[2] = 0;//不提示
		return ;
	}

	bSetStatus = bSend_Receive_config_Frame_Cmd( SetCmdData, SetFrameNumber, u32Config_fuc );

	if( bSetStatus ) //如果接收成功且设置成功则返回SUCCESS
	{
		general_return_status( SUCCESS, NULL, 0, pcOutTemp );
		pcOutTemp[2] = 0;//不提示
	}
	else
	{
		general_return_status( FAILE, NULL, 0, pcOutTemp );
		pcOutTemp[2] = 0;//不提示
	}

	//释放内存
	if( NULL != SetCmdData )
	{
		for( i = 0; i < SetFrameNumber; i++ )
		{
			if( NULL != SetCmdData[i].pcCmd )
			{
				free( SetCmdData[i].pcCmd );
				SetCmdData[i].pcCmd = NULL;
			}
		}

		free( SetCmdData );

		SetCmdData = NULL;
	}

}

/*************************************************
Description:	接收几帧命令后发送一帧命令
Input:
		SetCmdData		存储的命令
		SetFrameNumber	命令的个数
        u32Config_fuc   配置命令首地址

Output:	bool
Return:
Others:
*************************************************/

bool bSend_Receive_config_Frame_Cmd( const STRUCT_CMD * SetCmdData, const int SetFrameNumber,
                                     const uint32 * u32Config_fuc )
{
	UNN_2WORD_4BYTE uFrameLen;
	bool bReceiveStatus = false;
	bool bReturnStatus = false;
	byte *pcSendCache = NULL;
	byte cReceiveCache[5] = {0};//接收的缓存
	byte cCheckNum = 0;
	int i = 0;
	int j = 0;

	uFrameLen.u32Bit = 0;

	for( i = 0; i < SetFrameNumber; i++ )
	{
		uFrameLen.u32Bit += SetCmdData[i].iCmdLen ;
	}


	uFrameLen.u32Bit += 1 + 2 + 11 + 1 + SetFrameNumber + 1;

	pcSendCache = ( byte * )malloc( ( uFrameLen.u32Bit ) * sizeof( byte ) ); //发送帧的缓存

	pcSendCache[0] = FRAME_HEAD_SEND_RECEIVE;
	pcSendCache[1] = uFrameLen.u8Bit[1];
	pcSendCache[2] = uFrameLen.u8Bit[0];

	pcSendCache[3] = ( byte )u32Config_fuc[0]; //字节间间隔
	pcSendCache[4] = 0xff; //保留字节
	pcSendCache[5] = 0xff; //保留字节
	pcSendCache[6] = ( byte )u32Config_fuc[1]; //设置发送命令的打开/关闭状态
	pcSendCache[7] = ( byte )u32Config_fuc[2]; //插帧命令的控制方式
	pcSendCache[8] = u32Config_fuc[3] / 256; //发送插帧命令的条件：
	pcSendCache[9] = u32Config_fuc[3] % 256; //发送插帧命令的条件：
	pcSendCache[10] = u32Config_fuc[4] / 256; //要发送的命令大于一帧时，帧与帧之间的间隔。（单帧命令的发送不需要配置）
	pcSendCache[11] = u32Config_fuc[4] % 256; //要发送的命令大于一帧时，帧与帧之间的间隔。（单帧命令的发送不需要配置）
	pcSendCache[12] = ( byte )u32Config_fuc[5]; //发送的命令是否有回复
	pcSendCache[13] = ( byte )u32Config_fuc[6]; //发送命令的回复是否上传

	pcSendCache[14] = ( byte )SetFrameNumber; //命令条数

	for( i = 0 ; i < SetFrameNumber; i++ )
	{
		pcSendCache[14 + j + 1 ] = SetCmdData[i].iCmdLen;
		memcpy( &pcSendCache[14 + j + 2], SetCmdData[i].pcCmd, SetCmdData[i].iCmdLen );
		j += SetCmdData[i].iCmdLen + 1;
	}

	for( i = 0; ( i < ( int )uFrameLen.u32Bit - 1 ); i++ )
	{
		cCheckNum += pcSendCache[i];
	}

	pcSendCache[ uFrameLen.u32Bit - 1] = cCheckNum;

	send_cmd( pcSendCache, uFrameLen.u32Bit );

	if( ( bool )receive_cmd( cReceiveCache, 5, 3000 ) )
	{
		if( cReceiveCache[3] == 0x00 )
		{
			bReturnStatus = true;
		}
	}

	free( pcSendCache );

	return bReturnStatus;
}

