#include "JobContext.h"
#include <cstdio>
#include <cstdlib>
#include "Utils.h"

JobContext::JobContext( const MapReduceClient& client, const InputVec& inputVec, OutputVec& outputVec,
    int multiThreadLevel) : 
        client(client),
        inputVec(inputVec), 
        outputVec(outputVec),
        multiThreadLevel(multiThreadLevel),
        stage(UNDEFINED_STAGE),
        mapCounter(0),
        mapFinishedCounter(0),
        outputCounter(0),
        outputFinishedCounter(0),
        interSize(0),
        shuffleAmount(0),
        finished(false)
    {
        if (pthread_mutex_init(&outpuMutex, NULL) != 0) {
            exitErr("error on pthread_mutex_init");
        }
        if (pthread_mutex_init(&waitMutex, NULL) != 0) {
            exitErr("error on pthread_mutex_init");
        }

        threads = new pthread_t*[multiThreadLevel];
        threadsContext = new ThreadContext*[multiThreadLevel];
        shuffleBarrier = new ShuffleBarrier(multiThreadLevel);

        for (int threadId = 0; threadId < multiThreadLevel; threadId++) {
            threadsInter.push_back(new IntermediateVec);
        }
    }


JobContext::~JobContext() {
    delete shuffleBarrier;
    for (int threadId = 0; threadId < multiThreadLevel; threadId++) {
        delete threadsContext[threadId];
        delete threads[threadId];
        delete threadsInter[threadId];
    }
    delete[] threadsContext;
    delete[] threads;

    for (IntermediateVec *vec: shuffledInter) {
        delete vec;
    }

    if (pthread_mutex_destroy(&outpuMutex) != 0) {
        exitErr("error on pthread_mutex_destroy");
	}

    if (pthread_mutex_destroy(&waitMutex) != 0) {
		exitErr("error on pthread_mutex_destroy");
	}
}