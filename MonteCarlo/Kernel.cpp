#include "Kernel.h"
#include "NormsInv.h"
#include <stdlib.h>
#include <math.h>

// --------------------------------------------------------------------------------
double processObject( double mu, double sigma ) 
{
   double randonValue = (rand()%1000)/1000.0f + 0.00001f;
   return NormInv( randonValue, mu, sigma );
}
