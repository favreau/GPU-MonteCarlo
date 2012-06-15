#include "..\MonteCarlo\MonteCarlo.h"
#include "..\MonteCarlo\MonteCarlo_DelphiStub.h"
#include "..\MonteCarlo\NormInv.h"

#include <iostream>
#include <fstream>
#include <limits>
#undef max

const long nbObjects     = 500;   
const long nbFrequencies = 50;
const double range       = 3.0f;  
const long NbRuns        = 10;

const double annualPerf = 0.005f;
const double volatility = 0.01f; 

void runTestcase( int nbSeries, double mu, double sigma )
{
   HANDLE hContainer(0);
   HANDLE hObject(0);
   long status(0);
   long t1(0);
   char* in_buffer(0);

   std::cout << "Running kernel ";
   status = createContainer( hContainer, nbSeries, nbObjects, nbFrequencies );

   t1 = GetTickCount();
   status = runKernel( hContainer, mu, sigma, range, 64 );
   std::cout << GetTickCount() - t1 << "ms" << std::endl;

#if 0
   std::cout << "Objects: " << std::endl;
   for( long i(0); i<nbObjects; ++i ) {
      double value;
      status = getObject( hContainer, i, value );
      std::cout << value << std::endl;
   }
#endif // 0

#if 0
   double total(0);
   std::cout << "Frequencies: " << std::endl;
   for( long i(0); i<nbFrequencies; ++i ) {
      long value;
      status = getFrequency( hContainer, i, value );
      std::cout << value << ", ";
      total += value;
   }
   std::cout << "Total: " << total << std::endl;

   std::cout << "Performances: " << std::endl;
   for( unsigned long i(0); i<nbObjects; ++i ) {
      double value;
      status = getPerformance( hContainer, i, value );
      std::cout << value << ", ";
      total += value;
   }
   std::cout << std::endl;

   double total(0);
   std::cout << "Series: " << std::endl;
   for( unsigned long i(0); i<nbSeries; ++i ) {
      double value;
      status = getSerie( hContainer, i, value );
      std::cout << value << std::endl;
      total += value;
   }
   std::cout << "Total: " << total << std::endl;
#endif // 0

   status = deleteContainer( hContainer );

   std::cout << "Done ... " << std::endl;
}


int main( long argc, char** argv )
{
   if( argc<2 ) {
      std::cout << "Usage: MonteCarloTester <NbSeries>" << std::endl;
      return 0;
   }

   unsigned long nbSeries = atoi(argv[1]);

   double mu = powl( 1.0f + annualPerf, 1.0f/nbObjects ) - 1.0f;
   double sigma = volatility/sqrtl(nbObjects);

   std::cout << "----------------------------------------" << std::endl;

#if USE_TBB
   std::cout << "TBB Power!" << std::endl;
#else
#if USE_CUDA
   std::cout << "CUDA Power!" << std::endl;
#else
#if USE_OPENCL
   std::cout << "OpenCL Power!" << std::endl;
#endif // USE_OPENCL
#endif // USE_CUDA
#endif // USE_TBB

   std::cout << "Number of objects    : " << nbObjects << std::endl;
   std::cout << "Number of frequencies: " << nbFrequencies << std::endl;
   std::cout << "Mu                   : " << mu << std::endl;
   std::cout << "Sigma                : " << sigma << std::endl;
   std::cout << "Range                : " << -range/2 << " to " << range/2 << std::endl;
   std::cout << "Number of runs       : " << nbSeries << std::endl;
   std::cout << "----------------------------------------" << std::endl;

   for( int i(0); i<NbRuns; ++i ) {
      runTestcase( nbSeries, mu, sigma );
   }

   return 0;
}