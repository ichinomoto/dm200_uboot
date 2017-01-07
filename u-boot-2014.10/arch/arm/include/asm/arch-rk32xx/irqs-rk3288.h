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
#ifndef __RK3288_IRQS_H
#define __RK3288_IRQS_H

#define FIQ_START                       0

#define IRQ_VIRTUAL_TIMER               27
#define IRQ_SECURE_TIMER                28
#define IRQ_NOSECURE_TIMER              29

#define RKXX_IRQ(x)                     (x + 32)

#define IRQ_DMAC_BUS0                   RKXX_IRQ(0)
#define IRQ_DMAC_BUS1                   RKXX_IRQ(1)
#define IRQ_DMAC_PERI0                  RKXX_IRQ(2)
#define IRQ_DMAC_PERI1                  RKXX_IRQ(3)
#define IRQ_DDR_UPCTL0                  RKXX_IRQ(4)
#define IRQ_DDR_UPCTL1                  RKXX_IRQ(5)
#define IRQ_GPU_JOB                     RKXX_IRQ(6)
#define IRQ_GPU_MMU                     RKXX_IRQ(7)
#define IRQ_GPU_GPU                     RKXX_IRQ(8)
#define IRQ_VEPU                        RKXX_IRQ(9)
#define IRQ_VDPU                        RKXX_IRQ(10)
#define IRQ_VMMU                        RKXX_IRQ(11)
#define IRQ_HEVC                        RKXX_IRQ(12)
#define IRQ_VIP                         RKXX_IRQ(13)
#define IRQ_ISP                         RKXX_IRQ(14)
#define IRQ_VOP_BIG                     RKXX_IRQ(15)
#define IRQ_VOP_LIT                     RKXX_IRQ(16)
#define IRQ_IEP                         RKXX_IRQ(17)
#define IRQ_RGA                         RKXX_IRQ(18)
#define IRQ_DSI0_HOST                   RKXX_IRQ(19)
#define IRQ_DSI1_HOST                   RKXX_IRQ(20)
#define IRQ_CSI_HOST0                   RKXX_IRQ(21)
#define IRQ_CSI_HOST1                   RKXX_IRQ(22)
#define IRQ_USB_OTG                     RKXX_IRQ(23)
#define IRQ_USB_HOST0_EHCI              RKXX_IRQ(24)
#define IRQ_USB_HOST1                   RKXX_IRQ(25)
#define IRQ_HSIC                        RKXX_IRQ(26)
#define IRQ_GMAC                        RKXX_IRQ(27)
#define IRQ_GMAC_PMT                    RKXX_IRQ(28)
#define IRQ_GPS                         RKXX_IRQ(29)
#define IRQ_GPS_TIMER                   RKXX_IRQ(30)
#define IRQ_HSADC                       RKXX_IRQ(31)
#define IRQ_SDMMC                       RKXX_IRQ(32)
#define IRQ_SDIO0                       RKXX_IRQ(33)
#define IRQ_SDIO1                       RKXX_IRQ(34)
#define IRQ_EMMC                        RKXX_IRQ(35)
#define IRQ_SARADC                      RKXX_IRQ(36)
#define IRQ_TSADC                       RKXX_IRQ(37)
#define IRQ_NANDC0                      RKXX_IRQ(38)
#define IRQ_PERI_MMU                    RKXX_IRQ(39)
#define IRQ_NANDC1                      RKXX_IRQ(40)
#define IRQ_USB_HOST0_OHCI              RKXX_IRQ(41)
#define IRQ_TPS                         RKXX_IRQ(42)
#define IRQ_SCR                         RKXX_IRQ(43)
#define IRQ_SPI0                        RKXX_IRQ(44)
#define IRQ_SPI1                        RKXX_IRQ(45)
#define IRQ_SPI2                        RKXX_IRQ(46)
#define IRQ_PS2C                        RKXX_IRQ(47)
#define IRQ_CRYPTO                      RKXX_IRQ(48)
#define IRQ_HOST_PULSE0                 RKXX_IRQ(49)
#define IRQ_HOST_PULSE1                 RKXX_IRQ(50)
#define IRQ_HOST0                       RKXX_IRQ(51)
#define IRQ_HOST1                       RKXX_IRQ(52)
#define IRQ_I2S_8CH                     RKXX_IRQ(53)
#define IRQ_SPDIF_8CH                   RKXX_IRQ(54)
#define IRQ_UART_BT                     RKXX_IRQ(55)
#define IRQ_UART_BB                     RKXX_IRQ(56)
#define IRQ_UART_DBG                    RKXX_IRQ(57)
#define IRQ_UART_GPS                    RKXX_IRQ(58)
#define IRQ_UART_EXP                    RKXX_IRQ(59)
#define IRQ_I2C_PMU                     RKXX_IRQ(60)
#define IRQ_I2C_AUDIO                   RKXX_IRQ(61)
#define IRQ_I2C_SENSOR                  RKXX_IRQ(62)
#define IRQ_I2C_CAM                     RKXX_IRQ(63)
#define IRQ_I2C_TP                      RKXX_IRQ(64)
#define IRQ_I2C_HDMI                    RKXX_IRQ(65)
#define IRQ_TIMER_6CH_0                 RKXX_IRQ(66)
#define IRQ_TIMER_6CH_1                 RKXX_IRQ(67)
#define IRQ_TIMER_6CH_2                 RKXX_IRQ(68)
#define IRQ_TIMER_6CH_3                 RKXX_IRQ(68)
#define IRQ_TIMER_6CH_4                 RKXX_IRQ(70)
#define IRQ_TIMER_6CH_5                 RKXX_IRQ(71)
#define IRQ_TIMER_2CH_0                 RKXX_IRQ(72)
#define IRQ_TIMER_2CH_1                 RKXX_IRQ(73)
#define IRQ_PWM0                        RKXX_IRQ(74)
#define IRQ_PWM1                        RKXX_IRQ(75)
#define IRQ_PWM2                        RKXX_IRQ(76)
#define IRQ_PWM3                        RKXX_IRQ(77)
#define IRQ_RK_PWM                      RKXX_IRQ(78)
#define IRQ_WDT                         RKXX_IRQ(79)
#define IRQ_PMU                         RKXX_IRQ(80)
#define IRQ_GPIO0                       RKXX_IRQ(81)
#define IRQ_GPIO1                       RKXX_IRQ(82)
#define IRQ_GPIO2                       RKXX_IRQ(83)
#define IRQ_GPIO3                       RKXX_IRQ(84)
#define IRQ_GPIO4                       RKXX_IRQ(85)
#define IRQ_GPIO5                       RKXX_IRQ(86)
#define IRQ_GPIO6                       RKXX_IRQ(87)
#define IRQ_GPIO7                       RKXX_IRQ(88)
#define IRQ_GPIO8                       RKXX_IRQ(89)
#define IRQ_PERI_AHB_ARBITER0_USB       RKXX_IRQ(90)
#define IRQ_PERI_AHB_ARBITER1_EMEM      RKXX_IRQ(91)
#define IRQ_PERI_AHB_ARBITER2_MMU       RKXX_IRQ(92)
#define IRQ_OTG_ID                      RKXX_IRQ(93)
#define IRQ_OTG_BVALID                  RKXX_IRQ(94)
#define IRQ_OTG_LINESTATE               RKXX_IRQ(95)
#define IRQ_USBHOST0_LINESTATE          RKXX_IRQ(96)
#define IRQ_USBHOST1_LINESTATE          RKXX_IRQ(97)
#define IRQ_EPD_DP                      RKXX_IRQ(98)
#define IRQ_SDMMC_DETECT_N              RKXX_IRQ(99)
#define IRQ_SDIO0_DETECT_N              RKXX_IRQ(100)
#define IRQ_SDIO1_DETECT_N              RKXX_IRQ(101)
#define IRQ_HDMI_WAKEUP                 RKXX_IRQ(102)
#define IRQ_HDMI                        RKXX_IRQ(103)
#define IRQ_CCP                         RKXX_IRQ(104)
#define IRQ_CCS                         RKXX_IRQ(105)
#define IRQ_SDMMC_DETECT_DUAL_EDGE      RKXX_IRQ(106)
#define IRQ_GPIO7_B3_DUAL_EDGE          RKXX_IRQ(107)
#define IRQ_GPIO7_C6_DUAL_EDGE          RKXX_IRQ(108)
#define IRQ_GPIO8_A2_DUAL_EDGE          RKXX_IRQ(109)
#define IRQ_EPD_HDMI                    RKXX_IRQ(110)
#define IRQ_HEVC_MMU                    RKXX_IRQ(111)


#define NR_GIC_IRQS                     (5 * 32)
#define NR_GPIO_IRQS                    (9 * 32)
#define NR_BOARD_IRQS                   64
#define NR_IRQS                         (NR_GIC_IRQS + NR_GPIO_IRQS + NR_BOARD_IRQS)


#define NR_IRQS_MAXNUM			(NR_GIC_IRQS + NR_GPIO_IRQS)

#endif /* __RK3288_IRQS_H */

