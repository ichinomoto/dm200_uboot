/****************************************************************
//    CopyRight(C) 2008 by Rock-Chip Fuzhou
//      All Rights Reserved
//�ļ���:hw_SDPlatAdapt.c
//����:RK28 SD/MMC driver Platform adaptation implement file
//����:hcy
//��������:2008-11-08
//���ļ�¼:
//��ǰ�汾:1.00
$Log: hw_SDPlatAdapt.c,v $
****************************************************************/

#include "sdmmc_config.h"

#ifdef DRIVERS_SDMMC

/****************************************************************/
//������:SDPAM_FlushCache
//����:���cache
//����˵��:adr      �������     ��Ҫ�������ʼ��ַ
//         size     �������     ��Ҫ����Ĵ�С����λ�ֽ�
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
void   SDPAM_FlushCache(void *adr, uint32 size)
{
#if(EN_SD_DMA)
#ifdef MMUCACHE_ENABLE
	CacheFlushDRegion((uint32)adr,(uint32)size);
#endif
#endif
}

/****************************************************************/
//������:SDPAM_CleanCache
//����:����cache
//����˵��:adr      �������     ��Ҫ�������ʼ��ַ
//         size     �������     ��Ҫ����Ĵ�С����λ�ֽ�
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
void   SDPAM_CleanCache(void *adr, uint32 size)
{

}

/****************************************************************/
//������:SDPAM_GetAHBFreq
//����:�õ���ǰAHB����Ƶ��
//����˵��:
//����ֵ:���ص�ǰAHB����Ƶ�ʣ���λKHz
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
uint32 SDPAM_GetAHBFreq(SDMMC_PORT_E nSDCPort)
{
#if SDMMC_NO_PLATFORM
    return 25000;
#else
    return GetMmcCLK(nSDCPort);
#endif
}

/****************************************************************/
//������:SDPAM_SDCClkEnable
//����:ѡ���Ƿ���SDMMC�������Ĺ���ʱ��
//����˵��:nSDCPort   �������   �˿ں�
//         enable     �������   �Ƿ�ʹ��
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
void SDPAM_SDCClkEnable(SDMMC_PORT_E nSDCPort, uint32 enable)
{

}

/****************************************************************/
//������:SDPAM_SDCReset
//����:��SCU�ϸ�λSDMMC������
//����˵��:nSDCPort   �������   �˿ں�
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
void SDPAM_SDCReset(SDMMC_PORT_E nSDCPort)
{
    if(nSDCPort == SDC0)
    {
        SDCReset(0);
    }
    else if (nSDCPort == SDC1)
    {
        SDCReset(1);
    }
    else
    {
        SDCReset(2);
    }
}

/****************************************************************/
//������:SDPAM_SetMmcClkDiv
//����:����SCU��mmc_clk_div�ķ�Ƶֵ
//����˵��:nSDCPort   �������   �˿ں�
//         div        �������   ��Ƶֵ
//����ֵ:���ص�ǰAHB����Ƶ�ʣ���λKHz
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
void   SDPAM_SetMmcClkDiv(SDMMC_PORT_E nSDCPort, uint32 div)
{
#if SDMMC_NO_PLATFORM
    return;
#else
    if(nSDCPort == SDC0)
    {
        SCUSelSDClk(0, div);
    }
    else if (nSDCPort == SDC1)
    {
        SCUSelSDClk(1, div);
    }
    else
    {
        SCUSelSDClk(2, div);
    }
#endif
}

#if EN_SD_DMA
/****************************************************************/
//������:SDPAM_DMAStart
//����:����һ��DMA����
//����˵��:nSDCPort   �������   ��Ҫ���ݴ���Ķ˿ں�
//         dstAddr    �������   Ŀ���ַ
//         srcAddr    �������   Դ��ַ
//         size       �������   ���ݳ��ȣ���λ�ֽ�
//         rw         �������   ��ʾ������Ҫ�ӿ���������д������1:д������0:�ӿ�����
//         cb_f       �������   DMA������Ļص�����
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
bool SDPAM_DMAStart(SDMMC_PORT_E nSDCPort, uint32 dstAddr, uint32 srcAddr, uint32 size, bool rw, pFunc cb_f)
{
	uint32 dmac_chn = 0;
	uint32 src_addr, dst_addr;
	uint32 mode;
	int ret = 0;

//	printf("dstAddr = 0x%x, srcAddr = 0x%x, size = 0x%x, rw = %d\n", dstAddr, srcAddr, size, rw);
	if (nSDCPort == SDC0) {
		dmac_chn = DMACH_SDMMC;
	} else if (nSDCPort == SDC1) {
		dmac_chn = DMACH_SDIO;
	} else {
		dmac_chn = DMACH_EMMC;
	}

	if (rw) {
		mode = RK_DMASRC_MEM;
		src_addr = dstAddr;
		dst_addr = srcAddr;
	} else {
		mode = RK_DMASRC_HW;
		src_addr = srcAddr;
		dst_addr = dstAddr;
	}

	if (rk_dma_set_buffdone_fn(dmac_chn, cb_f) < 0) {
		printf("dma ch = %d set buffdone fail!\n", dmac_chn);
		return FALSE;
	}
	rk_dma_devconfig(dmac_chn, mode, src_addr);
	rk_dma_enqueue(dmac_chn, NULL, dst_addr, size << 2);
	ret = rk_dma_ctrl(dmac_chn, RK_DMAOP_START);

	if (ret < 0) {
		return FALSE;
	} else {
		return TRUE;
	}
}

