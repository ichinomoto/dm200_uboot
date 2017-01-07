/****************************************************************
//    CopyRight(C) 2008 by Rock-Chip Fuzhou
//      All Rights Reserved
//�ļ���:hw_SDM.c
//����:SD manager implement
//����:hcy
//��������:2008-11-08
//���ļ�¼:
//��ǰ�汾:1.00
$Log: hw_SDM.c,v $
****************************************************************/
#define SDM_DRIVER
#include "sdmmc_config.h"

#define RK29_eMMC_Debug 0

#if RK29_eMMC_Debug
static int eMMC_debug = 5;
#define eMMC_printk(n, format, arg...) \
	if (n <= eMMC_debug) {	 \
		PRINT_E(format,##arg); \
	}
#else
#define eMMC_printk(n, arg...)
static const int eMMC_debug = 0;
#endif

#ifdef DRIVERS_SDMMC

/****************************************************************/
//������:_RegisterCard
//����:ע��һ���µĿ���SD manager
//����˵��:cardInfo   �������    ����������Ϣ
//����ֵ:
//���ȫ�ֱ���:дgSDMDriver[i].cardInfo
//ע��:
/****************************************************************/
static void _RegisterCard(pSDM_CARD_INFO_T pCardInfo)
{
    uint32 i = pCardInfo->cardId;

    if (pCardInfo == NULL)
    {
        return;
    }
    
    if(i < SDM_MAX_MANAGER_PORT)
    {
        if (gSDMDriver[i].cardInfo.cardId == SDM_INVALID_CARDID)
        {
            SDOAM_Memcpy(&gSDMDriver[i].cardInfo, pCardInfo, sizeof(SDM_CARD_INFO_T));
        }
    }
    return;
}

/****************************************************************/
//������:_IsCardRegistered
//����:���ҿ��Ƿ��Ѿ�ע���������ע��Ķ˿ں�ͨ��port����
//����˵��:cardId   �������   ��Ҫ���ҵĿ���
//         pPort    �������   ���ؿ�ע��Ķ˿ں�
//����ֵ:TRUE   ���Ѿ�ע���
//       FALSE  δע��Ŀ�
//���ȫ�ֱ���:��ȡgSDMDriver[i].cardInfo.cardId
//ע��:
/****************************************************************/
static uint32 _IsCardRegistered(int32 cardId, uint32* pPort)
{
    uint32 i;
    *pPort = SDM_MAX_MANAGER_PORT;
    if(gSDMDriver[cardId].cardInfo.cardId == cardId)
    {
        *pPort = cardId;
        return TRUE;
    }
    return FALSE;
}

/****************************************************************/
//������:_ResponseTimeoutHandle
//����:�����������timeout�Ĵ���
//����˵��:pCardInfo     �������  ָ����Ϣ��ָ��
//����ֵ:SDM_CARD_NOTPRESENT  ��������
//       SDM_RESP_TIMEOUT     �����ڣ���ȴ�������ظ�
//       SDM_CARD_LOCKED      ������ס
//       SDM_SUCCESS          �ɹ����������·�����
//       SDM_FALSE            δ֪����
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
static int32 _ResponseTimeoutHandle(int32 cardId)
{
    uint32 status  = 0;
    uint32 repeatCount = 0;
    int32  ret = SDC_SUCCESS;
    uint32 port = SDM_MAX_MANAGER_PORT;

    if (!_IsCardRegistered(cardId, &port))
    {
        return SDM_PARAM_ERROR;
    }

    SDOAM_Printf("HANDLING:card presence\n");
    while (repeatCount < SDM_CMD_RESENT_COUNT)
    {
        SDOAM_Printf("HANDLING:read status\n");
        ret = SDC_SendCommand(cardId, (SD_SEND_STATUS | SD_NODATA_OP | SD_RSP_R1 | NO_WAIT_PREV), (gSDMDriver[port].cardInfo.rca << 16), &status);
        if (SDC_SUCCESS == ret)
        {
            if (status & CARD_IS_LOCKED)
            {
                SDOAM_Printf("HANDLING:card locked\n");
                ret = SDM_CARD_LOCKED;
                break;
            }
            else if (status & COM_CRC_ERROR)
            {
                SDOAM_Printf("HANDLING:previous cmd CRC er\n");
                ret = SDM_SUCCESS;
                break;
            }
            else
            {
                SDOAM_Printf("HANDLING:other er %x\n", status);
                ret = SDM_FALSE;
                break;
            }
        }
        else if (ret == SDC_RESP_TIMEOUT)
        {
            SDOAM_Printf("HANDLING:read status timeout\n");
            //if (TRUE == SDC_IsCardPresence(cardId))
            {
                ret = SDM_RESP_TIMEOUT;
                break;
            }
            /*else
            {
                ret = SDM_CARD_NOTPRESENT;
                break;
            }*/
        }
        else if ((ret == SDC_RESP_CRC_ERROR) || (ret == SDC_RESP_ERROR))
        {
            SDOAM_Printf("HANDLING:read status Rsp er or CRC er, Resend Read status\n");
            repeatCount++;
            continue;
        }
        else
        {
            SDOAM_Printf("HANDLING:read status other errror %d\n", ret);
            //��������ֱ�ӷ���
            break;
        }
    }
    if ((repeatCount == SDM_CMD_RESENT_COUNT) && (ret != SDM_SUCCESS))
    {
        SDOAM_Printf("HANDLING:retry two times, but not success\n");
        ret = SDM_FALSE;
    }

    return ret;
}

/****************************************************************/
//������:_DataErrorHandle
//����:������д�������ݴ���ʱ�����ڳ�������Ҫ����STOP����
//����˵��:pCardInfo     �������  ָ����Ϣ��ָ��
//����ֵ:SDM_CARD_NOTPRESENT  ��������
//       SDM_RESP_TIMEOUT     �����ڣ���ȴ�������ظ�
//       SDM_CARD_LOCKED      ������ס
//       SDM_SUCCESS          �ɹ����������·�����
//       SDM_FALSE            δ֪����
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
static int32 _DataErrorHandle(int32 cardId)
{
    uint32 status  = 0;
    int32  ret = SDM_SUCCESS;

    ret = SDC_SendCommand(cardId, (SD_STOP_TRANSMISSION | SD_NODATA_OP | SD_RSP_R1B | STOP_CMD | NO_WAIT_PREV), 0, &status);
    if (ret == SDC_RESP_TIMEOUT)
    {
        SDOAM_Printf("HANDLING:Send STOP cmd timeout\n");
        SDOAM_Printf("HANDLING:card presence but timeout\n");
        ret = SDC_RESP_TIMEOUT;
    }
    else if ((ret == SDC_SUCCESS) || (ret == SDC_RESP_ERROR) || (ret == SDC_RESP_CRC_ERROR))
    {
        SDOAM_Printf("HANDLING:Send STOP cmd not timeout, ReSend cmd\n");
        //STOP command �ɹ���response���󣬶���Ϊ����ȷ���ط�
        ret = SDM_SUCCESS;
    }
    else
    {
        //��������ֱ�ӷ���
        ;
    }

    return ret;
}

/****************************************************************/
//������:_SDMMC_Read
//����:SD\MMC���Ķ�������������С��λ��block(512�ֽ�)
//����˵��:cardId     �������  ��Ҫ�����Ŀ�
//         dataAddr   �������  ��Ҫ��ȡ����ʼblock��ַ
//         blockCount �������  ��Ҫ������ȡ���ٸ�block
//         pBuf       �������  �������ݴ�ŵ�buffer��ַ
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
static int32 _SDMMC_Read(int32 cardId, uint32 dataAddr, uint32 blockCount, void *pBuf)
{
    int32            ret = SDM_SUCCESS;
    int32            handleRet = SDM_SUCCESS;  //������ķ���ֵ
    uint32           repeatCount = 0;
    uint32           status = 0;

    while (repeatCount < SDM_CMD_RESENT_COUNT)
    {
        if (blockCount == 1)
        {
            ret = SDC_ReadBlockData(cardId, 
                                    (SD_READ_SINGLE_BLOCK | SD_READ_OP | SD_RSP_R1 | WAIT_PREV), 
                                    dataAddr, 
                                    &status, 
                                    (blockCount << 9), 
                                    pBuf);
        }
        else
        {
            uint32 PreDefined = 0;
            if(gSDMDriver[cardId].cardInfo.type & eMMC2G && gSDMDriver[cardId].cardInfo.bootSize)
            {
                ret = SDC_BusRequest(cardId, (SD_SET_BLOCK_COUNT | SD_NODATA_OP | SD_RSP_R1 | WAIT_PREV), blockCount, &status, 0, 0, NULL);
                if (ret == SDC_SUCCESS)
                {
                    PreDefined = 1;
                } 
            }
            ret = SDC_ReadBlockData(cardId, 
                                    (SD_READ_MULTIPLE_BLOCK | SD_READ_OP | SD_RSP_R1 | WAIT_PREV), 
                                    dataAddr, 
                                    &status, 
                                    (blockCount << 9), 
                                    pBuf);
            if(ret == SDC_SUCCESS && PreDefined == 0)
            {
               //SDOAM_Delay(200);  //hcy 09-06-12����CMMB�ı�̬�󿨣��ڶ��������STOP���������û����ʱ��Ҳ�������STOP�����ݽ������̫�̣��������᲻������
                                   //���ֳ�����������:��д��û����ֻ�Ƕ��������ݲ���������Ҫ�����ݡ�ԭ�������CA���ֵ�4���������ݣ�����������Ǵ����Դ��洢
                                   //�ռ��ڵĵ�4��������ʼ�����ݣ����Ҵ洢���ĵ�4�����ᱻ�޸ĳ�ȫ0��������������ᡣ
                ret = SDC_SendCommand(cardId, (SD_STOP_TRANSMISSION | SD_NODATA_OP | SD_RSP_R1B | STOP_CMD | NO_WAIT_PREV), 0, &status);
                if (ret == SDC_RESP_TIMEOUT)
                {
                    eMMC_printk(1,"HANDLING:Send STOP cmd timeout\n","");
                    ret = SDC_RESP_TIMEOUT;
                    break;
                }
                else
                {
                    break;
                }
            }
        }

        if (ret == SDC_SUCCESS)
        {
            break;
        }
        else if (ret == SDC_START_BIT_ERROR)
        {
            eMMC_printk(1,"HANDLING:SDC_START_BIT_ER\n","");
            repeatCount++;
            handleRet = SDC_UpdateCardFreq(cardId, (gSDMDriver[cardId].cardInfo.tran_speed - 2000));  //Ƶ�ʽ�2MHz
            if(handleRet != SDC_SUCCESS)
            {
                break;
            }
            continue;
        }
        else if ((ret == SDC_END_BIT_ERROR) || (ret == SDC_DATA_READ_TIMEOUT))
        {
            eMMC_printk(1,"HANDLING:%s DataEr handle\n", ((ret == SDC_END_BIT_ERROR) ? "SDC_END_BIT_ER" : "SDC_DATA_READ_TIMEOUT"));
            handleRet = _DataErrorHandle(cardId);
            if (handleRet == SDM_SUCCESS)
            {
                eMMC_printk(1,"HANDLING:DataEr handle success, ReSend Read cmd\n","");
                repeatCount++;
                handleRet = SDC_UpdateCardFreq(cardId, (gSDMDriver[cardId].cardInfo.tran_speed - 2000));  //Ƶ�ʽ�2MHz
                if(handleRet != SDC_SUCCESS)
                {
                    break;
                }
                continue;
            }
            else
            {
                //��������ֱ�ӷ���
                break;
            }
        }
        else if (ret == SDC_DATA_CRC_ERROR)
        {
            eMMC_printk(1,"HANDLING:DATA CRC ER ReSend Read cmd\n","");
            //�ط�
            repeatCount++;
            handleRet = SDC_UpdateCardFreq(cardId, (gSDMDriver[cardId].cardInfo.tran_speed - 2000));  //Ƶ�ʽ�2MHz
            if(handleRet != SDC_SUCCESS)
            {
                break;
            }
            continue;
        }
        else if (ret == SDC_RESP_TIMEOUT)
        {
            eMMC_printk(1,"HANDLING:Read cmd Rsp timeout\n","");
            handleRet = _ResponseTimeoutHandle(cardId);
            if (handleRet == SDM_SUCCESS)
            {
                eMMC_printk(1,"HANDLING:Rsp timeout handle success, ReSend Read cmd\n","");
                //�ط�һ��
                repeatCount++;
                continue;
            }
            else
            {
                break;
            }
        }
        else if ((ret == SDC_RESP_CRC_ERROR) || (ret == SDC_RESP_ERROR))
        {
            //�ظ������ܣ�ֻҪ���ݲ�����û����
            ret = SDM_SUCCESS;
            break;
        }
        else
        {
            //��������ֱ�ӷ���
            break;
        }
    }
    if ((repeatCount == SDM_CMD_RESENT_COUNT) && (ret != SDM_SUCCESS))
    {
        ret = SDM_FALSE;
    }

    if(handleRet == SDM_SUCCESS)
    {
        return ret;
    }
    else
    {
        return handleRet;
    }
}

/****************************************************************/
//������:_SDMMC_Write
//����:SD\MMC����д������д����С��λ��block(512�ֽ�)
//����˵��:cardId     �������  ��Ҫ�����Ŀ�
//         dataAddr   �������  ��Ҫд�����ʼblock��ַ
//         blockCount �������  ��Ҫ����д����ٸ�block
//         pBuf       �������  ��Ҫд������ݴ�ŵ�buffer��ַ
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
static int32 _SDMMC_Write(int32 cardId, uint32 dataAddr, uint32 blockCount, void *pBuf)
{
    int32            ret = SDM_SUCCESS;
    int32            handleRet = SDM_SUCCESS;  //������ķ���ֵ
    uint32           status  = 0;
    uint32           repeatCount = 0;

    while (repeatCount < SDM_CMD_RESENT_COUNT)
    {
        if (blockCount == 1)
        {
            ret = SDC_WriteBlockData(cardId, 
                                     (SD_WRITE_BLOCK | SD_WRITE_OP | SD_RSP_R1 | WAIT_PREV), 
                                     dataAddr, 
                                     &status, 
                                     (blockCount << 9), 
                                     pBuf);
        }
        else
        {
            ret = SDC_WriteBlockData(cardId, 
                                     (SD_WRITE_MULTIPLE_BLOCK | SD_WRITE_OP | SD_RSP_R1 | WAIT_PREV), 
                                     dataAddr, 
                                     &status, 
                                     (blockCount << 9), 
                                     pBuf);
            if (ret == SDC_SUCCESS)
            {
                ret = SDC_SendCommand(cardId, (SD_STOP_TRANSMISSION | SD_NODATA_OP | SD_RSP_R1B | STOP_CMD | NO_WAIT_PREV), 0, &status);
                if (ret == SDC_RESP_TIMEOUT)
                {
                    SDOAM_Printf("HANDLING:Send STOP cmd timeout\n");
                    //if (TRUE == SDC_IsCardPresence(cardId))
                    {
                        SDOAM_Printf("HANDLING:card presence but timeout\n");
                        ret = SDC_RESP_TIMEOUT;
                        break;
                    }
                    /* else
                    {
                        SDOAM_Printf("HANDLING:card not presence\n");
                        ret = SDM_CARD_NOTPRESENT;
                        break;
                    }*/
                }
                else
                {
                    break;
                }
            }
        }

        if (ret == SDC_SUCCESS)
        {
            break;
        }
        else if (ret == SDC_DATA_CRC_ERROR)
        {
            SDOAM_Printf("HANDLING:DATA CRC ER, DataEr handle\n", ret);
            handleRet = _DataErrorHandle(cardId);
            if (handleRet == SDM_SUCCESS)
            {
                SDOAM_Printf("HANDLING:DataEr handle success, ReSend Write cmd\n");
                repeatCount++;
                handleRet = SDC_UpdateCardFreq(cardId, (gSDMDriver[cardId].cardInfo.tran_speed - 2000));  //Ƶ�ʽ�2MHz
                if(handleRet != SDC_SUCCESS)
                {
                    break;
                }
                continue;
            }
            else
            {
                //��������ֱ�ӷ���
                break;
            }
        }
        else if (ret == SDC_END_BIT_ERROR)
        {
            SDOAM_Printf("HANDLING:END BIT ERRPR\n");
            //if (TRUE == SDC_IsCardPresence(cardId))
            {
                SDOAM_Printf("HANDLING:card presence, DataEr handle\n");
                handleRet = _DataErrorHandle(cardId);
                if (handleRet == SDM_SUCCESS)
                {
                    SDOAM_Printf("HANDLING:DataEr handle success, ReSend Write cmd\n");
                    repeatCount++;
                    handleRet = SDC_UpdateCardFreq(cardId, (gSDMDriver[cardId].cardInfo.tran_speed - 2000));  //Ƶ�ʽ�2MHz
                    if(handleRet != SDC_SUCCESS)
                    {
                        break;
                    }
                    continue;
                }
                else
                {
                    //��������ֱ�ӷ���
                    break;
                }
            }
           /* else
            {
                SDOAM_Printf("HANDLING:card not presence\n");
                ret = SDM_CARD_NOTPRESENT;
                break;
            }*/
        }
        else if (ret == SDC_RESP_TIMEOUT)
        {
            SDOAM_Printf("HANDLING:Write cmd Rsp timeout\n");
            handleRet = _ResponseTimeoutHandle(cardId);
            if (handleRet == SDM_SUCCESS)
            {
                SDOAM_Printf("HANDLING:Rsp timeout handle success, ReSend Write cmd\n");
                //�ط�һ��
                repeatCount++;
                continue;
            }
            else
            {
                break;
            }
        }
        else if ((ret == SDC_RESP_CRC_ERROR) || (ret == SDC_RESP_ERROR))
        {
            //�ظ������ܣ�ֻҪ���ݲ�����û����
            ret = SDM_SUCCESS;
            break;
        }
        else
        {
            //��������ֱ�ӷ���
            break;
        }
    }
    if ((repeatCount == SDM_CMD_RESENT_COUNT) && (ret != SDM_SUCCESS))
    {
        ret = SDM_FALSE;
    }

    if(handleRet == SDM_SUCCESS)
    {
        return ret;
    }
    else
    {
        return handleRet;
    }
}

/****************************************************************/
//������:_RegisterFunction
//����:���ݲ�ͬ������ע�᲻ͬ�ĺ���
//����˵��:pCardInfo     �������  ָ����Ϣ��ָ��
//����ֵ:
//���ȫ�ֱ���:
//ע��:�������ֻ��IdentifyCard����
/****************************************************************/
static void _RegisterFunction(pSDM_CARD_INFO_T pCardInfo)
{
    pCardInfo->fun.read  = _SDMMC_Read;
    pCardInfo->fun.write = _SDMMC_Write;
    return;
}

/****************************************************************/
//������:_AccessBootPartition
//����:��дboot partition����user area
//����˵��:cardId     �������  ��������Ŀ�
//         enable     �������  ʹ��
//         partition  �������  ��������ĸ�boot partition
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
static int32 _AccessBootPartition(int32 cardId, uint32 partition)
{
    pSDM_CARD_INFO_T pCardInfo = NULL;
    uint32           port = SDM_MAX_MANAGER_PORT;

    if (!_IsCardRegistered(cardId, &port))
    {
        return SDM_PARAM_ERROR;
    }
    
    pCardInfo = &gSDMDriver[port].cardInfo;
    return MMC_AccessBootPartition(pCardInfo,  partition);
}


/****************************************************************/
//������:_SetBootWidth
//����:��������ģʽ���߿��Լ���λ���Ƿ񱣳ָ��߿�
//����˵��:cardId     �������  ��������Ŀ�
//         enable     �������  �����߿�����
//         width      �������  bootģʽ�µ��߿�
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
static int32 _SetBootWidth(int32 cardId, uint32 enable, uint8 width)
{
    pSDM_CARD_INFO_T pCardInfo = NULL;
    uint32           port = SDM_MAX_MANAGER_PORT;
    HOST_BUS_WIDTH_E bootBusWidth = (HOST_BUS_WIDTH_E)width; 

    if (!_IsCardRegistered(cardId, &port))
    {
        return SDM_PARAM_ERROR;
    }
    
    pCardInfo = &gSDMDriver[port].cardInfo;
    return MMC_SetBootBusWidth(pCardInfo, enable, bootBusWidth);    
}




/****************************************************************/
//������:_SwitchBoot
//����:�л�boot partition����user area
//����˵��:cardId     �������  ��������Ŀ�
//         enable     �������  ʹ��
//         partition  �������  ��������ĸ�boot partition
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
static int32 _SwitchBoot(int32 cardId, uint32 enable, uint32 partition)
{
    pSDM_CARD_INFO_T pCardInfo = NULL;
    uint32           port = SDM_MAX_MANAGER_PORT;

    if (!_IsCardRegistered(cardId, &port))
    {
        return SDM_PARAM_ERROR;
    }
    
    pCardInfo = &gSDMDriver[port].cardInfo;
    return MMC_SwitchBoot(pCardInfo, enable, partition);
}

/****************************************************************/
//������:_IdentifyCard
//����:ʶ��cardIdָ���Ŀ�
//����˵��:cardId     �������  ��Ҫʶ��Ŀ�
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
static int32 _IdentifyCard(int32 cardId)
{
    SDM_CARD_INFO_T  cardInfo;
    uint32           status = 0;
    uint32           nf;
    uint32           mp;
    int32            ret = SDC_SUCCESS;

    SDOAM_Memset(&cardInfo, 0x00, sizeof(SDM_CARD_INFO_T));
    cardInfo.type = UNKNOW_CARD;
    cardInfo.cardId = cardId;
   // gSDMDriver[cardId].step = 0;
   // gSDMDriver[cardId].error = 0;
    /* reset all card */
    do
    {
        ret = SDC_ControlPower(cardId, TRUE);
        if (SDC_SUCCESS != ret)
        {
           // gSDMDriver[cardId].step = 0x11;
           // gSDMDriver[cardId].error = ret;
            break;
        }
        ret = SDC_SetHostBusWidth(cardId, BUS_WIDTH_1_BIT);
        if (SDC_SUCCESS != ret)
        {
           // gSDMDriver[cardId].step = 0x12;
           // gSDMDriver[cardId].error = ret;
            break;
        }
        ret = SDC_UpdateCardFreq(cardId, FOD_FREQ);
        if (SDC_SUCCESS != ret)
        {
           // gSDMDriver[cardId].step = 0x13;
           // gSDMDriver[cardId].error = ret;
            break;
        }
        SDOAM_Delay(500);  //�ȴ���Դ��ʱ���ȶ�
        ret = _Identify_SendCmd(cardId, (SD_GO_IDLE_STATE | SD_NODATA_OP | SD_RSP_NONE | NO_WAIT_PREV | SEND_INIT), 0, NULL, 0, 0, NULL);
        if (SDC_SUCCESS != ret)
        {
            //gSDMDriver[cardId].step = 0x14;
            //gSDMDriver[cardId].error = ret;
            break;
        }
        SDOAM_Delay(1000);  // 27�з���CMD0�������Ժ���ʱһ���ٷ�������������߿���ʶ����
        
        /**************************************************/
        // ��ʶ��
        /**************************************************/
        #if(SD_CARD_Support)
        ret = _Identify_SendCmd(cardId, (SD2_SEND_IF_COND | SD_NODATA_OP | SD_RSP_R7 | WAIT_PREV), 0x1AA, &status, 0, 0, NULL);
        if (SDC_SUCCESS == ret)
        {
            /* SDIO-only Card or SDIO-SDHC/SD2.0 Combo Card or SDIO-SDHC/SD2.0 only Card or SD2.0 or SDHC */
            nf = 0;
            mp = 0;
            ret = _Identify_SendCmd(cardId, (SDIO_IO_SEND_OP_COND | SD_NODATA_OP | SD_RSP_R4 | WAIT_PREV), 0, &status, 0, 0, NULL);
            if (SDC_SUCCESS == ret)
            {
                nf = (status >> 28) & 0x7;
                mp = (status >> 27) & 0x1;
                if ((mp == 1) && (nf > 0) && (status & 0xFFFF00))
                {
                    /* SDIO-SDHC/SD2.0 Combo Card */
                    //SDIOHC_SD20_ComboInit(&cardInfo);
                    cardInfo.type == UNKNOW_CARD;
                }
                else if ((mp == 0) && (nf > 0) && (status & 0xFFFF00))
                {
                    /* SDIO-only Card */
                    //SDIO_OnlyInit(&cardInfo);
                    cardInfo.type == UNKNOW_CARD;
                }
                else if(mp == 1)
                {
                    /* SDIO-SDHC/SD2.0 only Card */
                    SD20_Init(&cardInfo);
                }
                else
                {
                    /* unknow card */
                }
            }
            else if (ret == SDC_RESP_TIMEOUT)
            {
                /* SD2.0 or SDHC */
                SD20_Init(&cardInfo);
            }
            else
            {
                /* must be error occured */
                //gSDMDriver[cardId].step = 0x14;
                //gSDMDriver[cardId].error = ret;
            }
        }
        else if (SDC_RESP_TIMEOUT == ret)
        {
            /* SDIO-only Card or SDIO-SD1.X Combo Card or SDIO-SD1.X only Card or SD1.X or MMC or SD2.0 or later with voltage mismatch */
            nf = 0;
            mp = 0;
            ret = _Identify_SendCmd(cardId, (SDIO_IO_SEND_OP_COND | SD_NODATA_OP | SD_RSP_R4 | WAIT_PREV), 0, &status, 0, 0, NULL);
            if (SDC_SUCCESS == ret)
            {
                nf = (status >> 28) & 0x7;
                mp = (status >> 27) & 0x1;
                if((mp == 1) && (nf > 0) && (status & 0xFFFF00))
                {
                    /* SDIO-SD1.X Combo Card */
                    //SDIO_SD1X_ComboInit(&cardInfo);
                    cardInfo.type == UNKNOW_CARD;
                }
                else if ((mp == 0) && (nf > 0) && (status & 0xFFFF00))
                {
                    /* SDIO-only Card */
                    //SDIO_OnlyInit(&cardInfo);
                    cardInfo.type == UNKNOW_CARD;
                }
                else if (mp == 1)
                {
                    /* SDIO-SD1.X only Card */
                    SD1X_Init(&cardInfo);
                }
                else
                {
                    /* unknow card */
                }
            }
            else if (ret == SDC_RESP_TIMEOUT)
            {
                /* SD1.X or MMC or SD2.0 or later with voltage mismatch */
                ret = _Identify_SendCmd(cardId, (SD_APP_CMD | SD_NODATA_OP | SD_RSP_R1 | WAIT_PREV), 0, &status, 0, 0, NULL);
                if (SDC_SUCCESS == ret)
                {
                    /* SD1.X or SD2.0 or later with voltage mismatch */
                    SD1X_Init(&cardInfo);
                }
                else if (SDC_RESP_TIMEOUT == ret)
                {
        #endif
                    /* must be MMC */
                    MMC_Init(&cardInfo);
        #if(SD_CARD_Support)
                }
                else
                {
                    /* must be error occured */
                   // gSDMDriver[cardId].step = 0x15;
                   // gSDMDriver[cardId].error = ret;
                }
            }
            else
            {
                /* must be error occured */
                //gSDMDriver[cardId].step = 0x16;
                //gSDMDriver[cardId].error = ret;
            }
        }
        else
        {
            /* must be error occured */
            //gSDMDriver[cardId].step = 0x17;
            //gSDMDriver[cardId].error = ret;
        }
        #endif   
    }while (0);

    if (cardInfo.type == UNKNOW_CARD)
    {
        SDC_ResetController(cardId);
        //SDC_SetHostBusWidth(cardId, BUS_WIDTH_1_BIT);
        //SDC_ControlClock(cardId, FALSE);
        //SDC_ControlPower(cardId, FALSE);
        PRINTF("ERROR:Card Identify Failed %x\n", ret);
        return ret;
    }
    else
    {
        _RegisterFunction(&cardInfo);
        _RegisterCard(&cardInfo);
#if 0       
        PRINT_E("MSG:Card Identify SUCCESS\n","");
        if (cardInfo.type & SDIO)
        {
            if (cardInfo.type & (SDHC | SD20 | SD1X))
            {
                PRINT_E("MSG:SDIO Combo Card\n","");
            }
            else
            {
                PRINT_E("MSG:SDIO only Card\n","");
            }
        }
        else
        {
            if (cardInfo.type & SDHC)
            {
                PRINT_E("MSG:SDHC Card\n","");
            }
            if (cardInfo.type & SD20)
            {
                PRINT_E("MSG:SD2.0 Card\n","");
            }
            if (cardInfo.type & SD1X)
            {
                PRINT_E("MSG:SD1.x Card\n","");
            }
            if (cardInfo.type & MMC4)
            {
                PRINT_E("MSG:MMC4 Card\n","");
            }
            if (cardInfo.type & MMC)
            {
                PRINT_E("MSG:MMC Card\n","");
            }
            if (cardInfo.type & eMMC2G)
            {
                PRINT_E("MSG:eMMC2G Card\n","");
            }
        }
        PRINT_E("MSG:Manufacture Data:%d.%d\n", cardInfo.year, cardInfo.month);
        if(cardInfo.workMode & SDM_WIDE_BUS_MODE)
        {
            PRINT_E("MSG:Use Wide bus mode\n","");
        }
        if(cardInfo.workMode & SDM_HIGH_SPEED_MODE)
        {
            PRINT_E("MSG:Use High speed mode\n","");
        }
#endif
        return SDM_SUCCESS;
    }
}

/****************************************************************/
//������:_GenerateRCA   (RCA:Relative Card Address)
//����:����һ���µ�RCA����֤���RCA�����е�RCA�����ظ�
//     ʵ�ַ����ǣ�ɨ�����е�RCA��ȡ�����ֵ����1���õ��µ�RCA
//����˵��:
//����ֵ:�������ɵ���RCA
//���ȫ�ֱ���:��ȡgSDMDriver[i].cardInfo.rca
//ע��:ֻ��MMC��Ҫ�õ����������SD����RCA���ɿ��Զ����ɵġ�
/****************************************************************/
uint16 _GenerateRCA(void)
{
    uint16 max = 2;   //rca = 0001��MMC�ϵ���ʼ��ʱʹ�õ�Ĭ�ϵ�ַ�����Դ�2��ʼ
    uint32 i;

    for (i=0; i<SDM_MAX_MANAGER_PORT; i++)
    {
        if (gSDMDriver[i].cardInfo.rca > max)
        {
            max = gSDMDriver[i].cardInfo.rca;
        }
    }

    return (max + 1);  //��֪���᲻�����
}

/****************************************************************/
//������:_IsRCAUsable
//����:�жϵ�ǰ������RCA�Ƿ���ã���������еĿ�RCA��ͻ���򲻿���
//     ���û�г�ͻ�������
//����˵��:rca   �������    ��Ҫ����RCA
//����ֵ:TRUE     ����
//       FALSE    ������
//���ȫ�ֱ���:��ȡgSDMDriver[i].cardInfo.rca
//ע��:
/****************************************************************/
uint32 _IsRCAUsable(uint16 rca)
{
    uint32 i;

    for (i=0; i<SDM_MAX_MANAGER_PORT; i++)
    {
        if (gSDMDriver[i].cardInfo.rca == rca)
        {
            return FALSE;
        }
    }

    return TRUE;
}

/****************************************************************/
//������:_Identify_SendCmd
//����:��������
//����˵��:
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
int32 _Identify_SendCmd(int32 cardId,
                               uint32 cmd,
                               uint32 cmdArg,
                               uint32 *responseBuf,
                               uint32  blockSize,
                               uint32  dataLen,
                               void   *pDataBuf)
{
    int32 ret = SDM_SUCCESS;
    int32 retry = SDM_CMD_ERROR_RETRY_COUNT;

    do
    {
        ret = SDC_BusRequest(cardId, cmd, cmdArg, responseBuf, blockSize, dataLen, pDataBuf);
        retry--;
    }while((ret & (SDC_RESP_ERROR | SDC_RESP_CRC_ERROR | SDC_RESP_TIMEOUT)) && (retry > 0));
    return ret;
}
#if(SD_CARD_Support)
/****************************************************************/
//������:_Identify_SendAppCmd
//����:����application����
//����˵��:
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
int32 _Identify_SendAppCmd(int32 cardId,
                                    uint16 rca,
                                    uint32 cmd,
                                    uint32 cmdArg,
                                    uint32 *responseBuf,
                                    uint32  blockSize,
                                    uint32  dataLen,
                                    void   *pDataBuf)
{
    int32  ret = SDM_SUCCESS;
    uint32 status = 0;
    int32  retry = SDM_CMD_ERROR_RETRY_COUNT;

    do
    {
        ret = SDC_BusRequest(cardId, 
                             (SD_APP_CMD | SD_NODATA_OP | SD_RSP_R1 | WAIT_PREV), 
                             (rca << 16), 
                             &status, 
                             0, 
                             0, 
                             NULL);
        if ((ret == SDC_SUCCESS) && (status & 0x20))
        {
            ret = SDC_BusRequest(cardId, cmd, cmdArg, responseBuf, blockSize, dataLen, pDataBuf);
        }
        retry--;
    }while((ret & (SDC_RESP_ERROR | SDC_RESP_CRC_ERROR | SDC_RESP_TIMEOUT)) && (retry > 0));

    return ret;
}
#endif
/****************************************************************/
//������:SDM_Init
//����:SDMģ���ʼ��
//����˵��:
//����ֵ:
//���ȫ�ֱ���:дgSDMDriver[i]
//ע��:
/****************************************************************/
void SDM_Init(uint32 CardId)
{
    //uint32 i;
    //uint8  name[3][5] = {"SDC0", "SDC1", "SDC2"};

    //for (i=0; i<SDM_MAX_MANAGER_PORT; i++)
    {
        //mutex�Ĵ��������Ѿ���RockCreateSems����������
        gSDMDriver[CardId].mutex  = SDOAM_CreateMutex();
        gSDMDriver[CardId].bOpen  = FALSE;
        SDOAM_Memset(&gSDMDriver[CardId].cardInfo, 0x00, sizeof(SDM_CARD_INFO_T));
        gSDMDriver[CardId].cardInfo.cardId = SDM_INVALID_CARDID;
    }
    
    SDC_Init(CardId);
}

/****************************************************************/
//������:SDM_Open
//����:����cardIdָ���Ŀ�
//����˵��:cardId     �������  ��Ҫ�򿪵Ŀ�
//����ֵ:���ؾ��
//���ȫ�ֱ���:��дgSDMDriver[i].bOpen
//ע��:
/****************************************************************/
int32 SDM_Open(int32 cardId)
{
    uint32 port = SDM_MAX_MANAGER_PORT;

    if ((!SDC_IsCardIdValid(cardId)) || (!_IsCardRegistered(cardId, &port)))
    {
        return SDM_PARAM_ERROR;
    }

    if (gSDMDriver[port].bOpen)
    {
        return SDM_SUCCESS;
    }

    gSDMDriver[port].bOpen = TRUE;
    return SDM_SUCCESS;
}

/****************************************************************/
//������:SDM_Close
//����:�ر�cardIdָ���Ŀ�
//����˵��:cardId     �������  ��Ҫ�رյĿ�
//����ֵ:
//���ȫ�ֱ���:��дgSDMDriver[i].bOpen
//ע��:
/****************************************************************/
int32 SDM_Close(int32 cardId)
{
    return SDM_SUCCESS;
}

/****************************************************************/
//������:SDM_Read
//����:��cardIdָ���Ŀ����ж�������������С��λ��block(512�ֽ�)
//����˵��:cardId     �������  ��Ҫ�����Ŀ�
//         blockNum   �������  ��Ҫ��ȡ����ʼblock��
//         blockCount �������  ��Ҫ������ȡ���ٸ�block
//         pBuf       �������  �������ݴ�ŵ�buffer��ַ��Ҫ���ַ4�ֽڶ���
//����ֵ:
//���ȫ�ֱ���:��gSDMDriver[i]
//ע��:pBuf��ַҪ���ַ4�ֽڶ���
/****************************************************************/
int32 SDM_Read(int32 cardId, uint32 blockNum, uint32 blockCount, void *pBuf)
{
    SDM_PORT_INFO_T *pSDMDriver;
    uint32           port;
    int32            ret = SDM_SUCCESS;
    uint32           mul;
	//PRINT_E("SDM_Read %lx %x\n",blockNum,blockCount);
    //PRINT_E("SDM_Read blockNum = %x %x\n",blockNum , blockCount);
    Assert((blockCount != 0), "SDM_Read:read count = 0\n", blockCount);
    if (blockCount == 0)
    {
        return SDM_PARAM_ERROR;
    }

    if(SDC2 == cardId)
    {
        port = SDC2;
    }
    else
    {
        if ((!SDC_IsCardIdValid(cardId)) || (!_IsCardRegistered(cardId, &port)))
        {
            return SDM_PARAM_ERROR;
        }
    }

    pSDMDriver = &gSDMDriver[port];
    if (((blockNum + blockCount) > (pSDMDriver->cardInfo.capability)))//(blockCount > (pSDMDriver->cardInfo.capability)) || 
    {
        return SDM_PARAM_ERROR;
    }
    
    if((pSDMDriver->cardInfo.type) & (SDHC | eMMC2G))
    {
        mul = 0;  //SDHC��ַ����block(512)Ϊ��λ�ģ�������Э���ַ����byteΪ��λ
    }
    else
    {
        mul = 9;
    }
    
    SDOAM_RequestMutex(pSDMDriver->mutex);
    if (pSDMDriver->bOpen)
    {
        #if EN_SDC_INTERAL_DMA
        int i;
        int mod;
        char * pu8buf = pBuf;
		mod = (MAX_DATA_SIZE_IDMAC >> 9);
        for(i=0;i<blockCount;i+=mod)
        {
			if(blockCount - i < mod)
			{
				mod = blockCount - i;
			}
            ret = (pSDMDriver->cardInfo.fun.read)(cardId, ((blockNum+i) << mul), mod, pu8buf+i*512);
            if(ret != SDM_SUCCESS)
            {
                SDM_Close(cardId);
                break;
            }        
        }
        #else
        //��ַ4�ֽڶ���
        ret = (pSDMDriver->cardInfo.fun.read)(cardId, (blockNum << mul), blockCount, pBuf);
        #endif
    }
    else
    {
        ret = SDM_CARD_CLOSED;
    }
    if(ret != SDM_SUCCESS)
    {
        PRINT_E("SDM_Read error=%lx ret=%x\n",blockNum,ret);
        //SDM_Close(cardId);
    }
    SDOAM_ReleaseMutex(pSDMDriver->mutex);
    return ret;
}

/****************************************************************/
//������:SDM_Write
//����:��cardIdָ���Ŀ�����д������д����С��λ��block(512�ֽ�)
//����˵��:cardId     �������  ��Ҫ�����Ŀ�
//         blockNum   �������  ��Ҫд�����ʼblock��
//         blockCount �������  ��Ҫ����д����ٸ�block
//         pBuf       �������  ��Ҫд������ݴ�ŵ�buffer��ַ��Ҫ���ַ4�ֽڶ���
//����ֵ:
//���ȫ�ֱ���:��gSDMDriver[i]
//ע��:pBuf��ַҪ���ַ4�ֽڶ���
/****************************************************************/
int32 SDM_Write(int32 cardId, uint32 blockNum, uint32 blockCount, void *pBuf)
{
    SDM_PORT_INFO_T *pSDMDriver;
    uint32           port;
    int32            ret = SDM_SUCCESS;
    uint32           mul;
	//PRINT_E("SDM_Write %lx %x\n",blockNum,blockCount);
    Assert((blockCount != 0), "SDM_Write:read count = 0\n", blockCount);
    if (blockCount == 0)
    {
        return SDM_PARAM_ERROR;
    }

    if(SDC2 == cardId)
    {
        port = SDC2;
    }
    else
    {
        if ((!SDC_IsCardIdValid(cardId)) || (!_IsCardRegistered(cardId, &port)))
        {
            return SDM_PARAM_ERROR;
        }
    }

    pSDMDriver = &gSDMDriver[port];
    if (((blockNum + blockCount) > (pSDMDriver->cardInfo.capability)))//(blockCount > (pSDMDriver->cardInfo.capability)) || 
    {
        return SDM_PARAM_ERROR;
    }

    if (pSDMDriver->cardInfo.WriteProt) //ֻ��SD������д����,MMC�����ֵ����Ϊ0
    {
        return SDM_CARD_WRITE_PROT;
    }

    if((pSDMDriver->cardInfo.type) & (SDHC | eMMC2G))
    {
        mul = 0;  //SDHC��ַ����block(512)Ϊ��λ�ģ�������Э���ַ����byteΪ��λ
    }
    else
    {
        mul = 9;
    }
    
    SDOAM_RequestMutex(pSDMDriver->mutex);
    if (pSDMDriver->bOpen)
    {
        #if EN_SDC_INTERAL_DMA
        int i;
        int mod;
        char * pu8buf = pBuf;
		mod = (MAX_DATA_SIZE_IDMAC >> 9);
        for(i=0;i<blockCount;i+=mod)
        {
			if(blockCount - i < mod)
			{
				mod = blockCount - i;
			}
            ret = (pSDMDriver->cardInfo.fun.write)(cardId, ((blockNum+i) << mul), mod, pu8buf+i*512);
            if(ret != SDM_SUCCESS)
            {
                SDM_Close(cardId);
                break;
            }        
        }
        #else
        ret = (pSDMDriver->cardInfo.fun.write)(cardId, (blockNum << mul), blockCount, pBuf);
        #endif
    }
    else
    {
        ret = SDM_CARD_CLOSED;
    }
    if(ret != SDM_SUCCESS)
    {
        PRINT_E("SDM_Write error=%lx ret=%x\n",blockNum,ret);
        //SDM_Close(cardId);
    }
    SDOAM_ReleaseMutex(pSDMDriver->mutex);

    return ret;
}

/****************************************************************/
//������:SDM_IOCtrl
//����:IO���ƺ���
//����˵��:cmd     �������  ����������
//         param   �������  ��������
//����ֵ:
//���ȫ�ֱ���:��gSDMDriver[i]
//ע��:
/****************************************************************/
int32 SDM_IOCtrl(uint32 cmd, void *param)
{
    uint32           port = SDM_MAX_MANAGER_PORT;
    int32            ret = SDM_SUCCESS;
    uint32          *pTmp = NULL;
    int32            cardId = SDM_INVALID_CARDID;

    pTmp = (uint32 *)param;
    cardId = (int32)pTmp[0];

    if (!(cmd == SDM_IOCTRL_REGISTER_CARD))
    {
        if ((!SDC_IsCardIdValid(cardId)) || (!_IsCardRegistered(cardId, &port)))
        {
            if(cmd == SDM_IOCTR_IS_CARD_READY)
            {
                pTmp[1] = FALSE;
            }
            else if(cmd == SDM_IOCTR_GET_CAPABILITY)
            {
                pTmp[1] = 0;
            }
            else if(cmd == SDM_IOCTR_GET_PSN)
            {
                pTmp[1] = (uint32)NULL;//
            }
            else
            {
            }
            return SDM_PARAM_ERROR;
        }
    }
    else
    {
        if(!SDC_IsCardIdValid(cardId))
        {
            return SDM_PARAM_ERROR;
        }
    }

    switch (cmd)
    {
        case SDM_IOCTRL_REGISTER_CARD:
            ret = _IdentifyCard(cardId);
            break;
        case SDM_IOCTRL_UNREGISTER_CARD:
            //SDOAM_RequestMutex(gSDMDriver[port].mutex);  //hcy 09-09-22 ��ǰ��Ķ�д��ɣ�����ֱ�ӽ�ȥ�ؿ�������ʹ��ǰ��Ķ�дwhile�ڿ�����������
            //_UnRegisterCard(cardId);
            //SDOAM_ReleaseMutex(gSDMDriver[port].mutex);
            break;
#if(0)//SD_CARD_Support)
        case SDM_IOCTRL_SET_PASSWORD:
            SDOAM_RequestMutex(gSDMDriver[port].mutex);
            if (gSDMDriver[port].bOpen)
            {
                ret = _SetPassword(cardId, (uint8 *)pTmp[1], (uint8 *)pTmp[2], (uint32)pTmp[3]);
                if (ret == SDM_CARD_NOTPRESENT)
                {
                    SDM_Close(cardId);
                }
            }
            else
            {
                ret = SDM_CARD_CLOSED;
            }
            SDOAM_ReleaseMutex(gSDMDriver[port].mutex);
            break;
        case SDM_IOCTRL_CLEAR_PASSWORD:
            SDOAM_RequestMutex(gSDMDriver[port].mutex);
            if (gSDMDriver[port].bOpen)
            {
                ret = _ClearPassword(cardId, (uint8 *)pTmp[1]);
                if (ret == SDM_CARD_NOTPRESENT)
                {
                    SDM_Close(cardId);
                }
            }
            else
            {
                ret = SDM_CARD_CLOSED;
            }
            SDOAM_ReleaseMutex(gSDMDriver[port].mutex);
            break;
        case SDM_IOCTRL_FORCE_ERASE_PASSWORD:
            SDOAM_RequestMutex(gSDMDriver[port].mutex);
            if (gSDMDriver[port].bOpen)
            {
                ret = _ForceErasePassword(cardId);
                if (ret == SDM_CARD_NOTPRESENT)
                {
                    SDM_Close(cardId);
                }
            }
            else
            {
                ret = SDM_CARD_CLOSED;
            }
            SDOAM_ReleaseMutex(gSDMDriver[port].mutex);
            break;
        case SDM_IOCTRL_LOCK_CARD:
            SDOAM_RequestMutex(gSDMDriver[port].mutex);
            if (gSDMDriver[port].bOpen)
            {
                ret = _LockCard(cardId, (uint8 *)pTmp[1]);
                if (ret == SDM_CARD_NOTPRESENT)
                {
                    SDM_Close(cardId);
                }
            }
            else
            {
                ret = SDM_CARD_CLOSED;
            }
            SDOAM_ReleaseMutex(gSDMDriver[port].mutex);
            break;
        case SDM_IOCTRL_UNLOCK_CARD:
            SDOAM_RequestMutex(gSDMDriver[port].mutex);
            if (gSDMDriver[port].bOpen)
            {
                ret = _UnLockCard(cardId, (uint8 *)pTmp[1]);
                if (ret == SDM_CARD_NOTPRESENT)
                {
                    SDM_Close(cardId);
                }
            }
            else
            {
                ret = SDM_CARD_CLOSED;
            }
            SDOAM_ReleaseMutex(gSDMDriver[port].mutex);
            break;
        case SDM_IOCTR_FLUSH:
            SDOAM_RequestMutex(gSDMDriver[port].mutex);
            if (gSDMDriver[port].bOpen)
            {
                ret = SDC_WaitCardBusy(cardId);
            }
            else
            {
                ret = SDM_CARD_CLOSED;
            }
            SDOAM_ReleaseMutex(gSDMDriver[port].mutex);
            break;
#endif
        case SDM_IOCTR_GET_CAPABILITY:
            if (gSDMDriver[port].bOpen)
            {
                pTmp[1] = gSDMDriver[port].cardInfo.capability;
            }
            else
            {
                pTmp[1] = 0;
                ret = SDM_CARD_CLOSED;
            }
            break;
        case SDM_IOCTR_GET_PSN:
            if (gSDMDriver[port].bOpen)
            {
                pTmp[1] = gSDMDriver[port].cardInfo.psn;
            }
            else
            {
                pTmp[1] = (uint32)NULL;
                ret = SDM_CARD_CLOSED;
            }
            break;
        case SDM_IOCTR_IS_CARD_READY:
            if (gSDMDriver[port].bOpen)
            {
                pTmp[1] = TRUE;
            }
            else
            {
                pTmp[1] = FALSE;
                ret = SDM_CARD_CLOSED;
            }
            break;
        case SDM_IOCTR_GET_BOOT_CAPABILITY:
            if (gSDMDriver[port].bOpen)
            {
                pTmp[1] = gSDMDriver[port].cardInfo.bootSize;
            }
            else
            {
                pTmp[1] = 0;
                ret = SDM_CARD_CLOSED;
            }
            break;
        case SDM_IOCTR_INIT_BOOT_PARTITION:
            SDOAM_RequestMutex(gSDMDriver[port].mutex);
            ret = _SwitchBoot(cardId, 1, pTmp[1]);
            SDOAM_ReleaseMutex(gSDMDriver[port].mutex);
            break;
        case SDM_IOCTR_DEINIT_BOOT_PARTITION:
            SDOAM_RequestMutex(gSDMDriver[port].mutex);
            ret = _SwitchBoot(cardId, 0, pTmp[1]);
            SDOAM_ReleaseMutex(gSDMDriver[port].mutex);
            break;
            
       case SDM_IOCTR_SET_BOOT_BUSWIDTH:      //����bootģʽ�µ��߿�       
            SDOAM_RequestMutex(gSDMDriver[port].mutex);
            ret = _SetBootWidth(cardId, pTmp[1], pTmp[2]); 
            SDOAM_ReleaseMutex(gSDMDriver[port].mutex);
            break;    

        case SDM_IOCTR_ACCESS_BOOT_PARTITION: //ѡ���д������; 0--�û�����1--boot1�� 2--boot2;
            SDOAM_RequestMutex(gSDMDriver[port].mutex);
            ret = _AccessBootPartition(cardId, pTmp[1]);
            SDOAM_ReleaseMutex(gSDMDriver[port].mutex);
            break;    
            
        default:
            ret = SDM_PARAM_ERROR;
            break;
    }

    return ret;
}

/****************************************************************/
//������:SDM_SendCmd
//����:������Ӧ�õ��õķ������SDM�ڲ��������ʹ������ӿ�
//����˵��:
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
int32 SDM_SendCmd(int32 cardId,
                            uint32 cmd,
                            uint32 cmdArg,
                            uint32 *responseBuf,
                            uint32  blockSize,
                            uint32  dataLen,
                            void   *pDataBuf)
{
    int32    ret = SDM_SUCCESS;
    uint32   port = SDM_MAX_MANAGER_PORT;

    if ((!SDC_IsCardIdValid(cardId)) || (!_IsCardRegistered(cardId, &port)))
    {
        return SDM_PARAM_ERROR;
    }

    SDOAM_RequestMutex(gSDMDriver[port].mutex);
    if (gSDMDriver[port].bOpen)
    {
        //��ַ4�ֽڶ���
        ret = SDC_BusRequest(cardId, cmd, cmdArg, responseBuf, blockSize, dataLen, pDataBuf);
        if (ret != SDM_SUCCESS)
        {
            SDM_Close(cardId);
        }
    }
    else
    {
        ret = SDM_CARD_CLOSED;
    }
    SDOAM_ReleaseMutex(gSDMDriver[port].mutex);

    return ret;
}

#endif //end of #ifdef DRIVERS_SDMMC

