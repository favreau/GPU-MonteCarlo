// MonteCarlo.cpp : Defines the exported functions for the DLL application.
//

#include <limits>
#include <map>
#include <fstream>
#include <time.h>
#include "MonteCarlo.h"
#include "NormInv.h"

#if USE_OPENCL
#  include "OCLKernel.h"
#endif // USE_OPENCL

#if USE_TBB
#  include "TBBKernel.h"
#endif // USE_TBB

#if USE_PPL
#  include "PPLKernel.h"
#endif // USE_PPL

#if USE_CUDA
#  include "CudaSpecificHeaders.h"
#endif // USE_CUDA

#include "Kernel.h"

using namespace std;

// --------------------------------------------------------------------------------
CMonteCarlo::CMonteCarlo()
 : m_frequencies(0),
#ifndef USE_CUDA
   m_objects(0),
#endif // !USE_CUDA
#if USE_PPL
   m_pplKernel(0),
   m_pplSeriesKernel(0),
   m_pplFrequencyKernel(0),
#endif // USE_PPL
   m_series(0),
   m_performances(0),
   m_objectsSize(0),
   m_frequenciesSize(0)
{
}

// --------------------------------------------------------------------------------
CMonteCarlo::~CMonteCarlo(void)
{
#if USE_CUDA
   destroy_device();
#endif // USE_CUDA

#if USE_OPENCL
   delete m_oclKernel;
#endif // USE_OPENCL

#if USE_PPL
   if( m_pplKernel != 0) delete m_pplKernel;
   if( m_pplSeriesKernel != 0) delete m_pplSeriesKernel;
   if( m_pplFrequencyKernel != 0) delete m_pplFrequencyKernel;
#endif // USE_OPENCL

#ifndef USE_CUDA
   if( m_objects != 0 ) delete m_objects;
#endif // !USE_CUDA

   if( m_frequencies != 0 ) delete m_frequencies;
   if( m_series != 0) delete m_series;
   if( m_performances != 0 ) delete m_performances;
}

// --------------------------------------------------------------------------------
void CMonteCarlo::createContainer( 
   const int seriesSize,
   const int objectsSize, 
   const int frequenciesSize)
{
   m_seriesSize = seriesSize;
   m_objectsSize = objectsSize;
   m_frequenciesSize = frequenciesSize;

   if( m_performances != 0 ) delete m_performances;
   if( m_frequencies != 0 ) delete m_frequencies;
   if( m_series != 0 ) delete m_series;

   m_series = new REAL[seriesSize];
   memset( m_series, 0, m_seriesSize*sizeof(REAL));

   m_performances = new REAL[m_objectsSize];
   memset( m_performances, 0, m_objectsSize*sizeof(REAL));

#if USE_PPL
   m_frequencies = new volatile long[frequenciesSize];
   memset( (void*)m_frequencies, 0, m_frequenciesSize*sizeof(volatile long));
#else
   m_frequencies = new int[frequenciesSize];
   memset( m_frequencies, 0, m_frequenciesSize*sizeof(int));
#endif // USE_PPL

#ifndef USE_CUDA
   if( m_objects != 0 ) delete m_objects;
   m_objects = new REAL[m_objectsSize*m_seriesSize];
#endif // USE_CUDA

#if USE_CUDA
   initializeDevice();
#endif // USE_CUDA

#if USE_OPENCL
   m_oclKernel = new OCLKernel(
      m_series,
      m_seriesSize, 
      m_objects,
      m_objectsSize, 
      m_performances,
      m_objectsSize,
      m_frequencies,
      m_frequenciesSize
      );
   m_oclKernel->initializeDevices();
   m_oclKernel->compileKernels();
#endif // USE_OPENCL

#if USE_PPL
   m_pplKernel          = new PPLKernel( m_objects, m_objectsSize*m_seriesSize );
   m_pplSeriesKernel    = new PPLSeriesKernel( m_objects, m_objectsSize, m_series, m_seriesSize, m_performances );
   m_pplFrequencyKernel = new PPLFrequencyKernel( m_series, m_seriesSize, m_frequencies, m_frequenciesSize );
#endif // USE_PPL
}

int CMonteCarlo::positionInRange( REAL value, REAL range, int steps )
{
   REAL v = value-(-range/2.0f);
   int position = int(v/(range/steps));
   return position;
}

