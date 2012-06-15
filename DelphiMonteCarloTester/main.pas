unit main;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, ExtCtrls, TeEngine, Series, TeeProcs, Chart, ComCtrls,
  TeeFunci;

{ -- DLL interface -- }

const StandardDLL = 'MonteCarlo.dll';
const TBBDLL      = 'MonteCarlo_TBB.dll';
const CUDADLL     = 'MonteCarlo_CUDA.dll';
const OpenCLDLL   = 'MonteCarlo_OpenCL.dll';

const range = 3;

type REAL = double;

type
  TMPFMTechnology  = (tmtStandard, tmtTBB, tmtCUDA, tmtOpenCL);

  TCreateContainer = function(
    var out_hContainer: THandle;
    in_seriesSize      : integer;
    in_containerSize   : integer;
    in_frequenciesSize : integer) : integer; cdecl;

  TDeleteContainer = function( in_hContainer: THandle) : integer; cdecl;

  TRunKernel       = function( in_hContainer: THandle; in_mu : REAL; in_sigma : REAL; in_range: REAL; in_nbThreads: integer) : integer; cdecl;
    
  TGetFrequency    = function( in_hHandle: THandle; in_index: integer; var out_value: integer ) : integer; cdecl;
  TGetPerformance  = function( in_hHandle: THandle; in_index: integer; var out_value: REAL ) : integer; cdecl;
  TGetSerie        = function( in_hHandle: THandle; in_index: integer; var out_value: REAL ) : integer; cdecl;
  TGetObject       = function( in_hHandle: THandle; in_index: integer; var out_value: REAL ) : integer; cdecl;

  type
  TForm1 = class(TForm)
    Panel1: TPanel;
    Label1: TLabel;
    Label2: TLabel;
    btnRunCpp: TButton;
    Panel2: TPanel;
    ChartPerformance: TChart;
    tbPerformanceAnnuelle: TTrackBar;
    tbVolatilite: TTrackBar;
    tbIterations: TTrackBar;
    Label3: TLabel;
    lblIterations: TLabel;
    lblMu: TLabel;
    lblSigma: TLabel;
    lblExecutionTime: TLabel;
    tbFrequences: TTrackBar;
    Chart: TChart;
    Series1: TLineSeries;
    btnRunDelphi: TButton;
    Series2: TLineSeries;
    lblFrequencies: TLabel;
    Label5: TLabel;
    Label4: TLabel;
    tbSeries: TTrackBar;
    Label6: TLabel;
    Label7: TLabel;
    lblSeries: TLabel;
    tbNbThreads: TTrackBar;
    lblThreads: TLabel;
    Label9: TLabel;
    cbTechno: TComboBox;
    lblDataSize: TLabel;
    LineSeries1: TBarSeries;
    Series3: TBarSeries;
    procedure FormCreate(Sender: TObject);
    procedure btnRunCppClick(Sender: TObject);
    procedure tbPerformanceAnnuelleChange(Sender: TObject);
    procedure btnRunDelphiClick(Sender: TObject);
    procedure tbVolatiliteChange(Sender: TObject);
    procedure tbIterationsChange(Sender: TObject);
    procedure tbFrequencesChange(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure cbTechnoChange(Sender: TObject);
    procedure tbSeriesChange(Sender: TObject);
    procedure tbNbThreadsChange(Sender: TObject);
  private
    { Private declarations }
    procedure UpdateInformation;
  public
    { Public declarations }
    mx, my : Integer;
  end;

var
  dllHandle        : THandle;
  hCreateContainer : TCreateContainer;
  hDeleteContainer : TDeleteContainer;
  hRunKernel       : TRunKernel;
  hGetFrequency    : TGetFrequency;
  hGetPerformance  : TGetPerformance;
  hGetSerie        : TGetSerie;
  hGetObject       : TGetObject;
  Form1            : TForm1;

implementation

uses
  NormInv,
  Math, StrUtils;

{$R *.dfm}

procedure SafeDllCall( hHandle: THandle );
var
  status: integer;
begin
  status := hHandle;
  if( status <> 0 ) then
  begin
    raise Exception.Create('SafeCall Exception occured');
  end;
end;

{ -------------------------------------------------------------------------------- }
function MPFMLoadDll(aTechnology : TMPFMTechnology) : boolean;
begin
  Result := false;
  if( dllHandle <>  0 ) then
    FreeLibrary( dllHandle );
    
  try
    case( aTechnology ) of
      tmtStandard: dllHandle := LoadLibrary( StandardDLL );
      tmtTBB     : dllHandle := LoadLibrary( TbbDLL );
      tmtCUDA    : dllHandle := LoadLibrary( CudaDLL );
      tmtOpenCL  : dllHandle := LoadLibrary( OpenCLDLL );
    end;
  except
    On E : Exception do
    begin
      Raise Exception.Create('DLL Library not found');
    end;
  end;

  if( dllHandle <>  0 ) then
  begin
    hCreateContainer := GetProcAddress( dllHandle, 'createContainer');
    hDeleteContainer := GetProcAddress( dllHandle, 'deleteContainer');
    hRunKernel       := GetProcAddress( dllHandle, 'runKernel');
    hGetFrequency    := GetProcAddress( dllHandle, 'getFrequency');
    hGetPerformance  := GetProcAddress( dllHandle, 'getPerformance');
    hGetSerie        := GetProcAddress( dllHandle, 'getSerie');
    hGetObject       := GetProcAddress( dllHandle, 'getObject');
    Result := true;
  end;
end;

procedure TForm1.FormCreate(Sender: TObject);
begin
  dllHandle := 0;
end;

procedure TForm1.btnRunCppClick(Sender: TObject);
var
  hContainer: THandle;
  i: integer;
  value: integer;
  performance: REAL;
  t: cardinal;
  start, step: REAL;
  mu, sigma: REAL;
begin
  Screen.Cursor := crHourGlass;
  if( dllHandle = 0 ) then
    MPFMLoadDll( TMPFMTechnology(cbTechno.ItemIndex) );

  mu := Power( 1.0 + tbPerformanceAnnuelle.Position/100.0, 1/tbIterations.Position ) - 1.0;
  sigma := (tbVolatilite.position/sqrt(tbIterations.Position))/100.0;

  step := range/tbFrequences.Position;
  start := -range/2;

  SafeDllCall( hCreateContainer( hContainer, tbSeries.Position*1000, tbIterations.Position, tbFrequences.Position ));
  t := GetTickCount;
  SafeDllCall(hRunKernel(hContainer,mu,sigma, range, tbNbThreads.Position));
  lblExecutionTime.Caption := IntToStr( GetTickCount - t ) + ' ms';
  
  // Series Graph
  Chart.SeriesList[0].Clear;
  for i := 0 to tbFrequences.Position-1 do
  begin
    SafeDllCall( hGetFrequency(hContainer, i, value ));
    Chart.SeriesList[0].AddXY( start+i*step, value );
  end;
  Chart.SeriesList[0].AddXY( start+tbFrequences.Position*step, 0 );

  // Performance Graph
  ChartPerformance.SeriesList[0].Clear;
  for i := 0 to tbIterations.Position-1 do
  begin
    SafeDllCall( hGetPerformance(hContainer, i, performance ));
    ChartPerformance.SeriesList[0].AddXY( i, performance );
  end;

  SafeDllCall(hDeleteContainer(hContainer));

  chart.Refresh;
  ChartPerformance.Refresh;

  Screen.Cursor := crDefault;
end;

procedure TForm1.tbPerformanceAnnuelleChange(Sender: TObject);
begin
  UpdateInformation;
end;

procedure TForm1.btnRunDelphiClick(Sender: TObject);

  function positionInRange( value: double; range: double; steps: integer ) : integer;
  var
    v: double;
  begin
     v := value-(-range/2);
     result := trunc(v/(range/steps));
  end;

var
  i,s           : integer;
  mu, sigma     : double;
  series        : array of double;
  objects       : array of double;
  performances  : array of double;
  frequencies   : array of integer;
  currentPerformance  : extended;
  previousPerformance : extended;
  step : double;
  start: double;
  position: integer;
  t: ULong;
begin
  Screen.Cursor := crHourGlass;
  mu    := Power( 1.0 + tbPerformanceAnnuelle.Position/100, 1.0/tbIterations.Position ) - 1.0;
  sigma := (tbVolatilite.position/sqrt( tbIterations.Position ))/100;

  start := -range/2;

  t := GetTickCount;
  SetLength( series, tbSeries.Position*1000 );
  SetLength( objects, tbIterations.Position*tbSeries.Position*1000 );
  SetLength( performances, tbIterations.Position );

  Randomize;

  for s:= 0 to tbSeries.Position*1000-1 do
  begin
    // Compute performance
    currentPerformance := 1.0;
    for i := 0 to tbIterations.Position-1 do
    begin
      previousPerformance := currentPerformance;
      currentPerformance  := (1.0 + randg(mu, sigma)) * previousPerformance;
      if( s = 0 ) then
        performances[i] := currentPerformance;
    end;

    // Store performance
    series[s] := currentPerformance - 1.0;
  end;

  // Initialize Series
  SetLength( frequencies, tbFrequences.Position );
  for i := 0 to tbFrequences.Position-1 do
    frequencies[i] := 0;

  // Compute frequencies
  step := (range/tbFrequences.Position );
	for s:=0 to tbSeries.position*1000-1 do
  begin
    position := positionInRange(series[s], range, tbFrequences.Position );
    if( position >= 0 ) and (position < tbFrequences.Position) then
      frequencies[position] := frequencies[position] + 1;
  end;
  lblExecutionTime.Caption := IntToStr( GetTickCount - t ) + ' ms';

  // Performance Graph
  ChartPerformance.SeriesList[1].Clear;
  for i := 0 to tbIterations.Position-1 do
    ChartPerformance.SeriesList[1].AddXY( i, performances[i] );

  // Series Graph
  Chart.SeriesList[1].Clear;
  for i := 0 to tbFrequences.Position-1 do
    Chart.SeriesList[1].AddXY( start+i*step, frequencies[i] );
  Chart.SeriesList[1].AddXY( start+tbFrequences.Position*step, 0 );
  Screen.Cursor := crDefault;

  chart.Refresh;
  ChartPerformance.Refresh;

  Screen.Cursor := crDefault;
end;

procedure TForm1.tbVolatiliteChange(Sender: TObject);
begin
  UpdateInformation;
end;

procedure TForm1.tbIterationsChange(Sender: TObject);
begin
  UpdateInformation;
end;

procedure TForm1.tbFrequencesChange(Sender: TObject);
begin
  UpdateInformation;
end;

procedure TForm1.FormShow(Sender: TObject);
begin
  UpdateInformation;
  //btnRunCppClick( Sender );
end;

procedure TForm1.cbTechnoChange(Sender: TObject);
begin
  MPFMLoadDll( TMPFMTechnology(cbTechno.ItemIndex) );
  UpdateInformation;
end;

procedure TForm1.tbSeriesChange(Sender: TObject);
begin
  UpdateInformation;
end;

procedure TForm1.tbNbThreadsChange(Sender: TObject);
begin
  UpdateInformation;
end;

procedure TForm1.UpdateInformation;
var
  DataSize: integer;
begin
  lblSeries.Caption := IntToStr( tbSeries.Position*1000 ) + ' séries';
  lblThreads.Caption := IntToStr( tbNbThreads.Position ) + ' threads';
  lblFrequencies.Caption := IntToStr( tbFrequences.Position ) + ' segments';
  lblSigma.Caption := FloatToStr( tbVolatilite.Position/100 );
  lblIterations.Caption := IntToStr( tbIterations.Position ) + ' jours';
  lblMu.Caption := FloatToStr( tbPerformanceAnnuelle.Position/100 );

  DataSize := tbSeries.Position*1000*tbIterations.Position*sizeof(double) div 1024 div 1024;
  lblDataSize.Caption := IntToStr( dataSize ) + ' MBytes';
  if( dataSize >= 640 ) then
  begin
    if( TMPFMTechnology(cbTechno.ItemIndex) = tmtCuda) then
      btnRunCpp.Enabled := false; 
    lblDataSize.Color := clRed;
  end
  else
  begin
    lblDataSize.Color := clGreen;
    btnRunCpp.Enabled := true; 
  end;

  if ( TMPFMTechnology(cbTechno.ItemIndex) = tmtCuda ) or ( TMPFMTechnology(cbTechno.ItemIndex) = tmtOpenCL ) then
    btnRunCpp.Click;
end;


end.



