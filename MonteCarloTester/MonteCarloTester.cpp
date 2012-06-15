#include "..\MonteCarlo\MonteCarlo.h"
#include <iostream>
#include <fstream>

const long nbObjects = 60000; // Nombre de jours
const long nbFrequencies = 61;    // Nombre de séries
const long double range = 6.0f;    // Nombre de séries

const long double mu = 10.0f;    
const long double sigma = 100.0f; 

void main() 
{
   CMonteCarlo monteCarlo( mu, sigma );
   monteCarlo.createContainer( nbObjects, nbFrequencies, range );

   unsigned long t = GetTickCount();
#if 0
   std::string testdata_filename("testdata.csv");
	std::ifstream fin_testdata(testdata_filename.c_str(), std::ios_base::in );
#endif 0
   BusinessObject* object(0);
   for( unsigned long i(0); i<nbObjects;++i ) {
      monteCarlo.createObject( object );
#if 0
      double value;
      fin_testdata >> value;
      object->randonValue = value;
#endif // 0
   }
#if 0
   fin_testdata.close();
#endif // 0
   std::cout << "Initialization done in " << GetTickCount() - t << "ms" << std::endl;

   t = GetTickCount();
   monteCarlo.runKernel();
   std::cout << "Kernel executed in " << GetTickCount() - t << "ms" << std::endl;

   std::string object_filename("objects.csv");
	std::ofstream fout_object(object_filename.c_str(), std::ios_base::out );
   for( unsigned long i(0); i<nbObjects; ++i ) {
      BusinessObject* object = monteCarlo.getObject(i);
      fout_object << object->randonValue << ";" << object->normInv << std::endl;
   }
	fout_object.close();

   std::string frequencies_filename("frequencies.csv");
	std::ofstream fout_frequencies(frequencies_filename.c_str(), std::ios_base::out );
   for( unsigned long i(0); i<nbFrequencies; ++i ) {
      BusinessFrequencies* frequencies = monteCarlo.getFrequencies(i);
      fout_frequencies << frequencies->bottom << ";" << frequencies->frequency << std::endl;
      //std::cout << i << ":" << series->performance << std::endl;
   }
	fout_frequencies.close();
}
