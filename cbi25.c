/*********************************************************************
 *                                                                   *
 * CODBAR - Impressao de Bloquetos de Cobranca com Codigo de Barra   *
 *                                                                   *
 * cbI25.c  Codigo de Barras 2 de 5 intercalado                      *
 *                                                                   *
 *********************************************************************
 * Este modulo contem rotinas para codificacao de barras padrao I25  *
 *                                                                   *
 * As seguintes variaveis e rotinas publicas estao definidas:        *
 *                                                                   *
 * cbI25Start    - codigo de inicio da barra                         *
 * cbI25Stop     - codigo de fim da barra                            *
 *                                                                   *
 * cbI25Code()   - monta barra com codigo I25                        *
 * cbI25Expand() - expande um elemento da barra para impressao       *
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

char cbI25Start[] = "1010";
char cbI25Stop[]  = "11101";

static char cypher[][6] = {
       "00110",
       "10001",
       "01001",
       "11000",
       "00101",
       "10100",
       "01100",
       "00011",
       "10010",
       "01010"
};

/*--------------------------------------------------------------------*/
char *cbI25Code(data)
char *data;

{ char *bits;
  int j, first, second;
  char *p1, *p2;

  /* Calcula o tamanho do codigo resultado e aloca memoria */
  j = (strlen(data) * 5) + 1;
  if ((bits = malloc(j)) == NULL)
     return NULL;

  memset(bits, 0, j);
  p2 = strchr(bits, 0);
  p1 = data;

  while (*p1) {
        /* Pega o proximo par de numeros */
        first  = *p1++ - 48;
        second = *p1++ - 48;

        /* Intercala os numeros */
        j = 0;
        while (j < 5) {
              *p2++ = cypher[first][j];
              *p2++ = cypher[second][j++];
        }
  }
  *p2 = 0;

  /* Retorna o codigo resultado (precisa ser desalocado posteriormente!) */
  return bits;
}

/*--------------------------------------------------------------------*/
char *cbI25Expand(code, ofs)
char *code;
int  ofs;

{
  if (ofs%2 == 0)
     return (code[ofs] == '1') ? "111" : "1";
  else
     return (code[ofs] == '1') ? "000" : "0";
}

