/*********************************************************************
 *                                                                   *
 * CODBAR - Impressao de Bloquetos de Cobranca com Codigo de Barra   *
 *                                                                   *
 * makeform.c  Cria arquivo de PCode e formularios                   *
 *                                                                   *
 *********************************************************************
 * Este programa gera o arquivo CMDFILE, que contem os comandos      *
 * para a impressao de um bloqueto.                                  *
 * Obs: Todas as coordenadas devem ser calculadas para papel A4.     *
 *                                                                   *
 * By: Deluan Perez at General Software Tecnologia                   *
 *     Rio 40 Graus, Brasil.                                         *
 *                                                                   *
 *********************************************************************
 *       (C) Copyright General Software Tecnologia 1994,95           *
 *********************************************************************
 * 27 Mar 1995 - DP primeira versao                                  *
 *********************************************************************/

#define MSG_RECEB     /* Inclui a mensagem de recebimento no */
                      /* Recibo do Sacado do Formulario 1    */
#define MSG_RESPONSA  /* Inclui a mensagem de responsabilidade */
                      /* no campo instrucoes                   */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "winprof.h"
#include "codbar.h"
#include "pcl.h"

#include "banks.c"

#ifdef dos
#ifndef gnu
#  include <alloc.h>
#else
#  define coreleft()  32767
#endif
#else /* !dos */
#  define coreleft()  32767
#endif /* ?dos */

#define MYNAME          "MakeForm"
#define myname          "makeform"
#define MYDESCRIPTION   "Gera Formularios"

#define DEFAULT_MODEL   1

#define A_TIL     0xE3
#define A_GRAVE   0xE2
#define A_AGUDO   0xE1
#define E_AGUDO   233
#define E_GRAVE   234
#define I_AGUDO   0xED
#define O_TIL     245
#define O_AGUDO   243
#define U_AGUDO   0xFA
#define CEDILHA   231
#define N_O       0xBA

static int  VLine       OF((int, int, int, int));
static int  HLine       OF((int, int, int, int));
static int  Box         OF((int, int, int, int, int));
static int  PrintFrame  OF((int, int));
static int  PrintFrame2 OF((int, int));
static int  PrintText   OF((int, int));
static int  PrintText2  OF((int, int, int));
static int  FCopy       OF((FILE *, FILE *));
static long FSize       OF((FILE *));
static int  PrintXY     OF((int, int, char *));
static int  GetFontId   OF((char *));
static int  LoadFont    OF((char *));
static int  TrimFile    OF((FILE *, char *, int, int));
static int  Presents    OF((void));
static int  Usage       OF((void));
static int  SaveCmds    OF((char *, short, short));

FILE *cbPrnFile = NULL;

static int   OFS_X = 0, OFS_Y = 0;
static char  outname[100];
static int   cod_bco;

static COMMAND form_1[] = {
       /* Comandos Estaticos */
       { C_LOADMACRO,           1,   0},
       { C_LOADFONT,            0,   0, "ari06n__.sft"},
       { C_LOADFONT,            0,   0, "ari08n__.sft"},
       { C_LOADFONT,            0,   0, "ari09b__.sft"},
       { C_LOADFONT,            0,   0, "ari10n__.sft"},
//       { C_LOADFONT,            0,   0, "ari14b__.sft"},

       /* Comandos Dinamicos */
       { C_EXECMACRO,           1,   0},
       { C_RESETMARGINS,        0,   0},
       { C_TOPMARGIN,           1,   0},
       { C_SELECTFONT,          0,   0, "ari08n__.sft"},

       /* Comprovante de Entrega */
       { C_OFS,                 4,   10},
       { F_Cedente,             0,   3},
       { F_Agencia,             76,  3},
       { F_Cod_Cedente,         88,  3},
       { F_Sacado_1,            0,   9},
       { F_Nosso_Numero,        76,  9},
       { F_Vencimento,          0,   15},
       { F_Numero_Doc,          22,  15},
       { F_Comissao_Per,        49,  15,  "20"},
       { F_Valor_Doc,           83,  15,  "20"},

       /* Ficha de Caixa / Recibo do Sacado */
       { C_BEGINPROC,           0,   0},
       {    F_Cedente,          0,   3},
       {    F_Agencia,          102, 3},
       {    F_Cod_Cedente,      116, 3},

       {    F_Carteira_N,       135, 3},  /* Adaptacao exigida pelo */
       {    F_Nosso_Numero,     143, 3},  /* Banco America do Sul   */
/*     {    F_Nosso_Numero,     135, 3}, */

       {    F_Vencimento,       171, 3},
       {    F_Uso_Do_Banco,     0,   9},
       {    F_Carteira_S,       33,  9},
       {    F_Especie_Moeda,    53,  9},
       {    F_Qtd_Moeda,        70,  9,   "20"},
       {    F_Valor_Doc,        140, 9,   "25"},
       {    F_Data_Doc,         5,   15},
       {    F_Numero_Doc,       35,  15},
       {    F_Especie_Doc,      74,  15},
       {    F_Aceite,           92,  15},
       {    F_Data_Proces,      110, 15},
       {    F_Abatimento,       140, 15,  "25"},
       {    F_Instrucao_1,      0,   21},
       {    F_Instrucao_2,      0,   24},
       {    F_Instrucao_3,      0,   27},
       {    F_Instrucao_4,      0,   30},
       {    F_Instrucao_5,      0,   33},
       {    F_Sacado_1,         0,   39},
       {    F_Sacado_2,         0,   42},
       {    F_Sacador_Aval,     22,  45},
       { C_ENDPROC,             0,   0},

       /* Recibo do Sacado */
       { C_OFS,                 4,   55},
       { C_EXECPROC,            0,   -1},

       /* Ficha de Caixa */
       { C_OFS,                 4,   124},
       { C_EXECPROC,            0,   -1},

       /* Ficha de Compensacao */
       { C_OFS,                 4,   196},
       { F_Local_De_Pag_1,      0,   1},
       { F_Local_De_Pag_2,      0,   4},
       { F_Vencimento,          143, 2},
       { F_Cedente,             0,   10},
       { F_CGC_Cedente,         90,  10},
       { F_Agencia,             135, 10},
       { F_Cod_Cedente,         148, 10},
       { F_Data_Doc,            5,   17},
       { F_Numero_Doc,          39,  17},
       { F_Especie_Doc,         75,  17},
       { F_Aceite,              90,  17},
       { F_Data_Proces,         112, 17},
       { F_Carteira_N,          135, 17},
       { F_Nosso_Numero,        148, 17},
       { F_Carteira_S,          34,  23},
       { F_Uso_Do_Banco,        2,   23},
       { F_Especie_Moeda,       49,  23},
       { F_Qtd_Moeda,           56,  23,  "20"},
       { F_Valor_Doc,           140, 23,  "25"},
       { F_Abatimento,          140, 29,  "25"},
       { F_Instrucao_1,         0,   34},
       { F_Instrucao_2,         0,   37},
       { F_Instrucao_3,         0,   40},
       { F_Instrucao_4,         0,   43},
       { F_Instrucao_5,         0,   46},
       { F_Instrucao_6,         0,   49},
       { F_Sacado_1,            3,   61},
       { F_Sacado_2,            3,   64},
       { F_Sacado_3,            3,   67},
       { F_Sacador_Aval,    	22,  72},
       { F_Unidade_Cedente,     18,  55},
       { F_Parcela,            165,  2},
       { F_Codigo_Baixa,       171,  72},

       /* Imprime linha digitavel */
       { C_OFS,                 0,   0},
       { C_SELECTFONT,          0,   0, "ari10n__.sft"},
//       { C_SELECTFONT,          0,   0, "ari14b__.sft"},
       { C_PRINTCALC,          74, 191},

       /* Fim do Bloqueto */
       { C_STOPRUN,            -1,  -1}
};

