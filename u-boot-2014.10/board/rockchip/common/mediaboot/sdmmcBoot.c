/********************************************************************************
*********************************************************************************
			COPYRIGHT (c)   2004 BY ROCK-CHIP FUZHOU
				--  ALL RIGHTS RESERVED  --

File Name:  flash.C
Author:     XUESHAN LIN
Created:    1st Dec 2008
Modified:
Revision:   1.00
********************************************************************************
********************************************************************************/
#include	"../emmc/sdmmc_config.h"

#ifdef  DRIVERS_SDMMC

#define EMMC_BOOT_PART_SIZE         1024
#define SD_CARD_BOOT_PART_OFFSET    64
#define EMMC_CARD_ID                2
#define SD_CARD_FW_PART_OFFSET      8192
#define SD_CARD_SYS_PART_OFFSET     8064

typedef struct SDCardInfoTag
{
	uint16 Valid;
	uint16 AccessPart;
	uint32 FwPartOffset;
	uint32 UserPartOffset;
	uint32 UserPartSize;
	uint32 BootCapSize;
	uint32 UserCapSize;
}SD_Card_Info;


#ifdef RK_SDCARD_BOOT_EN
static uint32 gsdboot_mode = 0;
#endif
static SD_Card_Info gSdCardInfoTbl[3];
static uint32	sdmmc_Data[(1024*8*4/4)]; __attribute__((aligned(ARCH_DMA_MINALIGN)));


void SdmmcSDMInit(void)
{
	ftl_memset(gSdCardInfoTbl, 0, sizeof(gSdCardInfoTbl));
}

void emmc_dev_reset(void)
{
	EmmcPowerEn(0);
	DRVDelayMs(5);
	EmmcPowerEn(1);
	DRVDelayMs(1);
}


uint32 SdmmcReinit(uint32 ChipSel)
{
	int32  ret1 = SDM_SUCCESS;
	uint32 ioctlParam[5] = {0,0,0,0,0};
	uint32 retry = 2;

#if EN_SD_DMA
	if (SDPAM_DMAInit(ChipSel) == 0) {
		return -1;
	}
#endif

	SDM_Init(ChipSel);
	sdmmcGpioInit(ChipSel);   //����sdm init֮�󣬱���rst����͵�ƽ

EMMC_INIT_retry:    
	eMMC_SetDataHigh();
	ioctlParam[0] = ChipSel;
	ret1 = SDM_IOCtrl(SDM_IOCTRL_REGISTER_CARD, ioctlParam);
	if(ret1 != SDM_SUCCESS && retry > 0)
	{
		retry--;
		goto EMMC_INIT_retry;
	}
	PRINT_E("SdmmcInit = %x %x\n", ChipSel, ret1);
	if(ret1 == SDM_SUCCESS)  //����ʶ��
	{
		SDM_Open(ChipSel);
	}
	return ret1;
}


uint32 SdmmcInit(uint32 ChipSel)
{
	int32  ret1 = SDM_SUCCESS;
	int32 ret;
	int count =0;
	uint32 ioctlParam[5] = {0,0,0,0,0};

	ret1 = SdmmcReinit(ChipSel);
	if(ret1 == SDM_SUCCESS)  //����ʶ��
	{
#ifdef EMMC_NOT_USED_BOOT_PART
		ioctlParam[0] = ChipSel;
		ioctlParam[1] = 0;  //capbility
		ret1 = SDM_IOCtrl(SDM_IOCTR_GET_BOOT_CAPABILITY, ioctlParam);
		gSdCardInfoTbl[ChipSel].BootCapSize = ioctlParam[1];
		PRINT_E("BootCapSize=%lx\n", gSdCardInfoTbl[ChipSel].BootCapSize);

		gSdCardInfoTbl[ChipSel].BootCapSize = 0;

		ioctlParam[0] = ChipSel;
		ioctlParam[1] = 0;  //capbility
		ret1 = SDM_IOCtrl(SDM_IOCTR_GET_CAPABILITY, ioctlParam);
		gSdCardInfoTbl[ChipSel].UserCapSize = ioctlParam[1];
		PRINT_E("UserCapSize=%lx\n", gSdCardInfoTbl[ChipSel].UserCapSize);

		ret1 = SDM_Read(ChipSel, SD_CARD_BOOT_PART_OFFSET, 4, gIdDataBuf); // id blk data
#ifdef RK_SDCARD_BOOT_EN
		if(ChipSel == 0)
		{
			if(gIdDataBuf[0] == 0xFCDC8C3B)
			{
				gSdCardInfoTbl[ChipSel].FwPartOffset = SD_CARD_FW_PART_OFFSET;
				if(0 == gIdDataBuf[128+104/4]) // sd������
				{
					gsdboot_mode = SDMMC_SDCARD_UPDATE;
					printf("SDCard Update.\n");
				}
				else if(1 == gIdDataBuf[128+104/4])// sd ������
				{
					gsdboot_mode = SDMMC_SDCARD_BOOT;
					printf("SDCard Boot.\n");
				}
			}
			else
			{
				ret1 = -1;
			}
		}
		else
#endif /* RK_SDCARD_BOOT_EN */
		{
			gSdCardInfoTbl[ChipSel].FwPartOffset = SD_CARD_FW_PART_OFFSET;
		}
#endif /* EMMC_NOT_USED_BOOT_PART */
		PRINT_E("FwPartOffset = %lx, %x\n", gSdCardInfoTbl[ChipSel].FwPartOffset, gSdCardInfoTbl[ChipSel].BootCapSize);
		gSdCardInfoTbl[ChipSel].Valid = 1;

		return ret1;
	}

	return ERROR;
}

