
#include <stdio.h>
#include <string.h>
#include <zmalloc.h>
#include "zipmap.h"
// #include "endianconv.h"
#define ZIPMAP_BIGLEN 254
#define ZIPMAP_END 255
#define ZIPMAP_VALUE_MAX_FREE 4
#define ZIPMAP_LEN_BYTES(_l) (((_l) < ZIPMAP_BIGLEN) ? 1 : sizeof(unsigned int)+1)


#define memrev16ifbe(p) ((void)(0))
#define memrev32ifbe(p) ((void)(0))
#define memrev64ifbe(p) ((void)(0))
#define intrev16ifbe(v) (v)
#define intrev32ifbe(v) (v)
#define intrev64ifbe(v) (v)
unsigned char *zipmapNew(void) {
    unsigned char *zm = zmalloc(2);

    zm[0] = 0; /* Length */
    zm[1] = ZIPMAP_END;
    return zm;
}
static inline unsigned char *zipmapResize(unsigned char *zm, unsigned int len) {
    zm = zrealloc(zm, len);
    zm[len-1] = ZIPMAP_END;
    return zm;
}

unsigned char *zipmapSet(unsigned char *zm, unsigned char *key, unsigned int klen, unsigned char *val, unsigned int vlen, int *update) {
   unsigned int zmlen, offset;
   unsigned int freelen, reqlen = zipmapRequiredLength(klen, vlen);
   unsigned int empty, vempty;
   unsigned char *p;
   freelen = reqlen;
   if(update) *update = 0;
   p = zipmapLookupRaw(zm, key, klen, &zmlen);
    if(p == NULL) {
        zm = zipmapResize(zm, zmlen + reqlen);
        p = zm + zmlen - 1;
        zmlen = zmlen + reqlen;
        if(zm[0] < ZIPMAP_BIGLEN) {
            zm[0]++;
        }
    }else{
        if(update) {
            *update = 1;
        }
        freelen = zipmapRawEntryLength(p);
        if(freelen < reqlen) {
            offset = p - zm;
            zm = zipmapResize(zm, zmlen - freelen + reqlen);
            p = zm + offset;
            memmove(p + reqlen, p + freelen, zmlen - (offset + freelen + 1));
            zmlen = zmlen - freelen + reqlen;
            freelen = reqlen;
        }
    }
    empty = freelen - reqlen;
    if(empty >= ZIPMAP_VALUE_MAX_FREE) {
        offset = p - zm;
        memmove(p + reqlen, p + freelen, zmlen - (offset+freelen + 1));
        zmlen -= empty;
        zm = zipmapResize(zm, zmlen);
        p = zm + offset;
        vempty = 0;
    } else {
        vempty = empty;
    }
    p += zipmapEncodeLength(p, klen);
    memcpy(p, key, klen);
    p += klen;
    p += zipmapEncodeLength(p, vlen);
    *p++ = vempty;
    memcpy(p, val, vlen);
    return zm;
}


static unsigned long zipmapRequiredLength(unsigned int klen, unsigned int vlen) {
    unsigned int l;
    l = klen + vlen + 3;
    if(klen >= ZIPMAP_BIGLEN) l += 4;
    if(vlen >= ZIPMAP_BIGLEN) l += 4;
    return l;
}


static unsigned char *zipmapLookupRaw(unsigned char *zm, unsigned char *key, unsigned int klen, unsigned int *totlen) {
    unsigned char *p = zm + 1, *k = NULL;
    unsigned int l, llen;
    while(*p != ZIPMAP_END) {
        unsigned char free;
        l = zipmapDecodeLength(p);
        llen = zipmapEncodeLength(NULL, l);
        if(key != NULL && k == NULL && l == klen && !memcmp(p+llen, key, l)) {
            if(totlen != NULL) {
                k = p;
            }else{
                return p;
            }
        }
        p += llen + l;
        l = zipmapDecodeLength(p);
        p += zipmapEncodeLength(NULL, l);
        free = p[0];
        p += l + 1 + free;
    }
    if(totlen != NULL) *totlen = (unsigned int)(p-zm) + 1;
    return k;
}
/**
 * 获得  （key记录 + key)的长度 
 */ 
