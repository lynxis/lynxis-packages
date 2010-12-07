include $(TOPDIR)/rules.mk

PKG_NAME:=kexec_dualboot
PKG_VERSION:=0.1
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/kexec_dualboot
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Kexec script to boot a second image
  DEPENDS:=@LINUX_2_6
  URL:=http://www.c-base.org
endef

define Package/kexec_dualboot/description
  kexec external image
endef

define Build/Compile
endef

define Package/kexec_dualboot/install
	$(INSTALL_DIR) $(1)/sbin/ $(1)/etc/init.d/
	$(INSTALL_BIN) ./files/kexec_dualboot.sh $(1)/sbin/
	$(INSTALL_BIN) ./files/kexec $(1)/etc/init.d/
endef

$(eval $(call BuildPackage,kexec_dualboot))
