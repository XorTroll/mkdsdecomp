
#pragma once
#include <mk/mk_Base.hpp>

namespace mk::save {

    enum EmblemColor {
        EmblemColor_Transparent = 0,
        EmblemColor_Black = 1,
        EmblemColor_White = 2,
        EmblemColor_Gray = 3,
        EmblemColor_Brown = 4,
        EmblemColor_Red = 5,
        EmblemColor_Orange = 6,
        EmblemColor_LightOrange = 7,
        EmblemColor_Yellow = 8,
        EmblemColor_LightGreen = 9,
        EmblemColor_DarkGreen = 10,
        EmblemColor_LightBlue = 11,
        EmblemColor_DarkBlue = 12,
        EmblemColor_Purple = 13,
        EmblemColor_Crimson = 14,
        EmblemColor_Pink = 15,
    };

    enum RacerType {
        RacerType_Player = 0,
        RacerType_CPU = 1,
        RacerType_Ghost = 2,
        RacerType_Unk3 = 3,
        RacerType_Unk4 = 4,
    };

    enum CharacterId {
        CharacterId_Mario = 0,
        CharacterId_DK = 1,
        CharacterId_Toad = 2,
        CharacterId_Bowser = 3,
        CharacterId_Peach = 4,
        CharacterId_Wario = 5,
        CharacterId_Yoshi = 6,
        CharacterId_Luigi = 7,
        CharacterId_DryBones = 8,
        CharacterId_Daisy = 9,
        CharacterId_Waluigi = 10,
        CharacterId_ROB = 11,
        CharacterId_ShyGuy = 12,
        CharacterId_Count = 13,
        CharacterId_None = 4294967295,
    };

    enum Team {
        Team_Blue = 0,
        Team_Red = 1,
        Team_None = 4294967295,
    };

    enum CourseId {
        CourseId_cross_course = 0,
        CourseId_bank_course = 1,
        CourseId_beach_course = 2,
        CourseId_mansion_course = 3,
        CourseId_desert_course = 4,
        CourseId_town_course = 5,
        CourseId_pinball_course = 6,
        CourseId_ridge_course = 7,
        CourseId_snow_course = 8,
        CourseId_clock_course = 9,
        CourseId_mario_course = 10,
        CourseId_airship_course = 11,
        CourseId_stadium_course = 12,
        CourseId_garden_course = 13,
        CourseId_koopa_course = 14,
        CourseId_rainbow_course = 15,
        CourseId_old_mario_sfc = 16,
        CourseId_old_momo_64 = 17,
        CourseId_old_peach_agb = 18,
        CourseId_old_luigi_gc = 19,
        CourseId_old_donut_sfc = 20,
        CourseId_old_frappe_64 = 21,
        CourseId_old_koopa_agb = 22,
        CourseId_old_baby_gc = 23,
        CourseId_old_noko_sfc = 24,
        CourseId_old_choco_64 = 25,
        CourseId_old_luigi_agb = 26,
        CourseId_old_kinoko_gc = 27,
        CourseId_old_choco_sfc = 28,
        CourseId_old_hyudoro_64 = 29,
        CourseId_old_sky_agb = 30,
        CourseId_old_yoshi_gc = 31,
        CourseId_Count = 32,
    };

    enum KartId {
        KartId_StandardMR = 0,
        KartId_ShootingStar = 1,
        KartId_BDasher = 2,
        KartId_StandardDK = 3,
        KartId_Wildlife = 4,
        KartId_RambiRider = 5,
        KartId_StandardTD = 6,
        KartId_Mushmellow = 7,
        KartId_4WheelCradle = 8,
        KartId_StandardBW = 9,
        KartId_Hurricane = 10,
        KartId_Tyrant = 11,
        KartId_StandardPC = 12,
        KartId_LightTrippler = 13,
        KartId_Royale = 14,
        KartId_StandardWR = 15,
        KartId_Brute = 16,
        KartId_Dragonfly = 17,
        KartId_StandardYS = 18,
        KartId_Egg1 = 19,
        KartId_Cucumber = 20,
        KartId_StandardLG = 21,
        KartId_Poltergust4000 = 22,
        KartId_Streamliner = 23,
        KartId_StandardDB = 24,
        KartId_DryBomber = 25,
        KartId_Banisher = 26,
        KartId_StandardDS = 27,
        KartId_LightDancer = 28,
        KartId_PowerFlower = 29,
        KartId_StandardWL = 30,
        KartId_GoldMantis = 31,
        KartId_Zipper = 32,
        KartId_StandardRB = 33,
        KartId_ROBBLS = 34,
        KartId_ROBLGS = 35,
        KartId_StandardHH = 36,
        KartId_Count = 37,
    };

