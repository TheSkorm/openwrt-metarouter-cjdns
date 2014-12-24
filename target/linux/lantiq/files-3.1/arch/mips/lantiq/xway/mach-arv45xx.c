/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2010 John Crispin <blogic@openwrt.org>
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/gpio.h>
#include <linux/gpio_buttons.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>
#include <linux/input.h>
#include <linux/etherdevice.h>
#include <linux/ath5k_platform.h>
#include <linux/pci.h>

#include <lantiq_soc.h>
#include <lantiq_platform.h>
#include <dev-gpio-leds.h>
#include <dev-gpio-buttons.h>

#include "../machtypes.h"
#include "dev-wifi-rt2x00.h"
#include "dev-wifi-ath5k.h"
#include "devices.h"
#include "dev-dwc_otg.h"

static struct mtd_partition arv4510_partitions[] =
{
	{
		.name	= "uboot",
		.offset	= 0x0,
		.size	= 0x20000,
	},
	{
		.name	= "uboot_env",
		.offset	= 0x20000,
		.size	= 0x120000,
	},
	{
		.name	= "linux",
		.offset	= 0x40000,
		.size	= 0xfa0000,
	},
	{
		.name	= "board_config",
		.offset	= 0xfe0000,
		.size	= 0x20000,
	},
};

static struct mtd_partition arv45xx_partitions[] =
{
	{
		.name	= "uboot",
		.offset	= 0x0,
		.size	= 0x20000,
	},
	{
		.name	= "uboot_env",
		.offset	= 0x20000,
		.size	= 0x10000,
	},
	{
		.name	= "linux",
		.offset	= 0x30000,
		.size	= 0x3c0000,
	},
	{
		.name	= "board_config",
		.offset	= 0x3f0000,
		.size	= 0x10000,
	},
};

static struct mtd_partition arv7525_partitions[] =
{
	{
		.name	= "uboot",
		.offset	= 0x0,
		.size	= 0x10000,
	},
	{
		.name	= "uboot_env",
		.offset	= 0x10000,
		.size	= 0x10000,
	},
	{
		.name	= "linux",
		.offset	= 0x20000,
		.size	= 0x3d0000,
	},
	{
		.name	= "board_config",
		.offset	= 0x3f0000,
		.size	= 0x10000,
	},
};

static struct mtd_partition arv75xx_partitions[] =
{
	{
		.name	= "uboot",
		.offset	= 0x0,
		.size	= 0x10000,
	},
	{
		.name	= "uboot_env",
		.offset	= 0x10000,
		.size	= 0x10000,
	},
	{
		.name	= "linux",
		.offset	= 0x20000,
		.size	= 0x7d0000,
	},
	{
		.name	= "board_config",
		.offset	= 0x7f0000,
		.size	= 0x10000,
	},
};

static struct physmap_flash_data arv4510_flash_data = {
	.nr_parts	= ARRAY_SIZE(arv4510_partitions),
	.parts		= arv4510_partitions,
};

static struct physmap_flash_data arv45xx_flash_data = {
	.nr_parts	= ARRAY_SIZE(arv45xx_partitions),
	.parts		= arv45xx_partitions,
};

static struct physmap_flash_data arv7525_flash_data = {
	.nr_parts	= ARRAY_SIZE(arv7525_partitions),
	.parts		= arv7525_partitions,
};

static struct physmap_flash_data arv75xx_flash_data = {
	.nr_parts	= ARRAY_SIZE(arv75xx_partitions),
	.parts		= arv75xx_partitions,
};

static struct ltq_pci_data ltq_pci_data = {
	.clock  = PCI_CLOCK_EXT,
	.gpio   = PCI_GNT1 | PCI_REQ1,
	.irq    = {
		[14] = INT_NUM_IM0_IRL0 + 22,
	},
};

static struct ltq_eth_data ltq_eth_data = {
	.mii_mode	= PHY_INTERFACE_MODE_RMII,
};

static struct gpio_led
arv4510pw_gpio_leds[] __initdata = {
	{ .name = "soc:green:foo", .gpio = 4, .active_low = 1, },
};

static struct gpio_led
arv4518pw_gpio_leds[] __initdata = {
	{ .name = "soc:green:power", .gpio = 3, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:adsl", .gpio = 4, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:internet", .gpio = 5, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:wifi", .gpio = 6, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:yellow:wps", .gpio = 7, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:red:fail", .gpio = 8, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:usb", .gpio = 19, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:voip", .gpio = 100, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:fxs1", .gpio = 101, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:fxs2", .gpio = 102, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:fxo", .gpio = 103, .active_low = 1, .default_trigger = "default-on" },
};

