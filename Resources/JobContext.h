#ifndef JOB_CONTEXT_H
#define JOB_CONTEXT_H

#include "MapReduceClient.h"
#include "MapReduceFramework.h"
#include <pthread.h>
#include <vector>
#include <atomic>

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
    int multiThreadLevel;
    stage_t stage;

    pthread_t** threads;
    ThreadContext** threadsContext;
    std::atomic<int> mapCounter;
    std::vector<IntermediatePair>* threadsInter;
    

    JobContext(const MapReduceClient& client, const InputVec& inputVec, OutputVec& outputVec,
        int multiThreadLevel);
    ~JobContext();
};


JobContext::~JobContext()
{
}



#endif //JOB_CONTEXT_H