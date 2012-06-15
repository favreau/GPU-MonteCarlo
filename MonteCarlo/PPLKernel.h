#ifndef _PPLKERNEL_H_
#define _PPLKERNEL_H_

#include "DLL_API.h"

/**
* Objects
*/
class PPLKernel 
{
public:
   PPLKernel( REAL* object, size_t NbObjects );
	void run(REAL mu, REAL sigma) const;

private:
	REAL*  m_objects;
   size_t m_nbObjects;
   REAL   m_mu;
   REAL   m_sigma;
};

/**
* Series
*/
class PPLSeriesKernel 
{
public:
   PPLSeriesKernel( REAL* objects, int nbObjects, REAL* series, int nbSeries, REAL* m_performances );
	void run() const;

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
class PPLFrequencyKernel 
{
public:
   PPLFrequencyKernel( REAL* series, int nbSeries, volatile long* frequencies, int nbFrequencies );
	void run( REAL range ) const;

private:
	REAL* m_series;
   volatile long*  m_frequencies;
   int   m_nbFrequencies;
   int   m_nbSeries;
};

#endif // _PPLKERNEL_H_
