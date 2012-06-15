//#include <cutil_inline.h>
#include <curand.h>
#include <cuda.h>
#include <math.h>
#include <windows.h>

cudaDeviceProp _dDeviceProp; 

// Device buffers
float* _dObjects;
float* _dSeries;
float* _dPerformances;
int*   _dFrequencies;

// Variables
int _dNbObjects;
int _dNbFrequencies;
int _dNbSeries;

/**
* @brief Initializes the testcase by 
* - Allocating the GPU memory
* - Copying the buffer from RAM to GPU memory.
* This function is executed on the Host
* @ param hBuffer Pointer to array of floats in host memory
* @ param N Number of objects in the array (an Objects consists of 6 floats)
*/
extern "C" 
void initialize_device( int NbSeries, int NbObjects, int NbFrequencies )
{
#if 0
   // Get Device properties
   int deviceCount; 
   cudaGetDeviceCount(&deviceCount);

   int device(0); 
   cudaGetDeviceProperties(&_dDeviceProp, device); 
#endif // 0

   // Testcase initialization
   _dNbObjects     = NbObjects;
   _dNbFrequencies = NbFrequencies;
   _dNbSeries      = NbSeries;

   // Device allocation
   cudaMalloc( (void**)&_dObjects, NbSeries*NbObjects*sizeof(float) );
   cudaMalloc( (void**)&_dSeries, NbSeries*sizeof(float) );
   cudaMalloc( (void**)&_dFrequencies, NbFrequencies*sizeof(int) );
   cudaMalloc( (void**)&_dPerformances, NbObjects*sizeof(float) );
}

/**
* @brief 
*/
__global__ void performanceStorageKernel( float* series, float* objects, int nbObjects, float* performances )
{
   // Compute the index
   unsigned int x     = blockIdx.x*blockDim.x+threadIdx.x;
   unsigned int y     = blockIdx.y*blockDim.y+threadIdx.y;
   unsigned int index = (y*blockDim.x) + x;

   int objectsIndex = index*nbObjects;

   // Compute performance
   __shared__ float localPerformance[2];
   localPerformance[0] = 1.0; // current performance
   localPerformance[1] = 0.0; // previous performance
   for( int i(0); i<nbObjects; ++i ) {
      localPerformance[1] = localPerformance[0];
      localPerformance[0] = (1.0+objects[objectsIndex+i])*localPerformance[1];

      if( index == 0 ) performances[i] = localPerformance[0];
   }
   // Store performance
   series[index] = localPerformance[0] - 1.0;
}

/**
* @brief Kernel function to be executed on the GPU
* @param ptr Pointer to an array of floats stored in GPU memory
*/
__global__ void frequenciesKernel( float* series, float range, int* frequencies, int nbFrequencies  )
{
   // Compute the index
   unsigned int x     = blockIdx.x*blockDim.x+threadIdx.x;
   unsigned int y     = blockIdx.y*blockDim.y+threadIdx.y;
   unsigned int index = (y*blockDim.x) + x;

   float v = series[index]-(-range/2);
   int position = (v/(range/nbFrequencies));
   atomicAdd(&frequencies[position],1);
}

/**
* @brief Run the kernel on the GPU. 
* This function is executed on the Host.
*/
extern "C" 
void run_kernel( float range, float mu, float sigma, int random, int nbThreadsPerBlock )
{
#if 0
   // Get Device properties
   int deviceCount; 
   cudaGetDeviceCount(&deviceCount);
   int device(0); 
   cudaGetDeviceProperties(&_dDeviceProp, device); 
#endif // 0
   long t = 0;
   t = GetTickCount();

   // Create pseudo-random number generator
   curandGenerator_t gen;
   curandCreateGenerator(&gen, CURAND_RNG_PSEUDO_DEFAULT);
   curandSetPseudoRandomGeneratorSeed(gen, random);
   curandGenerateNormal(gen, _dObjects, _dNbSeries*_dNbObjects, mu, sigma );
   curandDestroyGenerator(gen);

   dim3 block(8, 8, 1);
   int gridDim = (int)sqrt(float(_dNbSeries));
   dim3 gridPerformances( gridDim/block.x, gridDim/block.y, 1);
   performanceStorageKernel<<<gridPerformances,block>>>(_dSeries, _dObjects, _dNbObjects, _dPerformances );

   // Reset memory
   cudaMemset( _dFrequencies, 0, _dNbFrequencies*sizeof(int) );

   // compute Frequencies 
   gridDim = (int)sqrt(float(_dNbSeries));
   dim3 gridFrequencies( gridDim/block.x, gridDim/block.y, 1);
   frequenciesKernel<<<gridFrequencies,block>>>( _dSeries, range, _dFrequencies, _dNbFrequencies );

   t = GetTickCount() - t;
   t = 1;
}

/*
* This function is executed on the Host.
* @brief Copy the data back to the Host and releases the buffer on
* GPU device.
*/ 
extern "C" 
void host_to_device( float* hOutObjects )
{
}

/*
* This function is executed on the Host.
* @brief Copy the data back to the Host and releases the buffer on
* GPU device.
* This function is executed on the Host
* @ param hBuffer Pointer to array of floats in host memory
* @ param N Number of objects in the array (an Objects consists of 6 floats)
*/
extern "C" 
void device_to_host( int* hOutFrequencies, float* hOutPerformances, float* hOutSeries )
{
   cudaMemcpy( hOutFrequencies,  _dFrequencies,  _dNbFrequencies*sizeof(int), cudaMemcpyDeviceToHost);
   cudaMemcpy( hOutPerformances, _dPerformances, _dNbObjects*sizeof(float),   cudaMemcpyDeviceToHost);
}

/*
* This function is executed on the Host.
* @brief Releases the buffer on GPU device.
* This function is executed on the Host
*/
extern "C" 
void destroy_device()
{
   cudaFree( _dObjects );
   cudaFree( _dFrequencies );
   cudaFree( _dSeries );
   cudaFree( _dPerformances );
}
