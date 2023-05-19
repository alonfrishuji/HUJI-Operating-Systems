#include "JobContext.h"

JobContext::JobContext( const MapReduceClient& client, const InputVec& inputVec, OutputVec& outputVec,
    int multiThreadLevel) : 
        client(client),
        inputVec(inputVec), 
        outputVec(outputVec),
        outpuMutex(PTHREAD_MUTEX_INITIALIZER),
        multiThreadLevel(multiThreadLevel),
        stage(UNDEFINED_STAGE),
        mapCounter(0),
        mapFinishedCounter(0),
        outputCounter(0),
        outputFinishedCounter(0),
        interSize(0),
        shuffleAmount(0)
    {
        threads = new pthread_t*[multiThreadLevel];
        threadsContext = new ThreadContext*[multiThreadLevel];
        shuffleBarrier = new ShuffleBarrier(multiThreadLevel);

        for (int threadId = 0; threadId < multiThreadLevel; threadId++) {
            threadsInter.push_back(new IntermediateVec);
        }
    }