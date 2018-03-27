/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	定义故障码数据处理函数
History:
	<author>	<time>		<desc>

************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dtc_lib.h"
#include "../public/protocol_config.h"
#include "../formula/formula_comply.h"
#include "../command/command.h"
#include "../public/public.h"
#include "../interface/protocol_define.h"
#include <assert.h>
#include "../InitConfigFromXml/init_config_from_xml_lib.h"
#include "../SpecialFunction/special_function.h"

STRUCT_SELECT_FUN stReadCurrentDTCFunGroup[] =
{
	{GENERAL_CURRENT_DTC, process_general_read_current_Dtc},
//	{SINGLE_LINE_CAN,process_read_single_line_can_current_Dtc},
};

/*************************************************
Description:	获取读当前码函数
Input:
	cType		配置类型
Output:	保留
Return:	pf_general_function 函数指针
Others:
*************************************************/
pf_general_function get_read_current_DTC_fun( byte cType )
{
	int i = 0;

	for( i = 0; i < sizeof( stReadCurrentDTCFunGroup ) / sizeof( stReadCurrentDTCFunGroup[0] ); i++ )
		if( cType == stReadCurrentDTCFunGroup[i].cType )
			return stReadCurrentDTCFunGroup[i].pFun;

	return 0;
}

/*************************************************
Description:	读取当前码
Input:	pIn		输入参数（保留）
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
void process_read_current_Dtc( void* pIn, void* pOut )
{
	pf_general_function pFun = NULL;

	pFun = get_read_current_DTC_fun( g_p_stProcessFunConfig->cCurrentDTCFunOffset );

	assert( pFun );

	pFun( pIn, pOut );
}

STRUCT_SELECT_FUN stReadHistoryDTCFunGroup[] =
{
	{GENERAL_HISTORY_DTC, process_general_read_history_Dtc},
};

/*************************************************
Description:	获取读历史码函数
Input:
	cType		配置类型
Output:	保留
Return:	pf_general_function 函数指针
Others:
*************************************************/
pf_general_function get_read_history_DTC_fun( byte cType )
{
	int i = 0;

	for( i = 0; i < sizeof( stReadHistoryDTCFunGroup ) / sizeof( stReadHistoryDTCFunGroup[0] ); i++ )
		if( cType == stReadHistoryDTCFunGroup[i].cType )
			return stReadHistoryDTCFunGroup[i].pFun;

	return 0;
}

