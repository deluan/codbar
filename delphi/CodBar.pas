unit Codbar;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs;

// Nomeia os campos a serem capturados de uma boleta original.
type
  TCBFields = (F_Local_De_Pag_1, F_Local_De_Pag_2, F_Vencimento, F_Cedente,
               F_CGC_Cedente, F_Agencia, F_Cod_Cedente, F_Data_Doc,
               F_Numero_Doc, F_Especie_Doc, F_Aceite, F_Data_Proces,
               F_Carteira_N, F_Carteira_S, F_Nosso_Numero, F_Uso_Do_Banco,
               F_Especie_Moeda, F_Qtd_Moeda, F_Valor_Doc, F_Valor_Desconto,
               F_Data_Desconto, F_Comissao_Per, F_Instrucao_1, F_Instrucao_2,
               F_Instrucao_3, F_Instrucao_4, F_Instrucao_5, F_Instrucao_6,
               F_Sacado_1, F_Sacado_2, F_Sacado_3, F_Sacador_Aval,
               F_Unidade_Cedente, F_Parcela, F_Abatimento, F_Auxiliar,
               F_Codigo_Baixa);

  TCBFieldArray = array[TCBFields] of String;


type
  TCodBar = class(TObject)
  private
    { Private declarations }
    FPrinter: String;
    FBanco: Word;
    FPath: String;
    FUpcase: Boolean;
    FUnaccent: Boolean;
    FFields: TCBFieldArray;
    ExePath: String;
    IniPath: String;
    DataPath: String;
    function ExecCodBar: Boolean;
    procedure WriteData;
    function GetFields(fld: TCBFields): String;
    procedure SetFields(fld: TCBFields; value: String);
    procedure SetPath(value: String);
  protected
    { Protected declarations }
  public
    { Public declarations }
    constructor Create;
    procedure Clear;
    procedure Print;
    property Fields[fld: TCBFields]: String read GetFields write SetFields; default;
  published
    { Published declarations }
    property Banco: Word read FBanco write FBanco;
    property Printer: String read FPrinter write FPrinter;
    property Path: String read FPath write SetPath;
    property Upcase: Boolean read FUpcase write FUpcase;
    property Unaccent: Boolean read FUnaccent write FUnaccent;
  end;

var CodbarPath: String;

implementation

const EXENAME  = 'codbar.exe';
      PIFNAME  = 'codbar.pif';
      ININAME  = 'codbar.ini';
      TMPNAME  = 'codbar.$$$';
      INIT_CFG = #27#13#27;
      NEW_BLOQ = #27#27;
      LINESIZE = 80;

