/*********************************************************************
 *                                                                   *
 * CODBAR - Impressao de Bloquetos de Cobranca com Codigo de Barra   *
 *                                                                   *
 * cbCalc.c  Calculos e Tabelas                                      *
 *                                                                   *
 *********************************************************************
 * Este modulo contem tabelas de uso do sistema e tambem a montagem  *
 * da linha digitavel e do codigo de barra.                          *
 *                                                                   *
 * As seguintes variaveis e rotinas publicas estao definidas:        *
 *                                                                   *
 * cbCalcInit()     - monta CalcData, baseado nos dados capturados   *
 * cbCalcNumeric()  - monta a linha digitavel                        *
 * cbCalcBarCode()  - monta o codigo de barras                       *
 * cbCalcCurrency() - devolve o codigo da moeda                      *
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

#include <time.h>

static char   CalcData[45];
static float  DocValue = 0;

static int    AddFreeField    OF((char *));
static int    _calcDV11       OF((char *, int, int));
static int    CalcDV11        OF((char *));
static int    CalcDV10        OF((char *));
static int    CalcASBACE      OF((char *));
static float  StrNum          OF((char *));
static char  *CodeFilter      OF((char *));
static int    CalcJul4        OF((char *));
static int    Doy             OF((int, int, int));
static int    Date2Int        OF((int, int, int));
static int    CalcFatorVencto OF((char *));

/*--------------------------------------------------------------------*/
int cbCalcInit()

{ char *s, *t, c;

/*
strcpy(f_value(F_Nosso_Numero), "000000000000019");
strcpy(f_value(F_Auxiliar),     "058753-2");
strcpy(f_value(F_Cod_Cedente),  "2527-5");
strcpy(f_value(F_Valor_Doc),    "400,0");
*/

  s = CalcData;
  memset(s, 0, sizeof(CalcData));

  /* Copia Codigo do Banco */
  sprintf(s, "%03d", cbCfgBank.code);

  /* Copia Codigo da Moeda */
  s[3] = (c = cbCalcCurrency(f_value(F_Especie_Moeda))) + 48;

  /* Se moeda for variavel (!= moeda corrente), valor = 0 */
  DocValue = (c == cbIniCurrency)
              ? StrNum(f_value(F_Valor_Doc))
              : 0;

  /* Copia o fator de vencimento */
  sprintf(s + strlen(s), "%04d", CalcFatorVencto(f_value(F_Vencimento)));

  /* Copia o valor do documento */
  sprintf(s + strlen(s), "%010.0f", DocValue*100);

  /* Copia o Campo Livre, conforme formatacao de cbIniFreeField */
  AddFreeField(s);

  /* Calcula o DAC modulo 11. Se este for '0', passar para '1'  */
  c = CalcDV11(s);
  if (c == '0')
     c = '1';

  /* Insere o DAC */
  t = strdup(s);
  memcpy(&s[5], &t[4], strlen(&t[4]));
  free(t);
  s[4] = c;

  return OK;
}

/*--------------------------------------------------------------------*/
char *cbCalcNumeric()

{ static char buf[60];
  char aux[15];

  memset(buf, 0, sizeof(buf));
  memset(aux, 0, sizeof(aux));

  /* Campo 1 */
  strncpy(aux, CalcData, 4); strncat(aux, &CalcData[19], 5);
  strncpy(buf, aux, 5);
  sprintf(buf + strlen(buf), ".%s%c  ", &aux[5], CalcDV10(aux));

  /* Campo 2 */
  memset(aux, 0, sizeof(aux)); strncpy(aux, &CalcData[19+5], 10);
  strncat(buf, aux, 5);
  sprintf(buf + strlen(buf), ".%s%c  ", &aux[5], CalcDV10(aux));

  /* Campo 3 */
  memset(aux, 0, sizeof(aux)); strncpy(aux, &CalcData[19+15], 10);
  strncat(buf, aux, 5);
  sprintf(buf + strlen(buf), ".%s%c  ", &aux[5], CalcDV10(aux));

  /* Campo 4 */
  sprintf(buf + strlen(buf), "%c  ", CalcData[4]);

  /* Campo 5 */
  strncat(buf, &CalcData[5], 14);

  return buf;
}