static struct gpio_keys_button
arv4518pw_gpio_keys[] __initdata = {
	{
		.desc		= "wifi",
		.type		= EV_KEY,
		.code		= BTN_0,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 28,
		.active_low	= 1,
	},
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= BTN_1,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 30,
		.active_low	= 1,
	},
	{
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= BTN_2,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 29,
		.active_low	= 1,
	},
};

static struct gpio_led
arv4520pw_gpio_leds[] __initdata = {
	{ .name = "soc:blue:power", .gpio = 3, .active_low = 1, },
	{ .name = "soc:blue:adsl", .gpio = 4, .active_low = 1, },
	{ .name = "soc:blue:internet", .gpio = 5, .active_low = 1, },
	{ .name = "soc:red:power", .gpio = 6, .active_low = 1, },
	{ .name = "soc:yellow:wps", .gpio = 7, .active_low = 1, },
	{ .name = "soc:red:wps", .gpio = 9, .active_low = 1, },
	{ .name = "soc:blue:voip", .gpio = 100, .active_low = 1, },
	{ .name = "soc:blue:fxs1", .gpio = 101, .active_low = 1, },
	{ .name = "soc:blue:fxs2", .gpio = 102, .active_low = 1, },
	{ .name = "soc:blue:fxo", .gpio = 103, .active_low = 1, },
	{ .name = "soc:blue:voice", .gpio = 104, .active_low = 1, },
	{ .name = "soc:blue:usb", .gpio = 105, .active_low = 1, },
	{ .name = "soc:blue:wifi", .gpio = 106, .active_low = 1, },
};

static struct gpio_led
arv452cpw_gpio_leds[] __initdata = {
	{ .name = "soc:blue:power", .gpio = 3, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:blue:adsl", .gpio = 4, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:blue:isdn", .gpio = 5, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:red:power", .gpio = 6, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:yellow:wps", .gpio = 7, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:red:wps", .gpio = 9, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:blue:fxs1", .gpio = 100, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:blue:fxs2", .gpio = 101, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:blue:wps", .gpio = 102, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:blue:fxo", .gpio = 103, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:blue:voice", .gpio = 104, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:blue:usb", .gpio = 105, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:blue:wifi", .gpio = 106, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:blue:internet", .gpio = 108, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:red:internet", .gpio = 109, .active_low = 1, .default_trigger = "default-on" },
};

static struct gpio_led
arv4525pw_gpio_leds[] __initdata = {
	{ .name = "soc:green:fxs1", .gpio = 4, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:fxs2", .gpio = 5, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:red:dsl", .gpio = 6, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:wifi", .gpio = 8, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:online", .gpio = 9, .active_low = 1, .default_trigger = "default-on" },
};

static struct gpio_led
arv752dpw22_gpio_leds[] __initdata = {
	{ .name = "soc:blue:power", .gpio = 3, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:red:internet", .gpio = 5, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:red:power", .gpio = 6, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:red:wps", .gpio = 8, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:red:fxo", .gpio = 103, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:red:voice", .gpio = 104, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:usb", .gpio = 105, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:wifi", .gpio = 106, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:wifi1", .gpio = 107, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:blue:wifi", .gpio = 108, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:blue:wifi1", .gpio = 109, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:eth1", .gpio = 111, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:eth2", .gpio = 112, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:eth3", .gpio = 113, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:eth4", .gpio = 114, .active_low = 1, .default_trigger = "default-on", },
};

static struct gpio_keys_button
arv752dpw22_gpio_keys[] __initdata = {
	{
		.desc		= "btn0",
		.type		= EV_KEY,
		.code		= BTN_0,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 12,
		.active_low	= 1,
	},
	{
		.desc		= "btn1",
		.type		= EV_KEY,
		.code		= BTN_1,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 13,
		.active_low	= 1,
	},
	{
		.desc 		= "btn2",
		.type 		= EV_KEY,
		.code 		= BTN_2,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio 		= 28,
		.active_low 	= 1,
	},
};

static struct gpio_led
arv7518pw_gpio_leds[] __initdata = {
	{ .name = "soc:green:power", .gpio = 2, .active_low = 1, },
	{ .name = "soc:green:adsl", .gpio = 4, .active_low = 1, },
	{ .name = "soc:green:internet", .gpio = 5, .active_low = 1, },
	{ .name = "soc:green:wifi", .gpio = 6, .active_low = 1, },
	{ .name = "soc:red:internet", .gpio = 8, .active_low = 1, },
	{ .name = "soc:green:usb", .gpio = 19, .active_low = 1, },
};

