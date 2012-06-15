#include <cmath>

double c0 = 2.515517f;
double c1 = 0.802853f;
double c2 = 0.010328f;
double d1 = 1.432788f;
double d2 = 0.189269f;
double d3 = 0.001308f;

long double NormInv(long double q, long double Mu, long double Sigma )
{
   try {

      double x;
      double p;
      double t;

      if(q == 0.5f) {
         return Mu;
      }
      else {
         q = 1.0f - q;
         p = (q > 0.0f && q < 0.5f) ? q : (1.0f - q); 
         double _log = log( 1.0f/pow( p,2.0) );
         t = sqrt( (_log<0.0)? 0.0 :_log );
         x = t-(c0 + c1 * t + c2 * pow(t,2.0))/(1.0f+d1*t+d2*pow(t,2.0) + d3*pow(t,3.0));

         if (q > 0.5f) {
            x *= -1.0f;
         }
      }
      return (x * Sigma) + Mu;
   }
   catch( ... ) {
      return 0.0;
   }
}
