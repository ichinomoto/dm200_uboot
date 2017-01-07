/*
 * Warp!! boot driver  Rev. 5.0.2
 *
 *  Copyright (C) 2008-2015  Lineo Solutions, Inc.
 *
 */

#include <common.h>
#include <command.h>
#include <version.h>
#include <warp.h>

#ifndef U_BOOT_VER_CODE
#define U_BOOT_VER_CODE     0
#endif

#ifndef U_BOOT_VER
#define U_BOOT_VER(a,b,c)       (((a) << 16) + ((b) << 8) + (c))
#endif

#if U_BOOT_VER_CODE < U_BOOT_VER(2010,9,0)
#define CMD_CONST
#else
#define CMD_CONST       const
#endif

#ifndef WARP_PAGE_SIZE
#define WARP_PAGE_SIZE          4096
#endif

#ifdef CONFIG_WARP

#define WARP_DRV_NUM            (sizeof(warp_drv_info) / \
                                 sizeof(struct warp_drv_info))

const struct warp_drv_info warp_drv_info[] = {
    WARP_DRV_INFO
};

const struct warp_savearea warp_savearea[] = {
    WARP_SAVEAREA
};

int warp_saveno = CONFIG_WARP_SAVENO;

void *warp_drvaddr;
void *warp_bfaddr;

static struct warp_boot *boot_param;
static u32 drv_total_size;

extern void warp_set_clock(void);

#ifdef CONFIG_WARP_NOR

static int warp_nor_load(void *addr, void *buf, int size)
{
    memcpy(buf, addr, size);
    return 0;
}

static int warp_nor_bferase(int saveno)
{
    unsigned long off, end;
    char cmd[256];

    off = CONFIG_WARP_NOR_BASE + warp_savearea[saveno].bootflag.offs;
    end = off + warp_savearea[saveno].bootflag.size - 1;
    printf("Warp!! bootflag clear NOR: 0x%08lx-0x%08lx\n", off, end);

    sprintf(cmd, "protect off 0x%08lx 0x%08lx", off, end);
    run_command(cmd, 0);
    sprintf(cmd, "erase 0x%08lx 0x%08lx", off, end);
    run_command(cmd, 0);
    return 0;
}

#endif

#ifdef CONFIG_WARP_SPI

#include <spi_flash.h>

#ifndef CONFIG_SF_DEFAULT_SPEED
#define CONFIG_SF_DEFAULT_SPEED         1000000
#endif
#ifndef CONFIG_SF_DEFAULT_MODE
#define CONFIG_SF_DEFAULT_MODE          SPI_MODE_3
#endif

static u32 spi_current_dev;
static struct spi_flash *current_spi;

static struct spi_flash *warp_spi_probe(u32 dev)
{
    int bus, cs;

    if (dev != spi_current_dev) {
        bus = WARP_DEV_TO_LUN(dev);
        cs = WARP_DEV_TO_PART(dev);
        if (!(current_spi = spi_flash_probe(bus, cs, CONFIG_SF_DEFAULT_SPEED,
                                            CONFIG_SF_DEFAULT_MODE)))
            printf("SPI flash probe error\n");
        else
            spi_current_dev = dev;
    }

    return current_spi;
}

static int warp_spi_load(u32 dev, u64 offs, void *buf, int size)
{
    struct spi_flash *spi;

    if (!(spi = warp_spi_probe(dev)))
        return 1;

    return spi_flash_read(spi, offs, size, buf);
}

static int warp_spi_bferase(int saveno)
{
    int ret, bus, cs;
    unsigned long size, off;
    struct spi_flash *spi;

    if (!(spi = warp_spi_probe(warp_savearea[saveno].bootflag.dev)))
        return 1;

    bus = WARP_DEV_TO_LUN(warp_savearea[saveno].bootflag.dev);
    cs = WARP_DEV_TO_PART(warp_savearea[saveno].bootflag.dev);
    off = warp_savearea[saveno].bootflag.offs;
    size = warp_savearea[saveno].bootflag.size;
    printf("Warp!! bootflag clear SPI%d:%d : 0x%08lx-0x%08lx\n",
           bus, cs, off, off + size - 1);

    if ((ret = spi_flash_erase(spi, off, size)) != 0) {
        printf("bootflag erase error %d\n", ret);
        return ret;
    }
    return 0;
}

