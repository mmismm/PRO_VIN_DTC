/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	������Э���йص����ñ�����ȫ�ֵ���Ϣ��ʾ��
History:
	<author>	<time>		<desc>

************************************************************************/
#ifndef _PROTOCOL_CONFIG_H
#define _PROTOCOL_CONFIG_H

#include "../interface/protocol_define.h"
// CANЭ������ **********************************************************************************************
typedef struct _STRUCT_ISO15765_CONFIG
{
	uint16	u16Code7F78Timeout;		//7F78��������ĳ�ʱʱ��
	uint16	u16ECUResTimeout;		//�ȴ�ECU�ظ��ĳ�ʱʱ��
	uint16	u16TimeBetweenFrames;	//��������ʱ��֡���
	byte	cRetransTime;			//�ط�����
	byte	cReserved;				//�����ֽڣ�����CANЭ��û��
	byte	cMultiframestime;       //��֡֡���ʱ��
} STRUCT_ISO15765_CONFIG;

// KWPЭ������ **********************************************************************************************
typedef struct _STRUCT_ISO14230_CONFIG
{
	uint16	u16Code7F78Timeout;		//7F78��������ĳ�ʱʱ��
	uint16	u16ECUResTimeout;		//�ȴ�ECU�ظ��ĳ�ʱʱ��
	uint16	u16TimeBetweenFrames;	//��������ʱ��֡���
	byte	cRetransTime;			//�ط�����
	byte	cTimeBetweenBytes;		//���������ֽڼ�ʱ��
} STRUCT_ISO14230_CONFIG;

// ������������ *********************************************************************************************
typedef struct _STRUCT_READ_DTC_CONFIG
{
	byte cDtcStartOffset;	//��Ч�ظ��б���DTC��ʼƫ�ƣ���SID�ظ���ʼ
	byte cDtcBytesInCmd;	//�����м����ֽڱ�ʾһ��������
	byte cStatusOffset;		//������״̬��һ�������������е�ƫ��
	byte cDtcBytesInDisplay;//һ����������Ҫ��ʾ�����ֽ�
	byte cCurrentDtcMask;	//��ǰ������mask
	byte cHistoryDtcMask;	//��ʷ������mask
} STRUCT_READ_DTC_CONFIG;

// UDS����֡���������� ***************************************************************************************
typedef struct _STRUCT_UDS_FREEZE_DTC_CONFIG
{
	byte cFreezeDtcStartOffset;		//��Ч�ظ��б��涳��֡DTC��ʼƫ�ƣ���SID�ظ���ʼ
	byte cFreezeRecordNumberOffset;	//��Ч�ظ��б��涳��֡�洢����ʼƫ�ƣ���SID�ظ���ʼ
	byte cDtcBytesInCmd;			//�����м����ֽڱ�ʾһ��������
	byte cDtcBytesInDisplay;		//һ����������Ҫ��ʾ�����ֽ�
	byte cReadFreezeDsMode;			//��ȡ����֡������ģʽ 0�����ݹ����� 1�����ݴ洢��
	byte cSaveRecordNumberOffset;	//�ڷ��������з��ô洢�ŵ�ƫ��
	byte cRecordNumberBytes;		//�洢��ռ���ֽ���
	byte cSaveDtcOffset;			//�ڷ��������з��ù������ƫ��
	byte cDtcBytes;					//������ռ���ֽ���
	byte cFreezeDsStartOffset;		//��Ч�ظ��б��涳��֡DS��ʼƫ�ƣ���SID�ظ���ʼ��ָ��ID����
} STRUCT_UDS_FREEZE_DTC_CONFIG;
// UDS����֡���������� **************************************************************************************
typedef struct _STRUCT_UDS_FREEZE_DS_CONFIG
{
	byte cDIDHighByte;			//����֡֡��DID���ֽ�
	byte cDIDLowByte;			//����֡֡��DID���ֽ�
	byte cNeedByteSum;			//��DID֧�ֵ���Ч�ֽ�����
	byte cDSItemSum;			//��DID��Ӧ��ʾ������������
	byte* pcSpecificDIDRule;	//Ҫ��ʾ������ID���ֽ�+������ID���ֽ�+��DID֧����Ч�ֽ�ȡ�ֽڵ�ƫ��+ȡ�ֽڵĸ���...
} STRUCT_UDS_FREEZE_DS_CONFIG;


// VCI���� **************************************************************************************************
typedef struct	_STRUCT_VCI_PARAMS
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
	byte cCanFilterID[40];
} STRUCT_VCI_PARAMS;
// ����ϵͳ���� *********************************************************************************************
typedef struct	_STRUCT_ACTIVE_ECU_CONFIG
{
	byte cTimeBetweenBytes;		//�ֽ����ֽڼ�ʱ��
	byte cActiveMode;			//���ʽ
	byte cDetectBandRateFlag;	//�Լ��־ 0x00:����Ҫ 0x01:��Ҫ
	uint16	u16ECUResByteTimeout;//�ȴ�ECU�ظ����ֽڳ�ʱʱ��
	byte cBandRate[3];			//������
	byte cActiveAddress[10];	//�����ַ��һ���ֽڱ�ʾ��ַ����
	byte cReceiveKeyByte[5];	//����ECU�ظ��Ĺؼ��ֵ�һ���ֽڱ�ʾ��ַ����
} STRUCT_ACTIVE_ECU_CONFIG;



