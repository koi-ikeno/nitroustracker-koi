#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/ds_rules

export TARGET		:=	$(shell basename $(CURDIR))
export TOPDIR		:=	$(CURDIR)

#---------------------------------------------------------------------------------
# path to tools - this can be deleted if you set the path in windows
#---------------------------------------------------------------------------------
export PATH		:=	$(DEVKITARM)/bin:$(PATH)

.PHONY: libdsmi libntxm tobkit tobkit-debug

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all: $(TARGET).nds $(TARGET).debug.nds

libntxm:
	@make -C libntxm/libntxm

tobkit:
	@make -C tobkit

tobkit-debug:
	@make -C tobkit DEBUG=true

libdsmi:
	@make -C dsmi/ds/libdsmi

#---------------------------------------------------------------------------------
$(TARGET).nds	:	libdsmi libntxm tobkit arm7/$(TARGET).elf arm9/$(TARGET).elf
	ndstool -c $(TARGET).nds -7 arm7/$(TARGET).arm7.elf -9 arm9/$(TARGET).arm9.elf -b icon.bmp "NitroTracker"

$(TARGET).debug.nds	:	libdsmi libntxm tobkit-debug arm7/$(TARGET).debug.elf arm9/$(TARGET).debug.elf
	ndstool -c $(TARGET).debug.nds -7 arm7/$(TARGET).arm7.debug.elf -9 arm9/$(TARGET).arm9.debug.elf -b icon.bmp "NitroTracker (debug)"

#---------------------------------------------------------------------------------
arm7/$(TARGET).debug.elf:
	$(MAKE) -C arm7 DEBUG=true

arm7/$(TARGET).elf:
	$(MAKE) -C arm7 DEBUG=false

#---------------------------------------------------------------------------------
arm9/$(TARGET).debug.elf:
	$(MAKE) -C arm9 DEBUG=true

arm9/$(TARGET).elf:
	$(MAKE) -C arm9 DEBUG=false

clean:
	$(MAKE) -C arm9 clean DEBUG=true
	$(MAKE) -C arm7 clean DEBUG=true
	$(MAKE) -C tobkit clean DEBUG=true
	$(MAKE) -C arm9 clean DEBUG=false
	$(MAKE) -C arm7 clean DEBUG=false
	$(MAKE) -C tobkit clean DEBUG=false
	rm -f $(TARGET).debug.nds $(TARGET).nds

# Custom targets for copying stuff to the DS
-include mytargets.mk

