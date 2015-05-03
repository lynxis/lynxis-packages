# preinit-flasher
Try to flash an openwrt image in preinit stage.
Preinit is the stage before the rescue system shows up.

## Requirements
* gpg key pair
* webserver listening to fe80::08ef:2342

When using squashfs/jffs you need to put an gpg pub key into this package, because
the overlay isn't avaiable in preinit stage.

## How it works
The preinit-flasher try to download an image from
`http://fe80::08ef:2342/preinit/<mac>?board_name=nanostation-m-xw&release=${DISTRIB_RELEASE}&codename=${DISTRIB_CODENAME}&target=${DISTRIB_TARGET}`
verify against the embedded gpg key and if succeed flash it without(!) the previous configuration.

*NOTE* there is also a userspace tool `flash_gpg_signed <url> <preserve?>`.
* preserve will be checked agains 'no'. Unless no it will always preserve the configuration.
* url will given to busybox wget because only busybox supports IPv6 link local addreses.
