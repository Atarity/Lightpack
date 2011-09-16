unit vis;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, ExtCtrls, StdCtrls, Settings, Sockets, GLWin32Viewer,
  GLCrossPlatform, BaseClasses, GLScene, GLObjects, GLCoordinates, GLCadencer,
  GLGeomObjects, GLMaterial, GLCustomShader, GLSLShader, PNGImage, ComCtrls,
  GLHUDObjects, GLBitmapFont, GLWindowsFont, JPEG, GLShadowPlane, VectorTypes,
  VectorGeometry, Menus, Mask, RXSpin, INIFiles, GLBlur, GLShadowVolume;

type
  TRGBColor = record
    r,g,b: byte;
  end;

  TForm1 = class(TForm)
    Timer1: TTimer;
    Button1: TButton;
    TcpClient1: TTcpClient;
    Button2: TButton;
    GLScene1: TGLScene;
    GLSceneViewer1: TGLSceneViewer;
    GLCadencer1: TGLCadencer;
    GLCamera1: TGLCamera;
    GLDummyCube1: TGLDummyCube;
    GLCube1: TGLCube;
    GLPlane1: TGLPlane;
    LED1: TGLSprite;
    GLMaterialLibrary1: TGLMaterialLibrary;
    LED2: TGLSprite;
    RLed1: TGLSphere;
    RLed2: TGLSphere;
    RLed4: TGLSphere;
    LED4: TGLSprite;
    RLed5: TGLSphere;
    LED5: TGLSprite;
    RLed6: TGLSphere;
    LED6: TGLSprite;
    RLed7: TGLSphere;
    LED7: TGLSprite;
    RLed8: TGLSphere;
    LED8: TGLSprite;
    RLed9: TGLSphere;
    LED9: TGLSprite;
    RLed10: TGLSphere;
    LED10: TGLSprite;
    GLCube2: TGLCube;
    GLCube3: TGLCube;
    GLCube4: TGLCube;
    RLed3: TGLSphere;
    LED3: TGLSprite;
    GLWindowsBitmapFont1: TGLWindowsBitmapFont;
    GLLightSource2: TGLLightSource;
    GLCube5: TGLCube;
    Timer2: TTimer;
    LEDS: TGLDummyCube;
    GLFlatText1: TGLFlatText;
    GLFlatText2: TGLFlatText;
    GLFlatText3: TGLFlatText;
    GLFlatText4: TGLFlatText;
    GLFlatText5: TGLFlatText;
    GLFlatText6: TGLFlatText;
    GLFlatText7: TGLFlatText;
    GLFlatText8: TGLFlatText;
    GLFlatText9: TGLFlatText;
    GLFlatText10: TGLFlatText;
    PopupMenu1: TPopupMenu;
    Color1: TMenuItem;
    GLTorus1: TGLTorus;
    PopUpMenuSetColor: TGLDummyCube;
    SetColor1: TGLCube;
    SetColor2: TGLCube;
    SetColor3: TGLCube;
    SetColor4: TGLCube;
    HintText1: TGLFlatText;
    HintText3: TGLFlatText;
    HintText4: TGLFlatText;
    HintText2: TGLFlatText;
    Timer3: TTimer;
    GroupBox2: TGroupBox;
    Shape1: TShape;
    RxSpinEdit1: TRxSpinEdit;
    Label2: TLabel;
    Label3: TLabel;
    RxSpinEdit2: TRxSpinEdit;
    Label4: TLabel;
    Label5: TLabel;
    Edit1: TEdit;
    Edit2: TEdit;
    Edit3: TEdit;
    GroupBox1: TGroupBox;
    Shape2: TShape;
    Label6: TLabel;
    Label7: TLabel;
    Label8: TLabel;
    Label9: TLabel;
    RxSpinEdit3: TRxSpinEdit;
    RxSpinEdit4: TRxSpinEdit;
    Edit4: TEdit;
    Edit5: TEdit;
    Edit6: TEdit;
    GroupBox3: TGroupBox;
    Shape3: TShape;
    Label10: TLabel;
    Label11: TLabel;
    Label12: TLabel;
    Label13: TLabel;
    RxSpinEdit5: TRxSpinEdit;
    RxSpinEdit6: TRxSpinEdit;
    Edit7: TEdit;
    Edit8: TEdit;
    Edit9: TEdit;
    GroupBox4: TGroupBox;
    Shape4: TShape;
    Label14: TLabel;
    Label15: TLabel;
    Label16: TLabel;
    Label17: TLabel;
    RxSpinEdit7: TRxSpinEdit;
    RxSpinEdit8: TRxSpinEdit;
    Edit10: TEdit;
    Edit11: TEdit;
    Edit12: TEdit;
    ColorDialog1: TColorDialog;
    Label18: TLabel;
    RxSpinEdit9: TRxSpinEdit;
    Button5: TButton;
    Button6: TButton;
    RxSpinEdit10: TRxSpinEdit;
    Label1: TLabel;
    Label19: TLabel;
    RxSpinEdit11: TRxSpinEdit;
    GLLightSource1: TGLLightSource;
    GLLightSource4: TGLLightSource;
    GLShadowVolume1: TGLShadowVolume;
    procedure Timer1Timer(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure GLCadencer1Progress(Sender: TObject; const deltaTime,
      newTime: Double);
    procedure SetSettings;
    procedure FormShow(Sender: TObject);
    procedure GLSceneViewer1MouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure GLSceneViewer1MouseUp(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure GLSceneViewer1MouseMove(Sender: TObject; Shift: TShiftState; X,
      Y: Integer);
    procedure Timer2Timer(Sender: TObject);
    procedure Timer3Timer(Sender: TObject);
    procedure UpdateLEDS;
    procedure SetupLED(id: integer);
    procedure Shape1MouseUp(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure Shape2MouseUp(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure Shape3MouseUp(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure Shape4MouseUp(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure LoadSettings;
    procedure SaveSettings;
    procedure Button5Click(Sender: TObject);
    procedure Button6Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  private
    oSph1,oSph2,oSph3,oSph4,updDelay: integer;
    cl1,cl2,cl3,cl4: TRGBColor;
    AClr1,AClr2,AClr3,AClr4: array[0..9] of integer;
    pickLED, SelLED: TGLCustomSceneObject;
    oClickTime: TTime;
    LedIsSetup: boolean;
    MyDocDir: string;
  public
    Sph1,Sph2,Sph3,Sph4: integer;
  end;

const
  plugin_ver = '1.0.0.7';

var
  Form1: TForm1;
  SetForm: TForm2;

implementation

{$R *.dfm}

procedure TForm1.Button1Click(Sender: TObject);
begin
  SetForm.Show;
end;

//переключение управл€ющего софта Ћайтпак на работу с плагином
procedure TForm1.Button2Click(Sender: TObject);
begin
  TCPClient1.Open;
  TCPClient1.Connect;
  TCPClient1.Sendln('lock');
end;

procedure TForm1.Button5Click(Sender: TObject);
begin
  SetForm.RxSpinEdit10.Value:= RxSpinEdit1.Value;
  SetForm.RxSpinEdit2.Value:= RxSpinEdit2.Value;
  SetForm.RxSpinEdit4.Value:= RxSpinEdit3.Value;
  SetForm.RxSpinEdit3.Value:= RxSpinEdit4.Value;
  SetForm.RxSpinEdit6.Value:= RxSpinEdit5.Value;
  SetForm.RxSpinEdit5.Value:= RxSpinEdit6.Value;
  SetForm.RxSpinEdit7.Value:= RxSpinEdit7.Value;
  SetForm.RxSpinEdit11.Value:= RxSpinEdit8.Value;

  SetForm.RxSpinEdit1.Value:= RxSpinEdit9.Value;

  SetForm.Edit1.Text:= Edit1.Text;
  SetForm.Edit2.Text:= Edit2.Text;
  SetForm.Edit3.Text:= Edit3.Text;
  SetForm.Edit4.Text:= Edit4.Text;
  SetForm.Edit5.Text:= Edit5.Text;
  SetForm.Edit6.Text:= Edit6.Text;
  SetForm.Edit7.Text:= Edit7.Text;
  SetForm.Edit8.Text:= Edit8.Text;
  SetForm.Edit9.Text:= Edit9.Text;
  SetForm.Edit10.Text:= Edit10.Text;
  SetForm.Edit11.Text:= Edit11.Text;
  SetForm.Edit12.Text:= Edit12.Text;

  Shape1.Brush.Color:= RGB(StrToInt(Edit1.Text),StrToInt(Edit2.Text),StrToInt(Edit3.Text));
  Shape2.Brush.Color:= RGB(StrToInt(Edit6.Text),StrToInt(Edit5.Text),StrToInt(Edit4.Text));
  Shape3.Brush.Color:= RGB(StrToInt(Edit9.Text),StrToInt(Edit8.Text),StrToInt(Edit7.Text));
  Shape4.Brush.Color:= RGB(StrToInt(Edit12.Text),StrToInt(Edit11.Text),StrToInt(Edit10.Text));

  updDelay:= round(RxSpinEdit11.Value);

  SaveSettings;
  SetForm.SaveSettings;
  SetSettings;
  UpdateLEDS;
end;

//загрузка дефолтных настроек
procedure TForm1.Button6Click(Sender: TObject);
begin
  if MessageBox(0,'«агрузить настройки по умолчанию?','Ћайтпак-DISCO',mb_okcancel or mb_iconquestion) = id_cancel then
    exit;

  RxSpinEdit1.Value:= 0;
  RxSpinEdit2.Value:= 199;
  Edit1.Text:= '255';
  Edit2.Text:= '0';
  Edit3.Text:= '0';

  RxSpinEdit3.Value:= 200;
  RxSpinEdit4.Value:= 799;
  Edit6.Text:= '0';
  Edit5.Text:= '0';
  Edit4.Text:= '255';

  RxSpinEdit5.Value:= 800;
  RxSpinEdit6.Value:= 3499;
  Edit9.Text:= '0';
  Edit8.Text:= '255';
  Edit7.Text:= '0';

  RxSpinEdit7.Value:= 3500;
  RxSpinEdit8.Value:= 20000;
  Edit12.Text:= '255';
  Edit11.Text:= '255';
  Edit10.Text:= '0';

  RxSpinEdit9.Value:= 10;
  RxSpinEdit10.Value:= 4;
  RxSpinEdit11.Value:= 1;

  RLED1.Position.X:= -45;
  RLED1.Position.Y:= -25;

  RLED2.Position.X:= -45;
  RLED2.Position.Y:= 0;

  RLED3.Position.X:= -45;
  RLED3.Position.Y:= 25;

  RLED4.Position.X:= -24;
  RLED4.Position.Y:= 25;

  RLED5.Position.X:= 0;
  RLED5.Position.Y:= 25;

  RLED6.Position.X:= 24;
  RLED6.Position.Y:= 25;

  RLED7.Position.X:= 45;
  RLED7.Position.Y:= 25;

  RLED8.Position.X:= 45;
  RLED8.Position.Y:= 0;

  RLED9.Position.X:= 45;
  RLED9.Position.Y:= -25;

  RLED10.Position.X:= 0;
  RLED10.Position.Y:= -25;


  Shape1.Brush.Color:= RGB(StrToInt(Edit1.Text),StrToInt(Edit2.Text),StrToInt(Edit3.Text));
  Shape2.Brush.Color:= RGB(StrToInt(Edit6.Text),StrToInt(Edit5.Text),StrToInt(Edit4.Text));
  Shape3.Brush.Color:= RGB(StrToInt(Edit9.Text),StrToInt(Edit8.Text),StrToInt(Edit7.Text));
  Shape4.Brush.Color:= RGB(StrToInt(Edit12.Text),StrToInt(Edit11.Text),StrToInt(Edit10.Text));

  SetForm.LoadDefaultSet;

  Button5.Click;
end;

procedure TForm1.FormCreate(Sender: TObject);
begin
  //устнавливаем разделитель времени и дробной части
  TimeSeparator:=':';
  DecimalSeparator:= '.';
end;

procedure TForm1.FormDestroy(Sender: TObject);
begin
  TCPClient1.Sendln('unlock');
  TCPClient1.Close;
  if SetForm <> nil then
    SetForm.Destroy;
end;

procedure TForm1.FormShow(Sender: TObject);
begin
  Caption:= 'Ћайтпак-DISCO ver.'+plugin_ver;
  if SetForm = nil then
    SetForm:= TForm2.Create(self);

  TCPClient1.Sendln('look');

  MyDocDir:= SetForm.GetSpecialPath($0005);

  LoadSettings;
  SetForm.LoadSettings;
  Button5.Click;
  SetSettings;
  UpdateLEDS;

  Button2.Click;
end;

procedure TForm1.GLCadencer1Progress(Sender: TObject; const deltaTime,
  newTime: Double);
begin
  GLSceneViewer1.Invalidate;
end;

procedure TForm1.GLSceneViewer1MouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
var
  ch: TComponent;
begin
  pickLED:= nil;

  //получаем объект по которому было нажатие на виртуальном мониторе
  pickLED:= (GLSceneViewer1.Buffer.GetPickedObject(x,y) as TGLCustomSceneObject);

  if pickLED = nil then
    exit;

  //есди это виртуальный светодиод
  if pickLED.Hint = 'LED' then
    begin
      //если был двойной клик левой кнопкой мыши
      if ((Time - oClickTime) < StrToTime('00:00:00.300')) and (ssLeft in Shift) then
        begin
          //мигаем реальным светодиодом
          if LedIsSetup = false then
            SetupLED(pickLED.Tag);
        end;

      oClickTime:= Time;

      if ssRight in Shift then
        begin
          SelLED:= pickLED;
          PopUpMenuSetColor.Position:= pickLED.Position;
          PopUpMenuSetColor.Visible:= true;
          Timer2.Tag:= 0;

          SetColor1.Position.X:= 0;
          SetColor2.Position.Y:= 0;
          SetColor3.Position.X:= 0;
          SetColor4.Position.Y:= 0;

          GLMaterialLibrary1.Materials.GetLibMaterialByName('BlendMat').Material.FrontProperties.Diffuse.Alpha:= 0;
          GLMaterialLibrary1.Materials.GetLibMaterialByName('BlendMat2').Material.FrontProperties.Diffuse.Alpha:= 0;
          GLMaterialLibrary1.Materials.GetLibMaterialByName('BlendMat3').Material.FrontProperties.Diffuse.Alpha:= 0;
          GLMaterialLibrary1.Materials.GetLibMaterialByName('BlendMat4').Material.FrontProperties.Diffuse.Alpha:= 0;
          Timer2.Enabled:= true;
        end;

      //отключаем работу светодиода (при вызове меню выбора цвета светодиода и клике левой кнопкой мыши по этому светодиоду)
      if (SelLED <> nil) and (ssLeft in Shift) and (SelLED = pickLED) then
        begin
          ch:= SetForm.FindComponent('CheckBox' + IntToStr((1 * 5) + (SelLED.Tag - 1)));
          (ch as TCheckBox).Checked:= false;
          ch:= SetForm.FindComponent('CheckBox' + IntToStr((3 * 5) + (SelLED.Tag - 1)));
          (ch as TCheckBox).Checked:= false;
          ch:= SetForm.FindComponent('CheckBox' + IntToStr((5 * 5) + (SelLED.Tag - 1)));
          (ch as TCheckBox).Checked:= false;
          ch:= SetForm.FindComponent('CheckBox' + IntToStr((7 * 5) + (SelLED.Tag - 1)));
          (ch as TCheckBox).Checked:= false;

          SelLED.Material.FrontProperties.Diffuse.Color:= VectorMake(0.1,0.1,0.1,1);

          AClr1[SelLED.Tag - 1]:= 2;
          AClr2[SelLED.Tag - 1]:= 2;
          AClr3[SelLED.Tag - 1]:= 2;
          AClr4[SelLED.Tag - 1]:= 2;

          SelLED:= nil;
          Timer3.Enabled:= true;
        end;
    end
  else //если был выбран какой либо цвет дл€ светодиода, устанавливаем этот цвет выбраному светодиода
    if pickLED.Hint = 'SetColor' then
      begin
        if SelLED <> nil then
          begin
            SelLED.Material.FrontProperties.Diffuse:= GLMaterialLibrary1.Materials.GetLibMaterialByName(pickLED.Material.LibMaterialName).Material.FrontProperties.Diffuse;
            SelLED.Material.FrontProperties.Diffuse.Alpha:= 1;

            ch:= SetForm.FindComponent('CheckBox' + IntToStr((1 * 5) + (SelLED.Tag - 1)));
            (ch as TCheckBox).Checked:= false;
            ch:= SetForm.FindComponent('CheckBox' + IntToStr((3 * 5) + (SelLED.Tag - 1)));
            (ch as TCheckBox).Checked:= false;
            ch:= SetForm.FindComponent('CheckBox' + IntToStr((5 * 5) + (SelLED.Tag - 1)));
            (ch as TCheckBox).Checked:= false;
            ch:= SetForm.FindComponent('CheckBox' + IntToStr((7 * 5) + (SelLED.Tag - 1)));
            (ch as TCheckBox).Checked:= false;

            ch:= SetForm.FindComponent('CheckBox' + IntToStr((pickLED.Tag * 5) + (SelLED.Tag - 1)));
            (ch as TCheckBox).Checked:= true;
          end;
        SelLED:= nil;
        Timer3.Enabled:= true;
      end
    else
      begin
        SelLED:= nil;
        Timer3.Enabled:= true;
      end;
end;

procedure TForm1.GLSceneViewer1MouseMove(Sender: TObject; Shift: TShiftState; X,
  Y: Integer);
var
  cv: TVector;
  p: TPoint;
begin
  if (not (ssLeft in Shift)) and (not (ssRight in Shift)) then
    pickLED:= (GLSceneViewer1.Buffer.GetPickedObject(x,y) as TGLCustomSceneObject);

  if pickLED <> nil then
    begin
      if pickLED.Hint = 'LED' then
        begin //подсвечиваем виртуальный светодиод при наведении на него курсора
          GLTorus1.Position:= pickLED.Position;
          GLTorus1.Visible:= true;
        end
      else
        begin
          GLTorus1.Visible:= false;
        end;

      //перемещаем виртуальный светодиод левой кнопкой мыши
      if (pickLED.Hint = 'LED') and (ssLeft in Shift) and (SelLED = nil) then
        begin
          GLTorus1.Visible:= false;
          p.X:= x;
          p.Y:= y;
          p.y:=GLSceneViewer1.Height-p.y;

          GLSceneViewer1.Buffer.ScreenVectorIntersectWithPlaneXY(vectormake(p.x, p.y, 0), 0, cv);
          //перемещение разрешено только в пределах виртуального монитора
          if (cv[0] < 50) and (cv[0] > -50) and (cv[1] < 30) and (cv[1] > -30) then
            pickLED.Position.SetPoint(cv);
        end;
    end;
end;

procedure TForm1.GLSceneViewer1MouseUp(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
  pickLED:= nil;
  SetForm.SaveSettings;
  UpdateLEDS;
end;

procedure TForm1.LoadSettings;
var
  inif: TINIFile;
begin
  inif:= TINIFile.Create(MyDocDir + '\ldisco.ini');

  RxSpinEdit1.Value:= inif.ReadInteger('lCol','min',0);
  RxSpinEdit2.Value:= inif.ReadInteger('lCol','max',199);
  Edit1.Text:= inif.ReadString('lCol','r','255');
  Edit2.Text:= inif.ReadString('lCol','g','0');
  Edit3.Text:= inif.ReadString('lCol','b','0');

  RxSpinEdit3.Value:= inif.ReadInteger('lmCol','min',200);
  RxSpinEdit4.Value:= inif.ReadInteger('lmCol','max',799);
  Edit6.Text:= inif.ReadString('lmCol','r','0');
  Edit5.Text:= inif.ReadString('lmCol','g','0');
  Edit4.Text:= inif.ReadString('lmCol','b','255');

  RxSpinEdit5.Value:= inif.ReadInteger('mCol','min',800);
  RxSpinEdit6.Value:= inif.ReadInteger('mCol','max',3499);
  Edit9.Text:= inif.ReadString('mCol','r','0');
  Edit8.Text:= inif.ReadString('mCol','g','255');
  Edit7.Text:= inif.ReadString('mCol','b','0');

  RxSpinEdit7.Value:= inif.ReadInteger('hCol','min',3500);
  RxSpinEdit8.Value:= inif.ReadInteger('hCol','max',20000);
  Edit12.Text:= inif.ReadString('hCol','r','255');
  Edit11.Text:= inif.ReadString('hCol','g','255');
  Edit10.Text:= inif.ReadString('hCol','b','0');

  RxSpinEdit9.Value:= inif.ReadFloat('main','fade',10);
  RxSpinEdit10.Value:= inif.ReadFloat('main','setsmooth',4);
  RxSpinEdit11.Value:= inif.ReadFloat('main','upddelay',1);

  RLED1.Position.X:= inif.ReadFloat('LED1','x',-45);
  RLED1.Position.Y:= inif.ReadFloat('LED1','y',-25);

  RLED2.Position.X:= inif.ReadFloat('LED2','x',-45);
  RLED2.Position.Y:= inif.ReadFloat('LED2','y',0);

  RLED3.Position.X:= inif.ReadFloat('LED3','x',-45);
  RLED3.Position.Y:= inif.ReadFloat('LED3','y',25);

  RLED4.Position.X:= inif.ReadFloat('LED4','x',-24);
  RLED4.Position.Y:= inif.ReadFloat('LED4','y',25);

  RLED5.Position.X:= inif.ReadFloat('LED5','x',0);
  RLED5.Position.Y:= inif.ReadFloat('LED5','y',25);

  RLED6.Position.X:= inif.ReadFloat('LED6','x',24);
  RLED6.Position.Y:= inif.ReadFloat('LED6','y',25);

  RLED7.Position.X:= inif.ReadFloat('LED7','x',45);
  RLED7.Position.Y:= inif.ReadFloat('LED7','y',25);

  RLED8.Position.X:= inif.ReadFloat('LED8','x',45);
  RLED8.Position.Y:= inif.ReadFloat('LED8','y',0);

  RLED9.Position.X:= inif.ReadFloat('LED9','x',45);
  RLED9.Position.Y:= inif.ReadFloat('LED9','y',-25);

  RLED10.Position.X:= inif.ReadFloat('LED10','x',0);
  RLED10.Position.Y:= inif.ReadFloat('LED10','y',-25);

  inif.Free;

  Shape1.Brush.Color:= RGB(StrToInt(Edit1.Text),StrToInt(Edit2.Text),StrToInt(Edit3.Text));
  Shape2.Brush.Color:= RGB(StrToInt(Edit6.Text),StrToInt(Edit5.Text),StrToInt(Edit4.Text));
  Shape3.Brush.Color:= RGB(StrToInt(Edit9.Text),StrToInt(Edit8.Text),StrToInt(Edit7.Text));
  Shape4.Brush.Color:= RGB(StrToInt(Edit12.Text),StrToInt(Edit11.Text),StrToInt(Edit10.Text));
end;

procedure TForm1.SaveSettings;
var
  inif: TINIFile;
begin
  inif:= TINIFile.Create(MyDocDir + '\ldisco.ini');

  inif.WriteFloat('lCol','min',RxSpinEdit1.Value);
  inif.WriteFloat('lCol','max',RxSpinEdit2.Value);
  inif.WriteString('lCol','r',Edit1.Text);
  inif.WriteString('lCol','g',Edit2.Text);
  inif.WriteString('lCol','b',Edit3.Text);

  inif.WriteFloat('lmCol','min',RxSpinEdit3.Value);
  inif.WriteFloat('lmCol','max',RxSpinEdit4.Value);
  inif.WriteString('lmCol','r',Edit6.Text);
  inif.WriteString('lmCol','g',Edit5.Text);
  inif.WriteString('lmCol','b',Edit4.Text);

  inif.WriteFloat('mCol','min',RxSpinEdit5.Value);
  inif.WriteFloat('mCol','max',RxSpinEdit6.Value);
  inif.WriteString('mCol','r',Edit9.Text);
  inif.WriteString('mCol','g',Edit8.Text);
  inif.WriteString('mCol','b',Edit7.Text);

  inif.WriteFloat('hCol','min',RxSpinEdit7.Value);
  inif.WriteFloat('hCol','max',RxSpinEdit8.Value);
  inif.WriteString('hCol','r',Edit12.Text);
  inif.WriteString('hCol','g',Edit11.Text);
  inif.WriteString('hCol','b',Edit10.Text);

  inif.WriteFloat('main','fade',RxSpinEdit9.Value);
  inif.WriteFloat('main','setsmooth',RxSpinEdit10.Value);
  inif.WriteFloat('main','upddelay',RxSpinEdit11.Value);

  inif.WriteFloat('LED1','x',RLED1.Position.X);
  inif.WriteFloat('LED1','y',RLED1.Position.Y);

  inif.WriteFloat('LED2','x',RLED2.Position.X);
  inif.WriteFloat('LED2','y',RLED2.Position.Y);

  inif.WriteFloat('LED3','x',RLED3.Position.X);
  inif.WriteFloat('LED3','y',RLED3.Position.Y);

  inif.WriteFloat('LED4','x',RLED4.Position.X);
  inif.WriteFloat('LED4','y',RLED4.Position.Y);

  inif.WriteFloat('LED5','x',RLED5.Position.X);
  inif.WriteFloat('LED5','y',RLED5.Position.Y);

  inif.WriteFloat('LED6','x',RLED6.Position.X);
  inif.WriteFloat('LED6','y',RLED6.Position.Y);

  inif.WriteFloat('LED7','x',RLED7.Position.X);
  inif.WriteFloat('LED7','y',RLED7.Position.Y);

  inif.WriteFloat('LED8','x',RLED8.Position.X);
  inif.WriteFloat('LED8','y',RLED8.Position.Y);

  inif.WriteFloat('LED9','x',RLED9.Position.X);
  inif.WriteFloat('LED9','y',RLED9.Position.Y);

  inif.WriteFloat('LED10','x',RLED10.Position.X);
  inif.WriteFloat('LED10','y',RLED10.Position.Y);
  inif.Free;
end;

procedure TForm1.SetSettings;
begin
  GLMaterialLibrary1.Materials.GetLibMaterialByName('BlendMat').Material.FrontProperties.Diffuse.Color:= VectorMake(StrToInt(SetForm.Edit1.Text)/(255),StrToInt(SetForm.Edit2.Text)/(255),StrToInt(SetForm.Edit3.Text)/(255),0);
  GLMaterialLibrary1.Materials.GetLibMaterialByName('BlendMat2').Material.FrontProperties.Diffuse.Color:= VectorMake(StrToInt(SetForm.Edit6.Text)/(255),StrToInt(SetForm.Edit5.Text)/(255),StrToInt(SetForm.Edit4.Text)/(255),0);
  GLMaterialLibrary1.Materials.GetLibMaterialByName('BlendMat3').Material.FrontProperties.Diffuse.Color:= VectorMake(StrToInt(SetForm.Edit9.Text)/(255),StrToInt(SetForm.Edit8.Text)/(255),StrToInt(SetForm.Edit7.Text)/(255),0);
  GLMaterialLibrary1.Materials.GetLibMaterialByName('BlendMat4').Material.FrontProperties.Diffuse.Color:= VectorMake(StrToInt(SetForm.Edit12.Text)/(255),StrToInt(SetForm.Edit11.Text)/(255),StrToInt(SetForm.Edit10.Text)/(255),0);

  TCPClient1.Sendln('setsmooth:'+FloatToStr(RxSpinEdit10.Value));

  Timer1.Enabled:= true;
end;

//"моргаем" реальным светодиодом при двойном клике на виртуальном
procedure TForm1.SetupLED(id: integer);
begin
  LedIsSetup:= true;
  TCPClient1.Sendln('setcolor:'+IntToStr(id)+'-255,255,255');
  sleep(500);
  Application.ProcessMessages;
  TCPClient1.Sendln('setcolor:'+IntToStr(id)+'-0,0,0');
  sleep(500);
  Application.ProcessMessages;

  TCPClient1.Sendln('setcolor:'+IntToStr(id)+'-255,255,255');
  sleep(500);
  Application.ProcessMessages;
  TCPClient1.Sendln('setcolor:'+IntToStr(id)+'-0,0,0');
  sleep(500);
  Application.ProcessMessages;

  TCPClient1.Sendln('setcolor:'+IntToStr(id)+'-255,255,255');
  sleep(500);
  Application.ProcessMessages;
  TCPClient1.Sendln('setcolor:'+IntToStr(id)+'-0,0,0');
  sleep(500);
  Application.ProcessMessages;
  LedIsSetup:= false;
end;

procedure TForm1.Shape1MouseUp(Sender: TObject; Button: TMouseButton;
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

procedure TForm1.Shape2MouseUp(Sender: TObject; Button: TMouseButton;
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

procedure TForm1.Shape3MouseUp(Sender: TObject; Button: TMouseButton;
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

procedure TForm1.Shape4MouseUp(Sender: TObject; Button: TMouseButton;
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

procedure TForm1.Timer1Timer(Sender: TObject);
var
  sr,sg,sb: string;
  sr2,sg2,sb2: string;
  sr3,sg3,sb3: string;
  sr4,sg4,sb4: string;
  i, fade: integer;
  j: Integer;
  sstring: string;
begin
  Timer1.Enabled:= false;
  fade:= Round(SetForm.RxSpinEdit1.Value);
  if fade <= 0 then
    fade := 1;

  //тут идут настройки цвета светодиодов
  if oSph1 > 0 then
    begin
      cl1.r:= StrToInt(SetForm.Edit1.Text);
      cl1.g:= StrToInt(SetForm.Edit2.Text);
      cl1.b:= StrToInt(SetForm.Edit3.Text);

      oSph1:= oSph1-fade;
      if oSph1 < 0 then
        oSph1:= 0;
      cl1.r:= muldiv(cl1.r, oSph1, 100);
      cl1.g:= muldiv(cl1.g, oSph1, 100);
      cl1.b:= muldiv(cl1.b, oSph1, 100);
    end;
  if Sph1 > oSph1 then
    begin
      cl1.r:= StrToInt(SetForm.Edit1.Text);
      cl1.g:= StrToInt(SetForm.Edit2.Text);
      cl1.b:= StrToInt(SetForm.Edit3.Text);

      cl1.r:= muldiv(cl1.r, Sph1, 100);
      cl1.g:= muldiv(cl1.g, Sph1, 100);
      cl1.b:= muldiv(cl1.b, Sph1, 100);
      oSph1:= Sph1;
    end;

  if oSph2 > 0 then
    begin
      cl2.r:= StrToInt(SetForm.Edit6.Text);
      cl2.g:= StrToInt(SetForm.Edit5.Text);
      cl2.b:= StrToInt(SetForm.Edit4.Text);

      oSph2:= oSph2-fade;
      if oSph2 < 0 then
        oSph2:= 0;
      cl2.r:= muldiv(cl2.r, oSph2, 100);
      cl2.g:= muldiv(cl2.g, oSph2, 100);
      cl2.b:= muldiv(cl2.b, oSph2, 100);
    end;
  if Sph2 > oSph2 then
    begin
      cl2.r:= StrToInt(SetForm.Edit6.Text);
      cl2.g:= StrToInt(SetForm.Edit5.Text);
      cl2.b:= StrToInt(SetForm.Edit4.Text);

      cl2.r:= muldiv(cl2.r, Sph2, 100);
      cl2.g:= muldiv(cl2.g, Sph2, 100);
      cl2.b:= muldiv(cl2.b, Sph2, 100);
      oSph2:= Sph2;
    end;

  if oSph3 > 0 then
    begin
      cl3.r:= StrToInt(SetForm.Edit9.Text);
      cl3.g:= StrToInt(SetForm.Edit8.Text);
      cl3.b:= StrToInt(SetForm.Edit7.Text);

      oSph3:= oSph3 - fade;
      if oSph3 < 0 then
        oSph3:= 0;
      cl3.r:= muldiv(cl3.r, oSph3, 100);
      cl3.g:= muldiv(cl3.g, oSph3, 100);
      cl3.b:= muldiv(cl3.b, oSph3, 100);
    end;
  if Sph3 > oSph3 then
    begin
      cl3.r:= StrToInt(SetForm.Edit9.Text);
      cl3.g:= StrToInt(SetForm.Edit8.Text);
      cl3.b:= StrToInt(SetForm.Edit7.Text);

      cl3.r:= muldiv(cl3.r, Sph3, 100);
      cl3.g:= muldiv(cl3.g, Sph3, 100);
      cl3.b:= muldiv(cl3.b, Sph3, 100);
      oSph3:= Sph3;
    end;

  if oSph4 > 0 then
    begin
      cl4.r:= StrToInt(SetForm.Edit12.Text);
      cl4.g:= StrToInt(SetForm.Edit11.Text);
      cl4.b:= StrToInt(SetForm.Edit10.Text);

      oSph4:= oSph4 - fade;
      if oSph4 < 0 then
        oSph4:= 0;
      cl4.r:= muldiv(cl4.r, oSph4, 100);
      cl4.g:= muldiv(cl4.g, oSph4, 100);
      cl4.b:= muldiv(cl4.b, oSph4, 100);
    end;
  if Sph4 > oSph4 then
    begin
      cl4.r:= StrToInt(SetForm.Edit12.Text);
      cl4.g:= StrToInt(SetForm.Edit11.Text);
      cl4.b:= StrToInt(SetForm.Edit10.Text);

      cl4.r:= muldiv(cl4.r, Sph4, 100);
      cl4.g:= muldiv(cl4.g, Sph4, 100);
      cl4.b:= muldiv(cl4.b, Sph4, 100);
      oSph4:= Sph4;
    end;

  sr:= IntToStr(cl1.r);
  sg:= IntToStr(cl1.g);
  sb:= IntToStr(cl1.b);

  sr2:= IntToStr(cl2.r);
  sg2:= IntToStr(cl2.g);
  sb2:= IntToStr(cl2.b);

  sr3:= IntToStr(cl3.r);
  sg3:= IntToStr(cl3.g);
  sb3:= IntToStr(cl3.b);

  sr4:= IntToStr(cl4.r);
  sg4:= IntToStr(cl4.g);
  sb4:= IntToStr(cl4.b);

  sstring:= '';

  //а вот тут происходит непосредственно отправка пакетов в Ћайтпак и отрисовка виртуальных светодиодов
  for I := 0 to 9 do  // проходимс€ циклом по всем светодиодам
    begin
    //**************************************************************************
      if AClr1[i] = 1 then  //AClr1 массив дл€ низких частот, где пор€дковый номер €чейки номер светодиода. «начение €чеки - 0 светодиод выключен; 1 - включен
        if (oSph1 > 0) or (Sph1 > oSph1) then
          begin
            sstring:= sstring + IntToStr(i+1)+'-'+sr+','+sg+','+sb+';';

            //ќтрисовка виртуальных светодиодов
            GLMaterialLibrary1.Materials.GetLibMaterialByName('MatLed'+IntToStr(i+1)).Material.FrontProperties.Diffuse.Red:= cl1.r / (255 * 10);
            GLMaterialLibrary1.Materials.GetLibMaterialByName('MatLed'+IntToStr(i+1)).Material.FrontProperties.Diffuse.Green:= cl1.g/ (255 * 10);
            GLMaterialLibrary1.Materials.GetLibMaterialByName('MatLed'+IntToStr(i+1)).Material.FrontProperties.Diffuse.Blue:= cl1.b/ (255 * 10);
          end;

      //если светодиод был только выключен в настройках плагина, задаем ему черный цвет, что бы он не оставалс€ гореть последним установленным цветом
      if AClr1[i] = 2 then
          begin
            TCPClient1.Sendln('setcolor:'+IntToStr(i+1)+'-0,0,0;');

            GLMaterialLibrary1.Materials.GetLibMaterialByName('MatLed'+IntToStr(i+1)).Material.FrontProperties.Diffuse.Red:= 0;
            GLMaterialLibrary1.Materials.GetLibMaterialByName('MatLed'+IntToStr(i+1)).Material.FrontProperties.Diffuse.Green:= 0;
            GLMaterialLibrary1.Materials.GetLibMaterialByName('MatLed'+IntToStr(i+1)).Material.FrontProperties.Diffuse.Blue:= 0;
            AClr1[i]:= 0;
          end;
      //************************************************************************
      if AClr2[i] = 1 then
        if (oSph2 > 0) or (Sph2 > oSph2) then
          begin
            sstring:= sstring + IntToStr(i+1)+'-'+sr2+','+sg2+','+sb2+';';

            GLMaterialLibrary1.Materials.GetLibMaterialByName('MatLed'+IntToStr(i+1)).Material.FrontProperties.Diffuse.Red:= cl2.r / (255 * 10);
            GLMaterialLibrary1.Materials.GetLibMaterialByName('MatLed'+IntToStr(i+1)).Material.FrontProperties.Diffuse.Green:= cl2.g / (255 * 10);
            GLMaterialLibrary1.Materials.GetLibMaterialByName('MatLed'+IntToStr(i+1)).Material.FrontProperties.Diffuse.Blue:= cl2.b / (255 * 10);
          end;

      if AClr2[i] = 2 then
          begin
            TCPClient1.Sendln('setcolor:'+IntToStr(i+1)+'-0,0,0;');

            GLMaterialLibrary1.Materials.GetLibMaterialByName('MatLed'+IntToStr(i+1)).Material.FrontProperties.Diffuse.Red:= 0;
            GLMaterialLibrary1.Materials.GetLibMaterialByName('MatLed'+IntToStr(i+1)).Material.FrontProperties.Diffuse.Green:= 0;
            GLMaterialLibrary1.Materials.GetLibMaterialByName('MatLed'+IntToStr(i+1)).Material.FrontProperties.Diffuse.Blue:= 0;
            AClr2[i]:= 0;
          end;
      //************************************************************************
      if AClr3[i] = 1 then
        if (oSph3 > 0) or (Sph3 > oSph3) then
          begin
            sstring:= sstring + IntToStr(i+1)+'-'+sr3+','+sg3+','+sb3+';';

            GLMaterialLibrary1.Materials.GetLibMaterialByName('MatLed'+IntToStr(i+1)).Material.FrontProperties.Diffuse.Red:= cl3.r / (255 * 10);
            GLMaterialLibrary1.Materials.GetLibMaterialByName('MatLed'+IntToStr(i+1)).Material.FrontProperties.Diffuse.Green:= cl3.g / (255 * 10);
            GLMaterialLibrary1.Materials.GetLibMaterialByName('MatLed'+IntToStr(i+1)).Material.FrontProperties.Diffuse.Blue:= cl3.b / (255 * 10);
          end;

      if AClr3[i] = 2 then
          begin
            TCPClient1.Sendln('setcolor:'+IntToStr(i+1)+'-0,0,0;');

            GLMaterialLibrary1.Materials.GetLibMaterialByName('MatLed'+IntToStr(i+1)).Material.FrontProperties.Diffuse.Red:= 0;
            GLMaterialLibrary1.Materials.GetLibMaterialByName('MatLed'+IntToStr(i+1)).Material.FrontProperties.Diffuse.Green:= 0;
            GLMaterialLibrary1.Materials.GetLibMaterialByName('MatLed'+IntToStr(i+1)).Material.FrontProperties.Diffuse.Blue:= 0;
            AClr3[i]:= 0;
          end;
      //************************************************************************
      if AClr4[i] = 1 then
        if (oSph4 > 0) or (Sph4 > oSph4) then
          begin
            sstring:= sstring + IntToStr(i+1)+'-'+sr4+','+sg4+','+sb4+';';

            GLMaterialLibrary1.Materials.GetLibMaterialByName('MatLed'+IntToStr(i+1)).Material.FrontProperties.Diffuse.Red:= cl4.r / (255 * 10);
            GLMaterialLibrary1.Materials.GetLibMaterialByName('MatLed'+IntToStr(i+1)).Material.FrontProperties.Diffuse.Green:= cl4.g / (255 * 10);
            GLMaterialLibrary1.Materials.GetLibMaterialByName('MatLed'+IntToStr(i+1)).Material.FrontProperties.Diffuse.Blue:= cl4.b / (255 * 10);
          end;

      if AClr4[i] = 2 then
          begin
            TCPClient1.Sendln('setcolor:'+IntToStr(i+1)+'-0,0,0;');

            GLMaterialLibrary1.Materials.GetLibMaterialByName('MatLed'+IntToStr(i+1)).Material.FrontProperties.Diffuse.Red:= 0;
            GLMaterialLibrary1.Materials.GetLibMaterialByName('MatLed'+IntToStr(i+1)).Material.FrontProperties.Diffuse.Green:= 0;
            GLMaterialLibrary1.Materials.GetLibMaterialByName('MatLed'+IntToStr(i+1)).Material.FrontProperties.Diffuse.Blue:= 0;
            AClr4[i]:= 0;
          end;

    end;

  if sstring <> '' then
    begin
      TCPClient1.Sendln('setcolor:' + sstring);
      sleep(updDelay);
    end;

  GLLightSource2.Diffuse.Red := ((cl1.r + cl2.r + cl3.r + cl4.r) div 8);
  GLLightSource2.Diffuse.Green:= ((cl1.g + cl2.g + cl3.g + cl4.g) div 8);
  GLLightSource2.Diffuse.Blue:= ((cl1.b + cl2.b + cl3.b + cl4.b) div 8);

  GLLightSource4.Diffuse.Red := ((cl1.r + cl2.r + cl3.r + cl4.r) div 8);
  GLLightSource4.Diffuse.Green:= ((cl1.g + cl2.g + cl3.g + cl4.g) div 8);
  GLLightSource4.Diffuse.Blue:= ((cl1.b + cl2.b + cl3.b + cl4.b) div 8);

  GLCadencer1.Progress;
  Timer1.Enabled:= true;
end;

//анимаци€ открыти€ меню выбора цвета светодиода
procedure TForm1.Timer2Timer(Sender: TObject);
begin
  SetColor1.Position.X:= SetColor1.Position.X - 1;
  SetColor2.Position.Y:= SetColor2.Position.Y + 1;
  SetColor3.Position.X:= SetColor3.Position.X + 1;
  SetColor4.Position.Y:= SetColor4.Position.Y - 1;

  GLMaterialLibrary1.Materials.GetLibMaterialByName('BlendMat').Material.FrontProperties.Diffuse.Alpha:= GLMaterialLibrary1.Materials.GetLibMaterialByName('BlendMat').Material.FrontProperties.Diffuse.Alpha + 0.1;
  GLMaterialLibrary1.Materials.GetLibMaterialByName('BlendMat2').Material.FrontProperties.Diffuse.Alpha:= GLMaterialLibrary1.Materials.GetLibMaterialByName('BlendMat2').Material.FrontProperties.Diffuse.Alpha + 0.1;
  GLMaterialLibrary1.Materials.GetLibMaterialByName('BlendMat3').Material.FrontProperties.Diffuse.Alpha:= GLMaterialLibrary1.Materials.GetLibMaterialByName('BlendMat3').Material.FrontProperties.Diffuse.Alpha + 0.1;
  GLMaterialLibrary1.Materials.GetLibMaterialByName('BlendMat4').Material.FrontProperties.Diffuse.Alpha:= GLMaterialLibrary1.Materials.GetLibMaterialByName('BlendMat4').Material.FrontProperties.Diffuse.Alpha + 0.1;

  if HintText1.ModulateColor.Alpha >= 1 then
    HintText1.ModulateColor.Alpha:= 0.9;

  HintText1.ModulateColor.Alpha:= HintText1.ModulateColor.Alpha + 0.1;
  HintText2.ModulateColor.Alpha:= HintText1.ModulateColor.Alpha;
  HintText3.ModulateColor.Alpha:= HintText1.ModulateColor.Alpha;
  HintText4.ModulateColor.Alpha:= HintText1.ModulateColor.Alpha;

  Timer2.Tag:= Timer2.Tag + 1;
  if Timer2.Tag >= 8 then
    begin
      Timer2.Enabled:= false;
      Timer2.Tag:= 0;
    end;
end;

//анимаци€ закрыти€ меню выбора цвета светодиода
procedure TForm1.Timer3Timer(Sender: TObject);
begin
  SetColor1.Position.X:= SetColor1.Position.X + 1;
  SetColor2.Position.Y:= SetColor2.Position.Y - 1;
  SetColor3.Position.X:= SetColor3.Position.X - 1;
  SetColor4.Position.Y:= SetColor4.Position.Y + 1;

  GLMaterialLibrary1.Materials.GetLibMaterialByName('BlendMat').Material.FrontProperties.Diffuse.Alpha:= GLMaterialLibrary1.Materials.GetLibMaterialByName('BlendMat').Material.FrontProperties.Diffuse.Alpha - 0.1;
  GLMaterialLibrary1.Materials.GetLibMaterialByName('BlendMat2').Material.FrontProperties.Diffuse.Alpha:= GLMaterialLibrary1.Materials.GetLibMaterialByName('BlendMat2').Material.FrontProperties.Diffuse.Alpha - 0.1;
  GLMaterialLibrary1.Materials.GetLibMaterialByName('BlendMat3').Material.FrontProperties.Diffuse.Alpha:= GLMaterialLibrary1.Materials.GetLibMaterialByName('BlendMat3').Material.FrontProperties.Diffuse.Alpha - 0.1;
  GLMaterialLibrary1.Materials.GetLibMaterialByName('BlendMat4').Material.FrontProperties.Diffuse.Alpha:= GLMaterialLibrary1.Materials.GetLibMaterialByName('BlendMat4').Material.FrontProperties.Diffuse.Alpha - 0.1;

  if HintText1.ModulateColor.Alpha <= 0.3 then
    HintText1.ModulateColor.Alpha:= 0.3;

  HintText1.ModulateColor.Alpha:= HintText1.ModulateColor.Alpha - 0.1;
  HintText2.ModulateColor.Alpha:= HintText1.ModulateColor.Alpha;
  HintText3.ModulateColor.Alpha:= HintText1.ModulateColor.Alpha;
  HintText4.ModulateColor.Alpha:= HintText1.ModulateColor.Alpha;

  Timer3.Tag:= Timer3.Tag + 1;
  if Timer3.Tag >= 8 then
    begin
      Timer3.Enabled:= false;
      Timer3.Tag:= 0;
      PopUpMenuSetColor.Visible:= false;
    end;
end;

procedure TForm1.UpdateLEDS;
var
  i,j: integer;
  ch: TComponent;
begin
  for I := 1 to 10 do
    begin
      (GLScene1.FindSceneObject('RLed'+IntToStr(i)) as TGLSphere).Material.FrontProperties.Diffuse.Color:= VectorMake(0.1,0.1,0.1,1);
    end;

  for I := 14 downto 5 do
    begin
      ch:= SetForm.FindComponent('CheckBox'+IntToStr(i));
      if ch <> nil then
        begin
          if (ch as TCheckBox).Checked then
            begin
              AClr1[(i - 4) - 1]:= 1;
              (GLScene1.FindSceneObject('RLed'+IntToStr(i - 4)) as TGLSphere).Material.FrontProperties.Diffuse:= GLMaterialLibrary1.Materials.GetLibMaterialByName('BlendMat').Material.FrontProperties.Diffuse;
              (GLScene1.FindSceneObject('RLed'+IntToStr(i - 4)) as TGLSphere).Material.FrontProperties.Diffuse.Alpha:= 1;
            end
          else
            begin
              AClr1[(i - 4) - 1]:= 2;
            end;
        end;
    end;

  for I := 24 downto 15 do
    begin
      ch:= SetForm.FindComponent('CheckBox'+IntToStr(i));
      if ch <> nil then
        begin
          if (ch as TCheckBox).Checked then
            begin
              AClr2[(i - 14) - 1]:= 1;
              (GLScene1.FindSceneObject('RLed'+IntToStr(i - 14)) as TGLSphere).Material.FrontProperties.Diffuse:= GLMaterialLibrary1.Materials.GetLibMaterialByName('BlendMat2').Material.FrontProperties.Diffuse;
              (GLScene1.FindSceneObject('RLed'+IntToStr(i - 14)) as TGLSphere).Material.FrontProperties.Diffuse.Alpha:= 1;
            end
          else
            begin
              AClr2[(i - 14) - 1]:= 2;
            end;
        end;
    end;

  for I := 34 downto 25 do
    begin
      ch:= SetForm.FindComponent('CheckBox'+IntToStr(i));
      if ch <> nil then
        begin
          if (ch as TCheckBox).Checked then
            begin
              AClr3[(i - 24) - 1]:= 1;
              (GLScene1.FindSceneObject('RLed'+IntToStr(i - 24)) as TGLSphere).Material.FrontProperties.Diffuse:= GLMaterialLibrary1.Materials.GetLibMaterialByName('BlendMat3').Material.FrontProperties.Diffuse;
              (GLScene1.FindSceneObject('RLed'+IntToStr(i - 24)) as TGLSphere).Material.FrontProperties.Diffuse.Alpha:= 1;
            end
          else
            begin
              AClr3[(i - 24) - 1]:= 2;
            end;
        end;
    end;

  for I := 44 downto 35 do
    begin
      ch:= SetForm.FindComponent('CheckBox'+IntToStr(i));
      if ch <> nil then
        begin
          if (ch as TCheckBox).Checked then
            begin
              AClr4[(i - 34) - 1]:= 1;
              (GLScene1.FindSceneObject('RLed'+IntToStr(i - 34)) as TGLSphere).Material.FrontProperties.Diffuse:= GLMaterialLibrary1.Materials.GetLibMaterialByName('BlendMat4').Material.FrontProperties.Diffuse;
              (GLScene1.FindSceneObject('RLed'+IntToStr(i - 34)) as TGLSphere).Material.FrontProperties.Diffuse.Alpha:= 1;
            end
          else
            begin
              AClr4[(i - 34) - 1]:= 2;
            end;
        end;
    end;
end;

end.
