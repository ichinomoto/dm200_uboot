/*
 *  Copyright (C) 2012 rockchips
 * zhangqing <zhangqing@rock-chips.com>
 * 
 */

/*#define DEBUG*/
#include <common.h>
#include <fdtdec.h>
#include <errno.h>
#include <power/act8931_pmic.h>
#include <asm/arch/rkplat.h>

DECLARE_GLOBAL_DATA_PTR;

struct pmic_act8931 act8931;

static struct fdt_regulator_match act8931_reg_matches[] = {
	{ .prop = "act_dcdc1" ,.min_uV = 3300000, .max_uV = 3300000, .boot_on =1},
	{ .prop = "act_dcdc2" ,.min_uV = 1200000, .max_uV = 1200000, .boot_on = 1},
	{ .prop = "act_dcdc3", .min_uV = 700000, .max_uV = 1500000, .boot_on = 1},
	{ .prop = "act_ldo1", .min_uV = 2800000, .max_uV = 2800000, .boot_on = 1 },
	{ .prop = "act_ldo2", .min_uV = 1800000, .max_uV = 1800000, .boot_on = 1 },
	{ .prop = "act_ldo3", .min_uV = 3300000, .max_uV = 3300000, .boot_on = 1 },
	{ .prop = "act_ldo4", .min_uV = 3300000, .max_uV = 3300000, .boot_on = 1 },
};


const static int voltage_map[] = {
	 600, 625, 650, 675, 700, 725, 750, 775,
	 800, 825, 850, 875, 900, 925, 950, 975,
	 1000, 1025, 1050, 1075, 1100, 1125, 1150,
	 1175, 1200, 1250, 1300, 1350, 1400, 1450,
	 1500, 1550, 1600, 1650, 1700, 1750, 1800, 
	 1850, 1900, 1950, 2000, 2050, 2100, 2150, 
	 2200, 2250, 2300, 2350, 2400, 2500, 2600, 
	 2700, 2800, 2850, 2900, 3000, 3100, 3200,
	 3300, 3400, 3500, 3600, 3700, 3800, 3900,
};


static struct act8931_reg_table act8931_regulators[] = {
	{
		.name		= "act_dcdc1",   //vccio
		.reg_ctl	= ACT8931_BUCK1_CONTR_BASE,
		.reg_vol	= ACT8931_BUCK1_SET_VOL_BASE,
	},
	{
		.name		= "act_dcdc2",    //ddr
		.reg_ctl	= ACT8931_BUCK2_CONTR_BASE,
		.reg_vol	= ACT8931_BUCK2_SET_VOL_BASE,
	},
	{
		.name		= "act_dcdc3",   //arm
		.reg_ctl	= ACT8931_BUCK3_CONTR_BASE,
		.reg_vol	= ACT8931_BUCK3_SET_VOL_BASE,
	},
	{
		.name		= "act_ldo1",   //
		.reg_ctl	= ACT8931_LDO1_CONTR_BASE,
		.reg_vol	= ACT8931_LDO1_SET_VOL_BASE,
	},
	{
		.name		= "act_ldo2",  //
		.reg_ctl	= ACT8931_LDO2_CONTR_BASE,
		.reg_vol	= ACT8931_LDO2_SET_VOL_BASE,
	},
	{
		.name		= "act_ldo3",   //
		.reg_ctl	= ACT8931_LDO3_CONTR_BASE,
		.reg_vol	= ACT8931_LDO3_SET_VOL_BASE,
	},
	{
		.name		= "act_ldo4",   //
		.reg_ctl	= ACT8931_LDO4_CONTR_BASE,
		.reg_vol	= ACT8931_LDO4_SET_VOL_BASE,
	},

 };


static int act8931_set_bits(uint reg_addr, uchar  mask, uchar val)
{
	uchar tmp = 0;
	int ret =0;

	ret = I2C_READ(reg_addr,&tmp);
	if (ret == 0){
		tmp = (tmp & ~mask) | val;
		ret = I2C_WRITE(reg_addr,&tmp);
	}
	return 0;	
}