/*************************************************
Description:	读取历史码
Input:	pIn		输入参数（保留）
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
void process_read_history_Dtc( void* pIn, void* pOut )
{
	pf_general_function pFun = NULL;

	pFun = get_read_history_DTC_fun( g_p_stProcessFunConfig->cHistoryDTCFunOffset );

	assert( pFun );

	pFun( pIn, pOut );
}

/*************************************************
Description:	读取当前故障码
Input:	pIn		输入参数（保留）
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
void process_general_read_current_Dtc( void* pIn, void* pOut )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	STRUCT_CHAIN_DATA_OUTPUT* pstDtcOut = ( STRUCT_CHAIN_DATA_OUTPUT* )malloc( sizeof( STRUCT_CHAIN_DATA_OUTPUT ) );
	byte *pcOutTemp = ( byte * )pOut;
	int iReceiveResult = TIME_OUT;
	int iReceiveValidLen = 0;//接收到的有效字节长度

	int iProcessStatus;//数据理状态

	byte cDtcMask = 0;

	byte cBufferOffset = 0;
	int iDTCConfigOffset = 0;
	int iCmdSum = 0;
	byte cConfigTemp[4] = {0};
	uint32 u32CmdTemp[6] = {0};
	int * piCmdIndex = NULL;
	int i = 0;
	byte cDtcConfigType = 0;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	iDTCConfigOffset = atoi( pstParam->pcData );//获得故障码配置ID

	pstParam = pstParam->pNextNode;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	cDtcConfigType = ( byte )atoi( pstParam->pcData ); //获得故障码配置模板号

	select_dtc_config( iDTCConfigOffset, cDtcConfigType );

	pstParam = pstParam->pNextNode;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	//获取读码命令条数、命令内容
	iCmdSum = get_string_type_data_to_uint32( u32CmdTemp, pstParam->pcData, pstParam->iLen );

	if( 0 == iCmdSum )
	{
		general_return_status( SUCCESS, NULL, 0, pcOutTemp );
		pcOutTemp[2] = 0;//不提示

		free( pstDtcOut );

		return ;
	}

	piCmdIndex = ( int * )malloc( sizeof( int ) * ( iCmdSum + 1 ) );

	piCmdIndex[0] = iCmdSum;

	for( i = 0; i < iCmdSum; i++ )
	{
		piCmdIndex[i + 1] = ( int )u32CmdTemp[i];
	}

	iReceiveResult = send_and_receive_cmd( piCmdIndex );
// 	cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[ piCmdIndex[1] ].cBufferOffset;
	cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[ piCmdIndex[iCmdSum] ].cBufferOffset;

	iReceiveValidLen = g_stBufferGroup[cBufferOffset].iValidLen;
	cDtcMask = g_p_stGeneralReadDtcConfig->cCurrentDtcMask;

	switch( iReceiveResult )
	{
	case SUCCESS:
	{
		iProcessStatus = process_Dtc_data( g_stBufferGroup[cBufferOffset].cBuffer, iReceiveValidLen,
		                                   pstDtcOut, cDtcMask );

		if( ( iProcessStatus == NO_HISTORY_DTC ) || ( iProcessStatus == NO_CURRENT_DTC ) )
		{
			general_return_status( iProcessStatus, NULL, 0, pOut );
		}
		else
		{
			copy_data_to_out( pOut, pstDtcOut );
			free_param_out_data_space( pstDtcOut );
		}

	}
	break;

	case NEGATIVE:
	case FRAME_TIME_OUT:
	case TIME_OUT:
	default:
		general_return_status( iReceiveResult, g_stBufferGroup[cBufferOffset].cBuffer, 3, pOut );
		break;

	}


	free( piCmdIndex );
	free( pstDtcOut );
}



/*

bool bjudge_Buffer_ALL_Zero(byte *pcSource, int leng )
{
	int  i = 0;
    bool bStatus = false;
	for (i = 0; i < leng; i++)
	{
		if (pcSource[i] != 0)
		{
			return false;
		}
	}

	return true;
}

int process_single_line_CAN_receive_surplus_Cmd(const byte cBufferOffset)
{
	uint16	u16ECUResTimeout	= g_p_stISO15765Config->u16ECUResTimeout;

	byte cDtcStartOffset	    = g_p_stGeneralReadDtcConfig->cDtcStartOffset;	//有效回复中保存DTC起始偏移，从SID回复开始
	byte cDtcBytesInCmd		    = g_p_stGeneralReadDtcConfig->cDtcBytesInCmd;	//命令中几个字节表示一个故障码
    int		iReceiveStatus      = TIME_OUT;
    byte    pcSource[20]        = {0};
	int     iValidLen  = 0;


	iReceiveStatus = process_CAN_receive_single_Cmd(pcSource, u16ECUResTimeout);

	if( iReceiveStatus != SUCCESS )
	{
		return iReceiveStatus;
	}
	iValidLen = cDtcStartOffset + cDtcBytesInCmd;
	g_stBufferGroup[cBufferOffset].iValidLen = iValidLen;

	memcpy( g_stBufferGroup[cBufferOffset].cBuffer[0], pcSource+2, iValidLen );

	if(bjudge_Buffer_ALL_Zero(pcSource+cDtcStartOffset+2,cDtcBytesInCmd))
	{
		return SUCCESS;
	}

   do
   {
	   iReceiveStatus = process_CAN_receive_single_Cmd(pcSource, u16ECUResTimeout);
	   if (iReceiveStatus != SUCCESS)
	   {
		   return iReceiveStatus;
	   }
	   memcpy( g_stBufferGroup[cBufferOffset].cBuffer[ g_stBufferGroup[cBufferOffset].iValidLen ], pcSource+cDtcStartOffset+2, cDtcBytesInCmd );
       g_stBufferGroup[cBufferOffset].iValidLen += cDtcBytesInCmd;

   } while (!bjudge_Buffer_ALL_Zero(pcSource+cDtcStartOffset+2,cDtcBytesInCmd));

   return SUCCESS;

}





int send_and_receive_single_line_can(STRUCT_CMD *pstSendCmd,const byte cBufferOffset)
{
	byte	cReserved			= g_p_stISO15765Config->cReserved;
	byte	cRetransTime	    = g_p_stISO15765Config->cRetransTime;

    int		iReceiveStatus      = TIME_OUT;
    bool	bSendStatus         = false;
    int		iValidLen = 0;

	while(cRetransTime-- )
	{
		bSendStatus = package_and_send_frame( FRAME_HEAD_MULTI_FRAME, pstSendCmd, cReserved );
		if( !bSendStatus )
		{
			return FAILE;
		}


        iReceiveStatus = process_single_line_CAN_receive_surplus_Cmd(cBufferOffset);
		if (iReceiveStatus == SUCCESS)
		{
			return  iReceiveStatus;
		}

	}
	return iReceiveStatus;




};

/ *************************************************
Description:	读单线can的当前故障码
Input:	pIn		输入参数（保留）
Output:	pOut	输出数据地址
Return:	保留
Others:
************************************************* /
void process_read_single_line_can_current_Dtc( void* pIn, void* pOut )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	STRUCT_CHAIN_DATA_OUTPUT* pstDtcOut = ( STRUCT_CHAIN_DATA_OUTPUT* )malloc( sizeof( STRUCT_CHAIN_DATA_OUTPUT ) );
	byte *pcOutTemp = ( byte * )pOut;
	int iReceiveResult = TIME_OUT;
	int iReceiveValidLen = 0;//接收到的有效字节长度
	int iProcessStatus;//数据理状态
    bool bSendStatus = false;
	byte cDtcMask = 0;

	byte cBufferOffset = 0;
	int iDTCConfigOffset = 0;
	int iCmdSum = 0;
	byte cConfigTemp[4] = {0};
	uint32 u32CmdTemp[6] = {0};
	int * piCmdIndex = NULL;
	int i = 0;
	byte cDtcConfigType = 0;

	assert(pstParam->pcData);
	assert(pstParam->iLen != 0);

	iDTCConfigOffset = atoi( pstParam->pcData );//获得故障码配置ID

	pstParam = pstParam->pNextNode;

	assert(pstParam->pcData);
	assert(pstParam->iLen != 0);

	cDtcConfigType = (byte)atoi( pstParam->pcData );//获得故障码配置模板号

	select_dtc_config(iDTCConfigOffset,cDtcConfigType);

	pstParam = pstParam->pNextNode;

	assert(pstParam->pcData);
	assert(pstParam->iLen != 0);

	//获取读码命令条数、命令内容
	iCmdSum = get_string_type_data_to_uint32(u32CmdTemp, pstParam->pcData, pstParam->iLen);

	if (0 == iCmdSum)
	{
		general_return_status( SUCCESS, NULL, 0, pcOutTemp );
		pcOutTemp[2] = 0;//不提示

		free( pstDtcOut );

		return ;
	}

	piCmdIndex = (int *)malloc(sizeof(int) * (iCmdSum + 1) );
	cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[ piCmdIndex[1] ].cBufferOffset;

	piCmdIndex[0] = iCmdSum;
	for(i = 0; i < iCmdSum; i++)
	{
		piCmdIndex[i + 1] = (int)u32CmdTemp[i];
		iReceiveResult = send_and_receive_single_line_can(&g_stInitXmlGobalVariable.m_p_stCmdList[piCmdIndex[1]],cBufferOffset);

	}

	iReceiveValidLen = g_stBufferGroup[cBufferOffset].iValidLen;
	cDtcMask = g_p_stGeneralReadDtcConfig->cCurrentDtcMask;

	switch( iReceiveResult )
	{
	case SUCCESS:
		{
			iProcessStatus = process_Dtc_data( g_stBufferGroup[cBufferOffset].cBuffer, iReceiveValidLen,
				pstDtcOut, cDtcMask );

			if( ( iProcessStatus == NO_HISTORY_DTC ) || ( iProcessStatus == NO_CURRENT_DTC ) )
			{
				general_return_status( iProcessStatus, NULL, 0, pOut );
			}
			else
			{
				copy_data_to_out( pOut, pstDtcOut );
				free_param_out_data_space( pstDtcOut );
			}

		}
		break;

	case NEGATIVE:
	case FRAME_TIME_OUT:
	case TIME_OUT:
	default:
		general_return_status( iReceiveResult, g_stBufferGroup[cBufferOffset].cBuffer, 3, pOut );
		break;

	}


	free(piCmdIndex);
	free( pstDtcOut );
}

*/



