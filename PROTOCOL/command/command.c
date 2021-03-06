/**********************************************************************
Copyright:		YiXiong S&T Co., Ltd.
Description:	定义与命令或具体功能配置相关的全局或局部变量
History:
	<author>	<time>		<desc>
************************************************************************/

#include "../interface/protocol_define.h"
#include "command.h"
#include <stdio.h>

//*********************************************************************************
//描述:		命令索引
//引用范围:	全局
//参数描述:	命令总数，具体命令索引...
//*********************************************************************************
int	g_iDefaultSessionCmdIndex[]		= {1, 0};
int	g_iExtendSessionCmdIndex[]		= {1, 0};
int	g_iRequestSeedCmdIndex[]		= {1, 0};
int	g_iSendKeyCmdIndex[]			= {1, 0};
int	g_iQuitActuatorTestCmdIndex[]	= {1, 0};
int g_iActiveECURequestSeedOffset;
int g_iActiveECUSendKeyOffset;

//*********************************************************************************
//描述:		具体命令 D50 D60 D70 C61X
//*********************************************************************************
byte cCmdBCM00[] = {0x07, 0x25, 0x02, 0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};//进入命令
byte cCmdBCM01[] = {0x07, 0x25, 0x02, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};//空闲命令
byte cCmdBCM02[] = {0x07, 0x25, 0x03, 0x22, 0xF1, 0x90, 0x00, 0x00, 0x00, 0x00};//读vin
byte cCmdBCM03[] = {0x07, 0x25, 0x30, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00};//30H设置

byte cCmdPEPS00[] = {0x07, 0x29, 0x02, 0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};//进入命令
byte cCmdPEPS01[] = {0x07, 0x29, 0x02, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};//空闲命令
byte cCmdPEPS02[] = {0x07, 0x29, 0x03, 0x22, 0xF1, 0x90, 0x00, 0x00, 0x00, 0x00};//读vin
byte cCmdPEPS03[] = {0x07, 0x29, 0x30, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00};//30H设置

byte cCmdEMS00[] = {0x07, 0xE0, 0x02, 0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};//进入命令
byte cCmdEMS01[] = {0x07, 0xE0, 0x02, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};//空闲命令
byte cCmdEMS02[] = {0x07, 0xE0, 0x03, 0x22, 0xF1, 0x90, 0x00, 0x00, 0x00, 0x00};//读vin
byte cCmdEMS03[] = {0x07, 0xE0, 0x30, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00};//30H设置
//*********************************************************************************
//描述:		各系统清除命令
//*********************************************************************************
byte bCmdClearDTC[][10] = {
	{0x07, 0x10, 0x04, 0x14, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00},  //ABS系统
	{0x07, 0x23, 0x04, 0x14, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00},  //AC系统
	{0x07, 0x25, 0x04, 0x14, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00},  //BCM系统
	{0x07, 0xE0, 0x04, 0x14, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00},  //EMS系统
	{0x07, 0x15, 0x04, 0x14, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00},  //EPS系统
	{0x07, 0x10, 0x04, 0x14, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00},  //ESP系统
	{0x07, 0x20, 0x04, 0x14, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00},  //HUM系统
	{0x07, 0x24, 0x04, 0x14, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00},  //ICM系统
	{0x07, 0x21, 0x04, 0x14, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00},  //SDM系统
	{0x07, 0xE1, 0x04, 0x14, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00},  //TCU系统
};
//*********************************************************************************
//描述:		各系统激活进入命令
//*********************************************************************************
byte bCmdActiveECU[][10] = {
	{0x07, 0x10, 0x02, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},  //ABS系统
	{0x07, 0x23, 0x02, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},  //AC系统
	{0x07, 0x25, 0x02, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},  //BCM系统
	{0x07, 0xE0, 0x02, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},  //EMS系统
	{0x07, 0x15, 0x02, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},  //EPS系统
	{0x07, 0x10, 0x02, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},  //ESP系统
	{0x07, 0x20, 0x02, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},  //HUM系统
	{0x07, 0x24, 0x02, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},  //ICM系统
	{0x07, 0x21, 0x02, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},  //SDM系统
	{0x07, 0xE1, 0x02, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},  //TCU系统
};

