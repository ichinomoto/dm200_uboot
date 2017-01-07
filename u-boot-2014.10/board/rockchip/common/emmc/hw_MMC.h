/****************************************************************
//    CopyRight(C) 2008 by Rock-Chip Fuzhou
//      All Rights Reserved
//�ļ���:hw_MMC.h
//����:MMC protocol api file
//����:hcy
//��������:2008-11-08
//���ļ�¼:
//��ǰ�汾:1.00
$Log: hw_MMC.h,v $
****************************************************************/
#ifdef DRIVERS_SDMMC

#ifndef _MMCP_API_H_
#define _MMCP_API_H_

/****************************************************************/
//���⺯������
/****************************************************************/
void   MMC_Init(void *pCardInfo);
int32  MMC_SwitchBoot(void *pCardInfo, uint32 enable, uint32 partition);
int32  MMC_AccessBootPartition(void *pCardInfo, uint32 partition);

uint8 MMC_GetMID(void);

#endif //end of #ifndef _MMCP_API_H

#endif //end of #ifdef DRIVERS_SDMMC
