#include <ntr/fs.h>
#include <ntr/os.h>
#include <ntr/mem.h>
#include <ntr/bios.h>

static bool g_Initialized = false;
static Fs_Archive g_RomArchive;
static u32 g_CardDmaChannel;

static Fs_Archive *g_ArchiveList = NULL;
static Fs_DirectoryPosition g_CurrentDirectoryPosition;

#define _NTR_FS_MAX_ARCHIVE_NAME_LEN 3

#define _NTR_FS_CHAR_IS_SLASH(ch) (((ch) == '/') || ((ch) == '\\'))

u32 Fs_PackArchiveMountName(const char *name, size_t name_len) {
    if((name_len == 0) || (name_len > _NTR_FS_MAX_ARCHIVE_NAME_LEN)) {
        return 0;
    }

    u32 packed_name = 0;
    u32 i = 0;
    u32 shift = 0;
    do {
        if(name[i] == '\0') {
            break;
        }

        u32 value = (u32)name[i];
        if((value - 0x41) <= 0x19) {
            value += 0x20;
        }
        packed_name |= value << shift;
        shift += 8;
        i++;
    } while(i < name_len);
    return packed_name;
}

Fs_Archive *Fs_FindArchiveByMountName(const char *name, size_t name_len) {
    u32 name_packed = Fs_PackArchiveMountName(name, name_len);

    u32 old_state = Os_DisableIRQ();
    Fs_Archive *cur_arc;
    for(cur_arc = g_ArchiveList; (cur_arc != NULL) && (cur_arc->name_packed != name_packed); cur_arc = cur_arc->next_arc);
    Os_RestoreIRQ(old_state);
    return cur_arc;
}

bool Fs_File_FindPath(Fs_File *file, const char *path, Fs_FileId *out_file_id, Fs_DirectoryPosition *out_dir_pos) {
    const char *cur_path = path;
    Fs_DirectoryPosition dir_pos;
    dir_pos.arc = g_CurrentDirectoryPosition.arc;
    if(_NTR_FS_CHAR_IS_SLASH(*path)) {
        cur_path++;
        dir_pos.unk1 = 0;
        dir_pos.unk2 = 0;
        dir_pos.unk3 = 0;
    }
    else {
        dir_pos.unk1 = g_CurrentDirectoryPosition.unk1;
        dir_pos.unk2 = g_CurrentDirectoryPosition.unk2;
        dir_pos.unk3 = g_CurrentDirectoryPosition.unk3;

        for(u32 i = 0; i <= _NTR_FS_MAX_ARCHIVE_NAME_LEN; i++) {
            if((cur_path[i] == '\0') || _NTR_FS_CHAR_IS_SLASH(cur_path[i])) {
                break;
            }

            if(cur_path[i] == ':') {
                Fs_Archive *mount_arc = Fs_FindArchiveByMountName(cur_path, i);
                if(mount_arc == NULL) {
                    return false;
                }
                if(mount_arc->flags & Fs_ArchiveFlag_Loaded) {
                    return false;
                }

                dir_pos.arc = mount_arc;
                dir_pos.unk1 = 0;
                dir_pos.unk2 = 0;
                dir_pos.unk3 = 0;
                
                char after_name_ch = cur_path[i + 1];
                cur_path += i + 1;
                if(_NTR_FS_CHAR_IS_SLASH(after_name_ch)) {
                    cur_path++;
                }
                break;
            }
        }
    }

    file->arc = dir_pos.arc;
    file->op_arg.find_path.path = cur_path;
    file->op_arg.find_path.dir_pos = dir_pos;
    if(out_dir_pos != NULL) {
        file->op_arg.find_path.is_dir = true;
        file->op_arg.find_path.out.dir = out_dir_pos;
    }
    else {
        file->op_arg.find_path.is_dir = false;
        file->op_arg.find_path.out.file = out_file_id;
    }
    // TODO: fs sendoperation findpath
}

bool Fs_FileId_LoadFromPath(Fs_FileId *out_id, const char *path) {
    Fs_File file;
    Fs_File_Initialize(&file);
    return Fs_File_FindPath(&file, path, out_id, NULL);
}

void Fs_InitializeMountRom(u32 default_dma_channel) {
    g_CardDmaChannel = default_dma_channel;
    // TODO: card lock id, card init

    const char *mount_name = "rom";
    size_t mount_name_len = __builtin_strlen(mount_name);
    Fs_Archive_Initialize(&g_RomArchive);
    Fs_RegisterArchive(&g_RomArchive, mount_name, mount_name_len);

    if(NTR_BIOS_BOOT_INDICATOR == Bios_BootIndicatorType_DownloadPlay) {
        // TODO
    }
    else {
        // TODO
    }
}

bool Fs_Archive_DefaultRead(Fs_Archive *arc, void *ptr, uintptr_t offset, size_t size) {
    Mem_Copy8(arc->fimg_data_start + offset, ptr, size);
    return true;
}

bool Fs_Archive_DefaultWrite(Fs_Archive *arc, void *ptr, uintptr_t offset, size_t size) {
    Mem_Copy8(ptr, arc->fimg_data_start + offset, size);
    return true;
}

////////////

void Fs_Archive_Initialize(Fs_Archive *arc) {
    Util_FillMemory8(arc, 0, sizeof(Fs_Archive));

    Os_ThreadQueue_Initialize(&arc->sync_queue);
    Os_ThreadQueue_Initialize(&arc->status_queue);
}

