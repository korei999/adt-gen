#pragma once
#include "list.h"

static inline int
intCmp(int a, int b)
{
    if (a > b) return 1;
    else if (a < b) return -1;
    else return 0;
}

#define HASHMAP_DEFAULT_LOAD_FACTOR 2.0
#define HASHMAP_DEFAULT_SIZE 16

#define HASHMAP_GEN_CODE(NAME, LIST, T, FNHASH, CMP, LOAD_FACTOR)                                                      \
    /* NAME: prefix, T: of T type + suffix, NOTE: hashmap will also generate a list */                                 \
    /* (calls `LIST_GEN_CODE(LIST, T)`) */                                                                             \
    LIST_GEN_CODE(LIST, T, CMP);                                                                                       \
                                                                                                                       \
    typedef struct NAME##_##T                                                                                          \
    {                                                                                                                  \
        LIST##_##T* pBuckets;                                                                                          \
        size_t bucketCount;                                                                                            \
        size_t entryCount;                                                                                             \
        size_t capacity;                                                                                               \
    } NAME##_##T;                                                                                                      \
                                                                                                                       \
    typedef struct NAME##ReturnNode_##T                                                                                \
    {                                                                                                                  \
        LIST##Node_##T* pNode;                                                                                         \
        size_t hash;                                                                                                   \
    } NAME##ReturnNode_##T;                                                                                            \
                                                                                                                       \
    static inline LIST##Node_##T* NAME##Insert_##T(NAME##_##T* restrict self, T value);                                \
                                                                                                                       \
    [[maybe_unused]] static inline NAME##_##T NAME##Create_##T(size_t cap)                                             \
    {                                                                                                                  \
        assert(cap > 0 && "cap should be > 0");                                                                        \
        return (NAME##_##T) {                                                                                          \
            .pBuckets = (LIST##_##T*)calloc(cap, sizeof(LIST##_##T)), .bucketCount = 0, .capacity = cap};              \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline void NAME##Clean_##T(NAME##_##T* restrict self)                                     \
    {                                                                                                                  \
        for (size_t i = 0; i < self->capacity; i++)                                                                    \
            LIST##Clean_##T(&self->pBuckets[i]);                                                                       \
        free(self->pBuckets);                                                                                          \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline double NAME##GetLoadFactor_##T(NAME##_##T* restrict self)                           \
    {                                                                                                                  \
        return ((double)self->entryCount / (double)self->bucketCount);                                                 \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline void NAME##Rehash_##T(NAME##_##T* restrict self, size_t cap)                        \
    {                                                                                                                  \
        NAME##_##T newMap = NAME##Create_##T(cap);                                                                     \
                                                                                                                       \
        for (size_t i = 0; i < self->capacity; i++)                                                                    \
            for (LIST##Node_##T* it = self->pBuckets[i].pFirst; it; it = it->pNext)                                    \
                NAME##Insert_##T(&newMap, it->data);                                                                   \
                                                                                                                       \
        NAME##Clean_##T(self);                                                                                         \
        *self = newMap;                                                                                                \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline LIST##Node_##T* NAME##Insert_##T(NAME##_##T* restrict self, T value)                \
    {                                                                                                                  \
        size_t hash = FNHASH(value) % self->capacity;                                                                  \
        if (NAME##GetLoadFactor_##T(self) >= LOAD_FACTOR)                                                              \
            NAME##Rehash_##T(self, self->capacity * 2);                                                                \
                                                                                                                       \
        if (!self->pBuckets[hash].pFirst)                                                                              \
            self->bucketCount++;                                                                                       \
        self->entryCount++;                                                                                            \
                                                                                                                       \
        return LIST##PushBack_##T(&self->pBuckets[hash], value);                                                       \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline NAME##ReturnNode_##T NAME##Search_##T(NAME##_##T* restrict self, T value)           \
    {                                                                                                                  \
        size_t hash = FNHASH(value) % self->capacity;                                                                  \
        return (NAME##ReturnNode_##T) {.pNode = LIST##Search_##T(&self->pBuckets[hash], value), .hash = hash};         \
    }