/*--------------------------------------------------------------------*/
char *cbCalcBarCode()

{
  /* Atencao: cbI25Code() aloca memoria para colocar o resultado. Esta */
  /* area precisa ser desalocada quando nao estiver mais em uso!       */
  return cbI25Code(CalcData);
}

/*--------------------------------------------------------------------*/
int cbCalcCurrency(s)
char *s;

{ /* int i;

  for (i = 0; cbIniCurrencyTable[i].id != NULL; i++)
     if (!strcmp(s, cbIniCurrencyTable[i].id))
        return cbIniCurrencyTable[i].code;

  return cbIniCurrency;
*/
  s = s; /* Warnings.... */
  return 9; /* So' pode ser REAL agora ... */
}

/*--------------------------------------------------------------------*/
static int AddFreeField(s)
char *s;

{ char aux[15], *p = NULL, *t;
  FREEFIELD *ff;

  for (ff = cbCfgBank.ff; ff->id != FF_END; ff++) {

      /* Copia a variavel para 'p' */
      switch (ff->id) {
             case F_Uso_Do_Banco:
             case F_Agencia:
             case F_Carteira_N:
             case F_Nosso_Numero:
             case F_Cod_Cedente:
             case F_Numero_Doc:
             case F_Vencimento:
             case F_Auxiliar:
                  p = strdup(f_value(ff->id));
                  break;
             case FF_DV10:
             case FF_DV11:
             case FF_DD_ASBACE:
                  p = malloc(ff->len+2);
                  strncpy(p, &(s[ff->ofs-1-1]), ff->len);
                  p[ff->len] = 0;
                  break;
             case FF_FIXED:
                  break;
             default:
                  return ERR_NOFIELD;
      }

      /* Formata 'p' em 'aux' */
      switch (ff->id) {
             case FF_FIXED:
                  sprintf(aux, "%0*ld", ff->len, ff->fixed);
                  break;
             case FF_DV10:
                  sprintf(aux, "%c", CalcDV10(p));
                  break;
             case FF_DV11:
                  sprintf(aux, "%c", CalcDV11(p));
                  break;
             case FF_DD_ASBACE:
                  sprintf(aux, "%02d", CalcASBACE(p));
                  break;
             default:
                  if (ff->fixed == FF_JUL4)
                     sprintf(aux, "%04d", CalcJul4(p));
                  else {
                     if (p != NULL) {
                        t = CodeFilter(p+ff->ofs);
                        if (ff->fixed < 0) {
                           t[strlen(t)+ff->fixed] = 0;
                        }
                        sprintf(aux, "%*s", ff->len, t);
                     }
                     else
                        aux[0] = 0;
                  }
      }

      /* Se usou 'p', libera espaco */
      if (p != NULL)
         free(p);

      /* Troca espacos de formatacao em 'aux' por '0's */
      for (p = aux; *p; p++)
          if (*p == ' ')
             *p = '0';
      p = NULL;

      /* Copia valor formatado para 's' */
      strncat(s, aux, ff->len);
  }

  return OK;
}

/*--------------------------------------------------------------------*/
static int CalcJul4(s)
char *s;

{ char *p, *t, i;
  int  day, mon, year;

  /* Filtra somente os digitos */
  t = malloc(strlen(s) + 1);
  for (p = s, i = 0; *p; p++) {
      if (isdigit(*p))
         t[i++] = *p;
  }

  /* Pega dia, mes, ano */
  sscanf(t, "%2d%2d%2d", &day, &mon, &year);

  /* Coloca no formato ddda */
  day = Doy(day, mon, year) * 10;
  day += t[5] - 48;
  free(t);

  return day;
}

/*--------------------------------------------------------------------*/
static int Doy(d, m, y)
int d, m, y;