/****************************************************************/
//������:SDPAM_DMAStop
//����:ֹͣһ���Ѿ����ù���DMA����
//����˵��:nSDCPort   �������   ��Ҫֹͣ�Ķ˿ں�
//         rw         �������   ��ʾֹͣ��������Ҫ�ӿ������Ĳ�������д�����Ĳ�����1:д������0:�ӿ�����
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
bool SDPAM_DMAStop(SDMMC_PORT_E nSDCPort, bool rw)
{
	uint32 dmac_chn = 0;
	int ret;

	if (nSDCPort == SDC0) {
		dmac_chn = DMACH_SDMMC;
	} else if (nSDCPort == SDC1) {
		dmac_chn = DMACH_SDIO;
	} else {
		dmac_chn = DMACH_EMMC;
	}

	ret = rk_dma_ctrl(dmac_chn, RK_DMAOP_STOP);
	if (ret < 0) {
		return FALSE;
	}
	rk_dma_ctrl(dmac_chn, RK_DMAOP_FLUSH);

	return TRUE;
}


static struct rk_dma_client rk_dma_sd_client = {
	.name = "rk-dma-sd",
};
static struct rk_dma_client rk_dma_sdio_client = {
	.name = "rk-dma-sdio",
};
static struct rk_dma_client rk_dma_emmc_client = {
	.name = "rk-dma-emmc",
};

bool SDPAM_DMAInit(SDMMC_PORT_E nSDCPort)
{
	struct rk_dma_client *dma_client = NULL;
	uint32 dmac_chn = 0;

	if (nSDCPort == SDC0) {
		dma_client = &rk_dma_sd_client;
		dmac_chn = DMACH_SDMMC;
	} else if (nSDCPort == SDC1) {
		dma_client = &rk_dma_sdio_client;
		dmac_chn = DMACH_SDIO;
	} else {
		dma_client = &rk_dma_emmc_client;
		dmac_chn = DMACH_EMMC;
	}

	if (rk_dma_request(dmac_chn, dma_client, NULL) < 0) {
		printf("Dmac request ch = %d fail!\n", dmac_chn);
		return FALSE;
	}

	if (rk_dma_config(dmac_chn, 4, 16) < 0) {
		printf("Dmac ch = %d config fail!\n", dmac_chn);
		return FALSE;
	}

	return TRUE;
}
#endif /* EN_SD_DMA */

/****************************************************************/
//������:SDPAM_INTCRegISR
//����:���жϿ�����ע��ĳ���˿ڵ��жϷ����߳�
//����˵��:nSDCPort   �������   ��Ҫע��Ķ˿ں�
//         Routine    �������   �����߳�
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
uint32   SDPAM_INTCRegISR(SDMMC_PORT_E nSDCPort, pFunc Routine)
{
#if SDMMC_NO_PLATFORM
	return TRUE;
#else
	if (nSDCPort == SDC0) {
		irq_install_handler(RKPLAT_IRQ_SDMMC, Routine, NULL);
	} else if (nSDCPort == SDC1) {
		irq_install_handler(RKPLAT_IRQ_SDIO, Routine, NULL);
	} else {
		irq_install_handler(RKPLAT_IRQ_EMMC, Routine, NULL);
	}
    return TRUE;
#endif
}

/****************************************************************/
//������:SDPAM_INTCEnableIRQ
//����:ʹ���жϿ�������ĳ�˿ڵ��ж�
//����˵��:nSDCPort   �������   ��Ҫʹ�ܵĶ˿ں�
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
uint32   SDPAM_INTCEnableIRQ(SDMMC_PORT_E nSDCPort)
{
#if SDMMC_NO_PLATFORM
	return TRUE;
#else
	uint32 ret = 0;
	if(nSDCPort == SDC0)
	{
		ret = irq_handler_enable(RKPLAT_IRQ_SDMMC);
	}
	else if(nSDCPort == SDC1)
	{
		ret = irq_handler_enable(RKPLAT_IRQ_SDIO);
	}
	else
	{
		ret = irq_handler_enable(RKPLAT_IRQ_EMMC);
	}

	if(ret == 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
#endif
}

/****************************************************************/
//������:SDPAM_IOMUX_SetSDPort
//����:��IO���õ�ĳ���˿ڣ����Ҹö˿ڵ������߿����widthָ��
//����˵��:nSDCPort   �������   �˿ں�
//         width      �������   �����߿��
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
uint32   SDPAM_IOMUX_SetSDPort(SDMMC_PORT_E nSDCPort, HOST_BUS_WIDTH_E width)
{
	return TRUE;
}


/****************************************************************/
//������:SDPAM_ControlPower
//����:����ָ���˿ڵ�card��Դ������ر�
//����˵��:nSDCPort �������   �˿ں�
//         enable   �������   1:������Դ��0:�رյ�Դ
//����ֵ:
//���ȫ�ֱ���:
//ע��:
/****************************************************************/
void SDPAM_ControlPower(SDMMC_PORT_E nSDCPort, uint32 enable)
{

}

#endif //end of #ifdef DRIVERS_SDMMC