#endif  /* CONFIG_WARP_SPI */

#ifdef CONFIG_WARP_NAND

#include <nand.h>

static u64 warp_nand_skipbad(nand_info_t *nand, u64 offs, int size)
{
    u64 end = offs + size;

    while (offs < end) {
        if (!nand_block_isbad(nand, offs))
            return offs;
        offs += nand->erasesize;
    }
    printf("all block is bad\n");
    return (u64)-1;
}

static int warp_nand_load(u32 dev, u64 offs, u32 area_size, void *buf, int size)
{
    int ret;
    size_t read_size;
    nand_info_t *nand = &nand_info[0];

    while (size > 0) {
        offs = warp_nand_skipbad(nand, offs, area_size);
        if (offs == (u64)-1)
            return 1;

        if (size >= nand->erasesize)
            read_size = nand->erasesize;
        else
            read_size = size;
        if ((ret = nand_read(nand, offs, &read_size, buf)) != 0) {
            printf("nand read error %d\n", ret);
            return ret;
        }
        buf += read_size;
        size -= read_size;
    }

    return ret;
}

static int warp_nand_bferase(int saveno)
{
    int ret;
    unsigned long off, end;
    nand_info_t *nand = &nand_info[0];

    off = warp_savearea[saveno].bootflag.offs;
    end = off + warp_savearea[saveno].bootflag.size;
    printf("Warp!! bootflag clear NAND: 0x%08lx-0x%08lx\n", off, end - 1);

    while (off < end) {
        if (!nand_block_isbad(nand, off)) {
            if ((ret = nand_erase(nand, off, nand->erasesize)) != 0) {
                printf("bootflag erase error %d\n", ret);
                return ret;
            }
        }
        off += nand->erasesize;
    }
    return 0;
}

#endif  /* CONFIG_WARP_NAND */

#if defined(CONFIG_WARP_ATA) || defined(CONFIG_WARP_SD)

static u32 current_dev;
static disk_partition_t part_info;
static block_dev_desc_t *dev_desc;

static int warp_get_part_info(u32 dev)
{
    int ret;
    u32 lun, part;

    lun = WARP_DEV_TO_LUN(dev);
    part = WARP_DEV_TO_PART(dev);

    if (WARP_DEV_TO_PART(current_dev) != part) {
        if (part == 0) {
            part_info.start = 0;
        } else {
            if ((ret = get_partition_info(dev_desc, part, &part_info)) != 0) {
                printf("partition %d:%d not found\n", lun, part);
                return ret;
            }
        }
    }

    current_dev = dev;
    return 0;
}

static int warp_dev_load(u64 offs, void *buf, int size)
{
    int ret, blk;

    offs += part_info.start;
    blk = (size + dev_desc->blksz - 1) / dev_desc->blksz;
    if ((ret = dev_desc->block_read(dev_desc->dev, offs, blk, buf)) >= 0)
        return 0;

    return 1;
}

static int warp_dev_bferase(int saveno, const char *name)
{
#if U_BOOT_VER_CODE >= U_BOOT_VER(1,3,0)
    int ret;
    unsigned long off;
    int lun, part;
    char *buf[512];

    if (!dev_desc->block_write) {
        printf("not support\n");
        return 0;
    }

    memset(buf, 0, 512);
    lun = WARP_DEV_TO_LUN(warp_savearea[saveno].bootflag.dev);
    part = WARP_DEV_TO_PART(warp_savearea[saveno].bootflag.dev);
    off = part_info.start + warp_savearea[saveno].bootflag.offs;
    printf("Warp!! bootflag clear %s%d part:%d offs:0x%08llx (sect:0x%08lx)\n",
           name, lun, part, warp_savearea[saveno].bootflag.offs, off);

    ret = dev_desc->block_write(dev_desc->dev, off, 1, (void *)buf);
    if (ret != 1) {
        printf("bootflag erase error %d\n", ret);
        return ret;
    }
#else
    printf("not support\n");
#endif
    return 0;
}

