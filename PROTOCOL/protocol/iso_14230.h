/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	��������ISO14230Э�鴦���շ����ݵ���غ���
History:
	<author>	<time>		<desc>

************************************************************************/
#ifndef _ISO_14230_H
#define _ISO_14230_H
#include "../interface/protocol_define.h"

int send_and_receive_cmd_by_iso_14230( byte * piCmdIndex );
int send_and_receive_single_cmd_by_iso_14230( STRUCT_CMD *pstSendCmd, const int cCmdIndex, byte* pcReceiveBuffer );
int process_KWP_receive_Cmd( const byte cBufferOffset, byte* pcSource );
void pre_process_cmd( STRUCT_CMD* pstSnedCmd );
int process_KWP_receive_single_Cmd( byte** ppcSource, int* piValidLen, const uint16 u16FrameTimeout );

#endif


