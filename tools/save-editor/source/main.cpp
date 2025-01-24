#define GLFW_INCLUDE_ES3
#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#ifndef __EMSCRIPTEN__
#error "This should be compiled as JS!"
#endif
#include <emscripten.h>

#include <mk/save/save_Format.hpp>
#include <util.hpp>

using namespace mk;

EM_JS(int, GetCanvasWidth, (), {
  return Module.canvas.width;
});

EM_JS(int, GetCanvasHeight, (), {
  return Module.canvas.height;
});

EM_JS(void, ResizeCanvas, (), {
    js_ResizeCanvas();
});

EM_JS(void, OpenUrl, (const char *url), {
    window.open(UTF8ToString(url), "_blank");
});

// Note: defined in JS since it is also used from there
EM_JS(void, ShowError, (const char *error), {
    js_ShowError(UTF8ToString(error));
});

EM_JS(void, ShowInformation, (const char *info), {
    alert(UTF8ToString(info));
});

#define IMGUI_NORM_COLOR_COMPONENT(x) ((double)x / 255.0)
#define IMGUI_RGB(r, g, b) ImVec4(IMGUI_NORM_COLOR_COMPONENT(r), IMGUI_NORM_COLOR_COMPONENT(g), IMGUI_NORM_COLOR_COMPONENT(b), 1.0)

namespace {

    constexpr auto ClearColor = ImVec4(0.14, 0.14, 0.4, 1.0);

    GLFWwindow *g_Window;
    int g_Width;
    int g_Height;

    bool g_DisplayAboutWindow = false;

    #define IMGUI_RGB(r, g, b) IM_COL32(r, g, b, 255)

    u32 g_EmblemColorTable[] = {
        IM_COL32(0, 0, 0, 0),
        IMGUI_RGB(0, 0, 0),
        IMGUI_RGB(251, 251, 251),
        IMGUI_RGB(130, 130, 130),
        IMGUI_RGB(113, 73, 32),
        IMGUI_RGB(251, 0, 0),
        IMGUI_RGB(243, 146, 24),
        IMGUI_RGB(251, 203, 154),
        IMGUI_RGB(251, 251, 0),
        IMGUI_RGB(0, 251, 0),
        IMGUI_RGB(0, 89, 32),
        IMGUI_RGB(0, 251, 251),
        IMGUI_RGB(0, 0, 251),
        IMGUI_RGB(138, 32, 251),
        IMGUI_RGB(203, 0, 146),
        IMGUI_RGB(251, 89, 251)
    };

    const char *g_CharacterNames[] = {
        "Mario",
        "DK",
        "Toad",
        "Bowser",
        "Peach",
        "Wario",
        "Yoshi",
        "Luigi",
        "Dry bones",
        "Daisy",
        "Waluigi",
        "ROB",
        "Shy Guy"
    };

    const char *g_CcNames[] = {
        "50cc",
        "100cc",
        "150cc",
        "Mirror 150cc"
    };

    const char *g_KartNames[] = {
        "Standard MR",
        "Shooting Star",
        "B Dasher",
        "Standard DK",
        "Wildlife",
        "Rambi Rider",
        "Standard TD",
        "Mushmellow",
        "4-Wheel Cradle",
        "Standard BW",
        "Hurricane",
        "Tyrant",
        "Standard PC",
        "Light Trippler",
        "Royale",
        "Standard WR",
        "Brute",
        "Dragonfly",
        "Standard YS",
        "Egg 1",
        "Cucumber",
        "Standard LG",
        "Poltergust 4000",
        "Streamliner",
        "Standard DB",
        "Dry Bomber",
        "Banisher",
        "Standard DS",
        "Light Dancer",
        "Power Flower",
        "Standard WL",
        "Gold Mantis",
        "Zipper",
        "Standard RB",
        "ROB-BLS",
        "ROB-LGS",
        "Standard HH"
    };

    const char *g_CupResultNames[] = {
        "Gold/1st",
        "Silver/2nd",
        "Bronze/3rd"
    };

    const char *g_CupNames[] = {
        "Mushroom",
        "Flower",
        "Star",
        "Special",
        "Shell",
        "Banana",
        "Leaf",
        "Lightning"
    };

    const char *g_SecretFlagBitNames[] = {
        "Mushroom and Flower cups beaten at 50cc",
        "Mushroom and Flower cups beaten at 100cc",
        "Mushroom and Flower cups beaten at 150cc",
        "Mushroom and Flower cups beaten at mirror 150cc",
        "Star cup beaten at 50cc",
        "Star cup beaten at 100cc",
        "Star cup beaten at 150cc",
        "Star cup beaten at mirror 150cc",
        "Shell and Banana cups beaten at 50cc",
        "Shell and Banana cups beaten at 100cc",
        "Shell and Banana cups beaten at 150cc",
        "Shell and Banana cups beaten at mirror 150cc",
        "Leaf cup beaten at 50cc",
        "Leaf cup beaten at 100cc",
        "Leaf cup beaten at 150cc",
        "Leaf cup beaten at mirror 150cc",
        "Special and Lightning cups beaten at 150cc",
        "Kart unlock 1",
        "Kart unlock 2",
        "Kart unlock 3",
        "Dry bones unlocked",
        "Daisy unlocked",
        "Waluigi unlocked",
        "ROB unlocked",
        "<bit 24>",
        "All cups beaten",
        "All missions beaten"
    };

