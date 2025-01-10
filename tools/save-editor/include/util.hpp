
#pragma once
#include <string>
#include <locale>
#include <codecvt>

inline std::string ConvertFromUtf16(const std::u16string &u_str) {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    return convert.to_bytes(u_str);
}

inline std::u16string ConvertToUtf16(const std::string &str) {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    return convert.from_bytes(str);
}