//*********************************************************************************
//描述:	VDI配置表
//具体：通讯类型(1)、通讯脚(2)、ECU波特率(3)、滤波ID个数、ID号(4个为一组)、ID号
//      (其它配置默认，后续如果有需要再扩展)
//*********************************************************************************	
byte bVDI_SET[][11] = {
	{0x80, 6, 14, 0x07, 0xA1, 0x20, 1, 0,0,0x07,0x90 },  //ABS系统
	{0x80, 3, 11, 0x01, 0xE8, 0x48, 1, 0,0,0x07,0xA3 },  //AC系统
	{0x80, 3, 11, 0x01, 0xE8, 0x48, 1, 0,0,0x07,0xA5 },  //BCM系统
	{0x80, 6, 14, 0x07, 0xA1, 0x20, 1, 0,0,0x07,0xE8 },  //EMS系统
	{0x80, 6, 14, 0x07, 0xA1, 0x20, 1, 0,0,0x07,0x95 },  //EPS系统
	{0x80, 6, 14, 0x07, 0xA1, 0x20, 1, 0,0,0x07,0x90 },  //ESP系统
	{0x80, 3, 11, 0x01, 0xE8, 0x48, 1, 0,0,0x07,0xA0 },  //HUM系统
	{0x80, 3, 11, 0x01, 0xE8, 0x48, 1, 0,0,0x07,0xA4 },  //ICM系统
	{0x80, 3, 11, 0x01, 0xE8, 0x48, 1, 0,0,0x07,0xA1 },  //SDM系统
	{0x80, 6, 14, 0x07, 0xA1, 0x20, 1, 0,0,0x07,0xE9 },  //TCU系统
};
//*********************************************************************************
//描述:		命令列表
//引用范围:	全局
//参数描述:	缓存偏移，保留字节，命令长度，具体命令指针
//*********************************************************************************
STRUCT_CMD g_stCmdList[] =
{
	{0, 0, 10, cCmdBCM00},//00  //进入命令----BCM
	{0, 0, 10, cCmdBCM01},//01  //空闲命令
	{2, 0, 10, cCmdBCM02},//02  //读VIN
	{0, 0, 10, cCmdBCM03},//03  //30H帧设置
	{0, 0, 10, cCmdPEPS00},//04  //进入命令----PEPS
	{0, 0, 10, cCmdPEPS01},//05  //空闲命令
	{2, 0, 10, cCmdPEPS02},//06  //读VIN
	{0, 0, 10, cCmdPEPS03},//07  //30H帧设置
	{0, 0, 10, cCmdEMS00},//08  //进入命令----EMS
	{0, 0, 10, cCmdEMS01},//09  //空闲命令
	{2, 0, 10, cCmdEMS02},//0A  //读VIN
	{0, 0, 10, cCmdEMS03},//0B  //30H帧设置
	{0, 0, 10, bCmdClearDTC[0]},
	{0, 0, 10, bCmdClearDTC[1]},
	{0, 0, 10, bCmdClearDTC[2]},
	{0, 0, 10, bCmdClearDTC[3]},
	{0, 0, 10, bCmdClearDTC[4]},
	{0, 0, 10, bCmdClearDTC[5]},
	{0, 0, 10, bCmdClearDTC[6]},
	{0, 0, 10, bCmdClearDTC[7]},
	{0, 0, 10, bCmdClearDTC[8]},
	{0, 0, 10, bCmdClearDTC[9]},
	{0, 0, 10, bCmdActiveECU[0]},
	{0, 0, 10, bCmdActiveECU[1]},
	{0, 0, 10, bCmdActiveECU[2]},
	{0, 0, 10, bCmdActiveECU[3]},
	{0, 0, 10, bCmdActiveECU[4]},
	{0, 0, 10, bCmdActiveECU[5]},
	{0, 0, 10, bCmdActiveECU[6]},
	{0, 0, 10, bCmdActiveECU[7]},
	{0, 0, 10, bCmdActiveECU[8]},
	{0, 0, 10, bCmdActiveECU[9]},
};
//*********************************************************************************
//描述:	VDI配置表
//具体：通讯类型(1)、通讯脚(2)、ECU波特率(3)、滤波ID个数、ID号(4个为一组)、ID号
//      (其它配置默认，后续如果有需要再扩展)
//*********************************************************************************	
byte VDI_BCM_CANDate[] = {0x80, 3, 11, 0x01, 0xE8, 0x48, 1, 0,0,0x07,0xA5 };
byte VDI_PEPS_Date[]   = {0x80, 3, 11, 0x01, 0xE8, 0x48, 1, 0,0,0x07,0xA9 };
byte VDI_EMS_Date[]    = {0x80, 6, 14, 0x07, 0xA1, 0x20, 1, 0,0,0x07,0xE8 };
//*********************************************************************************
//描述:	激活配置表
//具体：激活类型(1)、激活命令总个数、激活命令、命令(若为CAN则此命令为30H连接帧+激活命令)、串行激活配置
//      <active_params>串行激活配置偏移(字节时间，激活方式，是否自检，ECU回复字节超时时间（uint16）波特率（3）激活地址（10）地址（5）)…
//      (其它配置默认，后续如果有需要再扩展)
//激活类型0-CAN、1-kwp；
//*********************************************************************************	
byte Active_BCMDate[]  = { 0, 1, 0, 3  };
byte Active_PEPSDate[] = { 0, 1, 4, 7  };
byte Active_EMSDate[]  = { 0, 1, 8, 11 };
byte ActiveParams_KWP[][23]={ 
	{4,4,0,0x23,0x28, 0x00, 0x28, 0xA0,0x09,0x00,0x03,0xe8,0x00,0x00,0x19,0x00,0x00,0x19, 0,0,0,0,0} //串行激活
};
//*********************************************************************************
//描述:	读VIN配置表
//具体：命令个数，读VIN命令号，退出命令号，处理起始字节…
//      (其它配置默认，后续如果有需要再扩展)
//*********************************************************************************	
byte ReadVIN_BCM[]  = { 1, 2,  3 };
byte ReadVIN_PEPS[] = { 1, 6,  3 };
byte ReadVIN_EMS[]  = { 1, 10, 3 };
//*********************************************************************************
//描述:	全局资源表
//具体：VDI配置表编号、激活配置表编号、读VIN配置表编号
//typedef struct _STRUCT_SYSTotle
//{
//	byte* pVDIData;
//	byte* pActiveData;
//	byte* pReadVIN;
//} STRUCT_SYSTotle;
//*********************************************************************************	
STRUCT_SYSTotle g_stSYSList[] =
{
	{ VDI_BCM_CANDate, Active_BCMDate, ReadVIN_BCM },//00  B40----BCM
//	{ VDI_PEPS_Date,  Active_PEPSDate, ReadVIN_PEPS },//01  B40----PEPS
	{ VDI_EMS_Date,    Active_EMSDate, ReadVIN_EMS },//01  B40----EMS
};



