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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#ifndef __RK3036_TVE_H__
#define __RK3036_TVE_H__

#include <lcd.h>

#define TV_CTRL			(0x00)
	#define m_CVBS_MODE			(1 << 24)
	#define m_CLK_UPSTREAM_EN		(3 << 18)
	#define m_TIMING_EN			(3 << 16)
	#define m_LUMA_FILTER_GAIN		(3 << 9)
	#define m_LUMA_FILTER_BW		(1 << 8)
	#define m_CSC_PATH			(3 << 1)
	
	#define v_CVBS_MODE(x)			( (x & 1) << 24)
	#define v_CLK_UPSTREAM_EN(x)		( (x & 3) << 18)
	#define v_TIMING_EN(x)			( (x & 3) << 16)
	#define v_LUMA_FILTER_GAIN(x)		( (x & 3) << 9)
	#define v_LUMA_FILTER_UPSAMPLE(x)	( (x & 1) << 8)
	#define v_CSC_PATH(x)			( (x & 3) << 1)
	
#define TV_SYNC_TIMING		(0x04)
#define TV_ACT_TIMING		(0x08)
#define TV_ADJ_TIMING		(0x0c)
#define TV_FREQ_SC		(0x10)
#define TV_LUMA_FILTER0		(0x14)
#define TV_LUMA_FILTER1		(0x18)
#define TV_LUMA_FILTER2		(0x1C)
#define TV_ACT_ST		(0x34)
#define TV_ROUTING		(0x38)
	#define m_DAC_SENSE_EN		(1 << 27)
	#define m_Y_IRE_7_5		(1 << 19)
	#define m_Y_AGC_PULSE_ON	(1 << 15)
	#define m_Y_VIDEO_ON		(1 << 11)
	#define m_Y_SYNC_ON		(1 << 7)
	#define m_YPP_MODE		(1 << 3)
	#define m_MONO_EN		(1 << 2)
	#define m_PIC_MODE		(1 << 1)

	#define v_DAC_SENSE_EN(x)	((x & 1) << 27)
	#define v_Y_IRE_7_5(x)		((x & 1) << 19)
	#define v_Y_AGC_PULSE_ON(x)	((x & 1) << 15)
	#define v_Y_VIDEO_ON(x)		((x & 1) << 11)
	#define v_Y_SYNC_ON(x)		((x & 1) << 7)
	#define v_YPP_MODE(x)		((x & 1) << 3)
	#define v_MONO_EN(x)		((x & 1) << 2)
	#define v_PIC_MODE(x)		((x & 1) << 1)
	
#define TV_SYNC_ADJUST		(0x50)
#define TV_STATUS		(0x54)
#define TV_RESET		(0x68)
	#define m_RESET			(1 << 1);
	#define v_RESET(x)		( (x & 1) << 1)
#define TV_SATURATION		(0x78)
#define TV_BW_CTRL		(0x8C)
	#define m_CHROMA_BW	(3 << 4)
	#define m_COLOR_DIFF_BW	(0xf)
	
	enum {
		BP_FILTER_PASS = 0,
		BP_FILTER_NTSC,
		BP_FILTER_PAL,
	};
	enum {
		COLOR_DIFF_FILTER_OFF = 0,
		COLOR_DIFF_FILTER_BW_0_6,
		COLOR_DIFF_FILTER_BW_1_3,
		COLOR_DIFF_FILTER_BW_2_0
	};
	
	#define v_CHROMA_BW(x)		((3 & x) << 4)
	#define v_COLOR_DIFF_BW(x)	(0xF & x)
	
#define TV_BRIGHTNESS_CONTRAST	(0x90)

#define m_EXTREF_EN		(1 << 0)
#define m_VBG_EN		(1 << 1)
#define m_DAC_EN		(1 << 2)
#define m_SENSE_EN		(1 << 3)
#define m_BIAS_EN		(7 << 4)
#define m_DAC_GAIN		(0x3f << 7)
#define v_DAC_GAIN(x)		((x & 0x3f) << 7)

enum {
	TVOUT_CVBS_NTSC = 0,
	TVOUT_CVBS_PAL,
};

enum {
	SOC_RK3036 = 0,
	SOC_RK312X
};

#define TVOUT_DEAULT TVOUT_CVBS_PAL

struct rk3036_tve {
	u32				reg_phy_base;
	int				soctype;
	int				test_mode;
	int				saturation;
};

#define RK30_TVE_REGBASE 0x10118000 + 0x200
#define MAX_TVE_COUNT  2

extern  int rk3036_tve_init(vidinfo_t *panel);

#endif /* __RK3036_TVE_H__*/
