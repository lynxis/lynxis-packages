#
#

include $(TOPDIR)/rules.mk

PKG_NAME:=artnet-usbnode
PKG_VERSION:=0.1
PKG_RELEASE:=1
PKG_REV:=110cfe95a65826b1f21b1814fd70a634c970abcb

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=git://dev.c-base.org/libart-dmxcontrol/usbnode.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/artnet-usbnode
  SECTION:=multimedia
  CATEGORY:=Multimedia
  DEPENDS:=libartnet
  TITLE:=artnet-usbnode
endef

define Package/artnet-usbnode/description
an artnet-to-dmx-gateway based on enttec usbdmx-device
endef

define Package/artnet-usbnode/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/usbnode $(1)/usr/bin
endef

$(eval $(call BuildPackage,artnet-usbnode))
