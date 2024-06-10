#include "logs.h"
#include "adt/hashmap.h"
#include "adt/array.h"
#include "adt/threadpool.h"

#include <string.h>
#include <unistd.h>

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

QUEUE_GEN_CODE(IntQ, int);

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


atomic_int kek = 0;

int
hello0([[maybe_unused]] void* pArg)
{
    // sleep(2);
    long i = 0;
    while (i++ <= 999999999)
        ;
    COUT("%d, ", ++kek);
    return 0;
}

int
hello1([[maybe_unused]] void* pArg)
{
    // sleep(2);
    double i = 0;
    while (i++ <= 9999999.9f)
        ;
    COUT("%d, ", ++kek);
    return 0;
}

int
main()
{
    auto q = IntQCreate(1);
    IntQPush(&q, 1);
    IntQPush(&q, 2);
    IntQPush(&q, 3);
    IntQPush(&q, 4);
    IntQPush(&q, 5);
    IntQPush(&q, 6);

    for (long t = 0; t < q.size; t++)
        COUT("'%d', ", q.pData[t]);
    COUT("\n");

    IntQPop(&q);
    IntQPop(&q);

    for (long i = IntQFirstI(&q), t = 0; t < q.size; i = IntQNextI(&q, i), t++)
        COUT("'%d', ", q.pData[i]);
    COUT("\n");

    IntQClean(&q);

    COUT("available threads: %d\n", THREAD_NPROCS());
    auto tp = ThreadPoolCreate(THREAD_NPROCS());
    ThreadPoolStart(&tp);

    TaskNode j0 = {
        .pFn = hello0,
        .pArg = nullptr
    };
    TaskNode j1 = {
        .pFn = hello1,
        .pArg = nullptr
    };

    for (size_t i = 0; i < 50; i++)
        if (i % 2 == 0)
            ThreadPoolSubmit(&tp, j0);
        else
            ThreadPoolSubmit(&tp, j1);

    ThreadPoolWait(&tp);
    ThreadPoolStop(&tp);
    ThreadPoolClean(&tp);

    COUT("\n");

    return 0;
}
