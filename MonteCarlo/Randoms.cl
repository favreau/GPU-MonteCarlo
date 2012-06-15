
#define RAND_MAX 32767

__kernel void randn_notrig(__global float* objects, float mu, float sigma) 
{
   int index=get_global_id(0);

   float storedDeviate;			//	deviate from previous calculation
   float polar;
   float rsquared;
   float var1;
   float var2;

   //	If no deviate has been stored, the polar Box-Muller transformation is 
   //	performed, producing two independent normally-distributed random
   //	deviates.  One is stored for the next round, and one is returned.
   //	choose pairs of uniformly distributed deviates, discarding those 
   //	that don't fall within the unit circle
   do {
      var1=2.0*( rand()/RAND_MAX ) - 1.0;
      var2=2.0*( rand()/RAND_MAX ) - 1.0;
      rsquared=var1*var1+var2*var2;
   } while ( rsquared>=1.0 || rsquared == 0.0);

   //	calculate polar tranformation for each deviate
   polar=sqrt(-2.0*log(rsquared)/rsquared);

   //	store first deviate and set flag
   storedDeviate=var1*polar;

   //	return second deviate
   objects[index] = var2*polar*sigma + mu;
}
