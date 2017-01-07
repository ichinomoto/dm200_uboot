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
#ifndef __RK312X_GRF_H
#define __RK312X_GRF_H

#include <asm/io.h>

/* gpio iomux control */
#define GRF_GPIO0A_IOMUX        0x00a8
#define GRF_GPIO0B_IOMUX        0x00ac
#define GRF_GPIO0C_IOMUX        0x00b0
#define GRF_GPIO0D_IOMUX        0x00b4
#define GRF_GPIO1A_IOMUX        0x00b8
#define GRF_GPIO1B_IOMUX        0x00bc
#define GRF_GPIO1C_IOMUX        0x00c0
#define GRF_GPIO1D_IOMUX        0x00c4
#define GRF_GPIO2A_IOMUX        0x00c8
#define GRF_GPIO2B_IOMUX        0x00cc
#define GRF_GPIO2C_IOMUX        0x00d0
#define GRF_GPIO2D_IOMUX        0x00d4
#define GRF_GPIO3A_IOMUX        0x00d8
#define GRF_GPIO3B_IOMUX        0x00dc
#define GRF_GPIO3C_IOMUX        0x00e0
#define GRF_GPIO3D_IOMUX        0x00e4

#define GRF_GPIO2C_IOMUX2       0x00e8
#define GRF_CIF_IOMUX           0x00ec
#define GRF_CIF_IOMUX1          0x00f0

/* gpio ds control */
#define GRF_GPIO_DS             0x0100

/* gpio pull down/up control */
#define GRF_GPIO0L_PULL         0x0118
#define GRF_GPIO0H_PULL         0x011c
#define GRF_GPIO1L_PULL         0x0120
#define GRF_GPIO1H_PULL         0x0124
#define GRF_GPIO2L_PULL         0x0128
#define GRF_GPIO2H_PULL         0x012c
#define GRF_GPIO3L_PULL         0x0130
#define GRF_GPIO3H_PULL         0x0134

#define GRF_ACODEC_CON		0x013c

/* Soc control */
#define GRF_SOC_CON0            0x0140
#define GRF_SOC_CON1            0x0144
#define GRF_SOC_CON2            0x0148

/* Soc status */
#define GRF_SOC_STATUS0         0x014c

#define GRF_LVDS_CON0		0x0150

/* dmac control */
#define GRF_DMAC_CON0           0x015c
#define GRF_DMAC_CON1           0x0160
#define GRF_DMAC_CON2           0x0164

#define GRF_MAC_CON0		0x0168
#define GRF_MAC_CON1		0x016C

#define GRF_TVE_CON0		0x0170

/* UOC control */
#define GRF_UOC0_CON0           0x017c
#define GRF_UOC1_CON1           0x0184
#define GRF_UOC1_CON2           0x0188
#define GRF_UOC1_CON3           0x018c
#define GRF_UOC1_CON4           0x0190
#define GRF_UOC1_CON5           0x0194

#define GRF_DDRC_STAT           0x019c
#define GRF_UOC_CON6            0x01a0

#define GRF_SOC_STATUS1         0x01a4

/* cpu control */
#define GRF_CPU_CON0            0x01a8
#define GRF_CPU_CON1            0x01ac
#define GRF_CPU_CON2            0x01b0
#define GRF_CPU_CON3            0x01b4

/* cpu status */
#define GRF_CPU_STATUS0         0x01c0
#define GRF_CPU_STATUS1         0x01c4

/* PVT monitor status */
#define GRF_OS_REG0             0x01c8
#define GRF_OS_REG1             0x01cc
#define GRF_OS_REG2             0x01d0
#define GRF_OS_REG3             0x01d4
#define GRF_OS_REG4             0x01d8
#define GRF_OS_REG5             0x01dc
#define GRF_OS_REG6             0x01e0
#define GRF_OS_REG7             0x01e4

/* PVT monitor control */
#define GRF_PVTM_CON0		0x0200
#define GRF_PVTM_CON1		0x0204
#define GRF_PVTM_CON2		0x0208
#define GRF_PVTM_CON3		0x020C

/* PVT monitor status */
#define GRF_PVTM_STATUS0	0x0210
#define GRF_PVTM_STATUS1	0x0214
#define GRF_PVTM_STATUS2	0x0218
#define GRF_PVTM_STATUS3	0x021c

#define GRF_DFI_WRNUM           0x0220
#define GRF_DFI_RDNUM           0x0224
#define GRF_DFI_ACTNUM          0x0228
#define GRF_DFI_TIMERVAL        0x022c

#define GRF_NIF_FIFO0           0x0230
#define GRF_NIF_FIFO1           0x0234
#define GRF_NIF_FIFO2           0x0238
#define GRF_NIF_FIFO3           0x023c

#define GRF_USBPHY0_CON0        0x0280
#define GRF_USBPHY0_CON1        0x0284
#define GRF_USBPHY0_CON2        0x0288
#define GRF_USBPHY0_CON3        0x028c
#define GRF_USBPHY0_CON4        0x0290
#define GRF_USBPHY0_CON5        0x0294
#define GRF_USBPHY0_CON6        0x0298
#define GRF_USBPHY0_CON7        0x029c
#define GRF_USBPHY1_CON0        0x02a0
#define GRF_USBPHY1_CON1        0x02a4
#define GRF_USBPHY1_CON2        0x02a8
#define GRF_USBPHY1_CON3        0x02ac
#define GRF_USBPHY1_CON4        0x02b0
#define GRF_USBPHY1_CON5        0x02b4
#define GRF_USBPHY1_CON6        0x02b8
#define GRF_USBPHY1_CON7        0x02bc

#define GRF_UOC_STATUS0         0x02c0

#define GRF_CHIP_TAG            0x0300

#define GRF_MMC_DET_CNT         0x0304

#define GRF_EFUSE_PRG_EN        0x037c


#define grf_readl(offset)	readl(RKIO_GRF_PHYS + offset)
#define grf_writel(v, offset)	do { writel(v, RKIO_GRF_PHYS + offset); } while (0)


#endif /* __RK312X_GRF_H */

