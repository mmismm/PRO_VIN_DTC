/**********************************************************************
Copyright:		YiXiong S&T Co., Ltd.
Description:	声明与命令或具体功能配置相关的全局或局部变量
History:
	<author>	<time>		<desc>

************************************************************************/

#ifndef _PROTOCOL_CMD_H
#define _PROTOCOL_CMD_H
#include "../interface/protocol_define.h"

extern  STRUCT_CMD g_stCmdList[];
extern  STRUCT_SYSTotle g_stSYSList[];
extern  byte ActiveParams_KWP[][23];
extern	byte bVDI_SET[][11];

extern  int	g_iDefaultSessionCmdIndex[];
extern  int	g_iExtendSessionCmdIndex[];

extern	int	g_iRequestSeedCmdIndex[];
extern	int	g_iSendKeyCmdIndex[];
extern  int	g_iQuitActuatorTestCmdIndex[];

extern int g_iActiveECURequestSeedOffset;
extern int g_iActiveECUSendKeyOffset;


#endif