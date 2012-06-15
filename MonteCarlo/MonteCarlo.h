#ifndef _MONTECARLO_H_
#define _MONTECARLO_H_

#include <windows.h>
#include <iostream>
#include <fstream>
#include "DLL_API.h"

#if  USE_TBB
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#endif // USE_TBB

#if USE_OPENCL
#include "OCLKernel.h"
#endif // USE_OPENCL

#if USE_PPL
#include "PPLKernel.h"
#endif // USE_PPL


/**
* @brief Class that:
* - Allocates memory on host and on device for CUDA implementation
* - Allows adding of business objects into the preallocated buffer
* - Runs computation of business objects
* - Releases memory
*/ 
class MONTECARLO_API CMonteCarlo 
{
public:
   // Default constructor/destructor
	CMonteCarlo();
	~CMonteCarlo();

public:
   /**
   * @brief Create a container by allocating the appropriate memory
   * @param size Size of container
   * @param buffer If specified, the container buffer is initialized 
   * from this paramter. This is typically used when creating a container
   * from a binary file
   */
	void createContainer(
      const int seriesSize,
      const int containerSize, 
      const int frequenciesSize);

public:
#ifndef USE_CUDA
   /**
   * @brief Get a business object from container
   * @param index Index in the buffer for the Business Object
   * @return A pointer to the REAL
   */
	REAL getObject( long index );
#endif // USE_CUDA

   /**
   * @brief Get a business object from container
   * @param index Index in the buffer for the Business Object
   * @return A pointer to the REAL
   */
	REAL getPerformance( long index );

   /**
   * @brief Get business frequencies from container
   * @param index Index in the buffer for the Business Object
   * @return A pointer to the REAL
   */
	int getFrequency( long index );

   /**
   * @brief Get business frequencies from container
   * @param index Index in the buffer for the Business Object
   * @return A pointer to the REAL
   */
	REAL getSerie( long index );
public:
   /**
   * @brief Runs the kernel code of every Object
   * in the container
   */
	void runKernel( REAL mu, REAL sigma, REAL range, int NbThreads );

#if USE_CUDA
public:
   /**
   * @brief Copy the buffer from Host to Device
   */
	void initializeDevice();

   /**
   * @brief Copy the buffer from Device to Host
   */
   void deviceToHost();

   /**
   * @brief Copy the buffer from Host to Device
   */
   void hostToDevice();
#endif // USE_CUDA

private:
	int m_objectsSize;    // Size of the container
   int m_frequenciesSize;
   int m_seriesSize;

private:
   REAL*          m_performances;
   REAL*          m_series;
#if USE_PPL
   volatile long* m_frequencies;
#else
   int* m_frequencies;
#endif // USE_PPL

#ifndef USE_CUDA
private:
	REAL* m_objects;
#endif // USE_CUDA

#if USE_OPENCL
   OCLKernel* m_oclKernel;
#endif // USE_OPENCL

#if USE_PPL
   PPLKernel*          m_pplKernel;
   PPLSeriesKernel*    m_pplSeriesKernel;
   PPLFrequencyKernel* m_pplFrequencyKernel;
#endif // USE_PPL

private:
   int positionInRange( REAL value, REAL range, int steps );
};

#endif // _MONTECARLO_H_