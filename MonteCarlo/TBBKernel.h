#ifndef _TBBKERNEL_H_
#define _TBBKERNEL_H_

#include "MonteCarlo.h"

/**
* Objects
*/
class TBBKernel 
{
public:
   TBBKernel( REAL* object, REAL mu, REAL sigma );
	void operator() ( const tbb::blocked_range<unsigned int>& r ) const;

private:
	REAL* m_objects;
   REAL m_mu;
   REAL m_sigma;
};

/**
* Series
*/
class TBBSeriesKernel 
{
public:
   TBBSeriesKernel( REAL* objects, int nbObjects, REAL* series, int nbSeries, REAL* performances );
	void operator() ( const tbb::blocked_range<unsigned int>& r ) const;

private:
	REAL* m_objects;
   REAL* m_series;
   REAL* m_performances;
   int   m_nbObjects;
   int   m_nbSeries;
};

/**
* Frequencies
*/
class TBBFrequencyKernel 
{
public:
   TBBFrequencyKernel( REAL* series, int* frequencies, int nbFrequencies, REAL range );
	void operator() ( const tbb::blocked_range<unsigned int>& r ) const;

private:
	REAL* m_series;
   int* m_frequencies;
   int  m_nbFrequencies;
   REAL  m_range;
};


#endif // _TBBKERNEL_H_
