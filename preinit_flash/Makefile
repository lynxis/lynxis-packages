#
# Copyright (C) 2013-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=lazus-preinit-flasher
PKG_VERSION:=2014-01-12
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_MAINTAINER:=Jo-Philipp Wich <jow@openwrt.org>

include $(INCLUDE_DIR)/package.mk

define Package/lazus-preinit-flasher
  SECTION:=utils
  CATEGORY:=Base system
  TITLE:=Lazus Preinit Flasher
  DEPENDS:=+gnupg
endef

define Package/lazus-preinit-flasher/description
 It flashs the router if the ip is reachable
endef

define Build/Compile
endef

define Package/lazus-preinit-flasher/install
	$(INSTALL_DIR) $(1)/lib/preinit
	$(INSTALL_DATA) ./files/hook $(1)/lib/preinit/23_lazus_preinit_flasher
	$(INSTALL_DIR) $(1)/usr/lib/lazus-preinit-flasher
	$(INSTALL_DATA) ./files/image.asc $(1)/usr/lib/lazus-preinit-flasher/image.asc
endef

$(eval $(call BuildPackage,lazus-preinit-flasher))
