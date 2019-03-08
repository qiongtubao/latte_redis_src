#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <limits.h>
#include "sds.h"
#include "zmalloc.h"
/**
 * 获得类型
 *   < 32   SDS_TYPE_5
 *   < 256  SDS_TYPE_8  
 *   < 65536 SDS_TYPE_16
 *   < 4294967296  SDS_TYPE_32
 */
static inline char sdsReqType(size_t string_size) {
    if (string_size < 1<<5)  
        return SDS_TYPE_5;
    if (string_size < 1<<8)
        return SDS_TYPE_8;
    if (string_size < 1<<16)
        return SDS_TYPE_16;
#if (LONG_MAX == LLONG_MAX)
    if (string_size < 1ll<<32)
        return SDS_TYPE_32;
    return SDS_TYPE_64;
#else
    return SDS_TYPE_32;
#endif
}
static inline int sdsHdrSize(char type) {
    switch(type&SDS_TYPE_MASK) {
        case SDS_TYPE_5:
            return sizeof(struct sdshdr5);
        case SDS_TYPE_8:
            return sizeof(struct sdshdr8);
        case SDS_TYPE_16:
            return sizeof(struct sdshdr16);
        case SDS_TYPE_32:
            return sizeof(struct sdshdr32);
        case SDS_TYPE_64:
            return sizeof(struct sdshdr64);
    }
    return 0;
}

/**
 * 创建字符串 根据长度
 */ 
sds sdsnewlen(const void *init, size_t initlen) {
    void *sh;
    sds s;
    char type = sdsReqType(initlen);
    // 如果类型是type5  默认使用type8
    if (type == SDS_TYPE_5 && initlen == 0) type = SDS_TYPE_8;
    int hdrlen = sdsHdrSize(type);
    unsigned char *fp;
    sh = s_malloc(hdrlen+initlen);
    if (init == SDS_NOINIT) {
        init = NULL;
    } else if (!init) {
        memset(sh, 0, hdrlen + initlen + 1);
    }
    if (sh == NULL) {
        return NULL;
    }
    s = (char*)sh + hdrlen;
    fp = ((unsigned char*)s) - 1;
    switch(type) {
        case SDS_TYPE_5: {
            *fp = type | (initlen << SDS_TYPE_BITS);
            break;
        }
        case SDS_TYPE_8: {
            SDS_HDR_VAR(8,s);
            sh->len = initlen;
            sh->alloc = initlen;
            *fp = type;
            break;
        }
        case SDS_TYPE_16: {
            SDS_HDR_VAR(16,s);
            sh->len = initlen;
            sh->alloc = initlen;
            *fp = type;
            break;
        }
        case SDS_TYPE_32: {
            SDS_HDR_VAR(32,s);
            sh->len = initlen;
            sh->alloc = initlen;
            *fp = type;
            break;
        }
        case SDS_TYPE_64: {
            SDS_HDR_VAR(64,s);
            sh->len = initlen;
            sh->alloc = initlen;
            *fp = type;
            break;
        }
    }
    if(initlen && init) {
        memcpy(s, init, initlen);
    }
    s[initlen] = '\0';
    return s;

}
/**
 * 创建字符串 
 */ 
sds sdsnew(const void *init) {
    size_t initlen = (init == NULL) ? 0 : strlen(init);
    return sdsnewlen(init, initlen);
}
/**
 * 复制sds
 */ 
sds sdsdup(const sds s) {
    return sdsnewlen(s, sdslen(s));
}
/**
 * 追加字符串
 */
sds sdscat(sds s, const char *t) {
    return sdscatlen(s, t, strlen(t));
}

/**
 *  追加字符串 限定长度
 */ 
sds sdscatlen(sds s, const char *t, size_t len) {
    size_t curlen = sdslen(s);
    s = sdsMakeRoomFor(s,len);
    if (s == NULL) return NULL;
    memcpy(s+curlen, t, len);
    sdssetlen(s, curlen+len);
    s[curlen+len] = '\0';
    return s;
}
/**
 *  重新清理sds 数据
 */ 
sds sdsMakeRoomFor(sds s, size_t addlen) {
    void *sh, *newsh;
    size_t avail = sdsavail(s);
    size_t len, newlen;
    char type, oldtype = s[-1] & SDS_TYPE_MASK;
    int hdrlen;
    /* Return ASAP if there is enough space left. */
    if (avail >= addlen) return s;

    len = sdslen(s);

    sh = (char*)s-sdsHdrSize(oldtype);
    //获得新长度
    newlen = (len+addlen);
    if (newlen < SDS_MAX_PREALLOC)
        newlen *= 2;
    else
        newlen += SDS_MAX_PREALLOC;
    //通过新长度 数据类型
    type = sdsReqType(newlen);
    if (type == SDS_TYPE_5) type = SDS_TYPE_8;
    hdrlen = sdsHdrSize(type);
    if (oldtype==type) {
        //和原来类型一致,重新申请
        newsh = s_realloc(sh, hdrlen+newlen+1);
        if (newsh == NULL) return NULL;
        //把指针定向到char数组
        s = (char*)newsh+hdrlen;
    } else {
        /* Since the header size changes, need to move the string forward,
         * and can't use realloc */
        //申请内存
        newsh = s_malloc(hdrlen+newlen+1);
        if (newsh == NULL) return NULL;
        memcpy((char*)newsh+hdrlen, s, len+1);
        s_free(sh);
        s = (char*)newsh+hdrlen;
        s[-1] = type;
        //重新设置长度
        sdssetlen(s, len);
    }
    //设置使用长度
    sdssetalloc(s, newlen);
    return s;
}
//释放sds内存
void sdsfree(sds s) {
    if (s == NULL) return;
    s_free((char*)s-sdsHdrSize(s[-1]));
}

