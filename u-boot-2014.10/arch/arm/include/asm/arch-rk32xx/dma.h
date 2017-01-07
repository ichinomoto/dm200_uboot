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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef	__RK_DMA_PL330_H_
#define	__RK_DMA_PL330_H_


#define RK_DMAF_AUTOSTART		(1 << 0)
#define RK_DMAF_CIRCULAR		(1 << 1)

enum rk_dma_buffresult {
	RK_RES_OK,
	RK_RES_ERR,
	RK_RES_ABORT
};

enum rk_dmasrc {
	RK_DMASRC_HW,		/* source is memory */
	RK_DMASRC_MEM,		/* source is hardware */
	RK_DMASRC_MEMTOMEM
};

/* enum rk_chan_op
 *
 * operation codes passed to the DMA code by the user, and also used
 * to inform the current channel owner of any changes to the system state
*/

enum rk_chan_op {
	RK_DMAOP_START,
	RK_DMAOP_STOP,
	RK_DMAOP_PAUSE,
	RK_DMAOP_RESUME,
	RK_DMAOP_FLUSH,
	RK_DMAOP_TIMEOUT,		/* internal signal to handler */
	RK_DMAOP_STARTED,		/* indicate channel started */
};

struct rk_dma_client {
	char                *name;
};

/* rk_dma_cbfn_t
 *
 * buffer callback routine type
*/

typedef void (*rk_dma_cbfn_t)(void *buf, int size,
				   enum rk_dma_buffresult result);

typedef int  (*rk_dma_opfn_t)(enum rk_chan_op);


/* rk dmac platform configure */
#if defined(CONFIG_RKCHIP_RK3066) || defined(CONFIG_RKCHIP_RK3168) || defined(CONFIG_RKCHIP_RK3188)
	#define RK_PL330_DMAC_MAX	2
	#define CONFIG_RK_DMAC_0	/* dmac 0 */
	#define CONFIG_RK_DMAC_1	/* dmac 1 */

	#define RK_DMAC0_BASE		RKIO_DMAC1_PHYS
	#define RK_DMAC1_BASE		RKIO_DMAC2_PHYS

	#define RK_DMAC0_IRQ0		IRQ_DMAC0_0
	#define RK_DMAC0_IRQ1		IRQ_DMAC0_1
	#define RK_DMAC1_IRQ0		IRQ_DMAC1_0
	#define RK_DMAC1_IRQ1		IRQ_DMAC1_1
#elif defined(CONFIG_RKCHIP_RK3288)
	#define RK_PL330_DMAC_MAX	2
	#define CONFIG_RK_DMAC_0	/* dmac 0 */
	#define CONFIG_RK_DMAC_1	/* dmac 1 */

	#define RK_DMAC0_BASE		RKIO_SECURITY_DMAC_BUS_PHYS /* default is security dma bus */
	#define RK_DMAC1_BASE		RKIO_DMAC_PERI_PHYS

	#define RK_DMAC0_IRQ0		IRQ_DMAC_BUS0
	#define RK_DMAC0_IRQ1		IRQ_DMAC_BUS1
	#define RK_DMAC1_IRQ0		IRQ_DMAC_PERI0
	#define RK_DMAC1_IRQ1		IRQ_DMAC_PERI1
#elif defined(CONFIG_RKCHIP_RK3036) || defined(CONFIG_RKCHIP_RK3126) || defined(CONFIG_RKCHIP_RK3128)
	#define RK_PL330_DMAC_MAX	1
	#define CONFIG_RK_DMAC_0	/* dmac 0 */

	#define RK_DMAC0_BASE		RKIO_DMAC_PHYS

	#define RK_DMAC0_IRQ0		IRQ_DMAC_0
	#define RK_DMAC0_IRQ1		IRQ_DMAC_1
#else
	#error "Please config platform for dmac."
#endif


/*
 * PL330 can assign any channel to communicate with
 * any of the peripherals attched to the DMAC.
 * For the sake of consistency across client drivers,
 * We keep the channel names unchanged and only add
 * missing peripherals are added.
 * Order is not important since rk PL330 API driver
 * use these just as IDs.
 */
#if defined(CONFIG_RKCHIP_RK3066) || defined(CONFIG_RKCHIP_RK3168) || defined(CONFIG_RKCHIP_RK3188)
enum dma_ch {
	DMACH_UART0_TX,
	DMACH_UART0_RX,
	DMACH_UART1_TX,
	DMACH_UART1_RX,
	DMACH_I2S0_8CH_TX,
	DMACH_I2S0_8CH_RX,
	DMACH_I2S1_2CH_TX,
	DMACH_I2S1_2CH_RX,
	DMACH_SPDIF_TX,
	DMACH_I2S2_2CH_TX,
	DMACH_I2S2_2CH_RX,

