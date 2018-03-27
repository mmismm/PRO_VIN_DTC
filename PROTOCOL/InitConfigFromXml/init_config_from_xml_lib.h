/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	������xml��ȡ��ʼ�����ô�����
History:
	<author>	<time>		<desc>

***********************************************************************/
#ifndef _INIT_CONFIG_FROM_XML_LIB
#define _INIT_CONFIG_FROM_XML_LIB

#include "..\interface\protocol_define.h"

typedef void ( *pf_parammeter_is_point_to_uint32 )( const uint32 * );
typedef void ( *pf_parammeter_is_point_to_byte )( const byte * );

typedef struct _STRUCT_INIT_CONFIG
{
	byte cConfigType;
	pf_parammeter_is_point_to_uint32 pFun;
} STRUCT_INIT_CONFIG;

typedef struct _STRUCT_INIT_DTC_CONFIG
{
	byte cDTCConfigType;
	pf_parammeter_is_point_to_byte pFun;
} STRUCT_INIT_DTC_CONFIG;

typedef struct _STRUCT_INIT_XML_GLOBAL_VARIABLE
{
	byte m_cProtocolConfigType;				//Э����������
	byte m_cFreezeDSDIDSum;					//����֡������DID����
	STRUCT_CMD * m_p_stCmdList;				//�����б�
	int m_iSpecificCmdGetFromXmlSum;		//��xml��ȡ�ľ�����������
	STRUCT_CMD * m_p_stIdleLinkCmdList;		//�������������б�
	int m_iIdleLinkCmdGetFromXmlSum;		//��xml��ȡ�Ŀ���������������
	bool m_bKeepIdleLinkByProtocol;			//�Ƿ���protocol���ֿ������ӱ�־
} STRUCT_INIT_XML_GLOBAL_VARIABLE;

enum GET_CONFIG_DATA
{
    NO_LENGTH_LIMIT = -1,
};

enum CONFIG_TYPE
{
    VCI_CONFIG = 0,
    IDLE_LINK_CONFIG,
    ACTIVE_ECU_CONFIG,
    PROTOCOL_CONFIG,
    SPECIFIC_COMMAND_CONFIG,
    DTC_CONFIG,
    FREEZE_DTC_CONFIG,
    FREEZE_DS_CONFIG,
    PROCESS_FUN_CONFIG,
    SECURITY_ACCESS_CONFIG,
    ECU_INFORMATION_FORMULA,
    FREEZE_DATA_STREAM_FORMULA,
    SPECIAL_CONFIG,
};

enum IDLE_LINK_CONFIG_TYPE
{
    GENERAL_IDLE_LINK_CONFIG = 0,
};

enum ACTIVE_ECU_CONFIG_TYPE
{
    GENERAL_ACTIVE_ECU_CONFIG = 0,
};

enum PROTOCOL_CONFIG_TYPE
{
    ISO15765_CONFIG = 0,
    ISO14230_CONFIG,
};

enum DTC_CONFIG_TYPE
{
    GENERAL_DTC_CONFIG = 0,
};

enum FREEZE_DTC_CONFIG_TYPE
{
    UDS_FREEZE_DTC_CONFIG = 0,
};

enum FREEZE_DS_CONFIG_TYPE
{
    UDS_FREEZE_DS_CONFIG = 0,
};

enum PROCESS_FUN_CONFIG_TYPE
{
    GENERAL_PROCESS_FUN_CONFIG = 0,
};

enum SECURITY_ACCESS_CONFIG_TYPE
{
    GENERAL_SECURITY_ACCESS_CONFIG = 0,
};

enum SPECIAL_CONFIG_TYPE
{
    FC_FRAME_CONFIG = 0,
};




extern STRUCT_INIT_XML_GLOBAL_VARIABLE g_stInitXmlGobalVariable;

void process_init_config_from_xml( int iConfigType, void* pIn );
int get_config_data( uint32 *pu32Dest, const byte* pcSource, int iMaxLen );
int get_command_one_block_config_data( byte *pcDest, const byte* pcSource );
int get_cmd_config_content_data( byte *pcDest, const byte* pcSource, int iMaxLen );

void get_vci_config_data( void* pIn );
void free_vci_config_space( void );

void get_active_config_data( void* pIn );
void get_general_active_ecu_config_data( const uint32 * pu32Source );
void select_active_config( int iActiveConfigOffset, const byte cActiveConfigType );
void free_active_ecu_config_space( void );

void get_protocol_config_data( void* pIn );
void get_ISO15765_config_data( const uint32 * pu32Source );
void get_ISO14230_config_data( const uint32 * pu32Source );
void select_protocol_config( int iProtocolConfigOffset, const byte cProtocolConfigType );
void free_protocol_config_space( void );

int get_specific_command_config_data( void* pIn );
int get_command_config_data( void* pIn, STRUCT_CMD ** ppstCmd );
void free_specific_command_config_space( void );

void get_dtc_config_data( void* pIn );
void get_general_dtc_config_data( const byte * pcSource );
void select_dtc_config( int iConfigOffset, const byte cConfigType );
void free_read_dtc_config_space( void );

void get_freeze_dtc_config_data( void* pIn );
void get_UDS_freeze_dtc_config_data( const byte * pcSource );
void select_freeze_dtc_config( int iConfigOffset, const byte cConfigType );
void free_freeze_dtc_config_space( void );

void get_freeze_ds_config_data( void* pIn );
void get_UDS_freeze_ds_config_data( byte cConfigID, byte * pcSource );
void select_freeze_ds_config( int iConfigOffset, const byte cConfigType );
void free_freeze_ds_config_space( void );

void get_process_fun_config_data( void* pIn );
void get_general_process_fun_config_data( const uint32 * pu32Source );
void select_process_fun_config( int iConfigOffset, const byte cConfigType );
void free_process_fun_config_space( void );

void get_security_access_config( void* pIn );
void get_general_security_access_config_data( const uint32 * pu32Source );
void init_security_access_config( int iConfigOffset, const byte cConfigType );
void free_security_access_config_space( void );

void get_information_config( void* pIn );
void free_information_config_space( void );

void get_freeze_DS_formula_config( void* pIn );
void free_freeze_DS_formula_config_space( void );

void get_idle_link_config_data( void* pIn );
void get_general_idle_link_config_data( const uint32 * pu32Source );
void select_idle_link_config( int iConfigOffset, const byte cConfigType );
void free_idle_link_config_space( void );


void get_special_config( void* pIn );



#endif