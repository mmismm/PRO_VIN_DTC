/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	�����xml��ȡ��ʼ�����ô�����
History:
	<author>	<time>		<desc>

***********************************************************************/
#include "..\interface\protocol_define.h"
#include "init_config_from_xml_lib.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "..\public\protocol_config.h"
#include <ctype.h>


STRUCT_INIT_XML_GLOBAL_VARIABLE g_stInitXmlGobalVariable =
{
	0,
	0,
	NULL,
	0,
	NULL,
	0,
	false,
};
/*************************************************
Description:	�����xml��ȡ��������
Input:
	iConfigType		��������
	PIn				������������
Output:	����
Return:	��
Others: �����������ͽ���Ӧ��֧
*************************************************/
void process_init_config_from_xml( int iConfigType, void* pIn )
{
	switch( iConfigType )
	{
	case VCI_CONFIG:
	{
		get_vci_config_data( pIn );
	}
	break;

	case IDLE_LINK_CONFIG:
	{
		get_idle_link_config_data( pIn );
	}
	break;

	case ACTIVE_ECU_CONFIG:
	{
		get_active_config_data( pIn );
	}
	break;

	case PROTOCOL_CONFIG:
	{
		get_protocol_config_data( pIn );
	}
	break;

	case SPECIFIC_COMMAND_CONFIG:
	{
		get_specific_command_config_data( pIn );
	}
	break;

	case DTC_CONFIG:
	{
		get_dtc_config_data( pIn );
	}
	break;

	case FREEZE_DTC_CONFIG:
	{
		get_freeze_dtc_config_data( pIn );
	}
	break;

	case FREEZE_DS_CONFIG:
	{
		get_freeze_ds_config_data( pIn );
	}
	break;

	case PROCESS_FUN_CONFIG:
	{
		get_process_fun_config_data( pIn );
	}
	break;

	case SECURITY_ACCESS_CONFIG:
	{
		get_security_access_config( pIn );
	}
	break;

	case ECU_INFORMATION_FORMULA:
	{
		get_information_config( pIn );
	}
	break;

	case FREEZE_DATA_STREAM_FORMULA:
	{
		get_freeze_DS_formula_config( pIn );
	}
	break;

	case SPECIAL_CONFIG:
	{
		get_special_config( pIn );
	}
	break;



	default:
		break;
	}
}
/*************************************************
Description:	���xml�����õ�����
Input:
	pcSource	ȡ����������ĵ�ַ���ַ�����
	iMaxLen		ȡ���ݵĳ�������

Output:	pu32Dest	�����������ݵĵ�ַ
Return:	int			�������
Others:	��xml��������0X�ı�ʾ16���Ʒ�����10����
		�Ժ����ר�Ź�������������ݣ�����û�н���
		�����жϡ����� 2014-5-28 12:10:54
		�������ձ�����uint32���͵������С�
		pcSource�����ݸ�ʽ������00090x01,0xBB������0009��
		ʾ���ȣ�16���ƣ���0x01,0xBBΪxml��һ��Ԫ�ص�����
*************************************************/
int get_config_data( uint32 *pu32Dest, const byte* pcSource, int iMaxLen )
{
	byte cTemp[15] = {0};
	int num = 0;
	int i = 0, j = 0, k = 0, m = 0;
	int iScale = 16;

	if( 0 == iMaxLen )
		return iMaxLen;

	while( pcSource[i] != '\0' && i != iMaxLen )
	{
		memcpy( cTemp, &pcSource[i], 4 );
		cTemp[4] = '\0';
		iScale = 16;
		num = strtol( cTemp, NULL, iScale );
		i += 4;
		k = 0;

		if( 0 == num )
			continue;

		if( num < 3 )
		{
			iScale = 10;

			memcpy( cTemp, &pcSource[i], num );

			cTemp[num] = '\0';

			pu32Dest[m] = strtol( cTemp, NULL, iScale );
			m += 1;

			i += num;

			continue;
		}

		for( j = 0; j < num + 1; j++ )
		{
			if( isspace( pcSource[i + j] ) ) //����ո񡢻��С��Ʊ����
				continue;

			if( ',' == pcSource[i + j] )
			{
				cTemp[k] = '\0';
				pu32Dest[m] = strtol( cTemp, NULL, iScale );
				m += 1;
				k = 0;
				continue;
			}
			else if( j == num )
			{
				cTemp[k] = '\0';
				pu32Dest[m] = strtol( cTemp, NULL, iScale );
				m += 1;
				k = 0;
				break;
			}

			cTemp[k] = pcSource[i + j];

			if( 1 == k ) //��xml��0��x�м��пո���ʵ�Ǵ���ģ�������������ݴ�
			{
				if( cTemp[k] == 'x' || cTemp[k] == 'X' )
					iScale = 16;
				else
					iScale = 10;
			}

			k += 1;

		}

		i += num;
	}

	return m;

}

