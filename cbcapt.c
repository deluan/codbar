/*********************************************************************
 *                                                                   *
 * CODBAR - Impressao de Bloquetos de Cobranca com Codigo de Barra   *
 *                                                                   *
 * cbCapt.c  Capturacao dos Dados Originais                          *
 *                                                                   *
 *********************************************************************
 * Este modulo contem o codigo para capturar os dados necessarios p/ *
 * a impressao do novo bloqueto, a partir dos dados do bloqueto      *
 * original.                                                         *
 *                                                                   *
 * As seguintes variaveis e rotinas publicas estao definidas:        *
 *                                                                   *
 * cbCaptFields    - definicoes dos campos de captura                *
 * cbCaptPrnConfig - sequencia de config. inicial da impressora      *
 * cbCaptNewForm   - sequencia que inicia uma nova boleta            *
 * cbCaptFormLen   - num. de bytes que formam uma boleta original    *
 *                                                                   *
 * cbCaptChar()    - captura e processa um caracter.                 *
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

/* Este vetor mantem as definicoes dos campos do bloqueto original que */
/* serao capturados                                                    */

FIELD cbCaptFields[MAXFIELDS] = {
       {"F Local_De_Pag_1 ", 0, 0, NULL},
       {"F Local_De_Pag_2 ", 0, 0, NULL},
       {"F Vencimento     ", 0, 0, NULL},
       {"F Cedente        ", 0, 0, NULL},
       {"F CGC_Cedente    ", 0, 0, NULL},
       {"F Agencia        ", 0, 0, NULL},
       {"F Cod_Cedente    ", 0, 0, NULL},
       {"F Data_Doc       ", 0, 0, NULL},
       {"F Numero_Doc     ", 0, 0, NULL},
       {"F Especie_Doc    ", 0, 0, NULL},
       {"F Aceite         ", 0, 0, NULL},
       {"F Data_Proces    ", 0, 0, NULL},
       {"F Carteira_N     ", 0, 0, NULL},
       {"F Carteira_S     ", 0, 0, NULL},
       {"F Nosso_Numero   ", 0, 0, NULL},
       {"F Uso_Do_Banco   ", 0, 0, NULL},
       {"F Especie_Moeda  ", 0, 0, NULL},
       {"F Qtd_Moeda      ", 0, 0, NULL},
       {"F Valor_Doc      ", 0, 0, NULL},
       {"F Valor_Desconto ", 0, 0, NULL},
       {"F Data_Desconto  ", 0, 0, NULL},
       {"F Comissao_Per   ", 0, 0, NULL},
       {"F Instrucao_1    ", 0, 0, NULL},
       {"F Instrucao_2    ", 0, 0, NULL},
       {"F Instrucao_3    ", 0, 0, NULL},
       {"F Instrucao_4    ", 0, 0, NULL},
       {"F Instrucao_5    ", 0, 0, NULL},
       {"F Instrucao_6    ", 0, 0, NULL},
       {"F Sacado_1       ", 0, 0, NULL},
       {"F Sacado_2       ", 0, 0, NULL},
       {"F Sacado_3       ", 0, 0, NULL},
       {"F Sacador_Aval   ", 0, 0, NULL},
       {"F Unidade_Cedente", 0, 0, NULL},
       {"F Parcela        ", 0, 0, NULL},
       {"F Abatimento     ", 0, 0, NULL},
       {"F Auxiliar       ", 0, 0, NULL},
       {"F Codigo_Baixa   ", 0, 0, NULL}};

char *cbCaptPrnConfig;
char *cbCaptNewForm;
int   cbCaptFormLen;

/* Rotinas exclusivas a este modulo */

static int    CaptFormat OF((void));
static FIELD *FindField  OF((int));


/*--------------------------------------------------------------------*/
int cbCaptChar(c)
int c;

{ static FIELD *f        = NULL;
  static int i = 0, bpos = 0;
  static int p_new       = 0; /* Indice para cbCaptNewForm */
  static int p_config    = 0; /* Indice para cbCaptPrnConfig */
  int        complete    = 0; /* != 0 se completou uma boleta */

  /* Se for inicio de boleta, zera valores antigos */
  if (bpos == 0) {
     for (i = 0; i < (int)MAXFIELDS; i++) {
         f = &cbCaptFields[i];
         if (f->value != NULL)
            memset(f->value, 0, f->pos_f - f->pos_i + 1 + 1);
     }
     complete = i = p_config = p_new = 0;
     f = NULL;
  }

  /* Verifica marca de configuracao inicial. Se encontrar, recomeca */
  if (c == cbCaptPrnConfig[p_config]) {
     if (++p_config == strlen(cbCaptPrnConfig))
        return (bpos = 0);
  }
  else p_config = 0;

  /* Verifica marca de inicio de nova boleta */
  if (c == cbCaptNewForm[p_new])
     p_new++;
  else p_new = 0;

  /* Se terminou boleta, recomeca retornando flag "boleta cheia" */
  if (p_new == strlen(cbCaptNewForm) || c == EOF) {
     if ((complete = (bpos >= cbCaptFormLen)) != 0)
        CaptFormat();
     bpos = 0;
     return complete;
  }

  /* Incrementa offset da boleta */
  bpos++;

  /* Se a capturacao nao estiver ativa ... */
  if (f == NULL) {
     /* Se entrou no dominio de um campo, ativa capturacao */
     if ((f = FindField(bpos)) != NULL)
        i = f->pos_i;
     else /* Retorna imcompleto */
        return 0;
  }

  /* Copia (captura) caracter para valor do campo */
  f->value[bpos-i] = c;

  /* Quando alcancar limite do campo, desativa a capturacao */
  if (bpos == f->pos_f) {
     f = NULL;
     i = 0;
  }

  /* Retorna incompleto */
  return 0;
}

/*--------------------------------------------------------------------*/
static int CaptFormat()

{ char *s;
  int  i;

  for (i = 0; i < (int)MAXFIELDS; i++) {
      s = cbCaptFields[i].value;

      /* Processa os campos nao-nulos */
      if (s != NULL)
         trim(s);
  }

  return OK;
}

/*--------------------------------------------------------------------*/
static FIELD *FindField(pos)
int pos;

{ int i;

  for (i = 0; i < (int)MAXFIELDS; i++) {
      if (pos >= cbCaptFields[i].pos_i && pos <= cbCaptFields[i].pos_f)
         return &(cbCaptFields[i]);
  }

  return NULL;
}
