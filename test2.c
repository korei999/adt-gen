#include "utils/adt/array.h"
#include "utils/adt/hashmap.h"
#include "utils/adt/hashmap2.h"
#include "utils/adt/threadpool.h"
#include "utils/logs.h"
#include "utils/misc.h"
#include "utils/time.h"

static inline char*
randomString(size_t length)
{
    const char charset[] = "0123456789"
                           "abcdefghijklmnopqrstuvwxyz"
                           "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    char* ret = calloc(length, sizeof(char));

    for (size_t i = 0; i < length - 1; i++)
    {
        size_t idx = rand() % (LENGTH(charset) - 1);
        ret[i] = charset[idx];
    }

    return ret;
}

typedef char* pChar;
HASHMAP_GEN_CODE(HashMapChStr, ListStr, pChar, hashMurmurOAAT64, strcmp, ADT_HASHMAP_DEFAULT_LOAD_FACTOR);
HASHMAP_GEN_CODE(HashMapChStrFNV, ListStrFNV, pChar, hashFNV, strcmp, ADT_HASHMAP_DEFAULT_LOAD_FACTOR);
HASHMAP2_GEN_CODE(HashMapStr, pChar, hashMurmurOAAT64, strcmp, ADT_HASHMAP2_DEFAULT_LOAD_FACTOR);
HASHMAP2_GEN_CODE(HashMapStrFNV, pChar, hashFNV, strcmp, ADT_HASHMAP2_DEFAULT_LOAD_FACTOR);
ARRAY_GEN_CODE(ArrayStr, pChar);

#define SIZE 10000

typedef struct Arg0
{
    HashMapChStr* pMap;
    ArrayStr* pArr;
    f64* pTime;
} Arg0;

typedef struct Arg1
{
    HashMapStr* pMap;
    ArrayStr* pArr;
    f64* pTime;
} Arg1;

typedef struct Arg2
{
    HashMapChStrFNV* pMap;
    ArrayStr* pArr;
    f64* pTime;
} Arg2;

typedef struct Arg3
{
    HashMapStrFNV* pMap;
    ArrayStr* pArr;
    f64* pTime;
} Arg3;

int
task0(void* data)
{
    Arg0* pA = data;

    f64 t0 = msTimeNow();
    for (size_t i = 0; i < SIZE; i++)
    {
        for (size_t i = 0; i < pA->pArr->size; i++)
        {
            auto p = HashMapChStrSearch(pA->pMap, pA->pArr->pData[i]);
            if (!p.pNode || strcmp(p.pNode->data, pA->pArr->pData[i]) != 0)
            {
                LOG_WARN("task0 failed\n");
                goto done;
            }
        }
    }

done:
    f64 t1 = msTimeNow();
    f64 tChained = t1 - t0;

    *pA->pTime = tChained;

    COUT("task0 done\n");
    return 0;
};

int
task1(void* data)
{
    Arg1* pA = data;

    f64 t0 = msTimeNow();
    for (size_t i = 0; i < SIZE; i++)
    {
        for (size_t i = 0; i < pA->pArr->size; i++)
        {
            auto p = HashMapStrSearch(pA->pMap, pA->pArr->pData[i]);
            if (!p.pData || strcmp(*p.pData, pA->pArr->pData[i]) != 0)
            {
                LOG_WARN("task1 failed\n");
                goto done;
            }
        }
    }

done:
    f64 t1 = msTimeNow();
    f64 tLinear = t1 - t0;

    *pA->pTime = tLinear;

    COUT("task1 done\n");
    return 0;
};

int
task2(void* data)
{
    Arg2* pA = data;

    f64 t0 = msTimeNow();
    for (size_t i = 0; i < SIZE; i++)
    {
        for (size_t i = 0; i < pA->pArr->size; i++)
        {
            auto p = HashMapChStrFNVSearch(pA->pMap, pA->pArr->pData[i]);
            if (!p.pNode || strcmp(p.pNode->data, pA->pArr->pData[i]) != 0)
            {
                LOG_WARN("task2 failed\n");
                goto done;
            }
        }
    }

done:
    f64 t1 = msTimeNow();
    f64 tLinear = t1 - t0;

    *pA->pTime = tLinear;

    COUT("task2 done\n");
    return 0;
};

