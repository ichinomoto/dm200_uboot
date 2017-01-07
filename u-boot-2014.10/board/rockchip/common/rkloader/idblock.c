/*
 * (C) Copyright 2008-2014 Rockchip Electronics
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <malloc.h>
#include "../config.h"

DECLARE_GLOBAL_DATA_PTR;


char bootloader_ver[24] = "";
uint16 internal_boot_bloader_ver = 0;
uint16 update_boot_bloader_ver = 0;
uint32 g_id_block_size = 1024;

uint8* g_32secbuf;
uint8* g_cramfs_check_buf;

uint8* g_pIDBlock;
uint8* g_pLoader;
uint8* g_pReadBuf;
uint8* g_pFlashInfoData;

FlashInfo m_flashInfo;
uint16 g_IDBlockOffset[5];

extern uint32 gIdDataBuf[512];

extern void P_RC4(unsigned char * buf, unsigned short len);


static void GetIdblockDataNoRc4(char * fwbuf, int len)
{
	P_RC4((unsigned char*)fwbuf, len);
}


#define CALC_UNIT(a, b)		((a>0)?((a-1)/b+1):(a))			// ����a���Էֳɶ��ٸ�b��ʣ�ಿ����1��
#define MB2SECTOR(x)		(x*1024*(1024/SECTOR_SIZE))		// x MB�����������Sector��
#define BYTE2SECTOR(x)		(CALC_UNIT(x, SECTOR_SIZE))		// x Bytes�����Sector��
#define INT2BCD(num) (((num)%10)|((((num)/10)%10)<<4)|((((num)/100)%10)<<8)|((((num)/1000)%10)<<12))
#define PAGEALIGN(x)		(CALC_UNIT(x, 4))//x sectors�����page��

/***********************************************************
 *  Copyright (C),2007-2008, Fuzhou Rockchip Co.,Ltd.
 *  Function name : BuildNFBlockStateMap()
 *  Author:         Meiyou Chen
 *  Description:    ����ָ��Flash�Ŀ�״̬��
 *  Calls:          TestBadBlock()
 *  Input:          ucFlashIndex - ָ����Flash, uiNFBlockLen - �����ɵĿ���
 *  Output:         NFBlockState - Flash��״̬��
 *  Return:         TRUE - �ɹ�	FALSE - ʧ��
 *  History:        
 *           <author>      <time>     <version>       <desc>
 *          Meiyou Chen   07/12/20       1.0            ORG
 *
 **********************************************************/
static uint32 BuildNFBlockStateMap(uint8 ucFlashIndex, uint8 *NFBlockState, uint32 uiNFBlockLen)
{
	return TRUE;
}

/*************************** ����ָ��Flash�����п��״̬�� ******************************/
static uint32 BuildFlashStateMap(uint8 ucFlashIndex, FlashInfo *pFlash)
{
	memset((void*)pFlash->BlockState, 0, 200);
	return BuildNFBlockStateMap(ucFlashIndex, pFlash->BlockState, 200);
}

/***********************************************************
 *  Copyright (C),2007-2008, Fuzhou Rockchip Co.,Ltd.
 *  Function name :	FindSerialBlocks()
 *  Author:			Meiyou Chen
 *  Description:		����ָ��λ�ÿ�ʼ����������ÿ��λ��
 *  Calls:			
 *  Input:			NFBlockState - ��״̬��, iNFBlockLen - ��״̬�������Ŀ�ĸ���,
 *					iBegin - ��ʼ��, iLen - �����Ŀ���
 *  Output:			
 *  Return:			-1 - ʧ��, ����ֵָʾ�ҵ���λ��
 *  History:			
 *           <author>      <time>     <version>       <desc>
 *          Meiyou Chen   07/12/20       1.0            ORG
 *
 **********************************************************/
