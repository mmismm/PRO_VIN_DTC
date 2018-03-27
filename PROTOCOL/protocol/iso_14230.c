/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	�������ISO14230Э�鴦���շ����ݵ���غ���
History:
	<author>	<time>		<desc>

************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "iso_14230.h"
#include "iso_15765.h"
#include "../interface/protocol_interface.h"
#include "../public/protocol_config.h"
#include "../command/command.h"
#include "../public/public.h"
#include "../InitConfigFromXml/init_config_from_xml_lib.h"
#include "../function/idle_link_lib.h"

/*************************************************
Description:	����ISO14230Э���շ�����
Input:	piCmdIndex	����������ַ
Output:	none
Return:	int	�շ�����ʱ��״̬
Others:	���������������Է��ͺͽ��ն������
		��ECU������Ӧ�����ݷŵ�������Ӧ�Ļ�����
ע�⣺������ECU�����Ǵ�SID�Ļظ���ʼ�ģ��磺
CMD:80 59 F1 01 81 4C RET:80 F1 59 03 C1 EA 8F 07
�Ǵ�C1��ʼ���棬��Ϊ������Ӧ���7F��ʼ���档
*************************************************/
int send_and_receive_cmd_by_iso_14230( byte* piCmdIndex )
{
	int iStatus = TIME_OUT;
	int i = 0;
	int iCmdSum = piCmdIndex[0];
	byte cReceiveBuffer[256] = {0};
	STRUCT_CMD stSendCmd = {0};

    
	for( i = 0; i < iCmdSum; i++ )
	{
		iStatus = send_and_receive_single_cmd_by_iso_14230( ( STRUCT_CMD * )&stSendCmd, piCmdIndex[1 + i], cReceiveBuffer );

		if( NULL != stSendCmd.pcCmd )
			free( stSendCmd.pcCmd );

		if( ( iStatus != SUCCESS ) && ( iStatus != NEGATIVE ) ) //���״̬�Ȳ���SUCCESS�ֲ���NEGATIVE����Ϊ����
		{
			break;
		}
        time_delay_ms( 20 );
		//time_delay_ms( g_p_stISO14230Config->u16TimeBetweenFrames );
	}

	return iStatus;
}
/*************************************************
Description:	�շ����������
Input:
	cCmdIndex		��������
	pcReceiveBuffer	���ECU�ظ����ݵĻ���

Output:	none
Return:	int	�շ�����ʱ��״̬
Others:	����ISO14230Э�鴦��
*************************************************/
int send_and_receive_single_cmd_by_iso_14230( STRUCT_CMD *pstSendCmd, const int cCmdIndex, byte* pcReceiveBuffer )
{
	int		iReceiveStaus = TIME_OUT;
	byte	cTimeBetweenBytes		= 4; //g_p_stISO14230Config->cTimeBetweenBytes;
	byte	cRetransTime			= 3 ;//g_p_stISO14230Config->cRetransTime;
	bool	bSendStatus = false;

	byte	cBufferOffset = 0;


	pstSendCmd->cBufferOffset	= g_stCmdList[cCmdIndex].cBufferOffset;
	pstSendCmd->cReserved		= g_stCmdList[cCmdIndex].cReserved;
	pstSendCmd->iCmdLen			= g_stCmdList[cCmdIndex].iCmdLen;
	pstSendCmd->pcCmd			= ( byte* )malloc( sizeof( byte ) * pstSendCmd->iCmdLen );
	memcpy( pstSendCmd->pcCmd, g_stCmdList[cCmdIndex].pcCmd, pstSendCmd->iCmdLen );

	cBufferOffset = pstSendCmd->cBufferOffset;
	pre_process_cmd( pstSendCmd ); //����У���

	while( 1 )
	{
		switch( g_p_stVCI_params_config->cCommunicationType )
		{
		case 0x00:
			bSendStatus = package_and_send_frame( FRAME_HEAD_NORMAL_FRAME, pstSendCmd, cTimeBetweenBytes );
			break;

		case 0x01:
		case 0x50:
		case 0x51:/* ��ʱ���ϴ� */
			bSendStatus = package_and_send_frame_time( FRAME_HEAD_NORMAL_FRAME, pstSendCmd, cTimeBetweenBytes );
			break;

		default:
			return PROTOCOL_ERROR;
			break;
		}


		if( !bSendStatus )
		{
			return FAILE;
		}

		iReceiveStaus = process_KWP_receive_Cmd( cBufferOffset, pcReceiveBuffer );

		switch( iReceiveStaus )
		{

		case TIME_OUT:
		{
			if( ( --cRetransTime ) == 0 )
				return iReceiveStaus;
		}
		break;

		case SUCCESS:
		case FRAME_TIME_OUT:
		case NEGATIVE:

		default:
			return iReceiveStaus;
		}
	}
}


