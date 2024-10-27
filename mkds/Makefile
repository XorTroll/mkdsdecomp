#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

export TARGET	:=	mkds
export TOPDIR	:=	$(CURDIR)

export LIBNTR	:=	$(TOPDIR)/../libntr

NITROFS_DIR	:=	nitrofs

GAME_TITLE     := Mario Kart DS
GAME_SUBTITLE1 := Decompilation
GAME_SUBTITLE2 := By XorTroll

include $(DEVKITARM)/ds_rules

.PHONY: all clean lib

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all: lib $(TARGET).nds

#---------------------------------------------------------------------------------
$(TARGET).nds	: $(NITROFS_DIR) arm7/$(TARGET).elf arm9/$(TARGET).elf
	@ndstool	-c $(TARGET).nds -7 arm7/$(TARGET).elf -9 arm9/$(TARGET).elf \
	-b $(GAME_ICON) "$(GAME_TITLE);$(GAME_SUBTITLE1);$(GAME_SUBTITLE2)" \
	$(_ADDFILES)

lib:
	$(MAKE) -C $(LIBNTR)

#---------------------------------------------------------------------------------
arm7/$(TARGET).elf:
	
	$(MAKE) -C arm7
	
#---------------------------------------------------------------------------------
arm9/$(TARGET).elf:
	$(MAKE) -C arm9

#---------------------------------------------------------------------------------
clean:
	$(MAKE) -C $(LIBNTR) clean
	$(MAKE) -C arm9 clean
	$(MAKE) -C arm7 clean
	rm -f $(TARGET).nds $(TARGET).arm7 $(TARGET).arm9