    enum CupResult {
        CupResult_Gold = 0,
        CupResult_Silver = 1,
        CupResult_Bronze = 2,
        CupResult_Lost = 3,
    };

    enum CupId {
        CupId_Nitro_Mushroom = 0,
        CupId_Nitro_Flower = 1,
        CupId_Nitro_Star = 2,
        CupId_Nitro_Special = 3,
        CupId_Retro_Shell = 4,
        CupId_Retro_Banana = 5,
        CupId_Retro_Leaf = 6,
        CupId_Retro_Lightning = 7,
    };

    enum PackedSecretFlag {
        PackedSecretFlag_Cup_MushroomFlowerCups_50cc = 0,
        PackedSecretFlag_Cup_MushroomFlowerCups_100cc = 1,
        PackedSecretFlag_Cup_MushroomFlowerCups_150cc = 2,
        PackedSecretFlag_Cup_MushroomFlowerCups_150cc_Mirror = 3,
        PackedSecretFlag_Cup_StarCup_50cc = 4,
        PackedSecretFlag_Cup_StarCup_100cc = 5,
        PackedSecretFlag_Cup_StarCup_150cc = 6,
        PackedSecretFlag_Cup_StarCup_150cc_Mirror = 7,
        PackedSecretFlag_Cup_ShellBananaCups_50cc = 8,
        PackedSecretFlag_Cup_ShellBananaCups_100cc = 9,
        PackedSecretFlag_Cup_ShellBananaCups_150cc = 10,
        PackedSecretFlag_Cup_ShellBananaCups_150cc_Mirror = 11,
        PackedSecretFlag_Cup_LeafCup_50cc = 12,
        PackedSecretFlag_Cup_LeafCup_100cc = 13,
        PackedSecretFlag_Cup_LeafCup_150cc = 14,
        PackedSecretFlag_Cup_LeafCup_150cc_Mirror = 15,
        PackedSecretFlag_Cup_SpecialLightningCups_150cc = 16,
        PackedSecretFlag_CharacterKart_ExtraKartBasicUnlock = 17,
        PackedSecretFlag_CharacterKart_ExtraKartMediumUnlock = 18,
        PackedSecretFlag_CharacterKart_ExtraKartTotalUnlock = 19,
        PackedSecretFlag_CharacterKart_DryBonesUnlocked = 20,
        PackedSecretFlag_CharacterKart_DaisyUnlocked = 21,
        PackedSecretFlag_CharacterKart_WaluigiUnlocked = 22,
        PackedSecretFlag_CharacterKart_RobUnlocked = 23,
        PackedSecretFlag_CategoryUnk_Unk24 = 24,
        PackedSecretFlag_Ending_AllCups = 25,
        PackedSecretFlag_Ending_AllMissions = 26,
    };

    enum SaveDataHeader_Flags2 {
        SaveDataHeader_Flags2_None = 0,
        SaveDataHeader_Flags2_HasUserName = 1,
        SaveDataHeader_Flags2_Unk2 = 2,
        SaveDataHeader_Flags2_Unk4 = 4,
        SaveDataHeader_Flags2_Unk8 = 8,
        SaveDataHeader_Flags2_Unk16 = 16,
    };

    enum FriendCodeState {
        FriendCodeState_Unk0 = 0x0,
        FriendCodeState_Unk1 = 0x1,
        FriendCodeState_Unk2 = 0x2,
        FriendCodeState_Unk3 = 0x3,
    };

