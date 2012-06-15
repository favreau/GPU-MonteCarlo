#ifndef _CUDASPECIFICHEADERS_H_
#define _CUDASPECIFICHEADERS_H_

#include "DLL_API.h"

/*
* @brief Copy memory from Device to Host
* @param hContainer Pointer to the buffer of REALs 
* @param hContainer Size of buffer
*/
extern "C" MONTECARLO_API
void initialize_device( int NbSeries, int NbIterations, int NbFrequencies );
/*
* @brief Run kernel
*/
extern "C" MONTECARLO_API
void run_kernel( REAL range, REAL mu, REAL sigma, int random, int nbThreadsPerBlock );

/*
* @brief Release memory from Device
*/
extern "C" MONTECARLO_API
void destroy_device();

/*
* @brief Copy memory from Device to Host
* @param hContainer Pointer to the buffer of REALs 
* @param hContainer Size of buffer
*/
extern "C" MONTECARLO_API
void device_to_host( int* hOutFrequencies, REAL* hOutPerformances, REAL* hOutSeries );

/*
* @brief Copy memory from Device to Host
*/
extern "C" 
void host_to_device( REAL* hOutObjects );

#endif // _CUDASPECIFICHEADERS_H_
