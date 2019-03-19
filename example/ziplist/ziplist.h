#ifndef _ZIPLIST_H
#define _ZIPLIST_H
#include "zmalloc.h"

#define ZIPLIST_BYTES(zl)       (*((uint32_t*)(zl)))

/**
 * 
 * 
 * ziplist是一个专门编码的双链表
 * 非常节省内存。它存储字符串和整数值，
 * 其中整数被编码为实际整数而不是一系列整数
 * 人物。它允许在列表的任何一侧进行推送和弹出操作
 * 在O（1）时间。但是，因为每个操作都需要重新分配
 * ziplist使用的内存，实际的复杂性与之相关
 * ziplist使用的内存量
 * ZIPLIST总体布局
 * ======================
 * ziplist的总体布局如下：
 *
 * <zlbytes> <zltail> <zllen> <entry> <entry> ... <entry> <zlend>
 * 注意：如果没有另外指定，所有字段都以小端存储。
 *
 * <uint32_t zlbytes>是一个无符号整数，用于保存该字节数
 * ziplist占用，包括zlbytes字段本身的四个字节。
 * 需要存储此值以便能够调整整个结构的大小
 * 无需先遍历它。
 * <uint32_t zltail>是列表中最后一个条目的偏移量。这允许
 * 列表远端的弹出操作，无需完整
 * 遍历。
 * <uint16_t zllen>是条目数。当有超过
 * 2 ^ 16-2个条目，这个值设置为2 ^ 16-1，我们需要遍历
 * 整个列表，以了解它拥有多少项目。
 * <uint8_t zlend>是表示ziplist结尾的特殊条目。
 * 编码为单个字节，等于255.没有其他正常条目开始
 * 将字节设置为255的值。
 * ZIPLIST条目
 * ===============
 *
 * ziplist中的每个条目都以包含两个部分的元数据为前缀
 * 的信息。首先，存储前一个条目的长度
 * 能够从后到前遍历列表。其次，条目编码是
 * 提供。它表示条目类型，整数或字符串，在本例中
 * 字符串也表示字符串有效负载的长度。
 * 所以完整的条目存储如下：
 * <prevlen> <encoding> <entry-data>
 *
 * 有时编码表示条目本身，就像小整数一样
 * 我们稍后会看到。在这种情况下，<entry-data>部分缺失，我们
 * 可能只有：
 *
 * <prevlen> <encoding>
 *
 * 上一个条目<prevlen>的长度按以下方式编码：
 * 如果此长度小于254个字节，则只消耗一个
 * 字节表示长度为未指定的8位整数。当长度
 * 大于或等于254，它将消耗5个字节。第一个字节是
 * 设置为254（FE）表示跟随的值越大。剩下的4个
 * bytes将前一个条目的长度作为值。
 *
 * 所以实际上一个条目按以下方式编码：
 *
 * <prevlen从0到253> <encoding> <entry>
 *
 * 或者，如果前一个条目长度大于253个字节
 * 使用以下编码：
 *
 * 0xFE <4字节无符号小端子prevlen> <encoding> <entry>
 *
 * 条目的​​编码字段取决于的内容
 * 进入。当条目是字符串时，首先是编码的前2位
 * byte将保存用于存储字符串长度的编码类型，
 * 后跟字符串的实际长度。当条目是整数时
 * 前2位都设置为1.以下2位用于指定
 * 此标题后将存储哪种整数。概述
 * 不同类型和编码如下。第一个字节总是足够的
 * 确定进入的种类。
 *
 * | 00pppppp | -  1个字节
 * 字符串值，长度小于或等于63字节（6位）。
 * “pppppp”表示无符号6位长度。
 * | 01pppppp | qqqqqqqq | -  2个字节
 * 字符串值，长度小于或等于16383字节（14位）。

 * 重要提示：14位数存储在big endian中。
 * | 10000000 | qqqqqqqq | rrrrrrrr | ssssssss | tttttttt | -  5个字节
 * 长度大于或等于16384字节的字符串值。
 * 只有第一个字节后面的4个字节表示长度
 * 最多32 ^ 2-1。不使用第一个字节的低6位
 * 设置为零。
 * 重要提示：32位数字存储在big endian中。
 * | 11000000 | -  3个字节
 * 整数编码为int16_t（2个字节）。
 * | 11010000 | -  5个字节
 * 整数编码为int32_t（4字节）。
 * | 11100000 | -  9个字节
 * 整数编码为int64_t（8字节）。
 * | 11110000 | - 4字节
 * 整数编码为24位有符号（3字节）。
 * | 11111110 | -  2个字节
 * 整数编码为8位有符号（1字节）。
 * | 1111xxxx | - （xxxx在0000和1101之间）立即4位整数。
 * 0到12之间的无符号整数。编码值实际上来自
 * 1到13因为0000和1111不能使用，所以1应该是
 * 从编码的4位值中减去以获得正确的值。
 * | 11111111 | -  ziplist特殊条目结束。
 * 与ziplist标题一样，所有整数都表示为很少
 * endian字节顺序，即使在大端系统中编译此代码时也是如此。
 * 实际拉链的例子
 * ===========================
 * 以下是包含两个元素代表的ziplist
 * 数字“2”和“5”。它由15个字节组成，我们在视觉上
 * 分为几个部分：
 *  [总共15个字节]  [最后一个entry偏移12("5"的偏移量)] [entry个数] ...     ...     [结束255]
 *  [0f 00 00 00] [0c 00 00 00]                   [02 00]    [00 f3] [02 f6] [ff]
 *        |             |                            |          |       |     |
 *     zlbytes        zltail                      entries      "2"     "5"   end
 * 
 *  zlbytes zltail条目“2”“5”结束
 *  前4个字节表示数字15，即字节数   0f000000 =15
 *  整个ziplist由。第二个4字节是偏移量 0c00000 = 12
 *  entry个数
 * 来解析一下  [00 f3]
 *          [类型字符串]  [上一个entry长度0]
 *  00 ->   [00]        [000000]  
 *  
 *  f3 ->   1111 0011   由于1111 0011  在 （1111 0000, 11111110)  之间    即直接表示 2
 * 
 * 下面来表示字符串
 * 
 *  [前一个长度为2]   [00  00 0011]           [ASCII Hello World]
 *      |               |                          |  
 *    [02]            [0b]                  [48 65 6c 6c 6f 20 57 6f 72 6c 64]
 *   
 *             字符串      长度11位
 *    0b ->   [00]        [00 1011]
 * 
 *    "Hello World"   11位字符串
 *    
*/


