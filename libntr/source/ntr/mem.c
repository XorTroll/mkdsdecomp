#include <ntr/mem.h>
#include <ntr/util.h>
#include <ntr/os.h>

static Util_IntrusiveList g_GlobalHeapHeadList;
static int g_GlobalHeapHeadListInitialized = 0;

typedef struct Mem_MemoryRegion {
    uintptr_t start;
    uintptr_t end;
} Mem_MemoryRegion;

#define _NTR_MEM_REGION_SIZE(region) (size_t)((region)->end - (region)->start)

#define _NTR_MEM_EXP_HEAP_HEAD_MAGIC 0x45585048 // "EXPH"

#define _NTR_MEM_EXP_HEAP_FREE_BLOCK_HEAD_MAGIC 0x4652 // "FR"
#define _NTR_MEM_EXP_HEAP_USED_BLOCK_HEAD_MAGIC 0x5544 // "UD"

typedef struct Mem_ExpHeapMemoryBlockHead {
    u16 magic;
    u16 attributes;
    u32 block_size;
    struct Mem_ExpHeapMemoryBlockHead *prev_block;
    struct Mem_ExpHeapMemoryBlockHead *next_block;
} Mem_ExpHeapMemoryBlockHead;
_Static_assert(sizeof(Mem_ExpHeapMemoryBlockHead) == 0x10);

#define _NTR_MEM_EXP_GET_BLOCK(block_head) (((uintptr_t)block_head) + sizeof(Mem_ExpHeapMemoryBlockHead))
#define _NTR_MEM_EXP_GET_PTR_BLOCK_HEAD(ptr) ((Mem_ExpHeapMemoryBlockHead*)(((uintptr_t)ptr) - sizeof(Mem_ExpHeapMemoryBlockHead)))

#define _NTR_MEM_EXP_GET_MEMORY_BLOCK_ALIGNMENT_PADDING(block_head) (u16)(((block_head)->attributes >> 8) & 0x7F)

typedef enum Mem_CreateOption {
    Mem_CreateOption_None = 0,
    Mem_CreateOption_ZeroClear = NTR_BIT(0)
} Mem_CreateOption;

typedef enum Mem_AllocationMode {
    Mem_AllocationMode_FirstFit = 0,
    Mem_AllocationMode_BestFit = 1
} Mem_AllocationMode;

typedef enum Mem_AllocationDirection {
    Mem_AllocationDirection_Front = 0,
    Mem_AllocationDirection_Back = 1
} Mem_AllocationDirection;

typedef struct Mem_ExpHeapMemoryBlockList {
    Mem_ExpHeapMemoryBlockHead *list_head;
    Mem_ExpHeapMemoryBlockHead *list_tail;
} Mem_ExpHeapMemoryBlockList;

typedef struct Mem_ExpHeapHead {
    Mem_ExpHeapMemoryBlockList free_list;
    Mem_ExpHeapMemoryBlockList used_list;
    u16 group_id;
    u16 alloc_mode;
} Mem_ExpHeapHead;
_Static_assert(sizeof(Mem_ExpHeapHead) == 0x14);

#define _NTR_MEM_FRAME_HEAP_HEAD_MAGIC 0x46524D48 // "FRMH"

typedef struct Mem_FrameHeapState {
    u32 id;
    Mem_MemoryRegion heap_region;
    struct Mem_FrameHeapState *next_state;
} Mem_FrameHeapState;
_Static_assert(sizeof(Mem_FrameHeapState) == 0x10);

typedef struct Mem_FrameHeapHead {
    Mem_MemoryRegion cur_heap_region;
    Mem_FrameHeapState *cur_state;
} Mem_FrameHeapHead;
_Static_assert(sizeof(Mem_FrameHeapHead) == 0x0C);

struct Mem_HeapHead {
    u32 magic;
    Util_IntrusiveListNode child_heap_head_list_node;
    Util_IntrusiveList child_heap_head_list;
    Mem_MemoryRegion heap_region;
    u32 option;
    union {
        Mem_ExpHeapHead exp_heap_head;
        Mem_FrameHeapHead frm_heap_head;
    };
};
_Static_assert(sizeof(Mem_HeapHead) == 0x38);

#define _NTR_EXP_GET_BASE_HEAP_HEAD(exp_heap_head_p) ((Mem_HeapHead*)((uintptr_t)exp_heap_head_p - __builtin_offsetof(Mem_HeapHead, exp_heap_head)))

#define _NTR_FRAME_GET_BASE_HEAP_HEAD(frm_heap_head_p) ((Mem_HeapHead*)((uintptr_t)frm_heap_head_p - __builtin_offsetof(Mem_HeapHead, frm_heap_head)))

