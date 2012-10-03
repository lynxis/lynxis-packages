#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=jack
PKG_VERSION:=1.9.8
PKG_SOURCE:=jack-$(PKG_VERSION).tgz
PKG_SOURCE_URL:=http://www.grame.fr/~letz/
PKG_BUILD_DIR:=$(BUILD_DIR)/jack-$(PKG_VERSION)

PKG_RELEASE:=1
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/jack
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=jack
  URL:=http://jackaudio.org/
  DEPENDS:=+alsa-lib +libstdcpp +libcelt +libsamplerate
endef

define Package/jack/description
  JackD audio server
endef

define Build/Compile
	cd $(PKG_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION) && \
    echo $(TARGET_CXX) && \
	DESTDIR="$(PKG_INSTALL_DIR)" $(MAKE_VARS) \
	DEST_PLATFORM=mips32 \
	CC=$(TARGET_CC) \
	CPP=$(TARGET_CC) \
	CXX=$(TARGET_CXX) \
	AR=$(TARGET_AR) \
	NM=$(TARGET_NM) \
	RANLIB=$(TARGET_RANLIB) \
	STRIP=$(TARGET_STRIP) \
    PREFIX=/usr/ \
	./waf --destdir=$(PKG_INSTALL_DIR) configure --alsa --ports-per-application=4 --clients=4 --classic && \
	./waf build && \
	./waf --destdir=$(PKG_INSTALL_DIR) install
endef

define Build/Install
endef

define Package/jack/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/* $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/* $(1)/usr/lib
endef
$(eval $(call BuildPackage,jack))
