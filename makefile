KEXT=IOElectrify.kext
DIST=darkvoid-IOElectrify
BUILDDIR=./Build/Products

VERSION_ERA=$(shell ./print_version.sh)
ifeq "$(VERSION_ERA)" "10.10-"
	INSTDIR=/System/Library/Extensions
else
	INSTDIR=/Library/Extensions
endif

ifeq ($(findstring 32,$(BITS)),32)
OPTIONS:=$(OPTIONS) -arch i386
endif

ifeq ($(findstring 64,$(BITS)),64)
OPTIONS:=$(OPTIONS) -arch x86_64
endif

OPTIONS:=$(OPTIONS) -scheme IOElectrify

.PHONY: all
all:
	xcodebuild build $(OPTIONS) -configuration Debug
	xcodebuild build $(OPTIONS) -configuration Release

.PHONY: clean
clean:
	xcodebuild clean $(OPTIONS) -configuration Debug
	xcodebuild clean $(OPTIONS) -configuration Release

.PHONY: update_kernelcache
update_kernelcache:
	sudo touch /System/Library/Extensions
	sudo kextcache -update-volume /

.PHONY: test
test:
	if kextstat | grep --quiet IOElectrify; then sudo kextunload /tmp/$(KEXT); fi
	sudo rm -Rf /tmp/$(KEXT)
	sudo cp -R $(BUILDDIR)/Debug/$(KEXT) /tmp
	if [ "`which tag`" != "" ]; then sudo tag -a Purple /tmp/$(KEXT); fi
	sudo kextload /tmp/$(KEXT)

.PHONY: install_debug
install_debug:
	sudo rm -Rf $(INSTDIR)/$(KEXT)
	sudo cp -R $(BUILDDIR)/Debug/$(KEXT) $(INSTDIR)
	if [ "`which tag`" != "" ]; then sudo tag -a Purple $(INSTDIR)/$(KEXT); fi
	make update_kernelcache

.PHONY: install
install:
	sudo rm -Rf $(INSTDIR)/$(KEXT)
	sudo cp -R $(BUILDDIR)/Release/$(KEXT) $(INSTDIR)
	if [ "`which tag`" != "" ]; then sudo tag -a Blue $(INSTDIR)/$(KEXT); fi
	make update_kernelcache

.PHONY: distribute
distribute:
	if [ -e ./Distribute ]; then rm -r ./Distribute; fi
	mkdir ./Distribute
	cp -R $(BUILDDIR)/Debug ./Distribute
	cp -R $(BUILDDIR)/Release ./Distribute
	find ./Distribute -path *.DS_Store -delete
	ditto -c -k --sequesterRsrc --zlibCompressionLevel 9 ./Distribute ./Archive.zip
	mv ./Archive.zip ./Distribute/`date +$(DIST)-%Y-%m%d.zip`