Mem_HeapHead *Mem_FindTopParentHeapHead(Util_IntrusiveList *list, Mem_HeapHead *heap_head) {
    Mem_HeapHead *item_list_head = Util_IntrusiveList_GetNextItem(list, NULL);
    if(item_list_head == NULL) {
        // No items means no parents
        return NULL;
    }

    // Iterate all the list, find a heap head whose child is the input heap head
    while((item_list_head->heap_region.start > (uintptr_t)heap_head) || ((uintptr_t)heap_head >= item_list_head->heap_region.end)) {
        item_list_head = Util_IntrusiveList_GetNextItem(list, item_list_head);
        if(item_list_head == NULL) {
            // Couldn't fint it before reaching the end
            return NULL;
        }
    }

    // Keep looking recursively on this item's list?
    Mem_HeapHead *rec_item = Mem_FindTopParentHeapHead(&item_list_head->child_heap_head_list, heap_head);
    if(rec_item != NULL) {
        // Found parent in a deeper level
        return rec_item;
    }
    else {
        return item_list_head;
    }
}

Mem_HeapHead *Mem_FindGlobalTopParentHeapHead(Mem_HeapHead *heap_head) {
    return Mem_FindTopParentHeapHead(&g_GlobalHeapHeadList, heap_head);
}

Util_IntrusiveList *Mem_FindTopParentHeapHeadList(Mem_HeapHead *heap_head) {
    Mem_HeapHead *parent_heap_head = Mem_FindTopParentHeapHead(&g_GlobalHeapHeadList, heap_head);
    if(parent_heap_head != NULL) {
        // It's a child heap head, found it's top parent
        return &parent_heap_head->child_heap_head_list;
    }
    else {
        // It's a new heap head
        return &g_GlobalHeapHeadList;
    }
}

void Mem_GetMemoryBlockRegion(Mem_MemoryRegion *out_region, Mem_ExpHeapMemoryBlockHead *block_head) {
    out_region->start = (uintptr_t)block_head - (uintptr_t)(_NTR_MEM_EXP_GET_MEMORY_BLOCK_ALIGNMENT_PADDING(block_head));
    out_region->end = _NTR_MEM_EXP_GET_BLOCK(block_head) + block_head->block_size;
}

void Mem_InitializeHeapHead(Mem_HeapHead *out_heap_head, u32 magic, uintptr_t start, uintptr_t end, u8 option) {
    out_heap_head->magic = magic;
    out_heap_head->heap_region.start = start;
    out_heap_head->heap_region.end = end;
    out_heap_head->option = 0;
    out_heap_head->option &= ~0xFFu;
    out_heap_head->option |= option;
    Util_IntrusiveList_Initialize(&out_heap_head->child_heap_head_list, __builtin_offsetof(Mem_HeapHead, child_heap_head_list_node));

    if(g_GlobalHeapHeadListInitialized != 1) {
        // Lazy-initialize the global heap head list, when the first heap head is created
        Util_IntrusiveList_Initialize(&g_GlobalHeapHeadList, __builtin_offsetof(Mem_HeapHead, child_heap_head_list_node));
        g_GlobalHeapHeadListInitialized = 1;
    }

    // Insert itself in the heap head hierarchy
    Util_IntrusiveList *top_parent_heap_head_list = Mem_FindTopParentHeapHeadList(out_heap_head);
    Util_IntrusiveList_InsertLast(top_parent_heap_head_list, out_heap_head);
}

void Mem_FinalizeHeapHead(Mem_HeapHead *heap_head) {
    Util_IntrusiveList *top_parent_heap_head_list = Mem_FindTopParentHeapHeadList(heap_head);
    Util_IntrusiveList_RemoveItem(top_parent_heap_head_list, heap_head);
}

// ---

size_t Mem_Frame_GetAllocatableSize(Mem_HeapHead *heap_head, ssize_t align) {
    size_t actual_align = abs(align);
    uintptr_t cur_heap_start_aligned = NTR_UTIL_ALIGN_UP(heap_head->frm_heap_head.cur_heap_region.start, actual_align);
    if(cur_heap_start_aligned <= heap_head->frm_heap_head.cur_heap_region.end) {
        return heap_head->frm_heap_head.cur_heap_region.end - cur_heap_start_aligned;
    }
    else {
        return 0;
    }
}

void *Mem_Frame_AllocFromHead(Mem_FrameHeapHead *frm_heap_head, size_t size, size_t align) {
    uintptr_t cur_ptr_start_aligned = NTR_UTIL_ALIGN_UP(frm_heap_head->cur_heap_region.start, align);
    uintptr_t cur_ptr_end = cur_ptr_start_aligned + size;
    if(cur_ptr_end > frm_heap_head->cur_heap_region.end) {
        return 0;
    }
    if(_NTR_FRAME_GET_BASE_HEAP_HEAD(frm_heap_head)->option & Mem_CreateOption_ZeroClear) {
        Util_FillMemory32(0, (void*)frm_heap_head->cur_heap_region.start, cur_ptr_end - frm_heap_head->cur_heap_region.start);
    }
    void *ptr = (void*)cur_ptr_start_aligned;
    frm_heap_head->cur_heap_region.start = cur_ptr_end;
    return ptr;
}

