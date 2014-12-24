/*
 *  Ralink RT305x SoC platform device registration
 *
 *  Copyright (C) 2009-2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/physmap.h>
#include <linux/spi/spi.h>
#include <linux/rt2x00_platform.h>

#include <asm/addrspace.h>

#include <asm/mach-ralink/rt305x.h>
#include <asm/mach-ralink/rt305x_regs.h>
#include "devices.h"

#include <ramips_eth_platform.h>
#include <rt305x_esw_platform.h>

static struct resource rt305x_flash0_resources[] = {
	{
		.flags	= IORESOURCE_MEM,
		.start	= KSEG1ADDR(RT305X_FLASH0_BASE),
		.end	= KSEG1ADDR(RT305X_FLASH0_BASE) +
			  RT305X_FLASH0_SIZE - 1,
	},
};

static struct platform_device rt305x_flash0_device = {
	.name		= "physmap-flash",
	.resource	= rt305x_flash0_resources,
	.num_resources	= ARRAY_SIZE(rt305x_flash0_resources),
};

static struct resource rt305x_flash1_resources[] = {
	{
		.flags	= IORESOURCE_MEM,
		.start	= KSEG1ADDR(RT305X_FLASH1_BASE),
		.end	= KSEG1ADDR(RT305X_FLASH1_BASE) +
			  RT305X_FLASH1_SIZE - 1,
	},
};

static struct platform_device rt305x_flash1_device = {
	.name		= "physmap-flash",
	.resource	= rt305x_flash1_resources,
	.num_resources	= ARRAY_SIZE(rt305x_flash1_resources),
};

static int rt305x_flash_instance __initdata;
void __init rt305x_register_flash(unsigned int id,
				  struct physmap_flash_data *pdata)
{
	struct platform_device *pdev;
	u32 t;
	int reg;

	switch (id) {
	case 0:
		pdev = &rt305x_flash0_device;
		reg = MEMC_REG_FLASH_CFG0;
		break;
	case 1:
		pdev = &rt305x_flash1_device;
		reg = MEMC_REG_FLASH_CFG1;
		break;
	default:
		return;
	}

	t = rt305x_memc_rr(reg);
	t = (t >> FLASH_CFG_WIDTH_SHIFT) & FLASH_CFG_WIDTH_MASK;

	switch (t) {
	case FLASH_CFG_WIDTH_8BIT:
		pdata->width = 1;
		break;
	case FLASH_CFG_WIDTH_16BIT:
		pdata->width = 2;
		break;
	case FLASH_CFG_WIDTH_32BIT:
		pdata->width = 4;
		break;
	default:
		printk(KERN_ERR "RT305x: flash bank%u witdh is invalid\n", id);
		return;
	}

	pdev->dev.platform_data = pdata;
	pdev->id = rt305x_flash_instance;

	platform_device_register(pdev);
	rt305x_flash_instance++;
}

static void rt305x_fe_reset(void)
{
	rt305x_sysc_wr(RT305X_RESET_FE, SYSC_REG_RESET_CTRL);
	rt305x_sysc_wr(0, SYSC_REG_RESET_CTRL);
}

static struct resource rt305x_eth_resources[] = {
	{
		.start	= RT305X_FE_BASE,
		.end	= RT305X_FE_BASE + PAGE_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	}, {
		.start	= RT305X_CPU_IRQ_FE,
		.end	= RT305X_CPU_IRQ_FE,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct ramips_eth_platform_data ramips_eth_data = {
	.mac = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 },
	.reset_fe = rt305x_fe_reset,
	.min_pkt_len = 64,
};

static struct platform_device rt305x_eth_device = {
	.name		= "ramips_eth",
	.resource	= rt305x_eth_resources,
	.num_resources	= ARRAY_SIZE(rt305x_eth_resources),
	.dev = {
		.platform_data = &ramips_eth_data,
	}
};

static struct resource rt305x_esw_resources[] = {
	{
		.start	= RT305X_SWITCH_BASE,
		.end	= RT305X_SWITCH_BASE + PAGE_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
};

struct rt305x_esw_platform_data rt305x_esw_data = {
	.vlan_config		= RT305X_ESW_VLAN_CONFIG_NONE,
	.reg_initval_fct2	= 0x00d6500c,
	.reg_initval_fpa2	= 0x3f502b28,
};

static struct platform_device rt305x_esw_device = {
	.name		= "rt305x-esw",
	.resource	= rt305x_esw_resources,
	.num_resources	= ARRAY_SIZE(rt305x_esw_resources),
	.dev = {
		.platform_data = &rt305x_esw_data,
	}
};

void __init rt305x_register_ethernet(void)
{
	struct clk *clk;

	clk = clk_get(NULL, "sys");
	if (IS_ERR(clk))
		panic("unable to get SYS clock, err=%ld", PTR_ERR(clk));

	ramips_eth_data.sys_freq = clk_get_rate(clk);

	platform_device_register(&rt305x_esw_device);
	platform_device_register(&rt305x_eth_device);
}

static struct resource rt305x_wifi_resources[] = {
	{
		.start	= RT305X_WMAC_BASE,
		.end	= RT305X_WMAC_BASE + 0x3FFFF,
		.flags	= IORESOURCE_MEM,
	}, {
		.start	= RT305X_CPU_IRQ_WNIC,
		.end	= RT305X_CPU_IRQ_WNIC,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct rt2x00_platform_data rt305x_wifi_data;
static struct platform_device rt305x_wifi_device = {
	.name			= "rt2800_wmac",
	.resource		= rt305x_wifi_resources,
	.num_resources	= ARRAY_SIZE(rt305x_wifi_resources),
	.dev = {
		.platform_data = &rt305x_wifi_data,
	}
};

void __init rt305x_register_wifi(void)
{
	rt305x_wifi_data.eeprom_file_name = "RT305X.eeprom";
	platform_device_register(&rt305x_wifi_device);
}

static struct resource rt305x_wdt_resources[] = {
	{
		.start	= RT305X_TIMER_BASE,
		.end	= RT305X_TIMER_BASE + RT305X_TIMER_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device rt305x_wdt_device = {
	.name		= "ramips-wdt",
	.id		= -1,
	.resource	= rt305x_wdt_resources,
	.num_resources	= ARRAY_SIZE(rt305x_wdt_resources),
};

void __init rt305x_register_wdt(void)
{
	u32 t;

	/* enable WDT reset output on pin SRAM_CS_N */
	t = rt305x_sysc_rr(SYSC_REG_SYSTEM_CONFIG);
	t |= SYSTEM_CONFIG_SRAM_CS0_MODE_WDT <<
	     SYSTEM_CONFIG_SRAM_CS0_MODE_SHIFT;
	rt305x_sysc_wr(t, SYSC_REG_SYSTEM_CONFIG);

	platform_device_register(&rt305x_wdt_device);
}

