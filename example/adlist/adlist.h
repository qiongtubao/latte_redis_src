#ifndef __ADLIST_H__
#define __ADLIST_H__

#define listLength(l) ((l)->len)
#define listFirst(l) ((l)->head)
#define listLast(l) ((l)->tail)
typedef struct listNode {
    struct listNode *prev; //上一个节点
    struct listNode *next; //下一个节点
    void *value; //值
} listNode;


typedef struct listIter {
    //当前迭代位置的下一结点
    listNode *next;
    //迭代器的方向
    int direction;
} listIter;


typedef struct list {
    //列表头结点
    listNode *head; 
    //列表尾结点
    listNode *tail; 
    //复制函数指针
    void *(*dup)(void *ptr);
    //释放函数指针
    void (*free)(void *ptr);
    //匹配函数指针
    int (*match)(void *ptr, void *key);
    unsigned long len;
} list;

//创建列表函数
list *listCreate(void);
//列表释放
void listRelease(list *list);
//清空列表
void listEmpty(list *list);
//添加列表头节点
list *listAddNodeHead(list *list, void *value);
//添加列表尾节点
list *listAddNodeTail(list *list, void *value);
//创建迭代器
listIter *listGetIterator(list *list, int direction);
//迭代器下一个
listNode *listNext(listIter *iter);
// 在old_node结点的前面或后面插入新结点 
list *listInsertNode(list *list, listNode *old_node, void *value, int after);
//删除节点
void *listDelNode(list *list, listNode *node);
//释放迭代器内存
void listReleaseIterator(listIter *iter);
//迭代器重置 从头开始
void listRewind(list *list, listIter *li);
//迭代器重置 从尾开始
void listRewindTail(list *list, listIter *li);
//搜索 匹配key值的节点
listNode *listSearchKey(list *list, void *key);
//复制列表
list *listDup(list *orig);
//查找第几个节点
listNode *listIndex(list *list, long index);
//把尾节点移到头节点
void listRotate(list *list);
//合并2个list  并清空被合并的o
void listJoin(list *l, list*o);
/* Directions for iterators */
#define AL_START_HEAD 0
#define AL_START_TAIL 1

#endif /* __ADLIST_H__ */