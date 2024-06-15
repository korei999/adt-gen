#include "utils/adt/array.h"
#include "utils/adt/hashmapChained.h"
#include "utils/logs.h"
#include "utils/misc.h"

typedef char* pChar;
HASHMAP_CHAINED_GEN_CODE(BorkedMap, ListStr, pChar, hashMurmurOAAT64, strcmp, ADT_HASHMAP_CHAINED_DEFAULT_LOAD_FACTOR);
ARRAY_GEN_CODE(ArrayStr, pChar);

#define SIZE 10

void
printMap(const BorkedMap* pMap)
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
    // auto tt = time(NULL);
    // COUT("tt: %zu\n", tt);
    srand(1718453469);

    auto m0 = BorkedMapCreate(1);
    auto aC = ArrayStrCreate(SIZE);

    for (size_t i = 0; i < SIZE; i++)
    {
        char* rStr = randomString((rand() % (20 - 2)) + 2);
        ArrayStrPush(&aC, rStr);
        BorkedMapInsert(&m0, rStr);
    }

    for (size_t i = 0; i < SIZE; i++)
    {
        auto f = BorkedMapSearch(&m0, aC.pData[i]);
        COUT("rStr: '%s', f: '%s'\n", f.pNode ? f.pNode->data : "(nill)", aC.pData[i]);
        if (!f.pNode || strcmp(f.pNode->data, aC.pData[i]) != 0)
            COUT("\t '%s': not found\n\n", aC.pData[i]);
    }

    auto Y = BorkedMapSearch(&m0, "Y");
    COUT("Y '%s'\n", Y.pNode ? "found" : "not found");
    auto PYIiqXyDCU = BorkedMapSearch(&m0, "PYIiqXyDCU");
    COUT("PYIiqXyDCU '%s'\n", PYIiqXyDCU.pNode ? "found" : "not found");

    auto fl = ListStrSearch(&m0.pBuckets[3], "Y");
    COUT("Y from list '%s': '%s'\n", fl ? "found" : "not found", fl ? fl->data : "(nill)");


    printMap(&m0);

    for (size_t i = 0; i < SIZE; i++)
        free(aC.pData[i]);
    ArrayStrClean(&aC);
    BorkedMapClean(&m0);
}