static struct gpio_keys_button
arv7518pw_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= BTN_0,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 23,
		.active_low	= 1,
	},
	{
		.desc		= "wifi",
		.type		= EV_KEY,
		.code		= BTN_1,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 25,
		.active_low	= 1,
	},
};

static struct gpio_keys_button
arv7525pw_gpio_keys[] __initdata = {
	{
		.desc		= "restart",
		.type		= EV_KEY,
		.code		= BTN_0,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 29,
		.active_low	= 1,
	},
};

static void
arv45xx_register_ethernet(void)
{
#define ARV45XX_BRN_MAC			0x3f0016
	memcpy_fromio(&ltq_eth_data.mac.sa_data,
		(void *)KSEG1ADDR(LTQ_FLASH_START + ARV45XX_BRN_MAC), 6);
	ltq_register_etop(&ltq_eth_data);
}

static void
arv75xx_register_ethernet(void)
{
#define ARV75XX_BRN_MAC			0x7f0016
	memcpy_fromio(&ltq_eth_data.mac.sa_data,
		(void *)KSEG1ADDR(LTQ_FLASH_START + ARV75XX_BRN_MAC), 6);
	ltq_register_etop(&ltq_eth_data);
}

static void
bewan_register_ethernet(void)
{
#define BEWAN_BRN_MAC			0x3f0014
	memcpy_fromio(&ltq_eth_data.mac.sa_data,
		(void *)KSEG1ADDR(LTQ_FLASH_START + BEWAN_BRN_MAC), 6);
	ltq_register_etop(&ltq_eth_data);
}

static u16 arv45xx_ath5k_eeprom_data[ATH5K_PLAT_EEP_MAX_WORDS];
static u8 arv45xx_ath5k_eeprom_mac[6];

void __init
arv45xx_register_ath5k(void)
{
#define ARV45XX_BRN_ATH		0x3f0478
	int i;
	static u16 eeprom_data[ATH5K_PLAT_EEP_MAX_WORDS];
	u32 *p = (u32*)arv45xx_ath5k_eeprom_data;

	memcpy_fromio(arv45xx_ath5k_eeprom_mac,
		(void *)KSEG1ADDR(LTQ_FLASH_START + ARV45XX_BRN_MAC), 6);
	arv45xx_ath5k_eeprom_mac[5]++;
	memcpy_fromio(arv45xx_ath5k_eeprom_data,
		(void *)KSEG1ADDR(LTQ_FLASH_START + ARV45XX_BRN_ATH), ATH5K_PLAT_EEP_MAX_WORDS);
	// swap eeprom bytes
	for (i = 0; i < ATH5K_PLAT_EEP_MAX_WORDS>>1; i++){
		//arv4518_ath5k_eeprom_data[i] = ((eeprom_data[i]&0xff)<<8)|((eeprom_data[i]&0xff00)>>8);
		p[i] = ((eeprom_data[(i<<1)+1]&0xff)<<24)|((eeprom_data[(i<<1)+1]&0xff00)<<8)|((eeprom_data[i<<1]&0xff)<<8)|((eeprom_data[i<<1]&0xff00)>>8);
		if (i == 0xbf>>1){
			// printk ("regdomain: 0x%x --> 0x%x\n", p[i], (p[i] & 0xffff0000)|0x67);
			/* regdomain is invalid?? how did original fw convert 
			* value to 0x82d4 ??
			* for now, force to 0x67 */
			p[i] &= 0xffff0000;
			p[i] |= 0x67;
		}
	}
}

static void __init
arv3527p_init(void)
{
	ltq_register_gpio_stp();
	//ltq_add_device_gpio_leds(arv3527p_gpio_leds, ARRAY_SIZE(arv3527p_gpio_leds));
	ltq_register_nor(&arv45xx_flash_data);
	arv45xx_register_ethernet();
}

MIPS_MACHINE(LANTIQ_MACH_ARV3527P,
			"ARV3527P",
			"ARV3527P - Arcor Easybox 401",
			arv3527p_init);

static void __init
arv4510pw_init(void)
{
	ltq_register_gpio_stp();
	ltq_add_device_gpio_leds(-1, ARRAY_SIZE(arv4510pw_gpio_leds), arv4510pw_gpio_leds);
	ltq_register_nor(&arv4510_flash_data);
	ltq_pci_data.irq[12] = (INT_NUM_IM2_IRL0 + 31);
	ltq_pci_data.irq[15] = (INT_NUM_IM0_IRL0 + 26);
	ltq_pci_data.gpio |= PCI_EXIN2 | PCI_REQ2;
	ltq_register_pci(&ltq_pci_data);
	bewan_register_ethernet();
}

