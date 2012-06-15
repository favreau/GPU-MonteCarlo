#include <ppl.h>
#include <windows.h>

#include "PPLKernel.h"
#include "Kernel.h"
#include "NormInv.h"

int positionInRange( REAL value, REAL range, int steps )
{
   REAL v = value-(-range/2.0f);
   int position = int(v/(range/steps));
   return position;
}

PPLKernel::PPLKernel( REAL* objects, size_t nbObjects )
 : m_objects( objects ),
   m_nbObjects( nbObjects )
{
}

void PPLKernel::run( REAL mu, REAL sigma ) const 
{
   Concurrency::parallel_for(size_t(0), m_nbObjects, [&](size_t i)
   {
      m_objects[i] = randn_notrig( mu, sigma );
	});
}

PPLSeriesKernel::PPLSeriesKernel( REAL* objects, int nbObjects, REAL* series, int nbSeries, REAL* performances )
 : m_objects( objects ),
   m_nbObjects( nbObjects ),
   m_series( series ),
   m_nbSeries( nbSeries ),
   m_performances( performances )
{
}

void PPLSeriesKernel::run() const 
{
   Concurrency::parallel_for(0, m_nbSeries, [&](int index) {
      REAL currentPerformance = 1.0f;
      REAL previousPerformance = 0.0f;
      for( int i(0); i<m_nbObjects; ++i ) {
         previousPerformance = currentPerformance;
         currentPerformance = (1.0f + m_objects[index*m_nbObjects+i]) * previousPerformance;
         if( index == 0 ) m_performances[i] = currentPerformance;
      }
      // Store performance
      m_series[index] = currentPerformance - 1.0f;
   });
}

PPLFrequencyKernel::PPLFrequencyKernel( REAL* series, int nbSeries, volatile long* frequencies, int nbFrequencies )
 : m_series( series ),
   m_nbSeries( nbSeries ),
   m_frequencies( frequencies ),
   m_nbFrequencies( nbFrequencies )
{
}

void PPLFrequencyKernel::run( REAL range ) const 
{
   Concurrency::parallel_for(0, m_nbSeries, [&](int index) {
      int position = positionInRange(m_series[index], range, m_nbFrequencies);
      if( position>= 0 && position<m_nbFrequencies) {
         InterlockedIncrement( &m_frequencies[position] );
      }
   });
}
