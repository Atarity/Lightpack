program LightPack_SDK_Demo;

uses
  Forms,
  Unit1 in 'Unit1.pas' {Form1},
  LightPack in 'LightPack.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TForm1, Form1);
  Application.Run;
end.