static COMMAND form_2[] = {
       /* Comandos Estaticos */
       { C_LOADMACRO,           1,   0},
       { C_LOADFONT,            0,   0, "ari06n__.sft"},
       { C_LOADFONT,            0,   0, "ari08n__.sft"},
       { C_LOADFONT,            0,   0, "ari09b__.sft"},
       { C_LOADFONT,            0,   0, "ari10n__.sft"},

       /* Comandos Dinamicos */
       { C_EXECMACRO,           1,   0},
       { C_RESETMARGINS,        0,   0},
       { C_TOPMARGIN,           1,   0},
       { C_SELECTFONT,          0,   0, "ari08n__.sft"},

       /* Guia de uma Boleta */
       { C_BEGINPROC,           0,   0},
       {    F_Local_De_Pag_1,   0,   1},
       {    F_Local_De_Pag_2,   0,   4},
       {    F_Vencimento,     143,   2},
       {    F_Cedente,          0,  10},
       {    F_CGC_Cedente,     90,  10},
       {    F_Agencia,        135,  10},
       {    F_Cod_Cedente,    148,  10},
       {    F_Data_Doc,         0,  16},
       {    F_Numero_Doc,      39,  16},
       {    F_Especie_Doc,     75,  16},
       {    F_Aceite,          90,  16},
       {    F_Data_Proces,    112,  16},
       {    F_Carteira_N,     135,  16},
       {    F_Nosso_Numero,   148,  16},
       {    F_Carteira_S,      31,  22},
       {    F_Uso_Do_Banco,     2,  22},
       {    F_Especie_Moeda,   49,  22},
       {    F_Qtd_Moeda,       56,  22,  "20"},
       {    F_Valor_Doc,      143,  22,  "15"},
       {    F_Valor_Desconto,   0,  28,  "15"},
       {    F_Data_Desconto,   50,  28},
       {    F_Comissao_Per,    80,  28,  "17"},
       {    F_Instrucao_1,      0,  34},
       {    F_Instrucao_2,      0,  38},
       {    F_Instrucao_3,      0,  42},
       {    F_Instrucao_4,      0,  46},
       {    F_Instrucao_5,      0,  50},
       {    F_Instrucao_6,     -1,  -1},
       {    F_Sacado_1,         0,  58},
       {    F_Sacado_2,         0,  62},
       {    F_Sacado_3,         0,  66},
       {    F_Sacador_Aval,    -1,  -1},
       { C_ENDPROC,             0,   0},

       /* Ficha de Caixa */
       { C_OFS,                 4,  16},
       { C_EXECPROC,            0,  -1},

       /* Recibo do Sacado */
       { C_OFS,                 4, 106},
       { C_EXECPROC,            0,  -1},

       /* Ficha de Compensacao */
       { C_OFS,                 4, 195},
       { C_EXECPROC,            0,  -1},

       /* Imprime linha digitavel */
       { C_OFS,                 2,  14},
       { C_SELECTFONT,          0,   0, "ari10n__.sft"},
       { C_PRINTCALC,           0, 173},

       /* Fim do Bloqueto */
       { C_STOPRUN,            -1,  -1}
};

