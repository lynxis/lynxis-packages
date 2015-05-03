#
# Copyright (C) 2013-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=preinit-flasher
PKG_VERSION:=1.0
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_MAINTAINER:=Alexander Couzens <lynxis@fe80.eu>

include $(INCLUDE_DIR)/package.mk

define Package/preinit-flasher
  SECTION:=utils
  CATEGORY:=Base system
  TITLE:=Preinit Flasher
  DEPENDS:=+gnupg
endef

define Package/preinit-flasher/description
 Flash images from pre_init stage. Verify images by an gpg key.
endef

define Build/Compile
endef

define Package/preinit-flasher/install
	$(INSTALL_DIR) $(1)/lib/preinit
	$(INSTALL_DATA) ./files/hook $(1)/lib/preinit/45_preinit_flasher
	$(INSTALL_DIR) $(1)/usr/lib/preinit-flasher $(1)/usr/bin/
	$(INSTALL_DATA) ./files/image.asc $(1)/usr/lib/preinit-flasher/image.asc
	$(INSTALL_DATA) ./files/lib $(1)/usr/lib/preinit-flasher/lib
	$(INSTALL_BIN) ./files/flash_gpg_signed $(1)/usr/bin/flash_gpg_signed
endef

$(eval $(call BuildPackage,preinit-flasher))