// ���������� *********************************************************************************************
typedef struct _STRUCT_PROCESS_FUN_CONFIG
{
	byte cActiveECUFunOffset;
	byte cActuatorTestFunOffset;
	byte cClearDTCFunOffset;
	byte cDSFunOffset;
	byte cCurrentDTCFunOffset;
	byte cHistoryDTCFunOffset;
	byte cFreezeDTCFunOffset;
	byte cFreezeDSFunOffset;
	byte cInforFunOffset;
	byte cQuitSystemFunOffset;

} STRUCT_PROCESS_FUN_CONFIG;

// ��ȫ�������� *********************************************************************************************
typedef struct _STRUCT_SECURITY_ACCSEE_CONFIG
{
	int iDefaultSessionCmdID;
	int iExtendSessionCmdID;
	int iRequestSeedCmdID;
	int iSendKeyCmdID;
} STRUCT_SECURITY_ACCSEE_CONFIG;
// ���������㹫ʽ���� ***************************************************************************************
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
// ���������㹫ʽ������� *****************************************************************************************
typedef struct _STRUCT_DS_FORMULA_GROUP_CONFIG
{
	byte cItemSum;
	STRUCT_DS_FORMULA_CONFIG * pstDSFormulaConfig;
} STRUCT_DS_FORMULA_GROUP_CONFIG;

// �������ӵ����� *******************************************************************************************
typedef struct _STRUCT_IDLE_LINK_CONFIG
{
	byte cTimeBetweenBytes;			//�ֽڼ�ʱ��
	byte cIdleLinkStatus;			//��������״̬
	uint16	u16TimeBetweenGroups;	//��������ʱ������ļ��
	uint16	u16TimeBetweenFrames;	//��������ʱ֡��֡�ļ��
	byte cResponseFlag;				//�費��Ҫ���տ������ӻظ��ı�־ 0������Ҫ 1����Ҫ
	byte cCalculateChecksumFlag;	//����У��ͱ�־ 0������Ҫ����У��� ��0����Ҫ����У���
} STRUCT_IDLE_LINK_CONFIG;


// ȫ�ֱ�������**********************************************************************************************

/*
�洢��������,���ݴ�С����
*/

extern uint32 u32Config_fuc_Group[8][16];
extern uint32 *u32Config_fuc;


extern STRUCT_VCI_PARAMS *g_p_stVciParamsGroup[5];
extern STRUCT_VCI_PARAMS *g_p_stVCI_params_config;

extern STRUCT_ACTIVE_ECU_CONFIG *g_p_stGeneralActiveEcuConfigGroup[5];
extern STRUCT_ACTIVE_ECU_CONFIG *g_p_stGeneralActiveEcuConfig;

extern STRUCT_READ_DTC_CONFIG *g_p_stGeneralReadDtcConfigGroup[5];
extern STRUCT_READ_DTC_CONFIG *g_p_stGeneralReadDtcConfig;

extern STRUCT_ISO15765_CONFIG *g_p_stISO15765ConfigGroup[5];
extern STRUCT_ISO15765_CONFIG *g_p_stISO15765Config;

extern STRUCT_ISO14230_CONFIG *g_p_stISO14230ConfigGroup[5];
extern STRUCT_ISO14230_CONFIG *g_p_stISO14230Config;

extern STRUCT_UDS_FREEZE_DTC_CONFIG *g_p_stUDSFreezeDtcConfigGroup[5];
extern STRUCT_UDS_FREEZE_DTC_CONFIG *g_p_stUDSFreezeDtcConfig;

extern STRUCT_UDS_FREEZE_DS_CONFIG *g_p_stUDSFreezeDSConfigGroup[5];
extern STRUCT_UDS_FREEZE_DS_CONFIG *g_p_stUDSFreezeDSConfig;

extern STRUCT_PROCESS_FUN_CONFIG *g_p_stProcessFunConfigGroup[5];
extern STRUCT_PROCESS_FUN_CONFIG *g_p_stProcessFunConfig;

extern STRUCT_SECURITY_ACCSEE_CONFIG *g_p_stSecurityAccessConfigGroup[5];
extern STRUCT_SECURITY_ACCSEE_CONFIG *g_p_stSecurityAccessConfig;

extern STRUCT_DS_FORMULA_GROUP_CONFIG *g_p_stInformationGroupConfigGroup[10];
extern STRUCT_DS_FORMULA_GROUP_CONFIG *g_p_stInformationGroupConfig;

extern STRUCT_DS_FORMULA_GROUP_CONFIG *g_p_stFreezeDSFormulaConfig;

extern STRUCT_DS_FORMULA_GROUP_CONFIG g_stGeneralDSFormulaGroupConfig;

extern STRUCT_IDLE_LINK_CONFIG *g_p_stIdleLinkConfigGroup[5];
extern STRUCT_IDLE_LINK_CONFIG *g_p_stIdleLinkConfig;

extern const byte g_cTime_out[];
extern const byte g_cSuccess[];
extern const byte g_cFrame_time_out[];
extern const byte g_cProtocol_error[];
extern const byte g_cFaile[];
extern const byte g_cFormat_error[];
extern const byte g_cNull_point[];
extern const byte g_cNegative[];
extern const byte g_cMatch_error[];
extern const byte g_cNo_freeze_DS[];
extern const byte g_cNo_freeze_DTC[];
extern const byte g_cNo_current_dtc[];
extern const byte g_cNo_history_dtc[];
extern const byte g_cReceive_frame_type_error[];
extern const byte g_cMessage_type_error[];
extern const byte g_cInput_character_format_error[];
extern const byte g_cInput_character_number_error[];
#endif