/*--------------------------------------------------------------------*/
static int Presents()

{ char aux[80];
  int  tab;

  strcpy(aux, MYNAME);
  strcat(aux, " v");
  strcat(aux, VERSION);
  strcat(aux, " - ");
  tab = strlen(aux);

  fprintf(stderr, "%s%s para o Sistema %s.\n", aux, MYDESCRIPTION, NAME);
  fprintf(stderr, "%*c(c)%s, por %s.\n\n", tab, ' ', REVISION, AUTHOR);

  return 0;
}

/*--------------------------------------------------------------------*/
static int Usage()

{ int i;

  fprintf(stderr, "Uso:  %s cod_bco [modelo [arq.prn]]\n\n", myname);
  fputs("        cod_bco: Codigo do Banco\n", stderr);
  for (i = 0; i < sizeof(bancos)/sizeof(BANK); i++) {
      fprintf(stderr, "%17c%03d - %s", ' ', bancos[i].code, bancos[i].name);
      if (bancos[i].obs[0])
         fprintf(stderr, " (%s)", bancos[i].obs);
      fputc('\n', stderr);
  }
  fputc('\n', stderr);
  fputs("        modelo:  1 - Bloqueto normal (default)\n", stderr);
  fputs("                 2 - Bloqueto sem comprovante de entrega\n", stderr);

  exit(1);
  return 1;
}

/*--------------------------------------------------------------------*/
static int VLine(x, y1, y2, l)
int x;
int y1;
int y2;
int l;

{ int v;

  v = y2-y1;

  PrnXY(OFS_X+x, OFS_Y+y1);
  fprintf(cbPrnFile, "\x1b*c%da%db%dP", l, v, 0);

  return 0;
}

/*--------------------------------------------------------------------*/
static int HLine(y, x1, x2, l)
int y;
int x1;
int x2;
int l;

{ int h;

  h = x2-x1;

  PrnXY(OFS_X+x1, OFS_Y+y);
  fprintf(cbPrnFile, "\x1b*c%da%db%dP", h, l, 0);

  return 0;
}

/*--------------------------------------------------------------------*/
static int Box(x1, y1, x2, y2, l)
int x1, y1, x2, y2, l;

{
  VLine(x1, y1, y2,   l);
  VLine(x2, y1, y2+l, l);
  HLine(y1, x1, x2,   l);
  HLine(y2, x1, x2,   l);

  return 0;
}

/*--------------------------------------------------------------------*/
static int PrintXY(x, y, str)
int x;
int y;
char *str;

{
  /* Posiciona impressao */
  PrnXY(x + OFS_X, y + OFS_Y);

  /* Imprime texto */
  if (str != NULL)
     fprintf(cbPrnFile, str);

  return 0;
}

/*--------------------------------------------------------------------*/
static int PrintFrame(x, y)
int x;
int y;

