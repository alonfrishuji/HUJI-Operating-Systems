#ifndef JOB_CONTEXT_H
#define JOB_CONTEXT_H

#include "MapReduceClient.h"
#include "MapReduceFramework.h"
#include "ShuffleBarrier.h"
#include <pthread.h>
#include <vector>
#include <atomic>

class JobContext;

struct ThreadContext {
    JobContext *jobContext;
    int threadId;
};

class JobContext
{
private:
    /* data */
    

public:
    const MapReduceClient& client;
    const InputVec& inputVec;
    OutputVec& outputVec;
    pthread_mutex_t outpuMutex;

    int multiThreadLevel;
    std::atomic<stage_t> stage;

    pthread_t** threads;
    ThreadContext** threadsContext;
    std::atomic_int mapCounter;
    std::atomic_int mapFinishedCounter;
    std::atomic_int outputCounter;
    std::atomic_int outputFinishedCounter;
    std::vector<IntermediateVec*> threadsInter;
    std::vector<IntermediateVec*> shuffledInter;



    ShuffleBarrier *shuffleBarrier;
    int interSize;
    int shuffleAmount;
    

    JobContext(const MapReduceClient& client, const InputVec& inputVec, OutputVec& outputVec,
        int multiThreadLevel);
};


#endif //JOB_CONTEXT_H