void *Mem_Frame_AllocFromTail(Mem_FrameHeapHead *frm_heap_head, size_t size, size_t align) {
    uintptr_t cur_heap_end_aligned = NTR_UTIL_ALIGN_DOWN(frm_heap_head->cur_heap_region.end - size, align);
    if(cur_heap_end_aligned < frm_heap_head->cur_heap_region.start) {
        return 0;
    }
    if(_NTR_FRAME_GET_BASE_HEAP_HEAD(frm_heap_head)->option & Mem_CreateOption_ZeroClear) {
        Util_FillMemory32(0, (void*)cur_heap_end_aligned, frm_heap_head->cur_heap_region.end - cur_heap_end_aligned);
    }
    void *ptr = (void*)cur_heap_end_aligned;
    frm_heap_head->cur_heap_region.end = cur_heap_end_aligned;
    return ptr;
}

void *Mem_Frame_Allocate(Mem_HeapHead *heap_head, size_t size, ssize_t align) {
    if(size == 0) {
        size = 1;
    }

    size_t aligned_size = NTR_UTIL_ALIGN_UP(size, 0x4);

    if(align < 0) {
        return Mem_Frame_AllocFromTail(&heap_head->frm_heap_head, aligned_size, -align);
    }
    else {
        return Mem_Frame_AllocFromHead(&heap_head->frm_heap_head, aligned_size, align);
    }
}

Mem_HeapHandle Mem_Frame_Initialize(Mem_HeapHead *heap_head, uintptr_t end, int option) {
    Mem_InitializeHeapHead(heap_head, _NTR_MEM_FRAME_HEAP_HEAD_MAGIC, (uintptr_t)heap_head + sizeof(Mem_HeapHead), end, option);
    heap_head->frm_heap_head.cur_heap_region.start = heap_head->heap_region.start;
    heap_head->frm_heap_head.cur_heap_region.end = heap_head->heap_region.end;
    heap_head->frm_heap_head.cur_state = NULL;
    return heap_head;
}

Mem_HeapHandle Mem_Frame_Create(void *ptr, size_t size, int option) {
    uintptr_t aligned_start = NTR_UTIL_ALIGN_UP((uintptr_t)ptr, 0x4);
    uintptr_t aligned_end = NTR_UTIL_ALIGN_DOWN((uintptr_t)(ptr + size), 0x4);

    if((aligned_start <= aligned_end) && ((aligned_end - aligned_start) >= 0x30)) {
        Mem_HeapHead *heap_head = (Mem_HeapHead*)aligned_start;
        return Mem_Frame_Initialize(heap_head, aligned_end, option);
    }
    else {
        return NULL;
    }
}

void Mem_Frame_Finalize(Mem_HeapHead *heap_head) {
    Mem_FinalizeHeapHead(heap_head);
}

// ---

Mem_ExpHeapMemoryBlockHead *Mem_Exp_InitializeMemoryBlock(Mem_MemoryRegion *region, u16 magic) {
    Mem_ExpHeapMemoryBlockHead *head = (Mem_ExpHeapMemoryBlockHead*)region->start;
    head->magic = magic;
    head->attributes = 0;
    head->block_size = region->end - (region->start + sizeof(Mem_ExpHeapMemoryBlockHead));
    head->prev_block = NULL;
    head->next_block = NULL;
    return head;
}

Mem_HeapHandle Mem_Exp_Initialize(uintptr_t start, uintptr_t end, u8 option) {
    Mem_HeapHead *heap_head = (Mem_HeapHead*)start;
    Mem_ExpHeapHead *exp_heap_head = &heap_head->exp_heap_head;

    Mem_InitializeHeapHead(heap_head, _NTR_MEM_EXP_HEAP_HEAD_MAGIC, start + sizeof(Mem_HeapHead), end, option);
    exp_heap_head->group_id = 0;
    exp_heap_head->alloc_mode &= ~0x1u;
    exp_heap_head->alloc_mode = Mem_AllocationMode_FirstFit;

    Mem_ExpHeapMemoryBlockHead *start_free_block = Mem_Exp_InitializeMemoryBlock(&heap_head->heap_region, _NTR_MEM_EXP_HEAP_FREE_BLOCK_HEAD_MAGIC);
    heap_head->exp_heap_head.free_list.list_head = start_free_block;
    heap_head->exp_heap_head.free_list.list_tail = start_free_block;
    heap_head->exp_heap_head.used_list.list_head = NULL;
    heap_head->exp_heap_head.used_list.list_tail = NULL;
    return heap_head;
}

