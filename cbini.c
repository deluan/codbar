/*********************************************************************
 *                                                                   *
 * CODBAR - Impressao de Bloquetos de Cobranca com Codigo de Barra   *
 *                                                                   *
 * cbIni.c  Le Arquivo de Inicializacao Sistema                      *
 *                                                                   *
 *********************************************************************
 * Este modulo processa o arquivo .ini e mantem os dados de          *
 * configuracao do sistema.                                          *
 *                                                                   *
 * As seguintes variaveis e rotinas publicas estao definidas:        *
 *                                                                   *
 * cbIniPrinter   - impressora default                               *
 * cbIniCurrency  - moeda padrao                                     *
 * cbIniDir       - diretorio de dados do sistema                    *
 *                                                                   *
 * cbIniPath()    - monta nome de arquivo de dados                   *
 * cbIniInit()    - processa o arquivo .ini                          *
 *                                                                   *
 * By: Deluan Perez, General Software, Petropolis 0 Graus            *
 *********************************************************************
 *       (C) Copyright General Software Tecnologia 1994              *
 *********************************************************************
 * 26 Jun 1994 - DP primeira versao                                  *
 *********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "winprof.h"
#include "codbar.h"

#ifdef dos
int access(const char *, int);
#endif

char      *cbIniPrinter       = NULL;
int        cbIniCurrency      = 9;
int        cbIniNoFrame       = 0;
char      *cbIniDir           = NULL;

static int ChkBankFiles OF((int));

/*--------------------------------------------------------------------*/
char *cbIniPath(fname)
char *fname;

{ static char aux[64];

  if (cbIniDir != NULL) {
     strcpy(aux, cbIniDir);
     strcat(aux, "/");
  }
  else
     aux[0] = 0;

  return strcat(aux, fname);
}

/*--------------------------------------------------------------------*/
int cbIniInit()

{ int i, siz;
  PROFILE *ini;
  char aux[128], *s;
  char bankcode[5];

  /* Configura diretorio de dados */
  if ((s = getenv(DIR_DADOS)) != NULL)
     cbIniDir = strdup(s);

  /* Verifica se existem os arquivos de configuracao */
  if (access(cbIniPath(CFGFILE), 4)) 
     return ERR_ACCESS;

  /* Abre arquivo.ini */
  if ((ini = dbinit(cbIniPath(INIFILE))) == NULL)
     return ERR_OPENFILE;

  /* Se nao foi especificado codigo do banco, le o default */
  if (!cbCfgBank.code) {
     if (!dbgetstr(ini, "Default", "Codigo_Banco", "", aux, sizeof(aux)))
        return ERR_NOBANK;
     cbCfgBank.code = atoi(strtok(aux, "; "));
  }
  sprintf(bankcode, "%03d", cbCfgBank.code);

  /* Carrega configuracao do banco */
  if (cbCfgLoadBank(cbCfgBank.code, 0) != OK)
     return ERR_NOBANK;

  /* Verifica se existem os arquivos especificos ao banco selecionado */
  if (ChkBankFiles(cbCfgBank.code) != OK)
     return ERR_NOBANK;

  /* Se nao foi especificada nenhuma impressora, le a default */
  if (!cbIniPrinter) {
     if (!dbgetstr(ini, "Default", "Impressora", "", aux, sizeof(aux)))
        return ERR_INIT;
     cbIniPrinter = strdup(strtok(aux, "; "));
  }

  /* Verifica se deve suprimir formularios */
  if (!cbIniNoFrame) {
     if (dbgetstr(ini, "Default", "Suprime_Form", "", aux, sizeof(aux))) {
        trim(aux);
        cbIniNoFrame = (toupper(aux[0]) == 'S');
     }
  }

  /* Le sequencia de configuracao inicial */
  if (!dbgetstr(ini, bankcode, "Config_Inicial", "", aux, sizeof(aux)))
     return ERR_INIT;
  cbCaptPrnConfig = strdup(trim(aux));

  /* Le sequencia de inicio de nova boleta */
  if (!dbgetstr(ini, bankcode, "Nova_Boleta", "", aux, sizeof(aux)))
     return ERR_INIT;
  cbCaptNewForm = strdup(trim(aux));

  /* Se sequencias forem identicas, deixa somente cbCaptNewForm */
  if (!strcmp(cbCaptNewForm, cbCaptPrnConfig))
     cbCaptPrnConfig[0] = 0;

  /* Le no. de bytes que compoe uma boleta completa */
  cbCaptFormLen = dbgetint(ini, bankcode, "Tam_Boleta", 0);
  if (dberror) return ERR_INIT;

  /* Le pos_i e pos_f de cada campo em arquivo.ini */
  for (i = 0; i < (int)MAXFIELDS; i++) {
      cbCaptFields[i].pos_i = cbCaptFields[i].pos_f = 0;
      cbCaptFields[i].value = NULL;
      dbgetstr(ini, bankcode, cbCaptFields[i].name, "", aux, sizeof(aux));
      if (aux[0] != 0) {
         s = strtok(aux, ", ");
         if ((cbCaptFields[i].pos_i = atoi(s)) != 0) {
            s = strtok(NULL, ", ");
            cbCaptFields[i].pos_f = atoi(s);

            /* Aloca memoria para valor */
            siz = cbCaptFields[i].pos_f - cbCaptFields[i].pos_i + 1 + 1;
            if (siz > 2)
               if ((cbCaptFields[i].value = malloc(siz)) == NULL) {
                  dbterm(ini);
                  return ERR_NOMEMORY;
               }
         }
      }
  }

  dbterm(ini);
  return OK;
}

/*--------------------------------------------------------------------*/
static int ChkBankFiles(bcode)
int bcode;

{ 
  /* Monta path completo de CMDFILE */
  sprintf(cbPcodeFile, "%s%03d", B_CMDFILE, bcode);

  if (access(cbIniPath(cbPcodeFile), 4)) return ERR_NOBANK;
/*
  sprintf(fname, "%s%d", B_BANNERFILE, bcode);
  if (access(fname, 4)) return ERR_NOBANK;
*/
  return OK;
}
