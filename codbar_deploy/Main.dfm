object Form1: TForm1
  Left = 315
  Top = 275
  BorderStyle = bsDialog
  Caption = 'CodBar Deployment Center'
  ClientHeight = 271
  ClientWidth = 281
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnDestroy = FormDestroy
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 281
    Height = 271
    Align = alClient
    BevelInner = bvLowered
    BevelOuter = bvNone
    TabOrder = 0
    object StatusBar1: TStatusBar
      Left = 1
      Top = 251
      Width = 279
      Height = 19
      Panels = <
        item
          Text = 'General Software'
          Width = 100
        end>
      SimplePanel = False
    end
    object PageControl1: TPageControl
      Left = 1
      Top = 1
      Width = 279
      Height = 250
      ActivePage = TabSheet2
      Align = alClient
      ParentShowHint = False
      ShowHint = True
      TabIndex = 0
      TabOrder = 1
      object TabSheet2: TTabSheet
        Caption = 'Distrib'
        ImageIndex = 1
        object Label3: TLabel
          Left = 8
          Top = 20
          Width = 62
          Height = 13
          Caption = 'Codbar Path:'
        end
        object Label4: TLabel
          Left = 8
          Top = 48
          Width = 35
          Height = 13
          Caption = 'Cliente:'
        end
        object Edit1: TEdit
          Left = 72
          Top = 16
          Width = 169
          Height = 21
          TabOrder = 0
          Text = '..\'
        end
        object Button1: TButton
          Left = 88
          Top = 184
          Width = 75
          Height = 25
          Caption = 'Cria Distrib.'
          TabOrder = 1
        end
        object DBLookupComboBox1: TDBLookupComboBox
          Left = 72
          Top = 44
          Width = 169
          Height = 21
          KeyField = 'Cliente'
          ListField = 'Cliente'
          ListSource = DataSource1
          TabOrder = 2
        end
        object ProgressBar1: TProgressBar
          Left = 4
          Top = 152
          Width = 257
          Height = 17
          Min = 0
          Max = 100
          TabOrder = 3
        end
        object StaticText1: TStaticText
          Left = 4
          Top = 124
          Width = 257
          Height = 17
          AutoSize = False
          BevelKind = bkSoft
          TabOrder = 4
        end
      end
      object TabSheet1: TTabSheet
        Caption = 'Clentes'
        object Label1: TLabel
          Left = 12
          Top = 12
          Width = 35
          Height = 13
          Caption = 'Cliente:'
        end
        object Label2: TLabel
          Left = 12
          Top = 56
          Width = 39
          Height = 13
          Caption = 'Bancos:'
        end
        object DBEdit1: TDBEdit
          Left = 12
          Top = 28
          Width = 241
          Height = 21
          DataField = 'Cliente'
          DataSource = DataSource1
          TabOrder = 0
        end
        object DBMemo1: TDBMemo
          Left = 12
          Top = 72
          Width = 241
          Height = 89
          Hint = 'C'#243'digo dos Bancos, separados por espa'#231'o'
          DataField = 'Bancos'
          DataSource = DataSource1
          TabOrder = 2
        end
        object DBNavigator1: TDBNavigator
          Left = 12
          Top = 180
          Width = 240
          Height = 25
          DataSource = DataSource1
          ParentShowHint = False
          ShowHint = True
          TabOrder = 1
        end
      end
    end
  end
  object cds: TClientDataSet
    Aggregates = <>
    Params = <>
    Left = 100
    Top = 96
    object cdsCliente: TStringField
      FieldName = 'Cliente'
      Size = 100
    end
    object cdsBancos: TStringField
      FieldName = 'Bancos'
      Size = 512
    end
  end
  object DataSource1: TDataSource
    DataSet = cds
    Left = 132
    Top = 96
  end
end
