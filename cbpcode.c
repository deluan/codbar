/*********************************************************************
 *                                                                   *
 * CODBAR - Impressao de Bloquetos de Cobranca com Codigo de Barra   *
 *                                                                   *
 * cbPcode.c  Carrega programa externo e executa                     *
 *                                                                   *
 *********************************************************************
 * Este modulo processa o arquivo CMDFILE, que contem os comandos    *
 * para a impressao de um bloqueto.                                  *
 *                                                                   *
 * As seguintes rotinas e variaveis publicas estao definidas:        *
 *                                                                   *
 * cbPcodeFile      - Path do arquivo CMDFILE                        *
 *                                                                   *
 * cbPcodeLoad()    - Carrega programa externo                       *
 * cbPcodeExec()    - Gera tabela de offsets com valores default     *
 *                                                                   *
 * By: Deluan Perez, General Software, Rio 40 Graus                  *
 *********************************************************************
 *       (C) Copyright General Software Tecnologia 1994,95           *
 *********************************************************************
 * 27 Mar 1995 - DP primeira versao                                  *
 *********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "winprof.h"
#include "codbar.h"
#include "pcl.h"

char cbPcodeFile[20] = { 0 };

#define BLOCKSIZE  4096 /* para a funcao fcopy() */

static int   OFS_X   = 0;
static int   OFS_Y   = 0;

static COMMAND *form;

static int   GetFontId    OF((char *));
static int   LoadFile     OF((FILE *));
static int   LoadMacro    OF((int, char *));
static int   LoadFont     OF((char *));
static int   PrintXY      OF((int, int, char *));
static int   fcopy        OF((FILE *, FILE *, long, long));

/*--------------------------------------------------------------------*/
int cbPcodeLoad()

{ FILE *f;
  short size;

  /* Abre arquivo definido em cbPcodeFile */
  if (!cbPcodeFile[0] || (f = fopen(cbIniPath(cbPcodeFile), "rb")) == NULL)
     return -1;

  /* Pula cabecalho */
  fseek(f, 3 + (sizeof(short) * 2), 0);

  /* Carrega p-code */
  fread(&size, sizeof(size), 1, f);
  form = malloc(size * sizeof(COMMAND));
  fread(form, sizeof(COMMAND), size, f);

  /* Guarda posicao inicial da macro */
  sprintf(form->parm, "%ld", ftell(f));

  fclose(f);
  return OK;
}

/*--------------------------------------------------------------------*/
int cbPcodeExec(cmds, type)
int cmds;   /* 0 - Estaticos, 1 - Dinamicos, 2 - Tudo */
int type;   /* 0 - Form, 1 - Data, 2 - Form+Data */

{ COMMAND *fp;
  int      i, old_i, until, exec, just;
  struct { int b, e; } block[3];
/*  char    *s, *p;*/

  OFS_X = 0;
  OFS_Y = 0;

  if (cmds != 1) {
     /* Roda apenas os comandos estaticos */
     i = 0;
     while (form[i].cmd != C_STOPRUN) {
         fp  = &form[i];
         switch (fp->cmd) {
                case C_LOADMACRO:
                     LoadMacro(fp->x, fp->parm);
                     break;
                case C_LOADFONT:
                     LoadFont(fp->parm);
                     break;
         }
         i++;
     }
  }

  if (cmds != 0) {
     /* Roda apenas os comandos dinamicos */
     until = i = 0;
     exec  = 1;
     while (form[i].cmd != C_STOPRUN) {
         fp  = &form[i];
         if (exec || (!exec && fp->cmd == C_ENDPROC)) {
            switch (fp->cmd) {
                   case C_SELECTFONT:
                        SelectFont(fp->parm);
                        break;
                   case C_EXECMACRO:
                        if (type != 1) {
                           PrnCmd_n("&f", fp->x, 'Y');
                           PrnCmd_n("&f", 2, 'X');
                        }
                        break;
                   case C_OFS:
                        OFS_X = fp->x;
                        OFS_Y = fp->y;
                        break;
                   case C_BEGINPROC:
                        block[fp->x].b = i;
                        exec = fp->y;
                        break;
                   case C_ENDPROC:
                        block[fp->x].e = i;
                        exec = 1;
                        break;
                   case C_EXECPROC:
                        old_i = i;
                        i     = block[fp->x].b;
                        until = block[fp->x].e;
                        exec  = 1;
                        break;
                   case C_RESETMARGINS: /* Reset side margins */
                        fprintf(cbPrnFile, "\x1b%d", 9);
                        break;
                   case C_TOPMARGIN:    /* Top margin = fp->x */
                        PrnCmd_n("&l", fp->x, 'E');
                        break;
                   case C_PRINTCALC: /* Imprime a linha digitavel */
                        if (type == 0)
                           break;
/*
                        s = cbCalcNumeric();
                        s[37] = 0;
                        p = trim(&s[38]);
                        PrintXY(fp->x, fp->y, s);
*/
                        PrintXY(fp->x, fp->y, cbCalcNumeric());
                        break;
                   default:  /* Imprime dados da boleta */
                        if (fp->cmd >= 0 && type != 0)
                           if ((fp->x + fp->y >= 0) && (f_value(fp->cmd) != NULL)) {
                              PrintXY((fp->x), (fp->y), NULL);
                              just = atoi(fp->parm);
                              fprintf(cbPrnFile, "%*s", just, f_value(fp->cmd));
                           }
                        break;
            }
         }
         if (++i == until) {
            until = 0;
            i     = old_i+1;
         }
     }
  }

  return 0;
}


