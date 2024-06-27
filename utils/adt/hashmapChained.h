#pragma once
#include "list.h"

#define ADT_HASHMAP_CHAINED_DEFAULT_LOAD_FACTOR 2.0

#define HASHMAP_CHAINED_GEN_CODE(NAME, LIST, T, FNHASH, CMP, LOAD_FACTOR)                                              \
    /* NOTE: hashmap will also generate a list */                                                                      \
    LIST_GEN_CODE(LIST, T, CMP);                                                                                       \
                                                                                                                       \
    typedef struct NAME                                                                                                \
    {                                                                                                                  \
        LIST* pBuckets;                                                                                                \
        size_t bucketCount;                                                                                            \
        size_t entryCount;                                                                                             \
        size_t capacity;                                                                                               \
        Arena* pArena;                                                                                                 \
    } NAME;                                                                                                            \
                                                                                                                       \
    typedef struct NAME##ReturnNode                                                                                    \
    {                                                                                                                  \
        LIST##Node* pNode;                                                                                             \
        size_t hash;                                                                                                   \
        bool bInserted;                                                                                                \
    } NAME##ReturnNode;                                                                                                \
                                                                                                                       \
    static inline LIST##Node* NAME##Insert(NAME* self, T value);                                                       \
                                                                                                                       \
    [[maybe_unused]] static inline NAME NAME##Create(Arena* a, size_t cap)                                             \
    {                                                                                                                  \
        assert(cap > 0 && "cap should be > 0");                                                                        \
        NAME mapNew = {.bucketCount = 0, .capacity = cap, .entryCount = 0, .pArena = a};                               \
        mapNew.pBuckets = ArenaCalloc(a, cap, sizeof(LIST));                                                           \
        for (size_t i = 0; i < cap; i++)                                                                               \
            mapNew.pBuckets[i].pArena = a;                                                                             \
                                                                                                                       \
        return mapNew;                                                                                                 \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline double NAME##LoadFactor(NAME* self)                                                 \
    {                                                                                                                  \
        return ((double)self->entryCount / (double)self->bucketCount);                                                 \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline void NAME##Rehash(NAME* self, size_t cap)                                           \
    {                                                                                                                  \
        NAME newMap = NAME##Create(self->pArena, cap);                                                                 \
                                                                                                                       \
        for (size_t i = 0; i < self->capacity; i++)                                                                    \
            if (self->pBuckets[i].pFirst)                                                                              \
                for (LIST##Node* it = self->pBuckets[i].pFirst; it; it = it->pNext)                                    \
                    NAME##Insert(&newMap, it->data);                                                                   \
                                                                                                                       \
        *self = newMap;                                                                                                \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline LIST##Node* NAME##Insert(NAME* self, T value)                                       \
    {                                                                                                                  \
        if (NAME##LoadFactor(self) >= LOAD_FACTOR)                                                                     \
            NAME##Rehash(self, self->capacity * 2);                                                                    \
                                                                                                                       \
        size_t hash = FNHASH(value) % self->capacity;                                                                  \
                                                                                                                       \
        if (!self->pBuckets[hash].pFirst)                                                                              \
            self->bucketCount++;                                                                                       \
        self->entryCount++;                                                                                            \
                                                                                                                       \
        return LIST##PushBack(&self->pBuckets[hash], value);                                                           \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline NAME##ReturnNode NAME##Search(NAME* self, T value)                                  \
    {                                                                                                                  \
        size_t hash = FNHASH(value) % self->capacity;                                                                  \
        LIST##Node* pNode = LIST##Search(&self->pBuckets[hash], value);                                                \
                                                                                                                       \
        return (NAME##ReturnNode) {.pNode = pNode, .hash = hash, .bInserted = false};                                  \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline NAME##ReturnNode NAME##TryInsert(NAME* self, T value)                               \
    {                                                                                                                  \
        NAME##ReturnNode tried = NAME##Search(self, value);                                                            \
                                                                                                                       \
        if (tried.pNode)                                                                                               \
        {                                                                                                              \
            tried.bInserted = false;                                                                                   \
            return tried;                                                                                              \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            tried.pNode = NAME##Insert(self, value);                                                                   \
            tried.bInserted = true;                                                                                    \
            return tried;                                                                                              \
        }                                                                                                              \
    }
