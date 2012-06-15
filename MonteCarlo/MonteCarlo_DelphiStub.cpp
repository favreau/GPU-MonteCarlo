#include "MonteCarlo_DelphiStub.h"
#include "MonteCarlo.h"

// --------------------------------------------------------------------------------
extern "C" MONTECARLO_API 
long createContainer( 
   HANDLE& hContainer,
   long seriesSize,
   long containerSize, 
   long frequenciesSize )
{
   try {
      CMonteCarlo* MonteCarlo = new CMonteCarlo();
      MonteCarlo->createContainer( seriesSize, containerSize, frequenciesSize );
      hContainer = MonteCarlo;
      return 0;
   }
   catch( ... ) {
      return 1;
   }
}

// --------------------------------------------------------------------------------
extern "C" MONTECARLO_API 
long deleteContainer( const HANDLE hContainer )
{
   try {
      CMonteCarlo* MonteCarlo = (CMonteCarlo*)hContainer;
      delete MonteCarlo;
      return 0;
   }
   catch( ... ) {
      return 1;
   }
}

// --------------------------------------------------------------------------------
extern "C" MONTECARLO_API 
long runKernel( const HANDLE hContainer, double mu, double sigma, double range, int NbThreads )
{
   try {
      CMonteCarlo* MonteCarlo = (CMonteCarlo*)hContainer;
      MonteCarlo->runKernel( mu, sigma, range, NbThreads );
      return 0;
   }
   catch( ... ) {
      return 1;
   }
}

#if USE_CUDA
// --------------------------------------------------------------------------------
extern "C" MONTECARLO_API 
long deviceToHost( const HANDLE hContainer )
{
   try {
      CMonteCarlo* MonteCarlo = (CMonteCarlo*)hContainer;
      MonteCarlo->deviceToHost();
      return 0;
   }
   catch( ... ) {
      return 1;
   }
}
#else
// --------------------------------------------------------------------------------
extern "C" MONTECARLO_API 
long getObject( const HANDLE hContainer, long index, double& value )
{
   try {
      CMonteCarlo* MonteCarlo = (CMonteCarlo *)hContainer;
      value = MonteCarlo->getObject( index );
      return 0;
   }
   catch( ... ) {
      return 1;
   }
}
#endif // USE_CUDA

// --------------------------------------------------------------------------------
extern "C" MONTECARLO_API 
long getPerformance( const HANDLE hContainer, long index, double& value )
{
   try {
      CMonteCarlo* MonteCarlo = (CMonteCarlo *)hContainer;
      value = MonteCarlo->getPerformance( index );
      return 0;
   }
   catch( ... ) {
      return 1;
   }
}

// --------------------------------------------------------------------------------
extern "C" MONTECARLO_API 
long getFrequency( const HANDLE hContainer, long index, long& value )
{
   try {
      CMonteCarlo* MonteCarlo = (CMonteCarlo *)hContainer;
      value = MonteCarlo->getFrequency( index );
      return 0;
   }
   catch( ... ) {
      return 1;
   }
}

// --------------------------------------------------------------------------------
extern "C" MONTECARLO_API 
long getSerie( const HANDLE hContainer, long index, double& value )
{
   try {
      CMonteCarlo* MonteCarlo = (CMonteCarlo *)hContainer;
      value = MonteCarlo->getSerie( index );
      return 0;
   }
   catch( ... ) {
      return 1;
   }
}

