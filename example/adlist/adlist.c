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
//清空列表
void listEmpty(list *list) {
    unsigned long len;
    listNode *current, *next;
    current = list->head;
    len = list->len;
    //删除并释放节点内存
    while(len--) {
        next = current->next;
        if(list->free) {
            //触发释放对象函数方法
            list->free(current->value);
        }
        zfree(current);
        current = next;
    }
    //设置双向链头和尾为NULL
    list->head = list->tail = NULL;
    //设置长度为0
    list->len = 0;
}
//释放列表
void listRelease(list *list) {
    //清空列表
    listEmpty(list);
    //释放列表对象内存
    zfree(list);
}
//链头追加
list *listAddNodeHead(list *list, void *value)
{
    listNode *node;
    //创建新的节点
    if((node = zmalloc(sizeof(*node))) == NULL ){
        return NULL;
    }
    //设置值
    node->value = value;
    if(list->len == 0) {
        //空双向链
        list->head = list->tail = node;
        node->prev = node->next = NULL;
    } else {
        //设置此结点next与前头结点的位置关系
        node->prev = NULL;
        node->next = list->head;
        list->head->prev = node;
        list->head = node;
    }
    //节点数+1
    list->len++;
    return list;
}
//尾部追加  操作大体上与增加头结点一样
list *listAddNodeTail(list *list, void *value) {
    listNode *node;
    if((node = zmalloc(sizeof(*node))) == NULL) {
        return NULL;
    }
    node->value = value;
    if(list->len == 0) {
        list->head = list->tail = node;
        node->prev = node->next = NULL;
    }else{
        node->next = NULL;
        node->prev = list->tail;
        list->tail->next = node;
        list->tail = node;
    }
    list->len++;
    return list;
}

/**
 * 插入某个节点
 * list 双向链
 * old_node 旧节点
 * value  值
 * after 表示前后
 *    
 */
list *listInsertNode(list *list, listNode* old_node, void *value, int after) {
    listNode *node;
    //创建新节点
    if((node = zmalloc(sizeof(*node))) == NULL) {
        return NULL;
    }
    node->value = value;

    if(after) {
        //如果是在目标结点的后面插入的情况，将新结点的next指针指向老结点的next
        node->prev = old_node;
        node->next = old_node->next;
        if(list->tail == old_node) {
            //如果老结点已经是最后一个结点了，则新的结点直接成为尾部结点
            list->tail = node;
        }
    } else {
        //如果是在目标结点的前面插入的情况，将新结点的preview指针指向老结点的preview
        node->next = old_node;
        node->prev = old_node->prev;
        if (list->head == old_node) {
            //如果老结点已经是头结点了，则新的结点直接成为头部结点
            list->head = node;
        }
    }
    //检查Node的前后结点还有没有未连接的操作
    if(node->prev != NULL) {
        node->prev->next = node;
    }
    if(node->next != NULL) {
        node->next->prev = node;
    }
    list->len++;
    return list;

}
/**
 * 列表删除某结点
 */ 
void *listDelNode(list *list, listNode *node) {
    
    if(node->prev) {
        //如果结点prev结点存在，prev的结点的下一及诶单指向Node的next结点
        node->prev->next = node->next;
    }else{
        list->head = node->next;
    }
    if(node->next == NULL) {
        //如果不存在说明是被删除的是头结点，则重新赋值Node的next为新头结点
        node->next->prev = node->prev;
    }else{
        list->tail = node->prev;
    }
    //同样要调用list的free函数
    if(list->free) {
        list->free(node->value);
    }
    zfree(node);
    list->len--;
    return list;
}
/**
 * 获得列表迭代器
 * direction 方向
 * 
 */ 
