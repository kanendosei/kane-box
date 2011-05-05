#
# newLisp Makefile for OpenWRT "backfire 10.3" release
#
# Created by kane-box.com
#

include $(TOPDIR)/rules.mk

#
# These lines concatanate the package name and list the URL location
# where the source code is downloaded
#
PKG_NAME:=newlisp
PKG_VERSION:=10.2.18
#PKG_VERSION:=10.3.0
PKG_RELEASE:=1
PKG_FLAGS:=essential

#PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tgz

#PKG_SOURCE_URL:=http://www.newlisp.org/downloads/development/
PKG_SOURCE_URL:=http://www.kane-box.com/newlisp/

#
# md5 can be computed using the md5sum utility available on linux
#
#PKG_MD5SUM:=6a5a901512c293d0fa7756f80f5661cc # original
# 10.2.18
PKG_MD5SUM:=24d5a274c36f800056945b4093b38bfc
# 10.3.0 
#PKG_MD5SUM:=e97b1000b3e4ca76342381d8f0dfd960
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/newlisp
  SECTION:=base
  CATEGORY:=Base system
  TITLE:=newLisp - the best Lisp ever.
  URL:=http://www.newlisp.org
  MENU:=1
endef

define Package/newlisp/description
  newLisp is the best, smallest, most powerful
  version of Lisp ever created.
  And, it runs kane|box.
endef

#define Package/newlisp/config
#	source "$(SOURCE)/Config.in"
#endef

define Build/Configure
	rm -f $(PKG_BUILD_DIR)/.configured*
	grep 'CONFIG_NEWLISP_' $(TOPDIR)/.config | sed -e "s,\\(# \)\\?CONFIG_NEWLISP_\\(.*\\),\\1\\2,g" > $(PKG_BUILD_DIR)/.config
	yes '' | $(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		KBUILD_HAVE_NLS=no \
		ARCH="$(ARCH)"
endef

ifdef CONFIG_GCC_VERSION_LLVM
  TARGET_CFLAGS += -fnested-functions
endif

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		ARCH="$(ARCH)" \
		SKIP_STRIP=n \
		all
	#rm -rf $(PKG_INSTALL_DIR)
	$(FIND) $(PKG_BUILD_DIR) -lname "*newlisp" -exec rm \{\} \;
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		ARCH="$(ARCH)" #\
		#CONFIG_PREFIX="$(PKG_INSTALL_DIR)" 
endef

define Package/newlisp/install
        $(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/newlisp $(1)/usr/bin
endef

$(eval $(call BuildPackage,newlisp))
