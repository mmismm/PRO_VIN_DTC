/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:
	1.定义获取故障码状态函数；
	2.定义处理版本信息显示方式函数；
	3.定义处理普通数据流计算函数；
	4.定义处理冻结帧数据流计算函数；
	5.定义处理安全进入算法函数；

History:
	<author>	<time>		<desc>
************************************************************************/

#include <string.h>
#include "formula_comply.h"
#include "formula.h"
#include "../command/command.h"
#include "../public/public.h"
#include "../InitConfigFromXml/init_config_from_xml_lib.h"


//故障码各状态位为1的显示
byte *DTCStatusBitTrueArry[] =
{
	"ID_STR_DTC_STATUS_BIT0_TRUE",
	"ID_STR_DTC_STATUS_BIT1_TRUE",
	"ID_STR_DTC_STATUS_BIT2_TRUE",
	"ID_STR_DTC_STATUS_BIT3_TRUE",
	"ID_STR_DTC_STATUS_BIT4_TRUE",
	"ID_STR_DTC_STATUS_BIT5_TRUE",
	"ID_STR_DTC_STATUS_BIT6_TRUE",
	"ID_STR_DTC_STATUS_BIT7_TRUE",
};
//故障码各状态位为0的显示
byte *DTCStatusBitFalseArry[] =
{
	"ID_STR_DTC_STATUS_BIT0_FALSE",
	"ID_STR_DTC_STATUS_BIT1_FALSE",
	"ID_STR_DTC_STATUS_BIT2_FALSE",
	"ID_STR_DTC_STATUS_BIT3_FALSE",
	"ID_STR_DTC_STATUS_BIT4_FALSE",
	"ID_STR_DTC_STATUS_BIT5_FALSE",
	"ID_STR_DTC_STATUS_BIT6_FALSE",
	"ID_STR_DTC_STATUS_BIT7_FALSE",
};
/*************************************************
Description:	获得故障码状态
Input:
	cDctStatusData	故障码状态字节
	cDtcMask		故障码mask值

Output:	pcOut	结果输出地址
Return:	int		该故障码支持的状态个数
Others:
*************************************************/

int get_Dtc_status( byte cDctStatusData, byte *pcOut, byte cDtcMask )
{
	int i = 0;
	int iSupportStatusCounter = 0;//支持的状态计数
	byte temp_Status = 0;
	byte temp_SupportStatus = 0;
	bool bFirst = true;

	while( i < 8 )
	{
		temp_SupportStatus = ( ( cDtcMask >> i ) & 0x01 );
		temp_Status = ( ( cDctStatusData >> i ) & 0x01 );

		if( 0x01 == temp_SupportStatus )//位为1
		{
			if( i > 0 && !bFirst )
			{
				*pcOut = ',';
				pcOut++;
			}

			bFirst = true;//第一次进来置为真

			if( 0x01 == temp_Status )
			{
				memcpy( pcOut, DTCStatusBitTrueArry[i], strlen( DTCStatusBitTrueArry[i] ) );
				pcOut += strlen( DTCStatusBitTrueArry[i] );

			}
			else//位为0
			{
				memcpy( pcOut, DTCStatusBitFalseArry[i], strlen( DTCStatusBitFalseArry[i] ) );
				pcOut += strlen( DTCStatusBitFalseArry[i] );

			}

			bFirst = false;//置为假


			iSupportStatusCounter++;
		}

		i++;
	}

	*pcOut = '\0';

	return iSupportStatusCounter;
}


/*************************************************
Description:	处理版本信息显示格式
Input:
	pcSource	取值地址
	cIvalidLen	有效长度
	cStyle		显示方式

Output:	pcOut	结果输出地址
Return:	void
Others:
*************************************************/

void process_inform_format( const byte* pcSource, byte cIvalidLen, byte cStyle, byte* pcOut )
{
	switch( cStyle )
	{
	case 'A'://ASCII码方式处理
		DisASCII( pcSource, cIvalidLen, pcOut );
		break;

	case 'H':
	case 'B':
		DisHex( pcSource, cIvalidLen, pcOut );
		break;

	case 'D':
		break;

	default:
		break;
	}
}
/************************************************************************/
/* 下面是数据流字符串公式用到的                                         */
/************************************************************************/
/*************************************************
typedef struct _STRUCT_STRING_UNIT
{
	byte cCompareData;		//要比较的数据
	byte* pcOutputString;	//要输出的字符串
} STRUCT_STRING_UNIT;
*************************************************/

