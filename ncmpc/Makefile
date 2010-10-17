#
#
#
include $(TOPDIR)/rules.mk

PKG_NAME:=ncmpc
PKG_VERSION:=0.11.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://hem.bredband.net/kaw/ncmpc/files/
PKG_MD5SUM:=c90668b12f3676c73913a863482ec405

include $(INCLUDE_DIR)/package.mk

define Package/ncmpc
  SECTION:=multimedia
  CATEGORY:=Multimedia
  TITLE:=ncmpc
  URL:=http://hem.bredband.net/kaw/ncmpc
  DEPENDS:=+gettext +libiconv
endef

define Package/ncmpc/description
mpd client with ncurses
endef

TARGET_LDFLAGS+=-L$(STAGING_DIR)/usr/lib/libintl/lib/
TARGET_LDFLAGS+=-L$(STAGING_DIR)/usr/lib/libiconv/lib/

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

define Package/ncmpc/install
	$(INSTALL_DIR) $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/ncmpc $(1)/usr/bin
	$(INSTALL_DIR) $(1)/usr/man/man1/
	$(CP) $(PKG_INSTALL_DIR)/usr/man/man1/ncmpc.1 $(1)/usr/man/man1
	$(INSTALL_DIR) $(1)/usr/share/doc
	$(CP) $(PKG_INSTALL_DIR)/usr/share/doc/ncmpc $(1)/usr/share/doc
endef

$(eval $(call BuildPackage,ncmpc))