    const char *g_RankNames[] = {
        "Rank E",
        "Rank D",
        "Rank C",
        "Rank B",
        "Rank A",
        "Rank S",
        "Rank SS",
        "Rank SSS"
    };

    const char *g_CourseNames[] = {
        "Figure-8 Circuit",
        "Yoshi Falls",
        "Cheep Cheep Beach",
        "Luigi's Mansion",
        "Desert Hills",
        "Delfino Square",
        "Waluigi Pinball",
        "Shroom Ridge",
        "DK Pass",
        "Tick-Tock Clock",
        "Mario Circuit",
        "Airship Fortress",
        "Wario Stadium",
        "Peach Gardens",
        "Bowser Castle",
        "Rainbow Road",
        "SNES Mario Circuit 1",
        "N64 Moo Moo Farn",
        "GBA Peach Circuit",
        "GCN Luigi Circuit",
        "SNES Donut Plains 1",
        "N64 Frappe Snowland",
        "GBA Bowser Castle 2",
        "GCN Baby Park",
        "SNES Koopa Beach 2",
        "N64 Choco Mountain",
        "GBA Luigi Circuit",
        "GCN Mushroom Bridge",
        "SNES Choco Island 2",
        "N64 Banshee Boardwalk",
        "GBA Sky Garden",
        "GCN Yoshi Circuit"
    };

    void *g_RawSaveData;
    size_t g_RawSaveDataSize;
    bool g_PendingLoadRawSaveData = false;
    save::SaveData g_SaveData = {};

    constexpr size_t SecretFlagCount = 27;

    char g_EditHeaderNickname[10+1];
    bool g_EditSecretFlags[SecretFlagCount];

    bool g_EditEmblemEnabled;
    save::EmblemColor g_EditEmblemPixelData[save::EmblemWidthHeight * save::EmblemWidthHeight];
    constexpr auto EmblemGridColor = IM_COL32(120, 120, 120, 255);
    int g_GridSpacing = 1;
    save::EmblemColor g_PaintColor = save::EmblemColor_White;
    int g_EmblemScale = 10;

    struct GrandPrixCupInfo {
        bool valid;
        int time_min;
        int time_sec;
        int time_milli;
        int character_id;
        int kart_id;
        int points;
        int rank;
        int cup_res;
        char nickname[10+1];
    };
    GrandPrixCupInfo g_CupInfos[8][4];

    struct TimeTrialRecordInfo {
        bool valid;
        int time_min;
        int time_sec;
        int time_milli;
        int character_id;
        int kart_id;
        char nickname[10+1];
    };
    TimeTrialRecordInfo g_TimeTrialRecords[32][6];

    struct MissionRunStageInfo {
        int beaten;
        int rank;
    };
    MissionRunStageInfo g_MissionRunStageInfos[7][9];

    constexpr float WindowWidthFactor = 0.8;
    constexpr float WindowHeightFactor = 0.8;

    inline void SetEmblemPixel(const u32 x, const u32 y, save::EmblemColor clr) {
        g_EditEmblemPixelData[y * save::EmblemWidthHeight + x] = clr;
    }

    inline save::EmblemColor GetEmblemPixel(const u32 x, const u32 y) {
        return g_EditEmblemPixelData[y * save::EmblemWidthHeight + x];
    }

    inline bool HasSaveDataLoaded() {
        return g_SaveData.header.magic_nksy == save::MagicNKSY;
    }

