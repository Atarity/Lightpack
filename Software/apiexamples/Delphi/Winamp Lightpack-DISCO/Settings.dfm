object Form2: TForm2
  Left = 0
  Top = 0
  BorderStyle = bsToolWindow
  Caption = #1053#1072#1089#1090#1088#1086#1081#1082#1080' '#1051#1072#1081#1090#1087#1072#1082'-DISCO'
  ClientHeight = 356
  ClientWidth = 656
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Label6: TLabel
    Left = 145
    Top = 50
    Width = 12
    Height = 13
    Caption = #1043#1094
  end
  object Label7: TLabel
    Left = 69
    Top = 111
    Width = 4
    Height = 13
    Caption = '-'
  end
  object Label8: TLabel
    Left = 145
    Top = 111
    Width = 12
    Height = 13
    Caption = #1043#1094
  end
  object Label9: TLabel
    Left = 70
    Top = 173
    Width = 4
    Height = 13
    Caption = '-'
  end
  object Label10: TLabel
    Left = 145
    Top = 173
    Width = 12
    Height = 13
    Caption = #1043#1094
  end
  object Label12: TLabel
    Left = 145
    Top = 239
    Width = 12
    Height = 13
    Caption = #1043#1094
  end
  object Label1: TLabel
    Left = 248
    Top = 8
    Width = 83
    Height = 13
    Caption = 'R           G          B'
  end
  object Shape1: TShape
    Left = 176
    Top = 47
    Width = 37
    Height = 21
    OnMouseUp = Shape1MouseUp
  end
  object Shape2: TShape
    Left = 176
    Top = 108
    Width = 37
    Height = 21
    OnMouseUp = Shape2MouseUp
  end
  object Shape3: TShape
    Left = 176
    Top = 170
    Width = 37
    Height = 21
    OnMouseUp = Shape3MouseUp
  end
  object Shape4: TShape
    Left = 176
    Top = 234
    Width = 37
    Height = 21
    OnMouseUp = Shape4MouseUp
  end
  object Label2: TLabel
    Left = 20
    Top = 281
    Width = 108
    Height = 13
    Caption = #1057#1082#1086#1088#1086#1089#1090#1100' '#1079#1072#1090#1091#1093#1072#1085#1080#1103':'
  end
  object Label13: TLabel
    Left = 15
    Top = 28
    Width = 86
    Height = 13
    Caption = #1053#1080#1079#1082#1080#1077' '#1095#1072#1089#1090#1086#1090#1099':'
  end
  object Label14: TLabel
    Left = 15
    Top = 89
    Width = 127
    Height = 13
    Caption = #1057#1088#1077#1076#1085#1077'-'#1085#1080#1079#1082#1080#1077' '#1095#1072#1089#1090#1086#1090#1099':'
  end
  object Label15: TLabel
    Left = 15
    Top = 151
    Width = 94
    Height = 13
    Caption = #1057#1088#1077#1076#1085#1080#1077' '#1095#1072#1089#1090#1086#1090#1099':'
  end
  object Label16: TLabel
    Left = 15
    Top = 220
    Width = 93
    Height = 13
    Caption = #1042#1099#1089#1086#1082#1080#1077' '#1095#1072#1089#1090#1086#1090#1099':'
  end
  object RxSpinEdit2: TRxSpinEdit
    Left = 90
    Top = 47
    Width = 49
    Height = 21
    Value = 200.000000000000000000
    TabOrder = 0
  end
  object RxSpinEdit3: TRxSpinEdit
    Left = 78
    Top = 108
    Width = 61
    Height = 21
    Value = 800.000000000000000000
    TabOrder = 1
  end
  object RxSpinEdit4: TRxSpinEdit
    Left = 15
    Top = 108
    Width = 49
    Height = 21
    Value = 200.000000000000000000
    TabOrder = 2
  end
  object RxSpinEdit5: TRxSpinEdit
    Left = 78
    Top = 170
    Width = 61
    Height = 21
    Value = 3500.000000000000000000
    TabOrder = 3
  end
  object RxSpinEdit6: TRxSpinEdit
    Left = 15
    Top = 170
    Width = 49
    Height = 21
    Value = 800.000000000000000000
    TabOrder = 4
  end
  object RxSpinEdit7: TRxSpinEdit
    Left = 15
    Top = 239
    Width = 49
    Height = 21
    Value = 3500.000000000000000000
    TabOrder = 5
  end
  object Button1: TButton
    Left = 232
    Top = 320
    Width = 75
    Height = 25
    Caption = #1054#1050
    TabOrder = 6
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 336
    Top = 320
    Width = 75
    Height = 25
    Caption = #1055#1088#1080#1084#1077#1085#1080#1090#1100
    TabOrder = 7
    OnClick = Button2Click
  end
  object Edit1: TEdit
    Left = 232
    Top = 47
    Width = 33
    Height = 21
    TabOrder = 8
  end
  object Edit2: TEdit
    Left = 271
    Top = 47
    Width = 33
    Height = 21
    TabOrder = 9
  end
  object Edit3: TEdit
    Left = 310
    Top = 47
    Width = 33
    Height = 21
    TabOrder = 10
  end
  object Edit4: TEdit
    Left = 310
    Top = 108
    Width = 33
    Height = 21
    TabOrder = 11
  end
  object Edit5: TEdit
    Left = 271
    Top = 108
    Width = 33
    Height = 21
    TabOrder = 12
  end
  object Edit6: TEdit
    Left = 232
    Top = 108
    Width = 33
    Height = 21
    TabOrder = 13
  end
  object Edit7: TEdit
    Left = 310
    Top = 170
    Width = 33
    Height = 21
    TabOrder = 14
  end
  object Edit8: TEdit
    Left = 271
    Top = 170
    Width = 33
    Height = 21
    TabOrder = 15
  end
  object Edit9: TEdit
    Left = 232
    Top = 170
    Width = 33
    Height = 21
    TabOrder = 16
  end
  object Edit10: TEdit
    Left = 310
    Top = 234
    Width = 33
    Height = 21
    TabOrder = 17
  end
  object Edit11: TEdit
    Left = 271
    Top = 234
    Width = 33
    Height = 21
    TabOrder = 18
  end
  object Edit12: TEdit
    Left = 232
    Top = 234
    Width = 33
    Height = 21
    TabOrder = 19
  end
  object GroupBox1: TGroupBox
    Left = 356
    Top = 35
    Width = 281
    Height = 38
    Caption = #1057#1074#1077#1090#1086#1076#1080#1086#1076#1099
    TabOrder = 20
    object CheckBox5: TCheckBox
      Left = 8
      Top = 16
      Width = 29
      Height = 17
      Caption = '1'
      Checked = True
      State = cbChecked
      TabOrder = 0
    end
    object CheckBox6: TCheckBox
      Left = 37
      Top = 16
      Width = 29
      Height = 17
      Caption = '2'
      Checked = True
      State = cbChecked
      TabOrder = 1
    end
    object CheckBox7: TCheckBox
      Left = 64
      Top = 16
      Width = 29
      Height = 17
      Caption = '3'
      Checked = True
      State = cbChecked
      TabOrder = 2
    end
    object CheckBox8: TCheckBox
      Left = 89
      Top = 16
      Width = 29
      Height = 17
      Caption = '4'
      Checked = True
      State = cbChecked
      TabOrder = 3
    end
    object CheckBox9: TCheckBox
      Left = 116
      Top = 16
      Width = 29
      Height = 17
      Caption = '5'
      Checked = True
      State = cbChecked
      TabOrder = 4
    end
    object CheckBox10: TCheckBox
      Left = 140
      Top = 16
      Width = 29
      Height = 17
      Caption = '6'
      Checked = True
      State = cbChecked
      TabOrder = 5
    end
    object CheckBox11: TCheckBox
      Left = 165
      Top = 16
      Width = 29
      Height = 17
      Caption = '7'
      Checked = True
      State = cbChecked
      TabOrder = 6
    end
    object CheckBox12: TCheckBox
      Left = 192
      Top = 16
      Width = 29
      Height = 17
      Caption = '8'
      Checked = True
      State = cbChecked
      TabOrder = 7
    end
    object CheckBox13: TCheckBox
      Left = 220
      Top = 16
      Width = 29
      Height = 17
      Caption = '9'
      Checked = True
      State = cbChecked
      TabOrder = 8
    end
    object CheckBox14: TCheckBox
      Left = 249
      Top = 16
      Width = 29
      Height = 17
      Caption = '10'
      Checked = True
      State = cbChecked
      TabOrder = 9
    end
  end
  object GroupBox2: TGroupBox
    Left = 356
    Top = 98
    Width = 281
    Height = 38
    Caption = #1057#1074#1077#1090#1086#1076#1080#1086#1076#1099
    TabOrder = 21
    object CheckBox15: TCheckBox
      Left = 8
      Top = 16
      Width = 29
      Height = 17
      Caption = '1'
      Checked = True
      State = cbChecked
      TabOrder = 0
    end
    object CheckBox16: TCheckBox
      Left = 37
      Top = 16
      Width = 29
      Height = 17
      Caption = '2'
      Checked = True
      State = cbChecked
      TabOrder = 1
    end
    object CheckBox17: TCheckBox
      Left = 64
      Top = 16
      Width = 29
      Height = 17
      Caption = '3'
      Checked = True
      State = cbChecked
      TabOrder = 2
    end
    object CheckBox18: TCheckBox
      Left = 89
      Top = 16
      Width = 29
      Height = 17
      Caption = '4'
      Checked = True
      State = cbChecked
      TabOrder = 3
    end
    object CheckBox19: TCheckBox
      Left = 116
      Top = 16
      Width = 29
      Height = 17
      Caption = '5'
      Checked = True
      State = cbChecked
      TabOrder = 4
    end
    object CheckBox20: TCheckBox
      Left = 140
      Top = 16
      Width = 29
      Height = 17
      Caption = '6'
      Checked = True
      State = cbChecked
      TabOrder = 5
    end
    object CheckBox21: TCheckBox
      Left = 165
      Top = 16
      Width = 29
      Height = 17
      Caption = '7'
      Checked = True
      State = cbChecked
      TabOrder = 6
    end
    object CheckBox22: TCheckBox
      Left = 192
      Top = 16
      Width = 29
      Height = 17
      Caption = '8'
      Checked = True
      State = cbChecked
      TabOrder = 7
    end
    object CheckBox23: TCheckBox
      Left = 220
      Top = 16
      Width = 29
      Height = 17
      Caption = '9'
      Checked = True
      State = cbChecked
      TabOrder = 8
    end
    object CheckBox24: TCheckBox
      Left = 249
      Top = 16
      Width = 29
      Height = 17
      Caption = '10'
      Checked = True
      State = cbChecked
      TabOrder = 9
    end
  end
  object GroupBox3: TGroupBox
    Left = 356
    Top = 159
    Width = 281
    Height = 38
    Caption = #1057#1074#1077#1090#1086#1076#1080#1086#1076#1099
    TabOrder = 22
    object CheckBox25: TCheckBox
      Left = 8
      Top = 16
      Width = 29
      Height = 17
      Caption = '1'
      Checked = True
      State = cbChecked
      TabOrder = 0
    end
    object CheckBox26: TCheckBox
      Left = 37
      Top = 16
      Width = 29
      Height = 17
      Caption = '2'
      Checked = True
      State = cbChecked
      TabOrder = 1
    end
    object CheckBox27: TCheckBox
      Left = 64
      Top = 16
      Width = 29
      Height = 17
      Caption = '3'
      Checked = True
      State = cbChecked
      TabOrder = 2
    end
    object CheckBox28: TCheckBox
      Left = 89
      Top = 16
      Width = 29
      Height = 17
      Caption = '4'
      Checked = True
      State = cbChecked
      TabOrder = 3
    end
    object CheckBox29: TCheckBox
      Left = 116
      Top = 16
      Width = 29
      Height = 17
      Caption = '5'
      Checked = True
      State = cbChecked
      TabOrder = 4
    end
    object CheckBox30: TCheckBox
      Left = 140
      Top = 16
      Width = 29
      Height = 17
      Caption = '6'
      Checked = True
      State = cbChecked
      TabOrder = 5
    end
    object CheckBox31: TCheckBox
      Left = 165
      Top = 16
      Width = 29
      Height = 17
      Caption = '7'
      Checked = True
      State = cbChecked
      TabOrder = 6
    end
    object CheckBox32: TCheckBox
      Left = 192
      Top = 16
      Width = 29
      Height = 17
      Caption = '8'
      Checked = True
      State = cbChecked
      TabOrder = 7
    end
    object CheckBox33: TCheckBox
      Left = 220
      Top = 16
      Width = 29
      Height = 17
      Caption = '9'
      Checked = True
      State = cbChecked
      TabOrder = 8
    end
    object CheckBox34: TCheckBox
      Left = 249
      Top = 16
      Width = 29
      Height = 17
      Caption = '10'
      Checked = True
      State = cbChecked
      TabOrder = 9
    end
  end
  object GroupBox4: TGroupBox
    Left = 356
    Top = 222
    Width = 281
    Height = 38
    Caption = #1057#1074#1077#1090#1086#1076#1080#1086#1076#1099
    TabOrder = 23
    object CheckBox35: TCheckBox
      Left = 8
      Top = 16
      Width = 29
      Height = 17
      Caption = '1'
      Checked = True
      State = cbChecked
      TabOrder = 0
    end
    object CheckBox36: TCheckBox
      Left = 37
      Top = 16
      Width = 29
      Height = 17
      Caption = '2'
      Checked = True
      State = cbChecked
      TabOrder = 1
    end
    object CheckBox37: TCheckBox
      Left = 64
      Top = 16
      Width = 29
      Height = 17
      Caption = '3'
      Checked = True
      State = cbChecked
      TabOrder = 2
    end
    object CheckBox38: TCheckBox
      Left = 89
      Top = 16
      Width = 29
      Height = 17
      Caption = '4'
      Checked = True
      State = cbChecked
      TabOrder = 3
    end
    object CheckBox39: TCheckBox
      Left = 116
      Top = 16
      Width = 29
      Height = 17
      Caption = '5'
      Checked = True
      State = cbChecked
      TabOrder = 4
    end
    object CheckBox40: TCheckBox
      Left = 140
      Top = 16
      Width = 29
      Height = 17
      Caption = '6'
      Checked = True
      State = cbChecked
      TabOrder = 5
    end
    object CheckBox41: TCheckBox
      Left = 165
      Top = 16
      Width = 29
      Height = 17
      Caption = '7'
      Checked = True
      State = cbChecked
      TabOrder = 6
    end
    object CheckBox42: TCheckBox
      Left = 192
      Top = 16
      Width = 29
      Height = 17
      Caption = '8'
      Checked = True
      State = cbChecked
      TabOrder = 7
    end
    object CheckBox43: TCheckBox
      Left = 220
      Top = 16
      Width = 29
      Height = 17
      Caption = '9'
      Checked = True
      State = cbChecked
      TabOrder = 8
    end
    object CheckBox44: TCheckBox
      Left = 249
      Top = 16
      Width = 29
      Height = 17
      Caption = '10'
      Checked = True
      State = cbChecked
      TabOrder = 9
    end
  end
  object RxSpinEdit1: TRxSpinEdit
    Left = 140
    Top = 278
    Width = 49
    Height = 21
    MaxValue = 20.000000000000000000
    MinValue = 1.000000000000000000
    Value = 10.000000000000000000
    TabOrder = 24
  end
  object RxSpinEdit10: TRxSpinEdit
    Left = 15
    Top = 47
    Width = 49
    Height = 21
    TabOrder = 25
  end
  object RxSpinEdit11: TRxSpinEdit
    Left = 78
    Top = 239
    Width = 61
    Height = 21
    Value = 3500.000000000000000000
    TabOrder = 26
  end
  object ColorDialog1: TColorDialog
    Left = 388
    Top = 8
  end
end
