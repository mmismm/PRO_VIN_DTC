/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	��������֡����������������ݴ�����
History:
	<author>	<time>		<desc>

************************************************************************/
#ifndef _FREEZE_LIB_H
#define _FREEZE_LIB_H
#include "../interface/protocol_define.h"

enum READ_FREEZE_DTC_TYPE
{
    READ_FREEZE_DTC_BY_ISO14229 = 0,
};

enum READ_FREEZE_DS_TYPE
{
    READ_FREEZE_DS_BY_ISO14229 = 0,
    READ_ONE_MEMORY_REQUEST_ALL_DATA_BY_ISO14230,
    READ_FREEZE_ACCORD_XML,
};

void process_read_freeze_frame_DTC( void* pIn, void* pOut );
void process_read_freeze_frame_DS( void* pIn, void* pOut );

void process_read_freeze_frame_DS_by_ISO14229( void* pIn, void* pOut );
void process_read_one_memory_request_all_data_DS_by_ISO14230( void* pIn, void* pOut );
void process_read_freeze_frame_DS_by_xml( void* pIn, void* pOut );
void process_read_freeze_frame_DTC_by_ISO14229( void* pIn, void* pOut );

int process_freeze_DTC_data_by_ISO14229( byte* pcDctData, int iValidLen, STRUCT_CHAIN_DATA_OUTPUT* pstDtc );
int process_freeze_data_stream_by_ISO14229( byte* pcDsData, int iValidLen,  void* pOut );
int process_one_memory_request_all_data_freeze_data_stream_by_ISO14230( byte* pcDsData, int iValidLen,  void* pOut );
int process_freeze_data_stream_by_xml( byte* pcDsData, int iValidLen,  void* pOut );

#endif

