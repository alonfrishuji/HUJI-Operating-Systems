#include "MapReduceFramework.h"
#include "JobContext.h"
#include <utility>
#include <algorithm>
#include <pthread.h>
#include <cstdio>

bool sort_inter(const IntermediatePair &left, const IntermediatePair &right) {
        return (*(left.first)) < (*(right.first));
}


bool inter_equal(K2 *right, K2 *left) {
    return (!((*left) < (*right))) && (!((*right) < (*left)));
}


void threadMap(ThreadContext *threadContext, JobContext *jobContext) {
    if (jobContext->stage.load() == UNDEFINED_STAGE) {
        jobContext->stage = MAP_STAGE;
    }
    while (true) {
        int mapIndex = jobContext->mapCounter++;
        if (mapIndex >= jobContext->inputVec.size()) {
            break;
        }
        const InputPair& inputPair = jobContext->inputVec[mapIndex];
        jobContext->client.map(inputPair.first, inputPair.second, threadContext);
        (jobContext->mapFinishedCounter)++;
    }
    int threadId = threadContext->threadId;
    IntermediateVec *interVec = jobContext->threadsInter[threadId]; 
    std::sort(interVec->begin(), interVec->end(), sort_inter);
}


K2 *getMaxKey(JobContext *jobContext) {
    K2* maxKey = nullptr;
    for (IntermediateVec* interVec: jobContext->threadsInter) {
        if (!interVec->empty()) {
            K2* curKey = interVec->back().first;
            if (maxKey == nullptr || (*maxKey) < (*curKey)) {
                maxKey = curKey;
            }
        }
    }
    return maxKey;
}


void popMaxKey(JobContext *jobContext, K2 *maxKey) {
    IntermediateVec *maxPairs = new IntermediateVec;
    for (IntermediateVec* interVec: jobContext->threadsInter) {
        while (!interVec->empty() && inter_equal(maxKey, interVec->back().first)) {
            maxPairs->push_back(interVec->back());
            interVec->pop_back();
            jobContext->shuffleAmount++;
        }
    }
    jobContext->shuffledInter.push_back(maxPairs);
}


void shuffle(JobContext *jobContext) {
    for (IntermediateVec* interVec: jobContext->threadsInter) {
        jobContext->interSize += interVec->size();
    }
    jobContext->stage = SHUFFLE_STAGE;
    while (jobContext->shuffleAmount < jobContext->interSize) {
        K2 *maxKey = getMaxKey(jobContext);
        popMaxKey(jobContext, maxKey);
    }
}


void reduce(ThreadContext *threadContext, JobContext *jobContext) {
    while (true) {
        int outputIdx = jobContext->outputCounter++;
        if (outputIdx >= jobContext->shuffledInter.size()) {
            break;
        }
        IntermediateVec *curVec  = jobContext->shuffledInter[outputIdx];
        int curVecSize = curVec->size();
        jobContext->client.reduce(curVec, threadContext);
        jobContext->outputFinishedCounter += curVecSize;
    }
}


void* runThread(void* arg) {
    ThreadContext *threadContext = (ThreadContext*) arg;
    JobContext *jobContext = threadContext->jobContext;
    threadMap(threadContext, jobContext);
    printf("thred %d reached barrier\n", threadContext->threadId);
    jobContext->shuffleBarrier->barrier(threadContext->threadId);
    if (threadContext->threadId == 0) {
        printf("shufeling shufeling\n");
        shuffle(jobContext);
        jobContext->stage = REDUCE_STAGE;
        jobContext->shuffleBarrier->afterShuffle();
    }
    reduce(threadContext, jobContext); 
    return 0;
}


JobHandle startMapReduceJob(const MapReduceClient& client,
	const InputVec& inputVec, OutputVec& outputVec,
	int multiThreadLevel) {
        JobContext *jobContext = new JobContext(client, inputVec, outputVec, multiThreadLevel);
        for (int threadIdx = 0; threadIdx < multiThreadLevel; threadIdx++) {
            ThreadContext *threadContext = new ThreadContext();
            threadContext->jobContext = jobContext;
            threadContext->threadId = threadIdx;
            jobContext->threadsContext[threadIdx] = threadContext;
            jobContext->threads[threadIdx] = new pthread_t;
            pthread_create(jobContext->threads[threadIdx], NULL, runThread, threadContext);
        }
        return static_cast<JobHandle>(jobContext);
    }


void emit2 (K2* key, V2* value, void* context) {
    ThreadContext *threadContext = (ThreadContext*) context;
    IntermediatePair pair(key, value);
    threadContext->jobContext->threadsInter[threadContext->threadId]->push_back(pair);
}


void emit3 (K3* key, V3* value, void* context) {
    ThreadContext *threadContext = (ThreadContext*) context;
    OutputPair pair(key, value);
    if (pthread_mutex_lock(&threadContext->jobContext->outpuMutex) != 0) {
		fprintf(stderr, "[[Barrier]] error on pthread_mutex_lock");
		exit(1);
	}
    threadContext->jobContext->outputVec.push_back(pair);
    if (pthread_mutex_unlock(&threadContext->jobContext->outpuMutex) != 0) {
		fprintf(stderr, "[[Barrier]] error on pthread_mutex_unlock");
		exit(1);
	}
}


float getPercentageRatio(int finish, int max) {
    return max != 0.f ? (((float)finish/max) * 100.f) : 100.f;
}


float getPercentage(stage_t stage, JobContext *jobContext) {
    switch (stage)
    {
    case UNDEFINED_STAGE:
        return 0.f;
    case MAP_STAGE:
        return getPercentageRatio(jobContext->mapFinishedCounter, jobContext->inputVec.size());
    case SHUFFLE_STAGE:
        return getPercentageRatio(jobContext->shuffleAmount, jobContext->interSize);
    case REDUCE_STAGE:
        return getPercentageRatio(jobContext->outputFinishedCounter, jobContext->interSize);
    default:
        return 0.f;
    }
}


void getJobState(JobHandle job, JobState* state) {
    JobContext *jobContext = (JobContext*) job;
    state->stage = jobContext->stage.load();
    state->percentage = getPercentage(state->stage, jobContext); 
}


void waitForJob(JobHandle job) {
    JobContext *jobContext = (JobContext*) job;
}


void closeJobHandle(JobHandle job) {}




