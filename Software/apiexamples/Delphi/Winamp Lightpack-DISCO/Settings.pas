unit Settings;
//Эта форма ни как не отображается в плагине. Она осталась от самых первых версий плагина и нужна для хранения некоторых настроек
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, Mask, RXSpin, ExtCtrls, INIFiles, ComCtrls, ShlObj;

type
  TRGBColor = record
    r,g,b: byte;
  end;

  TColorsMap = record
    min,max: integer;
    Color: TRGBColor;
  end;

  TForm2 = class(TForm)
    RxSpinEdit2: TRxSpinEdit;
    Label6: TLabel;
    RxSpinEdit3: TRxSpinEdit;
    Label7: TLabel;
    RxSpinEdit4: TRxSpinEdit;
    Label8: TLabel;
    RxSpinEdit5: TRxSpinEdit;
    Label9: TLabel;
    RxSpinEdit6: TRxSpinEdit;
    Label10: TLabel;
    RxSpinEdit7: TRxSpinEdit;
    Label12: TLabel;
    ColorDialog1: TColorDialog;
    Button1: TButton;
    Button2: TButton;
    Label1: TLabel;
    Edit1: TEdit;
    Edit2: TEdit;
    Edit3: TEdit;
    Edit4: TEdit;
    Edit5: TEdit;
    Edit6: TEdit;
    Edit7: TEdit;
    Edit8: TEdit;
    Edit9: TEdit;
    Edit10: TEdit;
    Edit11: TEdit;
    Edit12: TEdit;
    Shape1: TShape;
    Shape2: TShape;
    Shape3: TShape;
    Shape4: TShape;
    GroupBox1: TGroupBox;
    CheckBox5: TCheckBox;
    CheckBox6: TCheckBox;
    CheckBox7: TCheckBox;
    CheckBox8: TCheckBox;
    CheckBox9: TCheckBox;
    CheckBox10: TCheckBox;
    CheckBox11: TCheckBox;
    CheckBox12: TCheckBox;
    CheckBox13: TCheckBox;
    CheckBox14: TCheckBox;
    GroupBox2: TGroupBox;
    CheckBox15: TCheckBox;
    CheckBox16: TCheckBox;
    CheckBox17: TCheckBox;
    CheckBox18: TCheckBox;
    CheckBox19: TCheckBox;
    CheckBox20: TCheckBox;
    CheckBox21: TCheckBox;
    CheckBox22: TCheckBox;
    CheckBox23: TCheckBox;
    CheckBox24: TCheckBox;
    GroupBox3: TGroupBox;
    CheckBox25: TCheckBox;
    CheckBox26: TCheckBox;
    CheckBox27: TCheckBox;
    CheckBox28: TCheckBox;
    CheckBox29: TCheckBox;
    CheckBox30: TCheckBox;
    CheckBox31: TCheckBox;
    CheckBox32: TCheckBox;
    CheckBox33: TCheckBox;
    CheckBox34: TCheckBox;
    GroupBox4: TGroupBox;
    CheckBox35: TCheckBox;
    CheckBox36: TCheckBox;
    CheckBox37: TCheckBox;
    CheckBox38: TCheckBox;
    CheckBox39: TCheckBox;
    CheckBox40: TCheckBox;
    CheckBox41: TCheckBox;
    CheckBox42: TCheckBox;
    CheckBox43: TCheckBox;
    CheckBox44: TCheckBox;
    RxSpinEdit1: TRxSpinEdit;
    Label2: TLabel;
    Label13: TLabel;
    Label14: TLabel;
    Label15: TLabel;
    Label16: TLabel;
    RxSpinEdit10: TRxSpinEdit;
    RxSpinEdit11: TRxSpinEdit;
    procedure SaveSettings;
    procedure LoadSettings;
    procedure Shape1MouseUp(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure Shape2MouseUp(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure Shape3MouseUp(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure Shape4MouseUp(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure Button2Click(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure LoadDefaultSet;
    function GetSpecialPath(CSIDL: word): string;
  private
    MyDocDir: string;
  public
    { Public declarations }
  end;

var
  Form2: TForm2;

implementation

{$R *.dfm}

function TForm2.GetSpecialPath(CSIDL: word): string;
var
  s: string;
begin
  SetLength(s, MAX_PATH);
  if not SHGetSpecialFolderPath(0, PChar(s), CSIDL, true) then
    s := '';
  result := PChar(s);
end;

procedure TForm2.Button1Click(Sender: TObject);
begin
  SaveSettings;
  Close;
end;

procedure TForm2.Button2Click(Sender: TObject);
begin
  SaveSettings;
end;

procedure TForm2.FormCreate(Sender: TObject);
begin
  MyDocDir:= GetSpecialPath($0005);
end;

procedure TForm2.LoadDefaultSet;
begin
  CheckBox5.Checked:= false;
  CheckBox6.Checked:= true;
  CheckBox7.Checked:= false;
  CheckBox8.Checked:= false;
  CheckBox9.Checked:= false;
  CheckBox10.Checked:= false;
  CheckBox11.Checked:= false;
  CheckBox12.Checked:= true;
  CheckBox13.Checked:= false;
  CheckBox14.Checked:= true;

  CheckBox15.Checked:= false;
  CheckBox16.Checked:= false;
  CheckBox17.Checked:= true;
  CheckBox18.Checked:= false;
  CheckBox19.Checked:= false;
  CheckBox20.Checked:= false;
  CheckBox21.Checked:= true;
  CheckBox22.Checked:= false;
  CheckBox23.Checked:= false;
  CheckBox24.Checked:= false;

  CheckBox25.Checked:= false;
  CheckBox26.Checked:= false;
  CheckBox27.Checked:= false;
  CheckBox28.Checked:= true;
  CheckBox29.Checked:= false;
  CheckBox30.Checked:= true;
  CheckBox31.Checked:= false;
  CheckBox32.Checked:= false;
  CheckBox33.Checked:= false;
  CheckBox34.Checked:= false;

  CheckBox35.Checked:= false;
  CheckBox36.Checked:= false;
  CheckBox37.Checked:= false;
  CheckBox38.Checked:= false;
  CheckBox39.Checked:= true;
  CheckBox40.Checked:= false;
  CheckBox41.Checked:= false;
  CheckBox42.Checked:= false;
  CheckBox43.Checked:= false;
  CheckBox44.Checked:= false;
end;

procedure TForm2.LoadSettings;
var
  inif: TINIFile;
begin
  inif:= TINIFile.Create(MyDocDir + '\ldisco.ini');

  CheckBox5.Checked:= inif.ReadBool('lCol','led1',false);
  CheckBox6.Checked:= inif.ReadBool('lCol','led2',true);
  CheckBox7.Checked:= inif.ReadBool('lCol','led3',false);
  CheckBox8.Checked:= inif.ReadBool('lCol','led4',false);
  CheckBox9.Checked:= inif.ReadBool('lCol','led5',false);
  CheckBox10.Checked:= inif.ReadBool('lCol','led6',false);
  CheckBox11.Checked:= inif.ReadBool('lCol','led7',false);
  CheckBox12.Checked:= inif.ReadBool('lCol','led8',true);
  CheckBox13.Checked:= inif.ReadBool('lCol','led9',false);
  CheckBox14.Checked:= inif.ReadBool('lCol','led10',true);

  CheckBox15.Checked:= inif.ReadBool('lmCol','led1',false);
  CheckBox16.Checked:= inif.ReadBool('lmCol','led2',false);
  CheckBox17.Checked:= inif.ReadBool('lmCol','led3',true);
  CheckBox18.Checked:= inif.ReadBool('lmCol','led4',false);
  CheckBox19.Checked:= inif.ReadBool('lmCol','led5',false);
  CheckBox20.Checked:= inif.ReadBool('lmCol','led6',false);
  CheckBox21.Checked:= inif.ReadBool('lmCol','led7',true);
  CheckBox22.Checked:= inif.ReadBool('lmCol','led8',false);
  CheckBox23.Checked:= inif.ReadBool('lmCol','led9',false);
  CheckBox24.Checked:= inif.ReadBool('lmCol','led10',false);

  CheckBox25.Checked:= inif.ReadBool('mCol','led1',false);
  CheckBox26.Checked:= inif.ReadBool('mCol','led2',false);
  CheckBox27.Checked:= inif.ReadBool('mCol','led3',false);
  CheckBox28.Checked:= inif.ReadBool('mCol','led4',true);
  CheckBox29.Checked:= inif.ReadBool('mCol','led5',false);
  CheckBox30.Checked:= inif.ReadBool('mCol','led6',true);
  CheckBox31.Checked:= inif.ReadBool('mCol','led7',false);
  CheckBox32.Checked:= inif.ReadBool('mCol','led8',false);
  CheckBox33.Checked:= inif.ReadBool('mCol','led9',false);
  CheckBox34.Checked:= inif.ReadBool('mCol','led10',false);

  CheckBox35.Checked:= inif.ReadBool('hCol','led1',false);
  CheckBox36.Checked:= inif.ReadBool('hCol','led2',false);
  CheckBox37.Checked:= inif.ReadBool('hCol','led3',false);
  CheckBox38.Checked:= inif.ReadBool('hCol','led4',false);
  CheckBox39.Checked:= inif.ReadBool('hCol','led5',true);
  CheckBox40.Checked:= inif.ReadBool('hCol','led6',false);
  CheckBox41.Checked:= inif.ReadBool('hCol','led7',false);
  CheckBox42.Checked:= inif.ReadBool('hCol','led8',false);
  CheckBox43.Checked:= inif.ReadBool('hCol','led9',false);
  CheckBox44.Checked:= inif.ReadBool('hCol','led10',false);

  inif.Free;
end;

procedure TForm2.SaveSettings;
var
  inif: TINIFile;
begin
  inif:= TINIFile.Create(MyDocDir + '\ldisco.ini');

  inif.WriteBool('lCol','led1',CheckBox5.Checked);
  inif.WriteBool('lCol','led2',CheckBox6.Checked);
  inif.WriteBool('lCol','led3',CheckBox7.Checked);
  inif.WriteBool('lCol','led4',CheckBox8.Checked);
  inif.WriteBool('lCol','led5',CheckBox9.Checked);
  inif.WriteBool('lCol','led6',CheckBox10.Checked);
  inif.WriteBool('lCol','led7',CheckBox11.Checked);
  inif.WriteBool('lCol','led8',CheckBox12.Checked);
  inif.WriteBool('lCol','led9',CheckBox13.Checked);
  inif.WriteBool('lCol','led10',CheckBox14.Checked);

  inif.WriteBool('lmCol','led1',CheckBox15.Checked);
  inif.WriteBool('lmCol','led2',CheckBox16.Checked);
  inif.WriteBool('lmCol','led3',CheckBox17.Checked);
  inif.WriteBool('lmCol','led4',CheckBox18.Checked);
  inif.WriteBool('lmCol','led5',CheckBox19.Checked);
  inif.WriteBool('lmCol','led6',CheckBox20.Checked);
  inif.WriteBool('lmCol','led7',CheckBox21.Checked);
  inif.WriteBool('lmCol','led8',CheckBox22.Checked);
  inif.WriteBool('lmCol','led9',CheckBox23.Checked);
  inif.WriteBool('lmCol','led10',CheckBox24.Checked);

  inif.WriteBool('mCol','led1',CheckBox25.Checked);
  inif.WriteBool('mCol','led2',CheckBox26.Checked);
  inif.WriteBool('mCol','led3',CheckBox27.Checked);
  inif.WriteBool('mCol','led4',CheckBox28.Checked);
  inif.WriteBool('mCol','led5',CheckBox29.Checked);
  inif.WriteBool('mCol','led6',CheckBox30.Checked);
  inif.WriteBool('mCol','led7',CheckBox31.Checked);
  inif.WriteBool('mCol','led8',CheckBox32.Checked);
  inif.WriteBool('mCol','led9',CheckBox33.Checked);
  inif.WriteBool('mCol','led10',CheckBox34.Checked);

  inif.WriteBool('hCol','led1',CheckBox35.Checked);
  inif.WriteBool('hCol','led2',CheckBox36.Checked);
  inif.WriteBool('hCol','led3',CheckBox37.Checked);
  inif.WriteBool('hCol','led4',CheckBox38.Checked);
  inif.WriteBool('hCol','led5',CheckBox39.Checked);
  inif.WriteBool('hCol','led6',CheckBox40.Checked);
  inif.WriteBool('hCol','led7',CheckBox41.Checked);
  inif.WriteBool('hCol','led8',CheckBox42.Checked);
  inif.WriteBool('hCol','led9',CheckBox43.Checked);
  inif.WriteBool('hCol','led10',CheckBox44.Checked);

  inif.Free;
end;

procedure TForm2.Shape1MouseUp(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
var
  rgbCol: array[0..2] of byte;
  col: integer;
begin
  if ColorDialog1.Execute then
    begin
      Shape1.Brush.Color:= ColorDialog1.Color;
      Col:= ColorToRGB(ColorDialog1.Color);
      rgbCol[0]:= col;
      rgbCol[1]:= col shr 8;
      rgbCol[2]:= col shr 16;
      Edit1.Text:= IntToStr(rgbCol[0]);
      Edit2.Text:= IntToStr(rgbCol[1]);
      Edit3.Text:= IntToStr(rgbCol[2]);
    end;
end;

procedure TForm2.Shape2MouseUp(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
var
  rgbCol: array[0..2] of byte;
  col: integer;
begin
  if ColorDialog1.Execute then
    begin
      Shape2.Brush.Color:= ColorDialog1.Color;
      Col:= ColorToRGB(ColorDialog1.Color);
      rgbCol[0]:= col;
      rgbCol[1]:= col shr 8;
      rgbCol[2]:= col shr 16;
      Edit6.Text:= IntToStr(rgbCol[0]);
      Edit5.Text:= IntToStr(rgbCol[1]);
      Edit4.Text:= IntToStr(rgbCol[2]);
    end;
end;

procedure TForm2.Shape3MouseUp(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
var
  rgbCol: array[0..2] of byte;
  col: integer;
begin
  if ColorDialog1.Execute then
    begin
      Shape3.Brush.Color:= ColorDialog1.Color;
      Col:= ColorToRGB(ColorDialog1.Color);
      rgbCol[0]:= col;
      rgbCol[1]:= col shr 8;
      rgbCol[2]:= col shr 16;
      Edit9.Text:= IntToStr(rgbCol[0]);
      Edit8.Text:= IntToStr(rgbCol[1]);
      Edit7.Text:= IntToStr(rgbCol[2]);
    end;
end;

procedure TForm2.Shape4MouseUp(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
var
  rgbCol: array[0..2] of byte;
  col: integer;
begin
  if ColorDialog1.Execute then
    begin
      Shape4.Brush.Color:= ColorDialog1.Color;
      Col:= ColorToRGB(ColorDialog1.Color);
      rgbCol[0]:= col;
      rgbCol[1]:= col shr 8;
      rgbCol[2]:= col shr 16;
      Edit12.Text:= IntToStr(rgbCol[0]);
      Edit11.Text:= IntToStr(rgbCol[1]);
      Edit10.Text:= IntToStr(rgbCol[2]);
    end;
end;

end.
