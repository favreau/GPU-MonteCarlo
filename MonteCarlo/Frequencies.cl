#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable

/**
* Position in range
*/
int positionInRange( float value, float range, int steps )
{
   float v = value-(-range/2);
   return (v/(range/steps));
}

/**
* Frequencies kernel
*/
__kernel void frequencies( 
	__global float* series, 
	__global int*   frequencies, 
	int             nbFrequencies,
	float           range )
{
   int index=get_global_id(0);
   int position = positionInRange( series[index], range, nbFrequencies );
   if( position>=0 && position<nbFrequencies) {
      atom_inc(&frequencies[position]);
   }
}