#endif  /* CONFIG_WARP_ATA || CONFIG_WARP_SD */

#ifdef CONFIG_WARP_ATA

#if U_BOOT_VER_CODE < U_BOOT_VER(1,3,3)
#error "ATA: not support"
#endif

#include <sata.h>

extern block_dev_desc_t sata_dev_desc[];

static int warp_ata_init(u32 dev)
{
    int ret;
    u32 lun;
    block_dev_desc_t *sata_get_dev(int dev);

    lun = WARP_DEV_TO_LUN(dev);

    if (((current_dev ^ dev) & ~WARP_PART_MASK)) {
        current_dev = (u32)-1;

        if (sata_dev_desc[lun].if_type != IF_TYPE_SATA) {
            memset(&sata_dev_desc[lun], 0, sizeof(struct block_dev_desc));
            sata_dev_desc[lun].if_type = IF_TYPE_SATA;
            sata_dev_desc[lun].dev = lun;
            sata_dev_desc[lun].part_type = PART_TYPE_UNKNOWN;
            sata_dev_desc[lun].type = DEV_TYPE_HARDDISK;
            sata_dev_desc[lun].lba = 0;
            sata_dev_desc[lun].blksz = 512;
            sata_dev_desc[lun].block_read = sata_read;
            sata_dev_desc[lun].block_write = sata_write;

            if ((ret = init_sata(lun)))
                return ret;
            if ((ret = scan_sata(lun)))
                return ret;
            if ((sata_dev_desc[lun].lba > 0) && (sata_dev_desc[lun].blksz > 0))
                init_part(&sata_dev_desc[lun]);
        }

        if ((dev_desc = sata_get_dev(lun)) == NULL) {
            printf("device %d not found\n", lun);
            return -1;
        }
    }

    return warp_get_part_info(dev);
}

static int warp_ata_load(u32 dev, u64 offs, void *buf, int size)
{
    int ret;

    ret = warp_ata_init(dev);
    if (ret != 0)
        return ret;

    return warp_dev_load(offs, buf, size);
}

static int warp_ata_bferase(int saveno)
{
    int ret;

    ret = warp_ata_init(warp_savearea[saveno].bootflag.dev);
    if (ret != 0)
        return ret;

    return warp_dev_bferase(saveno, "ATA");
}

#endif /* CONFIG_WARP_ATA */

#ifdef CONFIG_WARP_SD

static int warp_sd_load (u32 dev, u64 offs, void *buf, int size)
{
    disk_partition_t *part_info = get_disk_partition(WARP_PART_NAME);
    int blk = (size + part_info->blksz - 1) / part_info->blksz;
    u32 lba = (u32)offs + part_info->start;

    return StorageReadLba(lba, (void *)buf, blk);
}

static int warp_sd_bferase (int saveno)
{
    char *buf[512];
    disk_partition_t *part_info = get_disk_partition(WARP_PART_NAME);
    u32 lba = warp_savearea[saveno].bootflag.offs + part_info->start;

    memset(buf, 0, 512);

    return StorageWriteLba(lba, (void *)buf, 1);
}

#endif  /* CONFIG_WARP_SD */

static void warp_init_dev(void)
{
#ifdef CONFIG_WARP_SPI
    spi_current_dev = 0;
#endif
#if defined(CONFIG_WARP_ATA) || defined(CONFIG_WARP_SD)
    current_dev = 0;
#endif
}