    void LoadEditFields() {
        auto nickname = ConvertFromUtf16(reinterpret_cast<char16_t*>(g_SaveData.header.nickname));
        strncpy(g_EditHeaderNickname, nickname.c_str(), 10);

        for(u32 i = 0; i < SecretFlagCount; i++) {
            g_EditSecretFlags[i] = (g_SaveData.header.secret_unlock_flags & (1 << i)) != 0;
        }

        memset(&g_CupInfos, 0, sizeof(g_CupInfos));
        for(u32 cup = 0; cup < 8; cup++) {
            for(u32 cc = 0; cc < 4; cc++) {
                auto edit_info = &g_CupInfos[cup][cc];
                auto cup_info = &g_SaveData.gp.entries[cc].cups[cup];

                edit_info->valid = cup_info->valid;
                if(edit_info->valid) {
                    edit_info->time_milli = cup_info->time_milli;
                    edit_info->time_min = cup_info->time_min;
                    edit_info->time_sec = cup_info->time_sec;
                    edit_info->character_id = cup_info->character_id;
                    edit_info->kart_id = cup_info->kart_id;
                    edit_info->points = cup_info->points;
                    edit_info->rank = cup_info->rank;
                    edit_info->cup_res = cup_info->cup_res;

                    const auto nickname = ConvertFromUtf16(reinterpret_cast<char16_t*>(cup_info->nickname));
                    strncpy(edit_info->nickname, nickname.c_str(), nickname.length() * sizeof(char16_t));
                }
                else {
                    edit_info->time_milli = 999;
                    edit_info->time_min = 10;
                    edit_info->time_sec = 59;
                    edit_info->character_id = save::CharacterId_Mario;
                    edit_info->kart_id = save::KartId_StandardMR;
                    edit_info->points = 5;
                    edit_info->rank = save::Rank_E;
                    edit_info->cup_res = save::CupResult_Bronze;

                    const std::string nickname = "XorTroll";
                    strncpy(edit_info->nickname, nickname.c_str(), nickname.length());
                }
            }
        }

        memset(&g_TimeTrialRecords, 0, sizeof(g_TimeTrialRecords));
        for(u32 c = 0; c < 32; c++) {
            for(u32 r = 0; r < 6; r++) {
                auto edit_record = &g_TimeTrialRecords[c][r];
                auto tt_record = &g_SaveData.tt.records[r].course_entries[c];

                edit_record->valid = tt_record->valid;
                if(edit_record->valid) {
                    edit_record->time_milli = tt_record->time_milli;
                    edit_record->time_min = tt_record->time_min;
                    edit_record->time_sec = tt_record->time_sec;
                    edit_record->character_id = tt_record->character_id;
                    edit_record->kart_id = tt_record->kart_id;

                    const auto nickname = ConvertFromUtf16(reinterpret_cast<char16_t*>(tt_record->nickname));
                    strncpy(edit_record->nickname, nickname.c_str(), nickname.length());
                }
                else {
                    edit_record->time_milli = 999;
                    edit_record->time_min = 10;
                    edit_record->time_sec = 59;
                    edit_record->character_id = save::CharacterId_Mario;
                    edit_record->kart_id = save::KartId_StandardMR;

                    const std::string nickname = "XorTroll";
                    strncpy(edit_record->nickname, nickname.c_str(), nickname.length());
                }
            }
        }

        memset(&g_MissionRunStageInfos, 0, sizeof(g_MissionRunStageInfos));
        for(u32 l = 0; l < 7; l++) {
            for(u32 s = 0; s < 9; s++) {
                g_MissionRunStageInfos[l][s].beaten = g_SaveData.mr.levels[l].stage_entries[s].beaten;
                if(g_MissionRunStageInfos[l][s].beaten) {
                    g_MissionRunStageInfos[l][s].rank = g_SaveData.mr.levels[l].stage_entries[s].rank;
                }
                else {
                    g_MissionRunStageInfos[l][s].rank = save::Rank_E;
                }
            }
        }

        g_EditEmblemEnabled = g_SaveData.em.has_emblem != 0;
        for(u32 x = 0; x < save::EmblemWidthHeight; x++) {
            for(u32 y = 0; y < save::EmblemWidthHeight; y++) {
                const auto clr = save::GetEmblemColorAt(g_SaveData.em.emblem_data, x, y);
                SetEmblemPixel(x, y, clr);
            }
        }
    }

