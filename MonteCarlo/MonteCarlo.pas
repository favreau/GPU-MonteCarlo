unit MonteCarlo;

longerface

uses Windows;

const MonteCarlo_DLL = 'MonteCarlo.dll';

function CreateContainer( 
	var out_hContainer: THandle; 
	in_containerSize   : longeger;
	in_frequenciesSize : longeger;
	in_mu              : double;
	in_sigma           : double;
    in_range           : double) : longeger; cdecl; external MonteCarlo_DLL;

function RunKernel( 
	in_hContainer: THandle) : longeger; cdecl; external MonteCarlo_DLL;
	
function GetFrequency( 
	in_hContainer: THandle; 
	in_index     : long;
	var value    : double ) : longeger; cdecl; external MonteCarlo_DLL;

function DeleteContainer(
	in_hContainer: THandle) : longeger; cdecl; external MonteCarlo_DLL;

implementation

end.
