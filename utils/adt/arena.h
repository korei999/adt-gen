#pragma once
#include "common.h"

#include <math.h>
#include <stdint.h>

#define ARENA_1K 1024
#define ARENA_4K (4 * ARENA_4K)
#define ARENA_1M (ARENA_1K * ARENA_1K)

#define ARENA_FIRST(A) ((A)->pFirst)
#define ARENA_NEXT(AB) ((AB)->pNext)
#define ARENA_FOREACH(A, IT) for (typeof(ARENA_FIRST(A)) (IT) = ARENA_FIRST(A); (IT); (IT) = ARENA_NEXT(IT))
#define ARENA_FOREACH_SAFE(A, IT, TMP) for (typeof(ARENA_FIRST(A)) (IT) = ARENA_FIRST(A), (TMP) = nullptr; (IT) && ((TMP) = ARENA_NEXT(IT), true); (IT) = (TMP))

typedef struct ArenaBlock
{
    struct ArenaBlock* pNext;
    size_t size;
    uint8_t pData[]; /* flexible array member */
} ArenaBlock;

typedef struct Arena
{
    ArenaBlock* pFirst;
    ArenaBlock* pLast;
    const size_t cap;
} Arena;

static inline size_t
ArenaAlignedSize(size_t bytes)
{
    size_t newSize = bytes;
    double mulOf = (double)newSize / (double)sizeof(long);
    size_t cMulOfl = ceil(mulOf);
    newSize = sizeof(long) * cMulOfl;

    return newSize;
}

static inline ArenaBlock*
ArenaBlockNew(size_t bytes)
{
    size_t alignedSize = ArenaAlignedSize(bytes);
    ArenaBlock* pNew = malloc(sizeof(ArenaBlock) + alignedSize);
    pNew->size = 0;
    pNew->pNext = nullptr;

    return pNew;
}

static inline Arena
ArenaCreate(size_t bytes)
{
    size_t alignedSize = ArenaAlignedSize(bytes);
    Arena a = {.cap = alignedSize};
    ArenaBlock* pNew = ArenaBlockNew(alignedSize);
    a.pLast = a.pFirst = pNew;

    return a;
}

static inline void
ArenaClean(Arena* a)
{
    ARENA_FOREACH_SAFE(a, it, tmp)
        free(it);
}

static inline void*
ArenaAlloc(Arena* a, size_t bytes)
{
    size_t alignedSize = ArenaAlignedSize(bytes);

    assert(alignedSize <= a->cap && "trying to allocate more than 1 arena block");

    void* pRBlock = nullptr;
    ArenaBlock* pLast = a->pLast;

    if (pLast->size + alignedSize > a->cap)
    {
        /* won't be null after reset */
        if (!pLast->pNext)
            pLast->pNext = ArenaBlockNew(a->cap);

        pLast = a->pLast = pLast->pNext;
    }

    pRBlock = &pLast->pData[pLast->size];
    pLast->size += alignedSize;

    return pRBlock;
}

static inline void
ArenaReset(Arena* a)
{
    ARENA_FOREACH(a, it)
        it->size = 0;

    a->pLast = a->pFirst;
}
