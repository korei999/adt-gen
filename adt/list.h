#pragma once

#include <stdlib.h>

#define LIST_GEN_CODE(NAME, T, CMP)                                                                                    \
    /* NAME: prefix, T: of T type + suffix */                                                                          \
    typedef struct NAME##Node_##T                                                                                      \
    {                                                                                                                  \
        T data;                                                                                                        \
        struct NAME##Node_##T* pNext;                                                                                  \
        struct NAME##Node_##T* pPrev;                                                                                  \
    } NAME##Node_##T;                                                                                                  \
                                                                                                                       \
    typedef struct NAME##_##T                                                                                          \
    {                                                                                                                  \
        struct NAME##Node_##T* pFirst;                                                                                 \
        struct NAME##Node_##T* pLast;                                                                                  \
        size_t size;                                                                                                   \
    } NAME##_##T;                                                                                                      \
                                                                                                                       \
    [[maybe_unused]] static inline NAME##_##T NAME##Create_##T()                                                       \
    {                                                                                                                  \
        return (NAME##_##T) {.pFirst = nullptr, .pLast = nullptr, .size = 0};                                          \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline NAME##Node_##T* NAME##PushBack_##T(NAME##_##T* restrict self, T value)              \
    {                                                                                                                  \
        NAME##Node_##T* pNew = (NAME##Node_##T*)calloc(1, sizeof(NAME##Node_##T));                                     \
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
    [[maybe_unused]] static inline void NAME##Clean_##T(NAME##_##T* restrict self)                                     \
    {                                                                                                                  \
        for (NAME##Node_##T* it = self->pFirst; it != nullptr;)                                                        \
        {                                                                                                              \
            NAME##Node_##T* pSNext = it->pNext;                                                                        \
            free(it);                                                                                                  \
            it = pSNext;                                                                                               \
        }                                                                                                              \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline NAME##Node_##T* NAME##Search_##T(NAME##_##T* restrict self, T value)                \
    {                                                                                                                  \
        for (NAME##Node_##T* it = self->pFirst; it; it = it->pNext)                                                    \
        {                                                                                                              \
            if (CMP(it->data, value) == 0)                                                                             \
                return it;                                                                                             \
        }                                                                                                              \
        return nullptr;                                                                                                \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline void NAME##Remove_##T(NAME##_##T* self, NAME##Node_##T* restrict pNode)             \
    {                                                                                                                  \
        if (pNode == self->pFirst)                                                                                     \
        {                                                                                                              \
            self->pFirst = self->pFirst->pNext;                                                                        \
            if (self->pFirst)                                                                                          \
                self->pFirst->pPrev = nullptr;                                                                         \
        }                                                                                                              \
        else if (pNode == self->pLast)                                                                                 \
        {                                                                                                              \
            self->pLast = self->pLast->pPrev;                                                                          \
            if (self->pLast)                                                                                           \
                self->pLast->pNext = nullptr;                                                                          \
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
    [[maybe_unused]] static inline void NAME##RemoveValue_##T(NAME##_##T* self, T value)                               \
    {                                                                                                                  \
        NAME##Remove_##T(self, NAME##Search_##T(self, value));                                                         \
    }