uint32 SdmmcDeInit(uint32 ChipSel)
{
	int32  ret = SDM_SUCCESS;

#ifndef EMMC_NOT_USED_BOOT_PART
	if(gSdCardInfoTbl[ChipSel].BootCapSize)
	{
		uint32 ioctlParam[5] = {0,0,0,0,0};

		gEmmcBootPart = EMMC_BOOT_PART;
		gSdCardInfoTbl[EMMC_CARD_ID].AccessPart = EMMC_BOOT_PART;
		ioctlParam[0] = EMMC_CARD_ID;
		ioctlParam[1] = EMMC_BOOT_PART;
		SDM_IOCtrl(SDM_IOCTR_ACCESS_BOOT_PARTITION, ioctlParam);
		eMMC_Switch_ToMaskRom();
		emmc_dev_reset();
	}
#endif

	return ret;
}

#ifndef EMMC_NOT_USED_BOOT_PART
uint32 EmmcSetBootPart(uint32 ChipSel, uint32 BootPart, uint32 AccessPart)
{
	//д�ڶ������ݣ��ӵ�һ��boot������
	if(gEmmcBootPart != BootPart || gSdCardInfoTbl[ChipSel].AccessPart != AccessPart)
	{
		uint32 ioctlParam[5] = {0,0,0,0,0};
		gEmmcBootPart = BootPart;
		gSdCardInfoTbl[ChipSel].AccessPart = AccessPart;
		ioctlParam[0] = ChipSel;
		ioctlParam[1] = AccessPart;
		SDM_IOCtrl(SDM_IOCTR_ACCESS_BOOT_PARTITION, ioctlParam);
	}
}
#endif

uint32 SdmmcBootWritePBA(uint8 ChipSel, uint32 PBA, void *pbuf, uint32 nSec )
{
	uint32 i;
	uint16 len;
	uint8 pageSizeRaw;
	uint8 pageSizeLimit;
	uint32 BlockOffset;
	uint16 PageOffset;
	uint32 *pDataBuf = pbuf;

	pageSizeLimit = 32;
#ifndef EMMC_NOT_USED_BOOT_PART
	if(gSdCardInfoTbl[ChipSel].BootCapSize > 0)
	{
		if(PBA + nSec >= EMMC_BOOT_PART_SIZE*2) //�����˲�д
			return 0;

		if(PBA + nSec >= EMMC_BOOT_PART_SIZE) //�����˲�д
		{
			//д�ڶ������ݣ��ӵ�һ��boot������
			EmmcSetBootPart(ChipSel, EMMC_BOOT_PART, EMMC_BOOT_PART2);
			PBA &= (EMMC_BOOT_PART_SIZE-1);
		}
		else
		{
			//д��һ�����ݣ��ӵڶ���boot������
			EmmcSetBootPart(ChipSel, EMMC_BOOT_PART2, EMMC_BOOT_PART);
		}
	}
	else
#endif
	{
		if(gSdCardInfoTbl[ChipSel].FwPartOffset == SD_CARD_FW_PART_OFFSET)
		{
			if(PBA + nSec >= EMMC_BOOT_PART_SIZE*5) //�����˲�д
				return 0;
			PBA = PBA + SD_CARD_BOOT_PART_OFFSET;
		}
		else
		{
			return -1;//�쳣
		}
	}

	for (len=0; len<nSec; len+=pageSizeLimit)
	{
		for (i=0; i<(MIN(nSec, pageSizeLimit)); i++)
		{
			ftl_memcpy(sdmmc_Data+i*128, pDataBuf+(len+i)*132, 512);
		}
		SDM_Write(ChipSel, PBA+len, (MIN(nSec, pageSizeLimit)), sdmmc_Data);
	}

	return 0;
}

