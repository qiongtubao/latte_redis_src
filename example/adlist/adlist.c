#include <stdlib.h>
#include "adlist.h"
#include  "zmalloc.h"

//创建list
list *listCreate(void)
{
    struct list *list;

    if ((list = zmalloc(sizeof(*list))) == NULL)
        return NULL;
    list->head = list->tail = NULL;
    list->len = 0;
    list->dup = NULL;
    list->free = NULL;
    list->match = NULL;
    return list;
}
//链头追加
list *listAddNodeHead(list *list, void *value)
{
    listNode *node;
    if((node = zmalloc(sizeof(*node))) == NULL ){
        return NULL;
    }
    node->value = value;
    if(list->len == 0) {
        list->head = list->tail = node;
        node->prev = node->next = NULL;
    } else {
        node->prev = NULL;
        node->next = list->head;
        list->head->prev = node;
        list->head = node;
    }
    list->len++;
    return list;
}
//iter
listIter *listGetIterator(list *list, int direction) {
    listIter *iter;
    if((iter = zmalloc(sizeof(*iter))) == NULL) {
        return NULL;
    } 
    if(direction == AL_START_HEAD) {
        iter->next = list->head;
    } else {
        iter->next = list->tail;
    }
    iter->direction = direction;
    return iter;
}

listNode *listNext(listIter *iter) {
    listNode *current = iter->next;

    if (current != NULL) {
        if (iter->direction == AL_START_HEAD)
            iter->next = current->next;
        else
            iter->next = current->prev;
    }
    return current;
}