listIter *listGetIterator(list *list, int direction) {
    listIter *iter;
    if((iter = zmalloc(sizeof(*iter))) == NULL) {
        return NULL;
    } 
    if(direction == AL_START_HEAD) {
        //如果方向定义的是从头开始，则迭代器的next指针指向列表头结点
        iter->next = list->head;
    } else {
        //如果方向定义的是从尾开始，则迭代器的next指针指向列表尾结点
        iter->next = list->tail;
    }
    //赋值好迭代器方向并返回
    iter->direction = direction;
    return iter;
}
/**
 * 释放迭代器内存
 */ 
void listReleaseIterator(listIter *iter) {
    zfree(iter);
}
/**
 *  重置迭代器 从头开始
 */ 
void listRewind(list *list, listIter *li) {
    li->next = list->head;
    li->direction = AL_START_HEAD;
}
/**
 *  重置迭代器 从尾开始
 */ 
void listRewindTail(list *list, listIter *li) {
    li->next = list->tail;
    li->direction = AL_START_TAIL;
}
/**
 * 根据迭代器获取下一结点
 */
listNode *listNext(listIter *iter) {
    listNode *current = iter->next;
    //获取当前迭代器的当前结点
    if (current != NULL) {
        if (iter->direction == AL_START_HEAD)
            //如果方向为从头部开始，则当前结点等于当前的结点的下一结点
            iter->next = current->next;
        else
            //如果方向为从尾部开始，则当前结点等于当前的结点的上一结点
            iter->next = current->prev;
    }
    return current;
}
/**
 * 复制整个列表。 
*/
list *listDup(list *orig) {
    list *copy;
    listIter iter;
    listNode *node;
    if((copy = listCreate()) == NULL) {
        return NULL;
    }
    copy->dup = orig->dup;
    copy->free = orig->free;
    copy->match = orig->match;
    listRewind(orig, &iter);//不创建内存
    while((node = listNext(&iter)) != NULL) {
        void* value;
        if(copy->dup) {
            //复制
            value = copy->dup(node->value);
            if(value == NULL) {
                listRelease(copy);
                return NULL;
            }
        }else{
            //不复制
            value = node->value;
        }
        if (listAddNodeTail(copy, value) == NULL) {
        	//后面的结点都是从尾部逐一添加结点，如果内存溢出，同上操作
            listRelease(copy);
            return NULL;
        }
    }
    return copy;
}
/**
 * 查找值为key的节点
 * 
 */ 
listNode *listSearchKey(list *list, void *key) {
    listIter iter;
    listNode *node;
    listRewind(list, &iter);
    //遍历循环
    while((node = listNext(&iter)) != NULL) {
        if(list->match) {
            //如果定义了list的match方法，则调用match方法
            if(list->match(node->value, key)) {
                //如果方法返回true，则代表找到结点，释放迭代器
                return node;
            }
        }else{
            if(key == node->value) {
                return node;
            }
        }
    }
    return NULL;
}
/* 根据下标值返回相应的结点*/
/*下标有2种表示形式，从头往后一次0， 1， 2，...从后往前是 ...-3， -2， -1.-1为最后一个结点*/
listNode *listIndex(list *list, long index) {
    listNode *n;
    if(index < 0) {
        index = (-index) - 1;
        n = list->tail;
        while(index-- && n) {
            n = n->prev;
        }
    } else {
        n = list->head;
        while(index-- && n) {
            n = n->next;
        }
    }
    return n;
}
//把尾节点移到头节点
void listRotate(list *list) {
    listNode *tail = list->tail;
    if(listLength(list) <= 1) {
        return;
    }
    list->tail = tail->prev;
    list->tail->next = NULL;
    list->head->prev = tail;
    tail->prev = NULL;
    tail->next = list->head;
    list->head = tail;
}
/**
 * 合并2个list  并清空被合并的
 */ 
void listJoin(list *l, list*o) {
    if(o->head) {
        o->head->prev = l->tail;
    }
    if(l->tail) {
        l->tail->next = o->head;
    }else{
        l->head = o->head;
    }
    if(o->tail) {
        l->tail = o->tail;
    }
    l->len += o->len;
    o->head = o->tail = NULL;
    o->len = 0;
}   