#---------------------------------------------------------------------------------

export ASMHACKS_DIR			:=	$(CURDIR)/asmhacks

ifeq ($(strip $(TARGET)),)
$(error "Please build this providing a target asmhack! Example: `make TARGET=hello-world.c` (do not forget the extension)")
endif

ifeq ($(shell test -e $(ASMHACKS_DIR)/$(TARGET) && echo yes),yes)
else
$(error The target $(TARGET) (file $(ASMHACKS_DIR)/$(TARGET)) does not exist. Please provide a valid target.)
endif

#---------------------------------------------------------------------------------

export BASE_ROM		:=	mkds-eu.nds
export OUT_ROM		:=	mkds-eu-asm.nds

# (Change these according to your setup)
export NCPATCHER	:=	../../../Software/NCPatcher/build/ncpatcher
export EDITWL_BIN	:=	../../editwl/editwl-bin/bin/editwl-bin

# (This must match the "filesystem" value in your "ncpatcher.json")
export FS_DIR		:=	gen-fs

# (This must match the "build" value in your "ncpatcher.json")
export NP_BUILD_DIR	:=	build

# Note: includes, sources, C/C++ flags and other compile settings are customized in the NCPatcher JSON files

#---------------------------------------------------------------------------------

export ASMHACK_SOURCE_DIR	:=	$(CURDIR)/asmhack-source

export BUILD_DIR			:=	$(CURDIR)/build-gen
export FULL_NP_BUILD_DIR	:=	$(CURDIR)/$(NP_BUILD_DIR)
export FULL_FS_DIR			:=	$(CURDIR)/$(FS_DIR)

export BASE_ROM_FILE		:=	$(CURDIR)/$(BASE_ROM)
export BASE_ARM7_BIN		:=	$(BUILD_DIR)/arm7.bin
export BASE_ARM9_BIN		:=	$(BUILD_DIR)/arm9.bin
export BASE_HEADER_BIN		:=	$(BUILD_DIR)/header.bin
export BASE_ARM7_OVT_BIN	:=	$(BUILD_DIR)/arm7ovt.bin
export BASE_ARM9_OVT_BIN	:=	$(BUILD_DIR)/arm9ovt.bin

export GEN_ARM7_BIN		:=	$(FULL_FS_DIR)/arm7.bin
export GEN_ARM9_BIN		:=	$(FULL_FS_DIR)/arm9.bin

export OUT_ROM_FILE		:=	$(CURDIR)/$(OUT_ROM)

#---------------------------------------------------------------------------------

.PHONY: all start gen

all: start $(BASE_ARM7_BIN) $(BASE_ARM9_BIN) $(BASE_HEADER_BIN) $(BASE_ARM7_OVT_BIN) $(BASE_ARM9_OVT_BIN) gen

start:
	@rm -rf $(BUILD_DIR)
	@rm -rf $(FULL_FS_DIR)
	@rm -rf $(FULL_NP_BUILD_DIR)
	@rm -rf $(OUT_ROM_FILE)
	@mkdir -p $(BUILD_DIR)

$(BASE_ARM7_BIN):
	@$(EDITWL_BIN) rom extract-code --rom=$(BASE_ROM_FILE) --proc=arm7 --out=$(BASE_ARM7_BIN)

$(BASE_ARM9_BIN):
	@$(EDITWL_BIN) rom extract-code --rom=$(BASE_ROM_FILE) --proc=arm9 --out=$(BASE_ARM9_BIN)

$(BASE_HEADER_BIN):
	@$(EDITWL_BIN) rom extract-header --rom=$(BASE_ROM_FILE) --out=$(BASE_HEADER_BIN)

$(BASE_ARM7_OVT_BIN):
	@$(EDITWL_BIN) rom extract-overlay-table --rom=$(BASE_ROM_FILE) --proc=arm7 --out=$(BASE_ARM7_OVT_BIN)

$(BASE_ARM9_OVT_BIN):
	@$(EDITWL_BIN) rom extract-overlay-table --rom=$(BASE_ROM_FILE) --proc=arm9 --out=$(BASE_ARM9_OVT_BIN)

gen:
	@mkdir -p $(FULL_FS_DIR)
	@cp $(BASE_HEADER_BIN) $(FULL_FS_DIR)/header.bin
	@cp $(BASE_ARM7_BIN) $(FULL_FS_DIR)/arm7.bin
	@cp $(BASE_ARM9_BIN) $(FULL_FS_DIR)/arm9.bin
	@cp $(BASE_ARM7_OVT_BIN) $(FULL_FS_DIR)/arm7ovt.bin
	@cp $(BASE_ARM9_OVT_BIN) $(FULL_FS_DIR)/arm9ovt.bin
	@rm -rf $(ASMHACK_SOURCE_DIR)
	@mkdir -p $(ASMHACK_SOURCE_DIR)
	@cp $(ASMHACKS_DIR)/$(TARGET) $(ASMHACK_SOURCE_DIR)/$(TARGET)
	@$(NCPATCHER)
	@rm -rf $(ASMHACK_SOURCE_DIR)
	@$(EDITWL_BIN) rom replace-codes --rom=$(BASE_ROM) --in7=$(GEN_ARM7_BIN) --in9=$(GEN_ARM9_BIN) --out=$(OUT_ROM_FILE)