STRUCT_STRING_UNIT stStringUintArray000[] =
{
	0x00, "ID_STR_DS_CSTRING_000",//关	Off
	0x01, "ID_STR_DS_CSTRING_001",//开	On
};

STRUCT_STRING_UNIT stStringUintArray001[] =
{
	0x00, "ID_STR_DS_CSTRING_002",//Not Present
	0x01, "ID_STR_DS_CSTRING_003",//Present
};

STRUCT_STRING_UNIT stStringUintArray002[] =
{
	0x00, "ID_STR_DS_CSTRING_004",//Switch NOT in Light Off position
	0x01, "ID_STR_DS_CSTRING_005",//Switch in Light Off position
};

STRUCT_STRING_UNIT stStringUintArray003[] =
{
	0x00, "ID_STR_DS_CSTRING_006",//In park position
	0x01, "ID_STR_DS_CSTRING_007",//Not in park position
};

STRUCT_STRING_UNIT stStringUintArray004[] =
{
	0x00, "ID_STR_DS_CSTRING_008",//Not requested
	0x01, "ID_STR_DS_CSTRING_009",//Requested
};

STRUCT_STRING_UNIT stStringUintArray005[] =
{
	0x00, "ID_STR_DS_CSTRING_010",//Closed
	0x01, "ID_STR_DS_CSTRING_011",//Open
};

STRUCT_STRING_UNIT stStringUintArray006[] =
{
	0x00, "ID_STR_DS_CSTRING_012",//Inactive
	0x01, "ID_STR_DS_CSTRING_013",//Active
};

STRUCT_STRING_UNIT stStringUintArray007[] =
{
	0x00, "ID_STR_DS_CSTRING_014",//Low
	0x01, "ID_STR_DS_CSTRING_015",//High
};

STRUCT_STRING_UNIT stStringUintArray008[] =
{
	0x00, "ID_STR_DS_CSTRING_016",//NO
	0x01, "ID_STR_DS_CSTRING_017",//YES
};

STRUCT_STRING_UNIT stStringUintArray009[] =
{
	0x00, "ID_STR_DS_CSTRING_018",//C60F
	0x01, "ID_STR_DS_CSTRING_019",//C70G
};

STRUCT_STRING_UNIT stStringUintArray010[] =
{
	0x00, "ID_STR_DS_CSTRING_020",//Not Configured
	0x01, "ID_STR_DS_CSTRING_021",//Configured
};

STRUCT_STRING_UNIT stStringUintArray011[] =
{
	0x00, "ID_STR_DS_CSTRING_022",//No fault
	0x01, "ID_STR_DS_CSTRING_023",//Fault
};

STRUCT_STRING_UNIT stStringUintArray012[] =
{
	0x00, "ID_STR_DS_CSTRING_024",//Valid Signal
	0x01, "ID_STR_DS_CSTRING_025",//Invalid Signal
};

/*************************************************
typedef struct _STRUCT_DIS_STRING
{
byte cCompareDataSum;					//需要比较数据个数
byte* pcDefaultOutputString;			//不满足要求时输出的字符串
STRUCT_STRING_UNIT *pstStringUnitGroup;	//定义在上面
} STRUCT_DIS_STRING;
*************************************************/
STRUCT_DIS_STRING stDisStringArray[] =
{
	{0x02, "----", stStringUintArray000}, //00  bit.0=1:开 bit.0=0:关
	{0x02, "----", stStringUintArray001}, //01  ACC.0=1~Present   ACC.0=0~Not Present
	{0x02, "----", stStringUintArray002}, //02  ACC.0=1~Switch in Light Off position  ACC.0=0~Switch NOT in Light Off position
	{0x02, "----", stStringUintArray003}, //03  ACC.4=1~Not in park position     ACC.4=0~In park position
	{0x02, "----", stStringUintArray004}, //04  ACC.0=1~Requested      ACC.0=0~Not requested
	{0x02, "----", stStringUintArray005}, //05  ACC.2=1~Open           ACC.2=0~Closed
	{0x02, "----", stStringUintArray006}, //06  ACC.0=1~Active           ACC.0=0~Inactive
	{0x02, "----", stStringUintArray007}, //07  ACC.4=1~High        ACC.4=0~Low
	{0x02, "----", stStringUintArray008}, //08  ACC.0=1~YES        ACC.0=0~NO
	{0x02, "----", stStringUintArray009}, //09  ACC.1=1~C70G        ACC.1=0~C60F
	{0x02, "----", stStringUintArray010}, //10  ACC.0=1~Configured     ACC.0=0~Not Configured
	{0x02, "----", stStringUintArray011}, //11  ACC.0=1~Fault          ACC.0=0~No fault
	{0x02, "----", stStringUintArray012}, //12  ACC.1=1~Invalid Signal     ACC.1=0~Valid Signal
};
/*************************************************
Description:	根据数据流ID处理数据流计算公式
Input:
	iDsId		数据流项ID
	pcDsSource	取值地址

Output:	pcDsValueOut	结果输出地址
Return:	void
Others:
DisplayString(pcDsSource,stDisStringArraypcDsSource,stDisStringArray,0,0xff,0,pcDsValueOut);
*************************************************/

