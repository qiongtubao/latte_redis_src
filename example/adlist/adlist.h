#ifndef __ADLIST_H__
#define __ADLIST_H__


typedef struct listNode {
    struct listNode *prev; //上一个节点
    struct listNode *next; //下一个节点
    void *value; //值
} listNode;


typedef struct listIter {
    listNode *next;
    int direction;
} listIter;


typedef struct list {
    listNode *head; //list头
    listNode *tail; //list尾
    void *(*dup)(void *ptr);
    void (*free)(void *ptr);
    int (*match)(void *ptr, void *key);
    unsigned long len;
} list;

list *listCreate(void);
list *listAddNodeHead(list *list, void *value);
listIter *listGetIterator(list *list, int direction);
listNode *listNext(listIter *iter);
/* Directions for iterators */
#define AL_START_HEAD 0
#define AL_START_TAIL 1

#endif /* __ADLIST_H__ */