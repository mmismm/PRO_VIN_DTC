/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	������Э���йص����ñ�����ȫ�ֵ���Ϣ��ʾ��
History:
	<author>	<time>		<desc>

************************************************************************/

#include "protocol_config.h"
#include <stdio.h>

/*
typedef struct _STRUCT_ISO15765_CONFIG
{
uint16	u16Code7F78Timeout;		//7F78��������ĳ�ʱʱ��
uint16	u16ECUResTimeout;		//�ȴ�ECU�ظ��ĳ�ʱʱ��
uint16	u16TimeBetweenFrames;	//��������ʱ��֡���
byte	cRetransTime;			//�ط�����
byte	cReserved;				//�����ֽڣ�����CANЭ��û��
byte	cFCCmdOffset;			//FC֡����ƫ��
} STRUCT_ISO15765_CONFIG;
*/

STRUCT_ISO15765_CONFIG *g_p_stISO15765ConfigGroup[5] = {0};
STRUCT_ISO15765_CONFIG *g_p_stISO15765Config = NULL;
/*
typedef struct _STRUCT_ISO14230_CONFIG
{
uint16	u16Code7F78Timeout;		//7F78��������ĳ�ʱʱ��
uint16	u16ECUResTimeout;		//�ȴ�ECU�ظ��ĳ�ʱʱ��
uint16	u16TimeBetweenFrames;	//��������ʱ��֡���
byte	cRetransTime;			//�ط�����
byte	cTimeBetweenBytes;		//���������ֽڼ�ʱ��
}STRUCT_ISO14230_CONFIG;
*/

STRUCT_ISO14230_CONFIG *g_p_stISO14230ConfigGroup[5] = {0};
STRUCT_ISO14230_CONFIG *g_p_stISO14230Config = NULL;



/*
typedef struct
{
byte cDtcStartOffset;	//��Ч�ظ��б���DTC��ʼƫ�ƣ���SID�ظ���ʼ
byte cDtcBytesInCmd;	//�����м����ֽڱ�ʾһ��������
byte cStatusOffset;		//������״̬��һ�������������е�ƫ��
byte cDtcBytesInDisplay;//һ����������Ҫ��ʾ�����ֽ�
byte cCurrentDtcMask;	//��ǰ������mask
byte cHistoryDtcMask;	//��ʷ������mask
}STRUCT_READ_DTC_CONFIG;
*/

STRUCT_READ_DTC_CONFIG *g_p_stGeneralReadDtcConfigGroup[5] = {0};
STRUCT_READ_DTC_CONFIG *g_p_stGeneralReadDtcConfig = NULL;

/*
typedef struct _STRUCT_UDS_FREEZE_DTC_CONFIG
{
byte cFreezeDtcStartOffset;	//��Ч�ظ��б��涳��֡DTC��ʼƫ�ƣ���SID�ظ���ʼ
byte cFreezeRecordNumberOffset;	//��Ч�ظ��б��涳��֡�洢����ʼƫ�ƣ���SID�ظ���ʼ
byte cDtcBytesInCmd;	//�����м����ֽڱ�ʾһ��������
byte cDtcBytesInDisplay;//һ����������Ҫ��ʾ�����ֽ�
byte cReadFreezeDsMode; //��ȡ����֡������ģʽ 0�����ݹ����� 1�����ݴ洢��
byte cSaveRecordNumberOffset;//�ڷ��������з��ô洢�ŵ�ƫ��
byte cRecordNumberBytes;//�洢��ռ���ֽ���
byte cSaveDtcOffset;	//�ڷ��������з��ù������ƫ��
byte cDtcBytes;			//������ռ���ֽ���
byte cFreezeDsStartOffset;	//��Ч�ظ��б��涳��֡DS��ʼƫ�ƣ���SID�ظ���ʼ��ָ��ID����
}STRUCT_UDS_FREEZE_DTC_CONFIG;
*/

