unit Main;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, DBCtrls, StdCtrls, Mask, ComCtrls, ExtCtrls, DB, DBClient;

type
  TForm1 = class(TForm)
    Panel1: TPanel;
    StatusBar1: TStatusBar;
    PageControl1: TPageControl;
    TabSheet2: TTabSheet;
    Label3: TLabel;
    Edit1: TEdit;
    Button1: TButton;
    Label4: TLabel;
    TabSheet1: TTabSheet;
    Label1: TLabel;
    DBMemo1: TDBMemo;
    Label2: TLabel;
    DBNavigator1: TDBNavigator;
    cds: TClientDataSet;
    DataSource1: TDataSource;
    DBEdit1: TDBEdit;
    cdsCliente: TStringField;
    cdsBancos: TStringField;
    DBLookupComboBox1: TDBLookupComboBox;
    ProgressBar1: TProgressBar;
    StaticText1: TStaticText;
    procedure FormShow(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form1: TForm1;

implementation

{$R *.dfm}

procedure TForm1.FormShow(Sender: TObject);
begin
  try
     cds.CreateDataSet;
     if FileExists('CBDeploy.cds') then
        cds.LoadFromFile('CBDeploy.cds');
  except
  end;
  cds.Open;
  DBLookupComboBox1.KeyValue := cds.FieldByName('Cliente').AsString;
end;

procedure TForm1.FormDestroy(Sender: TObject);
begin
  try
     cds.MergeChangeLog;
     cds.SaveToFile('CBDeploy.cds');
  except
  end;
end;

end.
