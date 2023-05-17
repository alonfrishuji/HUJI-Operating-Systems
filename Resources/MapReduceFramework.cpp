#include "MapReduceFramework.h"
#include "JobContext.h"
#include <utility>

JobHandle startMapReduceJob(const MapReduceClient& client,
	const InputVec& inputVec, OutputVec& outputVec,
	int multiThreadLevel) {
        JobContext *jobContext = new JobContext(client, inputVec, outputVec, multiThreadLevel);
        for (int threadIdx = 0; threadIdx < multiThreadLevel; threadIdx++) {
            ThreadContext *threadContext = new ThreadContext();
            threadContext->jobContext = jobContext;
            threadContext->threadId = threadIdx;
            jobContext->threadsContext[threadIdx] = threadContext;
            pthread_create(jobContext->threads[threadIdx], NULL, runThread, threadContext);
        }
        return static_cast<JobHandle>(jobContext);
    }


void* runThread(void* arg) {
    ThreadContext *threadContext = (ThreadContext*) arg;
    JobContext *jobContext = threadContext->jobContext;
    threadMap(threadContext, jobContext);
    return 0;
}


void threadMap(ThreadContext *threadContext, JobContext *jobContext) {
    while (true) {
        int mapIndex = (jobContext->mapCounter)++;
        if (mapIndex >= jobContext->inputVec.size()) {
            break;
        }
        const InputPair& inputPair = jobContext->inputVec[mapIndex];
        jobContext->client.map(inputPair.first, inputPair.second, threadContext);
    }
}


void emit2 (K2* key, V2* value, void* context) {
    ThreadContext *threadContext = (ThreadContext*) context;
    IntermediatePair pair(key, value);
    threadContext->jobContext->threadsInter[threadContext->threadId].push_back(pair);
}
