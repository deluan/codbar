/*********************************************************************
 *                                                                   *
 * CODBAR - Impressao de Bloquetos de Cobranca com Codigo de Barra   *
 *                                                                   *
 * cbPrint.c  Impressao dos Bloquetos	                             *
 *                                                                   *
 *********************************************************************
 * Este modulo contem as rotinas de impressao laser do sistema.      *
 *                                                                   *
 * As seguintes variaveis e rotinas publicas estao definidas:        *
 *                                                                   *
 * cbPrintInit()  - inicializa a impressora, carrega fonts, etc..    *
 * cbPrintBegin() - inicia a impressao de uma pagina                 *
 * cbPrintFrame() - imprime o formulario vazio do bloqueto           *
 * cbPrintData()  - imprime somente os dados que compoe o bloqueto   *
 * cbPrintEnd()   - finaliza a impressao de uma pagina               *
 * cbPrintTerm()  - finaliza a impressao                             *
 *                                                                   *
 * By: @rivelaro/Perez, General Software, Petropolis 0 Graus         *
 *********************************************************************
 *       (C) Copyright General Software Tecnologia 1994              *
 *********************************************************************
 * 27 Jun 1994 - DP primeira versao                                  *
 *********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "winprof.h"
#include "codbar.h"
#include "pcl.h"

/* Numero de linhas e offset da barra */
#define BAR_LENGTH   151    /* dots */
#define BAR_X        0      /* milimetros */
#define BAR_Y        272    /* milimetros */

/* Numero de dots de cada coluna da barra */
#define POINT_SIZE   3      /* dots */

FILE *cbPrnFile = NULL;

static int   VLine        OF((int, int, int, int));
static int   OutputCode   OF((char));
static int   PrintBarCode OF((char *));

/*--------------------------------------------------------------------*/
int cbPrintInit()

{ 
  /* Carrega programa externo */
  if (cbPcodeLoad() != OK)
     return ERR_ACCESS;

  /* Abre arquivo de impressao */
  if (cbIniPrinter[0] == '-')
     cbPrnFile = stdout;
  else
     if ((cbPrnFile = fopen(cbIniPrinter, "wb")) == NULL)
        return ERR_OPENFILE;

  /* Reseta a impressora */
  fprintf(cbPrnFile, "\x1b%c", 'E');

  /* Roda comandos estaticos da boleta */
  cbPcodeExec(0, 2);

  return 0;
}

/*--------------------------------------------------------------------*/
int cbPrintBegin()
{
  /* Calcula os dados do codigo de barras */
  cbCalcInit();

  return 0;
}

/*--------------------------------------------------------------------*/
int cbPrintEnd()

{
  /* Envia FormFeed */
  fputc(FORMFEED, cbPrnFile);

  return 0;
}

/*--------------------------------------------------------------------*/
int cbPrintTerm()

{
  /* End Raster Graphics */
  PrnCmd_s("*r", "", 'B');

  /* Delete All Temporary Macros */
  PrnCmd_n("&f", 7, 'X');

  /* Reseta a impressora */
  fprintf(cbPrnFile, "\x1b%c", 'E');

  /* Se arq de impressao nao for stdout, fecha o arquivo */
  if (cbIniPrinter[0] != '-')
     fclose(cbPrnFile);

  return 0;
}

/*--------------------------------------------------------------------*/
int cbPrintFrame()

{
  /* Executa os comandos dinamicos de formulario */
  return cbPcodeExec(1, 0);
}

/*--------------------------------------------------------------------*/
int cbPrintData()

{ char *tmp;

  /* Imprime os dados do bloqueto (incluindo linha digitavel) */
  cbPcodeExec(1, 1);

  /* Imprime o codigo de barras */
  PrintBarCode(tmp = cbCalcBarCode());
  free(tmp);

  return 0;
}

/*--------------------------------------------------------------------*/
static int VLine(x, y1, y2, l)
int x;
int y1;
int y2;
int l;

{ int v;

  v = (y2-y1);

  PrnXY(x, y1);
  fprintf(cbPrnFile, "\x1b*c%da%db%dP", l, v, 0);

  return 0;
}

/*-----------------------------------------------------------------*/
static int OutputCode(b)
char b;

{ static int col, lin, ac = 0, last = -1;

  /* Inicializa variaveis estaticas */
  if (b == 0) {
     col  = m2d(BAR_X);
     lin  = m2d(BAR_Y);
     last = -1;
     ac   = 0;
     return 0;
  }

  /* Se e' a primeira vez... */
  if (last == -1) {
     last = b;
     ac   = 1;
  }
  else {
     /* Se repetiu o ultimo char, incrementa acumulador */
     if (last == b && b != 1)
        ac++;
     /* Senao processa acumulo */
     else {
        /* Se o ultimo char foi '1', entao "pinta" */
        if (last == '1')
           VLine(col, lin, lin + BAR_LENGTH, ac*3);
        /* Avanca coluna e zera acumulador */
        col += (ac * 3);
        last = b;
        ac   = 1;
     }
  }

  return 0;
}

/*-----------------------------------------------------------------*/
static int PrintBarCode(bits)
char *bits;

{ int j;
  char *s;

  /* Inicializa rotina de impressao */
  OutputCode(0);

  /* Imprime a sequencia de start do codigo de barras */
  for (s = cbI25Start; *s; s++)
      OutputCode(*s);

  /* Para cada elemento do codigo, expande e imprime */
  for (j = 0; j < strlen(bits); j++)
      for (s = cbI25Expand(bits, j); *s; s++)
          OutputCode(*s);

  /* Imprime a sequencia de stop do codigo */
  for (s = cbI25Stop; *s; s++)
      OutputCode(*s);

  /* Flush na saida de impressao */
  OutputCode(1);

  return 0;
}