sds sdscpylen(sds s, const char* t, size_t len) {
    if (sdsalloc(s) < len) {
        s = sdsMakeRoomFor(s,len-sdslen(s));
        if (s == NULL) return NULL;
    }
    memcpy(s, t, len);
    s[len] = '\0';
    sdssetlen(s, len);
    return s;    
}
//追加字符串sds
sds sdscatsds(sds s, const sds t) {
    return sdscatlen(s, t, sdslen(t));
}
//创建一个空的字符串
sds sdsempty(void) {
    return sdsnewlen("",0);
}

//sds重新赋值
sds sdscpy(sds s, const char* t) {
    return sdscpylen(s, t, strlen(t));
}

sds sdscatprintf(sds s, const char *fmt, ...) {
    va_list ap;
    sds t;
    va_start(ap, fmt);
    t = sdscatvprintf(s,fmt,ap);
    va_end(ap);
    return t;
}

//这里设置
sds sdscatvprintf(sds s, const char *fmt, va_list ap) {
    va_list cpy;
    char staticbuf[1024], *buf = staticbuf, *t;
    size_t buflen = strlen(fmt)*2;

    /* We try to start using a static buffer for speed.
     * If not possible we revert to heap allocation. */
    if (buflen > sizeof(staticbuf)) {
        buf = s_malloc(buflen);
        if (buf == NULL) return NULL;
    } else {
        buflen = sizeof(staticbuf);
    }

    /* Try with buffers two times bigger every time we fail to
     * fit the string in the current buffer size. */
    //一直格式化到成功 如果失败重新 申请2倍的内存长度
    while(1) {
        buf[buflen-2] = '\0';
        va_copy(cpy,ap);
        //buf 是存放格式化之后的字符串, 最长为buflen长度
        vsnprintf(buf, buflen, fmt, cpy);
        va_end(cpy);
        if (buf[buflen-2] != '\0') {
            if (buf != staticbuf) s_free(buf);
            buflen *= 2;
            buf = s_malloc(buflen);
            if (buf == NULL) return NULL;
            continue;
        }
        break;
    }

    /* Finally concat the obtained string to the SDS string and return it. */
    t = sdscat(s, buf);
    if (buf != staticbuf) s_free(buf);
    return t;
}

#define SDS_LLSTR_SIZE 21
/**
 * 把int转成字符串 每位用2位0x表示
 */
int sdsll2str(char *s, long long value) {
    char *p, aux;
    unsigned long long v;
    size_t l;

    /* Generate the string representation, this method produces
     * an reversed string. */
    v = (value < 0) ? -value : value;
    p = s;
    do {
        *p++ = '0'+(v%10);
        v /= 10;
    } while(v);
    if (value < 0) *p++ = '-';

    /* Compute length and add null term. */
    l = p-s;
    *p = '\0';

    /* Reverse the string. */
    p--;
    while(s < p) {
        aux = *s;
        *s = *p;
        *p = aux;
        s++;
        p--;
    }
    return l;
}

/**
 * uint 转字符串
 */ 
