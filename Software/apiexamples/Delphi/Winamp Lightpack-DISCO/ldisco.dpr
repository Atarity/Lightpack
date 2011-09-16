library ldisco;
//Для компиляции плагина необходима библиотека GLScene 1.0.0.0714, которую можно скачать с сайта http://glscene.ru
uses
  Windows,
  Graphics,
  IniFiles,
  SysUtils,
  vis in 'vis.pas' {Form1},
  Settings in 'Settings.pas' {Form2};

const
  VIS_HDRVER = $002;

type
  TRGBColor = record
    r,g,b: byte;
  end;

  TColorsMap = record
    min,max: integer;
    Color: TRGBColor;
  end;

  //указатель на структуру плагина
  PwinampVisModule = ^winampVisModule;

  //определяем типы для процедур и функций структуры
  waFunc = function( thismod: PwinampVisModule ): integer;cdecl;
  waProc = procedure( thismod: PwinampVisModule );cdecl;

  //структура плагина
  winampVisModule = record
    description: ansistring;
    hwndParent: HWND;
    hDllInstance: HWND;
    sRate: integer;
    nCh: integer;
    latencyMs: integer;
    delayMs: integer;
    spectrumNch: integer;
    waveformNch: integer;
    spectrumData: array[0..1, 0..575] of byte;
    waveformData: array[0..1, 0..575] of byte;
    Config: waProc;
    Init: waFunc;
    Render: waFunc;
    Quit: waProc;
    userData: pointer;
  end;

    //заголовок плагина
    PwinampVisHeader = ^winampVisHeader;
    winampVisHeader = record
      version: integer;
      description: ansistring;
      getModule: function( which: integer ): PwinampVisModule;cdecl;
    end;
var
    hdr: winampVisHeader;
    mod1: winampVisModule;
    VisForm: TForm1;
    SetForm: TForm2;
    lCol,lmCol,mCol,hCol: TColorsMap;
    inif: TINIFile;
    pl1,ph1, pl2,ph2, pl3,ph3, pl4,ph4: integer;
    ppl1,pph1, ppl2,pph2, ppl3,pph3, ppl4,pph4: real;

function getModule( which: integer ): PwinampVisModule;cdecl;
begin
  case which of
    0: result:=pointer( @mod1 );
  else result:=nil
  end;
end;

//передача заголовка плагина Winamp'у
function winampVisGetHeader: PwinampVisHeader;cdecl;
begin
  result:=@hdr;
end;

