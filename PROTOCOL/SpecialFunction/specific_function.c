/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	特殊功能的函数处理文件，将所有的处理函数写在 process_special_function 上边。这样不用写函数声明。
                所有特殊功能的操作都在一个文件内进行。
History:
	<author>    张学岭
	<time>		2015年5月18日14:08:25
	<desc>      文件最后有 函数模板，可以作参考
************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "special_function.h"
#include "..\InitConfigFromXml\init_config_from_xml_lib.h"
#include <assert.h>
#include "..\command\command.h"
#include "..\interface\protocol_interface.h"
#include "..\interface\protocol_define.h"
byte SYS_ConfigType=0;//确认协议通讯类型kwp或者CAN


bool FUN_setVDI(byte * set_VCIData)
{
	
	g_p_stVCI_params_config->cVoltage = 1;
    g_p_stVCI_params_config->cLevel = 0;
	g_p_stVCI_params_config->cLogic = 0;
	g_p_stVCI_params_config->cCANFrameMode = 0;
	g_p_stVCI_params_config->cVCIBaudRate[0] =0x01;
	g_p_stVCI_params_config->cVCIBaudRate[1] =0xC2;
	g_p_stVCI_params_config->cVCIBaudRate[2] =0x00;
	g_p_stVCI_params_config->cCanSamplingConfig[0] =0;
	g_p_stVCI_params_config->cCanSamplingConfig[1] =0;
	g_p_stVCI_params_config->cCanSamplingConfig[2] =0;
	g_p_stVCI_params_config->cCanSamplingConfig[3] =0;

	g_p_stVCI_params_config->cCommunicationType=set_VCIData[0];
	g_p_stVCI_params_config->cReceivePin = set_VCIData[1];
	g_p_stVCI_params_config->cSendPin    = set_VCIData[2];
	g_p_stVCI_params_config->cECUBaudRate[0] = set_VCIData[3];
	g_p_stVCI_params_config->cECUBaudRate[1] = set_VCIData[4];
	g_p_stVCI_params_config->cECUBaudRate[2] = set_VCIData[5];
	g_p_stVCI_params_config->cCanFilterIDGroupNum = set_VCIData[6]; 
	memcpy( g_p_stVCI_params_config->cCanFilterID,&set_VCIData[7],  set_VCIData[6]*4 );
	if( !(package_and_send_vci_config()) )
	{
		
		return false;

	}
	
	return true;

}
bool FUN_readVIN(byte * set_VINData, byte * VINData)
{
	int iReturnStatus = TIME_OUT;
	byte cBufferOffset = 0;//读码命令保存位置偏移量
	byte byteoffset;       //读码关键字节偏移量

	switch(SYS_ConfigType)
	{
		case ISO15765_CONFIG:
			iReturnStatus = send_and_receive_cmd_by_iso_15765( set_VINData );
			break;
		case ISO14230_CONFIG:
			iReturnStatus = send_and_receive_cmd_by_iso_14230( set_VINData );
			break;
		default:
			break;
	}
	if ( iReturnStatus!=SUCCESS )
	{
		return false;

	}
	cBufferOffset = g_stCmdList[set_VINData[1]].cBufferOffset ;
	byteoffset = set_VINData[set_VINData[0]+1];
	if (g_stBufferGroup[cBufferOffset].cBuffer[byteoffset] =='L' && g_stBufferGroup[cBufferOffset].cBuffer[byteoffset+1] =='N' && g_stBufferGroup[cBufferOffset].cBuffer[byteoffset+2] =='B' )
	{
		memcpy( VINData,&g_stBufferGroup[cBufferOffset].cBuffer[byteoffset],  17 );
		return true;
	}
	else
	{
		return false;
	}	

}
bool process_Active_CAN(byte * pset_Active)
{
	int iReturnStatus = TIME_OUT;
	bSend_FC_Frame_Cmd( &g_stCmdList[ pset_Active[pset_Active[1]+2] ] ,4,0,0X10 );
    iReturnStatus = send_and_receive_cmd_by_iso_15765( pset_Active+1 );
	if(iReturnStatus!= SUCCESS)
	{
		return false;

	}
	return true;

}
bool process_Active_KWP(byte * pset_Active, byte pParams[][23] )
{
	bool bSendStatus = false;
	int iReturnStatus = TIME_OUT;
	byte cRetransTime=3;
	byte cActiveparams;
	
	
	cActiveparams = pset_Active[pset_Active[1]+2];
	
	g_p_stGeneralActiveEcuConfig->cTimeBetweenBytes = pParams[cActiveparams][0];
	g_p_stGeneralActiveEcuConfig->cActiveMode = pParams[cActiveparams][1];
	g_p_stGeneralActiveEcuConfig->cDetectBandRateFlag = pParams[cActiveparams][2];
	g_p_stGeneralActiveEcuConfig->u16ECUResByteTimeout = (pParams[cActiveparams][3]<<8)+(pParams[cActiveparams][4]);
    memcpy( g_p_stGeneralActiveEcuConfig->cBandRate,&pParams[cActiveparams][5],  3 );
	memcpy( g_p_stGeneralActiveEcuConfig->cActiveAddress,&pParams[cActiveparams][8],  10 );
	memcpy( g_p_stGeneralActiveEcuConfig->cReceiveKeyByte,&pParams[cActiveparams][18],  5 );
	
	
	while(cRetransTime--)
	{
		bSendStatus = package_and_send_active_config();
		if( !bSendStatus )
		{
			return false;
		}
		iReturnStatus = send_and_receive_cmd_by_iso_14230( pset_Active+1 );
		if(iReturnStatus!= SUCCESS)
		{
			continue;
		}
		break;

	}
	if ( iReturnStatus!= SUCCESS )
	{
		return false;
	}	
	return true;

}
/*************************************************
Description: 扫描车型VIN
*************************************************/
void process_readVIN_function( void* pIn, void* pOut )
{
	byte cVIN[2][17]={0};
	byte all_vin[34]={0};
	byte SYSnum = 2;//系统个数
	byte cAcativeType,i;
	bool bStatus = false;
	//如果为空则新开辟空间
	if( g_p_stVCI_params_config == NULL )
		g_p_stVCI_params_config = ( STRUCT_VCI_PARAMS * )malloc( sizeof( STRUCT_VCI_PARAMS ) );
	if ( g_p_stGeneralActiveEcuConfig == NULL)
	    g_p_stGeneralActiveEcuConfig = (STRUCT_ACTIVE_ECU_CONFIG *)malloc( sizeof(STRUCT_ACTIVE_ECU_CONFIG) );
	
    for ( i = 0; i<SYSnum; i++ )
	{
// 		if ( 1 == i)
// 		{
// 			continue;
// 		}
		bStatus = FUN_setVDI(g_stSYSList[i].pVDIData);
		if(!bStatus)
			continue;		  
		else
		{
			cAcativeType=g_stSYSList[i].pActiveData[0];

			switch (cAcativeType)
			{
			case 0://CAN激活
				SYS_ConfigType = ISO15765_CONFIG; 				
				bStatus = process_Active_CAN(g_stSYSList[i].pActiveData);
				break;
			case 1://KWP2000 高低电平激活
                SYS_ConfigType = ISO14230_CONFIG;
				bStatus = process_Active_KWP(g_stSYSList[i].pActiveData, ActiveParams_KWP );
				break;
			default:
				break;
			} 
			if (!bStatus)
			   continue;
			else
			{
				bStatus=FUN_readVIN(g_stSYSList[i].pReadVIN,cVIN[i]);
				if (!bStatus)
				{
					break;
				}
			}
		}
	}

 	memcpy(all_vin,cVIN[0],17);
 	memcpy(all_vin+17,cVIN[1],17);

	special_return_status( PROCESS_OK | NO_JUMP | NO_TIP, NULL, 0, all_vin, 34, ORIGINAL, "ID_TEXT_BAICVIN", pOut );

// 	bStatus = !memcmp(cVIN[0],cVIN[2],17);
// 	if (!bStatus)
// 	{
// 		if (cVIN[0][0]=='L'&& cVIN[0][1]=='N' && cVIN[0][2]=='B'&& cVIN[1][0] !='L')
// 		{
// 			special_return_status( PROCESS_OK | NO_JUMP | NO_TIP, NULL, 0, cVIN[0], 17, ORIGINAL, "ID_TEXT_BAICVIN", pOut );
// 		}
// 		else if (cVIN[1][0]=='L'&& cVIN[1][1]=='N' && cVIN[1][2]=='B'&& cVIN[0][0] !='L')
// 		{
// 			special_return_status( PROCESS_OK | NO_JUMP | NO_TIP, NULL, 0, cVIN[1], 17, ORIGINAL, "ID_TEXT_BAICVIN", pOut );
// 		}
// 		else
// 		{
// 			general_return_status( FAILE, 0, 0, pOut );
// 		}		
// 	}
// 	else
// 	{
// 		special_return_status( PROCESS_OK | NO_JUMP | NO_TIP, NULL, 0, cVIN[0], 17, ORIGINAL, "ID_TEXT_BAICVIN", pOut );
// 	}

	free(g_p_stVCI_params_config);
	free(g_p_stGeneralActiveEcuConfig);
	return ;

}
/*************************************************
Description: 开始发送清码命令
*************************************************/
int Clear_DTC_set_VDI(byte sysDTC)
{
	g_p_stVCI_params_config->cVoltage = 1;
	g_p_stVCI_params_config->cLevel = 0;
	g_p_stVCI_params_config->cLogic = 0;
	g_p_stVCI_params_config->cCANFrameMode = 0;
	g_p_stVCI_params_config->cVCIBaudRate[0] =0x01;
	g_p_stVCI_params_config->cVCIBaudRate[1] =0xC2;
	g_p_stVCI_params_config->cVCIBaudRate[2] =0x00;
	g_p_stVCI_params_config->cCanSamplingConfig[0] =0;
	g_p_stVCI_params_config->cCanSamplingConfig[1] =0;
	g_p_stVCI_params_config->cCanSamplingConfig[2] =0;
	g_p_stVCI_params_config->cCanSamplingConfig[3] =0;

	g_p_stVCI_params_config->cCommunicationType=bVDI_SET[sysDTC][0];
	g_p_stVCI_params_config->cReceivePin = bVDI_SET[sysDTC][1];
	g_p_stVCI_params_config->cSendPin    = bVDI_SET[sysDTC][2];
	g_p_stVCI_params_config->cECUBaudRate[0] = bVDI_SET[sysDTC][3];
	g_p_stVCI_params_config->cECUBaudRate[1] = bVDI_SET[sysDTC][4];
	g_p_stVCI_params_config->cECUBaudRate[2] = bVDI_SET[sysDTC][5];
	g_p_stVCI_params_config->cCanFilterIDGroupNum = bVDI_SET[sysDTC][6]; 
	memcpy( g_p_stVCI_params_config->cCanFilterID,&bVDI_SET[sysDTC][7],  bVDI_SET[sysDTC][6]*4 );
	if( !(package_and_send_vci_config()) )
	{
		return FAILE;
	}

	return SUCCESS;
}

