#include <stdio.h>
#include <stdlib.h>
#include "OCLKernel.h"

#define MAX_SOURCE_SIZE (0x100000)

/*
 * getErrorDesc
 */
char* getErrorDesc(int err)
{
   switch (err)
   {
   case CL_SUCCESS                        : return "CL_SUCCESS";
   case CL_DEVICE_NOT_FOUND               : return "CL_DEVICE_NOT_FOUND";
   case CL_COMPILER_NOT_AVAILABLE         : return "CL_COMPILER_NOT_AVAILABLE";
   case CL_MEM_OBJECT_ALLOCATION_FAILURE  : return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
   case CL_OUT_OF_RESOURCES               : return "CL_OUT_OF_RESOURCES";
   case CL_OUT_OF_HOST_MEMORY             : return "CL_OUT_OF_HOST_MEMORY";
   case CL_PROFILING_INFO_NOT_AVAILABLE   : return "CL_PROFILING_INFO_NOT_AVAILABLE";
   case CL_MEM_COPY_OVERLAP               : return "CL_MEM_COPY_OVERLAP";
   case CL_IMAGE_FORMAT_MISMATCH          : return "CL_IMAGE_FORMAT_MISMATCH";
   case CL_IMAGE_FORMAT_NOT_SUPPORTED     : return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
   case CL_BUILD_PROGRAM_FAILURE          : return "CL_BUILD_PROGRAM_FAILURE";
   case CL_MAP_FAILURE                    : return "CL_MAP_FAILURE";

   case CL_INVALID_VALUE                  : return "CL_INVALID_VALUE";
   case CL_INVALID_DEVICE_TYPE            : return "CL_INVALID_DEVICE_TYPE";
   case CL_INVALID_PLATFORM               : return "CL_INVALID_PLATFORM";
   case CL_INVALID_DEVICE                 : return "CL_INVALID_DEVICE";
   case CL_INVALID_CONTEXT                : return "CL_INVALID_CONTEXT";
   case CL_INVALID_QUEUE_PROPERTIES       : return "CL_INVALID_QUEUE_PROPERTIES";
   case CL_INVALID_COMMAND_QUEUE          : return "CL_INVALID_COMMAND_QUEUE";
   case CL_INVALID_HOST_PTR               : return "CL_INVALID_HOST_PTR";
   case CL_INVALID_MEM_OBJECT             : return "CL_INVALID_MEM_OBJECT";
   case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
   case CL_INVALID_IMAGE_SIZE             : return "CL_INVALID_IMAGE_SIZE";
   case CL_INVALID_SAMPLER                : return "CL_INVALID_SAMPLER";
   case CL_INVALID_BINARY                 : return "CL_INVALID_BINARY";
   case CL_INVALID_BUILD_OPTIONS          : return "CL_INVALID_BUILD_OPTIONS";
   case CL_INVALID_PROGRAM                : return "CL_INVALID_PROGRAM";
   case CL_INVALID_PROGRAM_EXECUTABLE     : return "CL_INVALID_PROGRAM_EXECUTABLE";
   case CL_INVALID_KERNEL_NAME            : return "CL_INVALID_KERNEL_NAME";
   case CL_INVALID_KERNEL_DEFINITION      : return "CL_INVALID_KERNEL_DEFINITION";
   case CL_INVALID_KERNEL                 : return "CL_INVALID_KERNEL";
   case CL_INVALID_ARG_INDEX              : return "CL_INVALID_ARG_INDEX";
   case CL_INVALID_ARG_VALUE              : return "CL_INVALID_ARG_VALUE";
   case CL_INVALID_ARG_SIZE               : return "CL_INVALID_ARG_SIZE";
   case CL_INVALID_KERNEL_ARGS            : return "CL_INVALID_KERNEL_ARGS";
   case CL_INVALID_WORK_DIMENSION         : return "CL_INVALID_WORK_DIMENSION";
   case CL_INVALID_WORK_GROUP_SIZE        : return "CL_INVALID_WORK_GROUP_SIZE";
   case CL_INVALID_WORK_ITEM_SIZE         : return "CL_INVALID_WORK_ITEM_SIZE";
   case CL_INVALID_GLOBAL_OFFSET          : return "CL_INVALID_GLOBAL_OFFSET";
   case CL_INVALID_EVENT_WAIT_LIST        : return "CL_INVALID_EVENT_WAIT_LIST";
   case CL_INVALID_OPERATION              : return "CL_INVALID_OPERATION";
   case CL_INVALID_GL_OBJECT              : return "CL_INVALID_GL_OBJECT";
   case CL_INVALID_BUFFER_SIZE            : return "CL_INVALID_BUFFER_SIZE";
   case CL_INVALID_MIP_LEVEL              : return "CL_INVALID_MIP_LEVEL";
   default: return "unknown";
   }
}

/*
 * CheckStatus
 */
void CheckStatus( int status ) 
{
   if( status != CL_SUCCESS ) {
      printf("*** Something went wrong!!: %s ***\n", getErrorDesc(status) );
   }
}