{ int i;

  /* Comprovante de Entrega */
  OFS_X = x; OFS_Y = y;

  HLine(m2d(0),   m2d(0),   m2d(195), 7);
  HLine(m2d(28),  m2d(0),   m2d(195), 7);
  HLine(m2d(6),   m2d(0),   m2d(107), 2);
  HLine(m2d(12),  m2d(0),   m2d(107), 2);
  HLine(m2d(18),  m2d(0),   m2d(195), 2);
  VLine(m2d(74),  m2d(0),   m2d(12),  2);
  VLine(m2d(107), m2d(0),   m2d(28),  7);
  VLine(m2d(18),  m2d(12),  m2d(18),  2);
  VLine(m2d(45),  m2d(12),  m2d(18),  2);
  VLine(m2d(71),  m2d(12),  m2d(18),  2);
  VLine(m2d(37),  m2d(18),  m2d(28),  2);
  VLine(m2d(59),  m2d(18),  m2d(28),  2);
  VLine(m2d(132), m2d(18),  m2d(28),  2);

  VLine(m2d(109), m2d(3),   m2d(7),   3);
  VLine(m2d(109), m2d(8),   m2d(12),  3);
  VLine(m2d(109), m2d(13),  m2d(17),  3);
  VLine(m2d(113), m2d(3),   m2d(7),   3);
  VLine(m2d(113), m2d(8),   m2d(12),  3);
  VLine(m2d(113), m2d(13),  m2d(17),  3);
  VLine(m2d(132), m2d(3),   m2d(7),   3);
  VLine(m2d(132), m2d(8),   m2d(12),  3);
  VLine(m2d(132), m2d(13),  m2d(17),  3);
  VLine(m2d(136), m2d(3),   m2d(7),   3);
  VLine(m2d(136), m2d(8),   m2d(12),  3);
  VLine(m2d(136), m2d(13),  m2d(17),  3);
  VLine(m2d(158), m2d(3),   m2d(7),   3);
  VLine(m2d(158), m2d(8),   m2d(12),  3);
  VLine(m2d(158), m2d(13),  m2d(17),  3);
  VLine(m2d(162), m2d(3),   m2d(7),   3);
  VLine(m2d(162), m2d(8),   m2d(12),  3);
  VLine(m2d(162), m2d(13),  m2d(17),  3);

  /* Recibo do Sacado / Ficha de Caixa */

  for (i = 0; i < 2; i++) {

      switch (i) {
             case 0: /* Recibo do Sacado */
                  OFS_X = x; OFS_Y = y + m2d(45);
                  VLine(m2d(94),  m2d(0),   m2d(6),   2);
                  break;
             case 1: /* Ficha de Caixa */
                  OFS_X = x; OFS_Y = y + m2d(114);
                  VLine(m2d(100), m2d(0),   m2d(6),   2);
                  break;
      }

      HLine(m2d(0),   m2d(0),   m2d(195), 7);
      HLine(m2d(47),  m2d(0),   m2d(195), 7);
      HLine(m2d(36),  m2d(0),   m2d(195), 2);
      HLine(m2d(6),   m2d(0),   m2d(195), 2);
      HLine(m2d(12),  m2d(0),   m2d(195), 2);
      HLine(m2d(18),  m2d(0),   m2d(195), 2);
      HLine(m2d(24),  m2d(134), m2d(195), 2);
      HLine(m2d(30),  m2d(134), m2d(195), 2);
      HLine(m2d(36),  m2d(134), m2d(195), 2);
      VLine(m2d(134), m2d(0),   m2d(47),  7);
      VLine(m2d(170), m2d(0),   m2d(6),   2);
      VLine(m2d(31),  m2d(6),   m2d(18),  2);
      VLine(m2d(48),  m2d(6),   m2d(12),  2);
      VLine(m2d(68),  m2d(6),   m2d(12),  2);
      VLine(m2d(107), m2d(6),   m2d(12),  2);
      VLine(m2d(71),  m2d(12),  m2d(18),  2);
      VLine(m2d(89),  m2d(12),  m2d(18),  2);
      VLine(m2d(102), m2d(12),  m2d(18),  2);

      HLine(m2d(49),  m2d(100), m2d(133), 2);
      HLine(m2d(49),  m2d(161), m2d(193), 2);
      VLine(m2d(100), m2d(49),  m2d(54),  2);
      VLine(m2d(193), m2d(49),  m2d(54),  2);
  }

  /* Ficha de Compensacao */
  OFS_X = x; OFS_Y = y + m2d(184);

  HLine(m2d(0),   m2d(0),   m2d(195), 7);
  HLine(m2d(60),  m2d(0),   m2d(195), 7);
  HLine(m2d(76),  m2d(0),   m2d(195), 7);
  HLine(m2d(9),   m2d(0),   m2d(195), 2);
  HLine(m2d(16),  m2d(0),   m2d(195), 2);
  HLine(m2d(22),  m2d(0),   m2d(195), 2);
  HLine(m2d(28),  m2d(0),   m2d(195), 2);
  HLine(m2d(35),  m2d(134), m2d(195), 2);
  HLine(m2d(41),  m2d(134), m2d(195), 2);
  HLine(m2d(47),  m2d(134), m2d(195), 2);
  HLine(m2d(54),  m2d(134), m2d(195), 2);

  VLine(m2d(134), m2d(0),   m2d(60),  7);
  VLine(m2d(31),  m2d(16),  m2d(28),  2);
  VLine(m2d(71),  m2d(16),  m2d(22),  2);
  VLine(m2d(89),  m2d(16),  m2d(22),  2);
  VLine(m2d(102), m2d(16),  m2d(22),  2);
  VLine(m2d(48),  m2d(22),  m2d(28),  2);
  VLine(m2d(66),  m2d(22),  m2d(28),  2);
  VLine(m2d(107), m2d(22),  m2d(28),  2);
/*
  HLine(m2d(78),  m2d(100), m2d(133), 2);
  HLine(m2d(78),  m2d(161), m2d(193), 2);
*/
  return 0;
}

/*--------------------------------------------------------------------*/
static int PrintText(x, y)
int x, y;

