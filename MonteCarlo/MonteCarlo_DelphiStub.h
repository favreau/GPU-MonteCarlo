#ifndef _MonteCarlo_DELPHISTUB_H_
#define _MonteCarlo_DELPHISTUB_H_

#include <windows.h>
#include "DLL_API.h"

// External Definitions
extern "C" MONTECARLO_API 
long createContainer( 
   HANDLE& hContainer, 
   long seriesSize,
   long containerSize, 
   long frequenciesSize );

extern "C" MONTECARLO_API 
long deleteContainer( const HANDLE hContainer );

extern "C" MONTECARLO_API 
long destroy_device( const HANDLE hContainer );

extern "C" MONTECARLO_API 
long runKernel( const HANDLE hContainer, double mu, double sigma, double range, int NbThreads );

extern "C" MONTECARLO_API 
long getObject( const HANDLE hContainer, long index, double& value );

extern "C" MONTECARLO_API 
long getPerformance( const HANDLE hContainer, long index, double& value );

extern "C" MONTECARLO_API 
long getFrequency( const HANDLE hContainer, long index, long& value );

extern "C" MONTECARLO_API 
long getSerie( const HANDLE hContainer, long index, double& value );

#if USE_CUDA
extern "C" MONTECARLO_API 
long deviceToHost( const HANDLE hContainer );
#endif // USE_CUDA

#endif _MonteCarlo_DELPHISTUB_H_
