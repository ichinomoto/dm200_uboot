/*
 * Warp!! common define  Rev. 5.0.0
 *
 */

#define WARP_HEADER_ID          0x00
#define WARP_HEADER_VERSION     0x04
#define WARP_HEADER_CAPS        0x08
#define WARP_HEADER_LOWMEM_END  0x0c
#define WARP_HEADER_TEXT_END    0x10
#define WARP_HEADER_DATA_END    0x14
#define WARP_HEADER_BSS_END     0x18
#define WARP_HEADER_SNAPSHOT    0x20
#define WARP_HEADER_HIBERNATE   0x28
#define WARP_HEADER_SWITCH      0x30

#define WARP_ID_DRIVER          0x44483557      /* W5HD */
#define WARP_ID_BOOTFLAG        0x46423557      /* W5BF */
#define WARP_ID_USER_API        0x41553257      /* W2UA */

#define WARP_PART_SHIFT         0
#define WARP_LUN_SHIFT          8
#define WARP_DEV_SHIFT          16

#define WARP_PART_MASK          (0xff << WARP_PART_SHIFT)
#define WARP_LUN_MASK           (0xff << WARP_LUN_SHIFT)
#define WARP_DEV_MASK           (0xff << WARP_DEV_SHIFT)

#define WARP_DEV_NOR            (0x01 << WARP_DEV_SHIFT)
#define WARP_DEV_NAND           (0x02 << WARP_DEV_SHIFT)
#define WARP_DEV_ATA            (0x03 << WARP_DEV_SHIFT)
#define WARP_DEV_SD             (0x04 << WARP_DEV_SHIFT)
#define WARP_DEV_MEM            (0x05 << WARP_DEV_SHIFT)
#define WARP_DEV_SPI            (0x06 << WARP_DEV_SHIFT)
#define WARP_DEV_USER           (0x7e << WARP_DEV_SHIFT)
#define WARP_DEV_EXT            (0x7f << WARP_DEV_SHIFT)

#define WARP_DEV(dev, lun, part)        (WARP_DEV_##dev | \
                                         ((lun) << WARP_LUN_SHIFT) | \
                                         ((part) << WARP_PART_SHIFT))

#ifndef WARP_LUN_CONV
#define WARP_LUN_CONV(dev)      (dev)
#endif

#define WARP_DEV_TO_LUN(dev)    WARP_LUN_CONV(((dev) & WARP_LUN_MASK) >> \
                                              WARP_LUN_SHIFT)
#define WARP_DEV_TO_PART(dev)   (((dev) & WARP_PART_MASK) >> WARP_PART_SHIFT)

#define WARP_DRV_INVALID        0
#define WARP_DRV_FIXED          1
#define WARP_DRV_FLOATING       2

#define WARP_BF_LEN             0x400

#define WARP_SAVEAREA_NUM       (sizeof(warp_savearea) / \
                                 sizeof(struct warp_savearea))


struct warp_boot {
    u16         cpu_no;
    s16         switch_mode;
    s16         silent;
    s16         console;
    s32         bps;
    u32         drv_total_size;
    u64         v2p_offset;
    u64         text_v2p_offset;
    u32         lowmem_maxarea;
    u32         lowmem_maxsize;
    u64         bg;
    u32         reserve[12];
};

typedef struct warp_area {
    u32         dev;
    u32         size;
    u64         offs;
} warp_area;

struct warp_drv_info {
    u32 mode;
    unsigned long addr;
    warp_area drv;
};

struct warp_savearea {
    warp_area bootflag;
};

#ifdef CONFIG_WARP_HIBDRV_BG

struct warp_bg {
    int ret_code;
    int boot_ready;
    u64 boot_addr;
    u64 data_addr;
};

void boot_cpu(int cpu);
void kill_cpu(int cpu);

#endif

extern const struct warp_savearea warp_savearea[];

extern int warp_saveno;

extern void *warp_bfaddr;

int warp_drvload(void);
int warp_checkboot(int saveno);
int warp_boot(int saveno);
int warp_clear_bootf(int saveno);

#define WARP_DRV_ID(drv)        (*(u32 *)((void *)(drv) + WARP_HEADER_ID))
#define WARP_DRV_VERSION(drv)   (*(u32 *)((void *)(drv) + WARP_HEADER_VERSION))
#define WARP_DRV_CAPS(drv)      (*(u32 *)((void *)(drv) + WARP_HEADER_CAPS))
#define WARP_DRV_LOWMEM_END(drv) \
    (*(u32 *)((void *)(drv) + WARP_HEADER_LOWMEM_END))
#define WARP_DRV_TEXT_END(drv)  (*(u32 *)((void *)(drv) + WARP_HEADER_TEXT_END))
#define WARP_DRV_DATA_END(drv)  (*(u32 *)((void *)(drv) + WARP_HEADER_DATA_END))
#define WARP_DRV_BSS_END(drv)   (*(u32 *)((void *)(drv) + WARP_HEADER_BSS_END))
