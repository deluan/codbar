unit Main;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  Menus, ComCtrls, ExtCtrls, StdCtrls, Db, DBClient, Grids, DBGrids,
  DBCtrls, CodBar;

type
  TForm1 = class(TForm)
    Panel1: TPanel;
    StatusBar1: TStatusBar;
    DBGrid1: TDBGrid;
    cds: TClientDataSet;
    cdsVENCIMENTO: TDateTimeField;
    cdsVALORDOCUMENTO: TFloatField;
    cdsDATAEMISSAO: TDateTimeField;
    cdsNUMERODOCUMENTO: TStringField;
    cdsINSTR_3: TStringField;
    cdsINSTR_4: TStringField;
    cdsSACADO_1: TStringField;
    cdsSACADO_2: TStringField;
    DataSource1: TDataSource;
    DBNavigator1: TDBNavigator;
    Label1: TLabel;
    edtPrinter: TEdit;
    Label2: TLabel;
    edtPath: TEdit;
    cdsSACADO_3: TStringField;
    ComboBox1: TComboBox;
    Label3: TLabel;
    Button1: TButton;
    procedure FormShow(Sender: TObject);
    procedure Imprimir1Click(Sender: TObject);
    procedure Sair2Click(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure FormKeyPress(Sender: TObject; var Key: Char);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form1: TForm1;
  CB: TCodBar;  

implementation

{$R *.DFM}

procedure TForm1.FormShow(Sender: TObject);
begin
  edtPath.Text := ExtractFilePath(ParamStr(0));
  ComboBox1.ItemIndex := 0;
  try
     cds.CreateDataSet;
     if FileExists('Data.cds') then
        cds.LoadFromFile('Data.cds');
  except
  end;
  cds.Open;
end;

(*----------------------------------------------------------------------------*)
procedure TForm1.Imprimir1Click(Sender: TObject);
begin
  // Inicializa o Objeto Codbar
  CB := TCodBar.Create;

  CB.Printer := edtPrinter.Text;
  CB.Path := edtPath.Text;

  CB.Banco := StrToInt(Copy(ComboBox1.Items[ComboBox1.ItemIndex], 1, 3));

  // Preparar Variáveis do CB
  CB.Clear;
  with cds do begin
     CB.Fields[F_Cedente]        := 'Distribuidora Dimarco Ltda.';
     CB.Fields[F_CGC_Cedente]    := '01563986000251';
     CB.Fields[F_especie_moeda]  := 'R$';
     CB.Fields[F_especie_doc]    := 'DM';
     CB.Fields[F_Instrucao_2]    := 'Titulo sujeito a protesto';
     CB.Fields[F_Instrucao_1]    := 'Mora diaria R$ ' +
                                    FormatFloat(',#0.00',(FieldByName('VALORDOCUMENTO').AsFloat * 0.004));

     if cb.banco = 291 then // BCN.
     begin
       CB.Fields[F_uso_do_banco]   := '000 8650';
       CB.Fields[F_aceite]         := 'SEM';
       CB.Fields[F_Local_de_pag_1] := 'pagavel preferencialmente em qualquer agencia bradesco';
       CB.Fields[F_Agencia]        := '0406-5  /';
       CB.Fields[F_Cod_Cedente]    := '0120240-5';
       CB.Fields[F_Nosso_Numero]   := '00010043574-9';
       CB.Fields[F_Carteira_S]     := '19';
       CB.Fields[F_Carteira_N]     := '19';
     end;

     if cb.banco = 001 then // Brasil.
     begin
       CB.Fields[F_uso_do_banco]   := '000 8650';
       CB.Fields[F_aceite]         := 'SEM';
       CB.Fields[F_Local_de_pag_1] := 'pagavel preferencialmente em qualquer agencia bradesco';
       CB.Fields[F_Agencia]        := '0406-5  /';
       CB.Fields[F_Cod_Cedente]    := '0120240-5';
       CB.Fields[F_Nosso_Numero]   := '00010043574-9';
       CB.Fields[F_Carteira_S]     := '19';
       CB.Fields[F_Carteira_N]     := '19';
     end;

     if cb.banco = 237 then // Bradesco.
     begin
       CB.Fields[F_uso_do_banco]   := '000 8650';
       CB.Fields[F_aceite]         := 'SEM';
       CB.Fields[F_Local_de_pag_1] := 'pagavel preferencialmente em qualquer agencia bradesco';
       CB.Fields[F_Agencia]        := '0406-5  /';
       CB.Fields[F_Cod_Cedente]    := '0120240-5';
       CB.Fields[F_Nosso_Numero]   := '00010043574-9';
       CB.Fields[F_Carteira_S]     := '19';
       CB.Fields[F_Carteira_N]     := '19';
     end;

     if cb.banco = 453 then // Rural.
     begin
       CB.Fields[F_uso_do_banco] := '9100007489';
       CB.Fields[F_aceite]  := 'S';
       CB.Fields[F_Local_de_pag_1] := 'pagavel em qualquer banco ate o vencimento,';
       CB.Fields[F_Local_de_pag_2] := 'apos so nas agencias banco rural';
       CB.Fields[F_Agencia]        := '043';
       CB.Fields[F_Cod_Cedente]    := '06.0000721-6';
       CB.Fields[F_Nosso_Numero]   := '0031266-3';
       CB.Fields[F_Carteira_S]     := 'CV';
     end;

     if cb.banco = 320 then // Bic.
     begin
       CB.Fields[F_aceite]  := 'NAO';
       CB.Fields[F_Local_de_pag_1] := 'pagavel em qualquer banco ate o vencimento,';
       CB.Fields[F_Local_de_pag_2] := 'apos so nas agencias bicbanco';
       CB.Fields[F_Agencia]        := '040';
       CB.Fields[F_Cod_Cedente]    := '44097000-7';
       CB.Fields[F_Nosso_Numero]   := '028904-1';
       CB.Fields[F_Carteira_S]     := 'CC';
     end;

     if cb.banco = 422 then // Safra.
     begin
       CB.Fields[F_aceite]  := 'NAO';
       CB.Fields[F_Local_de_pag_1] := 'pagavel em qualquer banco ate o vencimento,';
       CB.Fields[F_Local_de_pag_2] := 'apos so nas agencias banco safra';
       CB.Fields[F_Agencia]        := '00600';
       CB.Fields[F_Cod_Cedente]    := '00301045-3';
       CB.Fields[F_Nosso_Numero]   := '36485197-0';
       CB.Fields[F_Carteira_S]     := '2';
     end;

     CB.Fields[F_Vencimento]     := FormatDateTime('dd/mm/yyyy', FieldByName('VENCIMENTO').AsDateTime);
     CB.Fields[F_Data_Doc]       := FormatDateTime('dd/mm/yyyy', FieldByName('DATAEMISSAO').AsDateTime);
     CB.Fields[F_Numero_Doc]     := FieldByName('NUMERODOCUMENTO').AsString;
     CB.Fields[F_Data_Proces]    := FormatDateTime('dd/mm/yyyy',FieldByName('DATAEMISSAO').AsDateTime);
     CB.Fields[F_Valor_Doc]      := FormatFloat(',##0.00', FieldByName('VALORDOCUMENTO').AsFloat);
     CB.Fields[F_Instrucao_3]    := FieldByName('INSTR_3').AsString;
     CB.Fields[F_Instrucao_4]    := FieldByName('INSTR_4').AsString;
     CB.Fields[F_Sacado_1]       := FieldByName('SACADO_1').AsString;
     CB.Fields[F_Sacado_2]       := FieldByName('SACADO_2').AsString;
     CB.Fields[F_Sacado_3]       := FieldByName('SACADO_3').AsString;
  end;

  // Emitir Boleto
  CB.Print;

  CB.Free;
end;

procedure TForm1.Sair2Click(Sender: TObject);
begin
  Application.Terminate;
end;

procedure TForm1.FormDestroy(Sender: TObject);
begin
  try
     cds.MergeChangeLog;
     cds.SaveToFile('Data.cds');
  except
  end;
end;

procedure TForm1.FormKeyPress(Sender: TObject; var Key: Char);
begin
     if Key = #27 then
        Close;
end;

end.
