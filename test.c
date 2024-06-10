#include "logs.h"
#include "adt/hashmap.h"
#include "adt/array.h"
#include "adt/threadpool.h"

#include <string.h>

#define LENGTH(A) (sizeof(A) / sizeof(A[0]))

static inline size_t
hashFNV(const char* str)
{
    size_t hash = 0xCBF29CE484222325;
    size_t size = strlen(str);
    for (size_t i = 0; i < size; i++)
        hash = (hash ^ (size_t)str[i]) * 0x100000001B3;
    return hash;
}

[[maybe_unused]] static inline void
randomString(char* dest, size_t length)
{
    const char charset[] = "0123456789"
                           "abcdefghijklmnopqrstuvwxyz"
                           "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (size_t i = 0; i < length; i++)
    {
        size_t idx = rand() % (LENGTH(charset) - 1);
        dest[i] = charset[idx];
    }
}

typedef char* pChar;

HASHMAP_GEN_CODE(HashMap, List, pChar, hashFNV, strcmp, ADT_HASHMAP_DEFAULT_LOAD_FACTOR);
ARRAY_GEN_CODE(Array, pChar);
LIST_GEN_CODE(List, int, intCmp);

typedef ListNode_int* pListNode_int;
ARRAY_GEN_CODE(Array, pListNode_int);

QUEUE_GEN_CODE(QInt, int);

void
printMap(const HashMap_pChar* restrict pMap)
{
    COUT("mapS.size: %zu, cap: %zu\n", pMap->bucketCount, pMap->capacity);
    for (size_t i = 0; i < pMap->capacity; i++)
    {
        if (pMap->pBuckets[i].pFirst)
        {
            COUT("(bucket: %zu): ", i);
            for (auto it = pMap->pBuckets[i].pFirst; it; it = it->pNext)
                COUT("'%s' ", it->data);
            COUT("\n");
        }
    }
}

int
main()
{
    auto q = QIntCreate(1);
    QIntPush(&q, 1);
    QIntPush(&q, 2);
    QIntPush(&q, 3);
    QIntPush(&q, 4);
    QIntPush(&q, 5);
    QIntPush(&q, 6);

    for (long t = 0; t < q.size; t++)
        COUT("'%d', ", q.pData[t]);
    COUT("\n");

    QIntPop(&q);
    QIntPop(&q);

    for (long i = QIntFirstI(&q), t = 0; t < q.size; i = QIntNextI(&q, i), t++)
        COUT("'%d', ", q.pData[i]);
    COUT("\n");

    QIntClean(&q);

    return 0;
}