static unsigned int zipmapRawKeyLength(unsigned char *p) {
    unsigned int l = zipmapDecodeLength(p);
    return zipmapEncodeLength(NULL,l) + l;
}
/**
 * 
 */ 
static unsigned int zipmapRawEntryLength(unsigned char *p) {
    unsigned int l = zipmapRawKeyLength(p);
    return l + zipmapRawValueLength(p+l);
}
/* Decode the encoded length pointed by 'p' */
static unsigned int zipmapDecodeLength(unsigned char *p) {
    unsigned int len = *p;

    if (len < ZIPMAP_BIGLEN) return len;
    memcpy(&len,p+1,sizeof(unsigned int));
    memrev32ifbe(&len);
    return len;
}
static unsigned int zipmapEncodeLength(unsigned char *p, unsigned int len) {
    if (p == NULL) {
        return ZIPMAP_LEN_BYTES(len);
    } else {
        if (len < ZIPMAP_BIGLEN) {
            p[0] = len;
            return 1;
        } else {
            p[0] = ZIPMAP_BIGLEN;
            memcpy(p+1,&len,sizeof(len));
            memrev32ifbe(p+1);
            return 1+sizeof(len);
        }
    }
}
/**
 *  获得  value + 标记value 长度的 字节数
 */ 
static unsigned int zipmapRawValueLength(unsigned char *p) {
    unsigned int l = zipmapDecodeLength(p);
    unsigned int used;

    used = zipmapEncodeLength(NULL,l);
    used += p[used] + 1 + l;
    return used;
}


unsigned char *zipmapRewind(unsigned char *zm) {
    return zm+1;
}

unsigned char *zipmapNext(unsigned char *zm, unsigned char **key, unsigned int *klen, unsigned char **value, unsigned int *vlen) {
    if (zm[0] == ZIPMAP_END) return NULL;
    if (key) {
        *key = zm;
        *klen = zipmapDecodeLength(zm);
        *key += ZIPMAP_LEN_BYTES(*klen);
    }
    zm += zipmapRawKeyLength(zm);
    if (value) {
        *value = zm+1;
        *vlen = zipmapDecodeLength(zm);
        *value += ZIPMAP_LEN_BYTES(*vlen);
    }
    zm += zipmapRawValueLength(zm);
    return zm;
}

/**
 *  判断key值是否存在
 */ 
int zipmapExists(unsigned char *zm, unsigned char *key, unsigned int klen) {
    return zipmapLookupRaw(zm,key,klen,NULL) != NULL;
}
/**
 * 删除kv
 */ 
unsigned char *zipmapDel(unsigned char *zm, unsigned char *key, unsigned int klen, int *deleted) {
    unsigned int zmlen, freelen;
    unsigned char *p = zipmapLookupRaw(zm, key, klen, &zmlen);
    if(p) {
        freelen = zipmapRawEntryLength(p);
        memmove(p, p + freelen, zmlen-((p-zm) + freelen + 1));
        zm = zipmapResize(zm, zmlen- freelen);
        if(zm[0] < ZIPMAP_BIGLEN) {
            zm[0]--;
        }
        if(deleted) {
            *deleted = 1;
        }
    }else{
        if(deleted) {
            *deleted = 0;
        }
    }
    return zm;
}
/**
 * 查询map长度
 */ 
unsigned int zipmapLen(unsigned char *zm) {
    unsigned int len = 0;
    if(zm[0] < ZIPMAP_BIGLEN) {
        len = zm[0];
    }else{
        unsigned char *p = zipmapRewind(zm);
        while((p = zipmapNext(p, NULL, NULL, NULL, NULL)) != NULL) {
            len++;
        }
        if(len < ZIPMAP_BIGLEN) {
            zm[0] = len;
        }
    }
    return len;
}