Mem_HeapHandle Mem_Exp_Create(void *ptr, size_t size, u8 option) {
    uintptr_t end = NTR_UTIL_ALIGN_DOWN((uintptr_t)(ptr + size), 0x4);
    uintptr_t start = NTR_UTIL_ALIGN_UP((uintptr_t)ptr, 0x4);

    if((start <= end) && ((end - start) >= 0x4C)) { // TODO: where does this size come from?
        return Mem_Exp_Initialize(start, end, option);
    }
    else {
        return NULL;
    }
}

Mem_ExpHeapMemoryBlockHead *Mem_ExpHeapMemoryBlockList_Insert(Mem_ExpHeapMemoryBlockList *list, Mem_ExpHeapMemoryBlockHead *block_head, Mem_ExpHeapMemoryBlockHead *prev_block_head) {
    Mem_ExpHeapMemoryBlockHead *next_block;

    block_head->prev_block = prev_block_head;
    if(prev_block_head) {
        next_block = prev_block_head->next_block;
        prev_block_head->next_block = block_head;
    }
    else {
        next_block = list->list_head;
        list->list_head = block_head;
    }

    block_head->next_block = next_block;
    if(next_block) {
        next_block->prev_block = block_head;
    }
    else {
        list->list_tail = block_head;
    }

    return block_head;
}

Mem_ExpHeapMemoryBlockHead *Mem_ExpHeapMemoryBlockList_Remove(Mem_ExpHeapMemoryBlockList *list, Mem_ExpHeapMemoryBlockHead *block_head) {
    Mem_ExpHeapMemoryBlockHead *prev_block = block_head->prev_block;
    Mem_ExpHeapMemoryBlockHead *next_block = block_head->next_block;

    if(prev_block != NULL) {
        prev_block->next_block = next_block;
    }
    else {
        list->list_head = next_block;
    }

    if(next_block) {
        next_block->prev_block = prev_block;
    }
    else {
        list->list_tail = prev_block;
    }
    return prev_block;
}

size_t Mem_Exp_GetAllocatableSize(Mem_HeapHead *heap_head, ssize_t align) {
    size_t actual_align = abs(align);

    Mem_ExpHeapMemoryBlockHead *cur_block_head = heap_head->exp_heap_head.free_list.list_head;
    size_t max_allocatable_size = 0;
    size_t min_offset = SIZE_MAX;
    if(cur_block_head != NULL) {
        do {
            uintptr_t cur_block_start_aligned = NTR_UTIL_ALIGN_UP(_NTR_MEM_EXP_GET_BLOCK(cur_block_head), actual_align);
            uintptr_t cur_block_end = _NTR_MEM_EXP_GET_BLOCK(cur_block_head) + cur_block_head->block_size;
            if((cur_block_start_aligned < cur_block_end) && (((max_allocatable_size < (cur_block_end - cur_block_start_aligned)) || (max_allocatable_size == (cur_block_end - cur_block_start_aligned))) && (min_offset > (cur_block_start_aligned - _NTR_MEM_EXP_GET_BLOCK(cur_block_head))))) {
                max_allocatable_size = cur_block_end - cur_block_start_aligned;
                min_offset = cur_block_start_aligned - _NTR_MEM_EXP_GET_BLOCK(cur_block_head);
            }
            cur_block_head = cur_block_head->next_block;
        } while(cur_block_head != NULL);
    }
    return max_allocatable_size;
}

