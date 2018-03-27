/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	�����ȡ�������ô�����
History:
	<author>	<time>		<desc>

***********************************************************************/
#include "..\interface\protocol_define.h"
#include "..\public\protocol_config.h"
#include "init_config_from_xml_lib.h"
#include <assert.h>
#include <stdlib.h>


STRUCT_INIT_CONFIG stInitActiveECUConfigGroup[] =
{
	{GENERAL_ACTIVE_ECU_CONFIG, get_general_active_ecu_config_data},
};


/*************************************************
Description:	��ȡ��ʼ��Э�����ú���
Input:
	cConfigType		��������
Output:	����
Return:	pf_parammeter_is_point_to_uint32 ����ָ��
Others:
*************************************************/
pf_parammeter_is_point_to_uint32 get_init_active_ECU_config_fun( byte cConfigType )
{
	int i = 0;

	for( i = 0; i < sizeof( stInitActiveECUConfigGroup ) / sizeof( stInitActiveECUConfigGroup[0] ); i++ )
		if( cConfigType == stInitActiveECUConfigGroup[i].cConfigType )
			return stInitActiveECUConfigGroup[i].pFun;

	return 0;//���ڲ����
}

/*************************************************
Description:	��ȡ��������
Input:
	PIn				������������
Output:	����
Return:	��
Others: ���ݲ�ͬ������ģ�����Ӧ�ķ�֧
*************************************************/
void get_active_config_data( void* pIn )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	uint32 u32ActiveConfigTemp[50] = {0};

	byte cActiveConfigType = 0;

	pf_parammeter_is_point_to_uint32 pFun = NULL;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	get_config_data( u32ActiveConfigTemp, pstParam->pcData, NO_LENGTH_LIMIT );

	cActiveConfigType	= ( byte )u32ActiveConfigTemp[1];

	pFun = get_init_active_ECU_config_fun( cActiveConfigType );

	assert( pFun );

	pFun( u32ActiveConfigTemp );

}
/*************************************************
Description:	��ȡͨ�ü�����������
Input:
	pu32Source		������������
Output:	����
Return:	��
Others: �����Ὺ�ٿռ�����������

byte cTimeBetweenBytes;		//�ֽ����ֽڼ�ʱ��
byte cActiveMode;			//���ʽ
byte cDetectBandRateFlag;	//�Լ��־ 0x00:����Ҫ 0x01:��Ҫ
uint16	u16ECUResByteTimeout;//�ȴ�ECU�ظ����ֽڳ�ʱʱ��
byte cBandRate[3];			//������
byte cActiveAddress[5];		//�����ַ��һ���ֽڱ�ʾ��ַ����
byte cReceiveKeyByte[10];	//����ECU�ظ��Ĺؼ��ֵ�һ���ֽڱ�ʾ��ַ����
*************************************************/
void get_general_active_ecu_config_data( const uint32 * pu32Source )
{
	byte cConfigOffset = 0;
	int iSaveOffset = 0;
	int i = 0;
	UNN_2WORD_4BYTE unnTemp;
	unnTemp.u32Bit = 0;

	cConfigOffset = ( byte )pu32Source[0];

	if( g_p_stGeneralActiveEcuConfigGroup[cConfigOffset] == NULL )
		g_p_stGeneralActiveEcuConfigGroup[cConfigOffset] = ( STRUCT_ACTIVE_ECU_CONFIG * )malloc( sizeof( STRUCT_ACTIVE_ECU_CONFIG ) );

	g_p_stGeneralActiveEcuConfigGroup[cConfigOffset]->cTimeBetweenBytes		= ( byte )pu32Source[2];
	g_p_stGeneralActiveEcuConfigGroup[cConfigOffset]->cActiveMode			= ( byte )pu32Source[3];
	g_p_stGeneralActiveEcuConfigGroup[cConfigOffset]->cDetectBandRateFlag	= ( byte )pu32Source[4];
	g_p_stGeneralActiveEcuConfigGroup[cConfigOffset]->u16ECUResByteTimeout	= ( uint16 )pu32Source[5];
	unnTemp.u32Bit = pu32Source[6];
	g_p_stGeneralActiveEcuConfigGroup[cConfigOffset]->cBandRate[0]			= unnTemp.u8Bit[2];
	g_p_stGeneralActiveEcuConfigGroup[cConfigOffset]->cBandRate[1]			= unnTemp.u8Bit[1];
	g_p_stGeneralActiveEcuConfigGroup[cConfigOffset]->cBandRate[2]			= unnTemp.u8Bit[0];

	g_p_stGeneralActiveEcuConfigGroup[cConfigOffset]->cActiveAddress[0]		= ( byte )pu32Source[7];

	for( i = 0; i < ( byte )pu32Source[7]; i++ ) //���漤���ַ
	{
		g_p_stGeneralActiveEcuConfigGroup[cConfigOffset]->cActiveAddress[1 + i] = ( byte )pu32Source[8 + i];
	}

	iSaveOffset = 6 + 1 + ( byte )pu32Source[7] + 1;

	g_p_stGeneralActiveEcuConfigGroup[cConfigOffset]->cReceiveKeyByte[0]	= ( byte )pu32Source[iSaveOffset];

	for( i = 0; i < g_p_stGeneralActiveEcuConfigGroup[cConfigOffset]->cReceiveKeyByte[0]; i++ ) //�������ECU�ظ��Ĺؼ���
	{
		g_p_stGeneralActiveEcuConfigGroup[cConfigOffset]->cReceiveKeyByte[1 + i] = ( byte )pu32Source[iSaveOffset + 1 + i];
	}
}

/*************************************************
Description:	ѡ�񼤻�����
Input:
	iConfigOffset		����ƫ��
	cConfigType			��������
Output:	����
Return:	��
Others: ���ݼ����������ͺ�����ƫ��ѡ����Ӧ������
*************************************************/
void select_active_config( int iConfigOffset, const byte cConfigType )
{
	switch( cConfigType )
	{
	case GENERAL_ACTIVE_ECU_CONFIG:
		g_p_stGeneralActiveEcuConfig = g_p_stGeneralActiveEcuConfigGroup[iConfigOffset];
		break;

	default:
		break;
	}
}

/*************************************************
Description:	�ͷŴ�ż������õĿռ�
Input:	��
Output:	����
Return:	��
Others: ÿ���һ�����þ��ڸú����������Ӧ���ͷŴ���
����quit_system_lib.c��free_xml_config_space
�����е��øú�����
*************************************************/
void free_active_ecu_config_space( void )
{
	int i = 0;

	for( i = 0; i < sizeof( g_p_stGeneralActiveEcuConfigGroup ) / sizeof( g_p_stGeneralActiveEcuConfigGroup[0] ); i++ )
	{
		if( NULL != g_p_stGeneralActiveEcuConfigGroup[i] )
			free( g_p_stGeneralActiveEcuConfigGroup[i] );
	}

}