    void SaveEditFields() {
        const auto nickname16 = ConvertToUtf16(g_EditHeaderNickname);
        memset(g_SaveData.header.nickname, 0, sizeof(g_SaveData.header.nickname));
        memcpy(g_SaveData.header.nickname, nickname16.c_str(), nickname16.length() * sizeof(char16_t));

        u32 secret_flags = 0;
        for(u32 i = 0; i < SecretFlagCount; i++) {
            if(g_EditSecretFlags[i]) {
                secret_flags |= 1 << i;
            }
        }
        g_SaveData.header.secret_unlock_flags = secret_flags;

        for(u32 cup = 0; cup < 8; cup++) {
            for(u32 cc = 0; cc < 4; cc++) {
                auto edit_info = &g_CupInfos[cup][cc];
                auto cup_info = &g_SaveData.gp.entries[cc].cups[cup];

                cup_info->valid = edit_info->valid;
                if(edit_info->valid) {
                    cup_info->time_milli = edit_info->time_milli;
                    cup_info->time_min = edit_info->time_min;
                    cup_info->time_sec = edit_info->time_sec;
                    cup_info->character_id = edit_info->character_id;
                    cup_info->kart_id = edit_info->kart_id;
                    cup_info->points = edit_info->points;
                    cup_info->rank = edit_info->rank;
                    cup_info->cup_res = edit_info->cup_res;

                    const auto nickname = ConvertToUtf16(edit_info->nickname);
                    memset(cup_info->nickname, 0, sizeof(cup_info->nickname));
                    memcpy(cup_info->nickname, nickname.c_str(), nickname.length() * sizeof(char16_t));
                }
            }
        }

        for(u32 c = 0; c < 32; c++) {
            for(u32 r = 0; r < 6; r++) {
                auto edit_info = &g_TimeTrialRecords[c][r];
                auto tt_info = &g_SaveData.tt.records[r].course_entries[c];

                tt_info->valid = edit_info->valid;
                if(edit_info->valid) {
                    tt_info->time_milli = edit_info->time_milli;
                    tt_info->time_min = edit_info->time_min;
                    tt_info->time_sec = edit_info->time_sec;
                    tt_info->character_id = edit_info->character_id;
                    tt_info->kart_id = edit_info->kart_id;

                    const auto nickname = ConvertToUtf16(edit_info->nickname);
                    memset(tt_info->nickname, 0, sizeof(tt_info->nickname));
                    memcpy(tt_info->nickname, nickname.c_str(), nickname.length() * sizeof(char16_t));
                }
            }
        }

        for(u32 l = 0; l < 7; l++) {
            for(u32 s = 0; s < 9; s++) {
                g_SaveData.mr.levels[l].stage_entries[s].beaten = g_MissionRunStageInfos[l][s].beaten;
                if(g_SaveData.mr.levels[l].stage_entries[s].beaten) {
                    g_SaveData.mr.levels[l].stage_entries[s].rank = g_MissionRunStageInfos[l][s].rank;
                }
            }
        }

        g_SaveData.em.has_emblem = g_EditEmblemEnabled;
        for(u32 x = 0; x < save::EmblemWidthHeight; x++) {
            for(u32 y = 0; y < save::EmblemWidthHeight; y++) {
                SetEmblemColorAt(g_SaveData.em.emblem_data, x, y, GetEmblemPixel(x, y));
            }
        }
    }

    void LoadSaveDataFields() {
        g_SaveData = {};
        if(ReadSaveData(g_SaveData, g_RawSaveData, true)) {
            ShowInformation("Savedata successfully decrypted and loaded ;)");
        }
        else {
            g_SaveData = {};
            ShowError("Unable to read savedata... is it a valid, encrypted MKDS savedata file?");
        }

        LoadEditFields();
    }

}

EM_JS(void, EncryptSaveSaveData, (void *data, const size_t size), {
    var data_array = Module.HEAPU8.subarray(data, data + size);
    var pom = document.createElement("a");
    pom.setAttribute("href", window.URL.createObjectURL(new Blob([data_array], {type: "octet/stream"})));
    pom.setAttribute("download", "mkds-savedata.sav");

    if(document.createEvent) {
        var event = document.createEvent("MouseEvents");
        event.initEvent("click", true, true);
        pom.dispatchEvent(event);
    }
    else {
        pom.click();
    }
});

extern "C" EMSCRIPTEN_KEEPALIVE void cpp_LoadSaveData(void *data, const size_t size) {
    if(g_RawSaveData != nullptr) {
        free(g_RawSaveData);
        g_RawSaveData = nullptr;
        g_RawSaveDataSize = 0;
    }

    g_SaveData = {};
    g_RawSaveData = data;
    g_RawSaveDataSize = size;
    g_PendingLoadRawSaveData = true;
}

EM_JS(void, LoadSaveData, (), {
    var input = document.createElement("input");
    input.type = "file";
    input.id = "file-selector";
    input.accept = ".sav";
    input.addEventListener("change", (event) => {
        var file = event.target.files[0];
        var reader = new FileReader();
        reader.addEventListener("load", () => {
            var file_data = new Uint8Array(reader.result);
            var file_ptr = Module._malloc(file_data.length);
            Module.HEAPU8.set(file_data, file_ptr);
            Module.ccall("cpp_LoadSaveData", null, ["number", "number"], [file_ptr, file_data.length]);
        }, false);
        reader.readAsArrayBuffer(file);
    });

    if(document.createEvent) {
        var event = document.createEvent("MouseEvents");
        event.initEvent("click", true, true);
        input.dispatchEvent(event);
    }
    else {
        input.click();
    }
});

namespace {

    void OnCanvasDimensionsChanged() {
        glfwSetWindowSize(g_Window, g_Width, g_Height);
        ImGui::SetCurrentContext(ImGui::GetCurrentContext());
    }

