#include <args.hxx>
#include <mk/gfx/gfx_Color.hpp>
#include <mk/save/save_Format.hpp>
#include <cstdio>
#include <bitset>

using namespace mk;

namespace {

    struct RgbColor {
        u8 r;
        u8 g;
        u8 b;
    };

    bool ReadFromFile(const std::string &path, u8 *&out_buf, size_t &out_buf_size) {
        auto f = fopen(path.c_str(), "rb");
        if(f) {
            fseek(f, 0, SEEK_END);
            const auto f_size = ftell(f);
            rewind(f);

            auto file_ptr = new u8[f_size]();

            if(fread(file_ptr, f_size, 1, f) != 1) {
                delete[] file_ptr;
                fclose(f);
                return false;
            }

            out_buf = file_ptr;
            out_buf_size = f_size;

            fclose(f);
            return true;
        }
        else {
            return false;
        }
    }

    bool WriteToFile(const std::string &path, const void *buf, const size_t buf_size) {
        auto f = fopen(path.c_str(), "wb");
        if(f) {
            if(fwrite(buf, buf_size, 1, f) != 1) {
                fclose(f);
                return false;
            }

            fclose(f);
            return true;
        }
        else {
            return false;
        }
    }

    void ConvertToRgb(const gfx::xbgr1555::Color &clr, RgbColor &out_clr) {
        out_clr.r = (int)(((float)clr.clr.r / 31.0) * 255.0) & 0xFF;
        out_clr.g = (int)(((float)clr.clr.g / 31.0) * 255.0) & 0xFF;
        out_clr.b = (int)(((float)clr.clr.b / 31.0) * 255.0) & 0xFF;
    }

    void ConvertFromRgb(const RgbColor &clr, gfx::xbgr1555::Color &out_clr) {
        out_clr.clr.r = (int)(((float)clr.r / 255.0) * 31.0);
        out_clr.clr.g = (int)(((float)clr.g / 255.0) * 31.0);
        out_clr.clr.b = (int)(((float)clr.b / 255.0) * 31.0);
    }

    bool ReadSaveDataFrom(const std::string &save_path, const bool is_enc, mk::save::SaveData &out_save_data, size_t &out_save_data_size) {
        u8 *enc_save_data;
        size_t save_data_size;
        if(!ReadFromFile(save_path, enc_save_data, save_data_size)) {
            std::cerr << "Unable to read input encrypted save data..." << std::endl;
            return false;
        }

        if(!save::ReadSaveData(out_save_data, enc_save_data, is_enc)) {
            std::cerr << "Unable to parse save data..." << std::endl;
            delete[] enc_save_data;
            return false;
        }

        out_save_data_size = save_data_size;
        delete[] enc_save_data;
        return true;
    }

    void PrintSaveData(const std::string &dec_save_path) {
        mk::save::SaveData save_data;
        size_t dummy_size;
        if(!ReadSaveDataFrom(dec_save_path, false, save_data, dummy_size)) {
            return;
        }

        std::cout << "> Header:" << std::endl;

        // TODO: print more savedata fields (I only needed these so far)

        std::cout << ">>> Profile info:" << std::endl;

        #define _PRINT_FRIEND_CODE(i, code) { \
            std::cout << ">>>>> Friend code " #i ":" << std::endl; \
            std::cout << "------- Unknown + flags: " << std::bitset<32>(code.unk_and_flags) << std::endl; \
            std::cout << "------- Unknown value: " << std::hex << (code.unk_and_flags & 0x1FF) << std::dec << std::endl; \
            const auto flags = ((code.unk_and_flags >> 11) & 0x1FFFFF); \
            std::cout << "------- Flags: " << std::bitset<21>(flags) << std::endl; \
            std::cout << "------- State: " << (flags & 3) << std::endl; \
            std::cout << "------- Profile ID: " << std::hex << code.profile_id << std::dec << std::endl; \
            std::cout << "------- CRC checksum: " << std::hex << code.crc_checksum << std::dec << std::endl; \
            std::cout << ">>>>>" << std::endl; \
        }

        _PRINT_FRIEND_CODE(1, save_data.header.profile_info.friend_code_1);
        _PRINT_FRIEND_CODE(2, save_data.header.profile_info.friend_code_2);

        std::cout << "----- Flags: " << std::bitset<8>(save_data.header.profile_info.flags) << std::endl;

        std::cout << "----- Profile ID: " << std::hex << save_data.header.profile_info.profile_id << std::dec << std::endl;

        std::string game_id_str((char*)&save_data.header.profile_info.game_id, 4);
        std::cout << "----- Game ID: " << game_id_str << std::endl;