void process_normal_ds_calculate( int iDsId, const byte* pcDsSource, byte* pcDsValueOut )
{
	switch( iDsId )
	{
	case   0:
		OneByteOperation( pcDsSource, 0, 0, 1, 10, "%4.1f", pcDsValueOut ); //x/10
		break;

	case   1:
		OneByteOperation( pcDsSource, 0, 0, 1, 10, "%4.1f", pcDsValueOut ); //x/10
		break;

	case   2:
		DisplayString( pcDsSource, stDisStringArray, 1, 0X04, 2, pcDsValueOut ); //ACC.2=1~Present   ACC.0=2~Not Present
		break;

	case   3:
		DisplayString( pcDsSource, stDisStringArray, 1, 0X08, 3, pcDsValueOut ); //ACC.3=1~Present   ACC.0=3~Not Present
		break;

	case   4:
		DisplayString( pcDsSource, stDisStringArray, 2, 0X01, 0, pcDsValueOut ); //ACC.0=1~Switch in Light Off position  ACC.0=0~Switch NOT in Light Off position
		break;

	case   5:
	case   13:
	case   32:
	case   39:
	case   56:
		DisplayString( pcDsSource, stDisStringArray, 0, 0X02, 1, pcDsValueOut ); //ACC.1=1~ON   ACC.1=0~OFF
		break;

	case   6:
	case  33:
	case  40:
	case  51:
	case  57:
		DisplayString( pcDsSource, stDisStringArray, 0, 0X04, 2, pcDsValueOut ); //ACC.2=1~ON   ACC.2=0~OFF
		break;

	case   7:
	case  34:
	case  41:
	case  52:
		DisplayString( pcDsSource, stDisStringArray, 0, 0X08, 3, pcDsValueOut ); //ACC.3=1~ON   ACC.3=0~OFF
		break;

	case   8:
	case  42:
	case  53:
	case  58:
		DisplayString( pcDsSource, stDisStringArray, 0, 0X10, 4, pcDsValueOut ); //ACC.4=1~ON   ACC.4=0~OFF
		break;

	case   9:
	case  36:
	case  43:
	case  54:
	case  59:
		DisplayString( pcDsSource, stDisStringArray, 0, 0X20, 5, pcDsValueOut ); //ACC.5=1~ON   ACC.5=0~OFF
		break;

	case   10:
	case   37:
	case   44:
	case   60:
		DisplayString( pcDsSource, stDisStringArray, 0, 0X40, 6, pcDsValueOut ); //ACC.6=1~ON   ACC.6=0~OFF
		break;

	case   11:
	case   38:
	case   45:
		DisplayString( pcDsSource, stDisStringArray, 0, 0X80, 7, pcDsValueOut ); //ACC.7=1~ON   ACC.7=0~OFF
		break;

	case   12:
	case   31:
	case   50:
	case   55:
		DisplayString( pcDsSource, stDisStringArray, 0, 0X01, 0, pcDsValueOut ); //ACC.0=1~ON   ACC.0=0~OFF
		break;

	case   14:
		DisplayString( pcDsSource, stDisStringArray, 3, 0X10, 4, pcDsValueOut ); //ACC.4=1~Not in park position     ACC.4=0~In park position
		break;

	case   15:
		OneByteOperation( pcDsSource, 0, 0, 1, 10, "%4.1f", pcDsValueOut ); //x/10
		break;

	case   16:
		OneByteOperation( pcDsSource, 0, 0, 1, 1, "%4.1f", pcDsValueOut ); //x 根据新协议修改 2013-4-8 13:27:19
		break;

	case   17:
		DisplayString( pcDsSource, stDisStringArray, 4, 0X01, 0, pcDsValueOut ); //ACC.0=1~Requested      ACC.0=0~Not requested
		break;

	case   18:
		DisplayString( pcDsSource, stDisStringArray, 4, 0X02, 1, pcDsValueOut ); //ACC.1=1~Requested      ACC.0=1~Not requested
		break;

	case   19:
	case   25:
		DisplayString( pcDsSource, stDisStringArray, 5, 0X04, 2, pcDsValueOut ); //ACC.2=1~Open           ACC.2=0~Closed
		break;

	case   20:
	case   28:
		DisplayString( pcDsSource, stDisStringArray, 5, 0X20, 5, pcDsValueOut ); //ACC.5=1~Open           ACC.5=0~Closed
		break;

	case   21:
		DisplayString( pcDsSource, stDisStringArray, 4, 0X40, 6, pcDsValueOut ); //ACC.6=1~Requested      ACC.0=6~Not requested
		break;

	case   22:
		DisplayString( pcDsSource, stDisStringArray, 4, 0X80, 7, pcDsValueOut ); //ACC.7=1~Requested      ACC.0=7~Not requested
		break;

	case   23:
	case   46:
		DisplayString( pcDsSource, stDisStringArray, 6, 0X01, 0, pcDsValueOut ); //ACC.0=1~Active           ACC.0=0~Inactive
		break;

	case   24:
		DisplayString( pcDsSource, stDisStringArray, 5, 0X02, 1, pcDsValueOut ); //ACC.1=1~Open           ACC.1=0~Closed
		break;

	case   26:
		DisplayString( pcDsSource, stDisStringArray, 5, 0X08, 3, pcDsValueOut ); //ACC.3=1~Open           ACC.3=0~Closed
		break;

	case   27:
		DisplayString( pcDsSource, stDisStringArray, 5, 0X10, 4, pcDsValueOut ); //ACC.4=1~Open           ACC.4=0~Closed
		break;

	case   29:
		DisplayString( pcDsSource, stDisStringArray, 5, 0X40, 6, pcDsValueOut ); //ACC.6=1~Open           ACC.6=0~Closed
		break;

	case   30:
	case   69:
	case   71:
	case   93:
		OneByteOperation( pcDsSource, 0, 0, 1, 1, "%4.1f", pcDsValueOut ); //x
		break;

	case   35:
		DisplayString( pcDsSource, stDisStringArray, 7, 0X10, 4, pcDsValueOut ); //ACC.4=1~High        ACC.4=0~Low
		break;

	case   47:
		DisplayString( pcDsSource, stDisStringArray, 6, 0X04, 2, pcDsValueOut ); //ACC.2=1~Active           ACC.2=0~Inactive
		break;

	case   48:
		DisplayString( pcDsSource, stDisStringArray, 6, 0X08, 3, pcDsValueOut ); //ACC.3=1~Active           ACC.3=0~Inactive
		break;

	case   49:
		DisplayString( pcDsSource, stDisStringArray, 6, 0X10, 4, pcDsValueOut ); //ACC.4=1~Active           ACC.4=0~Inactive
		break;

	case   61:
		DisplayString( pcDsSource, stDisStringArray, 8, 0X01, 0, pcDsValueOut ); //ACC.0=1~YES        ACC.0=0~NO
		break;

	case   62:
		DisplayString( pcDsSource, stDisStringArray, 9, 0X02, 1, pcDsValueOut ); //ACC.1=1~C70G        ACC.1=0~C60F
		break;

	case   63:
		DisplayString( pcDsSource, stDisStringArray, 8, 0X04, 2, pcDsValueOut ); //ACC.2=1~YES        ACC.2=0~NO
		break;

	case   64:
		DisplayString( pcDsSource, stDisStringArray, 8, 0X08, 3, pcDsValueOut ); //ACC.3=1~YES        ACC.3=0~NO
		break;

	case   65:
		DisplayString( pcDsSource, stDisStringArray, 8, 0X10, 4, pcDsValueOut ); //ACC.4=1~YES        ACC.4=0~NO
		break;

	case   66:
		DisplayString( pcDsSource, stDisStringArray, 8, 0X20, 5, pcDsValueOut ); //ACC.5=1~YES        ACC.5=0~NO
		break;

	case   67:
		DisplayString( pcDsSource, stDisStringArray, 8, 0X40, 6, pcDsValueOut ); //ACC.6=1~YES        ACC.6=0~NO
		break;

	case   68:
		DisplayString( pcDsSource, stDisStringArray, 8, 0X80, 7, pcDsValueOut ); //ACC.7=1~YES        ACC.7=0~NO
		break;

	case   70:
		TwoByteOperation( pcDsSource, 0, 0, 1, 10, 1, "%5.1f", pcDsValueOut ); // YX/10
		break;

	case   72:
	case   80:
	case   88:
		DisplayString( pcDsSource, stDisStringArray, 10, 0X01, 0, pcDsValueOut ); //ACC.0=1~Configured     ACC.0=0~Not Configured
		break;

	case   73:
	case   81:
		DisplayString( pcDsSource, stDisStringArray, 10, 0X02, 1, pcDsValueOut ); //ACC.1=1~Configured     ACC.0=1~Not Configured
		break;

	case   74:
	case   82:
		DisplayString( pcDsSource, stDisStringArray, 10, 0X04, 2, pcDsValueOut ); //ACC.2=1~Configured     ACC.2=0~Not Configured
		break;

	case   75:
	case   83:
		DisplayString( pcDsSource, stDisStringArray, 10, 0X08, 3, pcDsValueOut ); //ACC.3=1~Configured     ACC.3=0~Not Configured
		break;

	case   76:
	case   84:
		DisplayString( pcDsSource, stDisStringArray, 10, 0X10, 4, pcDsValueOut ); //ACC.4=1~Configured     ACC.4=0~Not Configured
		break;

	case   77:
	case   85:
		DisplayString( pcDsSource, stDisStringArray, 10, 0X20, 5, pcDsValueOut ); //ACC.5=1~Configured     ACC.5=0~Not Configured
		break;

	case   78:
	case   86:
		DisplayString( pcDsSource, stDisStringArray, 10, 0X40, 6, pcDsValueOut ); //ACC.6=1~Configured     ACC.6=0~Not Configured
		break;

	case   79:
	case   87:
		DisplayString( pcDsSource, stDisStringArray, 10, 0X80, 7, pcDsValueOut ); //ACC.7=1~Configured     ACC.7=0~Not Configured
		break;

	case   89:
		DisplayString( pcDsSource, stDisStringArray, 11, 0X01, 0, pcDsValueOut ); //ACC.0=1~Fault          ACC.0=0~No fault
		break;

	case   90:
		DisplayString( pcDsSource, stDisStringArray, 11, 0X02, 1, pcDsValueOut ); //ACC.1=1~Fault          ACC.1=0~No fault
		break;

	case   91:
		DisplayString( pcDsSource, stDisStringArray, 12, 0X02, 1, pcDsValueOut ); //ACC.1=1~Invalid Signal     ACC.1=0~Valid Signal
		break;

	case   92:
		DisplayString( pcDsSource, stDisStringArray, 12, 0X08, 3, pcDsValueOut ); //ACC.3=1~Invalid Signal     ACC.3=0~Valid Signal
		break;

	default:
		break;
	}
}
/*************************************************
Description:	根据冻结帧数据流ID处理数据流计算公式
Input:
	iDsId		数据流项ID
	pcDsSource	取值地址

Output:	pcDsValueOut	结果输出地址
Return:	void
Others:
*************************************************/