    struct FriendCode {
        u32 unk_and_flags;
        u32 profile_id;
        u32 crc_checksum;
    } __attribute__((packed));
    static_assert(sizeof(FriendCode) == 0xC, "FriendCode definition");

    enum PlayerProfileInfo_Flag {
        PlayerProfileInfo_Flag_None = 0x0,
        PlayerProfileInfo_Flag_NeedsSaving = 0x1,
    };

    struct PlayerProfileInfo {
        u32 size;
        FriendCode friend_code_1;
        FriendCode friend_code_2;
        u32 profile_id;
        u32 flags;
        u32 game_id;
        u32 unk_x28;
        u32 unk_x2C;
        u32 unk_x30;
        u32 unk_x34;
        u32 unk_x38;
        u32 crc_checksum;
    };
    static_assert(sizeof(PlayerProfileInfo) == 0x40, "PlayerProfileInfo definition");

    constexpr auto MKDSGameId = 0x414D434A; // "JCMA"

    constexpr u32 MagicMKDS = 0x53444B4D;
    constexpr u32 MagicSV10 = 0x30315653;
    constexpr u32 MagicNKSY = 0x59534B4E;

    struct SaveDataHeader {
        u32 magic_mkds;
        u32 magic_sv10;
        u32 magic_nksy;
        u16 nickname[10];
        u32 secret_unlock_flags;
        u16 unk24_some_size;
        u8 personal_ghost_bits[4];
        u8 download_ghost_bits[4];
        u8 unk2E_nkfe[2];
        u8 unk30;
        u8 flags_2;
        u8 unk32;
        u8 unk33;
        u32 unk34;
        u32 unk38;
        int unk3C_val_up_to_1e6;
        int unk40_some_idx;
        u32 unk44;
        u32 unk48;
        PlayerProfileInfo profile_info;
        u8 unk8C[116];

        static constexpr auto Magic = MagicNKSY;
    };
    static_assert(sizeof(SaveDataHeader) == 0x100, "SaveDataHeader definition");

    constexpr u32 MagicNKEM = 0x4D454B4E;

    struct SaveDataEmblem {
        u32 magic_nkem;
        u32 has_emblem;
        u8 emblem_data[512];
        u32 unk208[62];

        static constexpr auto Magic = MagicNKEM;
    };
    static_assert(sizeof(SaveDataEmblem) == 0x300, "SaveDataEmblem definition");

    struct SaveDataGrandPrixCupRecordEntry {
        u8 valid : 1;
        u8 character_id : 7;
        u8 kart_id;
        u8 points;
        u8 rank;

        u8 cup_res;
        u8 time_min : 7;
        u8 time_sec : 7;
        u16 time_milli : 10;

        u16 nickname[10];
    } __attribute__((packed));
    static_assert(sizeof(SaveDataGrandPrixCupRecordEntry) == 0x1C, "SaveDataGrandPrixCupRecordEntry definition");

    struct SaveDataGrandPrixCcEntry {
        SaveDataGrandPrixCupRecordEntry cups[8];
    };
    static_assert(sizeof(SaveDataGrandPrixCcEntry) == 0xE0, "SaveDataGrandPrixCcEntry definition");

    constexpr u32 MagicNKGP = 0x50474B4E;

    struct SaveDataGrandPrix {
        u32 magic_nkgp;
        SaveDataGrandPrixCcEntry entries[4];
        u32 unk384[31];

        static constexpr auto Magic = MagicNKGP;
    };
    static_assert(sizeof(SaveDataGrandPrix) == 0x400, "SaveDataGrandPrix definition");

    struct SaveDataTimeTrialCourseRecordEntry {
        u8 valid : 1;
        u8 character_id : 7;
        u8 time_min : 7;
        u8 time_sec : 7;
        u16 time_milli : 10;
        u8 kart_id;
        u8 unk5;
        u8 unk6;
        u8 unk7;
        u16 nickname[10];
    } __attribute__((packed));
    static_assert(sizeof(SaveDataTimeTrialCourseRecordEntry) == 0x1C, "SaveDataTimeTrialCourseRecordEntry definition");

