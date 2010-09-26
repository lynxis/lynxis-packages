#
#

include $(TOPDIR)/rules.mk

PKG_NAME:=artnet-examples
PKG_VERSION:=0.1
PKG_RELEASE:=1
PKG_REV:=557ffa3f99c225189895b65dc1bd285ad87f2f27

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=git://dev.c-base.org/libart-dmxcontrol/artnet-examples.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/artnet-examples
  SECTION:=multimedia
  CATEGORY:=Multimedia
  DEPENDS:=libartnet
  TITLE:=artnet-examples
endef

define Package/artnet-examples/description
example application for Art-Net. includes enntec usb gateway
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	( \
		cd $(PKG_BUILD_DIR) ; \
		libtoolize --copy --force ; \
		aclocal ; \
		automake --foreign --add-missing --copy ; \
		autoconf  \
	)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
endef

define Package/artnet-examples/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/artnet* $(1)/usr/bin
endef

$(eval $(call BuildPackage,artnet-examples))