void process_freeze_ds_calculate( int iDsId, const byte* pcDsSource, byte* pcDsValueOut )
{
	switch( iDsId )
	{
	case   0:
		DisplayString( pcDsSource, stDisStringArray, 1, 0X01, 0, pcDsValueOut ); //ACC.0=1~Present   ACC.0=0~Not Present
		break;

	case   1:
		DisplayString( pcDsSource, stDisStringArray, 1, 0X02, 1, pcDsValueOut ); //ACC.1=1~Present   ACC.0=1~Not Present
		break;

	case   2:
		DisplayString( pcDsSource, stDisStringArray, 1, 0X04, 2, pcDsValueOut ); //ACC.2=1~Present   ACC.0=2~Not Present
		break;

	case   3:
		DisplayString( pcDsSource, stDisStringArray, 1, 0X08, 3, pcDsValueOut ); //ACC.3=1~Present   ACC.0=3~Not Present
		break;

	case   4:
		OneByteOperation( pcDsSource, 0, 0, 1, 10, "%4.1f", pcDsValueOut ); //x/10
		break;

	case   5:
		TwoByteOperation( pcDsSource, 0, 0, 1, 10, 1, "%5.1f", pcDsValueOut ); // YX/10
		break;

	default:
		break;
	}
}

/************************************************************************/
/* 下面是安全进入计算公式                                               */
/************************************************************************/