static int warp_load_drv(int no, void *buf, u32 size)
{
    int ret = 1;
    const struct warp_drv_info *drv = &warp_drv_info[no];
    u32 dev = drv->drv.dev & WARP_DEV_MASK;

    if (0) {
#ifdef CONFIG_WARP_NOR
    } else if (dev == WARP_DEV_NOR) {
        ret = warp_nor_load((void *)CONFIG_WARP_NOR_BASE + drv->drv.offs,
                            buf, size);
#endif
#ifdef CONFIG_WARP_SPI
    } else if (dev == WARP_DEV_SPI) {
        ret = warp_spi_load(drv->drv.dev, drv->drv.offs, buf, size);
#endif
#ifdef CONFIG_WARP_NAND
    } else if (dev == WARP_DEV_NAND) {
        ret = warp_nand_load(drv->drv.dev, drv->drv.offs, drv->drv.size,
                             buf, size);
#endif
#ifdef CONFIG_WARP_ATA
    } else if (dev == WARP_DEV_ATA) {
        ret = warp_ata_load(drv->drv.dev, drv->drv.offs, buf, size);
#endif
#ifdef CONFIG_WARP_SD
    } else if (dev == WARP_DEV_SD) {
        ret = warp_sd_load(drv->drv.dev, drv->drv.offs, buf, size);
#endif
#ifdef CONFIG_WARP_EXTDRV
    } else if (dev == WARP_DEV_EXT) {
        ret = warp_extdrv_load(drv->drv.dev, drv->drv.offs, drv->drv.size,
                               buf, size);
#endif
    }

    if (ret != 0)
        printf("Can't load Warp!! driver %d.\n", no);

    return ret;
}

static int _warp_drvload(int fixed_only)
{
    int ret = 0;
    int no, size, sum = 0;
    int id = WARP_ID_DRIVER;
    void *header;
    void *floating_buf;

    if (warp_drv_info[0].mode == WARP_DRV_FIXED)
        warp_drvaddr = (void *)warp_drv_info[0].addr;
    else
        warp_drvaddr = (void *)CONFIG_WARP_LOAD_ADDR;
    floating_buf = warp_drvaddr;

    for (no = 0; no < WARP_DRV_NUM; no++) {
        const struct warp_drv_info *drv = &warp_drv_info[no];
        if (drv->mode == WARP_DRV_FLOATING) {
            if (fixed_only)
                continue;
            header = floating_buf;
        } else if (drv->mode == WARP_DRV_FIXED) {
            header = (void *)drv->addr;
        } else {
            continue;
        }

        if (fixed_only || drv->mode == WARP_DRV_FLOATING) {
            if (warp_load_drv(no, header, 32) != 0)
                return 1;
            if (WARP_DRV_ID(header) != id) {
                if (id == WARP_ID_DRIVER) {
                    return 1;
                } else {
                    printf("Can't find UserAPI driver %d\n", no - 1);
                    continue;
                }
            }
            size = WARP_DRV_DATA_END(header);
            if (warp_load_drv(no, header, size) != 0)
                return 1;

            flush_cache((unsigned long)header, size);
        }

        if (drv->mode == WARP_DRV_FLOATING || no == 0) {
            size = (WARP_DRV_DATA_END(header) + WARP_PAGE_SIZE - 1) &
                ~(WARP_PAGE_SIZE - 1);
            floating_buf += size;
            sum += size;
        }

        id = WARP_ID_USER_API;
    }

    drv_total_size = sum;
    return ret;
}

int warp_drvload(void)
{
    return _warp_drvload(1);
}

