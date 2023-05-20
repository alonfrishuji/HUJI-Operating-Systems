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
    pthread_mutex_t waitMutex;

    int multiThreadLevel;
    std::atomic<stage_t> stage;

    pthread_t** threads;
    ThreadContext** threadsContext;
    std::atomic<unsigned int> mapCounter;
    std::atomic<unsigned int> mapFinishedCounter;
    std::atomic<unsigned int> outputCounter;
    std::atomic<unsigned int> outputFinishedCounter;
    std::vector<IntermediateVec*> threadsInter;
    std::vector<IntermediateVec*> shuffledInter;



    ShuffleBarrier *shuffleBarrier;
    unsigned int interSize;
    unsigned int shuffleAmount;
    bool finished;
    

    JobContext(const MapReduceClient& client, const InputVec& inputVec, OutputVec& outputVec,
        int multiThreadLevel);
    ~JobContext();
};


#endif //JOB_CONTEXT_H