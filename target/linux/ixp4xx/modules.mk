#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define KernelPackage/ata-ixp4xx-cf
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=IXP4XX Compact Flash support
  DEPENDS:=@TARGET_ixp4xx
  KCONFIG:=CONFIG_PATA_IXP4XX_CF
  FILES:=$(LINUX_DIR)/drivers/ata/pata_ixp4xx_cf.ko
  AUTOLOAD:=$(call AutoLoad,41,pata_ixp4xx_cf,1)
  $(call AddDepends/ata)
endef

define KernelPackage/ata-ixp4xx-cf/description
 IXP4XX Compact Flash support.
endef

$(eval $(call KernelPackage,ata-ixp4xx-cf))


define KernelPackage/ixp4xx-beeper
  SUBMENU:=$(OTHER_MENU)
  TITLE:=IXP4XX Beeper support
  DEPENDS:=@TARGET_ixp4xx
  KCONFIG:= \
	CONFIG_INPUT_MISC=y \
	CONFIG_INPUT_IXP4XX_BEEPER
  FILES:=$(LINUX_DIR)/drivers/input/misc/ixp4xx-beeper.ko
  AUTOLOAD:=$(call AutoLoad,50,ixp4xx-beeper)
  $(call AddDepends/input)
endef

define KernelPackage/ixp4xx-beeper/description
 IXP4XX Beeper support
endef

$(eval $(call KernelPackage,ixp4xx-beeper))