	DMACH_HSADC,
	DMACH_SDMMC,
	DMACH_SDIO,
	DMACH_EMMC,
	DMACH_PID_FILTER,
	DMACH_UART2_TX,
	DMACH_UART2_RX,
	DMACH_UART3_TX,
	DMACH_UART3_RX,
	DMACH_SPI0_TX,
	DMACH_SPI0_RX,
	DMACH_SPI1_TX,
	DMACH_SPI1_RX,
	DMACH_DMAC0_MEMTOMEM,
	DMACH_DMAC1_MEMTOMEM,
	DMACH_DMAC2_MEMTOMEM,
	DMACH_DMAC3_MEMTOMEM,
	DMACH_DMAC4_MEMTOMEM,
	DMACH_DMAC5_MEMTOMEM,
	DMACH_DMAC6_MEMTOMEM,
	DMACH_DMAC7_MEMTOMEM,
	/* END Marker, also used to denote a reserved channel */
	DMACH_MAX,
};

#elif defined(CONFIG_RKCHIP_RK3288)

enum dma_ch {
	DMACH_I2S_TX,
	DMACH_I2S_RX,
	DMACH_SPDIF_TX,
	DMACH_SPDIF_8CH_TX,
	DMACH_UART2_DBG_TX,
	DMACH_UART2_DBG_RX,

	DMACH_HSADC,
	DMACH_UART0_BT_TX,
	DMACH_UART0_BT_RX,
	DMACH_UART1_BB_TX,
	DMACH_UART1_BB_RX,
	DMACH_UART3_GPS_TX,
	DMACH_UART3_GPS_RX,
	DMACH_UART4_EXP_TX,
	DMACH_UART4_EXP_RX,
	DMACH_SPI0_TX,
	DMACH_SPI0_RX,
	DMACH_SPI1_TX,
	DMACH_SPI1_RX,
	DMACH_SPI2_TX,
	DMACH_SPI2_RX,
	DMACH_DMAC0_MEMTOMEM,
	DMACH_DMAC1_MEMTOMEM,
	DMACH_DMAC2_MEMTOMEM,
	DMACH_DMAC3_MEMTOMEM,
	DMACH_DMAC4_MEMTOMEM,
	DMACH_DMAC5_MEMTOMEM,
	DMACH_DMAC6_MEMTOMEM,
	DMACH_DMAC7_MEMTOMEM,
	DMACH_DMAC8_MEMTOMEM,
	DMACH_DMAC9_MEMTOMEM,
	DMACH_DMAC10_MEMTOMEM,
	/* END Marker, also used to denote a reserved channel */
	DMACH_MAX,
};

#elif defined(CONFIG_RKCHIP_RK3036)

enum dma_ch {
	DMACH_I2S_TX,
	DMACH_I2S_RX,
	DMACH_UART0_TX,
	DMACH_UART0_RX,
	DMACH_UART1_TX,
	DMACH_UART1_RX,
	DMACH_UART2_TX,
	DMACH_UART2_RX,
	DMACH_SPI_TX,
	DMACH_SPI_RX,
	DMACH_SDMMC,
	DMACH_SDIO,
	DMACH_EMMC,
	DMACH_SPDIF_TX,

	DMACH_DMAC0_MEMTOMEM,
	DMACH_DMAC1_MEMTOMEM,
	DMACH_DMAC2_MEMTOMEM,
	DMACH_DMAC3_MEMTOMEM,
	DMACH_DMAC4_MEMTOMEM,
	DMACH_DMAC5_MEMTOMEM,
	DMACH_DMAC6_MEMTOMEM,
	DMACH_DMAC7_MEMTOMEM,
	DMACH_DMAC8_MEMTOMEM,
	DMACH_DMAC9_MEMTOMEM,
	DMACH_DMAC10_MEMTOMEM,
	DMACH_DMAC11_MEMTOMEM,
	DMACH_DMAC12_MEMTOMEM,
	DMACH_DMAC13_MEMTOMEM,
	DMACH_DMAC14_MEMTOMEM,
	DMACH_DMAC15_MEMTOMEM,
	DMACH_DMAC16_MEMTOMEM,
	DMACH_DMAC17_MEMTOMEM,
	/* END Marker, also used to denote a reserved channel */
	DMACH_MAX,
};

