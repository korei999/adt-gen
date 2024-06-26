#pragma once
#include "common.h"

#include <stdint.h>

#define ARENA_1K 1024
#define ARENA_4K 4 * 1024
#define ARENA_1M (ARENA_1K * ARENA_1K)

#define ARENA_FIRST(A) ((A)->pFirst)
#define ARENA_NEXT(AB) ((AB)->pNext)
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

static inline ArenaBlock*
ArenaNewBlock(size_t bytes)
{
    ArenaBlock* pNew = malloc(sizeof(ArenaBlock) + bytes);
    pNew->size = 0;
    pNew->pNext = nullptr;

    return pNew;
}

static inline Arena
ArenaCreate(size_t bytes)
{
    Arena a = {.cap = bytes};
    ArenaBlock* pNew = ArenaNewBlock(bytes);
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
    void* pRBlock = nullptr;
    ArenaBlock* pLast = a->pLast;

    if (pLast->size + bytes > a->cap)
    {
        pLast->pNext = ArenaNewBlock(a->cap);
        a->pLast = pLast->pNext;
    }

    pRBlock = &pLast->pData[pLast->size];
    pLast->size += bytes + 1;

    return pRBlock;
}
