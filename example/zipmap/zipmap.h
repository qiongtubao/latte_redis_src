#ifndef _ZIPMAP_H
#define _ZIPMAP_H


unsigned char *zipmapNew(void);
unsigned char *zipmapSet(unsigned char *zm, unsigned char *key, unsigned int klen, unsigned char *val, unsigned int vlen, int *update);
static unsigned long zipmapRequiredLength(unsigned int klen, unsigned int vlen);
static unsigned char *zipmapLookupRaw(unsigned char *zm, unsigned char *key, unsigned int klen, unsigned int *totlen);
static unsigned int zipmapRawEntryLength(unsigned char *p);
static unsigned int zipmapRawValueLength(unsigned char *p);
static unsigned int zipmapDecodeLength(unsigned char *p);
static unsigned int zipmapEncodeLength(unsigned char *p, unsigned int len);
static unsigned int zipmapRawKeyLength(unsigned char *p);
// static inline unsigned char *zipmapResize(unsigned char *zm, unsigned int len);

#endif