#ifndef _NTR_ARM9_BIOS_H
#define _NTR_ARM9_BIOS_H

#include <ntr/arm9/os/os_thread.h>
#include <ntr/arm9/mem/mem_addr.h>
#include <ntr/arm9/os/os_lock.h>

// Definitions for all the various fields set by the BIOS in the RAM

typedef enum Bios_BootIndicatorType {
    Bios_BootIndicatorType_Normal = 1,
    Bios_BootIndicatorType_DownloadPlay = 2
} Bios_BootIndicatorType;

// TODO: move this type to card headers when I get to them

typedef struct Bios_CardHeaderStart {
    char gameTitle[12];
    char gameCode[4];
    char makercode[2];
    u8 unitCode;
    u8 deviceType;
    u8 deviceSize;
    u8 reserved1[9];
    u8 romversion;
    u8 flags;
    u32 arm9romOffset;
    void *arm9executeAddress;
    void *arm9destination;
    u32 arm9binarySize;
    u32 arm7romOffset;
    void *arm7executeAddress;
    void *arm7destination;
    u32 arm7binarySize;
    u32 fntOffset;
    u32 fntSize;
    u32 fatOffset;
    u32 fatSize;
    u32 arm9overlaySource;
    u32 arm9overlaySize;
    u32 arm7overlaySource;
    u32 arm7overlaySize;
} Bios_CardHeaderStart;
_Static_assert(sizeof(Bios_CardHeaderStart) == 0x60);

typedef struct Bios_CardHeader {
    Bios_CardHeaderStart start_h;
    u32 cardControl13;
    u32 cardControlBF;
    u32 bannerOffset;
    u16 secureCRC16;
    u16 readTimeout;
    u32 unknownRAM1;
    u32 unknownRAM2;
    u32 bfPrime1;
    u32 bfPrime2;
    u32 romSize;
    u32 headerSize;
    u32 zeros88[14];
    u8 gbaLogo[156];
    u16 logoCRC16;
    u16 headerCRC16;
} Bios_CardHeader;
_Static_assert(sizeof(Bios_CardHeader) == 0x160);

typedef struct Bios_PersonalDataPacked {
    u16 language : 3;
    u16 gbaScreen : 1;
    u16 defaultBrightness : 2;
    u16 autoMode : 1;
    u16 RESERVED5 : 2;
    u16 settingsLost : 1;
    u16 RESERVED6 : 6;
} Bios_PersonalDataPacked;
_Static_assert(sizeof(Bios_PersonalDataPacked) == 0x2);

typedef struct Bios_PersonalData {
    u8 reserved[2];
    u8 theme;
    u8 birth_month;
    u8 birth_day;
    u8 unk;
    char16_t name[10];
    u16 name_len;
    u16 msg[26];
    u16 msg_len;
    u8 alarm_h;
    u8 alarm_min;
    u8 reserved_2[4];
    u16 calx1;
    u16 caly1;
    u8 calx1_px;
    u8 caly1_px;
    u16 calx2;
    u16 caly2;
    u8 calx2_px;
    u8 caly2_px;
    Bios_PersonalDataPacked data_packed;
    u8 reserved_3[2];
    u32 rtc_offset;
    u8 reserved_4[4];
} Bios_PersonalData;
_Static_assert(sizeof(Bios_PersonalData) == 0x70);

typedef struct Bios_SharedRegion {
    u32 nds_card_chip_id_1;
    u32 nds_card_chip_id_2;
    u16 nds_card_header_crc;
    u16 nds_card_secure_area_crc;
    u16 nds_card_crc_status;
    u16 nds_card_secure_area_crc_status;
    u16 boot_handler_task_no;
    u16 secure_disabled;
    u16 sio_debug_connection_exists;
    u16 unk_maybe_rtc_status;
    u8 random_rtc;
    u8 pad_x[55];
    u16 nds7_bios_crc;
    u8 unkunkunk[14];
    u32 unk_card_value;
    u32 wifi_flash_user_settings_status;
    u32 wifi_flash_user_settings_addr;
    u8 zero_y[8];
    u16 wifi_flash_fw_part5_crc16;
    u16 wifi_flash_fw_part3_part4_crc16;
    u8 zero_z[8];
    u32 nds9_nds7_msg;
    u32 nds7_boot_task;
    u8 zero_q[8];
    u32 unk_boot_flags;
    u8 unused_f[876];
    u32 nds_card_chip_id_1_copy;
    u32 nds_card_chip_id_2_copy;
    u16 nds_card_header_crc_copy;
    u16 nds_card_secure_area_crc_copy;
    u16 nds_card_crc_status_copy;
    u16 nds_card_secure_area_crc_status_copy;
    u16 nds7_bios_crc_copy;
    u16 secure_disabled_copy;
    u16 sio_debug_connection_exists_copy;
    u8 boot_check_info_2[10];
    u32 reset_parameter;
    u8 padding5[8];
    u32 rom_base_offset;
    u8 cartridge_module_info[12];
    u32 vblank_count;
    u16 boot_indicator;
    u8 dlplay_boot_info[60];
    u16 unused_s;
    Bios_PersonalData pdata;
    u8 zero_1[4];
    u8 mac_addr[6];
    u8 zero_2[6];
    u8 nvram_user_info[128];
    u8 isd_reserved1[32];
    void *mem_regions_start[9];
    void *mem_regions_end[9];
    u8 real_time_clock[8];
    u32 dma_clear_buf[4];
    Bios_CardHeader rom_header;
    u8 isd_reserved_2[32];
    u32 pxi_signal_param[2];
    u32 fifo_proc_handler_masks[2];
    u32 mic_last_address;
    u16 mic_sampling_data;
    u16 wm_callback_control;
    u16 wm_rssi_pool;
    u8 pad_3[2];
    u32 component_param;
    Os_ThreadInfo *arm9_thread_info;
    Os_ThreadInfo *arm7_thread_info;
    u16 buttons_xy;
    u8 touch_panel[4];
    u16 autoload_sync;
    u32 lock_id_flag_mainp[2];
    u32 lock_id_flag_subp[2];
    Os_Lock lock_VRAM_C;
    Os_Lock lock_VRAM_D;
    Os_Lock lock_WRAM_block_0;
    Os_Lock lock_WRAM_block_1;
    Os_Lock lock_card;
    Os_Lock unk_lock_cart;
    Os_Lock lock_init;
    u16 nds9_swi_is_debugger_check_scratch_addr;
    u16 nds7_swi_is_debugger_check_scratch_addr;
    u8 pad_4[2];
    u16 command_area;
} Bios_SharedRegion;
_Static_assert(sizeof(Bios_SharedRegion) == 0x800);

#define NTR_ARM9_BIOS_SHARED_REGION ((volatile Bios_SharedRegion*)(0x027FF800))

#endif