void *Mem_Exp_ConvertFreeBlockToUsedBlock(Mem_ExpHeapHead *exp_heap_head, Mem_ExpHeapMemoryBlockHead *block_head, uintptr_t block, size_t size, Mem_AllocationDirection mem_block_alloc_direction) {
    Mem_MemoryRegion free_region_front;
    Mem_GetMemoryBlockRegion(&free_region_front, block_head);

    uintptr_t end = free_region_front.end;
    free_region_front.end = (uintptr_t)_NTR_MEM_EXP_GET_PTR_BLOCK_HEAD(block);

    Mem_MemoryRegion free_region_back;
    free_region_back.start = (uintptr_t)block + size;
    free_region_back.end = end;
    Mem_ExpHeapMemoryBlockHead *prev_block_head = Mem_ExpHeapMemoryBlockList_Remove(&exp_heap_head->free_list, block_head);
    if(_NTR_MEM_REGION_SIZE(&free_region_front) >= sizeof(Mem_ExpHeapMemoryBlockHead)) {
        Mem_ExpHeapMemoryBlockHead *new_free_block = Mem_Exp_InitializeMemoryBlock(&free_region_front, _NTR_MEM_EXP_HEAP_FREE_BLOCK_HEAD_MAGIC);
        prev_block_head = Mem_ExpHeapMemoryBlockList_Insert(&exp_heap_head->free_list, new_free_block, prev_block_head);
    }
    else {
        free_region_front.end = free_region_front.start;
    }

    if(_NTR_MEM_REGION_SIZE(&free_region_back) >= sizeof(Mem_ExpHeapMemoryBlockHead)) {
        Mem_ExpHeapMemoryBlockHead *new_used_block = Mem_Exp_InitializeMemoryBlock(&free_region_front, _NTR_MEM_EXP_HEAP_USED_BLOCK_HEAD_MAGIC);
        Mem_ExpHeapMemoryBlockList_Insert(&exp_heap_head->free_list, new_used_block, prev_block_head);
    }
    else {
        free_region_back.end = free_region_back.start;
    }

    if(_NTR_EXP_GET_BASE_HEAP_HEAD(exp_heap_head)->option & Mem_CreateOption_ZeroClear) {
        Util_FillMemory32(0, (void*)free_region_front.end, free_region_back.start - free_region_front.end);
    }

    Mem_MemoryRegion used_region;
    used_region.start = (uintptr_t)_NTR_MEM_EXP_GET_PTR_BLOCK_HEAD(block);
    used_region.end = free_region_back.start;
    Mem_ExpHeapMemoryBlockHead *used_block = Mem_Exp_InitializeMemoryBlock(&used_region, _NTR_MEM_EXP_HEAP_USED_BLOCK_HEAD_MAGIC);
    
    // Set allocation direction
    used_block->attributes &= ~0x8000u;
    used_block->attributes |= (mem_block_alloc_direction) << 15;

    // Set memory block alignment padding
    size_t mem_block_align_pad = (uintptr_t)used_block - free_region_front.end;
    used_block->attributes &= ~0x7F00u;
    used_block->attributes |= (mem_block_align_pad & 0x7F) << 8;

    // Set group ID
    used_block->attributes &= ~0xFFu;
    used_block->attributes |= exp_heap_head->group_id;

    Mem_ExpHeapMemoryBlockList_Insert(&exp_heap_head->used_list, used_block, exp_heap_head->used_list.list_tail);
    return (void*)block;
}

int Mem_Exp_CoalesceFreedRegion(Mem_ExpHeapHead *heap_head, Mem_MemoryRegion *region) {
    Mem_MemoryRegion free_region;
    free_region.start = region->start;
    free_region.end = region->end;

    Mem_ExpHeapMemoryBlockHead *cur_free_block = heap_head->free_list.list_head;
    Mem_ExpHeapMemoryBlockHead *prev_free_block = NULL;
    if(cur_free_block != NULL) {
        while((uintptr_t)cur_free_block < region->start) {
            prev_free_block = cur_free_block;
            cur_free_block = cur_free_block->next_block;
            if(cur_free_block == NULL) {
                break;
            }
        }
        if((uintptr_t)cur_free_block == region->end) {
            free_region.end = _NTR_MEM_EXP_GET_BLOCK(cur_free_block) + cur_free_block->block_size;
            Mem_ExpHeapMemoryBlockList_Remove(&heap_head->free_list, cur_free_block);
        }
    }

    if((prev_free_block != NULL) && ((_NTR_MEM_EXP_GET_BLOCK(prev_free_block) + prev_free_block->block_size) == region->start)) {
        free_region.start = (uintptr_t)prev_free_block;
        prev_free_block = Mem_ExpHeapMemoryBlockList_Remove(&heap_head->free_list, prev_free_block);
    }

    if(_NTR_MEM_REGION_SIZE(&free_region) >= sizeof(Mem_ExpHeapMemoryBlockHead)) {
        Mem_ExpHeapMemoryBlockHead *new_free_block = Mem_Exp_InitializeMemoryBlock(&free_region, _NTR_MEM_EXP_HEAP_FREE_BLOCK_HEAD_MAGIC);
        Mem_ExpHeapMemoryBlockList_Insert(&heap_head->free_list, new_free_block, prev_free_block);
        return 1;
    }
    else {
        return 0;
    }
}