MIPS_MACHINE(LANTIQ_MACH_ARV4510PW,
			"ARV4510PW",
			"ARV4510PW - Wippies Homebox",
			arv4510pw_init);

static void __init
arv4518pw_init(void)
{
#define ARV4518PW_EBU			0
#define ARV4518PW_USB			14
#define ARV4518PW_SWITCH_RESET		13
#define ARV4518PW_MADWIFI_ADDR		0xb07f0400

	ltq_register_gpio_ebu(ARV4518PW_EBU);
	ltq_add_device_gpio_leds(-1, ARRAY_SIZE(arv4518pw_gpio_leds), arv4518pw_gpio_leds);
	ltq_register_gpio_keys_polled(-1, LTQ_KEYS_POLL_INTERVAL,
				ARRAY_SIZE(arv4518pw_gpio_keys), arv4518pw_gpio_keys);
	ltq_register_nor(&arv45xx_flash_data);
	ltq_pci_data.gpio = PCI_GNT2 | PCI_REQ2;
	ltq_register_pci(&ltq_pci_data);
	ltq_register_madwifi_eep(ARV4518PW_MADWIFI_ADDR);
	ltq_register_ath5k(arv45xx_ath5k_eeprom_data, arv45xx_ath5k_eeprom_mac);
	xway_register_dwc(ARV4518PW_USB);
	arv45xx_register_ethernet();

	gpio_request(ARV4518PW_SWITCH_RESET, "switch");
	gpio_direction_output(ARV4518PW_SWITCH_RESET, 1);
	gpio_export(ARV4518PW_SWITCH_RESET, 0);
}

MIPS_MACHINE(LANTIQ_MACH_ARV4518PW,
			"ARV4518PW",
			"ARV4518PW - SMC7908A-ISP, Airties WAV-221",
			arv4518pw_init);

static void __init
arv4520pw_init(void)
{
#define ARV4520PW_EBU			0x400
#define ARV4520PW_USB			28
#define ARV4520PW_SWITCH_RESET		110

	ltq_register_gpio_ebu(ARV4520PW_EBU);
	ltq_add_device_gpio_leds(-1, ARRAY_SIZE(arv4520pw_gpio_leds), arv4520pw_gpio_leds);
	ltq_register_nor(&arv45xx_flash_data);
	ltq_register_pci(&ltq_pci_data);
	ltq_register_tapi();
	arv45xx_register_ethernet();
	xway_register_dwc(ARV4520PW_USB);

	gpio_request(ARV4520PW_SWITCH_RESET, "switch");
	gpio_set_value(ARV4520PW_SWITCH_RESET, 1);
}

MIPS_MACHINE(LANTIQ_MACH_ARV4520PW,
			"ARV4520PW",
			"ARV4520PW - Airties WAV-281, Arcor A800",
			arv4520pw_init);

static void __init
arv452Cpw_init(void)
{
#define ARV452CPW_EBU			0x77f
#define ARV452CPW_USB			28
#define ARV452CPW_RELAY1		31
#define ARV452CPW_RELAY2		107
#define ARV452CPW_SWITCH_RESET		110
#define ARV452CPW_MADWIFI_ADDR		0xb07f0400

	ltq_register_gpio_ebu(ARV452CPW_EBU);
	ltq_add_device_gpio_leds(-1, ARRAY_SIZE(arv452cpw_gpio_leds), arv452cpw_gpio_leds);
	ltq_register_nor(&arv45xx_flash_data);
	ltq_register_pci(&ltq_pci_data);
	ltq_register_madwifi_eep(ARV452CPW_MADWIFI_ADDR);
	xway_register_dwc(ARV452CPW_USB);
	arv45xx_register_ethernet();
	arv45xx_register_ath5k();

	gpio_request(ARV452CPW_SWITCH_RESET, "switch");
	gpio_set_value(ARV452CPW_SWITCH_RESET, 1);
	gpio_export(ARV452CPW_SWITCH_RESET, 0);

	gpio_request(ARV452CPW_RELAY1, "relay1");
	gpio_direction_output(ARV452CPW_RELAY1, 1);
	gpio_export(ARV452CPW_RELAY1, 0);

	gpio_request(ARV452CPW_RELAY2, "relay2");
	gpio_set_value(ARV452CPW_RELAY2, 1);
	gpio_export(ARV452CPW_RELAY2, 0);
}

