/*
 *  Atheros AP94 reference board PCI initialization
 *
 *  Copyright (C) 2009-2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef _AR71XX_DEV_AP94_PCI_H
#define _AR71XX_DEV_AP94_PCI_H

#if defined(CONFIG_AR71XX_DEV_AP94_PCI)
void ap94_pci_init(u8 *cal_data0, u8 *mac_addr0,
		   u8 *cal_data1, u8 *mac_addr1) __init;

void ap94_pci_setup_wmac_led_pin(unsigned wmac, int pin) __init;
void ap94_pci_setup_wmac_gpio(unsigned wmac, u32 mask, u32 val) __init;

#else
static inline void ap94_pci_init(u8 *cal_data0, u8 *mac_addr0,
				 u8 *cal_data1, u8 *mac_addr1) {}

static inline void ap94_pci_setup_wmac_led_pin(unsigned wmac, int pin) {}
static inline void ap94_pci_setup_wmac_gpio(unsigned wmac,
					    u32 mask, u32 val) {}
#endif

#endif /* _AR71XX_DEV_AP94_PCI_H */

