/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	声明特殊功能函数、定义相关宏和数据类型
History:
	<author>	<time>		<desc>

************************************************************************/
#ifndef _SPECIAL_FUNCTION
#define _SPECIAL_FUNCTION

#include "../interface/protocol_define.h"
#include "../command/command.h"
#include "../public/public.h"
#include "../public/protocol_config.h"
#include "../protocol/iso_14230.h"
#include "../protocol/iso_15765.h"
#include "../formula/formula.h"
#include "../formula/formula_comply.h"

#define COMM_INTERRUPT	0X02
#define PROCESS_OK		0X01
#define PROCESS_FAIL	0X00

#define HAVE_JUMP		0X04
#define NO_JUMP			0X00

#define HAVE_TIP_DATA	0X10 //提示的时候把数据域的数据显示出来
#define HAVE_TIP		0X08
#define NO_TIP			0X00

//上传数据类型
enum UPLOAD_DATA_TYPE
{
    HEX_PRINT = 0,
    DEC_PRINT,
    ORIGINAL,
    DEC_COMBIN,
};


/********************************通用函数声明**************************************/
int process_Car_CLDTC( void* pIn, byte* pOut );
void process_readVIN_function( void* pIn, void* pOut );
int get_string_type_data_to_uint32( uint32 *pu32Dest, const byte* pcSource, const int iMaxLen );
int get_string_type_data_to_byte( byte *pcDest, const byte* pcSource, const int iMaxLen );
int special_return_status( const byte cTipMode, const byte* pcLable, const byte* pcTipContent, const byte* pcData,
                           const byte cDataLen, const int iUploadDataType, const byte* pcControlsID, void* pOut );
int add_data_and_controlsID( const int iAppendDataOffset, const byte* pcData, const byte cDataLen,
                             const int iUploadDataType, const byte* pcControlsID, void* pOut );

/********************************具体功能函数声明***********************************/
bool FUN_setVDI(byte * set_VCIData);//VCI设置
bool FUN_readVIN(byte * set_VINData, byte * VINData);//读VIN命令
bool process_Active_CAN(byte * set_VCIData);//CAN激活
bool process_Active_KWP(byte * set_VCIData, byte pParams[][23]);//串行激活
int Clear_DTC_set_VDI(byte sysDTC);
int Clear_DTC_start(int sysDTC);


#endif