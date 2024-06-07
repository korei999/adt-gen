#include "logs.h"
#include "adt/hashmap.h"
#include "adt/array.h"

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

static inline void
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

HASHMAP_GEN_CODE(HashMap, List, pChar, hashFNV, strcmp, HASHMAP_DEFAULT_LOAD_FACTOR);
ARRAY_GEN_CODE(Array, pChar);
LIST_GEN_CODE(List, int, intCmp);

typedef ListNode_int* pListNode_int;
ARRAY_GEN_CODE(Array, pListNode_int);

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
    auto mapS = HashMapCreate_pChar(HASHMAP_DEFAULT_SIZE);
    auto aCleanList = ArrayCreate_pChar(ARRAY_DEFAULT_SIZE);

    char aTmp[10];
    for (size_t i = 0; i < 50000; i++)
    {
        memset(aTmp, 0, LENGTH(aTmp));
        randomString(aTmp, (rand() % 3) + 5);
        char* pS = calloc(LENGTH(aTmp), sizeof(*pS));
        strcpy(pS, aTmp);
        ArrayPush_pChar(&aCleanList, HashMapInsert_pChar(&mapS, pS)->data);
    }
    HashMapInsert_pChar(&mapS, "KEKW123");

    // printMap(&mapS);
    COUT("load: %lf, bucketCount: %zu, entryCount: %zu, cap: %zu\n",
         HashMapGetLoadFactor_pChar(&mapS), mapS.bucketCount, mapS.entryCount, mapS.capacity);

    auto pFound = HashMapSearch_pChar(&mapS, "KEKW123");
    COUT("pFound: '%s', hash: %zu\n", pFound.pNode ? pFound.pNode->data : "(nill)", pFound.hash);

    for (size_t i = 0; i < aCleanList.size; i++)
        free(aCleanList.pData[i]);
    ArrayClean_pChar(&aCleanList);

    HashMapClean_pChar(&mapS);

    auto listInt = ListCreate_int();
    auto aInts = ArrayCreate_pListNode_int(10);

    for (int i = 0; i < 10; i++)
        ArrayPush_pListNode_int(&aInts, ListPushBack_int(&listInt, i));

    for (auto it = listInt.pFirst; it; it = it->pNext)
        COUT("%d, ", it->data);
    COUT("\n");

    ListRemove_int(&listInt, aInts.pData[0]);
    ListRemove_int(&listInt, aInts.pData[2]);
    ListRemove_int(&listInt, aInts.pData[aInts.size - 1]);
    ListRemove_int(&listInt, aInts.pData[4]);
    ListRemoveValue_int(&listInt, 6);
    ListRemoveValue_int(&listInt, 8);

    for (auto it = listInt.pFirst; it; it = it->pNext)
        COUT("%d, ", it->data);
    COUT("\n");

    ListClean_int(&listInt);
    ArrayClean_pListNode_int(&aInts);

    return 0;
}
