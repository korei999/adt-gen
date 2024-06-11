#pragma once
#include "list.h"

#define ADT_HASHMAP_DEFAULT_LOAD_FACTOR 2.0

#define HASHMAP_GEN_CODE(NAME, LIST, T, FNHASH, CMP, LOAD_FACTOR)                                                      \
    /* NAME: prefix, T: of T type + suffix, NOTE: hashmap will also generate a list */                                 \
    LIST_GEN_CODE(LIST, T, CMP);                                                                                       \
                                                                                                                       \
    typedef struct NAME                                                                                                \
    {                                                                                                                  \
        LIST* pBuckets;                                                                                                \
        size_t bucketCount;                                                                                            \
        size_t entryCount;                                                                                             \
        size_t capacity;                                                                                               \
    } NAME;                                                                                                            \
                                                                                                                       \
    typedef struct NAME##ReturnNode                                                                                    \
    {                                                                                                                  \
        LIST##Node* pNode;                                                                                             \
        size_t hash;                                                                                                   \
    } NAME##ReturnNode;                                                                                                \
                                                                                                                       \
    static inline LIST##Node* NAME##Insert(NAME* self, T value);                                                       \
                                                                                                                       \
    [[maybe_unused]] static inline NAME NAME##Create(size_t cap)                                                       \
    {                                                                                                                  \
        assert(cap > 0 && "cap should be > 0");                                                                        \
        return (NAME) {.pBuckets = (LIST*)calloc(cap, sizeof(LIST)), .bucketCount = 0, .capacity = cap};               \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline void NAME##Clean(NAME* restrict self)                                               \
    {                                                                                                                  \
        for (size_t i = 0; i < self->capacity; i++)                                                                    \
            LIST##Clean(&self->pBuckets[i]);                                                                           \
        free(self->pBuckets);                                                                                          \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline double NAME##GetLoadFactor(NAME* self)                                              \
    {                                                                                                                  \
        return ((double)self->entryCount / (double)self->bucketCount);                                                 \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline void NAME##Rehash(NAME* restrict self, size_t cap)                                  \
    {                                                                                                                  \
        NAME newMap = NAME##Create(cap);                                                                               \
                                                                                                                       \
        for (size_t i = 0; i < self->capacity; i++)                                                                    \
            for (LIST##Node* it = self->pBuckets[i].pFirst; it; it = it->pNext)                                        \
                NAME##Insert(&newMap, it->data);                                                                       \
                                                                                                                       \
        NAME##Clean(self);                                                                                             \
        *self = newMap;                                                                                                \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline LIST##Node* NAME##Insert(NAME* self, T value)                                       \
    {                                                                                                                  \
        size_t hash = FNHASH(value) % self->capacity;                                                                  \
        if (NAME##GetLoadFactor(self) >= LOAD_FACTOR)                                                                  \
            NAME##Rehash(self, self->capacity * 2);                                                                    \
                                                                                                                       \
        if (!self->pBuckets[hash].pFirst)                                                                              \
            self->bucketCount++;                                                                                       \
        self->entryCount++;                                                                                            \
                                                                                                                       \
        return LIST##PushBack(&self->pBuckets[hash], value);                                                           \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline NAME##ReturnNode NAME##Search(NAME* restrict self, T value)                         \
    {                                                                                                                  \
        size_t hash = FNHASH(value) % self->capacity;                                                                  \
        return (NAME##ReturnNode) {.pNode = LIST##Search(&self->pBuckets[hash], value), .hash = hash};                 \
    }
