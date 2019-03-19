
#ifndef __DB_H
#define __DB_H
#include "../dict/dict.h"
#include "../adlist/adlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../sds/sds.h"
#include "object.h"




typedef struct redisDb {
    dict *keyspace;                 /* The keyspace for this DB */
    dict *expires;              /* Timeout of keys with a timeout set */
    dict *blocking_keys;        /* Keys with clients waiting for data (BLPOP)*/
    dict *ready_keys;           /* Blocked keys that received a PUSH */
    dict *watched_keys;         /* WATCHED keys for MULTI/EXEC CAS */
    int id;                     /* Database ID */
    long long avg_ttl;          /* Average TTL, just for stats */
    list *defrag_later;         /* List of key names to attempt to defrag one by one, gradually. */
} redisDb;

typedef struct readyList {
    redisDb *db;
    robj *key;
} readyList;
void signalKeyAsReady(redisDb *db, robj *key);


void dbAdd(redisDb *db, robj *key, robj *val);

#endif