program DelphiMonteCarloTester;

uses
  Forms,
  main in 'main.pas' {Form1},
  NormInv in 'NormInv.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TForm1, Form1);
  Application.Run;
end.
