
#pragma once
#include <mk/save/save_Base.hpp>

namespace mk::save {

    struct SaveData {
        SaveDataHeader header;
        SaveDataEmblem em;
        SaveDataGrandPrix gp;
        SaveDataTimeTrial tt;
        SaveDataMissionRun mr;
        SaveDataFriendList fl;
    };

    bool ReadSaveData(SaveData &out_data, const void *save_ptr, const bool is_enc);
    void WriteSaveData(SaveData &data, void *save_ptr, const bool is_enc);


}
