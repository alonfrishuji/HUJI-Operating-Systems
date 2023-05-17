#ifndef SHUFFLE_BARRIER_H
#define SHUFFLE_BARRIER_H
#include <pthread.h>

// a multiple use barrier

class ShuffleBarrier {
public:
	ShuffleBarrier(int numThreads);
	~ShuffleBarrier();
	void barrier(int threadIdx);
	void afterShuffle();

private:
	pthread_mutex_t mutex;
	pthread_cond_t zeroCond;
	pthread_cond_t nonZeroCond;
	int count;
	int numThreads;
};

#endif //SHUFFLE_BARRIER_H
