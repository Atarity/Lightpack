unit Unit1;

interface

uses
  SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, ScktComp, StdCtrls, LightPack;

type
  TForm1 = class(TForm)
    Memo1: TMemo;
    Button1: TButton;
    Edit1: TEdit;
    Button2: TButton;
    Button3: TButton;
    Button4: TButton;
    Button5: TButton;
    Button6: TButton;
    Button7: TButton;
    Button8: TButton;
    Button9: TButton;
    Button10: TButton;
    Button11: TButton;
    Button12: TButton;
    Button13: TButton;
    Button14: TButton;
    Button15: TButton;
    Button16: TButton;
    CheckBox1: TCheckBox;
    procedure Button2Click(Sender: TObject);
    procedure Button3Click(Sender: TObject);
    procedure Button8Click(Sender: TObject);
    procedure Button9Click(Sender: TObject);
    procedure Button4Click(Sender: TObject);
    procedure Button5Click(Sender: TObject);
    procedure Button6Click(Sender: TObject);
    procedure Button7Click(Sender: TObject);
    procedure Button10Click(Sender: TObject);
    procedure Button11Click(Sender: TObject);
    procedure Button14Click(Sender: TObject);
    procedure Button13Click(Sender: TObject);
    procedure Button12Click(Sender: TObject);
    procedure Button15Click(Sender: TObject);
    procedure Button16Click(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure Button17Click(Sender: TObject);
    procedure CheckBox1Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form1: TForm1;
  myLightPack : TLightPack;

implementation

{$R *.dfm}


procedure TForm1.Button2Click(Sender: TObject);
var ConnectionMessage: String;
begin
  Memo1.Lines.Add('-- Connection --');
  ConnectionMessage := myLightPack.connect;
  if (ConnectionMessage <> '') then begin
    Memo1.Lines.Add('Connected: ' + ConnectionMessage);
    CheckBox1.Enabled := True;
    Button1.Enabled := True;
    Button2.Enabled := False;
    Button3.Enabled := True;
    Button4.Enabled := True;
    Button5.Enabled := True;
    Button6.Enabled := True;
    Button7.Enabled := True;
    Button8.Enabled := True;
  end else begin
    Memo1.Lines.Add('Connection error');
  end;
end;

procedure TForm1.Button3Click(Sender: TObject);
begin
  Memo1.Lines.Add('-- Disconnect --');
  myLightPack.disconnect;

 CheckBox1.Enabled := False;
  Button1.Enabled := False;
  Button2.Enabled := True;
  Button3.Enabled := False;
  Button4.Enabled := False;
  Button5.Enabled := False;
  Button6.Enabled := False;
  Button7.Enabled := False;
  Button8.Enabled := False;

  Button9.Enabled := False;
  Button10.Enabled := False;
  Button11.Enabled := False;
  Button12.Enabled := False;
  Button13.Enabled := False;
  Button14.Enabled := False;
  Button15.Enabled := False;
  Button16.Enabled := False;
end;

procedure TForm1.Button8Click(Sender: TObject);
begin
  Memo1.Lines.Add('>> lock');
  Memo1.Lines.Add('<< ' + myLightPack.lock);

  Button8.Enabled := False;
  Button9.Enabled := True;
  Button10.Enabled := True;
  Button11.Enabled := True;
  Button12.Enabled := True;
  Button13.Enabled := True;
  Button14.Enabled := True;
  Button15.Enabled := True;
  Button16.Enabled := True;
end;

procedure TForm1.Button9Click(Sender: TObject);
begin
  Memo1.Lines.Add('>> unlock');
  Memo1.Lines.Add('<< ' + myLightPack.unlock);

  Button8.Enabled := True;
  Button9.Enabled := False;
  Button10.Enabled := False;
  Button11.Enabled := False;
  Button12.Enabled := False;
  Button13.Enabled := False;
  Button14.Enabled := False;
  Button15.Enabled := False;
  Button16.Enabled := False;
end;

procedure TForm1.Button4Click(Sender: TObject);
begin
  Memo1.Lines.Add('>> getProfile');
  Memo1.Lines.Add('<< ' + myLightPack.getProfile);
end;

procedure TForm1.Button5Click(Sender: TObject);
begin
  Memo1.Lines.Add('>> getProfiles');
  Memo1.Lines.Add('<< ' +  myLightPack.getProfiles);
end;

procedure TForm1.Button6Click(Sender: TObject);
begin
  Memo1.Lines.Add('>> getStatus');
  Memo1.Lines.Add('<< ' + myLightPack.getStatus);
end;

procedure TForm1.Button7Click(Sender: TObject);
begin
  Memo1.Lines.Add('>> getAPIStatus');
  Memo1.Lines.Add('<< ' + myLightPack.getAPIStatus);
end;

procedure TForm1.Button10Click(Sender: TObject);
begin
  Memo1.Lines.Add('>> turnOn');
  Memo1.Lines.Add('<< ' + myLightPack.turnOn);
end;

procedure TForm1.Button11Click(Sender: TObject);
begin
  Memo1.Lines.Add('>> turnOff');
  Memo1.Lines.Add('<< ' + myLightPack.turnOff);
end;

procedure TForm1.Button14Click(Sender: TObject);
begin
  Memo1.Lines.Add('>> setColorToAll:255,0,255');
  Memo1.Lines.Add('<< ' + myLightPack.setColorToAll(255, 0, 255));
end;

procedure TForm1.Button13Click(Sender: TObject);
begin
  Memo1.Lines.Add('>> setColor:3-0,255,0');
  Memo1.Lines.Add('<< ' + myLightPack.setColor(3, 0, 255, 0));
end;

procedure TForm1.Button12Click(Sender: TObject);
begin
  Memo1.Lines.Add('>> setProfile:default');
  Memo1.Lines.Add('<< ' + myLightPack.setProfile('default'));
end;

procedure TForm1.Button15Click(Sender: TObject);
begin
  Memo1.Lines.Add('>> setGamma:1.543');
  Memo1.Lines.Add('<< ' + myLightPack.setGamma(1.543));
end;

procedure TForm1.Button16Click(Sender: TObject);
begin
  Memo1.Lines.Add('>> setSmooth:100');
  Memo1.Lines.Add('<< ' + myLightPack.setSmooth(100));
end;

procedure TForm1.Button1Click(Sender: TObject);
var command: string;
begin
  command := Edit1.Text;
  Memo1.Lines.Add('>> ' + command);
  Memo1.Lines.Add('<< ' + myLightPack.getAnswer(command));
end;

procedure TForm1.FormCreate(Sender: TObject);
begin
  myLightPack := TLightPack.lightpack('127.0.0.1', 3636, [1,2,3,4,5, 10,9,8,7,6]);
end;

procedure TForm1.FormDestroy(Sender: TObject);
begin
  myLightPack.disconnect;
end;

procedure TForm1.Button17Click(Sender: TObject);
  var i: Integer;
begin
  for i := 0 to 10 do begin
    myLightPack.setColorToAll(0,255,0);
    Sleep(100);
    Application.ProcessMessages;
    myLightPack.setColorToAll(0,0,0);
    Sleep(100);
    Application.ProcessMessages;
  end;
end;

procedure TForm1.CheckBox1Click(Sender: TObject);
begin
  myLightPack.debug := CheckBox1.Checked;
end;

end.