static int warp_bfload(int saveno, void *bfaddr)
{
    int ret = 1;
    const struct warp_savearea *sa = &warp_savearea[saveno];
    u32 dev = sa->bootflag.dev & WARP_DEV_MASK;

    if (0) {
#ifdef CONFIG_WARP_NOR
    } else if (dev == WARP_DEV_NOR) {
        ret = warp_nor_load((void *)CONFIG_WARP_NOR_BASE + sa->bootflag.offs,
                            bfaddr, WARP_BF_LEN);
#endif
#ifdef CONFIG_WARP_SPI
    } else if (dev == WARP_DEV_SPI) {
        ret = warp_spi_load(sa->bootflag.dev, sa->bootflag.offs,
                            bfaddr, WARP_BF_LEN);
            return ret;
#endif
#ifdef CONFIG_WARP_NAND
    } else if (dev == WARP_DEV_NAND) {
        ret = warp_nand_load(sa->bootflag.dev, sa->bootflag.offs,
                             sa->bootflag.size, bfaddr, WARP_BF_LEN);
#endif
#ifdef CONFIG_WARP_ATA
    } else if (dev == WARP_DEV_ATA) {
        ret = warp_ata_load(sa->bootflag.dev, sa->bootflag.offs,
                            bfaddr, WARP_BF_LEN);
#endif
#ifdef CONFIG_WARP_SD
    } else if (dev == WARP_DEV_SD) {
        ret = warp_sd_load(sa->bootflag.dev, sa->bootflag.offs,
                           bfaddr, WARP_BF_LEN);
            return ret;
#endif
#ifdef CONFIG_WARP_EXTDRV
    } else if (dev == WARP_DEV_EXT) {
        ret = warp_extdrv_load(sa->bootflag.dev, sa->bootflag.offs,
                               sa->bootflag.size, bfaddr, WARP_BF_LEN);
#endif
#ifdef CONFIG_WARP_USERDRV
    } else if (dev == WARP_DEV_USER) {
        ret = warp_userdrv_load(sa->bootflag.dev, sa->bootflag.offs,
                                sa->bootflag.size, bfaddr, WARP_BF_LEN);
#endif
    }

    return ret;
}

int warp_clear_bootf(int saveno)
{
    int ret = 0;
    char *str;
    u32 dev;

    if (saveno < 0) {
        if ((str = getenv("warp_saveno")))
            saveno = simple_strtoul(str, NULL, 10);
        else
            saveno = warp_saveno;
    }
    if (saveno < 0 || saveno >= WARP_SAVEAREA_NUM) {
        printf("Illegal saveno %d\n", saveno);
        return 1;
    }

    dev = warp_savearea[saveno].bootflag.dev & WARP_DEV_MASK;

    if (0) {
#ifdef CONFIG_WARP_NOR
    } else if (dev == WARP_DEV_NOR) {
        ret = warp_nor_bferase(saveno);
#endif
#ifdef CONFIG_WARP_SPI
    } else if (dev == WARP_DEV_SPI) {
        ret = warp_spi_bferase(saveno);
#endif
#ifdef CONFIG_WARP_NAND
    } else if (dev == WARP_DEV_NAND) {
        ret = warp_nand_bferase(saveno);
#endif
#ifdef CONFIG_WARP_ATA
    } else if (dev == WARP_DEV_ATA) {
        ret = warp_ata_bferase(saveno);
#endif
#ifdef CONFIG_WARP_SD
    } else if (dev == WARP_DEV_SD) {
        ret = warp_sd_bferase(saveno);
#endif
#ifdef CONFIG_WARP_EXTDRV
    } else if (dev == WARP_DEV_EXT) {
        ret = warp_extdrv_bferase(saveno);
#endif
#ifdef CONFIG_WARP_USERDRV
    } else if (dev == WARP_DEV_USER) {
        ret = warp_userdrv_bferase(saveno);
#endif
    }

    return ret;
}

int warp_checkboot(int saveno)
{
    int ret;
    char *str;

    if (saveno < 0) {
        if ((str = getenv("warp_saveno")))
            saveno = simple_strtoul(str, NULL, 10);
        else
            saveno = warp_saveno;
    }
    if (saveno < 0 || saveno >= WARP_SAVEAREA_NUM) {
        printf("Illegal saveno %d\n", saveno);
        return 1;
    }

    if ((ret = _warp_drvload(0)) != 0)
        return ret;

    warp_bfaddr = (warp_drvaddr + WARP_DRV_TEXT_END(warp_drvaddr));

    if ((ret = warp_bfload(saveno, warp_bfaddr)) != 0)
        return ret;

    if (*(unsigned int *)warp_bfaddr != WARP_ID_BOOTFLAG)
        return 1;

    boot_param = warp_bfaddr + WARP_BF_LEN;

    return 0;
}

static int warp_pre_boot(int saveno)
{
    int ret;

    if ((ret = warp_checkboot(saveno)) != 0)
        return ret;

    memset(boot_param, 0, sizeof(boot_param));
    boot_param->silent = -1;
    boot_param->console = -1;
    boot_param->bps = -1;
    boot_param->drv_total_size = drv_total_size;

    return 0;
}