void *Mem_Exp_AllocateFromHead(Mem_HeapHead *heap_head, size_t size, size_t align) {
    bool is_first_fit = heap_head->exp_heap_head.alloc_mode == Mem_AllocationMode_FirstFit;
    Mem_ExpHeapMemoryBlockHead *found_block_head = NULL;
    uintptr_t found_block = 0;
    size_t best_size = SIZE_MAX;

    Mem_ExpHeapMemoryBlockHead *cur_free_block_head = heap_head->exp_heap_head.free_list.list_head;
    if(cur_free_block_head != NULL) {
        do {
            size_t cur_block_size = cur_free_block_head->block_size;
            uintptr_t cur_block = _NTR_MEM_EXP_GET_BLOCK(cur_free_block_head);
            uintptr_t block_offset = NTR_UTIL_ALIGN_UP(_NTR_MEM_EXP_GET_BLOCK(cur_free_block_head), align);
            // Check if the data would fit in this block
            if((cur_block_size >= (size + block_offset - cur_block)) && (best_size > cur_block_size)) {
                found_block_head = cur_free_block_head;
                best_size = cur_block_size;
                found_block = block_offset;
                if(is_first_fit || (best_size == size)) {
                    break;
                }
            }

            if(((block_offset - cur_block) >= 0) && (best_size > cur_block_size)) {
                found_block_head = cur_free_block_head;
                best_size = cur_block_size;
                found_block = block_offset;
                if(is_first_fit || (best_size == size)) {
                    break;
                }
            }
            cur_free_block_head = cur_free_block_head->next_block;
        } while(cur_free_block_head != NULL);
    }

    if(found_block_head != NULL) {  
        return Mem_Exp_ConvertFreeBlockToUsedBlock(&heap_head->exp_heap_head, found_block_head, found_block, size, Mem_AllocationDirection_Front);
    }
    else {
        return NULL;
    }
}

void *Mem_Exp_AllocateFromTail(Mem_HeapHead *heap_head, size_t size, size_t align) {
    bool is_first_fit = heap_head->exp_heap_head.alloc_mode == Mem_AllocationMode_FirstFit;
    Mem_ExpHeapMemoryBlockHead *found_block_head = NULL;
    uintptr_t found_block = 0;
    size_t best_size = SIZE_MAX;

    Mem_ExpHeapMemoryBlockHead *cur_free_block_head = heap_head->exp_heap_head.free_list.list_tail;
    if(cur_free_block_head != NULL) {
        do {
            size_t cur_block_size = cur_free_block_head->block_size;
            uintptr_t cur_block = _NTR_MEM_EXP_GET_BLOCK(cur_free_block_head);
            uintptr_t block_offset = NTR_UTIL_ALIGN_DOWN(_NTR_MEM_EXP_GET_BLOCK(cur_free_block_head) + cur_block_size - size, align);
            // Check if the data would fit in this block
            if(((block_offset - cur_block) >= 0) && (best_size > cur_block_size)) {
                found_block_head = cur_free_block_head;
                best_size = cur_block_size;
                found_block = block_offset;
                if(is_first_fit || (best_size == size)) {
                    break;
                }
            }
            cur_free_block_head = cur_free_block_head->prev_block;
        } while(cur_free_block_head != NULL);
    }

    if(found_block_head != NULL) {  
        return Mem_Exp_ConvertFreeBlockToUsedBlock(&heap_head->exp_heap_head, found_block_head, found_block, size, Mem_AllocationDirection_Back);
    }
    else {
        return NULL;
    }
}

void *Mem_Exp_Allocate(Mem_HeapHead *heap_head, size_t size, ssize_t align) {
    // Prevent allocating empty blocks, I guess
    if(size == 0) {
        size = 1;
    }

    size_t aligned_size = NTR_UTIL_ALIGN_UP(size, 0x4);
    // Negative align value means allocating from tail instead of head
    if(align < 0) {
        return Mem_Exp_AllocateFromTail(heap_head, aligned_size, -align);
    }
    else {
        return Mem_Exp_AllocateFromHead(heap_head, aligned_size, align);
    }
}

void Mem_Exp_Free(Mem_HeapHead *heap_head, void *ptr) {
    Mem_MemoryRegion region;
    Mem_ExpHeapMemoryBlockHead *block_head = _NTR_MEM_EXP_GET_PTR_BLOCK_HEAD(ptr);
    Mem_GetMemoryBlockRegion(&region, block_head);

    Mem_ExpHeapMemoryBlockList_Remove(&heap_head->exp_heap_head.used_list, block_head);
    Mem_Exp_CoalesceFreedRegion(&heap_head->exp_heap_head, &region);
}

void Mem_Exp_Finalize(Mem_HeapHead *heap_head) {
    Mem_FinalizeHeapHead(heap_head);
}

// ....

Mem_HeapHandle Mem_CreateExpHeap(void *ptr, size_t size) {
    return Mem_Exp_Create(ptr, size, Mem_CreateOption_ZeroClear);
}