uint32 SdmmcBootReadPBA(uint8 ChipSel, uint32 PBA, void *pbuf, uint32 nSec )
{
	uint32 i;
	uint32 ret;
	uint16 len;
	uint8 pageSizeRaw;
	uint8 pageSizeLimit;
	uint32 BlockOffset;
	uint16 PageOffset;
	uint16 PageAlignOffset;
	uint16 idblk_flag = 0;
	uint16 read_len;
	uint32 *pDataBuf = pbuf;

	pageSizeLimit = 32;

#ifndef EMMC_NOT_USED_BOOT_PART
	if(gSdCardInfoTbl[ChipSel].BootCapSize > 0)
	{
		if(PBA + nSec >= EMMC_BOOT_PART_SIZE*2) //������ֻ����һ��
			PBA &= (EMMC_BOOT_PART_SIZE-1);

		if(PBA + nSec >= EMMC_BOOT_PART_SIZE)
		{
			//���ڶ������ݣ��ӵ�һ��boot������
			EmmcSetBootPart(ChipSel, EMMC_BOOT_PART, EMMC_BOOT_PART2);
			PBA &= (EMMC_BOOT_PART_SIZE-1);
		}
		else
		{
			//����һ������ ���ӵ�һ��boot������
			EmmcSetBootPart(ChipSel, EMMC_BOOT_PART, EMMC_BOOT_PART);
		}
	}
	else
#endif
	{
		if(gSdCardInfoTbl[ChipSel].FwPartOffset == SD_CARD_FW_PART_OFFSET)
		{
			if(PBA + nSec >= EMMC_BOOT_PART_SIZE*5) //�����˲�д
				PBA &= (EMMC_BOOT_PART_SIZE-1);
			PBA = PBA + SD_CARD_BOOT_PART_OFFSET;
		}
		else
		{
			return -1;//�쳣
		}
	}
        
	for (len=0; len<nSec; len+=pageSizeLimit)
	{
		SDM_Read(ChipSel, PBA+len, (MIN(nSec,pageSizeLimit)), sdmmc_Data);
		for (i=0; i<(MIN(nSec, pageSizeLimit)); i++)
		{
			ftl_memcpy(pDataBuf+(len+i)*132, sdmmc_Data+(i)*128, 512);
		}
	}

	return 0;
}

uint32 SdmmcBootReadLBA(uint8 ChipSel, uint32 LBA, void *pbuf, uint32 nSec)
{
	uint32 iret = FTL_OK;
	uint32 try_count = 2;

ReadRetry:
#ifndef EMMC_NOT_USED_BOOT_PART
	if(gSdCardInfoTbl[ChipSel].BootCapSize > 0)
	{
		EmmcSetBootPart(ChipSel,EMMC_BOOT_PART,EMMC_DATA_PART);
	}
	else
#endif
	{
		if(gSdCardInfoTbl[ChipSel].FwPartOffset == SD_CARD_FW_PART_OFFSET)
		{
			;//LBA = LBA + SD_CARD_FW_PART_OFFSET;
		}
		else
		{
			return -1;//�쳣
		}
	}
	iret = SDM_Read(ChipSel,LBA + gSdCardInfoTbl[ChipSel].FwPartOffset,nSec,pbuf);
	if(iret != FTL_OK && try_count > 0)
	{
		SdmmcDeInit(ChipSel);
		SdmmcInit(ChipSel);
		try_count--;
		//RkPrintf("SDM_Read FLT_ERROR, LBA=%x, iret = %x\n", LBA, iret);
		goto ReadRetry;
	}

	return (iret);
}

uint32 SdmmcBootWriteLBA(uint8 ChipSel, uint32 LBA, void *pbuf, uint32 nSec, uint16 mode)
{
	uint32 iret = FTL_OK;

#ifndef EMMC_NOT_USED_BOOT_PART
	if(gSdCardInfoTbl[ChipSel].BootCapSize > 0)
	{
		EmmcSetBootPart(ChipSel, EMMC_BOOT_PART, EMMC_DATA_PART);
	}
	else
#endif
	{
		if(gSdCardInfoTbl[ChipSel].FwPartOffset == SD_CARD_FW_PART_OFFSET)
		{
			;//LBA = LBA + SD_CARD_FW_PART_OFFSET;
		}
		else
		{
			return -1;//�쳣
		}
	}
	iret = SDM_Write(ChipSel, LBA + gSdCardInfoTbl[ChipSel].FwPartOffset, nSec, pbuf);
	//if(iret != FTL_OK)
	//    RkPrintf("SDM_Write FLT_ERROR, LBA=%x, iret = %x\n", LBA, iret);

	return iret;
}


void SdmmcReadID(uint8 ChipSel, void *buf)
{
	uint8 * pbuf = buf;

	pbuf[0] = 'E';
	pbuf[1] = 'M';
	pbuf[2] = 'M';
	pbuf[3] = 'C';
	pbuf[4] = ' ';
}