static int warp_boot_fg(int saveno)
{
    int ret;
    int (*hibernate)(void);

    if ((ret = warp_pre_boot(saveno)) != 0)
        return ret;

#if 0
    warp_set_clock();
#endif
    hibernate = (void *)(warp_drvaddr + WARP_HEADER_HIBERNATE);
    ret = hibernate();
    printf("Warp!! error can not boot %d\n", ret);
    return ret;
}

#ifdef CONFIG_WARP_HIBDRV_BG

static int warp_bg_bootable;

static struct warp_bg warp_bg;

int warp_pre_boot_bg(int saveno)
{
    int ret;

    warp_bg_bootable = 0;

    if ((ret = warp_pre_boot(saveno)) != 0)
        return ret;

    boot_param->bg = (u64)(u32)&warp_bg;
    warp_bg.ret_code = 1;
    warp_bg.boot_ready = 0;
    warp_bg.boot_addr = 0;
    flush_dcache_range((u32)boot_param, (u32)(boot_param + 1));
    flush_dcache_range((u32)&warp_bg, (u32)(&warp_bg + 1));
    boot_cpu(CONFIG_WARP_HIBDRV_CPU);
    warp_bg_bootable = 1;
    return 0;
}

static void warp_wait_boot_bg(void)
{
    if (!warp_bg_bootable)
        return;

    do {
        flush_dcache_range((u32)&warp_bg, (u32)(&warp_bg + 1));
    } while (warp_bg.ret_code > 0);

    kill_cpu(CONFIG_WARP_HIBDRV_CPU);

    warp_bg_bootable = 0;
}

static int warp_boot_bg(int saveno)
{
    warp_bg.boot_ready = 1;
    flush_dcache_range((u32)&warp_bg, (u32)(&warp_bg + 1));

    warp_wait_boot_bg();

    if (warp_bg.ret_code == 0)
        ((int (*)(void))(u32)warp_bg.boot_addr)();

    printf("Warp!! error can not boot %d\n", warp_bg.ret_code);
    return warp_bg.ret_code;
}

#endif

int warp_boot(int saveno)
{
    int ret;

#ifdef CONFIG_WARP_HIBDRV_BG
    if (warp_bg_bootable)
        ret = warp_boot_bg(saveno);
    else
#endif
        ret = warp_boot_fg(saveno);
    return ret;
}

static int do_warp(cmd_tbl_t *cmdtp, int flag,
                   int argc, char * CMD_CONST argv[])
{
    int saveno = -1;

    if (argc > 1)
        saveno = simple_strtoul(argv[1], NULL, 10);

    warp_init_dev();

#ifdef CONFIG_WARP_HIBDRV_BG
    warp_wait_boot_bg();
#endif

    warp_boot_fg(saveno);

    return 0;
}

static int do_clear_bootf(cmd_tbl_t *cmdtp, int flag,
                          int argc, char * CMD_CONST argv[])
{
    int saveno = -1;

    if (argc > 1)
        saveno = simple_strtoul(argv[1], NULL, 10);

    warp_init_dev();

    return warp_clear_bootf(saveno);
}

U_BOOT_CMD(
    warp, 2, 0, do_warp,
#if U_BOOT_VER_CODE < U_BOOT_VER(2009,3,0)
    "warp    - Warp!! boot\n",
#else
    "Warp!! boot",
#endif
    "[saveno]"
#if U_BOOT_VER_CODE < U_BOOT_VER(2009,8,0)
    "\n"
#endif
);

U_BOOT_CMD(
    clear_bootf, 2, 0, do_clear_bootf,
#if U_BOOT_VER_CODE < U_BOOT_VER(2009,3,0)
    "clear_bootf - clear Warp!! bootflag\n",
#else
    "clear Warp!! bootflag",
#endif
    "[saveno]"
#if U_BOOT_VER_CODE < U_BOOT_VER(2009,8,0)
    "\n"
#endif
);

#endif  /* CONFIG_WARP */