void Fs_Archive_SetExtraFileOperationHandler(Fs_Archive *arc, Fs_ArchiveExtraFileOperationHandlerFn handler_fn, u32 extra_op_mask) {
    Fs_ArchiveExtraFileOperationHandlerFn new_handler_fn = handler_fn;
    u32 new_extra_op_mask = extra_op_mask;

    if(extra_op_mask != 0) {
        if(handler_fn == NULL) {
            new_extra_op_mask = 0;
        }
    }
    else {
        // Just remove the extra operation handler
        new_handler_fn = NULL;
    }

    arc->extra_op_handler_fn = new_handler_fn;
    arc->extra_op_mask = new_extra_op_mask;
}

bool Fs_Archive_LoadFromFatFnt(Fs_Archive *arc, void *fimg_data_start, void *fat_start_ptr_rel, size_t fat_size, void *fnt_start_ptr_rel, size_t fnt_size, Fs_ArchiveReadFn read_fn, Fs_ArchiveWriteFn write_fn) {
    arc->fimg_data_start = fimg_data_start;
    arc->fat_size = fat_size;
    arc->fat_start_ptr_rel = fat_start_ptr_rel;
    arc->fat_start_ptr_rel_bak = fat_start_ptr_rel;
    arc->fnt_size = fnt_size;
    arc->fnt_start_ptr_rel = fnt_start_ptr_rel;
    arc->fnt_start_ptr_rel_bak = fnt_start_ptr_rel;

    Fs_ArchiveReadFn arc_read_fn = read_fn;
    if(arc_read_fn == NULL) {
        arc_read_fn = Fs_Archive_DefaultRead;
    }
    arc->read_fn = arc_read_fn;

    Fs_ArchiveWriteFn arc_write_fn = write_fn;
    if(arc_write_fn == NULL) {
        arc_write_fn = Fs_Archive_DefaultWrite;
    }
    arc->write_fn = arc_write_fn;

    arc->fat_fnt_fn = arc_read_fn;

    arc->used_read_buf = NULL;
    arc->flags |= Fs_ArchiveFlag_Loaded;
    return true;
}

void Fs_File_Initialize(Fs_File *file) {
    Fs_FileList_Initialize(&file->file_list);
    Os_ThreadQueue_Initialize(&file->op_thr_list);

    file->arc = NULL;
    file->op_id = Fs_OperationId_Invalid;
    file->flags = 0;
}

bool Fs_File_Open(Fs_File *file, const char *path) {
    Fs_FileId file_id;
    return Fs_FileId_LoadFromPath(&file_id, path) && true; // file openbyid...
}

bool Fs_File_Seek(Fs_File *file, ssize_t offset, Fs_SeekWhence whence) {
    ssize_t new_offset;
    if(whence == Fs_SeekWhence_Start) {
        new_offset = offset + file->info.file.start_offset;
    }
    else if(whence == Fs_SeekWhence_Current) {
        new_offset = offset + file->info.file.cur_offset;
    }
    else if(whence == Fs_SeekWhence_End) {
        new_offset = offset + file->info.file.end_offset;
    }
    else {
        return false;
    }

    if(new_offset < file->info.file.start_offset) {
        new_offset = file->info.file.start_offset;
    }
    if(new_offset > file->info.file.end_offset) {
        new_offset = file->info.file.end_offset;
    }

    file->info.file.cur_offset = new_offset;
    return true;
}

void Fs_Initialize(u32 default_dma_channel) {
    if(!g_Initialized) {
        g_Initialized = true;
        Fs_InitializeMountRom(default_dma_channel);
    }
}

bool Fs_IsInitialized(void) {
    return g_Initialized;
}

bool Fs_Chdir(const char *path) {
    Fs_File file;
    Fs_File_Initialize(&file);

    Fs_DirectoryPosition dir_pos;
    if(!Fs_File_FindPath(&file, path, NULL, &dir_pos)) {
        return false;
    }

    g_CurrentDirectoryPosition = dir_pos;
    return true;
}

bool Fs_RegisterArchive(Fs_Archive *arc, const char *name, size_t name_len) {
    u32 old_state = Os_DisableIRQ();
    bool res = false;

    if(Fs_FindArchiveByMountName(name, name_len) == NULL) {
        if(g_ArchiveList != NULL) {
            Fs_Archive *last_arc = g_ArchiveList;

            Fs_Archive *iter_arc;
            for(iter_arc = g_ArchiveList->next_arc; iter_arc != NULL; iter_arc = iter_arc->next_arc) {
                last_arc = iter_arc;
            }

            last_arc->next_arc = arc;
            arc->prev_arc = last_arc;
        }
        else {
            g_ArchiveList = arc;
            g_CurrentDirectoryPosition.arc = arc;
            g_CurrentDirectoryPosition.unk1 = 0;
            g_CurrentDirectoryPosition.unk2 = 0;
            g_CurrentDirectoryPosition.unk3 = 0;
        }

        arc->name_packed = Fs_PackArchiveMountName(name, name_len);
        arc->flags |= Fs_ArchiveFlag_Registered;
        res = true;
    }

    Os_RestoreIRQ(old_state);
    return res;
}

void Fs_UnregisterArchive(Fs_Archive *arc) {
    if(arc->name_packed != 0) {
        u32 old_state = Os_DisableIRQ();

        if(arc->next_arc != NULL) {
            arc->next_arc->prev_arc = arc->prev_arc;
        }
        if(arc->prev_arc != NULL) {
            arc->prev_arc->next_arc = arc->next_arc;
        }

        arc->name_packed = 0;
        arc->prev_arc = NULL;
        arc->next_arc = NULL;
        arc->flags &= ~Fs_ArchiveFlag_Registered;
        if(g_CurrentDirectoryPosition.arc == arc) {
            g_CurrentDirectoryPosition.arc = g_ArchiveList;
            g_CurrentDirectoryPosition.unk1 = 0;
            g_CurrentDirectoryPosition.unk2 = 0;
            g_CurrentDirectoryPosition.unk3 = 0;
        }

        Os_RestoreIRQ(old_state);
    }
}