/*************************************************
Description:	读取历史故障码
Input:	pIn		输入参数（保留）
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
void process_general_read_history_Dtc( void* pIn, void* pOut )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	STRUCT_CHAIN_DATA_OUTPUT* pstDtcOut = ( STRUCT_CHAIN_DATA_OUTPUT* )malloc( sizeof( STRUCT_CHAIN_DATA_OUTPUT ) );
	byte *pcOutTemp = ( byte * )pOut;
	int iReceiveResult = TIME_OUT;
	int iReceiveValidLen = 0;//接收到的有效字节长度

	int iProcessStatus;//数据理状态

	byte cDtcMask = 0;

	byte cBufferOffset = 0;
	int iDTCConfigOffset = 0;
	int iCmdSum = 0;
	byte cConfigTemp[4] = {0};
	uint32 u32CmdTemp[6] = {0};
	int *piCmdIndex  = NULL;
	int i = 0;
	byte cDtcConfigType = 0;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	iDTCConfigOffset = atoi( pstParam->pcData );//获得故障码配置ID

	pstParam = pstParam->pNextNode;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	cDtcConfigType = ( byte )atoi( pstParam->pcData ); //获得故障码配置模板号

	select_dtc_config( iDTCConfigOffset, cDtcConfigType );

	pstParam = pstParam->pNextNode;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	//获取读码命令条数、命令内容
	iCmdSum = get_string_type_data_to_uint32( u32CmdTemp, pstParam->pcData, pstParam->iLen );

	if( 0 == iCmdSum )
	{
		general_return_status( SUCCESS, NULL, 0, pcOutTemp );
		pcOutTemp[2] = 0;//不提示

		free( pstDtcOut );

		return ;
	}

	piCmdIndex = ( int * )malloc( sizeof( int ) * ( iCmdSum + 1 ) );

	piCmdIndex[0] = iCmdSum;

	for( i = 0; i < iCmdSum; i++ )
	{
		piCmdIndex[i + 1] = ( int )u32CmdTemp[i];
	}

	iReceiveResult = send_and_receive_cmd( piCmdIndex );

// 	cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[ piCmdIndex[1] ].cBufferOffset;
	cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[ piCmdIndex[iCmdSum] ].cBufferOffset;

	iReceiveValidLen = g_stBufferGroup[cBufferOffset].iValidLen;
	cDtcMask = g_p_stGeneralReadDtcConfig->cHistoryDtcMask;

	switch( iReceiveResult )
	{
	case SUCCESS:
	{
		iProcessStatus = process_Dtc_data( g_stBufferGroup[cBufferOffset].cBuffer, iReceiveValidLen,
		                                   pstDtcOut, cDtcMask );

		if( ( iProcessStatus == NO_HISTORY_DTC ) || ( iProcessStatus == NO_CURRENT_DTC ) )
		{
			general_return_status( iProcessStatus, NULL, 0, pOut );
		}
		else
		{
			copy_data_to_out( pOut, pstDtcOut );
			free_param_out_data_space( pstDtcOut );
		}

	}
	break;

	case NEGATIVE:
	case FRAME_TIME_OUT:
	case TIME_OUT:
	default:
		general_return_status( iReceiveResult, g_stBufferGroup[cBufferOffset].cBuffer, 3, pOut );
		break;

	}

	free( piCmdIndex );
	free( pstDtcOut );
}

/*
typedef struct
{
byte cDtcStartOffset;	//有效回复中保存DTC起始偏移，从SID回复开始
byte cDtcBytesInCmd;	//命令中几个字节表示一个故障码
byte cStatusOffset;		//故障码状态在一个完整故障码中的偏移
byte cDtcBytesInDisplay;//一个故障码需要显示几个字节
byte cCurrentDtcMask;	//当前故障码mask
byte cHistoryDtcMask;	//历史故障码mask
}STRUCT_READ_DTC_CONFIG;
*/
/*************************************************
Description:	处理故障码数据
Input:
	pcDctData	故障码回复命令存储地址
	iValidLen	有效长度
	cDtcMask	故障码状态

Output:	pstDtc	输出链表指针
Return:	int		读码结果（无当前码、无历史码
							、成功）
Others:
*************************************************/
int process_Dtc_data( byte* pcDctData, int iValidLen, STRUCT_CHAIN_DATA_OUTPUT* pstDtc, byte cDtcMask )
{
	int i = 0;
	int j = 0;
	int iDtcNum = 0;
	byte *pcDtcStart = NULL;//故障码起始存放地址
	byte cDtcStatusCache[256] = {0};

	byte cDtcStartOffset	= g_p_stGeneralReadDtcConfig->cDtcStartOffset;	//有效回复中保存DTC起始偏移，从SID回复开始
	byte cDtcBytesInCmd		= g_p_stGeneralReadDtcConfig->cDtcBytesInCmd;	//命令中几个字节表示一个故障码
	byte cStatusOffset		= g_p_stGeneralReadDtcConfig->cStatusOffset;	//故障码状态在一个完整故障码中的偏移
	byte cDtcBytesInDisplay = g_p_stGeneralReadDtcConfig->cDtcBytesInDisplay;//一个故障码需要显示几个字节

	STRUCT_CHAIN_DATA_NODE *phead  = NULL;
	STRUCT_CHAIN_DATA_NODE *ptemp1 = NULL;
	STRUCT_CHAIN_DATA_NODE *ptemp2 = NULL;
	STRUCT_CHAIN_DATA_NODE *ptemp3 = NULL;

	iDtcNum = ( iValidLen -  cDtcStartOffset ) / cDtcBytesInCmd;
	pcDtcStart = pcDctData + cDtcStartOffset;

	pstDtc->cKeyByte[0] = 1;
	pstDtc->cKeyByte[1] = 0;
	pstDtc->cKeyByte[2] = 0;
	pstDtc->stJump.cLenHighByte = 0;
	pstDtc->stJump.cLenLowByte = 0;
	pstDtc->stTip.cLenHighByte = 0;
	pstDtc->stTip.cLenLowByte = 0;
	pstDtc->pstData = NULL;

	if( 0 == iDtcNum ) //没有故障码
	{
		if( cDtcMask == g_p_stGeneralReadDtcConfig->cCurrentDtcMask )
			return NO_CURRENT_DTC;
		else if( cDtcMask == g_p_stGeneralReadDtcConfig->cHistoryDtcMask )
			return NO_HISTORY_DTC;
	}

	for( i = 0; i < iDtcNum; i++ )
	{
		ptemp2 = ( STRUCT_CHAIN_DATA_NODE * )malloc( sizeof( STRUCT_CHAIN_DATA_NODE ) ); //dtc id
		ptemp2->cLenHighByte = 0;
		ptemp2->cLenLowByte = cDtcBytesInDisplay * 2;//一个字节对应的字符长度为2
		ptemp2->pcData = ( byte * )malloc( ( cDtcBytesInDisplay * 2 + 1 ) * sizeof( byte ) );

		for( j = 0; j < cDtcBytesInDisplay; j++ )
		{
			sprintf( &( ptemp2->pcData[j * 2] ), "%02X", pcDtcStart[j + i * cDtcBytesInCmd] );
		}

		ptemp3 = ( STRUCT_CHAIN_DATA_NODE * )malloc( sizeof( STRUCT_CHAIN_DATA_NODE ) ); //dtc status

		get_Dtc_status( pcDtcStart[cStatusOffset + i * cDtcBytesInCmd], cDtcStatusCache, cDtcMask );

		/* 此段代码显示故障码状态*/
		ptemp3->cLenHighByte = 0;
		ptemp3->cLenLowByte = ( byte )strlen( cDtcStatusCache );
		ptemp3->pcData = ( byte * )malloc( strlen( cDtcStatusCache ) * sizeof( byte ) );
		memcpy( ptemp3->pcData, cDtcStatusCache, strlen( cDtcStatusCache ) );

		ptemp2->pNextNode = ptemp3;

		if( i == 0 )
		{
			phead = ptemp2;
			ptemp1 = ptemp3;
		}
		else
		{
			ptemp1->pNextNode = ptemp2;
			ptemp1 = ptemp3;
		}

	}

	ptemp1->pNextNode = NULL;

	pstDtc->pstData = phead;

	return SUCCESS;
}