static int FindSerialBlocks(uint8 *NFBlockState, int iNFBlockLen, int iBegin, int iLen)
{
	int iCount = 0;
	int iIndex = iBegin;
	while(iBegin < iNFBlockLen)
	{
		if(0 == NFBlockState[iBegin++])
			++iCount;
		else
		{
			iCount = 0;
			iIndex = iBegin;
		}
		if(iCount >= iLen)
			break;
	}
	if(iBegin >= iNFBlockLen)
		iIndex = -1;

	return iIndex;
}

//Ѱ��ID Block���ÿ���λ����ǰ���20���ÿ��������ҵ����������ǵ�Ƭ�ӣ��������������˾������Ƭ
static int FindIDBlock(FlashInfo* pFlashInfo, int iStart, int *iPos)
{
	ALLOC_CACHE_ALIGN_BUFFER(u8, ucSpareData, 4*528);
	int iRet = ERR_SUCCESS;
	int i = FindSerialBlocks(pFlashInfo->BlockState, MAX_BLOCK_SEARCH/*MAX_BLOCK_STATE*/, iStart, 1);

	*iPos = 0;
	if (i < 0) {
		return -1;
	}

	for(; i<MAX_BLOCK_SEARCH/*MAX_BLOCK_STATE*/; i=FindSerialBlocks(pFlashInfo->BlockState, MAX_BLOCK_SEARCH/*MAX_BLOCK_STATE*/, i+1, 1))
	{
		if (i < 0)
			break;
		memset(ucSpareData, 0, 4*528);
		iRet = StorageReadPba(i*g_id_block_size, ucSpareData, 4);

		if(ERR_SUCCESS != iRet)
		{
			continue;
		}
		unsigned int tag = 0;
		memcpy(&(tag), ucSpareData, sizeof(tag));
		if (tag == (unsigned int)0xfcdc8c3b) 
		{
			*iPos = i;
			return 0;//�ҵ�idb
		}
		else
		{
			continue;
		}
	}

	return -1;
}

static int FindAllIDB(void)
{
	int i, iRet, iIndex, iStart = 0, iCount = 0;

	if (g_FlashInfo.BlockSize)
		g_id_block_size = g_FlashInfo.BlockSize;

	memset(g_IDBlockOffset, 0xFFFF, 5);
	for(i=0; i<5; i++)
	{
		iRet = FindIDBlock( &m_flashInfo, iStart, &iIndex );
		if (iRet < 0)
		{
			return iCount;
		}
		g_IDBlockOffset[i] = iIndex;
		iCount++;
		iStart = iIndex+1;
	}

	return iCount;
}

#define SECTOR_OFFSET 528