//эта процедура вызывается при нажатии на кнопку "Конфигурация"
procedure config( this_mod: PwinampVisModule );cdecl;
begin
  MessageBox(0,'Plugin for Winamp LigthPack-DISCO by maxilam'+#13+'maxilam@rambler.ru','LigthPack-DISCO',MB_OK);
end;

//инициализация плагина, если всё нормально, то должны вернуть 0
function init( this_mod: PwinampVisModule ): integer;cdecl;
begin
  result:=0;
  VisForm:= TForm1.Create(nil);
  VisForm.Show;
end;

//получаем картину звука
function render( this_mod: PwinampVisModule ): integer;cdecl;
var
  i,v: integer;
  m1,m2,m3,m4: integer;
begin
  m1:=0; m2:= 0; m3:= 0; m4:=0;

  for i:=0 to 575 do
    begin
      if i in [pl1..ph1] then
        begin
          v:= round((this_mod.spectrumData[0,i] + this_mod.spectrumData[1,i]) / 2);
          if v > m1 then
            m1:= v;
        end;

      if i in [pl2..ph2] then
        begin
          v:= round((this_mod.spectrumData[0,i] + this_mod.spectrumData[1,i]) / 2);
          if v > m2 then
            m2:= v;
        end;

      if i in [pl3..ph3] then
        begin
          v:= round((this_mod.spectrumData[0,i] + this_mod.spectrumData[1,i]) / 2);
          if v > m3 then
            m3:= v;
        end;

      if (i >= pl4) and (i <= ph4) then
        begin
          v:= round((this_mod.spectrumData[0,i] + this_mod.spectrumData[1,i]) / 2);
          if v > m4 then
            m4:= v;
        end;

      VisForm.Sph1:= Round((m1 * 100) / 255);
      VisForm.Sph2:= Round((m2 * 100) / 255);
      VisForm.Sph3:= Round((m3 * 100) / 255);
      VisForm.Sph4:= Round((m4 * 100) / 255);
    end;

  VisForm.GLCadencer1.Progress;
  result:=0;
end;

//наши действия при закрытии плагина
procedure quit( this_mod: PwinampVisModule );cdecl;
begin
  if VisForm <> nil then
    VisForm.Destroy;
end;

procedure LoadSettings;
begin
    inif:= TINIFile.Create('LightPack-WINAMP.ini');

    lCol.min:= inif.ReadInteger('lCol','min',0);
    lCol.max:= inif.ReadInteger('lCol','max',200);
    lCol.Color.r:= inif.ReadInteger('lCol','r',255);
    lCol.Color.g:= inif.ReadInteger('lCol','g',0);
    lCol.Color.b:= inif.ReadInteger('lCol','b',0);

    lmCol.min:= inif.ReadInteger('lmCol','min',200);
    lmCol.max:= inif.ReadInteger('lmCol','max',800);
    lmCol.Color.r:= inif.ReadInteger('lmCol','r',0);
    lmCol.Color.g:= inif.ReadInteger('lmCol','g',0);
    lmCol.Color.b:= inif.ReadInteger('lmCol','b',255);

    mCol.min:= inif.ReadInteger('mCol','min',800);
    mCol.max:= inif.ReadInteger('mCol','max',3500);
    mCol.Color.r:= inif.ReadInteger('mCol','r',0);
    mCol.Color.g:= inif.ReadInteger('mCol','g',255);
    mCol.Color.b:= inif.ReadInteger('mCol','b',0);

    hCol.min:= inif.ReadInteger('hCol','min',3500);
    hCol.max:= inif.ReadInteger('hCol','max',20000);
    hCol.Color.r:= inif.ReadInteger('hCol','r',255);
    hCol.Color.g:= inif.ReadInteger('hCol','g',255);
    hCol.Color.b:= inif.ReadInteger('hCol','b',128);

    inif.Free;

    pl1:= 0;
    pph1:= (lCol.max * 100) / hCol.max;
    ph1:= Round((pph1 / 100) * 570);

    ppl2:= (lmCol.min * 100) / hCol.max;
    pph2:= (lmCol.max * 100) / hCol.max;
    pl2:= Round((ppl2 / 100) * 575);
    ph2:= Round((pph2 / 100) * 575);

    ppl3:= (mCol.min * 100) / hCol.max;
    pph3:= (mCol.max * 100) / hCol.max;
    pl3:= Round((ppl3 / 100) * 575);
    ph3:= Round((pph3 / 100) * 575);

    ppl4:= (hCol.min * 100) / hCol.max;
    pph4:= (hCol.max * 100) / hCol.max;
    pl4:= Round((ppl4 / 100) * 575);
    ph4:= Round((pph4 / 100) * 575);
end;

//экспортируемая функция
exports
    winampVisGetHeader;

begin
    LoadSettings;
    //задаём начальные значения для структуры плагина
    hdr.version:=VIS_HDRVER;
    hdr.description:='LigthPack-DISCO';
    hdr.getModule:=getModule;
    mod1.description:='LigthPack-DISCO';
    mod1.hwndParent:=0;
    mod1.hDllInstance:=0;
    mod1.sRate:=0;
    mod1.nCh:=0;
    mod1.latencyMs:=25;
    mod1.delayMs:=25;
    mod1.spectrumNch:=2;
    mod1.waveformNch:=0;
    mod1.Config:=@config;
    mod1.Init:=@init;
    mod1.Render:=@render;
    mod1.Quit:=@quit;
end.

