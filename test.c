#include "utils/adt/array.h"
#include "utils/adt/hashmap.h"
#include "utils/adt/hashmapChained.h"
#include "utils/adt/threadpool.h"
#include "utils/logs.h"

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

static inline size_t
hashInt(int num)
{
    return num;
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
    for (size_t i = 0; i < length - 1; i++)
    {
        size_t idx = rand() % (LENGTH(charset) - 1);
        dest[i] = charset[idx];
    }
    dest[length - 1] = '\0';
}

typedef char* pChar;

HASHMAP_CHAINED_GEN_CODE(HashMapPChar, ListPChar, pChar, hashFNV, strcmp, ADT_HASHMAP_CHAINED_DEFAULT_LOAD_FACTOR);
ARRAY_GEN_CODE(ArrayPChar, pChar);
LIST_GEN_CODE(IntList, int, intCmp);

typedef IntList* pIntList;
ARRAY_GEN_CODE(ArrayPIntList, pIntList);

QUEUE_GEN_CODE(IntQ, int);

HASHMAP_GEN_CODE(HashMap2PChar, pChar, hashFNV, strcmp, ADT_HASHMAP_DEFAULT_LOAD_FACTOR);
HASHMAP_GEN_CODE(HashMapInt, int, hashInt, intCmp, ADT_HASHMAP_DEFAULT_LOAD_FACTOR);

void
printMap(const HashMapPChar* pMap)
{
    COUT("mapS.size: {}, cap: {}\n", pMap->bucketCount, pMap->capacity);
    for (size_t i = 0; i < pMap->capacity; i++)
    {
        if (pMap->pBuckets[i].pFirst)
        {
            COUT("(bucket: {}): ", i);
            for (auto it = pMap->pBuckets[i].pFirst; it; it = it->pNext)
                COUT("'{}' ", it->data);
            COUT("\n", 0);
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
    COUT("{}, ", ai);
    ai++;
    return 0;
}

int
hello1([[maybe_unused]] void* pArg)
{
    // sleep(2);
    // double i = 0;
    // while (i++ <= 19999999.9f)
        // ;
    COUT("{}, ", ai);
    ai++;
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
        COUT("'{}', ", q.pData[t]);
    COUT("\n", 0);

    IntQPop(&q);
    IntQPop(&q);

    QUEUE_FOREACH_I(&q, i)
        COUT("'{}', ", q.pData[i]);
    COUT("\n", 0);

    IntQClean(&q);

    COUT("available threads: {}\n", hwConcurrency());
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

    COUT("wait...\n", 0);
    ThreadPoolWait(&tp);
    ThreadPoolStop(&tp);
    ThreadPoolClean(&tp);

    COUT("\n", 0);

    auto hm = HashMapPCharCreate(1);
    HashMapPCharInsert(&hm, "what");
    HashMapPCharInsert(&hm, "is");
    HashMapPCharInsert(&hm, "kekw");
    HashMapPCharInsert(&hm, "the");
    HashMapPCharInsert(&hm, "then");

    auto found0 = HashMapPCharSearch(&hm, "what");
    COUT("found: '{}', hash: '{}'\n", found0.pNode ? found0.pNode->data : "(nill)", found0.hash);

    auto found1 = HashMapPCharSearch(&hm, "kekw");
    COUT("found: '{}', hash: '{}'\n", found1.pNode ? found1.pNode->data : "(nill)", found1.hash);

    auto t0 = HashMapPCharTryInsert(&hm, "asdf");
    auto t1 = HashMapPCharTryInsert(&hm, "what");
    COUT("'{}': %s, hash: %{}\n", t0.pNode ? t0.pNode->data : "(nill)", t0.bInserted ? "true" : "false", t0.hash);
    COUT("'{}': %s, hash: %{}\n", t1.pNode ? t1.pNode->data : "(nill)", t1.bInserted ? "true" : "false", t1.hash);

    printMap(&hm);

    HashMapPCharClean(&hm);

    auto hm2 = HashMap2PCharCreate(16);

    /* return nodes are invalidated if `Rehash()` has been called */
    auto what = HashMap2PCharInsert(&hm2, "what");
    auto what2 = HashMap2PCharInsert(&hm2, "what");
    COUT("what : '{}', hash: {}, i: {}\n", *what.pData, what.hash, what.idx);
    COUT("what2: '{}', hash: {}, i: {}\n", *what2.pData, what2.hash, what2.idx);
    auto f0 = HashMap2PCharSearch(&hm2, "what");
    COUT("f0: '{}'\n", *f0.pData);
    auto f1 = HashMap2PCharSearch(&hm2, "kekw");
    COUT("f1: '{}'\n", f1.pData ? *f1.pData : "(nill)");
    COUT("hm2.loadFactor: {}\n", HashMap2PCharLoadFactor(&hm2));

    /* doesn't actually removes but marks as unoccupied and deleted */
    HashMap2PCharRemove(&hm2, f0.idx);
    COUT("f0: '{}'\n", f0.pData ? *f0.pData : "(nill)");
    /* but will still find if wasn't replaced */
    auto f2 = HashMap2PCharSearch(&hm2, *f0.pData);
    COUT("f2: '{}'\n", f2.pData ? *f2.pData : "(nill)");

    HashMap2PCharClean(&hm2);

    auto hm3 = HashMapIntCreate(ADT_DEFAULT_SIZE);

    HashMapIntInsert(&hm3, 2);
    HashMapIntInsert(&hm3, 3);
    auto f3_0 = HashMapIntSearch(&hm3, 3);
    COUT("f3_0: '{}'\n", f3_0.pData ? *f3_0.pData : 0);

    HashMapIntClean(&hm3);

    return 0;
}