/*--------------------------------------------------------------------*/
static int PrintXY(x, y, str)
int x;
int y;
char *str;

{
  /* Converte milimetros em dots */
  x = m2d(OFS_X+x);
  y = m2d(OFS_Y+y);

  /* Posiciona impressao */
  PrnXY(x, y);

  /* Imprime texto */
  if (str != NULL)
     fprintf(cbPrnFile, str);

  return 0;
}

/*-----------------------------------------------------------------*/
static int LoadMacro(macroid, mpos)
int   macroid;
char *mpos;

{ FILE *fp;
  long  msize;
  short mid;

  /* Abre arquivo pcode */
  if ((fp = fopen(cbIniPath(cbPcodeFile), "rb")) == NULL)
     return 1;

  /* Start Macro Definition */
  PrnCmd_n("&f", macroid, 'Y');
  PrnCmd_n("&f", 0, 'X');

  /* Carrega a macro */
  fseek(fp, atol(mpos), 0);
  fread(&mid, sizeof(mid), 1, fp);
  fread(&msize, sizeof(msize), 1, fp);
  fcopy(cbPrnFile, fp, -1L, msize);
  fclose(fp);

  /* Stop Macro Definition */
  PrnCmd_n("&f", 1, 'X');

  /* Make Temporary */
  PrnCmd_n("&f", macroid, 'Y');
  PrnCmd_n("&f", 9, 'X');

  return 0;
}

/*-----------------------------------------------------------------*/
static int LoadFont(fontname)
char *fontname;

{ FILE *fp;
  char *path;

  /* Carrega fonte */
  path = cbIniPath(fontname);
  if ((fp = fopen(path, "rb")) == NULL)
     return 1;

  LoadFile(fp);
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
static int LoadFile(src)
FILE *src;
{
  return fcopy(cbPrnFile, src, -1L, -1L);
}

/*--------------------------------------------------------------------*/
static int fcopy(dest, src, posi, length)
FILE *dest, *src;
long posi, length;

{ char  *buf;
  size_t bufs, r, done = 0;
  long   ft, posf = -1L;

  bufs = BLOCKSIZE;
  if ((buf = malloc(bufs)) == NULL) return -1;

  if (posi >= 0L)
     fseek(src, posi, 0);
  else
     posi = ftell(src);

  if (length >= 0L)
     posf = posi + length;

  do {
     if (posf >= 0L) {
        ft = ftell(src);
        r = (ft+bufs <= posf) ? bufs : (done = (posf - ft));
     }
     else r = bufs;
     r = fread(buf, 1, r, src);
     if (r)
        fwrite(buf, 1, r, dest);
  } while (r > 0L && !done);

  fflush(dest);
  free(buf);
  return 0;
}