/*************************************************
Description:	���xml��commandһ��������������
Input:
	pcSource	ȡ����������ĵ�ַ���ַ�����

Output:	pu32Dest	�����������ݵĵ�ַ
Return:	int			���������������ص��ַ�����
Others:
pcSource�����ݸ�ʽ������00090x01,0xBB������0009��
ʾ���ȣ�16���ƣ���0x01,0xBBΪxml��һ��Ԫ�ص�����
*************************************************/
int get_command_one_block_config_data( byte *pcDest, const byte* pcSource )
{
	byte cTemp[15] = {0};
	int iLen = 0;

	memcpy( cTemp, pcSource, 4 );
	cTemp[4] = '\0';

	iLen = 4;
	iLen += strtol( cTemp, NULL, 16 );

	if( 4 == iLen ) //����Ϊ4��ʾû������
		return iLen;

	get_cmd_config_content_data( pcDest , pcSource, iLen );

	return iLen;
}

/*************************************************
Description:	���xml������cmmand�о�����������
Input:
	pcSource	ȡ����������ĵ�ַ���ַ�����
	iMaxLen		ȡ���ݵĳ�������

Output:	pu32Dest	�����������ݵĵ�ַ
Return:	int			�������
Others: �������ձ�����byte���͵�������
*************************************************/
int get_cmd_config_content_data( byte *pcDest, const byte* pcSource, int iMaxLen )
{
	byte cTemp[256] = {0};
	int num = 0;
	int i = 0, j = 0, k = 0, m = 0;
	int iScale = 16;

	if( 0 == iMaxLen )
		return iMaxLen;

	while( pcSource[i] != '\0' && i != iMaxLen )
	{
		memcpy( cTemp, &pcSource[i], 4 );
		cTemp[4] = '\0';
		iScale = 16;

		num = strtol( cTemp, NULL, iScale );
		i += 4;
		k = 0;

		if( 0 == num )
			continue;

		if( num < 3 )
		{
			iScale = 10;

			memcpy( cTemp, &pcSource[i], num );

			cTemp[num] = '\0';

			pcDest[m] = ( byte )strtol( cTemp, NULL, iScale );
			m += 1;

			i += num;

			continue;
		}

		for( j = 0; j < num + 1; j++ )
		{
			if( isspace( pcSource[i + j] ) ) //����ո񡢻��С��Ʊ����
				continue;

			if( ',' == pcSource[i + j] )
			{
				cTemp[k] = '\0';
				pcDest[m] = ( byte )strtol( cTemp, NULL, iScale );
				m += 1;
				k = 0;
				continue;
			}
			else if( j == num )
			{
				cTemp[k] = '\0';
				pcDest[m] = ( byte )strtol( cTemp, NULL, iScale );
				m += 1;
				k = 0;
				break;
			}

			cTemp[k] = pcSource[i + j];

			if( 1 == k ) //��xml��0��x�м��пո���ʵ�Ǵ���ģ�������������ݴ�
			{
				if( cTemp[k] == 'x' || cTemp[k] == 'X' )
					iScale = 16;
				else
					iScale = 10;
			}

			k += 1;

		}

		i += num;
	}

	return m;

}