void SdmmcReadFlashInfo(void *buf)
{
	pFLASH_INFO pInfo = (pFLASH_INFO)buf;

	pInfo->BlockSize = EMMC_BOOT_PART_SIZE;
	pInfo->ECCBits = 0;
	pInfo->FlashSize = gSdCardInfoTbl[EMMC_CARD_ID].UserCapSize;
#ifdef RK_SDCARD_BOOT_EN
	if(pInfo->FlashSize == 0)
	{
		pInfo->FlashSize = gSdCardInfoTbl[0].UserCapSize;
	}
#endif
	pInfo->PageSize = 4;
	pInfo->AccessTime = 40;
	pInfo->ManufacturerName = 0;
	pInfo->FlashMask = 0;
	if(pInfo->FlashSize)
	{
		pInfo->FlashMask = 1;
	}
}

uint32 SdmmcGetCapacity(uint8 ChipSel)
{
	return gSdCardInfoTbl[ChipSel].UserCapSize - gSdCardInfoTbl[ChipSel].FwPartOffset;
}

uint32 SdmmcSysDataLoad(uint8 ChipSel, uint32 Index, void *Buf)
{
	uint32 ret = FTL_ERROR;

	PRINT_E("SdmmcSysDataLoad: %x, %x\n", ChipSel, Index);
	if(gSdCardInfoTbl[ChipSel].FwPartOffset == SD_CARD_FW_PART_OFFSET)
	{
#ifndef EMMC_NOT_USED_BOOT_PART
		if(gSdCardInfoTbl[ChipSel].BootCapSize > 0)
		{
			EmmcSetBootPart(ChipSel, EMMC_BOOT_PART, EMMC_DATA_PART);
		}
#endif
		ret = SDM_Read(ChipSel, SD_CARD_SYS_PART_OFFSET + Index, 1, Buf);
		PRINT_E("SdmmcSysDataLoad , %x, %x ret=%x\n", ChipSel, Index, ret);
	}

	return ret;
}

uint32 SdmmcSysDataStore(uint8 ChipSel, uint32 Index,void *Buf)
{
	uint32 ret = FTL_ERROR;

	PRINT_E("SdmmcSysDataStore: %x, %x\n", ChipSel, Index);
	if(gSdCardInfoTbl[ChipSel].FwPartOffset == SD_CARD_FW_PART_OFFSET)
	{
#ifndef EMMC_NOT_USED_BOOT_PART
		if(gSdCardInfoTbl[ChipSel].BootCapSize > 0)
		{
			EmmcSetBootPart(ChipSel, EMMC_BOOT_PART, EMMC_DATA_PART);
		}
#endif
		ret = SDM_Write(ChipSel, SD_CARD_SYS_PART_OFFSET + Index, 1, Buf);
		PRINT_E("SdmmcSysDataStore: %x, %x ret=%x\n", ChipSel, Index, ret);
	}

	return ret;
}

uint32 SdmmcGetFwOffset(uint8 ChipSel)
{
	uint32 offset = 0;

	if(gSdCardInfoTbl[ChipSel].FwPartOffset == SD_CARD_FW_PART_OFFSET)
	{
		offset = SD_CARD_FW_PART_OFFSET;
	}

	return offset;
}

uint32 SdmmcGetSysOffset(uint8 ChipSel)
{
	uint32 offset = 0;

	if(gSdCardInfoTbl[ChipSel].FwPartOffset == SD_CARD_FW_PART_OFFSET)
	{
		offset = SD_CARD_SYS_PART_OFFSET;
	}

	return offset;
}


/***************************************************************************
��������:������BLOCK��������0:�ÿ�; 1:����
��ڲ���:ChipSel, blkIndex=��ʼblock��,  nblk=blk�� ��mod: 0 Ϊ��ͨ������ 1Ϊǿ�Ʋ���
���ڲ���:0 Ϊû�л��飬1Ϊ�л���
���ú���:��
***************************************************************************/
uint32 SdmmcBootErase(uint8 ChipSel, uint32 blkIndex, uint32 nblk, uint8 mod)
{
	return OK;
}


void SdmmcCheckIdBlock(void)
{

}

#ifdef RK_SDCARD_BOOT_EN
uint32 SdmmcGetSDCardBootMode(void)
{
	return gsdboot_mode;
}

uint32 BootFromSdCard(uint8 ChipSel)
{
	uint32 ret = -1;

	if(gSdCardInfoTbl[ChipSel].FwPartOffset == SD_CARD_FW_PART_OFFSET)
	{
		if(0 == gIdDataBuf[128+104/4]) // sd������
		{
			ret = 0;//����0����ʼ���ɹ�
		}
		else if(1 == gIdDataBuf[128+104/4])// sd ������
		{
			//check sd ��
			PRINT_E("%s\n","run on sd0");
			ret = 0;//����0����ʼ���ɹ�
		}
	}

	return ret;
}
#endif

#endif