        std::cout << "----- unk_x28: " << std::hex << save_data.header.profile_info.unk_x28 << std::dec << std::endl;
        std::cout << "----- unk_x2C: " << std::hex << save_data.header.profile_info.unk_x2C << std::dec << std::endl;
        std::cout << "----- unk_x30: " << std::hex << save_data.header.profile_info.unk_x30 << std::dec << std::endl;
        std::cout << "----- unk_x34: " << std::hex << save_data.header.profile_info.unk_x34 << std::dec << std::endl;
        std::cout << "----- unk_x38: " << std::hex << save_data.header.profile_info.unk_x38 << std::dec << std::endl;
    }

    void DecryptSaveData(const std::string &enc_save_path, const std::string &dec_save_path) {
        mk::save::SaveData save_data;
        size_t save_data_size;
        if(!ReadSaveDataFrom(enc_save_path, true, save_data, save_data_size)) {
            return;
        }

        auto dec_save_data = new u8[save_data_size]();
        save::WriteSaveData(save_data, dec_save_data, false);

        if(!WriteToFile(dec_save_path, dec_save_data, save_data_size)) {
            std::cerr << "Unable to write decrypted save data..." << std::endl;
            delete[] dec_save_data;
            return;
        }

        delete[] dec_save_data;
    }

    void EncryptSaveData(const std::string &dec_save_path, const std::string &enc_save_path) {
        mk::save::SaveData save_data;
        size_t save_data_size;
        if(!ReadSaveDataFrom(dec_save_path, false, save_data, save_data_size)) {
            return;
        }

        auto enc_save_data = new u8[save_data_size]();
        save::WriteSaveData(save_data, enc_save_data, true);

        if(!WriteToFile(dec_save_path, enc_save_data, save_data_size)) {
            std::cerr << "Unable to write encrypted save data..." << std::endl;
            delete[] enc_save_data;
            return;
        }

        delete[] enc_save_data;
    }

}

int main(int argc, char **argv) {
    args::ArgumentParser parser("Various MKDS-related utilities");
    args::HelpFlag help(parser, "help", "Displays this help menu", {'h', "help"});

    args::Group commands(parser, "Commands:", args::Group::Validators::Xor);

    args::Command clr(commands, "clr", "Converts a color between formats (XBGR-1555 and RGB-888)");
    args::Group clr_required(clr, "", args::Group::Validators::All);
    args::ValueFlag<std::string> clr_color(clr_required, "color", "Input color", {'i', "in"});

    args::Command save_info(commands, "save-info", "Prints save data information");
    args::Group save_info_required(save_info, "", args::Group::Validators::All);
    args::ValueFlag<std::string> save_info_dec_save_file(save_info_required, "dec_save_file", "Input decrypted save file", {'i', "in"});

    args::Command save_dec(commands, "save-dec", "Decrypt save data");
    args::Group save_dec_required(save_dec, "", args::Group::Validators::All);
    args::ValueFlag<std::string> save_dec_enc_save_file(save_dec_required, "enc_save_file", "Input encrypted save file", {'i', "in"});
    args::ValueFlag<std::string> save_dec_dec_save_file(save_dec_required, "dec_save_file", "Output decrypted save file", {'o', "out"});

    args::Command save_enc(commands, "save-enc", "Encrypt save data");
    args::Group save_enc_required(save_enc, "", args::Group::Validators::All);
    args::ValueFlag<std::string> save_enc_dec_save_file(save_enc_required, "dec_save_file", "Input decrypted save file", {'i', "in"});
    args::ValueFlag<std::string> save_enc_enc_save_file(save_enc_required, "enc_save_file", "Output encrypted save file", {'o', "out"});

    try {
        parser.ParseCLI(argc, argv);
    }
    catch(std::exception &e) {
        std::cerr << parser;
        std::cout << e.what() << std::endl;
        return 1;
    }

    if(clr) {
        const auto color = clr_color.Get();

        // TODO: color parsing and conversion
    }
    else if(save_info) {
        const auto dec_save_file = save_info_dec_save_file.Get();

        PrintSaveData(dec_save_file);
    }
    else if(save_dec) {
        const auto enc_save_file = save_dec_enc_save_file.Get();
        const auto dec_save_file = save_dec_dec_save_file.Get();

        DecryptSaveData(enc_save_file, dec_save_file);
    }
    else if(save_enc) {
        const auto dec_save_file = save_enc_dec_save_file.Get();
        const auto enc_save_file = save_enc_enc_save_file.Get();

        EncryptSaveData(dec_save_file, enc_save_file);
    }

    return 0;
}