/*************************************************
Description:	�����׼KWP�Ľ���
Input:
	cBufferOffset	�洢����ƫ��
	pcSource		����ECU�ظ����ݵĻ���

Output:	none
Return:	int	�շ�����ʱ��״̬
Others:	����ISO14230Э�鴦����գ�����
		cBufferOffset��Ӧ����g_stBufferGroup
		��pcSource�ǵ���receive_cmd����ʱ�ݴ�
		���ݵĵ�ַ��
*************************************************/
int process_KWP_receive_Cmd( const byte cBufferOffset, byte* pcSource )
{
	uint16	u16Code7F78Timeout	= 5000;//g_p_stISO14230Config->u16Code7F78Timeout;
	uint16	u16ECUResTimeout	= 5000;//g_p_stISO14230Config->u16ECUResTimeout;

	byte*	pcDestination		= g_stBufferGroup[cBufferOffset].cBuffer;

	int		iNegativeResponseCounter = 0;
	int		iValidLen = 0;
	int 	iReceiveStatus	= FAILE;

	iReceiveStatus = process_KWP_receive_single_Cmd( &pcSource, &iValidLen, u16ECUResTimeout );

	if( iReceiveStatus != SUCCESS )
	{
		return iReceiveStatus;
	}

	if( ( iValidLen == 0x03 ) && ( pcSource[0] == 0x7f ) ) //�ж�������Ӧ
	{
		if( pcSource[2] == 0x78 )
		{
			while( 1 ) //���while����������ȴ����7F78��Ŀǰ����200��
			{
				iReceiveStatus = process_KWP_receive_single_Cmd( &pcSource, &iValidLen, u16Code7F78Timeout );

				if( iReceiveStatus != SUCCESS )
				{
					return iReceiveStatus;
				}

				if( ( iValidLen == 0x03 ) && ( pcSource[0] == 0x7f ) && ( pcSource[2] == 0x78 ) )
					iNegativeResponseCounter++;
				else if( ( iValidLen == 0x03 ) && ( pcSource[0] == 0x7f ) )
				{
					memcpy( pcDestination, &pcSource[0], 3 ); //����������Ӧ����
					g_stBufferGroup[cBufferOffset].iValidLen = 3;
					return NEGATIVE;
				}
				else
					break;

				if( iNegativeResponseCounter == 200 )
					return TIME_OUT;

			}//end while

		}//end if
		else
		{
			memcpy( pcDestination, &pcSource[0], 3 ); //����������Ӧ����
			g_stBufferGroup[cBufferOffset].iValidLen = 3;
			return NEGATIVE;
		}

	}//end if

	//������Ч���ݵ�ָ���Ļ�����
	memcpy( pcDestination, pcSource, iValidLen );
	//�޸Ļ������Ч�ֽ�
	g_stBufferGroup[cBufferOffset].iValidLen = iValidLen;

	return SUCCESS;
}
/*************************************************
Description:	������KWPЭ�鷽ʽ���յ����ظ�����
Input:	u16FrameTimeout			֡��ʱʱ��

Output:	ppcSource	��Ž������ݵ�ַ��ָ��ĵ�ַ
		piValidLen	����������Ч�ֽ�

Return:	int	����ECU�ظ���״̬
Others: ���ֽ��ϴ� ���ϴ��ľ�����Ч����
        ��ʱ���ϴ��� �ϴ��ĵ�һ���ֽ�����Ч���ݵĳ��ȣ��Ժ���ŵ�����Ч����
*************************************************/
int process_KWP_receive_single_Cmd( byte** ppcSource, int* piValidLen, const uint16 u16FrameTimeout )
{
	bool	bReceiveStatus	= false;
	uint16	u16FrameContentTimeout = 5000;//g_p_stISO14230Config->u16ECUResTimeout;

	bReceiveStatus = ( bool )receive_cmd( *ppcSource, 1, u16FrameTimeout ); //���յ�һ���ֽ�
	*piValidLen = **ppcSource;

	if( !bReceiveStatus )
	{
		return TIME_OUT;
	}

	if( g_p_stGeneralActiveEcuConfig->cActiveMode == 0x06 ) /* ����1���ֽ� */
	{
		return SUCCESS;
	}

	switch( g_p_stVCI_params_config->cCommunicationType ) //����ͨ�����õ�ģʽ
	{
	case 0x01:
	case 0x50:
	case 0x51:/* ��ʱ���ϴ�,�Ƚ���һ���ֽ� */
	{
		bReceiveStatus = ( bool )receive_cmd( *ppcSource, 1, u16FrameTimeout ); //���յ�һ���ֽ�

		if( !bReceiveStatus )
		{
			return TIME_OUT;
		}
	}
	break;

	default: /* Ĭ��������� ���ֽ��ϴ� */
		break;
	}

	switch( ( *ppcSource )[0] & 0xC0 )
	{
	case 0X00://û�е�ַ��Ϣ��
	{
		*piValidLen = ( *ppcSource )[0];
		bReceiveStatus = ( bool )receive_cmd( *ppcSource, *piValidLen + 1, u16FrameContentTimeout );
	}
	break;

	case 0XC0:
	case 0X80:
	{
		if( ( ( *ppcSource )[0] != 0XC0 ) && ( ( *ppcSource )[0] != 0X80 ) ) //�Ȳ���0X80Ҳ����0XC0
		{
			*piValidLen = ( ( *ppcSource )[0] & 0X3F );
			bReceiveStatus = ( bool )receive_cmd( *ppcSource, *piValidLen + 3, u16FrameContentTimeout );

			if( !bReceiveStatus ) //�жϽ���ʣ�ಿ��ʱ��״̬
				return FRAME_TIME_OUT;

			*ppcSource += 2;//���������ֽڵĵ�ַ
		}
		else
		{
			bReceiveStatus = ( bool )receive_cmd( *ppcSource, 2 + 1, u16FrameContentTimeout ); //���������ֽڵ�ַ��һ���ֽڳ���

			if( !bReceiveStatus )
				return FRAME_TIME_OUT;

			*piValidLen = ( *ppcSource )[2];

			bReceiveStatus = ( bool )receive_cmd( *ppcSource, *piValidLen + 1, u16FrameContentTimeout ); //���ݳ����ֽڽ���

			if( !bReceiveStatus ) //�жϽ���ʣ�ಿ��ʱ��״̬
				return FRAME_TIME_OUT;
		}
	}
	break;

	case 0X40://CARBģʽ
	{
		bReceiveStatus = ( bool )receive_cmd( *ppcSource, 2, u16FrameContentTimeout ); //6B + ECU��ַ

		if( !bReceiveStatus )
		{
			return FRAME_TIME_OUT;
		}

		bReceiveStatus = ( bool )receive_cmd( *ppcSource, *piValidLen - 3, u16FrameContentTimeout ); //����ʣ���ֽ�

		if( !bReceiveStatus )
		{
			return FRAME_TIME_OUT;
		}
	}

	default:
		return FORMAT_ERORR;
	}

	return SUCCESS;
}
/*************************************************
Description:	����Ԥ������
Input:	pstSnedCmd	�����������ṹ��ָ��
Output:	pstSnedCmd	�ú������У��ͷŵ�����
					������������һ���ֽ�
Return:	void
Others:
*************************************************/
void pre_process_cmd( STRUCT_CMD* pstSnedCmd )
{
	pstSnedCmd->pcCmd[pstSnedCmd->iCmdLen - 1] = calculate_Checksum( pstSnedCmd->pcCmd, pstSnedCmd->iCmdLen - 1 );
}


