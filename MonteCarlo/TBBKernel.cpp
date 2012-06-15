#include <tbb/spin_mutex.h>
#include "TBBKernel.h"
#include "Kernel.h"
#include "NormInv.h"

tbb::spin_mutex sm;

int positionInRange( REAL value, REAL range, int steps )
{
   REAL v = value-(-range/2.0f);
   int position = int(v/(range/steps));
   return position;
}

TBBKernel::TBBKernel( REAL* objects, REAL mu, REAL sigma )
 : m_objects( objects ),
   m_mu( mu ),
   m_sigma( sigma )
{
}

void TBBKernel::operator() ( const tbb::blocked_range<unsigned int>& r ) const 
{
	for( unsigned int i = r.begin(); i != r.end(); ++i ) {
      m_objects[i] = randn_notrig( m_mu, m_sigma );
	}
}

TBBSeriesKernel::TBBSeriesKernel( REAL* objects, int nbObjects, REAL* series, int nbSeries, REAL* performances )
 : m_objects( objects ),
   m_nbObjects( nbObjects ),
   m_series( series ),
   m_nbSeries( nbSeries ),
   m_performances( performances )
{
}

void TBBSeriesKernel::operator() ( const tbb::blocked_range<unsigned int>& r ) const 
{
	for( unsigned int index = r.begin(); index != r.end(); ++index ) {
      REAL currentPerformance = 1.0f;
      REAL previousPerformance = 0.0f;
      for( int i(0); i<m_nbObjects; ++i ) {
         previousPerformance = currentPerformance;
         currentPerformance = (1.0f + m_objects[index*m_nbObjects+i]) * previousPerformance;
         if( index == 0 ) m_performances[i] = currentPerformance;
      }
      // Store performance
      m_series[index] = currentPerformance - 1.0f;
   }
}

TBBFrequencyKernel::TBBFrequencyKernel( REAL* series, int* frequencies, int nbFrequencies, REAL range )
 : m_series( series ),
   m_frequencies( frequencies ),
   m_nbFrequencies( nbFrequencies ),
   m_range( range )
{
}

void TBBFrequencyKernel::operator() ( const tbb::blocked_range<unsigned int>& r ) const 
{
	for( unsigned int index = r.begin(); index != r.end(); ++index ) {
      int position = positionInRange(m_series[index], m_range, m_nbFrequencies );
      if( position>= 0 && position<m_nbFrequencies) {
         tbb::spin_mutex::scoped_lock lock(sm);
         m_frequencies[position] = m_frequencies[position] + 1;
      }
   }
}