// --------------------------------------------------------------------------------
void CMonteCarlo::runKernel( REAL mu, REAL sigma, REAL range, int NbThreads )
{
   //srand ( static_cast<unsigned int>(time(NULL)) );
   srand ( GetTickCount() );
   
#if USE_TBB
   // Générer <m_frequenciesSize>*<m_objectsSize> variables aléatoires suivant une loi normale avec la moyenne 
   // Mu et l'écart-type sigma spécifié. Soit p(j) chaque variable générée. 
   // Cela correspond à la macro excel NORMINV(). La fonction prend les paramètres 
   // suivants:
   // - La probabilité: il faut générer à chaque appel, une variable aléatoire uniforme entre 0 et 1. 
   // - La moyenne (mu)
   // - L'écart-type (sigma). 
   // Generated normal numbers
   tbb::parallel_for( tbb::blocked_range<unsigned int>(0, m_seriesSize*m_objectsSize, NbThreads),
      TBBKernel( m_objects, mu, sigma ) );

   for( int s(0); s<m_seriesSize; ++s ) {
      // Compute performance
      REAL currentPerformance = 1.0f;
      REAL previousPerformance = 0.0f;
      for( int i(0); i<m_objectsSize; ++i ) {
         previousPerformance = currentPerformance;
         currentPerformance = (1.0f + m_objects[s*m_objectsSize+i]) * previousPerformance;
         if( s == 0 ) m_performances[i] = currentPerformance;
      }
      // Store performance
      m_series[s] = currentPerformance - 1.0f;
   }

   // Compute frequence for each series
   tbb::parallel_for( tbb::blocked_range<unsigned int>(0, m_seriesSize, NbThreads), 
      TBBFrequencyKernel( m_series, m_frequencies, m_frequenciesSize, range ) );
#else
#if USE_CUDA
   run_kernel( range, mu, sigma, rand(), NbThreads );
   deviceToHost();
#else
#if USE_OPENCL
   int i(0);
#pragma omp parallel for num_threads( NbThreads )
   for( i = 0; i<m_seriesSize*m_objectsSize; ++i ) {
      m_objects[i] = randn_notrig( mu, sigma );
   }
   m_oclKernel->runKernel( mu, sigma, range );
#else
#if USE_PPL
   m_pplKernel->run( mu, sigma );
   m_pplSeriesKernel->run();
   m_pplFrequencyKernel->run( range );
#else
   long t = GetTickCount();

   for( int i(0); i<m_seriesSize*m_objectsSize; ++i ) {
      m_objects[i] = randn_notrig( mu, sigma );
   }
   t = GetTickCount() - t;
   //std::cout << t << std::endl;

   for( int s(0); s<m_seriesSize; ++s ) {
      // Compute performance
      REAL currentPerformance = 1.0f;
      REAL previousPerformance = 0.0f;
      for( int i(0); i<m_objectsSize; ++i ) {
         previousPerformance = currentPerformance;
         currentPerformance = (1.0f + m_objects[s*m_objectsSize+i]) * previousPerformance;
         if( s == 0 ) m_performances[i] = currentPerformance;
      }
      // Store performance
      m_series[s] = currentPerformance - 1.0f;
   }
   
   for( int s(0); s<m_seriesSize; ++s ) {
      int position = positionInRange(m_series[s], range, m_frequenciesSize );
      if( position>= 0 && position<m_frequenciesSize) {
         m_frequencies[position] = m_frequencies[position] + 1;
      }
   }
#endif // USE_PPL
#endif // USE_OPENCL
#endif // USE_CUDA
#endif // USE_TBB
}

#if USE_CUDA
// --------------------------------------------------------------------------------
void CMonteCarlo::initializeDevice()
{
   initialize_device( m_seriesSize, m_objectsSize, m_frequenciesSize );
}

// --------------------------------------------------------------------------------
void CMonteCarlo::hostToDevice()
{
}

// --------------------------------------------------------------------------------
void CMonteCarlo::deviceToHost()
{
   device_to_host( m_frequencies, m_performances, m_series );
}
#else
// --------------------------------------------------------------------------------
REAL CMonteCarlo::getObject( long index )
{
   if( index < m_objectsSize ) {
      return m_objects[ index ];
   }
   else {
      return 0;
   }
}
#endif // USE_CUDA

// --------------------------------------------------------------------------------
REAL CMonteCarlo::getPerformance( long index )
{
   REAL value(0);
   if( index < m_objectsSize ) {
      value = m_performances[ index ];
   }
   return value;
}

// --------------------------------------------------------------------------------
int CMonteCarlo::getFrequency( long index )
{
   int value(0);
   if( index < m_frequenciesSize ) {
      value = m_frequencies[ index ];
   }
   return value;
}

// --------------------------------------------------------------------------------
REAL CMonteCarlo::getSerie( long index )
{
   if( index < m_seriesSize ) {
      return m_series[ index ];
   }
   else {
      return 0;
   }
}
