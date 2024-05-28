
#pragma once
#include <save_base.hpp>

size_t GetMaxIdx0Value();

void Encrypt(u32 *data, const size_t data_len, const u32 rand_idx0);
bool Decrypt(u32 *data, const size_t data_len, const u32 magic);

u32 ComputeCRC(void *ptr, const size_t size);

bool ReadDecryptSection(const void *base_ptr, void *out_ptr, const size_t section_offset, const size_t section_size, const bool skip_8bytes, const u32 section_magic);
void WriteEncryptSection(void *base_ptr, const void *in_ptr, const size_t section_offset, const size_t section_size, const bool skip_8bytes, const u32 section_magic, const u32 rand_idx0);
