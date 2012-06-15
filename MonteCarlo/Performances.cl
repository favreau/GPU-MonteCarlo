/**
* Performance kernel
*/
__kernel void performances( 
	__global float* series, 
	__global float* objects, 
	int             nbObjects, 
	__global float* performances )
{
   int index=get_global_id(0)*nbObjects;
   float currentPerformance  = 1.0;
   float previousPerformance = 0.0;
   for( int i=0; i<nbObjects; ++i ) {
      previousPerformance = currentPerformance;
      currentPerformance = (1.0+objects[index+i])*previousPerformance;
      if( get_global_id(0) == 0 ) performances[i] = currentPerformance;
   }
   series[index] = currentPerformance - 1.0;
}
