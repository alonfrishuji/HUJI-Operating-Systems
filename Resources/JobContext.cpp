#include "JobContext.h"

JobContext::JobContext( const MapReduceClient& client, const InputVec& inputVec, OutputVec& outputVec,
    int multiThreadLevel) : 
        client(client),
        inputVec(inputVec), 
        outputVec(outputVec),
        multiThreadLevel(multiThreadLevel),
        stage(UNDEFINED_STAGE),
        mapCounter(0)
    {
        threads = new pthread_t*[multiThreadLevel];
        threadsContext = new ThreadContext*[multiThreadLevel];
        threadsInter = new std::vector<IntermediatePair>[multiThreadLevel];
    }