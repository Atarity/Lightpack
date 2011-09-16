unit LightPack;

interface

uses
  SysUtils,
  Forms,
  Dialogs,
  ScktComp;

type
  TLightPack = Class
  private
    waiting: Boolean;
    answer: String;

    lSocket: TClientSocket;
    lHost: String;
    lPort: Integer;
    lLedMap: Array[1..10] of Integer;

    procedure lSocketRead(Sender: TObject; Socket: TCustomWinSocket);
    procedure lSocketConnect(Sender: TObject; Socket: TCustomWinSocket);
    procedure lSocketError(Sender: TObject; Socket: TCustomWinSocket; ErrorEvent: TErrorEvent; var ErrorCode: Integer);

  public
    debug: Boolean;

    function connect(): String;
    procedure disconnect();
    function lock(): String;
    function unlock(): String;

    function getProfile(): String;
    function getProfiles(): String;
    function getStatus(): String;
    function getAPIStatus(): String;

    function turnOn(): String;
    function turnOff(): String;
    function setProfile(Profile: String): String;
    function setColor(n: Integer; r: Integer; g: Integer; b: Integer): String;
    function setColorToAll(r: Integer; g: Integer; b: Integer): String;
    function setGamma(Gamma: Double): String;
    function setSmooth(Smooth: Integer): String;

    function getAnswer(Command: String): String;
    function sendCommand(Command: String): String;

    Constructor Create; overload;
  published
    Constructor lightpack(Host: String; Port: Integer; LedMap: Array of Integer);
    Destructor  Destroy;
end;

implementation

{ --- Class procedures --- }

constructor TLightPack.Create;
var i: Integer;
begin
  inherited;
  lHost := '127.0.0.1';
  lPort := 3636;
  for i := Low(lLedMap) to High(lLedMap) do
    lLedMap[i] := i;
  lSocket := TClientSocket.Create( Nil );
  debug := False;

  lSocket.Host := lHost;
  lSocket.Port := lPort;
  lSocket.ClientType := ctNonBlocking;
  lSocket.OnRead := lSocketRead;
  lSocket.OnConnect := lSocketConnect;
  lSocket.OnError := lSocketError;
end;

constructor TLightPack.lightpack(Host: String; Port: Integer; LedMap: Array of Integer);
var i: Integer;
begin
  lHost := Host;
  lPort := Port;
  for i := Low(lLedMap) to High(lLedMap) do
    lLedMap[i] := LedMap[i-1];
  lSocket := TClientSocket.Create( Nil );
  debug := False;

  lSocket.Host := lHost;
  lSocket.Port := lPort;
  lSocket.ClientType := ctNonBlocking;
  lSocket.OnRead := lSocketRead;
  lSocket.OnConnect := lSocketConnect;
  lSocket.OnError := lSocketError;
end;

destructor TLightPack.Destroy;
begin
  lSocket.Free;
  inherited;
end;

{ --- Socket functions --- }

function TLightPack.getAnswer(Command: String): String;
begin
  waiting := True;
  answer := '';
  lSocket.Socket.SendText(Command + #$0D+#$0A);
  while (waiting = True) do
    Application.ProcessMessages;
  Result := answer;
end;

function TLightPack.sendCommand(Command: String): String;
begin
  waiting := True;
  answer := '';
  lSocket.Socket.SendText(Command + #$0D+#$0A);
  if (debug = True) then begin
    while (waiting = True) do
      Application.ProcessMessages;
    Result := answer;
  end;
end;

procedure TLightPack.lSocketRead(Sender: TObject; Socket: TCustomWinSocket);
begin
  waiting := False;
  answer := Socket.ReceiveText;
end;

procedure TLightPack.lSocketConnect(Sender: TObject; Socket: TCustomWinSocket);
begin
  // Nothing to do
end;

procedure TLightPack.lSocketError(Sender: TObject; Socket: TCustomWinSocket; ErrorEvent: TErrorEvent; var ErrorCode: Integer);
begin
  waiting := False;
  lSocket.Close;
  ErrorCode := 0;
end;

{ --- Base procedures for work with server --- }

function TLightPack.connect(): String;
begin
  lSocket.Open;
  waiting := True;
  answer := '';
  while (waiting = True) do
    Application.ProcessMessages;
  Result := answer;
end;

procedure TLightPack.disconnect();
begin
  lSocket.Close;
end;

function TLightPack.lock(): String;
begin
  // Lock api
  Result := sendCommand('lock');
end;

function TLightPack.unlock(): String;
begin
  // UnLock api
  Result := sendCommand('unlock');
end;

{ --- Get functions --- }

function TLightPack.getProfile(): String;
begin
  // Get current profile
  Result := getAnswer('getprofile');
  Delete(Result, 1, Pos(':', Result));
end;

function TLightPack.getProfiles(): String;
begin
  // Get all profiles
  Result := getAnswer('getprofiles');
  Delete(Result, 1, Pos(':', Result));
end;

function TLightPack.getStatus(): String;
begin
  // Get status
  Result := getAnswer('getstatus');
  Delete(Result, 1, Pos(':', Result));
end;

function TLightPack.getAPIStatus(): String;
begin
  // Get API status
  Result := getAnswer('getstatusapi');
  Delete(Result, 1, Pos(':', Result));
end;

{ --- Set functions --- }

function TLightPack.turnOn(): String;
begin
  // Turn on LightPack
  Result := sendCommand('setstatus:on');
end;

function TLightPack.turnOff(): String;
begin
  // Turn Off LightPack
  Result := sendCommand('setstatus:off');
end;

function TLightPack.setProfile(Profile: String): String;
begin
  // Active profile
  Result := sendCommand('setprofile:' + Profile);
end;

function TLightPack.setColor(n: Integer; r: Integer; g: Integer; b: Integer): String;
begin
  // Set color to one lamp
  Result := sendCommand('setcolor:' + IntToStr(lLedMap[n]) + '-' + IntToStr(r) + ',' + IntToStr(g) + ',' + IntToStr(b));
end;

function TLightPack.setColorToAll(r: Integer; g: Integer; b: Integer): String;
var
  i: Integer;
  command: String;
  sr, sg, sb: String;
begin
  // Set color to all lamps
  command := '';
  sr := IntToStr(r);
  sg := IntToStr(g);
  sb := IntToStr(b);
  for i := Low(lLedMap) to High(lLedMap) do begin
    command := command + IntToStr(lLedMap[i]) + '-' + sr + ',' + sg + ',' + sb + ';';
  end;
  Result := sendCommand('setcolor:' + command);
end;

function TLightPack.setGamma(Gamma: Double): String;
begin
  // Set gamma correction
  Result := sendCommand('setgamma:' + FloatToStr(Gamma));
end;

function TLightPack.setSmooth(Smooth: Integer): String;
begin
  // Set Smooth
  Result := sendCommand('setsmooth:' + IntToStr(Smooth));
end;

end.