int
task3(void* data)
{
    Arg3* pA = data;

    f64 t0 = msTimeNow();
    for (size_t i = 0; i < SIZE; i++)
    {
        for (size_t i = 0; i < pA->pArr->size; i++)
        {
            auto p = HashMapStrFNVSearch(pA->pMap, pA->pArr->pData[i]);
            if (!p.pData || strcmp(*p.pData, pA->pArr->pData[i]) != 0)
            {
                LOG_WARN("task3 failed\n");
                goto done;
            }
        }
    }

done:
    f64 t1 = msTimeNow();
    f64 tLinear = t1 - t0;

    *pA->pTime = tLinear;

    COUT("task3 done\n");
    return 0;
};

int
main()
{
    auto hm0 = HashMapChStrCreate(SIZE * 3);
    COUT("hm0.capacity: %zu\n", hm0.capacity);
    auto hm1 = HashMapStrCreate(SIZE * 3);
    auto hm2 = HashMapChStrFNVCreate(SIZE * 3);
    auto hm3 = HashMapStrFNVCreate(SIZE * 3);

    auto aClean = ArrayStrCreate(SIZE);

    for (size_t i = 0; i < SIZE; i++)
    {
        char* rStr = randomString((rand() % 15) + 2);
        ArrayStrPush(&aClean, rStr);

        HashMapChStrInsert(&hm0, rStr);
        HashMapStrInsert(&hm1, rStr);
        HashMapChStrFNVInsert(&hm2, rStr);
        HashMapStrFNVInsert(&hm3, rStr);
    }

    auto tp = ThreadPoolCreate(hwConcurrency());

    f64 tCh = 0.0, tLin = 0.0, tChFNV = 0.0, tLinFNV = 0.0;

    Arg0 a0 = {
        .pTime = &tCh,
        .pMap = &hm0,
        .pArr = &aClean,
    };

    Arg1 a1 = {
        .pTime = &tLin,
        .pMap = &hm1,
        .pArr = &aClean,
    };

    Arg2 a2 = {
        .pTime = &tChFNV,
        .pMap = &hm2,
        .pArr = &aClean,
    };

    Arg3 a3 = {
        .pTime = &tLinFNV,
        .pMap = &hm3,
        .pArr = &aClean,
    };

    ThreadPoolStart(&tp);
    ThreadPoolSubmit(&tp, (TaskNode){.pFn = task0, .pArg = &a0});
    ThreadPoolSubmit(&tp, (TaskNode){.pFn = task1, .pArg = &a1});
    ThreadPoolSubmit(&tp, (TaskNode){.pFn = task2, .pArg = &a2});
    ThreadPoolSubmit(&tp, (TaskNode){.pFn = task3, .pArg = &a3});

    ThreadPoolWait(&tp);

    COUT("hm0(chaining MURMUR) spent searching: %lf ms\n", tCh);
    COUT("hm0.loadFactor: %lf\n", HashMapChStrGetLoadFactor(&hm0));

    COUT("hm1(linear probing MURMUR) spent searching: %lf ms\n", tLin);
    COUT("hm1.loadFactor: %lf\n", HashMapStrGetLoadFactor(&hm1));

    COUT("hm2(chaining FNV) spent searching: %lf ms\n", tChFNV);
    COUT("hm2.loadFactor: %lf\n", HashMapChStrFNVGetLoadFactor(&hm2));

    COUT("hm3(linear probing FNV) spent searching: %lf ms\n", tLinFNV);
    COUT("hm3.loadFactor: %lf\n", HashMapStrFNVGetLoadFactor(&hm3));

    HashMapChStrClean(&hm0);
    HashMapStrClean(&hm1);
    HashMapChStrFNVClean(&hm2);
    HashMapStrFNVClean(&hm3);

    for (size_t i = 0; i < aClean.size; i++)
        free(aClean.pData[i]);

    ArrayStrClean(&aClean);

    ThreadPoolStop(&tp);
    ThreadPoolClean(&tp);

    return 0;
}