STRUCT_UDS_FREEZE_DTC_CONFIG *g_p_stUDSFreezeDtcConfigGroup[5] = {0};
STRUCT_UDS_FREEZE_DTC_CONFIG *g_p_stUDSFreezeDtcConfig = NULL;

/*
typedef struct _STRUCT_UDS_FREEZE_DS_CONFIG
{
byte cDIDHighByte;			//����֡֡��DID���ֽ�
byte cDIDLowByte;			//����֡֡��DID���ֽ�
byte cNeedByteSum;			//��DID֧�ֵ���Ч�ֽ�����
byte cDSItemSum;			//��DID��Ӧ��ʾ������������
byte* pcSpecificDIDRule;	//Ҫ��ʾ������ID���ֽ�+������ID���ֽ�+��DID֧����Ч�ֽ�ȡ�ֽڵ�ƫ��+ȡ�ֽڵĸ���...
} STRUCT_UDS_FREEZE_DS_CONFIG;
*/
STRUCT_UDS_FREEZE_DS_CONFIG *g_p_stUDSFreezeDSConfigGroup[5] = {0};
STRUCT_UDS_FREEZE_DS_CONFIG *g_p_stUDSFreezeDSConfig = NULL;

/*
typedef struct
{
byte cCommunicationType;
byte cVoltage;
byte cLevel;
byte cLogic;
byte cReceivePin;
byte cSendPin;
byte cCANFrameMode;
byte cECUBaudRate[3];
byte cVCIBaudRate[3];
byte cCanSamplingConfig[4];
byte cCanFilterIDGroupNum;
byte cCanFilterID[16];
}STRUCT_VCI_PARAMS;
*/
STRUCT_VCI_PARAMS *g_p_stVciParamsGroup[5] = {0};

STRUCT_VCI_PARAMS *g_p_stVCI_params_config = NULL;
/*
typedef struct	_STRUCT_ACTIVE_ECU_CONFIG
{
	byte cTimeBetweenBytes;		//�ֽ����ֽڼ�ʱ��
	byte cActiveMode;			//���ʽ
	byte cDetectBandRateFlag;	//�Լ��־ 0x00:����Ҫ 0x01:��Ҫ
	uint16	u16ECUResByteTimeout;//�ȴ�ECU�ظ����ֽڳ�ʱʱ��
	byte cBandRate[3];			//������
	byte cActiveAddress[5];		//�����ַ��һ���ֽڱ�ʾ��ַ����
	byte cReceiveKeyByte[10];	//����ECU�ظ��Ĺؼ��ֵ�һ���ֽڱ�ʾ��ַ����
} STRUCT_ACTIVE_ECU_CONFIG;
*/

STRUCT_ACTIVE_ECU_CONFIG *g_p_stGeneralActiveEcuConfigGroup[5] = {0};
STRUCT_ACTIVE_ECU_CONFIG *g_p_stGeneralActiveEcuConfig = NULL;



/*
typedef struct _STRUCT_PROCESS_FUN_CONFIG
{
byte cActiveECUFunOffset;
byte cActuatorTestFunOffset;
byte cClearDTCFunOffset;
byte cDSFunOffset;
byte cDTCFunOffset;
byte cFreezeDTCFunOffset;
byte cFreezeDSFunOffset;
byte cInforFunOffset;
byte cQuitSystemFunOffset;

}STRUCT_PROCESS_FUN_CONFIG;
*/
STRUCT_PROCESS_FUN_CONFIG *g_p_stProcessFunConfigGroup[5] = {0};
STRUCT_PROCESS_FUN_CONFIG *g_p_stProcessFunConfig = NULL;

/*
typedef struct _STRUCT_SECURITY_ACCSEE_CONFIG
{
byte cDefaultSessionCmdID;
byte cExtendSessionCmdID;
byte cRequestSeedCmdID;
byte cSendKeyCmdID;
}STRUCT_SECURITY_ACCSEE_CONFIG;
*/

