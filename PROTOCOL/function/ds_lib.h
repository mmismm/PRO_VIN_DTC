/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	�������������ݴ�����
History:
	<author>	<time>		<desc>
************************************************************************/
#ifndef _DS_LIB_H
#define _DS_LIB_H
#include "../interface/protocol_define.h"
#include "../public/protocol_config.h"

typedef struct _STRUCT_CMD_ID_ASSOCIATE_BUFFER_OFFSET
{
	int iCmdID;
	byte cBufferOffset;
} STRUCT_CMD_ID_ASSOCIATE_BUFFER_OFFSET;

typedef struct _STRUCT_DS_STORAGE_BUFFER
{
	bool bNegativeFlag;
	int iValidLen;
	byte cBuffer[512];
} STRUCT_DS_STORAGE_BUFFER;

enum READ_DATA_STREAM_TYPE
{
    READ_GENERAL_DATA_STREAM = 0,
};

enum FORMULA_TYPE
{
    FORMULA_PARSER = 0,
    PROTOCOL_PARSER,
};

byte get_current_screen_DS_config( void* pIn );
void free_general_DS_formula_config_space( void );
void process_read_data_stream( void* pIn, void* pOut );
int get_ds_config( STRUCT_DS_FORMULA_CONFIG * pstDSConfig, const byte * pcSource );
byte get_DS_buffer_offset( int iDSCmdID );

void process_read_general_data_stream( void* pIn, void* pOut );
void process_general_data_stream_data( STRUCT_DS_FORMULA_GROUP_CONFIG * pstDSGroupConfig, byte cItemSum,
                                       STRUCT_CHAIN_DATA_OUTPUT* pstDataStreamOut );

#endif