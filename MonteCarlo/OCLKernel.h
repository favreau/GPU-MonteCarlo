#ifndef _TBBKERNEL_H_
#define _TBBKERNEL_H_

#include <CL/opencl.h>
#include "DLL_API.h"

class OCLKernel
{
public:
   OCLKernel(REAL*, int, REAL*, int, REAL*, int, int*, int);
   ~OCLKernel();

public:
   void initializeDevices();
   void compileKernels();
   void runKernel( REAL, REAL, REAL );
   
private:
   char* loadFile( const char*, size_t&);

private:
   // OpenCL Objects
   cl_device_id     m_hDevice;
   cl_context       m_hContext;
   cl_command_queue m_hQueue;
   cl_kernel        m_hKernelRandoms;
   cl_kernel        m_hKernelPerformances; 
   cl_kernel        m_hKernelFrequencies;

private:
   cl_mem m_hDeviceSeries;
   cl_mem m_hDeviceObjects;
   cl_mem m_hDevicePerformances;
   cl_mem m_hDeviceFrequencies;

private:
   // Business attributes
   REAL* m_series;
   int   m_seriesSize;
   REAL* m_objects;
   int   m_containerSize;
   REAL* m_performances;
   int*  m_frequencies;
   int   m_frequenciesSize;
};

#endif // _TBBKERNEL_H_
