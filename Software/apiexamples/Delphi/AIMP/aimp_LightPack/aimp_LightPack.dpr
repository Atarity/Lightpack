library aimp_LightPack;

uses Windows, SysUtils, WinSock, AIMP_SDK, Messages, Dialogs, LightPack;

const
  BorderSize = 4;
  colors: Array [1..10] of String = ('0,0,255', '262,100,74', '133,0,122', '202,0,54', '255,0,0',
                                     '0,0,255', '262,100,74', '133,0,122', '202,0,54', '255,0,0');
  LedMap: Array [1..10] of Integer = (1,2,3,4,5, 6,7,8,9,10);

type
  TAIMP2DemoVisual = class(TInterfacedObject, IAIMP2VisualPlugin)
  private
    FScale: Single;
    FClientRect: TRect;
    myLightPack: TLightPack;
    connectionMessage: String;
    counter: Integer;
    leds: Array [1..10] of Boolean;

  public
    function AuthorName: PWideChar; stdcall;
    function Initialize: LongBool; stdcall;
    function PluginFlags: Cardinal; stdcall;
    function PluginInfo: PWideChar; stdcall;
    function PluginName: PWideChar; stdcall;
    procedure Deinitialize; stdcall;
    procedure DisplayClick(X: Integer; Y: Integer); stdcall;
    procedure DisplayRender(DC: HDC; AData: PAIMPVisualData); stdcall;
    procedure DisplayResize(AWidth: Integer; AHeight: Integer); stdcall;
  end;


function TAIMP2DemoVisual.AuthorName: PWideChar; stdcall;
begin
  Result := 'Nikolay Vasilchuk';
end;

function TAIMP2DemoVisual.Initialize: LongBool; stdcall;
var i: Integer;
begin
  Result := True;
  myLightPack := TLightPack.lightpack('127.0.0.1', 3636, LedMap);
  connectionMessage := myLightPack.connect;
  if (connectionMessage <> '') then begin
    myLightPack.lock;
    for i := 1 to 10 do begin
      leds[i] := False;
    end;
    counter := 0;
    myLightPack.setColorToAll(0,0,0);
    myLightPack.setSmooth(5);
  end;
end;

function TAIMP2DemoVisual.PluginFlags: Cardinal; stdcall;
begin
  Result := 0;
end;

function TAIMP2DemoVisual.PluginInfo: PWideChar; stdcall;
begin
  Result := 'LightPack visual plugin';
end;

function TAIMP2DemoVisual.PluginName: PWideChar; stdcall;
begin
  Result := 'LightPack';
end;

procedure TAIMP2DemoVisual.Deinitialize; stdcall;
begin
  myLightPack.disconnect;
  myLightPack.Destroy;
end;

procedure TAIMP2DemoVisual.DisplayClick(X: Integer; Y: Integer); stdcall;
begin
 //
end;

procedure TAIMP2DemoVisual.DisplayResize(AWidth: Integer; AHeight: Integer); stdcall;
begin
  FScale := (AWidth - 2 * BorderSize) / 100;

  FClientRect.Left := 0;
  FClientRect.Top := 0;
  FClientRect.Bottom := AHeight;
  FClientRect.Right := AWidth; 
end;

procedure TAIMP2DemoVisual.DisplayRender(DC: HDC; AData: PAIMPVisualData); stdcall;
var
  ABrush: HBRUSH;
  R: TRect;
  lR, lL: Integer;
  command: String;
  currentLeds : Array [1..10] of Boolean;
  i: Integer;
begin
  lL:= AData^.LevelL;
  lR:= AData^.LevelR;

  if (lL > 0) and (lR > 0) then begin
    ABrush := CreateSolidBrush($00363636);
    FillRect(DC, FClientRect, ABrush);
    DeleteObject(ABrush);

    ABrush := CreateSolidBrush($001D57AE);
    R.Top := BorderSize;
    R.Left := BorderSize;
    R.Bottom := FClientRect.Bottom div 2 - BorderSize;
    R.Right := Trunc(FScale * lL);
    FillRect(DC, R, ABrush);

    R.Top := FClientRect.Bottom div 2 + BorderSize;
    R.Bottom := FClientRect.Bottom - BorderSize;
    R.Right := Trunc(FScale * lR);
    FillRect(DC, R, ABrush);

    DeleteObject(ABrush);

    Inc(counter);
    if (counter = 3) then begin
      if (connectionMessage <> '') then begin
        for i := 1 to 10 do begin
          currentLeds[i] := False;
        end;
        counter := 0;
        command := '';

      	currentLeds[1] := lL > 5;
      	currentLeds[2] := lL > 15;
      	currentLeds[3] := lL > 30;
      	currentLeds[4] := lL > 50;
      	currentLeds[5] := lL > 70;

      	currentLeds[6] := lR > 5;
      	currentLeds[7] := lR > 15;
      	currentLeds[8] := lR > 30;
      	currentLeds[9] := lR > 50;
      	currentLeds[10] := lR > 70;

        for i := 1 to 10 do begin
          if (currentLeds[i] <> leds[i]) then begin
            leds[i] := currentLeds[i];
            if (currentLeds[i] = True) then
              command := command + IntToStr(LedMap[i]) + '-' + colors[i] + ';'
            else
              command := command + IntToStr(LedMap[i]) + '-0,0,0;'
          end;
        end;
        
        if (command <> '') then
          myLightPack.sendCommand('setcolor:' + command);
      end;
    end;
  end;
end;

// - - - -

function AIMP_QueryVisual: IAIMP2VisualPlugin; stdcall;
begin
  Result := TAIMP2DemoVisual.Create;
end;

exports
  AIMP_QueryVisual;

begin
end.
