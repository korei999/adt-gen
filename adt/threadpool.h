#pragma once
#include "queue.h"

#include <stdatomic.h>
#include <threads.h>

typedef struct JobNode
{
    thrd_start_t pFn;
    void* pArg;
} TaskNode;

QUEUE_GEN_CODE(TaskQ, TaskNode);

typedef struct ThreadPool
{
    atomic_bool bDone;
    thrd_t* aThreads;
    size_t nThreads;
    cnd_t cndQ;
    mtx_t mtxQ;
    cnd_t cndWait;
    mtx_t mtxWait;
    TaskQ qJobs;
} ThreadPool;

bool
ThreadPoolBusy(ThreadPool* self)
{
    mtx_lock(&self->mtxQ);
    bool ret = self->qJobs.size > 0;
    mtx_unlock(&self->mtxQ);

    return ret;
}

static inline int
threadLoop(void* pData)
{
    ThreadPool* self = (ThreadPool*)pData;

    while (!self->bDone)
    {
        TaskNode j;
        {
            mtx_lock(&self->mtxQ);

            while (!(self->qJobs.size > 0 || self->bDone))
                cnd_wait(&self->cndQ, &self->mtxQ);

            if (self->bDone)
            {
                mtx_unlock(&self->mtxQ);
                return 0;
            }

            j = *TaskQFirst(&self->qJobs);
            TaskQPop(&self->qJobs);

            mtx_unlock(&self->mtxQ);
        }

        j.pFn(j.pArg);
        if (self->qJobs.size == 0)
            cnd_signal(&self->cndWait);
    }

    return 0;
}

static inline void
ThreadPoolSubmit(ThreadPool* self, TaskNode job)
{
    {
        mtx_lock(&self->mtxQ);
        TaskQPush(&self->qJobs, job);
        mtx_unlock(&self->mtxQ);
    }
    cnd_signal(&self->cndQ);
}

static inline ThreadPool
ThreadPoolCreate(size_t nThreads)
{
    ThreadPool tp;
    tp.bDone = false,
    tp.aThreads = (thrd_t*)calloc(nThreads, sizeof(thrd_t)),
    tp.nThreads = nThreads,
    cnd_init(&tp.cndQ);
    mtx_init(&tp.mtxQ, mtx_plain);
    cnd_init(&tp.cndWait);
    mtx_init(&tp.mtxWait, mtx_plain);
    tp.qJobs = TaskQCreate(nThreads);

    return tp;
}

static inline void
ThreadPoolClean(ThreadPool* self)
{
    free(self->aThreads);
    TaskQClean(&self->qJobs);
    cnd_destroy(&self->cndQ);
    mtx_destroy(&self->mtxQ);
    cnd_destroy(&self->cndWait);
    mtx_destroy(&self->mtxWait);
}

static inline void
ThreadPoolStart(ThreadPool* self)
{
    for (size_t i = 0; i < self->nThreads; i++)
        thrd_create(&self->aThreads[i], threadLoop, self);
}

static inline void
ThreadPoolWait(ThreadPool* self)
{
    if (ThreadPoolBusy(self))
    {
        mtx_lock(&self->mtxWait);
        cnd_wait(&self->cndWait, &self->mtxWait);
        mtx_unlock(&self->mtxWait);
    }
}

static inline void
ThreadPoolStop(ThreadPool* self)
{
    self->bDone = true;
    cnd_broadcast(&self->cndQ);
    for (size_t i = 0; i < self->nThreads; i++)
        thrd_join(self->aThreads[i], nullptr);
}
