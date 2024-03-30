#ifndef _NTR_FS_H
#define _NTR_FS_H

#include "base.h"
#include "os/os_thread.h"

typedef enum Fs_FileFlag {
    Fs_FileFlag_TODO = 0
    // TODO
} Fs_FileFlag;

typedef enum Fs_ArchiveFlag {
    Fs_ArchiveFlag_Registered = 1,
    Fs_ArchiveFlag_Loaded = 2
    // TODO
} Fs_ArchiveFlag;

typedef enum Fs_OperationId {
    Fs_OperationId_Invalid = 14
    // TODO
} Fs_OperationId;

typedef enum Fs_SeekWhence {
    Fs_SeekWhence_Start = 0,
    Fs_SeekWhence_Current = 1,
    Fs_SeekWhence_End = 2
} Fs_SeekWhence;

typedef struct {
    Fs_Archive *arc;
    u16 unk1;
    u16 unk2;
    u32 unk3;
} Fs_DirectoryPosition;

struct Fs_Archive;
struct Fs_File;

typedef void(*Fs_ArchiveReadFn)(Fs_Archive *arc, void *ptr, void *offset, size_t size);
typedef void(*Fs_ArchiveWriteFn)(Fs_Archive *arc, void *ptr, void *offset, size_t size);
typedef void(*Fs_ArchiveFatFntFn)(Fs_Archive *arc, void *ptr, void *offset, size_t size);
typedef void(*Fs_ArchiveExtraFileOperationHandlerFn)(Fs_File *file, u32 op_id);

typedef struct {
    Fs_Archive *arc;
    u32 fat_entry_idx;
} Fs_FileId;

typedef struct {
    Fs_File *prev_file;
    Fs_File *next_file;
} Fs_FileList;

typedef struct {
    union {
        char name[4];
        u32 name_packed;
    };
    Fs_Archive *next_arc;
    Fs_Archive *prev_arc;
    Os_ThreadQueue sync_queue;
    Os_ThreadQueue status_queue;
    u32 flags;
    Fs_FileList file_list;
    void *fimg_data_start;
    void *fat_start_ptr_rel;
    size_t fat_size;
    void *fnt_start_ptr_rel;
    size_t fnt_size;
    void *fat_start_ptr_rel_bak;
    void *fnt_start_ptr_rel_bak;
    void *used_read_buf;
    Fs_ArchiveReadFn read_fn;
    Fs_ArchiveWriteFn write_fn;
    Fs_ArchiveFatFntFn fat_fnt_fn;
    Fs_ArchiveExtraFileOperationHandlerFn extra_op_handler_fn;
    u32 extra_op_mask;
} Fs_Archive;

typedef struct {
    Fs_FileList file_list;
    Fs_Archive *arc;
    u32 flags;
    u32 op_id;
    u32 op_res;
    Os_ThreadQueue op_thr_list;

    union {
        struct {
            u32 fat_entry_idx;
            size_t start_offset;
            size_t end_offset;
            size_t cur_offset;
        } file;
        struct {
            Fs_DirectoryPosition pos;
            u32 unk;
        } dir;
    } info;
    
    union {
        struct {
            const void *read_ptr;
            size_t s1;
            size_t s2;
        } read;
        struct {
            void *write_ptr;
            size_t s1;
            size_t s2;
        } write;
        // TODO: all
        struct {
            Fs_DirectoryPosition dir_pos;
            const char *path;
            bool is_dir; // u32
            union {
                Fs_DirectoryPosition *dir;
                Fs_FileId *file;
            } out;
        } find_path;
    } op_arg;
} Fs_File;

inline void Fs_FileList_Initialize(Fs_FileList *list) {
    list->prev_file = NULL;
    list->next_file = NULL;
}

void Fs_Archive_Initialize(Fs_Archive *arc);
bool Fs_Archive_LoadFromFatFnt(Fs_Archive *arc, void *fimg_data_start, void *fat_start_ptr_rel, size_t fat_size, void *fnt_start_ptr_rel, size_t fnt_size, Fs_ArchiveReadFn read_fn, Fs_ArchiveWriteFn write_fn);
void Fs_Archive_SetExtraFileOperationHandler(Fs_Archive *arc, Fs_ArchiveExtraFileOperationHandlerFn handler_fn, u32 extra_op_mask);

void Fs_File_Initialize(Fs_File *file);
bool Fs_File_Open(Fs_File *file, const char *path);
bool Fs_File_Seek(Fs_File *file, ssize_t offset, Fs_SeekWhence whence);

void Fs_Initialize(u32 default_dma_channel);
bool Fs_IsInitialized(void);
bool Fs_Chdir(const char *path);
bool Fs_RegisterArchive(Fs_Archive *arc, const char *name, size_t name_len);
void Fs_UnregisterArchive(Fs_Archive *arc);

#endif