typedef struct zlentry {
    //prevrawlensize为记录该长度数值所需要的字节数
    unsigned int prevrawlensize; 
    //prevrawlen为上一个数据结点的长度，
    unsigned int prevrawlen;    
    //len为当前数据结点的长度，
    unsigned int lensize;       /* 用于编码此条目类型/ len的字节。
                                     例如，字符串具有1,2或5个字节
                                    头。 整数总是使用单个字节。*/
    //lensize表示表示当前长度表示所需的字节数
    unsigned int len;            /* 用于表示实际条目的字节。
                                     对于字符串，这只是字符串长度
                                     而对于整数，它是1,2,3,4,8或
                                     0（立即为4位）取决于
                                     数字范围。 */
    //数据结点的头部信息长度的字节数
    unsigned int headersize;     /* 头字节 */
    //编码的方式  ZIP_STR_ 字符串   ZIP_INT_   数字
    unsigned char encoding;     
    //数据结点的数据(已包含头部等信息)，以字符串形式保存
    unsigned char *p;  
} zlentry;


/* <zlentry>的结构图线表示 <pre_node_len>(上一结点的长度信息)<node_encode>(本结点的编码方式和编码数据的长度信息)<node>(本结点的编码数据) */
unsigned char *ziplistNew(void);
unsigned char *ziplistMerge(unsigned char **first, unsigned char **second);
unsigned char *ziplistPush(unsigned char *zl, unsigned char *s, unsigned int slen, int where);
unsigned char *ziplistIndex(unsigned char *zl, int index);
unsigned int ziplistGet(unsigned char *p, unsigned char **sstr, unsigned int *slen, long long *sval);
unsigned char *ziplistDeleteRange(unsigned char *zl, int index, unsigned int num);
unsigned char *ziplistNext(unsigned char*zl, unsigned char *p);
unsigned char *ziplistPrev(unsigned char*zl, unsigned char *p);
unsigned int ziplistLen(unsigned char *zl);
//插入类型  2种
//头部插入
#define ZIPLIST_HEAD 0  
//尾部插入
#define ZIPLIST_TAIL 1

#define ZIP_INT_IMM_MASK 0x0f 

#endif /* _ZIPLIST_H */