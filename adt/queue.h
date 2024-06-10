#pragma once
#include "common.h"

#define QUEUE_GEN_CODE(NAME, T)                                                                                        \
    /* NAME: prefix, T: of T type + ending */                                                                          \
    typedef struct NAME                                                                                                \
    {                                                                                                                  \
        T* pData;                                                                                                      \
        long size;                                                                                                     \
        long capacity;                                                                                                 \
        long first;                                                                                                    \
        long last;                                                                                                     \
    } NAME;                                                                                                            \
                                                                                                                       \
    [[maybe_unused]] static inline void NAME##Push(NAME* self, T data);                                                \
                                                                                                                       \
    [[maybe_unused]] static inline NAME NAME##Create(size_t cap)                                                       \
    {                                                                                                                  \
        assert(cap > 0);                                                                                               \
        return (NAME) {.pData = (T*)calloc(cap, sizeof(T)), .size = 0, .capacity = cap, .first = 0, .last = 0};        \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline void NAME##Clean(NAME* self)                                                        \
    {                                                                                                                  \
        free(self->pData);                                                                                             \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline long NAME##FirstI(NAME* self)                                                       \
    {                                                                                                                  \
        return self->first;                                                                                            \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline long NAME##LastI(NAME* self)                                                        \
    {                                                                                                                  \
        if (self->size == 0)                                                                                           \
            return 0;                                                                                                  \
        else                                                                                                           \
            return self->last - 1;                                                                                     \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline long NAME##NextI(NAME* self, long i)                                                \
    {                                                                                                                  \
        long next = 1 + i;                                                                                             \
        if (next >= self->capacity)                                                                                    \
            next = 0;                                                                                                  \
        return next;                                                                                                   \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline long NAME##PrevI(NAME* self, long i)                                                \
    {                                                                                                                  \
        long prev = i - 1;                                                                                             \
        if (prev < 0)                                                                                                  \
            prev = self->capacity - 1;                                                                                 \
        return prev;                                                                                                   \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline T* NAME##First(NAME* self)                                                          \
    {                                                                                                                  \
        assert(self->size > 0);                                                                                        \
        return &self->pData[self->first];                                                                              \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline T* NAME##Last(NAME* self)                                                           \
    {                                                                                                                  \
        assert(self->size > 0);                                                                                        \
        return &self->pData[self->last];                                                                               \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline void NAME##Resize(NAME* self, size_t size)                                          \
    {                                                                                                                  \
        NAME qNew = NAME##Create(size);                                                                                \
                                                                                                                       \
        for (long i = self->first, t = 0; t < self->size; i = NAME##NextI(self, i), t++)                               \
            NAME##Push(&qNew, self->pData[i]);                                                                         \
                                                                                                                       \
        NAME##Clean(self);                                                                                             \
        *self = qNew;                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline void NAME##Push(NAME* self, T data)                                                 \
    {                                                                                                                  \
        if (self->size >= self->capacity)                                                                              \
            NAME##Resize(self, self->capacity * 2);                                                                    \
                                                                                                                       \
        long i = self->last;                                                                                           \
        long ni = NAME##NextI(self, i);                                                                                \
        self->pData[i] = data;                                                                                         \
        self->last = ni;                                                                                               \
        self->size++;                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    [[maybe_unused]] static inline T* NAME##Pop(NAME* self)                                                            \
    {                                                                                                                  \
        assert(self->size > 0);                                                                                        \
        T* ret = &self->pData[self->first];                                                                            \
        self->first = NAME##NextI(self, self->first);                                                                  \
        self->size--;                                                                                                  \
        return ret;                                                                                                    \
    }