int sdsull2str(char *s, unsigned long long v) {
    char *p, aux;
    size_t l;

    /* Generate the string representation, this method produces
     * an reversed string. */
    p = s;
    do {
        *p++ = '0'+(v%10);
        v /= 10;
    } while(v);

    /* Compute length and add null term. */
    l = p-s;
    *p = '\0';

    /* Reverse the string. */
    p--;
    while(s < p) {
        aux = *s;
        *s = *p;
        *p = aux;
        s++;
        p--;
    }
    return l;
}
sds sdscatfmt(sds s, char const *fmt, ...) {
    size_t initlen = sdslen(s);
    const char *f = fmt;
    long i;
    va_list ap;

    va_start(ap,fmt);
    f = fmt;    /* Next format specifier byte to process. */
    i = initlen; /* Position of the next byte to write to dest str. */
    while(*f) {
        char next, *str;
        size_t l;
        long long num;
        unsigned long long unum;

        /* Make sure there is always space for at least 1 char. */
        if (sdsavail(s)==0) {
            s = sdsMakeRoomFor(s,1);
        }

        switch(*f) {
        case '%':
            next = *(f+1);
            f++;
            switch(next) {
            case 's':
            case 'S':
                str = va_arg(ap,char*);
                l = (next == 's') ? strlen(str) : sdslen(str);
                if (sdsavail(s) < l) {
                    s = sdsMakeRoomFor(s,l);
                }
                memcpy(s+i,str,l);
                sdsinclen(s,l);
                i += l;
                break;
            case 'i':
            case 'I':
                if (next == 'i')
                    num = va_arg(ap,int);
                else
                    num = va_arg(ap,long long);
                {
                    char buf[SDS_LLSTR_SIZE];
                    l = sdsll2str(buf,num);
                    if (sdsavail(s) < l) {
                        s = sdsMakeRoomFor(s,l);
                    }
                    memcpy(s+i,buf,l);
                    sdsinclen(s,l);
                    i += l;
                }
                break;
            case 'u':
            case 'U':
                if (next == 'u')
                    unum = va_arg(ap,unsigned int);
                else
                    unum = va_arg(ap,unsigned long long);
                {
                    char buf[SDS_LLSTR_SIZE];
                    l = sdsull2str(buf,unum);
                    if (sdsavail(s) < l) {
                        s = sdsMakeRoomFor(s,l);
                    }
                    memcpy(s+i,buf,l);
                    sdsinclen(s,l);
                    i += l;
                }
                break;
            default: /* Handle %% and generally %<unknown>. */
                s[i++] = next;
                sdsinclen(s,1);
                break;
            }
            break;
        default:
            s[i++] = *f;
            sdsinclen(s,1);
            break;
        }
        f++;
    }
    va_end(ap);

    /* Add null-term */
    s[i] = '\0';
    return s;
}

sds sdstrim(sds s, const char *cset) {
    char *start, *end, *sp, *ep;
    size_t len;

    sp = start = s;
    ep = end = s+sdslen(s)-1;
    //查找sp
    while(sp <= end && strchr(cset, *sp)) sp++;
    //查找ep
    while(ep > sp && strchr(cset, *ep)) ep--;
    len = (sp > ep) ? 0 : ((ep-sp)+1);
    //由sp所指内存区域复制count个字节到dest所指内存区域。
    if (s != sp) memmove(s, sp, len);
    s[len] = '\0';
    sdssetlen(s,len);
    return s;
}

void sdsrange(sds s, ssize_t start, ssize_t end) {
    size_t newlen, len = sdslen(s);

    if (len == 0) return;
    if (start < 0) {
        start = len+start;
        if (start < 0) start = 0;
    }
    if (end < 0) {
        end = len+end;
        if (end < 0) end = 0;
    }
    newlen = (start > end) ? 0 : (end-start)+1;
    if (newlen != 0) {
        if (start >= (ssize_t)len) {
            newlen = 0;
        } else if (end >= (ssize_t)len) {
            end = len-1;
            newlen = (start > end) ? 0 : (end-start)+1;
        }
    } else {
        start = 0;
    }
    //计算开始位置和截取长度
    //从s+start 拷贝newlen个字节   
    if (start && newlen) memmove(s, s+start, newlen);
    s[newlen] = 0;
    sdssetlen(s,newlen);
}
/**
 * 对比 2个sds 
 *   > 返回 1
 *   == 返回 0
 *   <  返回 -1
 */ 
int sdscmp(const sds s1, const sds s2) {
    size_t l1, l2, minlen;
    int cmp;
    l1 = sdslen(s1);
    l2 = sdslen(s2);
    minlen = (l1 < l2) ? l1 : l2;
    cmp = memcmp(s1,s2,minlen);
    if (cmp == 0) return l1>l2? 1: (l1<l2? -1: 0);
    return cmp;
}
/**
 *  字符串转义   
 */
sds sdscatrepr(sds s, const char *p, size_t len) {
    s = sdscatlen(s,"\"",1);
    while(len--) {
        switch(*p) {
        case '\\':
        case '"':
            s = sdscatprintf(s,"\\%c",*p);
            break;
        case '\n': s = sdscatlen(s,"\\n",2); break;
        case '\r': s = sdscatlen(s,"\\r",2); break;
        case '\t': s = sdscatlen(s,"\\t",2); break;
        case '\a': s = sdscatlen(s,"\\a",2); break;
        case '\b': s = sdscatlen(s,"\\b",2); break;
        default:
            if (isprint(*p))
                s = sdscatprintf(s,"%c",*p);
            else
                s = sdscatprintf(s,"\\x%02x",(unsigned char)*p);
            break;
        }
        p++;
    }
    return sdscatlen(s,"\"",1);
}

void sdstoupper(sds s) {
    size_t len = sdslen(s), j;

    for (j = 0; j < len; j++) s[j] = toupper(s[j]);
}

void sdstolower(sds s) {
    size_t len = sdslen(s), j;

    for (j = 0; j < len; j++) s[j] = tolower(s[j]);
}
//字符串拼接
sds sdsjoin(char **argv, int argc, char *sep) {
    sds join = sdsempty();
    int j;

    for (j = 0; j < argc; j++) {
        join = sdscat(join, argv[j]);
        if (j != argc-1) join = sdscat(join,sep);
    }
    return join;
}