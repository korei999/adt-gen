#pragma once
#include "common.h"

#define LIST_GEN_CODE(NAME, T, CMP)                                                                                    \
    /* NAME: prefix, T: of T type + suffix */                                                                          \
    typedef struct NAME##Node                                                                                          \
    {                                                                                                                  \
        T data;                                                                                                        \
        struct NAME##Node* pNext;                                                                                      \
        struct NAME##Node* pPrev;                                                                                      \
    } NAME##Node;                                                                                                      \
                                                                                                                       \
    typedef struct NAME                                                                                                \
    {                                                                                                                  \
        NAME##Node* pFirst;                                                                                            \
        NAME##Node* pLast;                                                                                             \
        size_t size;                                                                                                   \
    } NAME;                                                                                                            \
                                                                                                                       \
    [[maybe_unused]] static inline NAME NAME##Create()                                                                 \
    {                                                                                                                  \
        return (NAME) {.pFirst = NULL, .pLast = NULL, .size = 0};                                                      \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline NAME##Node* NAME##PushBack(NAME* self, T value)                                     \
    {                                                                                                                  \
        NAME##Node* pNew = (NAME##Node*)calloc(1, sizeof(NAME##Node));                                                 \
        pNew->data = value;                                                                                            \
                                                                                                                       \
        if (!self->pFirst)                                                                                             \
        {                                                                                                              \
            self->pLast = self->pFirst = pNew;                                                                         \
            self->size = 1;                                                                                            \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            self->pLast->pNext = pNew;                                                                                 \
            pNew->pPrev = self->pLast;                                                                                 \
            self->pLast = pNew;                                                                                        \
            self->size++;                                                                                              \
        }                                                                                                              \
                                                                                                                       \
        return pNew;                                                                                                   \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline void NAME##Clean(NAME* self)                                                        \
    {                                                                                                                  \
        for (NAME##Node* it = self->pFirst; it;)                                                                       \
        {                                                                                                              \
            NAME##Node* pSNext = it->pNext;                                                                            \
            free(it);                                                                                                  \
            it = pSNext;                                                                                               \
        }                                                                                                              \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline NAME##Node* NAME##Search(NAME* self, T value)                                       \
    {                                                                                                                  \
        for (NAME##Node* it = self->pFirst; it; it = it->pNext)                                                        \
        {                                                                                                              \
            if (CMP(it->data, value) == 0)                                                                             \
                return it;                                                                                             \
        }                                                                                                              \
        return NULL;                                                                                                   \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline void NAME##Remove(NAME* self, NAME##Node* restrict pNode)                           \
    {                                                                                                                  \
        if (pNode == self->pFirst)                                                                                     \
        {                                                                                                              \
            self->pFirst = self->pFirst->pNext;                                                                        \
            if (self->pFirst)                                                                                          \
                self->pFirst->pPrev = NULL;                                                                            \
        }                                                                                                              \
        else if (pNode == self->pLast)                                                                                 \
        {                                                                                                              \
            self->pLast = self->pLast->pPrev;                                                                          \
            if (self->pLast)                                                                                           \
                self->pLast->pNext = NULL;                                                                             \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            pNode->pPrev->pNext = pNode->pNext;                                                                        \
            pNode->pNext->pPrev = pNode->pPrev;                                                                        \
        }                                                                                                              \
                                                                                                                       \
        free(pNode);                                                                                                   \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline void NAME##RemoveValue(NAME* self, T value)                                         \
    {                                                                                                                  \
        NAME##Remove(self, NAME##Search(self, value));                                                                 \
    }
