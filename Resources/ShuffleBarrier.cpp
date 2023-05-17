#include "ShuffleBarrier.h"
#include <cstdlib>
#include <cstdio>

ShuffleBarrier::ShuffleBarrier(int numThreads)
		: mutex(PTHREAD_MUTEX_INITIALIZER)
		, zeroCond(PTHREAD_COND_INITIALIZER)
		, nonZeroCond(PTHREAD_COND_INITIALIZER)
		, count(0)
		, numThreads(numThreads)
{ }


ShuffleBarrier::~ShuffleBarrier()
{
	if (pthread_mutex_destroy(&mutex) != 0) {
		fprintf(stderr, "[[Barrier]] error on pthread_mutex_destroy");
		exit(1);
	}
	if (pthread_cond_destroy(&zeroCond) != 0){
		fprintf(stderr, "[[Barrier]] error on pthread_cond_destroy");
		exit(1);
	}
	if (pthread_cond_destroy(&nonZeroCond) != 0){
		fprintf(stderr, "[[Barrier]] error on pthread_cond_destroy");
		exit(1);
	}
}


void ShuffleBarrier::barrier(int threadId)
{
	if (pthread_mutex_lock(&mutex) != 0){
		fprintf(stderr, "[[Barrier]] error on pthread_mutex_lock");
		exit(1);
	}
	if (++count < numThreads) {
		pthread_cond_t& cv = (threadId == 0) ? zeroCond : nonZeroCond;
		if (pthread_cond_wait(&cv, &mutex) != 0){
			fprintf(stderr, "[[Barrier]] error on pthread_cond_wait");
			exit(1);
		}
	} else {
		count = 0;
		if (threadId != 0) {
			if (pthread_cond_broadcast(&zeroCond) != 0) {
				fprintf(stderr, "[[Barrier]] error on pthread_cond_broadcast");
				exit(1);
			}
			if (pthread_cond_wait(&nonZeroCond, &mutex) != 0){
				fprintf(stderr, "[[Barrier]] error on pthread_cond_wait");
				exit(1);
			}
		}
	}
	if (threadId == 0) {
		if (pthread_mutex_lock(&mutex) != 0) {
			fprintf(stderr, "[[Barrier]] error on pthread_mutex_lock");
			exit(1);
		}
	}
	if (pthread_mutex_unlock(&mutex) != 0) {
		fprintf(stderr, "[[Barrier]] error on pthread_mutex_unlock");
		exit(1);
	}
}


void ShuffleBarrier::afterShuffle() {
	if (pthread_cond_broadcast(&nonZeroCond) != 0) {
		fprintf(stderr, "[[Barrier]] error on pthread_cond_broadcast");
		exit(1);
	}
}