var Acentos: Array[#128..#255] of Char;

function UnAccent(s: String) : String;
var i : Integer;
begin
     Result := s;
     for i := 1 to Length(Result) do
         if (Result[i] > #128) then
            Result[i] := Acentos[Result[I]];
end;

constructor TCodBar.Create;
begin
     inherited Create;
     FUnaccent := True;
     FUpcase := True;
end;

function TCodBar.GetFields(fld: TCBFields): String;
begin
     Result := FFields[fld];
     if FUnaccent then
        Result := CodBar.UnAccent(Result);
     if FUpcase then
        Result := AnsiUpperCase(Result);
end;

procedure TCodBar.SetFields(fld: TCBFields; value: String);
begin
     FFields[fld] := Copy(value, 1, LINESIZE);
end;

procedure TCodBar.Clear;
var I: TCBFields;
begin
     for I := Low(FFields) to High(FFields) do
         FFields[I] := '';
end;

procedure TCodBar.WriteData;
var F: TextFile;
    I: TCBFields;
begin
     AssignFile(F, DataPath);
     Rewrite(F);
     try
        Write(F, INIT_CFG);
        for I := Low(FFields) to High(FFields) do
            WriteLn(F, Format('%-*s', [LINESIZE, Fields[I]]));
     finally
        CloseFile(F);
     end;
end;

function TCodBar.ExecCodBar: Boolean;
var si: TStartupInfo;
    pi: TProcessInformation;
    Cmd: String;
    Status: DWORD;
    LastDir: String;
    PrnParm: String;
begin
     FillChar(si, SizeOf(si), 0);
     with si do begin
          cb := SizeOf(si);
          dwFlags:= StartF_UseShowWindow;
          if ExtractFileExt(ExePath) = '.exe' then
             wShowWindow := SW_SHOWMINIMIZED
          else
             wShowWindow := SW_SHOWNORMAL;
     end;

     { Para se executar o codbar, é necessário que o diretório corrente seja   }
     { o que está instalado o programa. Antes de trocar, guardamos o diretório }
     { corrente atual.                                                         }
     GetDir(0, LastDir);
     ChDir(FPath);

     if FPrinter = '' then
        PrnParm := ''
     else
        PrnParm := '-p ' + FPrinter;

     { Executa o codbar }
     Cmd := Format('%s %s -b %d %s', [ExePath, PrnParm, FBanco, DataPath]);
     Result := CreateProcess(Nil, PChar(Cmd), Nil, Nil, False,
                             Create_New_Console + Normal_Priority_Class,
                             Nil, Nil, si, pi);

     { Restaura o diretório corrente }
     ChDir(LastDir);

     { Se conseguiu executar o codbar, espera terminar }
     if Result then begin
        repeat
              Application.ProcessMessages;
              Result := GetExitCodeProcess(pi.hProcess, Status);
        until (Result = False) or (Status <> STILL_ACTIVE);
     end;
end;

procedure TCodBar.Print;
begin
     try
        WriteData;
        if not ExecCodBar then
           raise Exception.Create('Módulo CODBAR.EXE não pode ser executado!');
     finally
        DeleteFile(DataPath);
     end;
end;

procedure TCodBar.SetPath(value: String);
var Buf: ShortString;
begin
     FPath := value;
     ExePath := FPath + PIFNAME;
     if not FileExists(ExePath) then
        ExePath := FPath + EXENAME;
     IniPath := FPath + ININAME;
     SetLength(Buf, GetTempPath(SizeOf(Buf), Addr(Buf[1])));
     DataPath := Buf + '\' + TMPNAME;
end;

procedure InitAcentos;
begin
     Fillchar(Acentos, SizeOf(Acentos), '*');
     Acentos['Á'] := 'A';  Acentos['À'] := 'A';  Acentos['Ã'] := 'A';
     Acentos['Â'] := 'A';  Acentos['Ä'] := 'A';
     Acentos['É'] := 'E';  Acentos['È'] := 'E';
     Acentos['Ê'] := 'E';  Acentos['Ë'] := 'E';
     Acentos['Í'] := 'I';  Acentos['Ì'] := 'I';
     Acentos['Î'] := 'I';  Acentos['Ï'] := 'I';
     Acentos['Ó'] := 'O';  Acentos['Ò'] := 'O';  Acentos['Õ'] := 'O';
     Acentos['Ô'] := 'O';  Acentos['Ö'] := 'O';
     Acentos['Ú'] := 'U';  Acentos['Ù'] := 'U';
     Acentos['Û'] := 'U';  Acentos['Ü'] := 'U';
     Acentos['Ç'] := 'C';  Acentos['Ñ'] := 'N';  Acentos['Ÿ'] := 'Y';  

     Acentos['á'] := 'a';  Acentos['à'] := 'a';  Acentos['ã'] := 'a';
     Acentos['â'] := 'a';  Acentos['ä'] := 'a';
     Acentos['é'] := 'e';  Acentos['è'] := 'e';
     Acentos['ê'] := 'e';  Acentos['ë'] := 'e';
     Acentos['í'] := 'i';  Acentos['ì'] := 'i';
     Acentos['î'] := 'i';  Acentos['ï'] := 'i';
     Acentos['ó'] := 'o';  Acentos['ò'] := 'o';  Acentos['õ'] := 'o';
     Acentos['ô'] := 'o';  Acentos['ö'] := 'o';
     Acentos['ú'] := 'u';  Acentos['ù'] := 'u';
     Acentos['û'] := 'u';  Acentos['ü'] := 'u';
     Acentos['ç'] := 'c';  Acentos['ñ'] := 'n';  Acentos['ÿ'] := 'y';
end;

initialization
begin
     InitAcentos;
end;

end.