extern uint16 update_boot_bloader_ver;
extern uint16 internal_boot_bloader_ver;
static bool GenericIDBData(PBYTE pIDBlockData, UINT *needIdSectorNum)
{
	int iRet, i;
	UINT uiSectorPerBlock;
	Sector1Info *pSec1;
	Sector0Info *pSec0;
	//	Sector3Info *pSec3;
	RK28BOOT_HEAD *hdr = NULL;
	int hasFlashInfo = 0;

	//cmy: ��ȡ���е�IDBlock�������ݣ������н���

	//1.����ǰ4��secotor
	uiSectorPerBlock = g_id_block_size;
	memset((void*)pIDBlockData, 0, 4*SECTOR_OFFSET);

	iRet = StorageReadPba(g_IDBlockOffset[0]*uiSectorPerBlock, (void*)pIDBlockData, 4);
	if (iRet != ERR_SUCCESS)
	{
		PRINT_I("ERROR: %d\n", iRet);
		return FALSE;
	}

	//2.IDB����
	PRINT_I("DDDDD...\n");
	for(i=0; i<4; i++)
	{
		if(i != 1)
			P_RC4(pIDBlockData+SECTOR_OFFSET*i, 512);
	}
	memset(pIDBlockData+SECTOR_OFFSET*2, 0, SECTOR_OFFSET ); //chip info
	PRINT_I("OK\n");

	pSec0 = (Sector0Info *)(pIDBlockData);
	pSec1 = (Sector1Info *)(pIDBlockData+SECTOR_OFFSET);
#ifdef RK_FLASH_BOOT_EN
	if(pSec1->usFlashDataOffset && pSec1->usFlashDataLen)
	{
		hasFlashInfo = 1;
		if (gpMemFun->ReadInfo) {
			gpMemFun->ReadInfo(g_pFlashInfoData);
		}
	}
#endif

	//cmy: ʹ���µ�loader�������IDBlock������
	hdr = (RK28BOOT_HEAD*)g_pLoader;
	pSec0->reserved[4] = hdr->ucRc4Flag;

	PRINT_I("update loader data\n");

	// update page0
	// cmy: ����FlashData��FlashBoot����
	pSec0->usFlashDataSize = PAGEALIGN(BYTE2SECTOR(hdr->uiFlashDataLen))*4;
	pSec0->ucFlashBootSize = PAGEALIGN(BYTE2SECTOR(hdr->uiFlashBootLen))*4;

#define SMALL_PACKET                   512
	for(i=0; i<pSec0->ucFlashBootSize; i++)
	{
		if (hdr->ucRc4Flag) {
			P_RC4((void*)(g_pLoader+hdr->uiFlashBootOffset+i*512), 512);
		}
		ftl_memcpy((void*)(pIDBlockData+SECTOR_OFFSET*(4+pSec0->usFlashDataSize+i)), (void*)(g_pLoader+hdr->uiFlashBootOffset+i*512), 512);
	}
	for(i=0; i<pSec0->usFlashDataSize; i++)
	{
		if (hdr->ucRc4Flag) {
			P_RC4((void*)(g_pLoader+hdr->uiFlashDataOffset+i*512), 512);
		}
		ftl_memcpy((void*)(pIDBlockData+SECTOR_OFFSET*(4+i)), (void*)(g_pLoader+hdr->uiFlashDataOffset+i*512), 512);
	}
	pSec0->ucFlashBootSize += pSec0->usFlashDataSize;

	*needIdSectorNum = 4+pSec0->ucFlashBootSize;

	// cmy: ����Loader�İ汾��Ϣ
	PRINT_I("update date and version\n");
	pSec1->usLoaderYear = INT2BCD(hdr->tmCreateTime.usYear);
	pSec1->usLoaderDate = (INT2BCD(hdr->tmCreateTime.usMonth)<<8)|INT2BCD(hdr->tmCreateTime.usDate);
	pSec1->usLoaderVer = (INT2BCD(hdr->uiMajorVersion)<<8)|INT2BCD(hdr->uiMinorVersion);

	if(hasFlashInfo)
	{
		pSec1->usFlashDataOffset = *needIdSectorNum;
		*needIdSectorNum += (PAGEALIGN(pSec1->usFlashDataLen)*4);
	}

	// cmy: ���µ�IDBlock���ݽ��д��
	PRINT_I("EEEEE...\n");
	for(i=0; i<4; i++)
	{
		if(i != 1)
			P_RC4(pIDBlockData+SECTOR_OFFSET*i, 512);
	}

	if(hasFlashInfo)
	{
		for(i=0; i<pSec1->usFlashDataLen; i++)
		{
			ftl_memcpy((void*)(pIDBlockData+SECTOR_OFFSET*(pSec1->usFlashDataOffset+i)), (void*)g_pFlashInfoData, 512);
		}
	}

	PRINT_I("OK\n");

	return TRUE;
}


static int get_rk28boot(uint8 * pLoader, bool dataLoaded)
{
	const disk_partition_t *misc_part = get_disk_partition(MISC_NAME);
	if (!misc_part) {
		printf("misc partition not found!\n");
		return -1;
	}
	RK28BOOT_HEAD* hdr = (RK28BOOT_HEAD*)pLoader;
	int nBootSize = 0;

	if (!dataLoaded)
	{
		if(StorageReadLba(misc_part->start+96, (void*)pLoader,4)!=0 )
		{
			//PRINT_E("ERROR: StorageRead(%d, %d, %d, 0x%p) Failed!\n", 0, misc_part->offset+96, 4, pLoader);
			return -1;
		}
	}

	if (strcmp(BOOTSIGN, hdr->szSign))
	{
		return -2;
	}

	nBootSize = HEADINFO_SIZE
		+ hdr->uiUsbDataLen
		+ hdr->uiUsbBootLen
		+ hdr->uiFlashDataLen
		+ hdr->uiFlashBootLen;

	if (!dataLoaded)
	{
		if(rkloader_CopyFlash2Memory((int32)pLoader, misc_part->start+96, BYTE2SECTOR(nBootSize)))
			return -3;
	}

	update_boot_bloader_ver = (INT2BCD(hdr->uiMajorVersion)<<8)|INT2BCD(hdr->uiMinorVersion);
	return 0;
}

