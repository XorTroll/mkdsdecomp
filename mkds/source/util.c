#include "util.h"

void Util_FillMemory32(int val, void *src, size_t size) {
    u32 *cur_32 = (u32*)src;
    u32 *end_32 = (u32*)(src + size);

    while(cur_32 < end_32) {
        *cur_32 = val;
        cur_32++;
    }
}

#define _MKDS_UTIL_INTRUSIVE_LIST_ITEM_NODE(list, item) ((Util_IntrusiveListNode*)(item + list->item_list_node_offset))

void Util_IntrusiveList_InsertFirstItem(Util_IntrusiveList *self, void *item) {
    Util_IntrusiveListNode *item_node = _MKDS_UTIL_INTRUSIVE_LIST_ITEM_NODE(self, item);
    item_node->item_next = NULL;
    item_node->item_prev = NULL;

    self->item_list_head = item;
    self->item_list_tail = item;
    ++self->item_count;
}

// ....

void Util_IntrusiveList_Initialize(Util_IntrusiveList *self, u16 item_list_node_offset) {
    self->item_list_head = NULL;
    self->item_list_tail = NULL;
    self->item_count = 0;
    self->item_list_node_offset = item_list_node_offset;
}

void *Util_IntrusiveList_GetPrevItem(Util_IntrusiveList *self, void *item) {
    // NOTE: this code assumes that the item is actually part of the list
    // (otherwise it would make little sense)
    if(item != NULL) {
        return _MKDS_UTIL_INTRUSIVE_LIST_ITEM_NODE(self, item)->item_prev;
    }
    else {
        // Return end of the list
        return self->item_list_tail;
    }
}

void *Util_IntrusiveList_GetNextItem(Util_IntrusiveList *self, void *item) {
    // NOTE: this code assumes that the item is actually part of the list
    // (otherwise it would make little sense)
    if(item != NULL) {
        return ((Util_IntrusiveListNode*)(item + self->item_list_node_offset))->item_next;
    }
    else {
        // Return start of the list
        return self->item_list_head;
    }
}

void *Util_IntrusiveList_GetAtIndex(Util_IntrusiveList *self, int idx) {
    void *cur_item = Util_IntrusiveList_GetNextItem(self, NULL);
    if(cur_item == NULL) {
        // List is empty
        return NULL;
    }

    int i = 0;
    while(i != idx) {
        ++i;

        cur_item = Util_IntrusiveList_GetNextItem(self, cur_item);
        if(cur_item == NULL) {
            // List ended before reaching 'idx'
            return NULL;
        }
    }

    return cur_item;
}

void Util_IntrusiveList_InsertFirst(Util_IntrusiveList *self, void *item) {
    if(self->item_list_head != NULL) {
        _MKDS_UTIL_INTRUSIVE_LIST_ITEM_NODE(self, item)->item_prev = NULL;
        _MKDS_UTIL_INTRUSIVE_LIST_ITEM_NODE(self, item)->item_next = self->item_list_head;
        _MKDS_UTIL_INTRUSIVE_LIST_ITEM_NODE(self, self->item_list_head)->item_prev = item;
        self->item_list_head = item;

        ++self->item_count;
    }
    else {
        // List is currently empty
        Util_IntrusiveList_InsertFirstItem(self, item);
    }
}

void Util_IntrusiveList_InsertLast(Util_IntrusiveList *self, void *item) {
    if(self->item_list_head != NULL) {
        _MKDS_UTIL_INTRUSIVE_LIST_ITEM_NODE(self, item)->item_prev = self->item_list_tail;
        _MKDS_UTIL_INTRUSIVE_LIST_ITEM_NODE(self, item)->item_next = NULL;
        _MKDS_UTIL_INTRUSIVE_LIST_ITEM_NODE(self, self->item_list_tail)->item_next = item;
        self->item_list_tail = item;

        ++self->item_count;
    }
    else {
        // List is currently empty
        Util_IntrusiveList_InsertFirstItem(self, item);
    }
}

void Util_IntrusiveList_RemoveItem(Util_IntrusiveList *self, void *item) {
    Util_IntrusiveListNode *item_node = _MKDS_UTIL_INTRUSIVE_LIST_ITEM_NODE(self, item);
    if(item_node->item_prev != NULL) {
        _MKDS_UTIL_INTRUSIVE_LIST_ITEM_NODE(self, item_node->item_prev)->item_next = item_node->item_next;
    }
    else {
        self->item_list_head = item_node->item_next;
    }

    if(item_node->item_next != NULL) {
        _MKDS_UTIL_INTRUSIVE_LIST_ITEM_NODE(self, item_node->item_next)->item_prev = item_node->item_prev;
    }
    else {
        self->item_list_tail = item_node->item_prev;
    }

    item_node->item_prev = NULL;
    item_node->item_next = NULL;
    --self->item_count;
}