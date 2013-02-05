#
# Copyright (C) 2008-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=meteolinger
PKG_VERSION:=3.3
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/meteolinger
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libusb-compat +libc
  TITLE:=meteolinger wwsr
  URL:=http://meteolinger.lowlevel.cz/
endef

define Package/meteolinger/description
	wwsr from the meteolinger package is used to grab
	data from a weatherstation via usb
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)/
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	cd $(PKG_BUILD_DIR) ; \
		$(TARGET_CC) $(CFLAGS) $(EXTRA_CFLAGS) -o wwsr3.3 wwsr3.3.c -lusb -lm
endef

define Package/meteolinger/install
	$(INSTALL_DIR) $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wwsr3.3 $(1)/usr/bin/
endef

$(eval $(call BuildPackage,meteolinger))