//����ֵ��
// true  �ɹ�
// false ʧ��
static bool WriteXIDBlock(USHORT *pSysBlockAddr, int iIDBCount, UCHAR *idBlockData, UINT uiIdSectorNum)
{
	int i=0, ii=0;
	int write_failed = 0;

	////////////////// ID BLOCKS /////////////////////////////////////////////
	PBYTE writeBuf;
	PBYTE readBuf = (PBYTE)g_pReadBuf;
	UINT sysSectorAddr = 0;
	int iCMDRet;
	// ��дIDBʧ�ܺ�����һ��
	int retry = 0;

	PRINT_I("Enter write idb\n");
	for(i=0; i<iIDBCount && (iIDBCount-write_failed)>1; i++)
	{
		UINT uiNeedWriteSector = uiIdSectorNum;
		UINT uiWriteSector = 0;
		int iCount = 0;

		PRINT_D("Erase block %d\n", pSysBlockAddr[i]);
		iCMDRet = StorageEraseBlock(pSysBlockAddr[i], 1, 1);
		if (iCMDRet!=ERR_SUCCESS)
		{
			//PRINT_E("erase B%d failed: %d\n", pSysBlockAddr[i], iCMDRet);
			retry = !retry;
			retry?--i:++write_failed;
			continue;
		}

		sysSectorAddr = pSysBlockAddr[i]*g_id_block_size;
		PRINT_I("write IDB%d to SEC%d\n", i, sysSectorAddr);
		while(uiNeedWriteSector > 0)
		{
			uiWriteSector = (uiNeedWriteSector>MAX_WRITE_SECTOR)?MAX_WRITE_SECTOR:uiNeedWriteSector;
			writeBuf = idBlockData+iCount*MAX_WRITE_SECTOR*528;
			PRINT_D("write sector %08d ~ %08d\n", sysSectorAddr+iCount*MAX_WRITE_SECTOR, sysSectorAddr+iCount*MAX_WRITE_SECTOR+uiWriteSector-1);
			iCMDRet = StorageWritePba(sysSectorAddr+iCount*MAX_WRITE_SECTOR, writeBuf , uiWriteSector);
			if (ERR_SUCCESS != iCMDRet)
			{
				PRINT_E("write failed %d\n", iCMDRet);
				break;
			}

			// ��ȡ����
			memset(readBuf, 0xff, MAX_WRITE_SECTOR*(512+16));
			PRINT_D("read sector %08d ~ %08d\n", sysSectorAddr+iCount*MAX_WRITE_SECTOR, sysSectorAddr+iCount*MAX_WRITE_SECTOR+uiWriteSector-1);
			iCMDRet = StorageReadPba(sysSectorAddr+iCount*MAX_WRITE_SECTOR, readBuf, uiWriteSector);
			if (ERR_SUCCESS != iCMDRet)
			{
				PRINT_E("read failed %d\n", iCMDRet);
				break;
			}
			// У����д������
			PRINT_D("check data...\n");
			for (ii = 0; ii<uiWriteSector; ii++)
			{
				if(0 != memcmp(writeBuf+528*ii, readBuf+528*ii, 512))
				{
					PRINT_E("check failed %d\n", sysSectorAddr+iCount*MAX_WRITE_SECTOR+ii);
					break;
				}
			}
			if(ii!=uiWriteSector)
				break;

			PRINT_D("Okay!\n");
			++iCount;
			uiNeedWriteSector -= uiWriteSector;
		}
		if (uiNeedWriteSector == 0)
		{
			PRINT_D("IDB[%d] write complete\n", pSysBlockAddr[i]);
		}
		else
		{
			PRINT_D("IDB[%d] write abort\n", pSysBlockAddr[i]);
			retry = !retry;
			retry?--i:++write_failed;
			//		    RKU_EraseBlock(0, pSysBlockAddr[i], 1);
		}
	}

	// cmy: ���д���ٳɹ���һ��������true
	//      ���д��ʣ�����һ���˶�û��д�ɹ�������false
	return (i==iIDBCount);
}

