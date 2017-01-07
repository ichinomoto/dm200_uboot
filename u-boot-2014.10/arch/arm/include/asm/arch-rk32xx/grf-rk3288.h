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
#ifndef __RK3288_GRF_H
#define __RK3288_GRF_H

#include <asm/io.h>

/* gpio iomux control */
#define GRF_GPIO1D_IOMUX	0x000C

#define GRF_GPIO2A_IOMUX	0x0010
#define GRF_GPIO2B_IOMUX	0x0014
#define GRF_GPIO2C_IOMUX	0x0018

#define GRF_GPIO3A_IOMUX	0x0020
#define GRF_GPIO3B_IOMUX	0x0024
#define GRF_GPIO3C_IOMUX	0x0028
#define GRF_GPIO3DL_IOMUX	0x002C
#define GRF_GPIO3DH_IOMUX	0x0030

#define GRF_GPIO4AL_IOMUX	0x0034
#define GRF_GPIO4AH_IOMUX	0x0038
#define GRF_GPIO4BL_IOMUX	0x003C
#define GRF_GPIO4C_IOMUX	0x0044
#define GRF_GPIO4D_IOMUX	0x0048

#define GRF_GPIO5B_IOMUX	0x0050
#define GRF_GPIO5C_IOMUX	0x0054

#define GRF_GPIO6A_IOMUX	0x005C
#define GRF_GPIO6B_IOMUX	0x0060
#define GRF_GPIO6C_IOMUX	0x0064

#define GRF_GPIO7A_IOMUX	0x006C
#define GRF_GPIO7B_IOMUX	0x0070
#define GRF_GPIO7CL_IOMUX	0x0074
#define GRF_GPIO7CH_IOMUX	0x0078

#define GRF_GPIO8A_IOMUX	0x0080
#define GRF_GPIO8B_IOMUX	0x0084


/* gpio sr control */
#define GRF_GPIO1H_SR		0x0104

#define GRF_GPIO2L_SR		0x0108
#define GRF_GPIO2H_SR		0x010C

#define GRF_GPIO3L_SR		0x0110
#define GRF_GPIO3H_SR		0x0114

#define GRF_GPIO4L_SR		0x0118
#define GRF_GPIO4H_SR		0x011C

#define GRF_GPIO5L_SR		0x0120
#define GRF_GPIO5H_SR		0x0124

#define GRF_GPIO7L_SR		0x0130
#define GRF_GPIO7H_SR		0x0134

#define GRF_GPIO8L_SR		0x0138


/* gpio power down/up control */
#define GRF_GPIO1D_P		0x14C

#define GRF_GPIO2A_P		0x150
#define GRF_GPIO2B_P		0x154
#define GRF_GPIO2C_P		0x158

#define GRF_GPIO3A_P		0x160
#define GRF_GPIO3B_P		0x164
#define GRF_GPIO3C_P		0x168
#define GRF_GPIO3D_P		0x16C

#define GRF_GPIO4A_P		0x170
#define GRF_GPIO4B_P		0x174
#define GRF_GPIO4C_P		0x178
#define GRF_GPIO4D_P		0x17C

#define GRF_GPIO5B_P		0x184
#define GRF_GPIO5C_P		0x188

#define GRF_GPIO6A_P		0x190
#define GRF_GPIO6B_P		0x194
#define GRF_GPIO6C_P		0x198

#define GRF_GPIO7A_P		0x1A0
#define GRF_GPIO7B_P		0x1A4
#define GRF_GPIO7C_P		0x1A8

#define GRF_GPIO8A_P		0x1B0
#define GRF_GPIO8B_P		0x1B4


/* gpio drive strength control */
#define GRF_GPIO1D_E		0x1CC

#define GRF_GPIO2A_E		0x1D0
#define GRF_GPIO2B_E		0x1D4
#define GRF_GPIO2C_E		0x1D8

#define GRF_GPIO3A_E		0x1E0
#define GRF_GPIO3B_E		0x1E4
#define GRF_GPIO3C_E		0x1E8
#define GRF_GPIO3D_E		0x1EC

#define GRF_GPIO4A_E		0x1F0
#define GRF_GPIO4B_E		0x1F4
#define GRF_GPIO4C_E		0x1F8
#define GRF_GPIO4D_E		0x1FC

#define GRF_GPIO5B_E		0x204
#define GRF_GPIO5C_E		0x208

#define GRF_GPIO6A_E		0x210
#define GRF_GPIO6B_E		0x214
#define GRF_GPIO6C_E		0x218

#define GRF_GPIO7A_E		0x220
#define GRF_GPIO7B_E		0x224
#define GRF_GPIO7C_E		0x228

#define GRF_GPIO8A_E		0x230
#define GRF_GPIO8B_E		0x234


/* Soc control part 1 */
#define GRF_SOC_CON0		0x0244
#define GRF_SOC_CON1		0x0248
#define GRF_SOC_CON2		0x024C
#define GRF_SOC_CON3		0x0250
#define GRF_SOC_CON4		0x0254
#define GRF_SOC_CON5		0x0258
#define GRF_SOC_CON6		0x025C
#define GRF_SOC_CON7		0x0260
#define GRF_SOC_CON8		0x0264
#define GRF_SOC_CON9		0x0268
#define GRF_SOC_CON10		0x026C
#define GRF_SOC_CON11		0x0270
#define GRF_SOC_CON12		0x0274
#define GRF_SOC_CON13		0x0278
#define GRF_SOC_CON14		0x027C


