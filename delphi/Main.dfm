object Form1: TForm1
  Left = 300
  Top = 208
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Dimarco - Codbar Avulso'
  ClientHeight = 388
  ClientWidth = 536
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = True
  Position = poScreenCenter
  OnDestroy = FormDestroy
  OnKeyPress = FormKeyPress
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 536
    Height = 388
    Align = alClient
    BevelInner = bvLowered
    BevelOuter = bvNone
    TabOrder = 0
    object Label1: TLabel
      Left = 85
      Top = 8
      Width = 33
      Height = 13
      Caption = 'Printer:'
    end
    object Label2: TLabel
      Left = 149
      Top = 8
      Width = 25
      Height = 13
      Caption = 'Path:'
    end
    object Label3: TLabel
      Left = 10
      Top = 8
      Width = 34
      Height = 13
      Caption = 'Banco:'
    end
    object StatusBar1: TStatusBar
      Left = 1
      Top = 368
      Width = 534
      Height = 19
      Panels = <
        item
          Text = 'General Software'
          Width = 100
        end>
      SimplePanel = False
    end
    object DBGrid1: TDBGrid
      Left = 8
      Top = 56
      Width = 513
      Height = 257
      DataSource = DataSource1
      TabOrder = 1
      TitleFont.Charset = DEFAULT_CHARSET
      TitleFont.Color = clWindowText
      TitleFont.Height = -11
      TitleFont.Name = 'MS Sans Serif'
      TitleFont.Style = []
      Columns = <
        item
          Expanded = False
          FieldName = 'SACADO_1'
          Visible = True
        end
        item
          Expanded = False
          FieldName = 'SACADO_2'
          Visible = True
        end
        item
          Expanded = False
          FieldName = 'SACADO_3'
          Visible = True
        end
        item
          Expanded = False
          FieldName = 'VALORDOCUMENTO'
          Visible = True
        end
        item
          Expanded = False
          FieldName = 'DATAEMISSAO'
          Visible = True
        end
        item
          Expanded = False
          FieldName = 'NUMERODOCUMENTO'
          Visible = True
        end
        item
          Expanded = False
          FieldName = 'VENCIMENTO'
          Visible = True
        end
        item
          Expanded = False
          FieldName = 'INSTR_3'
          Visible = True
        end
        item
          Expanded = False
          FieldName = 'INSTR_4'
          Visible = True
        end>
    end
    object DBNavigator1: TDBNavigator
      Left = 8
      Top = 328
      Width = 240
      Height = 25
      DataSource = DataSource1
      TabOrder = 2
    end
    object edtPrinter: TEdit
      Left = 85
      Top = 22
      Width = 57
      Height = 21
      TabOrder = 3
      Text = 'LPT1'
    end
    object edtPath: TEdit
      Left = 149
      Top = 22
      Width = 286
      Height = 21
      Color = clBtnFace
      ReadOnly = True
      TabOrder = 4
    end
    object ComboBox1: TComboBox
      Left = 9
      Top = 22
      Width = 69
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 5
      Items.Strings = (
        '237 - Bradesco'
        '422 - Safra'
        '320 - BicBanco'
        '453 - Rural'
        '291 - BCN'
        '001 - Brasil')
    end
    object Button1: TButton
      Left = 445
      Top = 20
      Width = 75
      Height = 25
      Caption = '&Imprime'
      Default = True
      TabOrder = 6
      OnClick = Imprimir1Click
    end
  end
  object cds: TClientDataSet
    Aggregates = <>
    Params = <>
    Left = 224
    Top = 112
    object cdsVENCIMENTO: TDateTimeField
      FieldName = 'VENCIMENTO'
    end
    object cdsVALORDOCUMENTO: TFloatField
      FieldName = 'VALORDOCUMENTO'
    end
    object cdsDATAEMISSAO: TDateTimeField
      FieldName = 'DATAEMISSAO'
    end
    object cdsNUMERODOCUMENTO: TStringField
      FieldName = 'NUMERODOCUMENTO'
      Size = 15
    end
    object cdsINSTR_3: TStringField
      FieldName = 'INSTR_3'
      Size = 40
    end
    object cdsINSTR_4: TStringField
      FieldName = 'INSTR_4'
      Size = 40
    end
    object cdsSACADO_1: TStringField
      FieldName = 'SACADO_1'
      Size = 80
    end
    object cdsSACADO_2: TStringField
      FieldName = 'SACADO_2'
      Size = 80
    end
    object cdsSACADO_3: TStringField
      FieldName = 'SACADO_3'
      Size = 40
    end
  end
  object DataSource1: TDataSource
    DataSet = cds
    Left = 256
    Top = 112
  end
end
