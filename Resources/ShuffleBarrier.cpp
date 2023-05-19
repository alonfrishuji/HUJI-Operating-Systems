#include "ShuffleBarrier.h"
#include <cstdlib>
#include <cstdio>
#include "Utils.h"

ShuffleBarrier::ShuffleBarrier(int numThreads)
		: mutex(PTHREAD_MUTEX_INITIALIZER)
		, zeroCond(PTHREAD_COND_INITIALIZER)
		, nonZeroCond(PTHREAD_COND_INITIALIZER)
		, count(0)
		, numThreads(numThreads) {}


ShuffleBarrier::~ShuffleBarrier()
{
	if (pthread_mutex_destroy(&mutex) != 0) {
		exitErr("error on pthread_mutex_destroy");
	}
	if (pthread_cond_destroy(&zeroCond) != 0){
		exitErr("error on pthread_cond_destroy");
	}
	if (pthread_cond_destroy(&nonZeroCond) != 0){
		exitErr("error on pthread_cond_destroy");
	}
}


void ShuffleBarrier::barrier(int threadId)
{
	if (pthread_mutex_lock(&mutex) != 0){
		exitErr("error on pthread_mutex_lock");
	}
	if (++count < numThreads) {
		pthread_cond_t& cv = (threadId == 0) ? zeroCond : nonZeroCond;
		if (pthread_cond_wait(&cv, &mutex) != 0) {
			exitErr("error on pthread_cond_wait");
		}
	} else {
		count = 0;
		if (threadId != 0) {
			if (pthread_cond_broadcast(&zeroCond) != 0) {
				exitErr("error on pthread_cond_broadcast");
			}
			if (pthread_cond_wait(&nonZeroCond, &mutex) != 0){
				exitErr("error on pthread_cond_wait");
			}
		}
	}
	if (pthread_mutex_unlock(&mutex) != 0) {
		exitErr("error on pthread_mutex_unlock");
	}
}


void ShuffleBarrier::afterShuffle() {
	if (pthread_cond_broadcast(&nonZeroCond) != 0) {
		exitErr("error on pthread_cond_broadcast");
	}
}
