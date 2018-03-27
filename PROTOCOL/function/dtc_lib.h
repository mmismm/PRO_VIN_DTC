/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	声明故障码数据处理函数
History:
	<author>	<time>		<desc>

************************************************************************/
#ifndef _DTC_LIB_H
#define _DTC_LIB_H
#include "../interface/protocol_define.h"

enum READ_CURRENT_DTC_TYPE
{
    GENERAL_CURRENT_DTC = 0,
    SINGLE_LINE_CAN,

};

enum READ_HISTORY_DTC_TYPE
{
    GENERAL_HISTORY_DTC = 0,
};

int process_Dtc_data( byte* pcDctData, int iValidLen, STRUCT_CHAIN_DATA_OUTPUT* pstDtc, byte cDtcMask );
void process_read_current_Dtc( void* pIn, void* pOut );
void process_read_history_Dtc( void* pIn, void* pOut );
void process_general_read_current_Dtc( void* pIn, void* pOut );
void process_general_read_history_Dtc( void* pIn, void* pOut );
//void process_read_single_line_can_current_Dtc( void* pIn, void* pOut );
#endif