static struct resource rt305x_spi_resources[] = {
	{
		.flags	= IORESOURCE_MEM,
		.start	= RT305X_SPI_BASE,
		.end	= RT305X_SPI_BASE + RT305X_SPI_SIZE - 1,
	},
};

static struct platform_device rt305x_spi_device = {
	.name		= "ramips-spi",
	.id		= 0,
	.resource	= rt305x_spi_resources,
	.num_resources	= ARRAY_SIZE(rt305x_spi_resources),
};

void __init rt305x_register_spi(struct spi_board_info *info, int n)
{
	spi_register_board_info(info, n);
	platform_device_register(&rt305x_spi_device);
}

static struct resource rt305x_usb_resources[] = {
	{
		.start	= RT305X_OTG_BASE,
		.end	= RT305X_OTG_BASE + 0x3FFFF,
		.flags	= IORESOURCE_MEM,
	}, {
		.start	= RT305X_INTC_IRQ_OTG,
		.end	= RT305X_INTC_IRQ_OTG,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device rt305x_usb_device = {
	.name			= "dwc_otg",
	.resource		= rt305x_usb_resources,
	.num_resources	= ARRAY_SIZE(rt305x_usb_resources),
	.dev = {
		.platform_data = NULL,
	}
};

void __init rt305x_register_usb(void)
{
	platform_device_register(&rt305x_usb_device);
}