/*
set charge current
0. disable charging  
1. usb charging, 500mA
2. ac adapter charging, 1.5A
*/
int pmic_act8931_charger_setting(int current)
{

    printf("%s %d\n",__func__,current);
    switch (current){
    case 0:
        //disable charging
        break;
    case 1:
        //set charger current to 500ma
        break;
    case 2:
         //set charger current to 1.5A
        break;
    default:
        break;
    }
    return 0;
}


static int check_volt_table(const int *volt_table,int volt)
{
	int i=0;

	for(i=VOL_MIN_IDX;i<VOL_MAX_IDX;i++){
		if(volt <= (volt_table[i]*1000))
			return i;
	}
	return -1;
}

int act8931_regulator_set(void)
{
	int i;
	int volt;
	uchar reg_val;
	struct fdt_regulator_match *match;
	struct act8931_reg_table *regulator;
	for (i = 0; i < ACT8931_NUM_REGULATORS; i++) {
		match = &act8931_reg_matches[i];
		if (match->boot_on && (match->min_uV == match->max_uV)) {
			volt = match->min_uV;
			reg_val = check_volt_table(voltage_map, volt);
			regulator = &act8931_regulators[i];
			if ((volt == 0) || (reg_val == -1)) {
				printf("invalid volt = %d or reg_val = %d\n", volt, reg_val);
				continue;
			}
			act8931_set_bits(regulator->reg_vol,LDO_VOL_MASK,reg_val);
			act8931_set_bits(regulator->reg_ctl,LDO_EN_MASK,LDO_EN_MASK);
			debug("set %s--%s volt:%d\n", match->prop, match->name, volt);
		}
	}

	return 0;
}

static int act8931_i2c_probe(u32 bus ,u32 addr)
{
	char val;
	int ret;
	i2c_set_bus_num(bus);
	i2c_init(ACT8931_I2C_SPEED, 0);
	ret = i2c_probe(addr);
	if (ret < 0)
		return -ENODEV;
	val = i2c_reg_read(addr, 0x22);
	if (val == 0xff)
		return -ENODEV;
	else
		return 0;
	
	
}

static int act8931_parse_dt(const void* blob)
{
	int node, nd;
	struct fdt_gpio_state gpios[2];
	u32 bus, addr;
	int ret;

	node = fdt_node_offset_by_compatible(blob,
					0, COMPAT_ACTIVE_ACT8931);
	if (node < 0) {
		printf("can't find dts node for act8931\n");
		return -ENODEV;
	}

	if (!fdt_device_is_available(blob,node)) {
		debug("device act8931 is disabled\n");
		return -1;
	}
	ret = fdt_get_i2c_info(blob, node, &bus, &addr);
	if (ret < 0) {
		debug("pmic act8931 get fdt i2c failed\n");
		return ret;
	}

	ret = act8931_i2c_probe(bus, addr);
	if (ret < 0) {
		debug("pmic act8931 i2c probe failed\n");
		return ret;
	}
	
	nd = fdt_get_regulator_node(blob, node);
	if (nd < 0)
		printf("%s: Cannot find regulators\n", __func__);
	else
		fdt_regulator_match(blob, nd, act8931_reg_matches,
					ACT8931_NUM_REGULATORS);

	fdtdec_decode_gpios(blob, node, "gpios", gpios, 2);

	act8931.pmic = pmic_alloc();
	act8931.node = node;
	act8931.pmic->hw.i2c.addr = addr;
	act8931.pmic->bus = bus;
	act8931.pwr_hold.gpio = gpios[1].gpio;
	act8931.pwr_hold.flags = !(gpios[1].flags  & OF_GPIO_ACTIVE_LOW);

	return 0;
}


int pmic_act8931_init(unsigned char bus)
{
	int ret;
	if (!act8931.pmic) {
		ret = act8931_parse_dt(gd->fdt_blob);
		if (ret < 0)
			return ret;
	}
	gpio_direction_output(act8931.pwr_hold.gpio,
			act8931.pwr_hold.flags); /*power hold*/
	i2c_set_bus_num(act8931.pmic->bus);
	i2c_init(ACT8931_I2C_SPEED, 0);
	act8931_regulator_set( );

	return 0;
}

void pmic_act8931_shut_down(void)
{
	gpio_direction_output(act8931.pwr_hold.gpio,
			!(act8931.pwr_hold.flags)); 
	mdelay(100);
}