    void MainLoop() {
        const auto cur_width = GetCanvasWidth();
        const auto cur_height = GetCanvasHeight();
        if((cur_width != g_Width) || (cur_height != g_Height)) {
            g_Width = cur_width;
            g_Height = cur_height;
            OnCanvasDimensionsChanged();
        }

        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        const auto width = (u32)(g_Width * WindowWidthFactor);
        const auto height = (u32)(g_Height * WindowHeightFactor);
        ImGui::SetNextWindowPos(ImVec2((g_Width - width) / 2, (g_Height - height) / 2), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Once);

        ImGui::Begin("Mario Kart DS save editor", nullptr, ImGuiWindowFlags_MenuBar);

        if(g_PendingLoadRawSaveData) {
            LoadSaveDataFields();
            g_PendingLoadRawSaveData = false;
        }

        if(ImGui::BeginMenuBar()) {
            if(ImGui::BeginMenu("File")) {
                if(ImGui::MenuItem("Open")) {
                    LoadSaveData();
                }
                if(ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Open and load MKDS savedata file");
                }

                if(HasSaveDataLoaded()) {
                    if(ImGui::MenuItem("Reset")) {
                        LoadEditFields();
                    }
                    if(ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("Restore edited values from opened savedata");
                    }

                    if(ImGui::MenuItem("Save")) {
                        SaveEditFields();
                        WriteSaveData(g_SaveData, g_RawSaveData, true);
                        EncryptSaveSaveData(g_RawSaveData, g_RawSaveDataSize);
                    }
                    if(ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("Save current MKDS savedata");
                    }
                }

                ImGui::EndMenu();
            }

            ImGui::MenuItem("About", nullptr, &g_DisplayAboutWindow);
            if(ImGui::IsItemHovered()) {
                ImGui::SetTooltip("About this project");
            }

            ImGui::EndMenuBar();

            if(HasSaveDataLoaded()) {
                ImGui::InputText("Nickname##Header", g_EditHeaderNickname, sizeof(g_EditHeaderNickname));

                ImGui::Separator();

                if(ImGui::CollapsingHeader("Secret unlock flags")) {
                    ImGui::Indent();

                    if(ImGui::Button("Enable all")) {
                        for(u32 i = 0; i < SecretFlagCount; i++) {
                            g_EditSecretFlags[i] = true;
                        }
                    }
                    ImGui::SameLine();
                    if(ImGui::Button("Disable all")) {
                        for(u32 i = 0; i < SecretFlagCount; i++) {
                            g_EditSecretFlags[i] = false;
                        }
                    }

                    ImGui::Separator();

                    for(u32 i = 0; i < SecretFlagCount; i++) {
                        ImGui::Selectable(g_SecretFlagBitNames[i], &g_EditSecretFlags[i]);
                        if(ImGui::IsItemHovered()) {
                            ImGui::SetTooltip("Click to enable/disable");
                        }
                    }

                    ImGui::Unindent();
                }

                if(ImGui::CollapsingHeader("Grand Prix")) {
                    ImGui::Indent();

                    if(ImGui::BeginTabBar("GrandPrixTabs")) {
                        for(u32 cup = 0; cup < 8; cup++) {
                            if(ImGui::BeginTabItem((std::string(g_CupNames[cup]) + " cup").c_str())) {
                                for(u32 cc = 0; cc < 4; cc++) {
                                    if(cc > 0) {
                                        ImGui::Separator();
                                    }
                                    const auto cc_cup_prefix = std::to_string(cup) + "_" + std::to_string(cc);
                                    if(g_CupInfos[cup][cc].valid) {
                                        if(ImGui::CollapsingHeader(g_CcNames[cc])) {
                                            ImGui::Indent();

                                            ImGui::InputText(("Nickname##Gp" + cc_cup_prefix).c_str(), g_CupInfos[cup][cc].nickname, sizeof(g_CupInfos[cup][cc].nickname));
                                            ImGui::Combo(("Character##Gp" + cc_cup_prefix).c_str(), &g_CupInfos[cup][cc].character_id, g_CharacterNames, std::size(g_CharacterNames));
                                            ImGui::Combo(("Kart##Gp" + cc_cup_prefix).c_str(), &g_CupInfos[cup][cc].kart_id, g_KartNames, std::size(g_KartNames));
                                            ImGui::InputInt3(("Time (min:sec:milli)##Gp" + cc_cup_prefix).c_str(), &g_CupInfos[cup][cc].time_min);
                                            ImGui::Combo(("Result##Gp" + cc_cup_prefix).c_str(), &g_CupInfos[cup][cc].cup_res, g_CupResultNames, std::size(g_CupResultNames));
                                            ImGui::InputInt(("Points##Gp" + cc_cup_prefix).c_str(), &g_CupInfos[cup][cc].points);
                                            ImGui::Combo(("Rank##Gp" + cc_cup_prefix).c_str(), &g_CupInfos[cup][cc].rank, g_RankNames, std::size(g_RankNames) - 1);    

                                            ImGui::Unindent();
                                        }
                                        if(ImGui::Button(("Delete##Gp" + cc_cup_prefix).c_str())) {
                                            g_CupInfos[cup][cc].valid = false;
                                        }
                                    }
                                    else {
                                        ImGui::Text("No record for %s", g_CcNames[cc]);
                                        if(ImGui::Button(("Create##Gp" + cc_cup_prefix).c_str())) {
                                            g_CupInfos[cup][cc].valid = true;
                                        }
                                    }
                                }

                                ImGui::EndTabItem();
                            }
                        }

                        ImGui::EndTabBar();
                    }

                    ImGui::Unindent();
                }

                if(ImGui::CollapsingHeader("Time trials")) {
                    ImGui::Indent();

                    if(ImGui::BeginTabBar("TimeTrialTabs")) {
                        for(u32 cup = 0; cup < 8; cup++) {
                            if(ImGui::BeginTabItem((std::string(g_CupNames[cup]) + " cup").c_str())) {
                                for(u32 course = 0; course < 4; course++) {
                                    const auto course_id = course + cup * 4;

                                    #define _TT_DEFINE_FIELDS(r, label, no_arg_fmt, ...) { \
                                        if(g_TimeTrialRecords[course_id][r].valid) { \
                                            if(r > 0) { \
                                                ImGui::Separator(); \
                                            } \
                                            if(ImGui::CollapsingHeader(label)) { \
                                                ImGui::Indent(); \
                                                ImGui::InputText(("Nickname##Tt" + c_r_prefix).c_str(), g_TimeTrialRecords[course_id][r].nickname, sizeof(g_TimeTrialRecords[course_id][r].nickname)); \
                                                ImGui::Combo(("Character##Tt" + c_r_prefix).c_str(), &g_TimeTrialRecords[course_id][r].character_id, g_CharacterNames, std::size(g_CharacterNames)); \
                                                ImGui::Combo(("Kart##Tt" + c_r_prefix).c_str(), &g_TimeTrialRecords[course_id][r].kart_id, g_KartNames, std::size(g_KartNames)); \
                                                ImGui::InputInt3(("Time (min:sec:milli)##Tt" + c_r_prefix).c_str(), &g_TimeTrialRecords[course_id][r].time_min); \
                                                ImGui::Unindent(); \
                                            } \
                                            if(ImGui::Button(("Delete##Tt" + c_r_prefix).c_str())) { \
                                                g_TimeTrialRecords[course_id][r].valid = false; \
                                            } \
                                        } \
                                        else { \
                                            ImGui::Text(no_arg_fmt, ##__VA_ARGS__); \
                                            if(ImGui::Button(("Create##Tt" + c_r_prefix).c_str())) { \
                                                g_TimeTrialRecords[course_id][r].valid = true; \
                                            } \
                                        } \
                                    }

                                    if(ImGui::CollapsingHeader(g_CourseNames[course_id])) {
                                        ImGui::Indent();

                                        for(u32 r = 0; r < 5; r++) {
                                            const auto c_r_prefix = std::to_string(course_id) + "_" + std::to_string(r);
                                            _TT_DEFINE_FIELDS(r, ("Record " + std::to_string(r + 1)).c_str(), "No record %d...", r + 1);
                                        }

                                        const auto c_r_prefix = std::to_string(course_id) + "_bestlap";
                                        _TT_DEFINE_FIELDS(5, "Best lap", "No best lap record...");

                                        ImGui::Unindent();
                                    }
                                }

                                ImGui::EndTabItem();
                            }
                        }

                        ImGui::EndTabBar();
                    }

                    ImGui::Unindent();
                }
                
                if(ImGui::CollapsingHeader("Mission run")) {
                    ImGui::Indent();

                    if(ImGui::Button("Set all at max rank")) {
                        for(u32 l = 0; l < 7; l++) {
                            for(u32 s = 0; s < 9; s++) {
                                g_MissionRunStageInfos[l][s].beaten = true;
                                g_MissionRunStageInfos[l][s].rank = save::Rank_Star3;
                            }
                        }
                    }
                    ImGui::SameLine();
                    if(ImGui::Button("Set all unbeaten")) {
                        for(u32 l = 0; l < 7; l++) {
                            for(u32 s = 0; s < 9; s++) {
                                g_MissionRunStageInfos[l][s].beaten = false;
                            }
                        }
                    }

                    ImGui::Separator();

                    if(ImGui::BeginTabBar("MissionTabs")) {
                        for(u32 l = 0; l < 7; l++) {
                            if(ImGui::BeginTabItem(("Level " + std::to_string(l + 1)).c_str())) {
                                for(u32 s = 0; s < 9; s++) {
                                    if(s > 0) {
                                        ImGui::Separator();
                                    }

                                    const auto mr_prefix = std::to_string(l) + "_" + std::to_string(s);

                                    if(g_MissionRunStageInfos[l][s].beaten) {
                                        ImGui::Combo(("Stage " + std::to_string(l + 1) + "-" + std::to_string(s + 1) + " rank").c_str(), &g_MissionRunStageInfos[l][s].rank, g_RankNames, std::size(g_RankNames));
                                        if(ImGui::Button(("Delete##Mr" + mr_prefix).c_str())) {
                                            g_MissionRunStageInfos[l][s].beaten = false;
                                        }
                                    }
                                    else {
                                        ImGui::Text("Stage %d-%d is unbeaten...", l + 1, s + 1);
                                        if(ImGui::Button(("Create##Mr" + mr_prefix).c_str())) {
                                            g_MissionRunStageInfos[l][s].beaten = true;
                                        }
                                    }

                                    
                                }

                                ImGui::EndTabItem();
                            }
                        }

                        ImGui::EndTabBar();
                    }

                    ImGui::Unindent();
                }

                if(ImGui::CollapsingHeader("Edit emblem")) {
                    ImGui::Indent();

                    ImGui::Selectable(g_EditEmblemEnabled ? "Emblem enabled" : "Emblem disabled", &g_EditEmblemEnabled);
                    if(ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("Click to enable/disable");
                    }

                    ImGui::Separator();

                    ImGui::SliderInt("Grid spacing", &g_GridSpacing, 0, 10);
                    ImGui::SliderInt("Edit scale", &g_EmblemScale, 1, 100);

                    for(u32 i = 1; i < 16; i++) {
                        std::string button_label = "##ColorButton" + std::to_string(i);
                        if(g_PaintColor == i) {
                            button_label = "*";
                        }
                        if (i > 1) {
                            ImGui::SameLine();
                        }
                        if(ImGui::Button(button_label.c_str(), ImVec2(32, 32))) {
                            g_PaintColor = static_cast<save::EmblemColor>(i);
                        }

                        if(g_PaintColor == i) {
                            ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), g_EmblemColorTable[i]);
                        }
                        else {
                            const auto r_min = ImGui::GetItemRectMin();
                            const auto r_max = ImGui::GetItemRectMax();
                            const auto o_center = ImVec2(r_min.x + (r_max.x - r_min.x) / 2, r_min.y + (r_max.y - r_min.y) / 2);
                            ImGui::GetWindowDrawList()->AddCircleFilled(o_center, 14, g_EmblemColorTable[i]);
                        }
                    }

                    auto draw_list = ImGui::GetWindowDrawList();
                    const auto canvas_pos = ImGui::GetCursorScreenPos();
                    const auto canvas_size = ImGui::GetContentRegionAvail();

                    for(u32 x = 0; x < save::EmblemWidthHeight; x++) {
                        for(u32 y = 0; y < save::EmblemWidthHeight; y++) {
                            const auto clr = GetEmblemPixel(x, y);
                            const auto pos1 = ImVec2(canvas_pos.x + (x + 1) * g_GridSpacing + x * g_EmblemScale, canvas_pos.y + (y + 1) * g_GridSpacing + y * g_EmblemScale);
                            const auto pos2 = ImVec2(pos1.x + g_EmblemScale, pos1.y + g_EmblemScale);
                            draw_list->AddRectFilled(pos1, pos2, g_EmblemColorTable[static_cast<u32>(clr)]);
                        }
                    }

                    for(u32 i = 0; i <= save::EmblemWidthHeight; i++) {
                        const auto v_pos1 = ImVec2(canvas_pos.x + i * g_GridSpacing + i * g_EmblemScale, canvas_pos.y);
                        const auto v_pos2 = ImVec2(v_pos1.x + g_GridSpacing, v_pos1.y + (save::EmblemWidthHeight + 1) * g_GridSpacing + save::EmblemWidthHeight * g_EmblemScale);
                        draw_list->AddRectFilled(v_pos1, v_pos2, EmblemGridColor);

                        const auto h_pos1 = ImVec2(canvas_pos.x, canvas_pos.y + i * g_GridSpacing + i * g_EmblemScale);
                        const auto h_pos2 = ImVec2(h_pos1.x + (save::EmblemWidthHeight + 1) * g_GridSpacing + save::EmblemWidthHeight * g_EmblemScale, h_pos1.y + g_GridSpacing);
                        draw_list->AddRectFilled(h_pos1, h_pos2, EmblemGridColor);
                    }

                    const auto mouse_pos = ImGui::GetMousePos();
                    const bool is_in_canvas = ((mouse_pos.x >= canvas_pos.x) && (mouse_pos.x <= (canvas_pos.x + canvas_size.x)) && (mouse_pos.y >= canvas_pos.y) && (mouse_pos.y <= (canvas_pos.y + canvas_size.y)));

                    const auto click_l = ImGui::IsMouseDown(ImGuiMouseButton_Left);
                    const auto click_r = ImGui::IsMouseDown(ImGuiMouseButton_Right);
                    if(is_in_canvas && (click_l || click_r)) {
                        auto pos = ImVec2(mouse_pos.x - canvas_pos.x, mouse_pos.y - canvas_pos.y);

                        u32 x = 0;
                        u32 y = 0;
                        while((pos.x > 0) or (pos.y > 0)) {
                            if(pos.x > 0) {
                                pos.x -= g_GridSpacing + g_EmblemScale;
                                if(pos.x > 0) {
                                    x++;
                                }
                            }
                            if(pos.y > 0) {
                                pos.y -= g_GridSpacing + g_EmblemScale;
                                if(pos.y > 0) {
                                    y++;
                                }
                            }
                        }

                        if((x < save::EmblemWidthHeight) && (y < save::EmblemWidthHeight)) {
                            const auto clr = click_r ? save::EmblemColor_Transparent : g_PaintColor;
                            SetEmblemPixel(x, y, clr);
                        }
                    }

                    ImGui::SameLine();
                    const auto space_wh = save::EmblemWidthHeight * g_EmblemScale + (save::EmblemWidthHeight + 1) * g_GridSpacing + 5 * (g_EmblemScale + g_GridSpacing);
                    ImGui::InvisibleButton("##EmptySpace", ImVec2(space_wh, space_wh));

                    ImGui::Unindent();
                }
            }
            else {
                ImGui::Text("Open a MKDS savedata file to edit...");
            }
        }

        ImGui::End();

        if(g_DisplayAboutWindow) {
            ImGui::SetNextWindowSize(ImVec2(600, 250), ImGuiCond_Once);
            ImGui::Begin("About this save editor", &g_DisplayAboutWindow);

            if(ImGui::Button("GitHub")) {
                OpenUrl("https://github.com/XorTroll/mkds-re");
            }
            ImGui::SameLine();
            ImGui::TextWrapped("part of my Mario Kart DS reverse-engineering work");

            ImGui::Separator();

            ImGui::TextWrapped("C++20 (%ld), clang v" __clang_version__, __cplusplus);

            ImGui::Separator();

            const auto em_ver = (char*)emscripten_get_compiler_setting("EMSCRIPTEN_VERSION");
            if(ImGui::Button("emscripten")) {
                OpenUrl("https://emscripten.org/");
            }
            ImGui::SameLine();
            ImGui::TextWrapped("v%s", em_ver);

            ImGui::Separator();

            if(ImGui::Button("ImGui")) {
                OpenUrl("https://github.com/ocornut/imgui");
            }
            ImGui::SameLine();
            ImGui::TextWrapped("v" IMGUI_VERSION " (%d)", IMGUI_VERSION_NUM);

            ImGui::Separator();

            if(ImGui::Button("FiraCode")) {
                OpenUrl("https://github.com/tonsky/FiraCode");
            }

            ImGui::End();
        }

        ImGui::Render();

        glfwMakeContextCurrent(g_Window);
        int display_w, display_h;
        glfwGetFramebufferSize(g_Window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(ClearColor.x, ClearColor.y, ClearColor.z, ClearColor.w);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwMakeContextCurrent(g_Window);
    }

    int InitializeGlfw() {
        if(glfwInit() != GLFW_TRUE) {
            ShowError("Failed to initialize GLFW!");
            return 1;
        }

        // We don't want the old OpenGL
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Open a window and create its OpenGL context
        g_Window = glfwCreateWindow(g_Width, g_Height, "WebGui Demo", nullptr, nullptr);
        if(g_Window == nullptr) {
            ShowError("Failed to open GLFW window!");
            glfwTerminate();
            return -1;
        }

        // Initialize GLFW
        glfwMakeContextCurrent(g_Window);
        return 0;
    }

    constexpr ImWchar ImGuiGlyphRanges[] = {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x0370, 0x03FF, // Greek and Coptic
        0x2100, 0x214F, // Letterlike Symbols (https://codepoints.net/letterlike_symbols)
        0x0000
    };

    int InitializeImGui() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(g_Window, true);
        ImGui_ImplOpenGL3_Init();

        ImGui::StyleColorsDark();

        auto &io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF("assets/FiraCode-Regular.ttf", 18.0f, nullptr, ImGuiGlyphRanges);
        io.Fonts->AddFontFromFileTTF("assets/FiraCode-Regular.ttf", 26.0f, nullptr, ImGuiGlyphRanges);
        io.Fonts->AddFontDefault();

        ResizeCanvas();

        return 0;
    }

    #define _RES_TRY(expr) { \
        const auto tmp_res = (expr); \
        if(tmp_res != 0) { \
            return tmp_res; \
        } \
    }

    int Initialize() {
        srand(time(nullptr));

        g_Width = GetCanvasWidth();
        g_Height = GetCanvasHeight();

        _RES_TRY(InitializeGlfw());
        _RES_TRY(InitializeImGui());

        return 0;
    }

    void Finalize() {
        glfwTerminate();
    }

}

extern "C" int main(int argc, char **argv) {
    if(Initialize() != 0) {
        ShowError("Failed to initialize!");
        return 1;
    }

    emscripten_set_main_loop(MainLoop, 0, 1);

    Finalize();
    return 0;
}
