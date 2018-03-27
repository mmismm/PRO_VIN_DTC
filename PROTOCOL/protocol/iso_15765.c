/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	�������ISO15765Э�鴦���շ����ݵ���غ���
History:
	<author>	<time>		<desc>

************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "iso_15765.h"
#include "../interface/protocol_interface.h"
#include "../public/protocol_config.h"
#include "../command/command.h"
#include "../InitConfigFromXml/init_config_from_xml_lib.h"
#include "../function/idle_link_lib.h"

/*************************************************
Description:	����ISO15765Э���շ�����
Input:	piCmdIndex	����������ַ

Output:	none
Return:	int	�շ�����ʱ��״̬
Others:	���������������Է��ͺͽ��ն������
		��ECU������Ӧ�����ݷŵ�������Ӧ�Ļ�����
ע�⣺������ECU�����Ǵ�SID�Ļظ���ʼ�ģ��磺
7E0  03 22 15 08 00 00 00 00
7E8  04 62 15 08 79 00 00 00
�Ǵ�62��ʼ���棬��Ϊ������Ӧ���7F��ʼ���档
*************************************************/
int send_and_receive_cmd_by_iso_15765( byte * piCmdIndex )
{
	int iStatus = TIME_OUT;
	int i = 0;
	byte iCmdSum = piCmdIndex[0];
	byte cReceiveBuffer[20] = {0};
	STRUCT_CMD stSendCmd = {0};

	for( i = 0; i < iCmdSum; i++ )
	{
		iStatus = send_and_receive_single_cmd_by_iso_15765( ( STRUCT_CMD * )&stSendCmd, piCmdIndex[1 + i], cReceiveBuffer );

		if( NULL != stSendCmd.pcCmd )
			free( stSendCmd.pcCmd );

		if( ( iStatus != SUCCESS ) && ( iStatus != NEGATIVE ) ) //���״̬�Ȳ���SUCCESS�ֲ���NEGATIVE����Ϊ����
		{
			break;
		}

		time_delay_ms( 20 );
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
Others:	����ISO15765Э�鴦��
*************************************************/
int send_and_receive_single_cmd_by_iso_15765( STRUCT_CMD *pstSendCmd, byte cCmdIndex, byte* pcReceiveBuffer )
{
	int		i = 0;
	int		j = 0;
	int		iReceiveStatus = TIME_OUT;
	byte	cIso15765ReservedByte	= 0XAA;
	byte	cRetransTime			= 3;
	uint16	u16ECUResTimeout		= 5000;
	bool	bSendStatus = false;
	int		iCmdLen = 0;
	bool	bMultiFrame = false;//��֡��־
	int 	iReceiveFCStatus = FAILE;//�ȴ�FC֡��־
	byte	cFrameSum = 0;
	STRUCT_CMD stCopySendCmd = {0};

	byte	cBufferOffset = 0;

	pstSendCmd->cBufferOffset	= g_stCmdList[cCmdIndex].cBufferOffset;
	pstSendCmd->cReserved		= g_stCmdList[cCmdIndex].cReserved;
	pstSendCmd->iCmdLen			= g_stCmdList[cCmdIndex].iCmdLen;
	pstSendCmd->pcCmd			= ( byte* )malloc( sizeof( byte ) * pstSendCmd->iCmdLen );
	memcpy( pstSendCmd->pcCmd, g_stCmdList[cCmdIndex].pcCmd, pstSendCmd->iCmdLen );



	cBufferOffset = pstSendCmd->cBufferOffset;

	if( pstSendCmd->iCmdLen > 2 && ( pstSendCmd->pcCmd[2] & 0x10 ) == 0x10 )
	{
		bMultiFrame = true;
		iCmdLen = pstSendCmd->iCmdLen;

		cFrameSum = ( iCmdLen - 10 ) / 7;
		cFrameSum += ( ( iCmdLen - 10 ) % 7 == 0 ) ? 0 : 1;
	}

	//��֡���ͣ���װ��һ֡����
	if( bMultiFrame )
	{
		i = j = cRetransTime;  //���ʹ���

		while( 1 )
		{
			stCopySendCmd.iCmdLen = pstSendCmd->iCmdLen + cFrameSum * 4 + 4;   //�����
			stCopySendCmd.pcCmd = ( byte* )malloc( sizeof( byte ) * stCopySendCmd.iCmdLen );

			stCopySendCmd.pcCmd[0] = 2;	//��֡֡ʱ����

			stCopySendCmd.pcCmd[1] = 0x01; //֡����  0x01: CAN��֡��ʽ����һ֡��30֡�ظ��� 0x02:��һ֡�ȴ��ظ�һ֡
			stCopySendCmd.pcCmd[2] = cFrameSum + 1; //֡����

			stCopySendCmd.pcCmd[3] = 10; //��һ֡������ֽڸ���

			memcpy( stCopySendCmd.pcCmd + 4, pstSendCmd->pcCmd, 10 );

			for( i = 0; i < cFrameSum; i++ )
			{
				stCopySendCmd.pcCmd[14 + i * 11 + 1] = stCopySendCmd.pcCmd[4]; //ID
				stCopySendCmd.pcCmd[14 + i * 11 + 2] = stCopySendCmd.pcCmd[5]; //ID

				stCopySendCmd.pcCmd[14 + i * 11 + 3] = 0x20 | ( ( i + 1 ) & 0x0F );


				if( i != cFrameSum - 1 ) //����������һ֡
				{
					memcpy( stCopySendCmd.pcCmd + 14 + i * 11 + 4, pstSendCmd->pcCmd + 10 + i * 7, 7 );
					stCopySendCmd.pcCmd[14 + i * 11] = 2 + 1 + 7; //ÿ֡������ֽڸ���

				}
				else
				{
					memcpy( stCopySendCmd.pcCmd + 14 + i * 11 + 4, pstSendCmd->pcCmd + 10 + i * 7, iCmdLen - 10 - i * 7 );
					stCopySendCmd.pcCmd[14 + i * 11] = 2 + 1 + iCmdLen - 10 - i * 7; //ÿ֡������ֽڸ���
				}
			}

			bSendStatus = package_and_send_frame( FRAME_HEAD_MULTI_FRAME, &stCopySendCmd, cIso15765ReservedByte );

			//�ͷ��ڴ�
			if( NULL != stCopySendCmd.pcCmd )
			{
				free( stCopySendCmd.pcCmd );
				stCopySendCmd.pcCmd = NULL;
			}

			if( !bSendStatus )
			{
				return FAILE;
			}

			iReceiveStatus = process_CAN_receive_Cmd( cBufferOffset, pcReceiveBuffer );

			if( TIME_OUT == iReceiveStatus )
			{
				if( ( --j ) == 0 )
					return iReceiveStatus;
			}
			else
				return iReceiveStatus;
		}
	}

	while( 1 )
	{
		bSendStatus = package_and_send_frame( FRAME_HEAD_NORMAL_FRAME, pstSendCmd, cIso15765ReservedByte );

		if( !bSendStatus )
		{
			return FAILE;
		}

		iReceiveStatus = process_CAN_receive_Cmd( cBufferOffset, pcReceiveBuffer );

		switch( iReceiveStatus )
		{
		case TIME_OUT:
		{
			if( ( --cRetransTime ) == 0 )
				return iReceiveStatus;


		}
		break;

		case SUCCESS:
		case FRAME_TIME_OUT:
		case NEGATIVE:

		default:
			return iReceiveStatus;

		}
	}
}
/*************************************************
Description:	�����׼CAN�Ľ���
Input:	cBufferOffset	�洢����ƫ��
		pcSource		����ECU�ظ����ݵĻ���

Output:	none
Return:	int	�շ�����ʱ��״̬
Others:	����ISO15765Э�鴦����գ�����
		cBufferOffset��Ӧ����g_stBufferGroup
		��pcSource�ǵ���receive_cmd����ʱ�ݴ�
		���ݵĵ�ַ��
*************************************************/
int process_CAN_receive_Cmd( const byte cBufferOffset, byte* pcSource )
{
	int	iReceiveStatus	= FAILE;
	bool	bSendStatus		= false;
    
	uint16	u16Code7F78Timeout	=4000; //g_p_stISO15765Config->u16Code7F78Timeout;
	uint16	u16ECUResTimeout	=4000; //g_p_stISO15765Config->u16ECUResTimeout;
	byte	cReserved			=0xAA; //g_p_stISO15765Config->cReserved;

	byte*	pcDestination		= g_stBufferGroup[cBufferOffset].cBuffer;

	byte	cBSmax				= 0;

	int		i = 0;
	int		iNegativeResponseCounter = 0;
	int		iValidLen = 0;

	//����ECU�ظ�����ĵ�һ֡
	iReceiveStatus = process_CAN_receive_single_Cmd( pcSource, u16ECUResTimeout );

	if( iReceiveStatus != SUCCESS )
	{
		return iReceiveStatus;
	}

	if( ( pcSource[2] == 0x03 ) && ( pcSource[3] == 0x7f ) ) //�ж�������Ӧ
	{
		if( pcSource[5] == 0x78 )
		{
			while( 1 ) //���while����������ȴ����7F78��Ŀǰ����200��
			{
				iReceiveStatus = process_CAN_receive_single_Cmd( pcSource, u16Code7F78Timeout );

				if( iReceiveStatus != SUCCESS )
				{
					return iReceiveStatus;
				}

				if( ( pcSource[2] == 0x03 ) && ( pcSource[3] == 0x7f ) && ( pcSource[5] == 0x78 ) )
					iNegativeResponseCounter++;
				else if( ( pcSource[2] == 0x03 ) && ( pcSource[3] == 0x7f ) )
				{
					memcpy( pcDestination, &pcSource[3], 3 ); //����������Ӧ����
					g_stBufferGroup[cBufferOffset].iValidLen = 3;
					return NEGATIVE;
				}
				else
					break;

				if( iNegativeResponseCounter == 200 )
					return TIME_OUT;

			}

		}//end if
		else
		{
			memcpy( pcDestination, &pcSource[3], 3 ); //����������Ӧ����
			g_stBufferGroup[cBufferOffset].iValidLen = 3;
			return NEGATIVE;
		}

	}//end if

	if( ( pcSource[2] & 0x10 ) != 0x10 ) //��֡
	{
		iValidLen = pcSource[2];

		g_stBufferGroup[cBufferOffset].iValidLen = iValidLen;

		memcpy( pcDestination, &pcSource[3], iValidLen );

		return SUCCESS;
	}

	//���������Ϊ�Ƕ�֡

	//cBSmax	= g_stInitXmlGobalVariable.m_p_stCmdList[g_p_stISO15765Config->cFCCmdOffset].pcCmd[3];
	iValidLen = ( pcSource[2] & 0x0F );
	iValidLen <<= 8;
	iValidLen += pcSource[3];

	g_stBufferGroup[cBufferOffset].iValidLen = iValidLen;

	memcpy( pcDestination, &pcSource[4], 6 ); //�ȱ����һ֡����

	pcDestination += 6;
	/*

		//����FC֡
		bSendStatus = package_and_send_frame( FRAME_HEAD_NORMAL_FRAME,
											( STRUCT_CMD * ) & (g_stInitXmlGobalVariable.m_p_stCmdList[g_p_stISO15765Config->cFCCmdOffset]), cReserved );

		if( !bSendStatus )
		{
			return FAILE;
		}*/

	for( ; i < ( ( iValidLen - 6 ) / 7 ); i++ )
	{
		iReceiveStatus = process_CAN_receive_single_Cmd( pcSource, u16ECUResTimeout );

		if( iReceiveStatus != SUCCESS )
		{
			return iReceiveStatus;
		}

		memcpy( &pcDestination[i * 7], &pcSource[3], 7 ); //��������һ֡

	}//end for

	if( ( iValidLen - 6 ) % 7  == 0 ) //���������֡�ͷ��سɹ�
	{
		return SUCCESS;
	}

	//����ʣ���ֽڣ�����1�������ֽڡ�2���ֽ�ID��CF֡�����ֽں�ʣ�����Ч�ֽ�
	iReceiveStatus = process_CAN_receive_single_Cmd( pcSource, u16ECUResTimeout );

	if( iReceiveStatus != SUCCESS )
	{
		return iReceiveStatus;
	}

	memcpy( &pcDestination[i * 7], &pcSource[3], ( ( iValidLen - 6 ) % 7 ) ); //�����������֡

	return SUCCESS;
}

/*************************************************
Description:	�������������֡(ʱ����Կ���)��
Input:
cFrameHead		����֡ͷ
pstFrameContent	��������ṹ��ָ��
cReservedByte	�����б����ֽ�

Output:	none
Return:	bool	���ط���״̬���ɹ���ʧ�ܣ�
Others:	�ú����᳢�Է������Σ������շ�װ��
      �ظ�����
*************************************************/
bool package_and_send_frame_time( const byte cFrameHead, STRUCT_CMD* pstFrameContent, const byte cReservedByte )
{
	UNN_2WORD_4BYTE uFrameLen;
	bool bReturnStatus = false;
	byte *pcSendCache = NULL;
	byte cReceiveCache[5] = {0};//���յĻ���
	byte cCheckNum = 0;
	int i = 0;

	uFrameLen.u32Bit = 1 + 2 + 1 + pstFrameContent->iCmdLen + 1 + 2;

	pcSendCache = ( byte * )malloc( ( uFrameLen.u32Bit ) * sizeof( byte ) ); //����֡�Ļ���

	pcSendCache[0] = cFrameHead;
	pcSendCache[1] = uFrameLen.u8Bit[1];
	pcSendCache[2] = uFrameLen.u8Bit[0];
	pcSendCache[3] = cReservedByte;

	pcSendCache[4] = 0x00;
	pcSendCache[5] = 0xff;

	memcpy( &pcSendCache[6], pstFrameContent->pcCmd, pstFrameContent->iCmdLen );

	for( i = 0; ( i < ( int )uFrameLen.u32Bit - 1 ); i++ )
	{
		cCheckNum += pcSendCache[i];
	}

	pcSendCache[ uFrameLen.u32Bit - 1] = cCheckNum;

	send_cmd( pcSendCache, uFrameLen.u32Bit );

	if( ( bool )receive_cmd( cReceiveCache, 5, 3000 ) )
	{
		if( cReceiveCache[3] == 0x00 )
		{
			bReturnStatus = true;
		}
	}

	free( pcSendCache );

	return bReturnStatus;
}

/*************************************************
Description:	�������������֡
Input:
	cFrameHead		����֡ͷ
	pstFrameContent	��������ṹ��ָ��
	cReservedByte	�����б����ֽ�

Output:	none
Return:	bool	���ط���״̬���ɹ���ʧ�ܣ�
Others:	�ú����᳢�Է������Σ������շ�װ��
		�ظ��������жϷ���״̬��
*************************************************/
bool package_and_send_frame( const byte cFrameHead, STRUCT_CMD* pstFrameContent, const byte cReservedByte )
{
	UNN_2WORD_4BYTE uFrameLen;
	bool bReturnStatus = false;
	byte *pcSendCache = NULL;
	byte cReceiveCache[5] = {0};//���յĻ���
	byte cCheckNum = 0;
	int i = 0;

	uFrameLen.u32Bit = 1 + 2 + 1 + pstFrameContent->iCmdLen + 1;

	pcSendCache = ( byte * )malloc( ( uFrameLen.u32Bit ) * sizeof( byte ) ); //����֡�Ļ���

	pcSendCache[0] = cFrameHead;
	pcSendCache[1] = uFrameLen.u8Bit[1];
	pcSendCache[2] = uFrameLen.u8Bit[0];
	pcSendCache[3] = cReservedByte;

	memcpy( &pcSendCache[4], pstFrameContent->pcCmd, pstFrameContent->iCmdLen );

	for( i = 0; ( i < ( int )uFrameLen.u32Bit - 1 ); i++ )
	{
		cCheckNum += pcSendCache[i];
	}

	pcSendCache[ uFrameLen.u32Bit - 1] = cCheckNum;

	send_cmd( pcSendCache, uFrameLen.u32Bit );

	if( ( bool )receive_cmd( cReceiveCache, 5, 3000 ) )
	{
		if( cReceiveCache[3] == 0x00 )
		{
			bReturnStatus = true;
		}
	}

	free( pcSendCache );

	return bReturnStatus;
}
/*************************************************
Description:	�����׼CAN�Ľ���

Input:
	pcSource		����ECU�ظ����ݵĻ���
	u16Timeout		����һ֡����ĳ�ʱ

Output:	pcSource	����ECU�ظ����ݵĻ���
Return:	int	�շ�����ʱ��״̬
Others:	VCI�������ݵĸ�ʽ�ǣ�1���ֽ���Ч�ֽڳ���
							+2���ֽ�ID+��Ч�ֽ�
*************************************************/
int process_CAN_receive_single_Cmd( byte* pcSource, const uint16 u16Timeout )
{
	bool	bReceiveStatus	= false;
	byte	cValidLen = 0;//

	//�Ƚ���CAN֡���ݵ���Ч�ֽڳ���
	bReceiveStatus = ( bool )receive_cmd( &cValidLen, 1, u16Timeout );

	if( !bReceiveStatus )
		return TIME_OUT;

	//����ID+��Ч����
	bReceiveStatus = ( bool )receive_cmd( pcSource, cValidLen + 2, u16Timeout );

	if( !bReceiveStatus )
		return TIME_OUT;

	return SUCCESS;

}

