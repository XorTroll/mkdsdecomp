#include <mk/save/save_Format.hpp>
#include <mk/save/save_Crypto.hpp>

namespace mk::save {

    namespace {

        enum class SaveDataSection {
            Header,
            Emblem,
            GrandPrix,
            TimeTrial,
            MissionRun,
            FriendList,
            // UnknownNKFE,
            // PersonalGhost,

            Count
        };

        inline constexpr bool SkipFirst8BytesFromDecrypt(const SaveDataSection section) {
            return section == SaveDataSection::Header;
        }

        constexpr size_t SaveDataSectionOffsets[] = {
            0x0,
            0x100,
            0x400,
            0x800,
            0x1E00,
            0x1F00,
            // 0x2B00,
            // 0xAF00,
        };
        static_assert(std::size(SaveDataSectionOffsets) == static_cast<size_t>(SaveDataSection::Count));

    }

    #define READ_DECRYPT_SECTION(ptr, fmt, type) { \
        if(!ReadDecryptSection(ptr, &fmt, SaveDataSectionOffsets[static_cast<u32>(type)], sizeof(fmt), SkipFirst8BytesFromDecrypt(type), decltype(fmt)::Magic)) { \
            return false; \
        } \
    }

    #define READ_SECTION(ptr, fmt, type) { \
        memcpy(&fmt, reinterpret_cast<const u8*>(ptr) + SaveDataSectionOffsets[static_cast<u32>(type)], sizeof(fmt)); \
    }

    #define VERIFY_SECTION_CRC(fmt, type) { \
        /* Get current CRC value, last (BE) u16 of the savedata section */ \
        auto crc_ptr = reinterpret_cast<u32*>(&fmt); \
        auto ptr_end = reinterpret_cast<u8*>(crc_ptr) + sizeof(fmt); \
        const auto cur_crc_val = __builtin_bswap16(*(reinterpret_cast<u16*>(ptr_end) - 1)); \
        /* Zero last u32 of the section and properly calculate CRC */ \
        auto crc_ptr_end_m4 = reinterpret_cast<u32*>(reinterpret_cast<u8*>(crc_ptr) + sizeof(fmt)) - 1; \
        *crc_ptr_end_m4 = 0; \
        const auto crc_val = ComputeCrc(crc_ptr, sizeof(fmt)); \
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
        const auto crc_val = ComputeCrc(crc_ptr, sizeof(fmt)); \
        *(reinterpret_cast<u16*>(ptr_end) - 2) = 0x1234; /* This value is some bits from Os_GetTick in console, so we can just set it to a random value */ \
        *(reinterpret_cast<u16*>(ptr_end) - 1) = __builtin_bswap16(crc_val); \
    }

    #define WRITE_ENCRYPT_SECTION(ptr, fmt, type) { \
        /* In game, the random value is generated using Os_GetTick bits as two seeds */ \
        const u32 rand_idx0 = rand() % GetMaxIdx0Value(); \
        WriteEncryptSection(ptr, &fmt, SaveDataSectionOffsets[static_cast<u32>(type)], sizeof(fmt), SkipFirst8BytesFromDecrypt(type), decltype(fmt)::Magic, rand_idx0); \
    }

    #define WRITE_SECTION(ptr, fmt, type) { \
        memcpy(reinterpret_cast<u8*>(ptr) + SaveDataSectionOffsets[static_cast<u32>(type)], &fmt, sizeof(fmt)); \
    }

    bool ReadSaveData(SaveData &out_data, const void *save_ptr, const bool is_enc) { 
        if(is_enc) {
            READ_DECRYPT_SECTION(save_ptr, out_data.header, SaveDataSection::Header);
            READ_DECRYPT_SECTION(save_ptr, out_data.em, SaveDataSection::Emblem);
            READ_DECRYPT_SECTION(save_ptr, out_data.gp, SaveDataSection::GrandPrix);
            READ_DECRYPT_SECTION(save_ptr, out_data.tt, SaveDataSection::TimeTrial);
            READ_DECRYPT_SECTION(save_ptr, out_data.mr, SaveDataSection::MissionRun);
            READ_DECRYPT_SECTION(save_ptr, out_data.fl, SaveDataSection::FriendList);
        }
        else {
            READ_SECTION(save_ptr, out_data.header, SaveDataSection::Header);
            READ_SECTION(save_ptr, out_data.em, SaveDataSection::Emblem);
            READ_SECTION(save_ptr, out_data.gp, SaveDataSection::GrandPrix);
            READ_SECTION(save_ptr, out_data.tt, SaveDataSection::TimeTrial);
            READ_SECTION(save_ptr, out_data.mr, SaveDataSection::MissionRun);
            READ_SECTION(save_ptr, out_data.fl, SaveDataSection::FriendList);
        }

        VERIFY_SECTION_CRC(out_data.header, SaveDataSection::Header);
        VERIFY_SECTION_CRC(out_data.em, SaveDataSection::Emblem);
        VERIFY_SECTION_CRC(out_data.gp, SaveDataSection::GrandPrix);
        VERIFY_SECTION_CRC(out_data.tt, SaveDataSection::TimeTrial);
        VERIFY_SECTION_CRC(out_data.mr, SaveDataSection::MissionRun);
        VERIFY_SECTION_CRC(out_data.fl, SaveDataSection::FriendList);

        return true;
    }

    void WriteSaveData(SaveData &data, void *save_ptr, const bool is_enc) {
        UPDATE_SECTION_CRC(data.header, SaveDataSection::Header);
        UPDATE_SECTION_CRC(data.em, SaveDataSection::Emblem);
        UPDATE_SECTION_CRC(data.gp, SaveDataSection::GrandPrix);
        UPDATE_SECTION_CRC(data.tt, SaveDataSection::TimeTrial);
        UPDATE_SECTION_CRC(data.mr, SaveDataSection::MissionRun);
        UPDATE_SECTION_CRC(data.fl, SaveDataSection::FriendList);

        if(is_enc) {
            WRITE_ENCRYPT_SECTION(save_ptr, data.header, SaveDataSection::Header);
            WRITE_ENCRYPT_SECTION(save_ptr, data.em, SaveDataSection::Emblem);
            WRITE_ENCRYPT_SECTION(save_ptr, data.gp, SaveDataSection::GrandPrix);
            WRITE_ENCRYPT_SECTION(save_ptr, data.tt, SaveDataSection::TimeTrial);
            WRITE_ENCRYPT_SECTION(save_ptr, data.mr, SaveDataSection::MissionRun);
            WRITE_ENCRYPT_SECTION(save_ptr, data.fl, SaveDataSection::FriendList);
        }
        else {
            WRITE_SECTION(save_ptr, data.header, SaveDataSection::Header);
            WRITE_SECTION(save_ptr, data.em, SaveDataSection::Emblem);
            WRITE_SECTION(save_ptr, data.gp, SaveDataSection::GrandPrix);
            WRITE_SECTION(save_ptr, data.tt, SaveDataSection::TimeTrial);
            WRITE_SECTION(save_ptr, data.mr, SaveDataSection::MissionRun);
            WRITE_SECTION(save_ptr, data.fl, SaveDataSection::FriendList);
        }
    }

}
