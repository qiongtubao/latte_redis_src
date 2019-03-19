#ifndef _ZIPMAP_H
#define _ZIPMAP_H
#define ZIPMAP_END 255

/**
 * 
 * \x02      \x03     foo   \x03    \x00    bar   \x05  hello  \x05  \x00  world  \xff
 *   |         |       |     |       |       |      |     |      |     |      |     |
 * 2个键值对   3个字节   key  3个字节  分隔内容  value   。。。                         结束符
 *  
 *  
 *  
 */ 
unsigned char *zipmapNew(void);
unsigned char *zipmapSet(unsigned char *zm, unsigned char *key, unsigned int klen, unsigned char *val, unsigned int vlen, int *update);
unsigned char *zipmapRewind(unsigned char *zm);

static unsigned long zipmapRequiredLength(unsigned int klen, unsigned int vlen);
static unsigned char *zipmapLookupRaw(unsigned char *zm, unsigned char *key, unsigned int klen, unsigned int *totlen);
static unsigned int zipmapRawEntryLength(unsigned char *p);
static unsigned int zipmapRawValueLength(unsigned char *p);
static unsigned int zipmapDecodeLength(unsigned char *p);
static unsigned int zipmapEncodeLength(unsigned char *p, unsigned int len);
static unsigned int zipmapRawKeyLength(unsigned char *p);
unsigned char *zipmapNext(unsigned char *zm, unsigned char **key, unsigned int *klen, unsigned char **value, unsigned int *vlen);
// static inline unsigned char *zipmapResize(unsigned char *zm, unsigned int len);
int zipmapExists(unsigned char *zm, unsigned char *key, unsigned int klen);
unsigned char *zipmapDel(unsigned char *zm, unsigned char *key, unsigned int klen, int *deleted);
//打印
// void zipmapRepr(unsigned char *p);
unsigned int zipmapLen(unsigned char *zm);
#endif