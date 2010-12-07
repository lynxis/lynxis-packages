#
#
#
include $(TOPDIR)/rules.mk

PKG_NAME:=libmpdclient
PKG_VERSION:=2.3
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/musicpd

include $(INCLUDE_DIR)/package.mk

# TARGET_CFLAGS += $(FPIC)

define Package/libmpdclient
  SECTION:=multimedia
  CATEGORY:=Multimedia
  TITLE:=lib
endef

define Package/libmpdclient/description
a client library for mpd
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/include/mpd $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig/
	$(CP) $(PKG_BUILD_DIR)/libmpdclient.pc $(1)/usr/lib/pkgconfig
endef

define Package/musicpd/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/src/.libs/libmpdclient.* $(1)/usr/lib
endef

$(eval $(call BuildPackage,libmpdclient))