/*
 * OCLKernel constructor
 */
OCLKernel::OCLKernel( 
   REAL* series,
   int   seriesSize, 
   REAL* objects,
   int   containerSize, 
   REAL* performances,
   int   performancesSize,
   int*  frequencies,
   int   frequenciesSize ) 
 : m_hDevice(0),
   m_hContext(0),
   m_hQueue(0),
   m_series(series),
   m_seriesSize(seriesSize),
   m_objects(objects),
   m_containerSize(containerSize), 
   m_performances(performances),
   m_frequencies(frequencies),
   m_frequenciesSize(frequenciesSize)
{
   cl_int  status(0);
   // Find out how many devices there are
   cl_platform_id platform_id = 0;
   cl_uint ret_num_devices;
   cl_uint ret_num_platforms;
   CheckStatus(clGetPlatformIDs(1, &platform_id, &ret_num_platforms));
   CheckStatus(clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &m_hDevice, &ret_num_devices));
   m_hContext = clCreateContext(0, 1, &m_hDevice, 0, 0, &status );
   CheckStatus(status);
   m_hQueue = clCreateCommandQueue(m_hContext, m_hDevice, 0, 0);
}

/*
 * compileKernels
 */
void OCLKernel::compileKernels()
{
   cl_int status(0);
   size_t source_size;
   char buffer[2048];
   size_t len;
   const char* source_str;

   // Build the randoms kernel
#if 0
   source_str = loadFile("e:\\svn\\artecpp\\etudes\\trunk\\montecarlo\\montecarlo\\Randoms.cl", source_size);
   cl_program hProgramRandoms = clCreateProgramWithSource( m_hContext, 1, (const char **)&source_str, (const size_t*)&source_size, &status );
   CheckStatus(status);

   CheckStatus(clBuildProgram(hProgramRandoms, 1, &m_hDevice, 0, 0, 0));
   m_hKernelPerformances = clCreateKernel( hProgramRandoms, "randn_notrig", &status );
   CheckStatus(status);

   CheckStatus(clGetProgramBuildInfo(hProgramRandoms, m_hDevice, CL_PROGRAM_BUILD_LOG, 2048*sizeof(char), &buffer, &len ));
   printf( "%s", buffer );
   delete [] source_str;
#endif

   // Build the performances kernel
   source_str = loadFile("e:\\svn\\artecpp\\etudes\\trunk\\montecarlo\\montecarlo\\Performances.cl", source_size);
   cl_program hProgramPerformances = clCreateProgramWithSource( m_hContext, 1, (const char **)&source_str, (const size_t*)&source_size, &status );
   CheckStatus(status);

   CheckStatus(clBuildProgram(hProgramPerformances, 1, &m_hDevice, 0, 0, 0));
   m_hKernelPerformances = clCreateKernel( hProgramPerformances, "performances", &status );
   CheckStatus(status);

   CheckStatus(clGetProgramBuildInfo(hProgramPerformances, m_hDevice, CL_PROGRAM_BUILD_LOG, 2048*sizeof(char), &buffer, &len ));
   printf( "%s", buffer );
   delete [] source_str;


   // Build the frequencies kernel
   source_str = loadFile("e:\\svn\\artecpp\\etudes\\trunk\\montecarlo\\montecarlo\\Frequencies.cl", source_size);

   cl_program hProgramFrequencies = clCreateProgramWithSource( m_hContext, 1, (const char **)&source_str, (const size_t*)&source_size, &status );
   CheckStatus(status);

   CheckStatus(clBuildProgram(hProgramFrequencies, 1, &m_hDevice, 0, 0, 0));
   m_hKernelFrequencies = clCreateKernel( hProgramFrequencies, "frequencies", &status );
   CheckStatus(status);

   CheckStatus(clGetProgramBuildInfo(hProgramFrequencies, m_hDevice, CL_PROGRAM_BUILD_LOG, 2048*sizeof(char), &buffer, &len ));
   printf( "%s", buffer );
   delete [] source_str;


   CheckStatus(clReleaseProgram(hProgramPerformances));
   CheckStatus(clReleaseProgram(hProgramFrequencies));
}

void OCLKernel::initializeDevices()
{
   // Setup device memory
   m_hDeviceSeries       = clCreateBuffer( m_hContext, CL_MEM_WRITE_ONLY, sizeof(REAL)*m_seriesSize,       0, NULL);
   m_hDeviceObjects      = clCreateBuffer( m_hContext, CL_MEM_READ_ONLY,  sizeof(REAL)*m_containerSize,    0, NULL);
   m_hDevicePerformances = clCreateBuffer( m_hContext, CL_MEM_WRITE_ONLY, sizeof(REAL)*m_containerSize,    0, NULL);
   m_hDeviceFrequencies  = clCreateBuffer( m_hContext, CL_MEM_WRITE_ONLY, sizeof(int) *m_frequenciesSize,  0, NULL);
}


/*
 * runKernel
 */
