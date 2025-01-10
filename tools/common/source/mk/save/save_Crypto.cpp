#include <mk/save/save_Crypto.hpp>

namespace mk::save {

    namespace {

        // Straight from reverse-engineered MKDS code

        u16 g_CRCConst = 0x1021;

        u32 g_Table0[29] = {
            0xB6F2BA0C, 0x1A88BEF7, 0x0DDC276B, 0xB000F810, 0x6D254A35,
            0x4C69572C, 0x0E49CFC2, 0xA4391D94, 0x98E199E1, 0x10F88349,
            0xA7CCE380, 0xF4287C11, 0x93857987, 0xF1F1171F, 0x3487B1FA,
            0x65B88727, 0xA1711A9C, 0x2338B1FE, 0xF742B4DF, 0xA33235EC,
            0x5350D303, 0xC9C1BE55, 0xC4FD2E1D, 0x8039E1DB, 0xB6386F71,
            0x34CBB389, 0x1224001A, 0x59CDEA11, 0x5371F238
        };
        constexpr auto Table0Size = std::size(g_Table0);

        u32 g_TableA[19] = {
            0x83F3E485,
            0x1F854511,
            0x33FAE87B,
            0xA6949A1E,
            0xDB1DA3C5,
            0xD5986A2F,
            0xAF6CC662,
            0xFB04BD0F,
            0x573F3084,
            0xDFB9A74F,
            0xF60A05B6,
            0xDF57E7C7,
            0x11723659,
            0x8EAE9859,
            0xE08EB664,
            0xD310802A,
            0x498B2FA5,
            0x555452F1,
            0xE5575C26
        };
        constexpr auto TableASize = std::size(g_TableA);

        u32 g_TableB[11] = {
            0xC0A2CFB9,
            0xFCA4F59A,
            0xD87E2588,
            0xB615DD30,
            0xEACC91EE,
            0xB6A8F95D,
            0x7BD8B080,
            0xCEC11555,
            0x59CEF8CE,
            0xFFF88DB3,
            0xA867F2A3
        };
        constexpr auto TableBSize = std::size(g_TableB);

        u32 g_TableC[17] = {
            0x54E85EA2,
            0xDE7CE656,
            0xD25261B4,
            0x401D1EC4,
            0x220DECE2,
            0x1F0CF6F4,
            0x7176B2A6,
            0x58D602EC,
            0x859D5B49,
            0xE5258B0C,
            0xB31A2910,
            0x2DDE04B1,
            0xEEEFA031,
            0x5B852490,
            0x44AD6965,
            0x8E3206A5,
            0x5E22B868
        };
        constexpr auto TableCSize = std::size(g_TableC);

        #define ENCDEC32(val, i0, iA, iB, iC) (val ^ g_Table0[i0] + g_TableA[iA] + g_TableB[iB] + g_TableC[iC])

    }

    size_t GetMaxIdx0Value() {
        return Table0Size;
    }

    u32 FindTable0IndexForValue(u32 enc_val, u32 dec_val) {
        u32 i = 0;
        do {
            if(dec_val == ENCDEC32(enc_val, i, 0, 0, 0)) {
                return i;
            }

            i = (i + 1) & 0xFFFF;
        } while (i < Table0Size);
        return 0xFFFFFFFF;
    }

    void Encrypt(u32 *data, const size_t data_len, const u32 rand_idx0) {
        u32 idxA = 0;
        u32 idxC = 0;
        u32 idxB = 0;

        u32 idx0 = FindTable0IndexForValue(ENCDEC32(*data, rand_idx0, 0, 0, 0), *data);

        u32 *data_end = (u32*)((char *)data + data_len);
        do {
            *data = ENCDEC32(*data, idx0, idxA, idxB, idxC); data++;
            idxB = (idxB + 1) & 0xFFFF;
            if(idxB >= TableBSize) {
                idxB = 0;
                idxC = (idxC + 1) & 0xFFFF;
                if(idxC >= TableCSize) {
                    idxC = 0;
                    idxA = (idxA + 1) & 0xFFFF;
                    if(idxA >= TableASize) {
                        idxA = 0;
                        idx0 = (idx0 + 1) & 0xFFFF;
                        if(idx0 >= Table0Size) {
                            idx0 = 0;
                        }
                    }
                }
            }
        } while(data < data_end);
    }

    bool Decrypt(u32 *data, const size_t data_len, const u32 magic) {
        u32 idxA = 0;
        u32 idxC = 0;
        u32 idxB = 0;

        u32 idx0 = FindTable0IndexForValue(*data, magic);
        if(idx0 == 0xFFFFFFFF) {
            return false;
        }

        idx0 = idx0 & 0xFFFF;
        u32 *data_end = (u32 *)((uintptr_t)data + data_len);
        do {
            *data = ENCDEC32(*data, idx0, idxA, idxB, idxC); data++;
        
            idxB = (idxB + 1) & 0xFFFF;
            if(TableBSize <= idxB) {
                idxB = 0;
                idxC = (idxC + 1) & 0xFFFF;
                if(TableCSize <= idxC) {
                    idxC = 0;
                    idxA = (idxA + 1) & 0xFFFF;
                    if(TableASize <= idxA) {
                        idxA = 0;
                        idx0 = (idx0 + 1) & 0xFFFF;
                        if(Table0Size <= idx0) {
                            idx0 = 0;
                        }
                    }
                }
            }
        } while(data < data_end);
        return true;
    }

    u32 ComputeCrc(void *ptr, const size_t size) {
        u32 cur_val = 0;
        u32 off = 0;
        auto ptr8 = reinterpret_cast<u8*>(ptr);
        if(size > 0) {
            do {
                auto cur_ptr_val = *ptr8;
                for(u32 i = 0; i < 8; i++) {
                    if((cur_val & 0x8000) != 0) {
                        cur_val = ((2 * cur_val) & 0xFFFF) ^ g_CRCConst;
                    }
                    else {
                        cur_val = (2 * cur_val) & 0xFFFF;
                    }
                    
                    if((cur_ptr_val & 0x80) != 0) {
                        cur_val = (cur_val & 0xFFFF) ^ 1;
                    }

                    cur_ptr_val *= 2;
                }
                ++ptr8;
                off++;
            } while(off < size);
        }
        return cur_val;
    }

    ////////////////////////////

    bool ReadDecryptSection(const void *base_ptr, void *out_ptr, const size_t section_offset, const size_t section_size, const bool skip_8bytes, const u32 section_magic) {
        memcpy(out_ptr, reinterpret_cast<const u8*>(base_ptr) + section_offset, section_size);

        auto dec_ptr = reinterpret_cast<u32*>(out_ptr);
        auto dec_size = section_size;
        if(skip_8bytes) {
            dec_ptr += 2;
            dec_size -= 2 * sizeof(u32);
        }

        return Decrypt(dec_ptr, dec_size, section_magic);
    }

    void WriteEncryptSection(void *base_ptr, const void *in_ptr, const size_t section_offset, const size_t section_size, const bool skip_8bytes, const u32 section_magic, const u32 rand_idx0) {
        auto enc_ptr = reinterpret_cast<u32*>(reinterpret_cast<u8*>(base_ptr) + section_offset);
        memcpy(enc_ptr, in_ptr, section_size);

        auto enc_size = section_size;
        if(skip_8bytes) {
            enc_ptr += 2;
            enc_size -= 2 * sizeof(u32);
        }

        Encrypt(enc_ptr, enc_size, rand_idx0);
    }

}