{ char aux[100], i;

  /* Titulos */
  SelectFont(ARI09b);

  OFS_X = x; OFS_Y = y;

  PrintXY(m2d(156), m2d(-1),  "Comprovante de Entrega");
  PrintXY(m2d(166), m2d(44),  "Recibo do Sacado");
  PrintXY(m2d(171), m2d(113), "Ficha de Caixa");
/*
  sprintf(aux, "Ficha de Compensa%c%co", CEDILHA, A_TIL);
  PrintXY(m2d(158), m2d(277), aux);
*/
  SelectFont(ARI06n);

  /* Comprovante de Entrega */
  OFS_X = x; OFS_Y = y;

  PrintXY(m2d(0),     m2d(2)+3,   "Cedente");
  sprintf(aux, "Ag%cncia/C%cdigo Cedente", E_GRAVE, O_AGUDO);
  PrintXY(m2d(75),    m2d(2)+3,   aux);
  sprintf(aux, "Motivos da n%co entrega (Para uso da empresa entregadora)", A_TIL);
  PrintXY(m2d(118),   m2d(2)+3,   aux);
  PrintXY(m2d(0),     m2d(8),     "Sacado");
  sprintf(aux, "Nosso N%cmero", U_AGUDO);
  PrintXY(m2d(75),    m2d(8),     aux);
  PrintXY(m2d(0),     m2d(14),    "Vencimento");
  sprintf(aux, "N%c do Documento",  N_O);
  PrintXY(m2d(19),    m2d(14),    aux);
  sprintf(aux, "Mora Di%cria",  A_AGUDO);
  PrintXY(m2d(46),    m2d(14),    aux);
  PrintXY(m2d(72),    m2d(14),    "Valor do Documento");
  PrintXY(m2d(38),    m2d(20),    "Data");
  PrintXY(m2d(60),    m2d(20),    "Assinatura");
  PrintXY(m2d(108),   m2d(20),    "Data");
  PrintXY(m2d(133),   m2d(20),    "Entregador");
  sprintf(aux, "Recebi(emos) o bloqueto/t%ctulo", I_AGUDO);
  PrintXY(m2d(0),     m2d(22),    aux);
  sprintf(aux, "com as caracter%csticas acima", I_AGUDO);
  PrintXY(m2d(0),     m2d(25),    aux);
  PrintXY(m2d(114),   m2d(6),     "Mudou-se");
  PrintXY(m2d(137),   m2d(6),     "Ausente");
  sprintf(aux, "N%co existe n%c indicado", A_TIL, N_O);
  PrintXY(m2d(163),   m2d(6),     aux);
  PrintXY(m2d(114),   m2d(11),    "Recusado");
  sprintf(aux, "N%co procurado", A_TIL);
  PrintXY(m2d(137),   m2d(11),    aux);
  sprintf(aux, "Endere%co insuficiente", CEDILHA);
  PrintXY(m2d(163),   m2d(11),    aux);
  PrintXY(m2d(114),   m2d(16),    "Desconhecido");
  PrintXY(m2d(137),   m2d(16),    "Falecido");
  PrintXY(m2d(163),   m2d(16),    "Outros (Anotar no verso)");

  /* Recibo do Sacado / Ficha de Caixa */

  for (i = 0; i < 2; i++) {

      switch (i) {
             case 0: /* Recibo do Sacado */
                  OFS_X = x; OFS_Y = y + m2d(45);
                  sprintf(aux, "Ag%cncia/C%cdigo Cedente", E_GRAVE, O_AGUDO);
                  PrintXY(m2d(95),    m2d(2)+4,   aux);
#ifdef MSG_RECEB
                  sprintf(aux, "Recebimento atrav%cs do cheque n%c", E_AGUDO, N_O);
                  PrintXY(m2d(0), m2d(50),    aux);
                  PrintXY(m2d(0), m2d(53)-4,  "do banco");
                  sprintf(aux, "Esta quita%c%co s%c ter%c validade ap%cs pagamento",
                                CEDILHA, A_TIL, O_AGUDO, A_AGUDO, O_AGUDO);
                  PrintXY(m2d(0), m2d(56)-8,  aux);
                  PrintXY(m2d(0), m2d(59)-12, "do cheque pelo Banco sacado.");
                  HLine(m2d(50),  m2d(34),  m2d(47), 1);
                  HLine(m2d(53),  m2d(10),  m2d(20), 1);
#endif
                  break;
             case 1: /* Ficha de Caixa */
                  OFS_X = x; OFS_Y = y + m2d(114);
                  sprintf(aux, "Ag%cncia/C%cdigo Cedente", E_GRAVE, O_AGUDO);
                  PrintXY(m2d(101),   m2d(2)+4,   aux);
                  break;
      }

      PrintXY(m2d(0),     m2d(2)+4,   "Cedente");
      sprintf(aux, "Nosso N%cmero", U_AGUDO);
      PrintXY(m2d(135),   m2d(2)+4,   aux);
      PrintXY(m2d(171),   m2d(2)+4,   "Vencimento");
      PrintXY(m2d(0),     m2d(8),     "Uso do Banco");
      PrintXY(m2d(32),    m2d(8),     "Carteira");
      sprintf(aux, "Esp%ccie", E_AGUDO);
      PrintXY(m2d(49),    m2d(8),     aux);
      PrintXY(m2d(69),    m2d(8),     "Quantidade");
      PrintXY(m2d(108),   m2d(8),     "Valor");
      PrintXY(m2d(135),   m2d(8),     "(=) Valor do Documento");
      sprintf(aux, "Data de Emiss%co", A_TIL);
      PrintXY(m2d(0),     m2d(14),    aux);
      sprintf(aux, "N%cmero do Documento", U_AGUDO);
      PrintXY(m2d(32),    m2d(14),    aux);
      sprintf(aux, "Esp%ccie Doc.", E_AGUDO);
      PrintXY(m2d(72),    m2d(14),    aux);
      PrintXY(m2d(90),    m2d(14),    "Aceite");
      PrintXY(m2d(103),   m2d(14),    "Data de Processamento");
      PrintXY(m2d(135),   m2d(14),    "(-) Desconto/Abatimento");
#ifdef MSG_RESPONSA
      sprintf(aux, "Instru%c%ces  (Texto de Responsabilidade do Cedente)",
                    CEDILHA, O_TIL);
#else
      sprintf(aux, "Instru%c%ces", CEDILHA, O_TIL);
#endif
      PrintXY(m2d(0),     m2d(20),    aux);
      sprintf(aux, "(-) Outras Dedu%c%ces", CEDILHA, O_TIL);
      PrintXY(m2d(135),   m2d(20),    aux);
      PrintXY(m2d(135),   m2d(26),    "(+) Mora/Multa");
      sprintf(aux, "(+) Outros Acr%cscimos", E_AGUDO);
      PrintXY(m2d(135),   m2d(32),    aux);
      PrintXY(m2d(0),     m2d(38),    "Sacado");
      PrintXY(m2d(135),   m2d(38),    "(=) Valor Cobrado");
      PrintXY(m2d(0),     m2d(47)-4,  "Sacador/Avalista");

      sprintf(aux, "Autentica%c%co Mec%cnica", CEDILHA, A_TIL, A_GRAVE);
      PrintXY(m2d(135),   m2d(50),    aux);

  }

  /* Ficha de Compensacao */
  OFS_X = x; OFS_Y = y + m2d(184);

  PrintXY(m2d(0),     m2d(2)+3,   "Local de Pagamento");
  PrintXY(m2d(135),   m2d(2)+3,   "Vencimento");
  PrintXY(m2d(0),     m2d(11),    "Cedente");
  sprintf(aux, "Ag%cncia/C%cdigo Cedente", E_GRAVE, O_AGUDO);
  PrintXY(m2d(135),   m2d(11),    aux);
  sprintf(aux, "Data de Emiss%co", A_TIL);
  PrintXY(m2d(0),     m2d(18),    aux);
  sprintf(aux, "N%cmero do Documento", U_AGUDO);
  PrintXY(m2d(32),    m2d(18),    aux);
  sprintf(aux, "Esp%ccie Doc.", E_AGUDO);
  PrintXY(m2d(72),    m2d(18),    aux);
  PrintXY(m2d(90),    m2d(18),    "Aceite");
  PrintXY(m2d(103),   m2d(18),    "Data de Processamento");
  if (cod_bco != 215)
     sprintf(aux, "Nosso N%cmero/C%cdigo do Documento", U_AGUDO, O_AGUDO);
  else
     sprintf(aux, "Nosso N%cmero", U_AGUDO);
  PrintXY(m2d(135),   m2d(18),   aux);
  PrintXY(m2d(0),     m2d(24),    "Uso do Banco");
  PrintXY(m2d(32),    m2d(24),    "Carteira");
  sprintf(aux, "Esp%ccie", E_AGUDO);
  PrintXY(m2d(49),    m2d(24),    aux);
  PrintXY(m2d(67),    m2d(24),    "Quantidade");
  PrintXY(m2d(108),   m2d(24),    "Valor");
  PrintXY(m2d(135),   m2d(24),    "(=) Valor do Documento");
#ifdef MSG_RESPONSA
      sprintf(aux, "Instru%c%ces  (Texto de Responsabilidade do Cedente)",
                    CEDILHA, O_TIL);
#else
      sprintf(aux, "Instru%c%ces", CEDILHA, O_TIL);
#endif
  PrintXY(m2d(0),     m2d(30),    aux);
  PrintXY(m2d(135),   m2d(30),    "(-) Desconto/Abatimento");
  sprintf(aux, "(-) Outras Dedu%c%ces", CEDILHA, O_TIL);
  PrintXY(m2d(135),   m2d(37),    aux);
  PrintXY(m2d(135),   m2d(43),    "(+) Mora/Multa");
  sprintf(aux, "(+) Outros Acr%cscimos", E_AGUDO);
  PrintXY(m2d(135),   m2d(49),    aux);
  PrintXY(m2d(135),   m2d(56),    "(=) Valor Cobrado");
  PrintXY(m2d(0),     m2d(59)+3,    "Unidade Cedente");
  PrintXY(m2d(0),     m2d(62)+3,  "Sacado");
  PrintXY(m2d(0),     m2d(75)+6,  "Sacador/Avalista");
  sprintf(aux, "C%cdigo de Baixa", O_AGUDO);
  PrintXY(m2d(153),   m2d(75)+6,  aux);

  sprintf(aux, "Autentica%c%co Mec%cnica / ", CEDILHA, A_TIL, A_GRAVE);
/*  PrintXY(m2d(135),   m2d(78)+6,  aux);*/
  PrintXY(m2d(133), m2d(80),  aux);

  SelectFont(ARI09b);
  sprintf(aux, "Ficha de Compensa%c%co", CEDILHA, A_TIL);
  PrintXY(m2d(158), m2d(80), aux);

  return 0;
}

