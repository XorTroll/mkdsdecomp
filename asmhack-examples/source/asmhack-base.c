#include <string.h>

void IntToBinaryString(int value, char *out_str, size_t size) {
    if (size < sizeof(int) * 8 + 1) { // Ensure the out_str is large enough
        if (size > 0) {
            out_str[0] = '\0'; // Null-terminate in case of insufficient out_str
        }
        return;
    }

    out_str[size - 1] = '\0'; // Ensure the last character is null-terminated
    size_t pos = size - 2;   // Start filling from the end

    for (int i = 0; i < sizeof(int) * 8; i++) {
        out_str[pos--] = (value & 1) ? '1' : '0';
        value >>= 1;
    }

    // Shift the out_str to skip leading zeros
    char *start = out_str + pos + 1;
    memmove(out_str, start, strlen(start) + 1);
}
