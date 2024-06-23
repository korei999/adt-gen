#include "utils/adt/array.h"
#include "utils/adt/hashmap.h"
#include "utils/adt/hashmapChained.h"
#include "utils/adt/threadpool.h"
#include "utils/logs.h"
#include "utils/misc.h"
#include "ultratypes.h"

#include <string.h>

typedef char* pChar;
HASHMAP_CHAINED_GEN_CODE(HashMapChStr, ListStr, pChar, hashMurmurOAAT64, strcmp, ADT_HASHMAP_CHAINED_DEFAULT_LOAD_FACTOR);
HASHMAP_CHAINED_GEN_CODE(HashMapChStrFNV, ListStrFNV, pChar, hashFNV, strcmp, ADT_HASHMAP_CHAINED_DEFAULT_LOAD_FACTOR);
HASHMAP_GEN_CODE(HashMapStr, pChar, hashMurmurOAAT64, strcmp, ADT_HASHMAP_DEFAULT_LOAD_FACTOR);
HASHMAP_GEN_CODE(HashMapStrFNV, pChar, hashFNV, strcmp, ADT_HASHMAP_DEFAULT_LOAD_FACTOR);
ARRAY_GEN_CODE(ArrayStr, pChar);

#define SIZE 7777
#define MAX_STRING_SIZE 20

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
    Arg0 pA = *(Arg0*)data;

    f64 t0 = msTimeNow();
    for (size_t i = 0; i < SIZE; i++)
    {
        for (size_t i = 0; i < pA.pArr->size; i++)
        {
            auto p = HashMapChStrSearch(pA.pMap, pA.pArr->pData[i]);
            if (!p.pNode || strcmp(p.pNode->data, pA.pArr->pData[i]) != 0)
            {
                LOG_WARN("task0(CH) failed: {}, {}\n", !p.pNode ? "(nill)" : p.pNode->data, pA.pArr->pData[i]);
                goto done;
            }
        }
    }

done:
    f64 t1 = msTimeNow();
    f64 tChained = t1 - t0;

    *pA.pTime = tChained;

    COUT("task0 done\n", 0);
    return 0;
};

int
task1(void* data)
{
    Arg1 pA = *(Arg1*)data;

    f64 t0 = msTimeNow();
    for (size_t i = 0; i < SIZE; i++)
    {
        for (size_t i = 0; i < pA.pArr->size; i++)
        {
            auto p = HashMapStrSearch(pA.pMap, pA.pArr->pData[i]);
            if (!p.pData || strcmp(*p.pData, pA.pArr->pData[i]) != 0)
            {
                LOG_WARN("task1 failed\n", 0);
                goto done;
            }
        }
    }

done:
    f64 t1 = msTimeNow();
    f64 tLinear = t1 - t0;

    *pA.pTime = tLinear;

    COUT("task1 done\n", 0);
    return 0;
};

int
task2(void* data)
{
    Arg2 pA = *(Arg2*)data;

    f64 t0 = msTimeNow();
    for (size_t i = 0; i < SIZE; i++)
    {
        for (size_t i = 0; i < pA.pArr->size; i++)
        {
            auto p = HashMapChStrFNVSearch(pA.pMap, pA.pArr->pData[i]);
            if (!p.pNode || strcmp(p.pNode->data, pA.pArr->pData[i]) != 0)
            {
                LOG_WARN("task2(CH) failed: {}, {}\n", 2, !p.pNode ? "(nill)" : p.pNode->data, pA.pArr->pData[i]);
                goto done;
            }
        }
    }

done:
    f64 t1 = msTimeNow();
    f64 tLinear = t1 - t0;

    *pA.pTime = tLinear;

    COUT("task2 done\n", 0);
    return 0;
};

int
task3(void* data)
{
    Arg3 pA = *(Arg3*)data;

    f64 t0 = msTimeNow();
    for (size_t i = 0; i < SIZE; i++)
    {
        for (size_t i = 0; i < pA.pArr->size; i++)
        {
            auto p = HashMapStrFNVSearch(pA.pMap, pA.pArr->pData[i]);
            if (!p.pData || strcmp(*p.pData, pA.pArr->pData[i]) != 0)
            {
                LOG_WARN("task3 failed\n", 0);
                goto done;
            }
        }
    }

done:
    f64 t1 = msTimeNow();
    f64 tLinear = t1 - t0;

    *pA.pTime = tLinear;

    COUT("task3 done\n", 0);
    return 0;
};

int
main()
{
    srand(time(NULL));

    auto hm0 = HashMapChStrCreate(ADT_DEFAULT_SIZE);
    auto hm1 = HashMapStrCreate(ADT_DEFAULT_SIZE);
    auto hm2 = HashMapChStrFNVCreate(ADT_DEFAULT_SIZE);
    auto hm3 = HashMapStrFNVCreate(ADT_DEFAULT_SIZE);

    COUT("hm0.capacity: {}\n", hm0.capacity);

    auto aClean = ArrayStrCreate(SIZE);

    for (size_t i = 0; i < SIZE; i++)
    {
        char* rStr = randomString((rand() % (MAX_STRING_SIZE - 3)) + 3);
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

    LOG_OK("hm0(chaining MURMUR) spent searching: {} ms, loadFactor: {}, cap: {}\n", tCh, HashMapChStrLoadFactor(&hm0), hm0.capacity);
    LOG_OK("hm1(linear probing MURMUR) spent searching: {} ms, loadFactor: {}, cap: {}\n", tLin, HashMapStrLoadFactor(&hm1), hm1.capacity);
    LOG_OK("hm2(chaining FNV) spent searching: {} ms, loadFactor: {}, cap: {}\n", tChFNV, HashMapChStrFNVLoadFactor(&hm2), hm2.capacity);
    LOG_OK("hm3(linear probing FNV) spent searching: {} ms, loadFactor: {}, cap: {}\n", tLinFNV, HashMapStrFNVLoadFactor(&hm3), hm3.capacity);

    HashMapChStrClean(&hm0);
    HashMapStrClean(&hm1);
    HashMapChStrFNVClean(&hm2);
    HashMapStrFNVClean(&hm3);

    auto lL = ListStrCreate();

    for (size_t i = 0; i < aClean.size; i++)
        ListStrPushBack(&lL, aClean.pData[i]);

    for (size_t i = 0; i < aClean.size; i++)
        free(aClean.pData[i]);

    ListStrClean(&lL);
    ArrayStrClean(&aClean);

    ThreadPoolStop(&tp);
    ThreadPoolClean(&tp);

    return 0;
}
