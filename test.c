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

static inline int
intCmp(int a, int b)
{
    if (a > b) return 1;
    else if (a < b) return -1;
    else return 0;
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

HASHMAP_GEN_CODE(HashMapPChar, ListPChar, pChar, hashFNV, strcmp, ADT_HASHMAP_DEFAULT_LOAD_FACTOR);
ARRAY_GEN_CODE(ArrayPChar, pChar);
LIST_GEN_CODE(IntList, int, intCmp);

typedef IntList* pIntList;
ARRAY_GEN_CODE(ArrayPIntList, pIntList);

QUEUE_GEN_CODE(IntQ, int);

void
printMap(const HashMapPChar* pMap)
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

atomic_int ai = 0;

int
hello0([[maybe_unused]] void* pArg)
{
    // sleep(2);
    // long i = 0;
    // while (i++ <= 999999999)
        // ;
    COUT("%d, ", ++ai);
    return 0;
}

int
hello1([[maybe_unused]] void* pArg)
{
    // sleep(2);
    // double i = 0;
    // while (i++ <= 19999999.9f)
        // ;
    COUT("%d, ", ++ai);
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

    COUT("available threads: %d\n", hwConcurrency());
    auto tp = ThreadPoolCreate(hwConcurrency());
    ThreadPoolStart(&tp);

    TaskNode j0 = {
        .pFn = hello0,
        .pArg = nullptr
    };
    TaskNode j1 = {
        .pFn = hello1,
        .pArg = nullptr
    };

    for (size_t i = 0; i < 100; i++)
        if (i % 2 == 0)
            ThreadPoolSubmit(&tp, j0);
        else
            ThreadPoolSubmit(&tp, j1);

    COUT("wait...\n");
    ThreadPoolWait(&tp);
    ThreadPoolStop(&tp);
    ThreadPoolClean(&tp);

    COUT("\n");

    auto hm = HashMapPCharCreate(ADT_DEFAULT_SIZE);
    HashMapPCharInsert(&hm, "what");
    HashMapPCharInsert(&hm, "is");
    HashMapPCharInsert(&hm, "kekw");
    HashMapPCharInsert(&hm, "the");
    HashMapPCharInsert(&hm, "then");

    auto pFound0 = HashMapPCharSearch(&hm, "what");
    COUT("found: '%s', hash: '%zu'\n", pFound0.pNode ? pFound0.pNode->data : "(nill)", pFound0.hash);

    auto pFound1 = HashMapPCharSearch(&hm, "kekw");
    COUT("found: '%s', hash: '%zu'\n", pFound1.pNode ? pFound1.pNode->data : "(nill)", pFound1.hash);

    printMap(&hm);

    HashMapPCharClean(&hm);

    return 0;
}
