/****************************************************************
//    CopyRight(C) 2008 by Rock-Chip Fuzhou
//      All Rights Reserved
//�ļ���:hw_SD.h
//����:SD protocol api file
//����:hcy
//��������:2008-11-08
//���ļ�¼:
//��ǰ�汾:1.00
$Log: hw_SD.h,v $
****************************************************************/
#ifdef DRIVERS_SDMMC

#ifndef _SDP_API_H_
#define _SDP_API_H_

/****************************************************************/
//���⺯������
/****************************************************************/
void   SD1X_Init(void *pCardInfo);
void   SD20_Init(void *pCardInfo);

#endif //end of #ifndef _SDP_API_H

#endif //end of #ifdef DRIVERS_SDMMC
