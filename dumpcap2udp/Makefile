#
#

include $(TOPDIR)/rules.mk

PKG_NAME:=dumpcap2udp
PKG_VERSION:=0.1
PKG_RELEASE:=1
PKG_REV:=master

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=git://github.com/lynxis/dumpcap2udp.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/dumpcap2udp
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libpcap
  TITLE:=dumpcap2udp
  URL:=https://github.com/lynxis/dumpcap2udp
endef

define Package/dumpcap2udp/description
an utility that captures packages and send them over a udp socket
endef

define Package/dumpcap2udp/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dumpcap2udp $(1)/usr/bin
endef

$(eval $(call BuildPackage,dumpcap2udp))
