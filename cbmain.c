/*********************************************************************
 *                                                                   *
 * CODBAR - Impressao de Bloquetos de Cobranca com Codigo de Barra   *
 *                                                                   *
 * cbMain.c  O Modulo Principal                                      *
 *                                                                   *
 *********************************************************************
 * CODBAR e' um sistema de impressao de bloquetos de cobranca,       *
 * desenvolvido para ser uma ponte entre os sistemas de impressao    *
 * atuais e o novo padrao com codigo de barra, adotado pelo Banco    *
 * Central.                                                          *
 *                                                                   *
 * By: General Software Tecnologia                                   *
 *     Rio 40 Graus, Brasil.                                         *
 *                                                                   *
 *********************************************************************
 *       (C) Copyright General Software Tecnologia 1994              *
 *********************************************************************
 * 26 Jun 1994 - Primeira versao (1.00)                              *
 *********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "winprof.h"
#include "codbar.h"


/* Rotinas exclusivas a este modulo */

static int   ProcessInput   OF((char *));
static int   LoadForm       OF((FILE *));
static int   PrintForm      OF((int));
static int   Presents       OF((void));
static int   Usage          OF((void));


/* Possiveis opcoes de ativacao (ver usage()) */

#define OPTIONS "hlfdp:b:M:sn:"


typedef enum _task {
        TASK_BCODE,
        TASK_PFORM,
        TASK_BANKS,
        MAXTASK
} TASK;

enum _filter {
     FILT_DATA,
     FILT_FRAME,
     FILT_NONE
} FILTER;


/* Variaveis locais */

static int  maxforms = 0;    /* -M : numero maximo de boletas p/ imprimir */
static int  silent   = 0;    /* -s : exibe mensagens ? */
static char *num_doc = NULL; /* -n : documento p/ imprimir (NULL p/ todos) */

/*--------------------------------------------------------------------*/
int main(argc, argv)
int argc;
char **argv;

{ int   c;
  TASK  task      = TASK_BCODE;
  int   ret       = 0;

  opterr = 0;

  /* Se nao houve parametros, mostra help e termina */
  if (argc == 1) {
     Presents();
     Usage();
  }

  /* Processa parametros */
  while ((c = getopt(argc, argv, OPTIONS)) != EOF) {
        switch (c) {
           case 'p': cbIniPrinter   = strdup(optarg);   break;
           case 'b': cbCfgBank.code = atoi(optarg);     break;
           case 'l': task = TASK_BANKS;                 break;
           case 'f': task = TASK_PFORM;                 break;
           case 'd': cbIniNoFrame++;                    break;
           case 's': silent++;                          break;
           case 'M': maxforms = atoi(optarg);           break;
           case 'n': num_doc = strdup(optarg);          break;
           default:  Presents();
                     if (c != 'h' && !silent)
                        fputs("ERRO: Parametro(s) invalido(s)!\n\x07", stderr);
                     Usage();
        }
  }

  /* Mostra mensagem de copyright */
  if (!silent)
     Presents();

#ifndef dos
  /* Verifica se nao e' uma copia pirata */
  if (ProtOk()) {
     fputs("ERRO: Falha de autenticacao!\x07 Favor contactar:\n", stderr);
     fputs("      General Software Tecnologia - Tel. (021) 252-2510\n", stderr);
     exit(1);
  }
#endif

  /* Nome do arquivo de boletas deve ser o ultimo parametro */
  if (argv[optind] == NULL) {
     if (!silent) {
        fputs("ERRO: Arquivo de boletas nao especificado!\n\x07", stderr);
        Usage();
     }
     return 1;
  }

  /* Processa arquivos de configuracao */
  if ((ret = cbIniInit()) != 0) {
     if (!silent)
        switch (ret) {
           /* Se name = ????, faltou entrada no .cfg, senao faltou p-code */
           case ERR_NOBANK:
                if (!cbCfgBank.name[0])
                   strcpy(cbCfgBank.name, "??????");
                fprintf(stderr, "ERRO: Banco %03d (%s) nao cadastrado!\n\x07",
                                cbCfgBank.code, cbCfgBank.name);
                break;
           /* Faltou o arquivo .cfg */
           case ERR_ACCESS:
                fputs("ERRO: Arquivo de configuracao nao encontrado!\n\x07", stderr);
                break;
           /* Faltou alguma variavel no .ini */
           case ERR_INIT:
           default:
                fputs("ERRO: Problemas durante a inicializacao!\n\x07", stderr);
                break;
        }
     return 1;
  }

  if (!silent)
     fprintf(stderr, "Inicializando Impressao...\r");
  cbPrintInit();

  switch (task) {
     case TASK_BCODE: ret = ProcessInput(argv[optind++]);
                      break;
     case TASK_PFORM: ret = PrintForm(FILT_DATA);
                      break;
     case TASK_BANKS: break;
  }

  cbPrintTerm();

  return ret;
}

