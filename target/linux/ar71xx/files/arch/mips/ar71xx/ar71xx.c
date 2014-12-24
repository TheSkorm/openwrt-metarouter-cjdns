/*
 *  AR71xx SoC routines
 *
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>

#include <asm/mach-ar71xx/ar71xx.h>

static DEFINE_MUTEX(ar71xx_flash_mutex);
static DEFINE_SPINLOCK(ar71xx_device_lock);

void __iomem *ar71xx_ddr_base;
EXPORT_SYMBOL_GPL(ar71xx_ddr_base);

void __iomem *ar71xx_pll_base;
EXPORT_SYMBOL_GPL(ar71xx_pll_base);

void __iomem *ar71xx_reset_base;
EXPORT_SYMBOL_GPL(ar71xx_reset_base);

void __iomem *ar71xx_gpio_base;
EXPORT_SYMBOL_GPL(ar71xx_gpio_base);

void __iomem *ar71xx_usb_ctrl_base;
EXPORT_SYMBOL_GPL(ar71xx_usb_ctrl_base);

void ar71xx_device_stop(u32 mask)
{
	unsigned long flags;
	u32 mask_inv;
	u32 t;

	switch (ar71xx_soc) {
	case AR71XX_SOC_AR7130:
	case AR71XX_SOC_AR7141:
	case AR71XX_SOC_AR7161:
		spin_lock_irqsave(&ar71xx_device_lock, flags);
		t = ar71xx_reset_rr(AR71XX_RESET_REG_RESET_MODULE);
		ar71xx_reset_wr(AR71XX_RESET_REG_RESET_MODULE, t | mask);
		spin_unlock_irqrestore(&ar71xx_device_lock, flags);
		break;

	case AR71XX_SOC_AR7240:
	case AR71XX_SOC_AR7241:
	case AR71XX_SOC_AR7242:
		mask_inv = mask & RESET_MODULE_USB_OHCI_DLL_7240;
		spin_lock_irqsave(&ar71xx_device_lock, flags);
		t = ar71xx_reset_rr(AR724X_RESET_REG_RESET_MODULE);
		t |= mask;
		t &= ~mask_inv;
		ar71xx_reset_wr(AR724X_RESET_REG_RESET_MODULE, t);
		spin_unlock_irqrestore(&ar71xx_device_lock, flags);
		break;

	case AR71XX_SOC_AR9130:
	case AR71XX_SOC_AR9132:
		spin_lock_irqsave(&ar71xx_device_lock, flags);
		t = ar71xx_reset_rr(AR91XX_RESET_REG_RESET_MODULE);
		ar71xx_reset_wr(AR91XX_RESET_REG_RESET_MODULE, t | mask);
		spin_unlock_irqrestore(&ar71xx_device_lock, flags);
		break;

	case AR71XX_SOC_AR9330:
	case AR71XX_SOC_AR9331:
		spin_lock_irqsave(&ar71xx_device_lock, flags);
		t = ar71xx_reset_rr(AR933X_RESET_REG_RESET_MODULE);
		ar71xx_reset_wr(AR933X_RESET_REG_RESET_MODULE, t | mask);
		spin_unlock_irqrestore(&ar71xx_device_lock, flags);
		break;

	case AR71XX_SOC_AR9341:
	case AR71XX_SOC_AR9342:
	case AR71XX_SOC_AR9344:
		spin_lock_irqsave(&ar71xx_device_lock, flags);
		t = ar71xx_reset_rr(AR934X_RESET_REG_RESET_MODULE);
		ar71xx_reset_wr(AR934X_RESET_REG_RESET_MODULE, t | mask);
		spin_unlock_irqrestore(&ar71xx_device_lock, flags);
		break;

	default:
		BUG();
	}
}
EXPORT_SYMBOL_GPL(ar71xx_device_stop);

void ar71xx_device_start(u32 mask)
{
	unsigned long flags;
	u32 mask_inv;
	u32 t;

	switch (ar71xx_soc) {
	case AR71XX_SOC_AR7130:
	case AR71XX_SOC_AR7141:
	case AR71XX_SOC_AR7161:
		spin_lock_irqsave(&ar71xx_device_lock, flags);
		t = ar71xx_reset_rr(AR71XX_RESET_REG_RESET_MODULE);
		ar71xx_reset_wr(AR71XX_RESET_REG_RESET_MODULE, t & ~mask);
		spin_unlock_irqrestore(&ar71xx_device_lock, flags);
		break;

	case AR71XX_SOC_AR7240:
	case AR71XX_SOC_AR7241:
	case AR71XX_SOC_AR7242:
		mask_inv = mask & RESET_MODULE_USB_OHCI_DLL_7240;
		spin_lock_irqsave(&ar71xx_device_lock, flags);
		t = ar71xx_reset_rr(AR724X_RESET_REG_RESET_MODULE);
		t &= ~mask;
		t |= mask_inv;
		ar71xx_reset_wr(AR724X_RESET_REG_RESET_MODULE, t);
		spin_unlock_irqrestore(&ar71xx_device_lock, flags);
		break;

	case AR71XX_SOC_AR9130:
	case AR71XX_SOC_AR9132:
		spin_lock_irqsave(&ar71xx_device_lock, flags);
		t = ar71xx_reset_rr(AR91XX_RESET_REG_RESET_MODULE);
		ar71xx_reset_wr(AR91XX_RESET_REG_RESET_MODULE, t & ~mask);
		spin_unlock_irqrestore(&ar71xx_device_lock, flags);
		break;

	case AR71XX_SOC_AR9330:
	case AR71XX_SOC_AR9331:
		spin_lock_irqsave(&ar71xx_device_lock, flags);
		t = ar71xx_reset_rr(AR933X_RESET_REG_RESET_MODULE);
		ar71xx_reset_wr(AR933X_RESET_REG_RESET_MODULE, t & ~mask);
		spin_unlock_irqrestore(&ar71xx_device_lock, flags);
		break;

	case AR71XX_SOC_AR9341:
	case AR71XX_SOC_AR9342:
	case AR71XX_SOC_AR9344:
		spin_lock_irqsave(&ar71xx_device_lock, flags);
		t = ar71xx_reset_rr(AR934X_RESET_REG_RESET_MODULE);
		ar71xx_reset_wr(AR934X_RESET_REG_RESET_MODULE, t & ~mask);
		spin_unlock_irqrestore(&ar71xx_device_lock, flags);
		break;

	default:
		BUG();
	}
}
EXPORT_SYMBOL_GPL(ar71xx_device_start);

void ar71xx_device_reset_rmw(u32 clear, u32 set)
{
	unsigned long flags;
	unsigned int reg;
	u32 t;

	switch (ar71xx_soc) {
	case AR71XX_SOC_AR7130:
	case AR71XX_SOC_AR7141:
	case AR71XX_SOC_AR7161:
		reg = AR71XX_RESET_REG_RESET_MODULE;
		break;

	case AR71XX_SOC_AR7240:
	case AR71XX_SOC_AR7241:
	case AR71XX_SOC_AR7242:
		reg = AR724X_RESET_REG_RESET_MODULE;
		break;

	case AR71XX_SOC_AR9130:
	case AR71XX_SOC_AR9132:
		reg = AR91XX_RESET_REG_RESET_MODULE;
		break;

	case AR71XX_SOC_AR9330:
	case AR71XX_SOC_AR9331:
		reg = AR933X_RESET_REG_RESET_MODULE;
		break;

	case AR71XX_SOC_AR9341:
	case AR71XX_SOC_AR9342:
	case AR71XX_SOC_AR9344:
		reg = AR934X_RESET_REG_RESET_MODULE;
		break;

	default:
		BUG();
	}

	spin_lock_irqsave(&ar71xx_device_lock, flags);
	t = ar71xx_reset_rr(reg);
	t &= ~clear;
	t |= set;
	ar71xx_reset_wr(reg, t);
	spin_unlock_irqrestore(&ar71xx_device_lock, flags);
}
EXPORT_SYMBOL_GPL(ar71xx_device_reset_rmw);

int ar71xx_device_stopped(u32 mask)
{
	unsigned long flags;
	u32 t;

	switch (ar71xx_soc) {
	case AR71XX_SOC_AR7130:
	case AR71XX_SOC_AR7141:
	case AR71XX_SOC_AR7161:
		spin_lock_irqsave(&ar71xx_device_lock, flags);
		t = ar71xx_reset_rr(AR71XX_RESET_REG_RESET_MODULE);
		spin_unlock_irqrestore(&ar71xx_device_lock, flags);
		break;

	case AR71XX_SOC_AR7240:
	case AR71XX_SOC_AR7241:
	case AR71XX_SOC_AR7242:
		spin_lock_irqsave(&ar71xx_device_lock, flags);
		t = ar71xx_reset_rr(AR724X_RESET_REG_RESET_MODULE);
		spin_unlock_irqrestore(&ar71xx_device_lock, flags);
		break;

	case AR71XX_SOC_AR9130:
	case AR71XX_SOC_AR9132:
		spin_lock_irqsave(&ar71xx_device_lock, flags);
		t = ar71xx_reset_rr(AR91XX_RESET_REG_RESET_MODULE);
		spin_unlock_irqrestore(&ar71xx_device_lock, flags);
		break;

	case AR71XX_SOC_AR9330:
	case AR71XX_SOC_AR9331:
		spin_lock_irqsave(&ar71xx_device_lock, flags);
		t = ar71xx_reset_rr(AR933X_RESET_REG_RESET_MODULE);
		spin_unlock_irqrestore(&ar71xx_device_lock, flags);
		break;

	case AR71XX_SOC_AR9341:
	case AR71XX_SOC_AR9342:
	case AR71XX_SOC_AR9344:
		spin_lock_irqsave(&ar71xx_device_lock, flags);
		t = ar71xx_reset_rr(AR934X_RESET_REG_RESET_MODULE);
		spin_unlock_irqrestore(&ar71xx_device_lock, flags);
		break;

	default:
		BUG();
	}

	return ((t & mask) == mask);
}
EXPORT_SYMBOL_GPL(ar71xx_device_stopped);

void ar71xx_ddr_flush(u32 reg)
{
	ar71xx_ddr_wr(reg, 1);
	while ((ar71xx_ddr_rr(reg) & 0x1))
		;

	ar71xx_ddr_wr(reg, 1);
	while ((ar71xx_ddr_rr(reg) & 0x1))
		;
}
EXPORT_SYMBOL_GPL(ar71xx_ddr_flush);

void ar71xx_flash_acquire(void)
{
	mutex_lock(&ar71xx_flash_mutex);
}
EXPORT_SYMBOL_GPL(ar71xx_flash_acquire);

void ar71xx_flash_release(void)
{
	mutex_unlock(&ar71xx_flash_mutex);
}
EXPORT_SYMBOL_GPL(ar71xx_flash_release);