void OCLKernel::runKernel( REAL mu, REAL sigma, REAL range )
{
   cl_int status(0);

   size_t global_item_size(0);
   size_t local_item_size(1);
#if 0
   // Run the kernel!!
   CheckStatus(clSetKernelArg( m_hKernelPerformances, 0, sizeof(cl_mem), (void*)&m_hDeviceObjects ));
   CheckStatus(clSetKernelArg( m_hKernelPerformances, 0, sizeof(REAL), (void*)&mu));
   CheckStatus(clSetKernelArg( m_hKernelPerformances, 0, sizeof(REAL), (void*)&sigma ));
   global_item_size = m_containerSize;
   CheckStatus(clEnqueueNDRangeKernel(
      m_hQueue, m_hKernelRandoms, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL));
#endif // 0

   // Initialise Input arrays
   CheckStatus(clEnqueueWriteBuffer(
      m_hQueue, m_hDeviceObjects, CL_TRUE, 0, m_containerSize*sizeof(REAL), m_objects, 0, NULL, NULL));
   
   // Initialise Output arrays
   CheckStatus(clEnqueueWriteBuffer(
      m_hQueue, m_hDeviceFrequencies, CL_TRUE, 0, m_frequenciesSize*sizeof(int), m_frequencies, 0, NULL, NULL));

   CheckStatus(clSetKernelArg( m_hKernelPerformances, 0, sizeof(cl_mem), (void*)&m_hDeviceSeries ));
   CheckStatus(clSetKernelArg( m_hKernelPerformances, 1, sizeof(cl_mem), (void*)&m_hDeviceObjects ));
   CheckStatus(clSetKernelArg( m_hKernelPerformances, 2, sizeof(int),    (void*)&m_containerSize ));
   CheckStatus(clSetKernelArg( m_hKernelPerformances, 3, sizeof(cl_mem), (void*)&m_hDevicePerformances ));

   // Run the kernel!!
   global_item_size = m_containerSize;
   CheckStatus(clEnqueueNDRangeKernel(
      m_hQueue, m_hKernelPerformances, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL));

   // Performances
   CheckStatus(clEnqueueReadBuffer(m_hQueue, m_hDevicePerformances, CL_TRUE, 0, sizeof(int)*m_containerSize, m_performances, 0, NULL, NULL));

   // --------------------------------------------------------------------------------
   // Build the frequencies kernel
   // --------------------------------------------------------------------------------

   CheckStatus(clSetKernelArg( m_hKernelFrequencies, 0, sizeof(cl_mem), (void*)&m_hDeviceSeries ));
   CheckStatus(clSetKernelArg( m_hKernelFrequencies, 1, sizeof(cl_mem), (void*)&m_hDeviceFrequencies ));
   CheckStatus(clSetKernelArg( m_hKernelFrequencies, 2, sizeof(int),    (void*)&m_frequenciesSize ));
   CheckStatus(clSetKernelArg( m_hKernelFrequencies, 3, sizeof(REAL),   (void*)&range ));
   
   // Run the kernel!!
   global_item_size = m_seriesSize;
   CheckStatus(clEnqueueNDRangeKernel(m_hQueue, m_hKernelFrequencies, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL));

   // ------------------------------------------------------------
   // Read back the results
   // ------------------------------------------------------------

   // Frequencies
   CheckStatus(clEnqueueReadBuffer(
      m_hQueue, m_hDeviceFrequencies, CL_TRUE, 0, sizeof(int)*m_frequenciesSize, m_frequencies, 0, NULL, NULL));

   // Frequencies
   CheckStatus(clEnqueueReadBuffer(
      m_hQueue, m_hDevicePerformances, CL_TRUE, 0, sizeof(REAL)*m_containerSize, m_performances, 0, NULL, NULL));

   CheckStatus(clFlush(m_hQueue));
   CheckStatus(clFinish(m_hQueue));
}

/*
 *
 */
OCLKernel::~OCLKernel()
{
   // Clean up
   CheckStatus(clReleaseMemObject(m_hDeviceSeries));
   CheckStatus(clReleaseMemObject(m_hDeviceObjects));
   CheckStatus(clReleaseMemObject(m_hDevicePerformances));
   CheckStatus(clReleaseMemObject(m_hDeviceFrequencies));

   CheckStatus(clReleaseKernel(m_hKernelPerformances));
   CheckStatus(clReleaseKernel(m_hKernelFrequencies));

   CheckStatus(clReleaseCommandQueue(m_hQueue));
   CheckStatus(clReleaseContext(m_hContext));
}

/*
 *
 */
char* OCLKernel::loadFile( const char* filename, size_t& length )
{
   // Load the kernel source code into the array source_str
   FILE *fp;
   char *source_str;

   fp = fopen(filename, "r");
   if (!fp) {
      printf("Failed to load kernel.\n");
   }
   source_str = (char*)malloc(MAX_SOURCE_SIZE);
   length = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
   fclose( fp );
   return source_str;
}


