/*
 * (C) Copyright 2008-2014 Rockchip Electronics
 * zyw, rockchips iomux driver
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

#include <common.h>
#include <asm/arch/rkplat.h>

#ifdef CONFIG_RK_IOMUX


#if defined(CONFIG_RKCHIP_RK3288)
	#include "iomux-rk3288.c"
#elif defined(CONFIG_RKCHIP_RK3036)
	#include "iomux-rk3036.c"
#elif defined(CONFIG_RKCHIP_RK3126) || defined(CONFIG_RKCHIP_RK3128)
	#include "iomux-rk312X.c"
#else
	#error "PLS config iomux-rk32xx.c!"
#endif


void rk_iomux_config(int iomux_id)
{
	switch (iomux_id){
		case RK_PWM0_IOMUX:
		case RK_PWM1_IOMUX:
		case RK_PWM2_IOMUX:
		case RK_PWM3_IOMUX:
		case RK_PWM4_IOMUX:
			rk_pwm_iomux_config(iomux_id);
			break;
		case RK_I2C0_IOMUX:        
		case RK_I2C1_IOMUX:
		case RK_I2C2_IOMUX:
		case RK_I2C3_IOMUX:
		case RK_I2C4_IOMUX:
			rk_i2c_iomux_config(iomux_id);
			break;

		case RK_UART0_IOMUX:
		case RK_UART1_IOMUX:
		case RK_UART2_IOMUX:
		case RK_UART3_IOMUX:
		case RK_UART4_IOMUX:
			rk_uart_iomux_config(iomux_id);
			break;    
		case RK_LCDC0_IOMUX:
		case RK_LCDC1_IOMUX:
			rk_lcdc_iomux_config(iomux_id);
			break;
		case RK_SPI0_CS0_IOMUX:
		case RK_SPI0_CS1_IOMUX:
		case RK_SPI1_CS0_IOMUX:
		case RK_SPI1_CS1_IOMUX:
		case RK_SPI2_CS0_IOMUX:
		case RK_SPI2_CS1_IOMUX:
			rk_spi_iomux_config(iomux_id);
			break;
		case RK_EMMC_IOMUX:
			rk_emmc_iomux_config(iomux_id);
			break;
		case RK_SDCARD_IOMUX:
			rk_sdcard_iomux_config(iomux_id);
			break;
		case RK_HDMI_IOMUX:
			rk_hdmi_iomux_config(iomux_id);
			break;
		default :
			printf("RK have not this iomux id!\n");
			break;
	}
}

#else

void rk_iomux_config(int iomux_id) {};

#endif /* CONFIG_RK_IOMUX */

