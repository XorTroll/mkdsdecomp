
#pragma once
#include <save_base.hpp>

#define EMBLEM_VAL_COLOR1(emblem_val) (static_cast<EmblemColor>(emblem_val & 0xF))
#define EMBLEM_VAL_COLOR2(emblem_val) (static_cast<EmblemColor>((emblem_val >> 4) & 0xF))
#define EMBLEM_VAL_FROM_COLORS(clr1, clr2) (clr1 | (clr2 << 4))

inline EmblemColor GetEmblemColorAt(const u8 *emblem_ptr, const u32 x, const u32 y) {
    const auto off = x + EmblemWidthHeight * y;
    const auto emblem_val = emblem_ptr[off / 2];
    if((off & 1) == 0) {
        return EMBLEM_VAL_COLOR1(emblem_val);
    }
    else {
        return EMBLEM_VAL_COLOR2(emblem_val);
    }
}

inline void SetEmblemColorAt(u8 *emblem_ptr, const u32 x, const u32 y, const EmblemColor clr) {
    const auto off = x + EmblemWidthHeight * y;
    auto emblem_val = emblem_ptr[off / 2];
    const auto clr_1 = EMBLEM_VAL_COLOR1(emblem_val);
    const auto clr_2 = EMBLEM_VAL_COLOR2(emblem_val);
    if((off & 1) != 0) {
        emblem_val = EMBLEM_VAL_FROM_COLORS(clr, clr_2);
    }
    else {
        emblem_val = EMBLEM_VAL_FROM_COLORS(clr_1, clr);
    }
    emblem_ptr[off / 2] = emblem_val;
}

enum class EditableSaveDataSection {
    Header,
    Emblem,
    GrandPrix,
    TimeTrial,
    MissionRun,
    FriendList,

    Count
};

constexpr size_t EditableSaveDataSectionOffsets[] = {
    0x0,
    0x100,
    0x400,
    0x800,
    0x1E00,
    0x1F00
};
static_assert(std::size(EditableSaveDataSectionOffsets) == static_cast<size_t>(EditableSaveDataSection::Count));

inline constexpr bool SkipFirst8BytesFromDecrypt(const EditableSaveDataSection section) {
    return section == EditableSaveDataSection::Header;
}

struct SaveData {
    SaveDataHeader header;
    SaveDataEmblem em;
    SaveDataGrandPrix gp;
    SaveDataTimeTrial tt;
    SaveDataMissionRun mr;
    SaveDataFriendList fl;
};

bool ReadSaveData(SaveData &out_data, const void *save_ptr, const size_t size);
void WriteSaveData(SaveData &data, void *save_ptr, const size_t size);