/*--------------------------------------------------------------------*/
static int Presents()

{ char aux[80];
  int  tab;

  if (silent)
     return 0;

  strcpy(aux, NAME);
  strcat(aux, " v");
  strcat(aux, VERSION);
  strcat(aux, " - ");
  tab = strlen(aux);

  fprintf(stderr, "%s%s.\n", aux, DESCRIPTION);
  fprintf(stderr, "%*c(c)%s, por %s.\n\n", tab, ' ', REVISION, AUTHOR);

  return 0;
}

/*--------------------------------------------------------------------*/
static int Usage()

{
  if (silent)
     return 0;

  fputs("\nUso:  codbar -h                - Mostra ajuda (esta tela)\n", stderr);
/*
  fputs("      codbar -l                - Lista os bancos disponiveis\n", stderr);
*/
  fputs("      codbar -f [opcoes]       - Imprime um formulario vazio\n", stderr);
  fputs("      codbar [opcoes] arq_bloq - Imprime os bloquetos do arquivo arq_bloq\n", stderr);
  fputs("\nOpcoes: -p printer : Saida sera' direcionada para o arquivo 'printer'\n", stderr);
/*
  fputs("        -c num     : Especifica numero de formularios a serem impressos\n", stderr);
*/
  fputs("        -b cod_bco : Utiliza configuracao para o banco especificado\n", stderr);
  fputs("        -n num_doc : Imprime somente o bloqueto especificado\n", stderr);
  fputs("        -d         : Imprime somente os dados (suprime formularios)\n", stderr);

  exit(1);

  return 1;
}

/*--------------------------------------------------------------------*/
static int ProcessInput(fname)
char *fname;

{ FILE *fp;
  int  filt;

  /* Abre arquivo de entrada */
  if ((fp = fopen(fname, "rb")) == NULL)
     return 1;

  filt = (cbIniNoFrame) ? FILT_FRAME : FILT_NONE;

  /* Processa todo o arquivo de entrada, quebrando-o em boletas, ate' EOF */
  while (!feof(fp)) {
        if (LoadForm(fp) == 0)
           if (PrintForm(filt))
              break;
  }

  if (!silent) fputc('\n', stderr);

  /* Fecha o arquivo de entrada */
  fclose(fp);

  return 0;
}

/*--------------------------------------------------------------------*/
static int LoadForm(fp)
FILE *fp;

{ int c;

  /* Simula processo de impressao */
  do {
     c = fgetc(fp);
     if (cbCaptChar(c) == 1)
        return 0;
  } while (c != EOF);

  cbCaptChar(EOF);
  return 0;
}

/*--------------------------------------------------------------------*/
static int PrintForm(filt)
int filt;

{ static int numbloq = 0;

  /* Se for o documento especificado, ou todos ... */
  if (num_doc == NULL || !strcmp(num_doc, f_value(F_Numero_Doc))) {
     /* Limita o numero de boletas em maxforms */
     if (++numbloq > maxforms && maxforms > 0)
        return 1;

     if (!silent) {
        fprintf(stderr, "Imprimindo boleta #%d          \r", numbloq);
        fflush(stderr);
     }

     /* Inicializa impressao */
     cbPrintBegin();

     /* Se necessario, imprime o formulario */
     if (filt != FILT_FRAME || filt == FILT_NONE)
        cbPrintFrame();

     /* Se necessario, imprime os dados */
     if (filt != FILT_DATA || filt == FILT_NONE)
        cbPrintData();

     /* Finaliza impressao */
     cbPrintEnd();

     if (!silent) fputc(7, stderr);
  }
  return 0;
}