void Mem_DestroyHeap(Mem_HeapHandle heap_handle) {
    // Dispose this heap head
    if(heap_handle->magic == _NTR_MEM_EXP_HEAP_HEAD_MAGIC) {
        return Mem_Exp_Finalize(heap_handle);
    }
    else if(heap_handle->magic == _NTR_MEM_FRAME_HEAP_HEAD_MAGIC) {
        return Mem_Frame_Finalize(heap_handle);
    }

    // If it's a child heap head, release its allocated memory from the parent heap head
    Mem_HeapHead *parent_heap_head = Mem_FindGlobalTopParentHeapHead(heap_handle);
    if(parent_heap_head != NULL) {
        // Since the heap head ptr is already the start of its allocated data, just free it (only needed with EXP heap heads)
        if(parent_heap_head->magic == _NTR_MEM_EXP_HEAP_HEAD_MAGIC) {
            Mem_Exp_Free(parent_heap_head, heap_handle);
        }
    }
}

void *Mem_AllocateHeap(Mem_HeapHandle heap_handle, size_t size) {
    // Basically 'Mem_AllocateHeapAligned(heap_handle, size, 4)'

    if(heap_handle->magic == _NTR_MEM_EXP_HEAP_HEAD_MAGIC) {
        return Mem_Exp_Allocate(heap_handle, size, 4);
    }
    if(heap_handle->magic == _NTR_MEM_FRAME_HEAP_HEAD_MAGIC) {
        return Mem_Frame_Allocate(heap_handle, size, 4);
    }

    // Invalid handle
    return NULL;
}

void *Mem_AllocateHeapAligned(Mem_HeapHandle heap_handle, size_t size, ssize_t align) {    
    if(heap_handle->magic == _NTR_MEM_EXP_HEAP_HEAD_MAGIC) {
        return Mem_Exp_Allocate(heap_handle, size, align);
    }
    if(heap_handle->magic == _NTR_MEM_FRAME_HEAP_HEAD_MAGIC) {
        return Mem_Frame_Allocate(heap_handle, size, align);
    }

    // Invalid handle
    return NULL;
}

void Mem_FreeHeap(Mem_HeapHandle heap_handle, void *ptr) {
    // Only EXP heaps do actually need to free allocated data
    if(heap_handle->magic == _NTR_MEM_EXP_HEAP_HEAD_MAGIC) {
        Mem_Exp_Free(heap_handle, ptr);
    }
}

void Mem_SetExpHeapGroupId(Mem_HeapHandle heap_handle, u16 group_id) {
    heap_handle->exp_heap_head.group_id = group_id;
}

size_t Mem_GetExpHeapTotalFreeSize(Mem_HeapHandle heap_handle) {
    Mem_ExpHeapMemoryBlockHead *cur_block = heap_handle->exp_heap_head.free_list.list_head;
    size_t total_size;
    for(total_size = 0; cur_block != NULL; total_size += cur_block->block_size) {
        cur_block = cur_block->next_block;
    }
    return total_size;
}

void Mem_ResizeExpHeap(Mem_HeapHandle heap_handle, void *ptr, size_t size) {
    Mem_ExpHeapMemoryBlockHead *ptr_block = _NTR_MEM_EXP_GET_PTR_BLOCK_HEAD(ptr);
    size_t aligned_size = NTR_UTIL_ALIGN_UP(size, 0x4);
    size_t orig_block_size = ptr_block->block_size;
    if(aligned_size != orig_block_size) {
        if(aligned_size <= orig_block_size) {
            Mem_MemoryRegion region;
            region.start = (uintptr_t)ptr + aligned_size;
            region.end = (uintptr_t)ptr + orig_block_size;
            ptr_block->block_size = aligned_size;
            if(Mem_Exp_CoalesceFreedRegion(&heap_handle->exp_heap_head, &region) == 0) {
                // Unable to coalesce
                ptr_block->block_size = orig_block_size;
            }
        }
        else {
            Mem_ExpHeapMemoryBlockHead *i;
            for(i = heap_handle->exp_heap_head.free_list.list_head; i != NULL; i = i->next_block) {
                if((uintptr_t)i == (uintptr_t)(ptr + orig_block_size)) {
                    break;
                }
            }
            if((i != NULL) && (aligned_size <= (orig_block_size + i->block_size + sizeof(Mem_ExpHeapMemoryBlockHead)))) {
                Mem_MemoryRegion region;
                Mem_GetMemoryBlockRegion(&region, i);

                Mem_ExpHeapMemoryBlockHead *prev_free_block = Mem_ExpHeapMemoryBlockList_Remove(&heap_handle->exp_heap_head.free_list, i);
                uintptr_t block_start = region.start;
                region.start = (uintptr_t)ptr + aligned_size;
                if(_NTR_MEM_REGION_SIZE(&region) < sizeof(Mem_ExpHeapMemoryBlockHead)) {
                    region.end = region.start;
                }
                ptr_block->block_size = aligned_size;
                if(_NTR_MEM_REGION_SIZE(&region) >= sizeof(Mem_ExpHeapMemoryBlockHead)) {
                    Mem_ExpHeapMemoryBlockHead *new_free_block = Mem_Exp_InitializeMemoryBlock(&region, _NTR_MEM_EXP_HEAP_FREE_BLOCK_HEAD_MAGIC);
                    Mem_ExpHeapMemoryBlockList_Insert(&heap_handle->exp_heap_head.free_list, new_free_block, prev_free_block);
                }

                if(heap_handle->option & Mem_CreateOption_ZeroClear) {
                    Util_FillMemory32(0, (void*)block_start, region.start - block_start);
                }
            }
        }
    }
}

