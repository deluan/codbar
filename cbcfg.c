/*********************************************************************
 *                                                                   *
 * CODBAR - Impressao de Bloquetos de Cobranca com Codigo de Barra   *
 *                                                                   *
 * cbCfg.c  Le Configuracao do Sistema                               *
 *                                                                   *
 *********************************************************************
 * Este modulo processa o arquivo CFGFILE e mantem os dados de       *
 * configuracao de um banco.                                         *
 *                                                                   *
 * As seguintes variaveis e rotinas publicas estao definidas:        *
 *                                                                   *
 * cbCfgBank       - Dados do ultimo banco carregado                 *
 *                                                                   *
 * cbCfgLoadBank() - Carrega dados para um determinado banco         *
 *                                                                   *
 * By: Deluan Perez, General Software, Rio 40 0 Graus                *
 *********************************************************************
 *       (C) Copyright General Software Tecnologia 1994,95           *
 *********************************************************************
 * 07 Abr 1995 - DP primeira versao                                  *
 *********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "winprof.h"
#include "codbar.h"


BANK cbCfgBank = {0, "", "", {{FF_END}}};

static int ReadBank  OF((FILE *, BANK *));

/*--------------------------------------------------------------------*/
int cbCfgLoadBank(bcode, carteira)
int bcode, carteira;

{ FILE      *fp;
  BANK       bco;
  short      found = 0;

  carteira = carteira; /* Warnings... */

  /* Abre arquivo de configuracao */
  if ((fp = fopen(cbIniPath(CFGFILE), "rb")) == NULL)
     return ERR_OPENFILE;

  /* Pula cabecalho */
  fseek(fp, 3 + sizeof(short), 0);

  /* Procura banco especificado */
  while (ReadBank(fp, &bco) && !feof(fp))
        if (bco.code == bcode) {
           found = 1;
           cbCfgBank = bco;
           break;
        }

  fclose(fp);

  return (found) ? 0 : ERR_NOFIELD;
}

/*--------------------------------------------------------------------*/
static int ReadBank(fp, bco)
FILE *fp;
BANK *bco;

{ int len;

  /* Le codigo do banco */
  if (fread(&bco->code, sizeof(bco->code), 1, fp)) {

     /* Le nome do banco */
     len = fgetc(fp);
     fread(&bco->name, len, 1, fp);
     bco->name[len] = 0;

     /* Le campo livre */
     len = fgetc(fp);
     fread(&bco->ff, sizeof(FREEFIELD), len, fp);
  }

  return bco->code;
}