/*************************************************
  Description:	计算安全算法
  Calls:	seedToKey;
  Called By:	process_security_access_algorithm;

  Input:	Group	保存ECU回复seed的首地址

  Output:	Group	结果输出地址
  Return:	byte	key占用字节数
  Others:	函数定义会随系统而异
*************************************************/
byte security_calculate( byte *Group )
{
	uint32 Learmask = 0;
	UNN_2WORD_4BYTE Seed;
	uint8 Cnt = 0;
	Learmask = 0x5827A3B6;//0XA2159E16

	for( ; Cnt < 4; ++Cnt )
	{
		Seed.u8Bit[3 - Cnt]	= *( Group + Cnt );
	}

	if( Seed.u32Bit != 0 )
	{
		for( Cnt = 0; Cnt < 35; ++Cnt )
		{
			if( Seed.u32Bit & 0x80000000 )
			{
				Seed.u32Bit <<= 1;
				Seed.u32Bit ^= Learmask;
			}
			else
			{
				Seed.u32Bit <<= 1;
			}
		}
	}

	for( Cnt = 0; Cnt < 4; ++Cnt )
	{
		*( Group + Cnt ) = Seed.u8Bit[3 - Cnt];
	}

	return 4;

}
/*************************************************
Description:	根据安全等级处理安全算法
Input:	cAccessLevel	安全等级

Output:	pOut	结果输出地址
Return:	bool	算法执行状态（成功、失败）
Others:	函数具体实现会因系统而异
*************************************************/

