#include <iostream>
#include <sys/time.h>
#include "osm.h"
using namespace std;
#pragma GCC diagnostic ignored "-Wunused-value"

/*
osm_operation_time : calc difference between current_time
and end_time (being calculated after executing the Addition Action)
and returning the computed mesurement in nano-sec.
Loop-unrolling being used.
*/
double osm_operation_time(unsigned int iterations){
    if (iterations == 0)
    {
        return -1;
    }
    struct timeval current_time;
    struct timeval end_time;
    gettimeofday(&current_time, NULL);
    // unrooling loop
    for (size_t i = 0; i < iterations; i+=10)
    {
        1+1;
        1+1;
        1+1;
        1+1;
        1+1;
        1+1;
        1+1;
        1+1;
        1+1;
        1+1;
    }
    gettimeofday(&end_time, NULL);
    double s_to_ns = 1e9 / (double)iterations;
    double ms_to_ns = 1e3 / (double)iterations;
    double nano_exceute_operation = ((end_time.tv_sec - current_time.tv_sec)* s_to_ns
     + (end_time.tv_usec - current_time.tv_usec)* ms_to_ns);  
    return (nano_exceute_operation);
}
void func(){}


/*
osm_operation_time : calc difference between current_time
and end_time (being calculated after executing the NoArgsFuncCall Action)
and returning the computed mesurement in nano-sec.
Loop-unrolling being used.
*/
double osm_function_time(unsigned int iterations){
    if (iterations == 0)
    {
        return -1;
    }
    struct timeval current_time;
    struct timeval end_time;
    gettimeofday(&current_time, NULL);
    for (size_t i = 0; i < iterations; i+=10)
    {
        func();
        func();
        func();
        func();
        func();
        func();
        func();
        func();
        func();
        func();
    }
    gettimeofday(&end_time, NULL);
    double s_to_ns = 1e9 / (double)iterations;
    double ms_to_ns = 1e3 / (double)iterations;
    double nano_exceute_operation = ((end_time.tv_sec -current_time.tv_sec)* s_to_ns
     + (end_time.tv_usec -current_time.tv_usec)* ms_to_ns);  
    return (nano_exceute_operation);
}

/*
osm_operation_time : calc difference between current_time
and end_time (being calculated after executing the TrapCall Action)
and returning the computed mesurement in nano-sec.
Loop-unrolling being used.
*/
double osm_syscall_time(unsigned int iterations){
    if (iterations == 0)
    {
        return -1;
    }
    struct timeval current_time;
    struct timeval end_time;
    gettimeofday(&current_time, NULL);
    for (size_t i = 0; i < iterations; i+=10)
    {
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;

    }
    gettimeofday(&end_time, NULL);
    double s_to_ns = 1e9 / (double)iterations;
    double ms_to_ns = 1e3 / (double)iterations;
    double nano_exceute_operation = ((end_time.tv_sec - current_time.tv_sec)* s_to_ns
     + (end_time.tv_usec -current_time.tv_usec)* ms_to_ns);  
    return (nano_exceute_operation);

}

int main()
{
    double addition = osm_operation_time(100000000);
    double no_args_func_call = osm_function_time(100000000);
    double sys_call = osm_syscall_time(100000000);
    std::cout << addition << endl;
    std::cout << no_args_func_call << endl;
    std::cout << sys_call << endl;
}