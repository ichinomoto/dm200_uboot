/****************************************************************
//    CopyRight(C) 2008 by Rock-Chip Fuzhou
//      All Rights Reserved
//�ļ���:hwapi_SDM.h
//����:SD manageer api
//����:hcy
//��������:2008-11-08
//���ļ�¼:
//��ǰ�汾:1.00
$Log: hwapi_SDM.h,v $
****************************************************************/
#ifdef DRIVERS_SDMMC

#ifndef _SDM_API_
#define _SDM_API_

#include "hw_SDCommon.h"

/* SDM return value */
#define SDM_SUCCESS              (0)                     //�����ɹ�
#define SDM_FALSE                (0x1 << 0)              //����ʧ��
#define SDM_CARD_NOTPRESENT      (0x1 << 1)              //Ҫ�����Ŀ����ڿ�����
#define SDM_PARAM_ERROR          (0x1 << 2)              //�����д�
#define SDM_RESP_ERROR           (0x1 << 3)              //���Ļظ�����
#define SDM_RESP_CRC_ERROR       (0x1 << 4)              //���Ļظ�CRCУ�����
#define SDM_RESP_TIMEOUT         (0x1 << 5)              //���Ļظ�timeout
#define SDM_DATA_CRC_ERROR       (0x1 << 6)              //��������CRC����
#define SDM_DATA_READ_TIMEOUT    (0x1 << 7)              //����������timeout
#define SDM_END_BIT_ERROR        (0x1 << 8)              //���ݽ���λ����
#define SDM_START_BIT_ERROR      (0x1 << 9)              //������ʼλ����
#define SDM_BUSY_TIMEOUT         (0x1 << 10)             //busyʱ��̫����
#define SDM_DMA_BUSY             (0x1 << 11)             //dma busy
#define SDM_ERROR                (0x1 << 12)             //SDMMC host controller error
#define SDM_VOLTAGE_NOT_SUPPORT  (0x1 << 13)             //���Ĺ�����ѹ����host���ṩ��Χ�ڣ������޷���������
#define SDM_FUNC_NOT_SUPPORT     (0x1 << 14)             //Ҫ����еĲ���������֧��
#define SDM_UNKNOWABLECARD       (0x1 << 15)             //����ʶ�Ŀ�
#define SDM_CARD_WRITE_PROT      (0x1 << 16)             //����д����
#define SDM_CARD_LOCKED          (0x1 << 17)             //������ס��
#define SDM_CARD_CLOSED          (0x1 << 18)             //���Ѿ�������SDM_Close�ر���

/* SDM IOCTRL cmd */
#define SDM_IOCTRL_REGISTER_CARD         (0x0)           //ע��һ�ſ�
#define SDM_IOCTRL_UNREGISTER_CARD       (0x1)           //ע��һ�ſ�
#define SDM_IOCTRL_SET_PASSWORD          (0x2)           //��������
#define SDM_IOCTRL_CLEAR_PASSWORD        (0x3)           //�������
#define SDM_IOCTRL_FORCE_ERASE_PASSWORD  (0x4)           //ǿ�Ʋ������룬���ڵ��������ݶ�����ʧ
#define SDM_IOCTRL_LOCK_CARD             (0x5)           //������û������Ŀ�������ɹ�
#define SDM_IOCTRL_UNLOCK_CARD           (0x6)           //����
#define SDM_IOCTR_GET_CAPABILITY         (0x7)           //��ȡ����
#define SDM_IOCTR_GET_PSN                (0x8)           //�õ�����Product serial number
#define SDM_IOCTR_IS_CARD_READY          (0x9)           //�õ����Ƿ�׼������������
#define SDM_IOCTR_FLUSH                  (0xA)           //�Կ�����flush����
#define SDM_IOCTR_GET_BOOT_CAPABILITY    (0xB)           //��ȡBoot partition����
#define SDM_IOCTR_INIT_BOOT_PARTITION    (0xC)           //�л���R/W Boot partition,�����л����ĸ�boot partition��IOCTL��������
#define SDM_IOCTR_DEINIT_BOOT_PARTITION  (0xD)           //�رն�boot partition�ķ��ʣ��л���user area
#define SDM_IOCTR_ACCESS_BOOT_PARTITION  (0xE)           //Access boot partition or user area
#define SDM_IOCTR_SET_BOOT_BUSWIDTH      (0xF)           //��������ģʽ�µ��߿�
#define SDM_IOCTR_SET_BOOT_PART_SIZE     (0x10)           //��������ģʽ�µ��߿�


/****************************************************************/
//���⺯������
/****************************************************************/
void   SDM_Init(uint32 CardId);
int32  SDM_Open(int32 cardId);
int32  SDM_Close(int32 cardId);
int32  SDM_Read(int32 cardId, uint32 blockNum, uint32 blockCount, void *pBuf);
int32  SDM_Write(int32 cardId, uint32 blockNum, uint32 blockCount, void *pBuf);
int32  SDM_IOCtrl(uint32 cmd, void *param);

//ר�Ÿ�CMMBʹ�õ�
int32 SDM_SendCmd(int32 cardId,
                            uint32 cmd,
                            uint32 cmdArg,
                            uint32 *responseBuf,
                            uint32  blockSize,
                            uint32  dataLen,
                            void   *pDataBuf);

#endif //end of #ifndef _SDM_API_

#endif //endi of #ifdef DRIVERS_SDMMC
