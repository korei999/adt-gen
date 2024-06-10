#pragma once

#include "common.h"

#define ARRAY_GEN_CODE(NAME, T)                                                                                        \
    /* NAME: prefix, T: of T type + ending */                                                                          \
    typedef struct NAME##_##T                                                                                          \
    {                                                                                                                  \
        T* pData;                                                                                                      \
        size_t size;                                                                                                   \
        size_t capacity;                                                                                               \
    } NAME##_##T;                                                                                                      \
                                                                                                                       \
    [[maybe_unused]] static inline NAME##_##T NAME##Create_##T(size_t cap)                                             \
    {                                                                                                                  \
        assert(cap > 0);                                                                                               \
        return (NAME##_##T) {.pData = (T*)calloc(cap, sizeof(T)), .size = 0, .capacity = cap};                         \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline void NAME##Push_##T(NAME##_##T* restrict self, T value)                             \
    {                                                                                                                  \
        if (self->size >= self->capacity)                                                                              \
        {                                                                                                              \
            self->capacity *= 2;                                                                                       \
            self->pData = (T*)reallocarray(self->pData, self->capacity, sizeof(T));                                    \
        }                                                                                                              \
                                                                                                                       \
        self->pData[self->size++] = value;                                                                             \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline void NAME##Clean_##T(NAME##_##T* restrict self)                                     \
    {                                                                                                                  \
        free(self->pData);                                                                                             \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline T* NAME##Pop_##T(NAME##_##T* restrict self)                                         \
    {                                                                                                                  \
        assert(self->size > 0 && "poping empty array");                                                                \
        return &self->pData[--self->size];                                                                             \
    }
