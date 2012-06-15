// MonteCarloCLI.h

#pragma once

#include "../MonteCarlo/MonteCarlo.h"

using namespace System;

namespace MonteCarloCLI {

   public ref class MonteCarloWrapper
   {
   public:
      MonteCarloWrapper() : m_monteCarlo(0) 
      {
      };

      ~MonteCarloWrapper()
      {
         if( m_monteCarlo) delete m_monteCarlo;
      }
   
   public:
      void CreateContainer(
         const int seriesSize,
         const int containerSize, 
         const int frequenciesSize)
      {
         if( m_monteCarlo ) delete m_monteCarlo;
         m_monteCarlo = new CMonteCarlo();
         m_monteCarlo->createContainer( seriesSize, containerSize, frequenciesSize );
      }

      void destroyContainer()
      {
#ifndef USE_PPL
         if( m_monteCarlo != 0 ) delete m_monteCarlo;
#endif // USE_PPL
         m_monteCarlo = 0;
      }

   public:
      void runKernel(REAL mu, REAL sigma, REAL range, int nbThreads) 
      {
         m_monteCarlo->runKernel( mu, sigma, range, nbThreads );  
      }

   public:
   	int getFrequency( long index )
      {
         return m_monteCarlo->getFrequency( index );
      }

   	REAL getPerformance( long index )
      {
         return m_monteCarlo->getPerformance( index );
      }

   private:
      CMonteCarlo* m_monteCarlo;
   };
};