{ static int dim[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  int ret, i;

  if ((m == 2) && (y % 4 == 0) || (y == 0)) dim[1]++;

  ret = d;
  for (i = 0; i < m-1; i++)
      ret += dim[i];

  if ((m == 2) && (y % 4 == 0) || (y == 0)) dim[1]--;

  return ret + 1; /* OBS: NÆo sei porque o do +1, ... */
}

/*--------------------------------------------------------------------*/
static char *CodeFilter(s)
char *s;

{ static char aux[20];
  char *t, *p;
  int   i;

  t = aux;
  for (p = s, i = 0; *p; p++) {
      if (isalnum(*p))
         t[i++] = *p;
  }

  t[i] = 0;

  return t;
}

/*--------------------------------------------------------------------*/
static int _calcDV11(s, vmin, vmax)
char *s;
int vmin, vmax;

{ int   d, res;
  char *p;

  res = 0;
  d   = vmin;

  /* Posiciona p no ultimo caracter de s */
  p = strchr(s, 0); p--;

  while (p >= s) {
        res += (*p-48)*d;
        if (++d > vmax)
           d = vmin;
        p--;
  }

  return res;
}

/*--------------------------------------------------------------------*/
static int CalcASBACE(s)
char *s;

{ int d1, d2;
  char *s2;

  d1 = CalcDV10(s) - 48;

  s2 = malloc(strlen(s)+4);
  sprintf(s2, "%s%d", s, d1);

  do {
     sprintf(s2, "%s%d", s, d1);
     d2 = _calcDV11(s2, 2, 7) % 11;
     if (d2 == 1) {
        d1 = (d1 + 1) % 10;
        d2 = -1;
     }
  } while (d2 < 0);

  free(s2);

  if (d2)
     d2 = 11 - d2;
  else
     d2 = 0;

  return d1 * 10 + d2;
}

/*--------------------------------------------------------------------*/
static int CalcDV11(s)
char *s;

{ int res;

  res = _calcDV11(s, 2, 9);

  res = (res % 11);
  res = 11 - res;
  if (res > 9)
     res = 1;

  return res + 48;
}

/*--------------------------------------------------------------------*/
static int CalcDV10(s)
char *s;

{ int i, d, res, ac;

  i = strlen(s)-1;
  ac = res = 0;

  d = 2;
  while (i >= 0) {
        res = (s[i--]-48) * d;
        if (res >= 10)
           res = 1 + (res - 10);
        ac += res;
        d = 3 - d;
  }

  ac %= 10;
  res = 10 - ac;
  if (res == 10)
     res = 0;

  return res + 48;
}

/*--------------------------------------------------------------------*/
int Date2Int(D, M, A)
int D, M, A;

{
  if (D*M*A == 0)
     return 0;
  else {
     if (M < 3) {
        A--;
        M += 12;
     }
     M++;
     return (int)(365.25*A) + (int)(30.6001*M)+D;
  }
}

/*--------------------------------------------------------------------*/
static int CalcFatorVencto(data)
char *data;

{ int d, m, a;
  sscanf(data, "%d/%d/%d", &d, &m, &a);
  if (a < 100)
     a += 1900;

  return (Date2Int(d, m, a) - DATA_REF);
}

/*--------------------------------------------------------------------*/
static float StrNum(s)
char *s;

{ char *t, *p, *aux;
  int   i;
  float r;

  aux = strdup(s);
  for (t = aux; *t; t++)
      if (!isdigit(*t) && *t != '.' && *t != ',')
         *t = '0';

  t = malloc(strlen(aux)+1);
  for (p = aux, i = 0; *p; p++)
      if (*p != '.') {
         if (*p == ',')
            t[i] = '.';
         else
            t[i] = *p;
         i++;
      }

  t[i] = 0;
  r = atof(t);
  free(t);
  free(aux);
  return r;
}

/*--------------------------------------------------------------------*/
char *trim(s)
char *s;

{ char *t, *p;

  for (t = s; *t == ' ' && *t; t++);
  for (p = t+strlen(t)-1; *p == ' ' && p != t; p--);
  t[p-t+1] = 0;
  strcpy(s, t);
  return(s);
}

