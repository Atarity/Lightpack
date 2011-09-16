object Form1: TForm1
  Left = 202
  Top = 123
  Width = 403
  Height = 498
  Caption = 'LightPack SDK Demo'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object Memo1: TMemo
    Left = 8
    Top = 8
    Width = 249
    Height = 409
    TabOrder = 0
  end
  object Button1: TButton
    Left = 264
    Top = 432
    Width = 113
    Height = 17
    Caption = 'Send'
    Enabled = False
    TabOrder = 1
    OnClick = Button1Click
  end
  object Edit1: TEdit
    Left = 8
    Top = 432
    Width = 249
    Height = 21
    TabOrder = 2
  end
  object Button2: TButton
    Left = 264
    Top = 32
    Width = 113
    Height = 25
    Caption = 'Connect'
    TabOrder = 3
    OnClick = Button2Click
  end
  object Button3: TButton
    Left = 264
    Top = 392
    Width = 113
    Height = 25
    Caption = 'Disconnect'
    Enabled = False
    TabOrder = 4
    OnClick = Button3Click
  end
  object Button4: TButton
    Left = 264
    Top = 64
    Width = 113
    Height = 17
    Caption = 'getProfile'
    Enabled = False
    TabOrder = 5
    OnClick = Button4Click
  end
  object Button5: TButton
    Left = 264
    Top = 88
    Width = 113
    Height = 17
    Caption = 'getProfiles'
    Enabled = False
    TabOrder = 6
    OnClick = Button5Click
  end
  object Button6: TButton
    Left = 264
    Top = 112
    Width = 113
    Height = 17
    Caption = 'getStatus'
    Enabled = False
    TabOrder = 7
    OnClick = Button6Click
  end
  object Button7: TButton
    Left = 264
    Top = 136
    Width = 113
    Height = 17
    Caption = 'getAPIStatus'
    Enabled = False
    TabOrder = 8
    OnClick = Button7Click
  end
  object Button8: TButton
    Left = 264
    Top = 160
    Width = 113
    Height = 25
    Caption = 'lock'
    Enabled = False
    TabOrder = 9
    OnClick = Button8Click
  end
  object Button9: TButton
    Left = 264
    Top = 360
    Width = 113
    Height = 25
    Caption = 'unlock'
    Enabled = False
    TabOrder = 10
    OnClick = Button9Click
  end
  object Button10: TButton
    Left = 264
    Top = 192
    Width = 113
    Height = 17
    Caption = 'turnOn'
    Enabled = False
    TabOrder = 11
    OnClick = Button10Click
  end
  object Button11: TButton
    Left = 264
    Top = 216
    Width = 113
    Height = 17
    Caption = 'turnOff'
    Enabled = False
    TabOrder = 12
    OnClick = Button11Click
  end
  object Button12: TButton
    Left = 264
    Top = 240
    Width = 113
    Height = 17
    Caption = 'setProfile'
    Enabled = False
    TabOrder = 13
    OnClick = Button12Click
  end
  object Button13: TButton
    Left = 264
    Top = 264
    Width = 113
    Height = 17
    Caption = 'setColor'
    Enabled = False
    TabOrder = 14
    OnClick = Button13Click
  end
  object Button14: TButton
    Left = 264
    Top = 288
    Width = 113
    Height = 17
    Caption = 'setColorToAll'
    Enabled = False
    TabOrder = 15
    OnClick = Button14Click
  end
  object Button15: TButton
    Left = 264
    Top = 312
    Width = 113
    Height = 17
    Caption = 'setGamma'
    Enabled = False
    TabOrder = 16
    OnClick = Button15Click
  end
  object Button16: TButton
    Left = 264
    Top = 336
    Width = 113
    Height = 17
    Caption = 'setSmooth'
    Enabled = False
    TabOrder = 17
    OnClick = Button16Click
  end
  object CheckBox1: TCheckBox
    Left = 272
    Top = 8
    Width = 97
    Height = 17
    Caption = 'Debug mode'
    Enabled = False
    TabOrder = 18
    OnClick = CheckBox1Click
  end
end