    struct SaveDataTimeTrialRecordListEntry {
        SaveDataTimeTrialCourseRecordEntry course_entries[32];
    };
    static_assert(sizeof(SaveDataTimeTrialRecordListEntry) == 0x380, "SaveDataTimeTrialCourseEntry definition");

    constexpr u32 MagicNKTA = 0x41544B4E;

    struct SaveDataTimeTrial {
        u32 magic_nkta;
        SaveDataTimeTrialRecordListEntry records[5];
        SaveDataTimeTrialRecordListEntry record_best_lap;
        u32 unk1504[63];

        static constexpr auto Magic = MagicNKTA;
    };
    static_assert(sizeof(SaveDataTimeTrial) == 0x1600, "SaveDataTimeTrial definition");

    enum Rank {
        Rank_E = 0,
        Rank_D = 1,
        Rank_C = 2,
        Rank_B = 3,
        Rank_A = 4,
        Rank_Star1 = 5,
        Rank_Star2 = 6,
        Rank_Star3 = 7,
    };

    struct SaveDataMissionRunLevelStageEntry {
        u8 beaten: 1;
        u8 rank: 7;
    };

    struct SaveDataMissionRunLevelEntry {
        SaveDataMissionRunLevelStageEntry stage_entries[9];
    };
    static_assert(sizeof(SaveDataMissionRunLevelEntry) == 0x9, "SaveDataMissionRunLevelEntry definition");

    constexpr u32 MagicNKMR = 0x524D4B4E;

    struct SaveDataMissionRun {
        u32 magic_nkmr;
        SaveDataMissionRunLevelEntry levels[7];
        u8 nkmrs[188];

        static constexpr auto Magic = MagicNKMR;
    };
    static_assert(sizeof(SaveDataMissionRun) == 0x100, "SaveDataMissionRun definition");

    struct GhostTime {
        u8 unk1;
        u8 unk2;
        u8 unk3;
    };
    static_assert(sizeof(GhostTime) == 0x3, "GhostTime definition");

    constexpr u32 MagicNKPG = 0x47504B4E;
    constexpr u32 MagicNKDG = 0x47444B4E;

    struct GhostHeader {
        u32 magic_nkdg_nkpg;
        u16 _bf_4;
        u16 unk1;
        u8 flags;
        u8 _bf_9;
        u16 _bf_a;
        u16 name[10];
        GhostTime lap_times[5];
        u8 unk;
    };
    static_assert(sizeof(GhostHeader) == 0x30, "GhostHeader definition");

    struct GhostHeaderEx {
        GhostHeader header;
        u8 emblem[512];
    };
    static_assert(sizeof(GhostHeaderEx) == 0x230, "GhostHeaderEx definition");

    struct SaveDataPersonalGhost {
        GhostHeaderEx pg_header_ex;
        u32 unk230[756];

        static constexpr auto Magic = MagicNKPG;
    };
    static_assert(sizeof(SaveDataPersonalGhost) == 0xE00, "SaveDataPersonalGhost definition");

    struct StructOI {
        u16 oi1;
        u16 oi2;
        u16 oi3;
        u16 oi4;
        u16 oi5;
        u16 oi6;
        u16 oi7;
        u8 racer_name[20];
        u16 flags;
    };
    static_assert(sizeof(StructOI) == 0x24, "StructOI definition");

    constexpr u32 MagicNKFL = 0x4C464B4E;

    struct SaveDataFriendList {
        u32 magic_nkfl;
        StructOI unk4_ois[60];
        FriendCode friend_codes[60];
        u32 unkB44[47];

        static constexpr auto Magic = MagicNKFL;
    };
    static_assert(sizeof(SaveDataFriendList) == 0xC00, "SaveDataFriendList definition");

    constexpr u32 EmblemWidthHeight = 32;

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

}