bool process_security_access_algorithm( byte cAccessLevel, void* pOut )
{
	bool bProcessSingleCmdStatus = false;
	byte cBufferOffset = 0;//缓存偏移
	byte cRequestSeedCmdOffset = 0;
	byte cSendKeyCmdOffset = 0;

	byte cDataArray[10] = {0};
	byte cNeedBytes = 0;

	//根据安全等级确定命令偏移
	switch( cAccessLevel )
	{
	case 0:
		cRequestSeedCmdOffset	= g_iRequestSeedCmdIndex[1];
		cSendKeyCmdOffset		= g_iSendKeyCmdIndex[1];
		break;

	case 1:
		cRequestSeedCmdOffset	= g_iActiveECURequestSeedOffset;
		cSendKeyCmdOffset		= g_iActiveECUSendKeyOffset;
		break;

	default:
		break;
	}

	cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[ cRequestSeedCmdOffset ].cBufferOffset;

	bProcessSingleCmdStatus = process_single_cmd_without_subsequent_processing( cRequestSeedCmdOffset, pOut );

	if( !bProcessSingleCmdStatus )
	{
		return false;
	}

	memcpy( cDataArray, ( g_stBufferGroup[cBufferOffset].cBuffer + 2 ), 4 );

	//根据安全等级确定计算公式
	switch( cAccessLevel )
	{
	case 0://安全等级为0
	case 1://激活时的算法
		cNeedBytes = security_calculate( cDataArray );
		break;

	default:
		break;
	}

	memcpy( ( g_stInitXmlGobalVariable.m_p_stCmdList[cSendKeyCmdOffset].pcCmd + 5 ), cDataArray, cNeedBytes );

	bProcessSingleCmdStatus = process_single_cmd_without_subsequent_processing( cSendKeyCmdOffset, pOut );

	if( !bProcessSingleCmdStatus )
	{
		return false;
	}

	return true;

}