/*************************************************
Description: 开始发送清码命令
*************************************************/
int Clear_DTC_start(int sysDTC)
{
	int bStatus = 0;
	byte iCmdIndex[2] = {1,0};

	//设置VDI
	bStatus = Clear_DTC_set_VDI(sysDTC);
	if (bStatus)
	{
		return 1;
	}

	//设置30帧
	//bSend_FC_Frame_Cmd( &g_stCmdList[ pset_Active[pset_Active[1]+2] ] ,4,0,0x10 );

	//激活系统（进入扩展层）
	iCmdIndex[1] = sysDTC + 22;
	bStatus = send_and_receive_cmd_by_iso_15765( iCmdIndex );

	//发送清除故障码命令
	iCmdIndex[1] = sysDTC + 12;
	bStatus = send_and_receive_cmd_by_iso_15765( iCmdIndex );
	if (bStatus)
	{
		return 2;
	}

	return SUCCESS;
}


/*************************************************
Description: 向有故障的系统发送清码命令
*************************************************/
int process_Car_CLDTC( void* pIn, byte* pOut )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	byte bSysDTC[100] = {0};
	byte bSysDTCNumber = 0;  //有故障码的系统的个数
	byte bStatus = 0;
	byte bClearDtcFail[10] = {0};//
	byte p_system_location[10][2] = {0};
	int iDataLen = 0;
	int i = 0, j = 0;

	uint32 uTemp = 0;

	if( g_p_stVCI_params_config == NULL )
		g_p_stVCI_params_config = ( STRUCT_VCI_PARAMS * )malloc( sizeof( STRUCT_VCI_PARAMS ) );

	//检出所有存在故障码的系统
	for ( i = 0; i < pstParam->iLen;)
	{
		if ( ',' != pstParam->pcData[i] )
		{
			p_system_location[bSysDTCNumber][0] = i;
			switch (pstParam->pcData[i])
			{
			case '0':  //EMS
				bSysDTCNumber++;
				bSysDTC[bSysDTCNumber-1] = EMS_SYS;
				p_system_location[bSysDTCNumber-1][1] = 1;
				break;
			case '1':  //TCU
				bSysDTCNumber++;
				bSysDTC[bSysDTCNumber-1] = TCU_SYS;
				p_system_location[bSysDTCNumber-1][1] = 1;
				break;
			case '2':  //BCM
				bSysDTCNumber++;
				bSysDTC[bSysDTCNumber-1] = BCM_SYS;
				p_system_location[bSysDTCNumber-1][1] = 1;
				break;
			case '3':  //ICM
				bSysDTCNumber++;
				bSysDTC[bSysDTCNumber-1] = ICM_SYS;
				p_system_location[bSysDTCNumber-1][1] = 1;
				break;
			case '4':  //ABS
				bSysDTCNumber++;
				bSysDTC[bSysDTCNumber-1] = ABS_SYS;
				p_system_location[bSysDTCNumber-1][1] = 1;
				break;
			case '5':  //ESP
				bSysDTCNumber++;
				bSysDTC[bSysDTCNumber-1] = ESP_SYS;
				p_system_location[bSysDTCNumber-1][1] = 1;
				break;
			case '6':  //EPS
				bSysDTCNumber++;
				bSysDTC[bSysDTCNumber-1] = EPS_SYS;
				p_system_location[bSysDTCNumber-1][1] = 1;
				break;
			case '7':  //HUM
				bSysDTCNumber++;
				bSysDTC[bSysDTCNumber-1] = HUM_SYS;
				p_system_location[bSysDTCNumber-1][1] = 1;
				break;
			case '8':  //SDM
				bSysDTCNumber++;
				bSysDTC[bSysDTCNumber-1] = SDM_SYS;
				p_system_location[bSysDTCNumber-1][1] = 1;
				break;
			case '9':  //ACC
				bSysDTCNumber++;
				bSysDTC[bSysDTCNumber-1] = ACC_SYS;
				p_system_location[bSysDTCNumber-1][1] = 1;
				break;
			default:
				return FAILE;
				break;
			}
			while( ',' != pstParam->pcData[i] && i < pstParam->iLen )
			{
				i++;
			}
		}
		i++;
	}

	//根据存在故障码的系统，进行清码
	for ( i = 0; i < bSysDTCNumber; i++ )
	{
		bStatus = Clear_DTC_start(bSysDTC[i]);
		if ( 1 == bStatus )
		{
			bClearDtcFail[j] = i;
			j++;
		}
		else if ( 2 == bStatus )
		{
			bClearDtcFail[j] = i;
			j++;
		}
	}

	if ( 0 == j)
	{
		special_return_status( PROCESS_OK | NO_JUMP | NO_TIP, NULL, NULL, "END", 3, ORIGINAL, NULL, pOut );
		uTemp = pOut[3] * 16777216 + pOut[4] * 65536 + pOut[5] * 256 + pOut[6] - 2;
		pOut[3] = uTemp >> 24 & 0xFF;
		pOut[4] = uTemp >> 16 & 0xFF;
		pOut[5] = uTemp >> 8 & 0xFF;
		pOut[6] = uTemp & 0xFF;
	}
	else
	{
		iDataLen = special_return_status( PROCESS_OK | NO_JUMP | NO_TIP, NULL, NULL, &pstParam->pcData[p_system_location[bClearDtcFail[0]][0]], p_system_location[bClearDtcFail[0]][1], ORIGINAL, NULL, pOut );
		uTemp = pOut[3] * 16777216 + pOut[4] * 65536 + pOut[5] * 256 + pOut[6] - 2;
		pOut[3] = uTemp >> 24 & 0xFF;                                              
		pOut[4] = uTemp >> 16 & 0xFF;                                              
		pOut[5] = uTemp >> 8 & 0xFF;                                               
		pOut[6] = uTemp & 0xFF;
		iDataLen -= 2;
		for ( i = 1; i < j; i++ )
		{
			iDataLen = add_data_and_controlsID( iDataLen, &pstParam->pcData[p_system_location[bClearDtcFail[i]][0]], p_system_location[bClearDtcFail[i]][1], ORIGINAL, NULL,pOut);
			uTemp = pOut[3] * 16777216 + pOut[4] * 65536 + pOut[5] * 256 + pOut[6] - 2;
			pOut[3] = uTemp >> 24 & 0xFF;                                              
			pOut[4] = uTemp >> 16 & 0xFF;                                              
			pOut[5] = uTemp >> 8 & 0xFF;                                               
			pOut[6] = uTemp & 0xFF;  
			iDataLen -= 2;
		}
	}

	return SUCCESS;

}