STRUCT_SECURITY_ACCSEE_CONFIG *g_p_stSecurityAccessConfigGroup[5] = {0};
STRUCT_SECURITY_ACCSEE_CONFIG *g_p_stSecurityAccessConfig = NULL;

/*
typedef struct _STRUCT_DS_FORMULA_CONFIG
{
int  iDSID;
int	 iDSCmdID;
byte cValidByteOffset;
byte cValidByteNumber;
byte cFormulaType;
byte * pcFormula;
int  iFormulaLen;
byte * pStrFormat;
} STRUCT_DS_FORMULA_CONFIG;
*/

/*
typedef struct _STRUCT_INFORMATION_GROUP_CONFIG
{
byte cDisplayItemSum;
STRUCT_DS_FORMULA_CONFIG * pstDSFormulaConfig;
}STRUCT_DS_FORMULA_GROUP_CONFIG;
*/

STRUCT_DS_FORMULA_GROUP_CONFIG *g_p_stInformationGroupConfigGroup[10] = {0};
STRUCT_DS_FORMULA_GROUP_CONFIG	*g_p_stInformationGroupConfig = NULL;

STRUCT_DS_FORMULA_GROUP_CONFIG *g_p_stFreezeDSFormulaConfig = NULL;

STRUCT_DS_FORMULA_GROUP_CONFIG g_stGeneralDSFormulaGroupConfig = {0};

/*
typedef struct _STRUCT_IDLE_LINK_CONFIG
{
byte cTimeBetweenBytes;			//�ֽڼ�ʱ��
byte cIdleLinkStatus;			//��������״̬
uint16	u16TimeBetweenGroups;	//��������ʱ������ļ��
uint16	u16TimeBetweenFrames;	//��������ʱ֡��֡�ļ��
byte cResponseFlag;				//�費��Ҫ���տ������ӻظ��ı�־ 0������Ҫ 1����Ҫ
byte cCalculateChecksumFlag;	//����У��ͱ�־ 0������Ҫ����У��� ��0����Ҫ����У���
}STRUCT_IDLE_LINK_CONFIG;
*/

STRUCT_IDLE_LINK_CONFIG *g_p_stIdleLinkConfigGroup[5] = {0};
STRUCT_IDLE_LINK_CONFIG *g_p_stIdleLinkConfig = NULL;

/*
typedef struct _STRUCT_STORAGE_BUFFER
{
int iValidLen;
byte cBuffer[512];
}STRUCT_STORAGE_BUFFER;
*/
STRUCT_STORAGE_BUFFER	g_stBufferGroup[20] = //
{
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
};

uint32 u32Config_fuc_Group[8][16] = {0};
uint32 *u32Config_fuc = NULL;


//���ص���ʾ����
const byte g_cTime_out[]		= "TIME_OUT";
const byte g_cSuccess[]			= "SUCCESS";
const byte g_cFrame_time_out[]	= "FRAME_TIME_OUT";
const byte g_cProtocol_error[]	= "PROTOCOL_ERROR";
const byte g_cFaile[]			= "FAILE";
const byte g_cFormat_error[]	= "FORMAT_ERORR";
const byte g_cNull_point[]		= "NULL_POINT";
const byte g_cNegative[]		= "NEGATIVE";
const byte g_cMatch_error[]		= "MATCH_ERROE";
const byte g_cNo_freeze_DS[]	= "NO_FREEZE_DS";
const byte g_cNo_freeze_DTC[]	= "NO_FREEZE_DTC";
const byte g_cNo_current_dtc[]	= "NO_CURRENT_DTC";
const byte g_cNo_history_dtc[]	= "NO_HISTORY_DTC";
const byte g_cReceive_frame_type_error[]	= "RECEIVE_FRAME_TYPE_ERROR";
const byte g_cMessage_type_error[]			= "MESSAGE_TYPE_ERROR";
const byte g_cInput_character_format_error[]	= "INPUT_CHARACTER_FORMAT_ERROR";
const byte g_cInput_character_number_error[]	= "INPUT_CHARACTER_NUMBER_ERROR";