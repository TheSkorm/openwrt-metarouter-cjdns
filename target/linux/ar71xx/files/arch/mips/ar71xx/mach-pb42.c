/*
 *  Atheros PB42 board support
 *
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <asm/mach-ar71xx/ar71xx.h>

#include "machtype.h"
#include "devices.h"
#include "dev-m25p80.h"
#include "dev-gpio-buttons.h"
#include "dev-pb42-pci.h"
#include "dev-usb.h"

#define PB42_KEYS_POLL_INTERVAL		20	/* msecs */
#define PB42_KEYS_DEBOUNCE_INTERVAL	(3 * PB42_KEYS_POLL_INTERVAL)

#define PB42_GPIO_BTN_SW4	8
#define PB42_GPIO_BTN_SW5	3

static struct gpio_keys_button pb42_gpio_keys[] __initdata = {
	{
		.desc		= "sw4",
		.type		= EV_KEY,
		.code		= BTN_0,
		.debounce_interval = PB42_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= PB42_GPIO_BTN_SW4,
		.active_low	= 1,
	}, {
		.desc		= "sw5",
		.type		= EV_KEY,
		.code		= BTN_1,
		.debounce_interval = PB42_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= PB42_GPIO_BTN_SW5,
		.active_low	= 1,
	}
};

static const char *pb42_part_probes[] = {
	"RedBoot",
	NULL,
};

static struct flash_platform_data pb42_flash_data = {
	.part_probes	= pb42_part_probes,
};

#define PB42_WAN_PHYMASK	BIT(20)
#define PB42_LAN_PHYMASK	(BIT(16) | BIT(17) | BIT(18) | BIT(19))
#define PB42_MDIO_PHYMASK	(PB42_LAN_PHYMASK | PB42_WAN_PHYMASK)

static void __init pb42_init(void)
{
	ar71xx_add_device_m25p80(&pb42_flash_data);

	ar71xx_add_device_mdio(0, ~PB42_MDIO_PHYMASK);

	ar71xx_init_mac(ar71xx_eth0_data.mac_addr, ar71xx_mac_base, 0);
	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ar71xx_eth0_data.phy_mask = PB42_WAN_PHYMASK;

	ar71xx_init_mac(ar71xx_eth1_data.mac_addr, ar71xx_mac_base, 1);
	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth1_data.speed = SPEED_100;
	ar71xx_eth1_data.duplex = DUPLEX_FULL;

	ar71xx_add_device_eth(0);
	ar71xx_add_device_eth(1);

	ar71xx_register_gpio_keys_polled(-1, PB42_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(pb42_gpio_keys),
					 pb42_gpio_keys);

	pb42_pci_init();
}

MIPS_MACHINE(AR71XX_MACH_PB42, "PB42", "Atheros PB42", pb42_init);