#elif defined(CONFIG_RKCHIP_RK3126) || defined(CONFIG_RKCHIP_RK3128)
enum dma_ch {
	DMACH_I2S_2CH_TX,
	DMACH_I2S_2CH_RX,
	DMACH_UART0_TX,
	DMACH_UART0_RX,
	DMACH_UART1_TX,
	DMACH_UART1_RX,
	DMACH_UART2_TX,
	DMACH_UART2_RX,
	DMACH_SPI_TX,
	DMACH_SPI_RX,
	DMACH_SDMMC,
	DMACH_SDIO,
	DMACH_EMMC,
	DMACH_SPDIF_TX,
	DMACH_I2S_8CH_TX,
	DMACH_I2S_8CH_RX,
	DMACH_DMAC0_MEMTOMEM,
	DMACH_DMAC1_MEMTOMEM,
	DMACH_DMAC2_MEMTOMEM,
	DMACH_DMAC3_MEMTOMEM,
	DMACH_DMAC4_MEMTOMEM,
	DMACH_DMAC5_MEMTOMEM,
	DMACH_DMAC6_MEMTOMEM,
	DMACH_DMAC7_MEMTOMEM,
	DMACH_DMAC8_MEMTOMEM,
	DMACH_DMAC9_MEMTOMEM,
	DMACH_DMAC10_MEMTOMEM,
	DMACH_DMAC11_MEMTOMEM,
	DMACH_DMAC12_MEMTOMEM,
	DMACH_DMAC13_MEMTOMEM,
	DMACH_DMAC14_MEMTOMEM,
	DMACH_DMAC15_MEMTOMEM,
	/* END Marker, also used to denote a reserved channel */
	DMACH_MAX,
};
#else
	#error "Please config platform for dmac."
#endif

/*
 * Every PL330 DMAC has max 32 peripheral interfaces,
 * of which some may be not be really used in your
 * DMAC's configuration.
 * Populate this array of 32 peri i/fs with relevant
 * channel IDs for used peri i/f and DMACH_MAX for
 * those unused.
 *
 * The platforms just need to provide this info
 * to the rk DMA API driver for PL330.
 */
struct rk_pl330_platdata {
	enum dma_ch peri[32];
};



/* rk_dma_request
 *
 * request a dma channel exclusivley
*/

extern int rk_dma_request(unsigned int channel,
			       struct rk_dma_client *, void *dev);


/* rk_dma_ctrl
 *
 * change the state of the dma channel
*/

extern int rk_dma_ctrl(unsigned int channel, enum rk_chan_op op);

/* rk_dma_setflags
 *
 * set the channel's flags to a given state
*/

extern int rk_dma_setflags(unsigned int channel,
				unsigned int flags);

/* rk_dma_free
 *
 * free the dma channel (will also abort any outstanding operations)
*/

extern int rk_dma_free(unsigned int channel, struct rk_dma_client *);

/* rk_dma_enqueue_ring
 *
 * place the given buffer onto the queue of operations for the channel.
 * The buffer must be allocated from dma coherent memory, or the Dcache/WB
 * drained before the buffer is given to the DMA system.
*/

extern int rk_dma_enqueue_ring(enum dma_ch channel, void *id,
			       dma_addr_t data, int size, int numofblock, bool sev);

/* rk_dma_enqueue
 *
 * place the given buffer onto the queue of operations for the channel.
 * The buffer must be allocated from dma coherent memory, or the Dcache/WB
 * drained before the buffer is given to the DMA system.
*/

extern int rk_dma_enqueue(enum dma_ch channel, void *id,
			       dma_addr_t data, int size);


/* rk_dma_config
 *
 * configure the dma channel
*/

extern int rk_dma_config(unsigned int channel, int xferunit, int brst_len);

/* rk_dma_devconfig
 *
 * configure the device we're talking to
*/

extern int rk_dma_devconfig(unsigned int channel,
		enum rk_dmasrc source, unsigned long devaddr);

/* rk_dma_getposition
 *
 * get the position that the dma transfer is currently at
*/

extern int rk_dma_getposition(unsigned int channel,
				   dma_addr_t *src, dma_addr_t *dest);

extern int rk_dma_set_opfn(unsigned int, rk_dma_opfn_t rtn);
extern int rk_dma_set_buffdone_fn(unsigned int, rk_dma_cbfn_t rtn);

extern int rk_pl330_dmac_init(int dmac_id);
extern int rk_pl330_dmac_deinit(int dmac_id);

#endif	/* __RK_DMA_PL330_H_ */