int Mem_CreateFrameHeapState(Mem_HeapHandle heap_handle, u32 state_id) {
    Mem_FrameHeapState *new_state = Mem_Frame_AllocFromHead(&heap_handle->frm_heap_head, sizeof(Mem_FrameHeapState), 0x4);
    if(new_state != NULL) {
        new_state->id = state_id;
        new_state->heap_region.start = heap_handle->frm_heap_head.cur_heap_region.start;
        new_state->heap_region.end = heap_handle->frm_heap_head.cur_heap_region.end;
        new_state->next_state = heap_handle->frm_heap_head.cur_state;
        heap_handle->frm_heap_head.cur_state = new_state;
        return 1;
    }
    else {
        return 0;
    }
}

int Mem_RestoreFrameHeapState(Mem_HeapHandle heap_handle, u32 state_id) {
    Mem_FrameHeapState *cur_state = heap_handle->frm_heap_head.cur_state;
    if((state_id != 0) && (cur_state != NULL)) {
        do {
            if(cur_state->id == state_id) {
                break;
            }
            cur_state = cur_state->next_state;
        } while(cur_state != NULL);
    }

    if(cur_state != NULL) {
        heap_handle->frm_heap_head.cur_heap_region.start = cur_state->heap_region.start;
        heap_handle->frm_heap_head.cur_heap_region.end = cur_state->heap_region.end;
        heap_handle->frm_heap_head.cur_state = cur_state->next_state;
        return 1;
    }
    else {
        return 0;
    }
}

Mem_HeapHandle Mem_CreateChildExpHeap(Mem_HeapHandle parent_heap_handle) {
    // Use all available size in the parent heap head

    size_t allocatable_size = 0;
    if(parent_heap_handle->magic == _NTR_MEM_EXP_HEAP_HEAD_MAGIC) {
        allocatable_size = Mem_Exp_GetAllocatableSize(parent_heap_handle, 0x4);
    }
    else if(parent_heap_handle->magic == _NTR_MEM_FRAME_HEAP_HEAD_MAGIC) {
        allocatable_size = Mem_Frame_GetAllocatableSize(parent_heap_handle, 0x4);
    }

    void *allocated_ptr = NULL;
    if(parent_heap_handle->magic == _NTR_MEM_EXP_HEAP_HEAD_MAGIC) {
        allocated_ptr = Mem_Exp_Allocate(parent_heap_handle, allocatable_size, 0x4);
    }
    else if(parent_heap_handle->magic == _NTR_MEM_FRAME_HEAP_HEAD_MAGIC) {
        allocated_ptr = Mem_Frame_Allocate(parent_heap_handle, allocatable_size, 0x4);
    }

    return Mem_Exp_Create(allocated_ptr, allocatable_size, Mem_CreateOption_ZeroClear);
}

Mem_HeapHandle Mem_CreateChildFrameHeapFromHead(Mem_HeapHandle parent_heap_handle, size_t size) {
    void *ptr = NULL;
    if(parent_heap_handle->magic == _NTR_MEM_EXP_HEAP_HEAD_MAGIC) {
        ptr = Mem_Exp_Allocate(parent_heap_handle, size, 0x4);
    }
    else if(parent_heap_handle->magic == _NTR_MEM_FRAME_HEAP_HEAD_MAGIC) {
        ptr = Mem_Frame_Allocate(parent_heap_handle, size, 0x4);
    }

    return Mem_Frame_Create(ptr, size, Mem_CreateOption_ZeroClear);
}

Mem_HeapHandle Mem_CreateChildFrameHeapFromTail(Mem_HeapHandle parent_heap_handle, size_t size) {
    void *ptr = NULL;
    if(parent_heap_handle->magic == _NTR_MEM_EXP_HEAP_HEAD_MAGIC) {
        ptr = Mem_Exp_Allocate(parent_heap_handle, size, -0x4);
    }
    else if(parent_heap_handle->magic == _NTR_MEM_FRAME_HEAP_HEAD_MAGIC) {
        u32 old_state = Os_DisableIRQ();
        ptr = Mem_Frame_Allocate(parent_heap_handle, size, -0x4);
        Os_RestoreIRQ(old_state);
    }

    return Mem_Frame_Create(ptr, size, Mem_CreateOption_ZeroClear);
}