/*--------------------------------------------------------------------*/
static int PrintFrame2(x, y)
int x;
int y;

{
  OFS_X = x; OFS_Y = y;

  Box(m2d(0),     m2d(0),   m2d(190), m2d(74), 2);
  HLine(m2d(9),   m2d(0),   m2d(190), 1);
  HLine(m2d(15),  m2d(0),   m2d(190), 1);
  HLine(m2d(21),  m2d(0),   m2d(190), 1);
  HLine(m2d(27),  m2d(0),   m2d(190), 1);
  HLine(m2d(33),  m2d(0),   m2d(190), 1);
  HLine(m2d(39),  m2d(128), m2d(190), 1);
  HLine(m2d(45),  m2d(128), m2d(190), 1);
  HLine(m2d(51),  m2d(128), m2d(190), 1);
  HLine(m2d(57),  m2d(0),   m2d(190), 1);

  VLine(m2d(128), m2d(0),   m2d(57),  1);
  VLine(m2d(25),  m2d(15),  m2d(21),  1);
  VLine(m2d(68),  m2d(15),  m2d(21),  1);
  VLine(m2d(87),  m2d(15),  m2d(21),  1);
  VLine(m2d(102), m2d(15),  m2d(21),  1);
  VLine(m2d(29),  m2d(21),  m2d(27),  1);
  VLine(m2d(43),  m2d(21),  m2d(27),  1);
  VLine(m2d(66),  m2d(21),  m2d(27),  1);
  VLine(m2d(110), m2d(21),  m2d(27),  1);
  VLine(m2d(48),  m2d(27),  m2d(33),  1);
  VLine(m2d(79),  m2d(27),  m2d(33),  1);
  VLine(m2d(128), m2d(27),  m2d(33),  1);

  return 0;
}

