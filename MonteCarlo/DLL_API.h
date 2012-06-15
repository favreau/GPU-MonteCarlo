#ifndef _DLL_API_H_
#define _DLL_API_H_

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MonteCarlo_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MonteCarlo_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef MONTECARLO_EXPORTS
#define MONTECARLO_API __declspec(dllexport)
#else
#define MONTECARLO_API __declspec(dllimport)
#endif

// Float or double??
typedef float REAL;

#endif // _DLL_API_H_