MIPS_MACHINE(LANTIQ_MACH_ARV452CPW,
			"ARV452CPW",
			"ARV452CPW - Arcor A801",
			arv452Cpw_init);


static void __init
arv4525pw_init(void)
{
#define ARV4525PW_MADWIFI_ADDR		0xb07f0400
	ltq_add_device_gpio_leds(-1, ARRAY_SIZE(arv4525pw_gpio_leds), arv4525pw_gpio_leds);
	ltq_register_nor(&arv45xx_flash_data);
	ltq_pci_data.clock = PCI_CLOCK_INT;
	ltq_register_pci(&ltq_pci_data);
	ltq_register_madwifi_eep(ARV4525PW_MADWIFI_ADDR);
	ltq_eth_data.mii_mode = PHY_INTERFACE_MODE_MII;
	arv45xx_register_ethernet();
}

MIPS_MACHINE(LANTIQ_MACH_ARV4525PW,
			"ARV4525PW",
			"ARV4525PW - Speedport W502V",
			arv4525pw_init);

static void __init
arv7525pw_init(void)
{
	ltq_add_device_gpio_leds(-1, ARRAY_SIZE(arv4525pw_gpio_leds), arv4525pw_gpio_leds);
	ltq_register_nor(&arv7525_flash_data);
	ltq_register_gpio_keys_polled(-1, LTQ_KEYS_POLL_INTERVAL,
				ARRAY_SIZE(arv7525pw_gpio_keys), arv7525pw_gpio_keys);
	ltq_pci_data.clock = PCI_CLOCK_INT;
	ltq_pci_data.gpio = PCI_GNT1 | PCI_EXIN1;
	ltq_pci_data.irq[14] = (INT_NUM_IM3_IRL0 + 31);
	ltq_register_pci(&ltq_pci_data);
	ltq_eth_data.mii_mode = PHY_INTERFACE_MODE_MII;
	ltq_register_rt2x00("RT2860.eeprom");
	ltq_register_tapi();
	arv45xx_register_ethernet();
}

MIPS_MACHINE(LANTIQ_MACH_ARV7525PW,
			"ARV7525PW",
			"ARV7525PW - Speedport W303V",
			arv7525pw_init);

static void __init
arv7518pw_init(void)
{
#define ARV7518PW_EBU			0x2
#define ARV7518PW_USB			14

	ltq_register_gpio_ebu(ARV7518PW_EBU);
	ltq_add_device_gpio_leds(-1, ARRAY_SIZE(arv7518pw_gpio_leds), arv7518pw_gpio_leds);
	ltq_register_gpio_keys_polled(-1, LTQ_KEYS_POLL_INTERVAL,
				ARRAY_SIZE(arv7518pw_gpio_keys), arv7518pw_gpio_keys);
	ltq_register_nor(&arv75xx_flash_data);
	ltq_register_pci(&ltq_pci_data);
	ltq_register_tapi();
	xway_register_dwc(ARV7518PW_USB);
	arv75xx_register_ethernet();
	//arv7518_register_ath9k(mac);
}

MIPS_MACHINE(LANTIQ_MACH_ARV7518PW,
			"ARV7518PW",
			"ARV7518PW - ASTORIA",
			arv7518pw_init);

static void __init
arv752dpw22_init(void)
{
#define ARV752DPW22_EBU			0x2
#define ARV752DPW22_USB			100
#define ARV752DPW22_RELAY		101

	ltq_register_gpio_ebu(ARV752DPW22_EBU);
	ltq_add_device_gpio_leds(-1, ARRAY_SIZE(arv752dpw22_gpio_leds), arv752dpw22_gpio_leds);
	ltq_register_gpio_keys_polled(-1, LTQ_KEYS_POLL_INTERVAL,
				ARRAY_SIZE(arv752dpw22_gpio_keys), arv752dpw22_gpio_keys);
	ltq_register_nor(&arv75xx_flash_data);
	ltq_pci_data.irq[15] = (INT_NUM_IM3_IRL0 + 31);
	ltq_pci_data.gpio |= PCI_EXIN1 | PCI_REQ2;
	ltq_register_pci(&ltq_pci_data);
	xway_register_dwc(ARV752DPW22_USB);
	arv75xx_register_ethernet();

	gpio_request(ARV752DPW22_RELAY, "relay");
	gpio_set_value(ARV752DPW22_RELAY, 1);
	gpio_export(ARV752DPW22_RELAY, 0);
}

MIPS_MACHINE(LANTIQ_MACH_ARV752DPW22,
			"ARV752DPW22",
			"ARV752DPW22 - Arcor A803",
			arv752dpw22_init);