/*--------------------------------------------------------------------*/
static int PrintText2(x, y, t)
int x, y, t;

{ char aux[100];

  OFS_X = x; OFS_Y = y;

  SelectFont(ARI06n);
  PrintXY(m2d(1),   m2d(2),  "Local de Pagamento");
  PrintXY(m2d(129), m2d(2),  "Vencimento");
  PrintXY(m2d(1),   m2d(11), "Cedente");
  sprintf(aux, "Ag%cncia/C%cd.Cedente", E_GRAVE, O_AGUDO);
  PrintXY(m2d(129), m2d(11), aux);
  PrintXY(m2d(3),   m2d(17), "Data Documento");
  PrintXY(m2d(35),  m2d(17), "N\xFAmero Documento");
  sprintf(aux, "Esp%ccie Doc.", E_AGUDO);
  PrintXY(m2d(71),  m2d(17), aux);
  PrintXY(m2d(91),  m2d(17), "Aceite");
  PrintXY(m2d(106), m2d(17), "Data Processam.");
  PrintXY(m2d(129), m2d(17), "Nosso N\xFAmero.");
  PrintXY(m2d(5),   m2d(23), "Uso do Banco");
  PrintXY(m2d(32),  m2d(23), "Carteira");
  PrintXY(m2d(49),  m2d(23), "Esp.Moeda");
  PrintXY(m2d(75),  m2d(23), "Quantidade");
  PrintXY(m2d(113), m2d(23), "Vr.Moeda");
  PrintXY(m2d(129), m2d(23), "1-(=)Valor Documento");
  PrintXY(m2d(14),  m2d(29), "Valor do Desconto");
  PrintXY(m2d(60),  m2d(29), "At\xE9");
  PrintXY(m2d(89),  m2d(29), "Com.Perman\xEAncia por dia");
  PrintXY(m2d(129), m2d(29), "2-(-)Desconto/Abatimento");
  sprintf(aux, "Instru%c%ces", CEDILHA, O_TIL);
  PrintXY(m2d(1),   m2d(35), aux);
  sprintf(aux, "3-(-)Outras Dedu%c%ces", CEDILHA, O_TIL);
  PrintXY(m2d(129), m2d(35), aux);
  PrintXY(m2d(129), m2d(41), "4-(+)Mora/Multa");
  PrintXY(m2d(129), m2d(47), "5-(+)Outros Acr\xE9scimos");
  PrintXY(m2d(129), m2d(53), "6-(=)Valor Cobrado");
  PrintXY(m2d(1),   m2d(59), "Sacado");
  PrintXY(m2d(1),   m2d(74), "Sacador/Avalista");
  switch (t) {
         case 0:
         case 1: PrintXY(m2d(141),  m2d(59), "Autentica\xE7\xE3o Mec\xE2nica"); break;
         case 2: PrintXY(m2d(141),  m2d(77), "Autentica\xE7\xE3o Mec\xE2nica"); break;
  }

  SelectFont(ARI09b);
  switch (t) {
         case 0: PrintXY(m2d(168), m2d(-1), "Ficha de Caixa");   break;
         case 1: PrintXY(m2d(163), m2d(-1), "Recibo do Sacado"); break;
         case 2: sprintf(aux, "Ficha de Compensa%c%co", CEDILHA, A_TIL);
                 PrintXY(m2d(154), m2d(93), aux);
                 break;
  }
  return 0;
}

/*-----------------------------------------------------------------*/
static int LoadFont(fontname)
char *fontname;

{ FILE *fp;

  /* Carrega fonte */
  if ((fp = fopen(fontname, "rb")) == NULL)
     return 1;

  FCopy(cbPrnFile, fp);
  fclose(fp);

  return 0;
}

/*-----------------------------------------------------------------*/
static int GetFontId(fname)
char *fname;

{ char aux[10];
  int  i, ret;

  memcpy(aux, &(fname[3]), 2); aux[2] = 0;
  i = atoi(aux);
  ret = 1000 + (i * 10);
  switch (toupper(fname[5])) {
         case 'N': break;
         case 'B': ret += 5; break;
         case 'I': ret += 3; break;
         default:  break;
  }

  return ret;
}

/*--------------------------------------------------------------------*/
static int FCopy(dest, src)
FILE *dest, *src;
{ char  *buf;
  size_t bufs, r;

  bufs = coreleft();

  if ((buf = malloc(bufs)) == NULL) return -1;

  do {
     r = fread(buf, 1, bufs, src);
     fwrite(buf, 1, r, dest);
  } while (r > 0L);

  free(buf);
  return 0;
}

/*--------------------------------------------------------------------*/
static long FSize(f)
FILE *f;

{ long old, size;

  old = ftell(f);
  fseek(f, 0, 2);
  size = ftell(f);
  fseek(f, old, 0);

  return size;
}

