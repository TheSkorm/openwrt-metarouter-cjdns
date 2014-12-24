/*
 *  Atheros DB120 reference board PCI initialization
 *
 *  Copyright (C) 2010-2011 Jaiganesh Narayanan <jnarayanan@atheros.com>
 *  Copyright (C) 2011 Gabor Juhos <juhosg@openwrt.org>
 *
 *  Parts of this file are based on Atheros linux 2.6.31 BSP
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef _AR71XX_DEV_DB120_PCI_H
#define _AR71XX_DEV_DB120_PCI_H

#if defined(CONFIG_AR71XX_DEV_DB120_PCI)
void db120_pci_init(u8 *cal_data, u8 *mac_addr);
#else
static inline void db120_pci_init(u8 *cal_data, u8 *mac_addr) { }
#endif

#endif /* _AR71XX_DEV_DB120_PCI_H */
