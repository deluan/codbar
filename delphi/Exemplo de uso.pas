(*-----------------------------------------------------------------*)
procedure ImprimirBoleto(rec: String);
var
  ini: TIniFile;
  Taxa: Real;
begin
  // Prepara a query
  Ini := TIniFile.Create('C:\WINDOWS\WIN.INI');

  // Inicializa o Objeto Codbar
  CodBar := TCodBar.Create;
  CodBar.Banco := qryReportBoleta.FieldByName('IDBANCO').AsInteger;
  CodBar.Printer := Ini.ReadString('GS_Receber', 'Codbar.Printer', '');
  CodBar.Path := Ini.ReadString('GS_Receber', 'Codbar.Path', '');
  Ini.Free;

  // Preparar Variáveis do CodBar
  CodBar.Clear;
  with qryReportBoleta do begin
       CodBar.Fields[F_Local_De_Pag_1] := FieldByName('LOCAL_PG1').AsString;
       CodBar.Fields[F_Local_De_Pag_2] := FieldByName('LOCAL_PG2').AsString;
       CodBar.Fields[F_Vencimento]     := FormatDateTime('dd/mm/yyyy', FieldByName('DATAVENCIMENTO').AsDateTime);
       CodBar.Fields[F_Cod_Cedente]    := FieldByName('COD_CEDENTE').AsString;
       CodBar.Fields[F_Agencia]        := FieldByName('IDAGENCIA').AsString;
       CodBar.Fields[F_Cedente]        := FieldByName('ERAZAO_SOCIAL').AsString;
       CodBar.Fields[F_CGC_Cedente]    := FieldByName('ECGC').AsString;
       CodBar.Fields[F_Data_Doc]       := FormatDateTime('dd/mm/yyyy', FieldByName('DATAEMISSAO').AsDateTime);
       CodBar.Fields[F_Numero_Doc]     := FieldByName('NUMDUPLICATA').AsString;
       CodBar.Fields[F_Carteira_N]     := FieldByName('CARTEIRA').AsString;
       CodBar.Fields[F_Carteira_S]     := FieldByName('CARTEIRA').AsString;
       CodBar.Fields[F_Nosso_Numero]   := FieldByName('NOSSO_NUMERO').AsString;

       CodBar.Fields[F_Valor_Doc]      := FormatFloat(',##0.00', FieldByName('VALOR').AsFloat);
       CodBar.Fields[F_Instrucao_1]    := FieldByName('NOTASFISCAIS').AsString;
       if Taxa > 0 then
          CodBar.Fields[F_Instrucao_2]    :=
             'Apos o vencimento R$ '+ FormatFloat('##0.00', Taxa) + ' por dia.'
       else
          CodBar.Fields[F_Instrucao_2]    := '';

       CodBar.Fields[F_Instrucao_3]    := FieldByName('INSTR_1').AsString;
       CodBar.Fields[F_Instrucao_4]    := FieldByName('INSTR_2').AsString;
       CodBar.Fields[F_Instrucao_5]    := FieldByName('INSTR_3').AsString;
       CodBar.Fields[F_Instrucao_6]    := FieldByName('INSTR_4').AsString;
       CodBar.Fields[F_Sacado_1]       := FieldByName('RAZAO_SOCIAL').AsString;
       CodBar.Fields[F_Sacado_2]       := FieldByName('ENDERECO').AsString +' - '+
                                          FieldByName('CIDADE').AsString +' - '+
                                          FieldByName('UF').AsString;
       CodBar.Fields[F_Sacado_3]       := 'CEP ' + FieldByName('CEP').AsString +
                                          ' CGC ' + FieldByName('CGC').AsString;;
  end;
  // Emitir Boleto
  CodBar.Print;

  CodBar.Free;
end;
