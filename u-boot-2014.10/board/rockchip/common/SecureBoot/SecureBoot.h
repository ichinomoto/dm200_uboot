/*
 * (C) Copyright 2008-2014 Rockchip Electronics
 * Peter, Software Engineering, <superpeter.cai@gmail.com>.
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

#ifndef _RK_SECUREBOOT_H_
#define _RK_SECUREBOOT_H_


/* Secure Boot mode */
#define SBOOT_MODE_NS       0
#define SBOOT_MODE_RK       1
#define SBOOT_MODE_DX       2


#ifdef SECUREBOOT_CRYPTO_EN

#define PUBLIC_KEY_LEN              512         //BYTE UNIT
#define OTP_HASH_LEN                32          //public key hash length store in efuse. BYTE UNIT
#define OTP_HASH_ADDR               32          //BYTE UNIT
#define OTP_SECURE_FLAG_ADDR        31          //BYTE UNIT

typedef /*__packed*/ struct tagBOOT_HEADER {
	uint32 tag;
	uint32 version;
	uint32 flags;
	uint32 size;
	uint32 reserved1[3];
	uint16 HashBits;
	uint16 RSABits;                /* length in bits of modulus */
	uint32 RSA_N[64];
	uint32 RSA_E[64];
	uint32 RSA_C[64];
	uint32 HashData[(8+1)*2];   //ǰ8������ddr����hash����8������loader����hash
	//uint32 signature[64];
} BOOT_HEADER, *PBOOT_HEADER;

typedef /*__packed*/ struct tagBOOT_HASH {
	uint32 Hash1[8];    //ddr����hash
	uint32 reserved1;
	uint32 Hash2[8];   //loader����hash
	//uint32 reserved2;
} BOOT_HASH, *PBOOT_HASH;

#endif /* SECUREBOOT_CRYPTO_EN */


#define RKNAND_SYS_STORGAE_DATA_LEN	504
typedef struct tagRKNAND_SYS_STORGAE {
	uint32  tag;
	uint32  len;
	uint8   data[RKNAND_SYS_STORGAE_DATA_LEN];
} RKNAND_SYS_STORGAE;

typedef struct tagSYS_DATA_INFO {
	uint32 systag;          // "SYSD" 0x44535953
	uint32 syslen;          // 504
	uint32 sysdata[(512-8)/4];  //

	uint32 drmtag;          // "DRMK" 0x4B4D5244
	uint32 drmlen;          // 504
	uint32 drmdata[(512-8)/4];  //

	uint32 reserved[(2048-1024)/4];  //����
} SYS_DATA_INFO, *pSYS_DATA_INFO;

typedef struct tagBOOT_CONFIG_INFO {
	uint32 bootTag;          // "SYSD" 0x44535953
	uint32 bootLen;          // 504
	uint32 bootMedia;        // 1:flash 2:emmc 4:sdcard0 8:sdcard1
	uint32 BootPart;         // 0 disable , 1~N : part 1~N
	uint32 secureBootEn;     // 0 disable , 1:enable
	uint32 sdPartOffset;     
	uint32 sdSysPartOffset;  
	uint32 sys_reserved[(508-28)/4];
	uint32 hash; // 0 disable , 1:enable
} BOOT_CONFIG_INFO, *pBOOT_CONFIG_INFO;

typedef struct tagDRM_KEY_INFO {
	uint32 drmtag;           // "DRMK" 0x4B4D5244
	uint32 drmLen;           // 504
	uint32 keyBoxEnable;     // 0:flash 1:emmc 2:sdcard1 3:sdcard2
	uint32 drmKeyLen;        //0 disable , 1~N : part 1~N
	uint32 publicKeyLen;     //0 disable , 1:enable
	uint32 secureBootLock;   //0 disable , 1:lock
	uint32 secureBootLockKey;//�ӽ�����ʹ��
	uint32 reserved0[(0x40-0x1C)/4];
	uint8  drmKey[0x80];      // key data
	uint32 reserved2[(0xFC-0xC0)/4];
	uint8  publicKey[0x104];      // key data
} DRM_KEY_INFO, *pDRM_KEY_INFO;

extern DRM_KEY_INFO gDrmKeyInfo;

extern uint32 SecureMode;
extern uint32 SecureBootEn;
extern uint32 SecureBootCheckOK;
extern uint32 SecureBootLock;
extern uint32 SecureBootLock_backup;
extern BOOT_CONFIG_INFO gBootConfig;

uint32 SecureBootCheck(void);
void SecureBootUnlock(uint8 *pKey);
void SecureBootUnlockCheck(uint8 *pKey);
void SecureBootLockLoader(void);

uint32 SecureBootImageSecureCheck(rk_boot_img_hdr *hdr, int unlocked);
uint32 SecureBootSetSysData2Kernel(uint32 SecureBootFlag);
uint32 SecureBootSecureDisable(void);
void SecureBootSecureState2Kernel(uint32 SecureState);

#endif	/* _RK_SECUREBOOT_H_ */
