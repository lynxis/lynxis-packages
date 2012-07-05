#
#
#
include $(TOPDIR)/rules.mk

PKG_NAME:=libartnet
PKG_VERSION:=0.1
PKG_RELEASE:=1
PKG_REV:=c54003450c9923421e2700e25ead6ffc37a8d4f1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=git://dev.c-base.org/libart-dmxcontrol/libartnet.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)


include $(INCLUDE_DIR)/package.mk

TARGET_CFLAGS += $(FPIC)

define Package/libartnet
  SECTION:=multimedia
  CATEGORY:=Multimedia
  TITLE:=lib Art-Net
  URL:=http://lynxis.crew.c-base.org/
  DEPENDS:=+libpthread
endef

define Package/libartnet/description
libartnet is a multimedia library which using dmx.
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	( \
		cd $(PKG_BUILD_DIR) ; \
		libtoolize --copy --force ; \
		aclocal ; \
		automake --foreign --add-missing --copy ; \
		autoconf ; \
	)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/artnet  $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libartnet*  $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libartnet.pc $(1)/usr/lib/pkgconfig
endef

define Package/libartnet/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/* $(1)/usr/lib
endef

$(eval $(call BuildPackage,libartnet))