/*--------------------------------------------------------------------*/
static int TrimFile(dest, fname, ini, fim)
FILE *dest;
char *fname;
int  ini, fim;

{ FILE *filent;
  int   ch;
  long  i, size;

  if ((filent = fopen(fname, "rb" )) == NULL)
     return -1;

  fseek(filent, 0, 2);
  size = ftell(filent);
  fseek(filent, ini, 0);

  for (i = ini; i < size-fim; i++) {
      ch = fgetc(filent);
      fputc(ch, dest);
  }

  fclose(filent);

  return 0;
}

/*--------------------------------------------------------------------*/
static int SaveCmds(fname, cod_bco, modelo)
char *fname;
short cod_bco, modelo;

{ FILE    *f, *mf;
  short   size, i;
  COMMAND *p;
  long    msize;
  short   mid;

  /* Abre arquivo */
  if ((f = fopen(fname, "wb")) == NULL)
     return -1;

  /* Calcula o tamanho do array */
  if (modelo == 2) {
     size = sizeof(form_2) / sizeof(*p);
     p    = form_2;
  }
  else {
     size = sizeof(form_1) / sizeof(*p);
     p    = form_1;
  }

  /* Preenche variaveis no "programa-array" */
  for (i = 0; i < sizeof(ofs_cod) / (sizeof(short)*2); i++)
      if (ofs_cod[i].code == cod_bco) {
         p[size-2].x = ofs_cod[i].ofs;
         break;
      }

  /* Grava id */
  fprintf(f, CMDFILE_ID);

  /* Grava versao do sistema */
  i = 0;
  fwrite(&i, sizeof(i), 1, f);

  /* Grava codigo do banco */
  fwrite(&cod_bco, sizeof(cod_bco), 1, f);

  /* Grava size e o array */
  fwrite(&size, sizeof(size), 1, f);
  fwrite(p, size * sizeof(*p), 1, f);

  /* Grava macro */
  mid = 1;
  fwrite(&mid, sizeof(mid), 1, f);

  mf = fopen(outname, "rb");
  msize = FSize(mf);
  fwrite(&msize, sizeof(msize), 1, f);
  FCopy(f, mf);
  fclose(mf);

  /* Fim da sessao de macros */
  mid = 0;
  fwrite(&mid, sizeof(mid), 1, f);

  /* Grava o chksum */
  mid = 0;
  fwrite(&mid, sizeof(mid), 1, f);

  /* Fecha o arquivo */
  fclose(f);

  return 0;
}

/*--------------------------------------------------------------------*/
int main(argc, argv)
int argc;
char **argv;

{ char lname[100], cname[100];
  int  modelo, redir;

  Presents();
  outname[0] = redir = 0;

  /* Se nao houve parametros, mostra help e termina */
  switch (argc) {
         case 2:
              modelo  = DEFAULT_MODEL;
              cod_bco = atoi(argv[1]);
              break;
         case 4:
              strcpy(outname, argv[3]);
              redir = 1;
         case 3:
              modelo  = atoi(argv[2]);
              if (modelo != 1 && modelo != 2)
                 Usage();
              cod_bco = atoi(argv[1]);
              break;
         default:
              Usage();
              break;
  }

  if (!redir)
     sprintf(outname, "cbform%d.%03d", modelo, cod_bco);

  if ((cbPrnFile = fopen(outname, "wb")) == NULL)
     return -1;

  if (redir)
     fprintf(cbPrnFile, "\x1b%c", 'E');

  sprintf(lname, "lgo%03d_%d.prn", cod_bco, modelo);

  puts("Copiando logotipo...");
  if (TrimFile(cbPrnFile, lname, 2, 3)) {
     fprintf(stderr, "%s: Arquivo \'%s\' nao encontrado!\x07\n", myname, lname);
     fclose(cbPrnFile);
     unlink(outname);
     exit(-1);
  }

  if (redir) {
     puts("Copiando fontes...");
     if (LoadFont(ARI06n)) {
        fprintf(stderr, "%s: Arquivo \'%s\' nao encontrado!\x07\n", myname, ARI06n);
        fclose(cbPrnFile);
        unlink(outname);
        exit(-1);
     }
     if (LoadFont(ARI09b)) {
        fprintf(stderr, "%s: Arquivo \'%s\' nao encontrado!\x07\n", myname, ARI09b);
        fclose(cbPrnFile);
        unlink(outname);
        exit(-1);
     }
  }

  puts("Gerando formulario...");
  switch (modelo) {
         case 1:
              PrintFrame(m2d(4), m2d(8)+3);
              PrintText (m2d(4), m2d(8)+3);
              break;
         case 2:
              PrintText2(23, 141,  0);
              PrintText2(23, 1197, 1);
              PrintText2(23, 2254, 2);

              PrintFrame2(23, 141);
              PrintFrame2(23, 1197);
              PrintFrame2(23, 2254);
              break;
  }

  if (redir)
     fputc('\f', cbPrnFile);

  fclose(cbPrnFile);

  if (!redir) {
     puts("Gerando programacao do formulario...");
     sprintf(cname, "%s%03d", B_CMDFILE, cod_bco);
     SaveCmds(cname, cod_bco, modelo);
     unlink(outname);
  }

  if (redir)
     printf("Arquivo de impressao \'%s\' OK.\n", outname);
  else
     printf("Arquivo de p-code \'%s\' OK.\n", cname);

  return 0;
}