// cmy: ����loader
int rkidb_update_loader(bool dataLoaded)
{
	int iRet=0,iResult;
	int iIDBCount;
	UINT uiNeedIdSectorNum;
	PRINT_E("update loader\n");

	FW_ReIntForUpdate();//����֮ǰ���idblk��������ȷ��

	// ��MISC������ȡ��rk28loader(L).bin�����ݣ������g_pLoader
	PRINT_I("get loader\n");
	iResult = get_rk28boot(g_pLoader, dataLoaded);
	if( iResult )
	{
		PRINT_E("rk28boot Err:%d\n", iResult);
		iRet = -6;
		goto Exit_update;
	}

	PRINT_E("ver %x %x\n",internal_boot_bloader_ver,update_boot_bloader_ver);
	PRINT_I("SecPerBlock=%d\n", g_id_block_size);
	//**************1.����״̬��******************
	PRINT_I("create flash block map\n");
	if( !BuildFlashStateMap(0, &m_flashInfo) )
	{
		PRINT_E("failed1\n");
		iRet = -1;
		goto Exit_update;
	}

	//**************2.��������IDB******************
	PRINT_I("Search all id block...\n");
	iIDBCount = FindAllIDB();
	if ( iIDBCount<=0 )
	{
		PRINT_E("failed2\n");
		iRet = -2;
		goto Exit_update;
	}
	else if(iIDBCount == 1)
	{// ���ٱ�֤��һ��idb��������
		//PRINT_E("Remain last one IDBlock!\n");
		iRet = -3;
		goto Exit_update;
	}

	//PRINT_I("ID BLOCK:\n");
	//for(i=0; i<iIDBCount; i++)
	//    PRINT_I("%d\n", g_IDBlockOffset[i]);

	//**************3.����IDB******************
	PRINT_I("generic id block\n");
	if ( !GenericIDBData(g_pIDBlock, &uiNeedIdSectorNum) )
	{
		PRINT_E("failed3\n");
		iRet = -4;
		goto Exit_update;
	}

	//**************4.д��IDB******************
	PRINT_I("write id block\n");
	if( !WriteXIDBlock(g_IDBlockOffset, iIDBCount, g_pIDBlock, uiNeedIdSectorNum) )
		iRet = -5;
Exit_update:
	PRINT_I(">>> LEVEL update(%d)\n", iRet);
	return iRet;    
}


void rkidb_setup_space(uint32 begin_addr)
{
	uint32 next = 0;

	g_32secbuf = (uint8*)begin_addr;
	next += 32*528;
	g_cramfs_check_buf = (uint8*)begin_addr;
	g_pIDBlock = (uint8*)begin_addr;
	next = begin_addr + 2048*528;
	g_pLoader = (uint8*)next;
	next += 1024*1024;
	g_pReadBuf = (uint8*)next;
	next += MAX_WRITE_SECTOR*528;
	g_pFlashInfoData = (uint8*)next;
	next += 2048;
	if ((next - begin_addr) > CONFIG_RK_GLOBAL_BUFFER_SIZE) {
		printf("CONFIG_RK_GLOBAL_BUFFER_SIZE too small:%d < %lu\n",
				CONFIG_RK_GLOBAL_BUFFER_SIZE, (next - begin_addr));
		while(1);
	}
}


#define IDBLOCK_SN          3//the sector 3
#define IDBLOCK_SECTORS     1024
#define IDBLOCK_NUM         4
#define IDBLOCK_SIZE        512
#define SECTOR_OFFSET       528