/* Soc status */
#define GRF_SOC_STATUS0		0x0280
#define GRF_SOC_STATUS1		0x0284
#define GRF_SOC_STATUS2		0x0288
#define GRF_SOC_STATUS3		0x028C
#define GRF_SOC_STATUS4		0x0290
#define GRF_SOC_STATUS5		0x029C
#define GRF_SOC_STATUS6		0x02A0
#define GRF_SOC_STATUS7		0x02A4
#define GRF_SOC_STATUS8		0x02A8
#define GRF_SOC_STATUS9		0x02AC
#define GRF_SOC_STATUS10	0x02B0
#define GRF_SOC_STATUS11	0x02B4
#define GRF_SOC_STATUS12	0x02B8
#define GRF_SOC_STATUS13	0x02BC
#define GRF_SOC_STATUS14	0x02C0
#define GRF_SOC_STATUS15	0x02C4
#define GRF_SOC_STATUS16	0x02C8
#define GRF_SOC_STATUS17	0x02CC
#define GRF_SOC_STATUS18	0x02D0
#define GRF_SOC_STATUS19	0x02D4
#define GRF_SOC_STATUS20	0x02D8
#define GRF_SOC_STATUS21	0x02DC

/* peri dmac control */
#define GRF_PERIDMAC_CON0	0x02E0
#define GRF_PERIDMAC_CON1	0x02E4
#define GRF_PERIDMAC_CON2	0x02E8
#define GRF_PERIDMAC_CON3	0x02EC

/* ddrc control */
#define GRF_DDRC0_CON0		0x02F0
#define GRF_DDRC1_CON0		0x02F4


/* cpu control */
#define GRF_CPU_CON0		0x02F8
#define GRF_CPU_CON1		0x02FC
#define GRF_CPU_CON2		0x0300
#define GRF_CPU_CON3		0x0304
#define GRF_CPU_CON4		0x0308


/* cpu status */
#define GRF_CPU_STATUS0		0x0318


/* UOC control */
#define GRF_UOC0_CON0		0x0320
#define GRF_UOC0_CON1		0x0324
#define GRF_UOC0_CON2		0x0328
#define GRF_UOC0_CON3		0x032C
#define GRF_UOC0_CON4		0x0330

#define GRF_UOC1_CON0		0x0334
#define GRF_UOC1_CON1		0x0338
#define GRF_UOC1_CON2		0x033C
#define GRF_UOC1_CON3		0x0340
#define GRF_UOC1_CON4		0x0344

#define GRF_UOC2_CON0		0x0348
#define GRF_UOC2_CON1		0x034C
#define GRF_UOC2_CON2		0x0350
#define GRF_UOC2_CON3		0x0354

#define GRF_UOC3_CON0		0x0358
#define GRF_UOC3_CON1		0x035C

#define GRF_UOC4_CON0		0x0360
#define GRF_UOC4_CON1		0x0364


/* PVT monitor control */
#define GRF_PVTM_CON0		0x0368
#define GRF_PVTM_CON1		0x036C
#define GRF_PVTM_CON2		0x0370


/* PVT monitor status */
#define GRF_PVTM_STATUS0	0x0374
#define GRF_PVTM_STATUS1	0x0378
#define GRF_PVTM_STATUS2	0x037C


/* IO Voltage select */
#define GRF_IO_VSEL		0x0380


/* saradc test bit */
#define GRF_SARADC_TESTBIT	0x0384


/* tsadc test bit low */
#define GRF_TSADC_TESTBIT_L	0x0388


/* tsadc test bit high */
#define GRF_TSADC_TESTBIT_H	0x038C


/* PVT monitor status */
#define GRF_OS_REG0		0x0390
#define GRF_OS_REG1		0x0394
#define GRF_OS_REG2		0x0398
#define GRF_OS_REG3		0x039C


/* Soc status part 2 */
#define GRF_SOC_CON15		0x02A4
#define GRF_SOC_CON16		0x02A8


/* secure grf soc control part 1 */
#define SGRF_SOC_CON0		0x00
#define SGRF_SOC_CON1		0x04
#define SGRF_SOC_CON2		0x08
#define SGRF_SOC_CON3		0x0C
#define SGRF_SOC_CON4		0x10
#define SGRF_SOC_CON5		0x14

#define SGRF_BUSDMAC_CON0	0x20
#define SGRF_BUSDMAC_CON1	0x24

/* secure grf cpu control */
#define SGRF_CPU_CON0		0x40
#define SGRF_CPU_CON1		0x44
#define SGRF_CPU_CON2		0x48

/* secure grf soc control part 2 */
#define SGRF_SOC_CON6		0x50
#define SGRF_SOC_CON7		0x54
#define SGRF_SOC_CON8		0x58
#define SGRF_SOC_CON9		0x5C
#define SGRF_SOC_CON10		0x60
#define SGRF_SOC_CON11		0x64
#define SGRF_SOC_CON12		0x68
#define SGRF_SOC_CON13		0x6C
#define SGRF_SOC_CON14		0x70
#define SGRF_SOC_CON15		0x74
#define SGRF_SOC_CON16		0x78
#define SGRF_SOC_CON17		0x7C
#define SGRF_SOC_CON18		0x80
#define SGRF_SOC_CON19		0x84
#define SGRF_SOC_CON20		0x88
#define SGRF_SOC_CON21		0x8C

/* secure grf status */
#define SGRF_SOC_STATUS0	0x100
#define SGRF_SOC_STATUS1	0x104

/* secure grf fast boot address */
#define SGRF_FAST_BOOT_ADDR	0x120

#define grf_readl(offset)	readl(RKIO_GRF_PHYS + offset)
#define grf_writel(v, offset)	do { writel(v, RKIO_GRF_PHYS + offset); } while (0)

#define pmu_readl(offset)	readl(RKIO_PMU_PHYS + offset)
#define pmu_writel(v, offset)	do { writel(v, RKIO_PMU_PHYS + offset); } while (0)


#endif /* __RK3288_GRF_H */

