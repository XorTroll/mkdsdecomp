#include <save.hpp>
#include <save_enc.hpp>

#define READ_DECRYPT_SECTION(ptr, fmt, type) { \
    if(!ReadDecryptSection(ptr, &fmt, EditableSaveDataSectionOffsets[static_cast<u32>(type)], sizeof(fmt), SkipFirst8BytesFromDecrypt(type), decltype(fmt)::Magic)) { \
        return false; \
    } \
}

#define VERIFY_SECTION_CRC(fmt, type) { \
    /* Get current CRC value, last (BE) u16 of the savedata section */ \
    auto crc_ptr = reinterpret_cast<u32*>(&fmt); \
    auto ptr_end = reinterpret_cast<u8*>(crc_ptr) + sizeof(fmt); \
    const auto cur_crc_val = __builtin_bswap16(*(reinterpret_cast<u16*>(ptr_end) - 1)); \
    /* Zero last u32 of the section and properly calculate CRC */ \
    auto crc_ptr_end_m4 = reinterpret_cast<u32*>(reinterpret_cast<u8*>(crc_ptr) + sizeof(fmt)) - 1; \
    *crc_ptr_end_m4 = 0; \
    const auto crc_val = ComputeCRC(crc_ptr, sizeof(fmt)); \
    if(cur_crc_val != crc_val) { \
        return false; \
    } \
}

#define UPDATE_SECTION_CRC(fmt, type) { \
    auto crc_ptr = reinterpret_cast<u32*>(&fmt); \
    auto ptr_end = reinterpret_cast<u8*>(crc_ptr) + sizeof(fmt); \
    /* Zero last u32 of the section and properly calculate CRC */ \
    auto crc_ptr_end_m4 = reinterpret_cast<u32*>(reinterpret_cast<u8*>(crc_ptr) + sizeof(fmt)) - 1; \
    *crc_ptr_end_m4 = 0; \
    const auto crc_val = ComputeCRC(crc_ptr, sizeof(fmt)); \
    *(reinterpret_cast<u16*>(ptr_end) - 2) = 0x1234; /* This value is some bits from Os_GetTick in console, so we can just set it to a random value */ \
    *(reinterpret_cast<u16*>(ptr_end) - 1) = __builtin_bswap16(crc_val); \
}

#define WRITE_ENCRYPT_SECTION(ptr, fmt, type) { \
    /* In game, the random value is generated using Os_GetTick bits as two seeds */ \
    const u32 rand_idx0 = rand() % GetMaxIdx0Value(); \
    WriteEncryptSection(ptr, &fmt, EditableSaveDataSectionOffsets[static_cast<u32>(type)], sizeof(fmt), SkipFirst8BytesFromDecrypt(type), decltype(fmt)::Magic, rand_idx0); \
}

bool ReadSaveData(SaveData &out_data, const void *save_ptr, const size_t size) { 
    READ_DECRYPT_SECTION(save_ptr, out_data.header, EditableSaveDataSection::Header); VERIFY_SECTION_CRC(out_data.header, EditableSaveDataSection::Header);
    READ_DECRYPT_SECTION(save_ptr, out_data.em, EditableSaveDataSection::Emblem); VERIFY_SECTION_CRC(out_data.em, EditableSaveDataSection::Emblem);
    READ_DECRYPT_SECTION(save_ptr, out_data.gp, EditableSaveDataSection::GrandPrix); VERIFY_SECTION_CRC(out_data.gp, EditableSaveDataSection::GrandPrix);
    READ_DECRYPT_SECTION(save_ptr, out_data.tt, EditableSaveDataSection::TimeTrial); VERIFY_SECTION_CRC(out_data.tt, EditableSaveDataSection::TimeTrial);
    READ_DECRYPT_SECTION(save_ptr, out_data.mr, EditableSaveDataSection::MissionRun); VERIFY_SECTION_CRC(out_data.mr, EditableSaveDataSection::MissionRun);
    READ_DECRYPT_SECTION(save_ptr, out_data.fl, EditableSaveDataSection::FriendList); VERIFY_SECTION_CRC(out_data.fl, EditableSaveDataSection::FriendList);
    return true;
}

void WriteSaveData(SaveData &data, void *save_ptr, const size_t size) {
    UPDATE_SECTION_CRC(data.header, EditableSaveDataSection::Header); WRITE_ENCRYPT_SECTION(save_ptr, data.header, EditableSaveDataSection::Header);
    UPDATE_SECTION_CRC(data.em, EditableSaveDataSection::Emblem); WRITE_ENCRYPT_SECTION(save_ptr, data.em, EditableSaveDataSection::Emblem);
    UPDATE_SECTION_CRC(data.gp, EditableSaveDataSection::GrandPrix); WRITE_ENCRYPT_SECTION(save_ptr, data.gp, EditableSaveDataSection::GrandPrix);
    UPDATE_SECTION_CRC(data.tt, EditableSaveDataSection::TimeTrial); WRITE_ENCRYPT_SECTION(save_ptr, data.tt, EditableSaveDataSection::TimeTrial);
    UPDATE_SECTION_CRC(data.mr, EditableSaveDataSection::MissionRun); WRITE_ENCRYPT_SECTION(save_ptr, data.mr, EditableSaveDataSection::MissionRun);
    UPDATE_SECTION_CRC(data.fl, EditableSaveDataSection::FriendList); WRITE_ENCRYPT_SECTION(save_ptr, data.fl, EditableSaveDataSection::FriendList);
}