extern uint16 g_IDBlockOffset[];

int rkidb_get_idblk_data(void)
{
#ifdef CONFIG_SECOND_LEVEL_BOOTLOADER
	// if storage media is nand, get id block data,
	// else if emmc or sdcard, has been get when sdmmc init.
	if (StorageGetBootMedia() == BOOT_FROM_FLASH) {
		uint32 index;
		int idbCount = FindAllIDB();
		uint8 *psrc, *pdst;

		if (idbCount <= 0) {
			printf("id block not found.\n");
			return false;
		}

		memset((void*)g_pIDBlock, 0, SECTOR_OFFSET * IDBLOCK_NUM);

		if (StorageReadPba(g_IDBlockOffset[0] * g_FlashInfo.BlockSize,
					(void*)g_pIDBlock, IDBLOCK_NUM) != ERR_SUCCESS) {
			printf("read id block error.\n");
			return false;
		}

		pdst = (uint8 *)gIdDataBuf;
		psrc = (uint8 *)g_pIDBlock;
		for (index = 0; index < IDBLOCK_NUM; index++) {
			memcpy(pdst + IDBLOCK_SIZE * index, psrc + SECTOR_OFFSET * index, IDBLOCK_SIZE);
		}
	}
#endif
	// some block rc4 decode
	GetIdblockDataNoRc4((char*)&gIdDataBuf[128*2],512);
	GetIdblockDataNoRc4((char*)&gIdDataBuf[128*3],512);

#if 0
	int i = 0;
	for (i = 0; i < 512 * IDBLOCK_NUM; i ++) {
		printf("%02x ", pdst[i]);
		if ((i+1) % 16 == 0) {
			printf("\n");
		}
	}
	printf("\n");
#endif
	return true;
}


int rkidb_get_rc4_flag(void)
{
	Sector0Info idb0_info;

	if (gIdDataBuf[0] == 0xFCDC8C3B) {
		memcpy((char *)&idb0_info, gIdDataBuf, 512);
		GetIdblockDataNoRc4((char*)&idb0_info, 512);
		/* id block: sector0 offset 8 is rc4 flag: 0 or 1 */
		return idb0_info.uiRc4Flag;
	}

	return -1;
}


int rkidb_get_bootloader_ver(void)
{
	uint8 *buf = (uint8*)&gIdDataBuf[0];
	memset(bootloader_ver,0,24);

	if(*(uint32*)buf == 0xfcdc8c3b)
	{
		uint16 year, date;

		year = *(uint16*)((uint8*)buf+512+18);
		date = *(uint16*)((uint8*)buf+512+20);
		internal_boot_bloader_ver = *(uint16*)((uint8*)buf+512+22);
		sprintf(bootloader_ver,"%04X-%02X-%02X#%X.%02X",
				year,
				(uint8)((date>>8)&0x00FF), (uint8)(date&0x00FF),
				(uint8)((internal_boot_bloader_ver>>8)&0x00FF), (uint8)(internal_boot_bloader_ver&0x00FF));
		return 0;
	}

	return -1;
}

int rkidb_get_sn(char* buf)
{
	int size;
	Sector3Info *pSec3;
	uint8 *pidbbuf = (uint8 *)gIdDataBuf;

	pSec3 = (Sector3Info *)(pidbbuf + IDBLOCK_SIZE * IDBLOCK_SN);

	size = pSec3->snSize;
	if (size <= 0 || size > SN_MAX_SIZE) {
		printf("empty serial no.\n");
		return false;
	}
	strncpy(buf, (char*)pSec3->sn, size);
	buf[size] = '\0';
	printf("sn:%s\n", buf);
	return true;
}


int rkidb_erase_drm_key(void)
{
	ALLOC_CACHE_ALIGN_BUFFER(u8, buf, RK_BLK_SIZE);
	memset(buf, 0, RK_BLK_SIZE);
	StorageSysDataStore(1, buf);
	gDrmKeyInfo.publicKeyLen = 0;
	return 0;
}

