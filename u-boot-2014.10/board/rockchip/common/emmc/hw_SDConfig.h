/****************************************************************
//    CopyRight(C) 2008 by Rock-Chip Fuzhou
//      All Rights Reserved
//�ļ���:hw_SDConfig.h
//����:RK28 SD driver configurable file
//����:hcy
//��������:2008-11-08
//���ļ�¼:
//��ǰ�汾:1.00
$Log: hw_SDConfig.h,v $
****************************************************************/
#ifdef DRIVERS_SDMMC

#ifndef _SD_CONFIG_H_
#define _SD_CONFIG_H_

/********************����******************************/
//����ⷽʽ����
#define SD_CONTROLLER_DET     (1 << 0) //ʹ��SDMMC�������Դ��Ŀ�����
#define SD_GPIO_DET           (1 << 1) //ʹ���ⲿGPIO�������
#define SD_ALWAYS_PRESENT     (1 << 2) //�����Ǵ��ڣ�����β�

#define EMMC_DATA_PART        (0)  // 1
#define EMMC_BOOT_PART        (1)  // 1
#define EMMC_BOOT_PART2       (2)  // 1

#define SD_CARD_Support       (1)


/*****************�����ò���**************************/
//#define SDMMC0_USED           (1)      //��������Ӳ�����Ƿ�ʹ����SDMMC0�˿ڣ�1:ʹ����SDMMC0��������0:û��ʹ��
#define SDMMC0_BUS_WIDTH      (4)      //Ӳ������ΪSDMMC0���������������߿�ȣ�1:һ�������ߣ�4:�ĸ������ߣ�8:�˸������ߣ�����ֵ��֧��
#define SDMMC0_DET_MODE       SD_CONTROLLER_DET      //����ⷽʽѡ�񣬴�SD_CONTROLLER_DET��SD_GPIO_DET��SD_ALWAYS_PRESENT��ѡ��
#define SDMMC0_EN_POWER_CTL   (0)      //�Ƿ��SD����Դ���п��ƣ�1:Ҫ����SD����Դ��0:����SD����Դ���п���

//#define SDMMC1_USED           (1)      //��������Ӳ�����Ƿ�ʹ����SDMMC1�˿ڣ�1:ʹ����SDMMC1��������0:û��ʹ��
#define SDMMC1_BUS_WIDTH      (4)      //Ӳ������ΪSDMMC1���������������߿�ȣ�1:һ�������ߣ�4:�ĸ������ߣ�����ֵ��֧�֣�SDMMC1��������֧��8��������
#define SDMMC1_DET_MODE       SD_GPIO_DET      //����ⷽʽѡ�񣬴�SD_CONTROLLER_DET��SD_GPIO_DET��SD_ALWAYS_PRESENT��ѡ��
#define SDMMC1_EN_POWER_CTL   (0)      //�Ƿ��SD����Դ���п��ƣ�1:Ҫ����SD����Դ��0:����SD����Դ���п���

//#define SDMMC2_USED           (1)      //��������Ӳ�����Ƿ�ʹ����SDMMC1�˿ڣ�1:ʹ����SDMMC1��������0:û��ʹ��
#define SDMMC2_BUS_WIDTH      (8)       //Ӳ������ΪSDMMC1���������������߿�ȣ�1:һ�������ߣ�4:�ĸ������ߣ�����ֵ��֧�֣�SDMMC1��������֧��8��������
#define SDMMC2_DET_MODE       SD_ALWAYS_PRESENT  //����ⷽʽѡ�񣬴�SD_CONTROLLER_DET��SD_GPIO_DET��SD_ALWAYS_PRESENT��ѡ��
#define SDMMC2_EN_POWER_CTL   (0)      //�Ƿ��SD����Դ���п��ƣ�1:Ҫ����SD����Դ��0:����SD����Դ���п���


/* dmac config */
#if defined(CONFIG_RK_MMC_DMA)

/* external mac */
#if defined(CONFIG_RK_MMC_EDMAC) && defined(CONFIG_RK_PL330) && defined(CONFIG_RK_DMAC)
#define EN_SD_DMA             (1)      //�Ƿ���DMA���������ݴ��䣬1:DMA��ʽ��0:�жϷ�ʽ
#define EN_SD_INT             (1)      //�Ƿ�����жϷ�������ѯһЩSDMMC����������Ҫλ��1:���жϷ�ʽ��0:����ѯ��ʽ��Ŀǰ��������ѯ��ʽ������⻹������жϵ�
#else
#define EN_SD_DMA             (0)
#define EN_SD_INT             (0)      //�Ƿ�����жϷ�������ѯһЩSDMMC����������Ҫλ��1:���жϷ�ʽ��0:����ѯ��ʽ��Ŀǰ��������ѯ��ʽ������⻹������жϵ�
#endif

/* internal dmac */
#ifdef CONFIG_RK_MMC_IDMAC
#define EN_SDC_INTERAL_DMA    (1)
#define EN_SD_DATA_TRAN_INT   (0)
#else
#define EN_SDC_INTERAL_DMA    (0)
#define EN_SD_DATA_TRAN_INT   (0)
#endif

#else

#define EN_SD_DMA             (0)
#define EN_SDC_INTERAL_DMA    (0)

#endif /* CONFIG_RK_MMC_DMA */


#define EN_SD_PRINTF          (0)      //�Ƿ�����SD�����ڲ�������Ϣ��ӡ��1:������ӡ��0:�رմ�ӡ
#define DEBOUNCE_TIME         (25)     //���β��������ʱ��,��λms

#define FOD_FREQ              (200)    //��ʶ��׶�ʹ�õ�Ƶ��,��λKHz,Э��涨���400KHz
//���������������Ƶ��ΪFREQ_HCLK_MAX/8
#define SD_FPP_FREQ           (24000)  //��׼SD����������Ƶ�ʣ���λKHz��Э��涨���25MHz
#define SDHC_FPP_FREQ         (40000)  //SDHC���ڸ���ģʽ�µĹ���Ƶ�ʣ���λKHz��Э��涨���50MHz
#define MMC_FPP_FREQ          (18000)  //��׼MMC����������Ƶ�ʣ���λKHz��Э��涨���20MHz
#define MMCHS_26_FPP_FREQ     (25000)  //����ģʽֻ֧�����26M��HS-MMC�����ڸ���ģʽ�µĹ���Ƶ�ʣ���λKHz��Э��涨���26MHz

#if defined(EN_SD_DMA) || (EN_SDC_INTERAL_DMA)
#define MMCHS_52_FPP_FREQ     (40000)  //����ģʽ��֧�����52M��HS-MMC�����ڸ���ģʽ�µĹ���Ƶ�ʣ���λKHz��Э��涨���52MHz
#else
#define MMCHS_52_FPP_FREQ     (40000)  //����ģʽ��֧�����52M��HS-MMC�����ڸ���ģʽ�µĹ���Ƶ�ʣ���λKHz��Э��涨���52MHz
#endif

#if(!EN_SD_PRINTF)
#define SDOAM_Printf(...)
#else
#define SDOAM_Printf printf
#endif

#endif //end of #ifndef _SD_CONFIG_H_
#endif //end of #ifdef DRIVERS_SDMMC

