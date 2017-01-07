/*
 * (C) Copyright 2008-2014 Rockchip Electronics
 *
 * Configuation settings for the rk3xxx chip platform.
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
#ifndef _PARAMETER_H_
#define _PARAMETER_H_

#define PARAMETER_NUM			8			// parameter�ļ��ı��ݸ���
#define PARAMETER_OFFSET		1024			// ÿ��parameter��ƫ����

#define MAX_LINE_CHAR			(1024*64)	        // Parameters�ж��Line��ɣ�����ÿ��Line���ռ1024 Bytes
#define MAX_LOADER_PARAM		(128*512)		// Parameters��ռ�����Sector��(��tag��length��crc��)
#define PARM_TAG			0x4D524150
#define MAGIC_CODE			0x00280028
#define EATCHAR(x, c)			for (; *(x) == (c); (x)++) ; // ȥ���ַ���x�����Ϊc���ַ�
#define PART_NAME	32
#define MAX_MTDID	64

#define MTD_WRITEABLE			0x400	/* Device is writeable */
#define MTD_POWERUP_LOCK		0x2000	/* Always locked after reset */


#define SIZE_REMAINING			0xFFFFFFFF


#define PARTNAME_MISC			"misc"
#define PARTNAME_KERNEL			"kernel"
#define PARTNAME_BOOT			"boot"
#define PARTNAME_RECOVERY		"recovery"
#define PARTNAME_SYSTEM		    	"system"
#define PARTNAME_BACKUP			"backup"
#define PARTNAME_SNAPSHOT		"snapshot"

typedef struct tag_cmdline_mtd_partition {
	char mtd_id[MAX_MTDID];
	int num_parts;
	disk_partition_t parts[CONFIG_MAX_PARTITIONS];
} cmdline_mtd_partition;

typedef struct tagLoaderParam
{
	uint32	tag;
	uint32	length;
	char	parameter[1];
	uint32	crc;
} LoaderParam, *PLoaderParam;

typedef struct tagBootInfo
{
	uint32 magic_code;
	uint16 machine_type;
	uint16 boot_index;		// 0 - normal boot, 1 - recovery
	uint32 atag_addr;
	uint32 misc_offset;
	uint32 kernel_load_addr;
	uint32 boot_offset;		// ��SectorΪ��λ
	uint32 recovery_offset;		// ��SectorΪ��λ
	uint32 ramdisk_offset;	// ��SectorΪ��λ
	uint32 ramdisk_size;	// ��ByteΪ��λ
	uint32 ramdisk_load_addr;
	uint32 is_kernel_in_boot;
	
	uint32 check_mask;	// 00 - ��У�飬 01 - check kernel, 10 - check ramdisk, 11 - both check
	char cmd_line[MAX_LINE_CHAR];
	cmdline_mtd_partition cmd_mtd;

	int index_misc;
	int index_kernel;
	int index_boot;
	int index_recovery;
	int index_system;
	int index_backup;
	int index_snapshot;
	char fw_version[MAX_LINE_CHAR];
	char fdt_name[MAX_LINE_CHAR];
} BootInfo, *PBootInfo;


extern BootInfo			gBootInfo;
extern uint32			parameter_lba;

extern void ParseParam(PBootInfo pboot_info, char *param, uint32 len);
extern int32 GetParam(uint32 param_addr, void* buf);
const disk_partition_t* get_disk_partition(const char* name);
void dump_disk_partitions(void);
int load_disk_partitions(void);

#endif /* _PARAMETER_H_ */

