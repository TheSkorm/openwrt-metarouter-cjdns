/*
 *  Atheros AR9XXX SoCs built-in WMAC device support
 *
 *  Copyright (C) 2010-2011 Jaiganesh Narayanan <jnarayanan@atheros.com>
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  Parts of this file are based on Atheros 2.6.15/2.6.31 BSP
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/etherdevice.h>
#include <linux/platform_device.h>
#include <linux/ath9k_platform.h>

#include <asm/mach-ar71xx/ar71xx.h>

#include "dev-ar9xxx-wmac.h"

#define MHZ_25	(25 * 1000 * 1000)

static struct ath9k_platform_data ar9xxx_wmac_data = {
	.led_pin = -1,
};
static char ar9xxx_wmac_mac[6];

static struct resource ar9xxx_wmac_resources[] = {
	{
		/* .start and .end fields are filled dynamically */
		.flags	= IORESOURCE_MEM,
	}, {
		.start	= AR71XX_CPU_IRQ_IP2,
		.end	= AR71XX_CPU_IRQ_IP2,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device ar9xxx_wmac_device = {
	.name		= "ath9k",
	.id		= -1,
	.resource	= ar9xxx_wmac_resources,
	.num_resources	= ARRAY_SIZE(ar9xxx_wmac_resources),
	.dev = {
		.platform_data = &ar9xxx_wmac_data,
	},
};

static void ar913x_wmac_init(void)
{
	ar71xx_device_stop(RESET_MODULE_AMBA2WMAC);
	mdelay(10);

	ar71xx_device_start(RESET_MODULE_AMBA2WMAC);
	mdelay(10);

	ar9xxx_wmac_resources[0].start = AR91XX_WMAC_BASE;
	ar9xxx_wmac_resources[0].end = AR91XX_WMAC_BASE + AR91XX_WMAC_SIZE - 1;
}

static int ar933x_r1_get_wmac_revision(void)
{
	return ar71xx_soc_rev;
}

static int ar933x_wmac_reset(void)
{
	unsigned retries = 0;

	ar71xx_device_stop(AR933X_RESET_WMAC);
	ar71xx_device_start(AR933X_RESET_WMAC);

	while (1) {
		u32 bootstrap;

		bootstrap = ar71xx_reset_rr(AR933X_RESET_REG_BOOTSTRAP);
		if ((bootstrap & AR933X_BOOTSTRAP_EEPBUSY) == 0)
			return 0;

		if (retries > 20)
			break;

		udelay(10000);
		retries++;
	}

	pr_err("ar93xx: WMAC reset timed out");
	return -ETIMEDOUT;
}

static void ar933x_wmac_init(void)
{
	ar9xxx_wmac_device.name = "ar933x_wmac";
	ar9xxx_wmac_resources[0].start = AR933X_WMAC_BASE;
	ar9xxx_wmac_resources[0].end = AR933X_WMAC_BASE + AR933X_WMAC_SIZE - 1;
	if (ar71xx_ref_freq == MHZ_25)
		ar9xxx_wmac_data.is_clk_25mhz = true;

	if (ar71xx_soc_rev == 1)
		ar9xxx_wmac_data.get_mac_revision = ar933x_r1_get_wmac_revision;

	ar9xxx_wmac_data.external_reset = ar933x_wmac_reset;

	ar933x_wmac_reset();
}

static void ar934x_wmac_init(void)
{
	ar9xxx_wmac_device.name = "ar934x_wmac";
	ar9xxx_wmac_resources[0].start = AR934X_WMAC_BASE;
	ar9xxx_wmac_resources[0].end = AR934X_WMAC_BASE + AR934X_WMAC_SIZE - 1;
	ar9xxx_wmac_resources[1].start = AR934X_IP2_IRQ_WMAC;
	ar9xxx_wmac_resources[1].start = AR934X_IP2_IRQ_WMAC;
	if (ar71xx_ref_freq == MHZ_25)
		ar9xxx_wmac_data.is_clk_25mhz = true;
}

void __init ar9xxx_add_device_wmac(u8 *cal_data, u8 *mac_addr)
{
	switch (ar71xx_soc) {
	case AR71XX_SOC_AR9130:
	case AR71XX_SOC_AR9132:
		ar913x_wmac_init();
		break;

	case AR71XX_SOC_AR9330:
	case AR71XX_SOC_AR9331:
		ar933x_wmac_init();
		break;

	case AR71XX_SOC_AR9341:
	case AR71XX_SOC_AR9342:
	case AR71XX_SOC_AR9344:
		ar934x_wmac_init();
		break;

	default:
		BUG();
	}

	if (cal_data)
		memcpy(ar9xxx_wmac_data.eeprom_data, cal_data,
		       sizeof(ar9xxx_wmac_data.eeprom_data));

	if (mac_addr) {
		memcpy(ar9xxx_wmac_mac, mac_addr, sizeof(ar9xxx_wmac_mac));
		ar9xxx_wmac_data.macaddr = ar9xxx_wmac_mac;
	}

	platform_device_register(&ar9xxx_wmac_device);
}
