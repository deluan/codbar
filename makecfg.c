/*
$History: $
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "winprof.h"
#include "codbar.h"

#include "banks.c"

static int WriteBanks    OF((FILE *, int));

/*--------------------------------------------------------------------*/
int main(argc, argv)
int argc;
char **argv;

{ FILE  *fp;
  short  version, i;

  if ((fp = fopen(CFGFILE, "wb")) == NULL)
     return 1;

  /* Grava assinatura */
  fprintf(fp, CFGFILE_ID);

  /* Grava Numero da versao */
  version = 0;
  fwrite(&version, sizeof(version), 1, fp);

  /* Grava Config. dos bancos */
  if (argc == 1) {
     printf("Adicionando TODOS os bancos:\n");
     WriteBanks(fp, 0);
  }
  else {
     printf("Adicionando bancos:\n");
     for (i = 1; i < argc; i++)
         WriteBanks(fp, atoi(argv[i]));
  }

  /* Fim da sessao dos bancos */
  version = 0;
  fwrite(&version, sizeof(version), 1, fp);

  /* Nome do cedente */
  fputc(0, fp);

  /* Grava Checksum */
  version = 0;
  fwrite(&version, sizeof(version), 1, fp);

  fclose(fp);

  return 0;
}

/*--------------------------------------------------------------------*/
static int WriteBanks(fp, bankcode)
FILE *fp;
int bankcode;

{ int i, len;
  FREEFIELD *ff;

  for (i = 0; i < sizeof(bancos)/sizeof(BANK); i++) {
      if (bankcode == 0 || bancos[i].code == bankcode) {
         printf("\t%d - %s", bancos[i].code, bancos[i].name);
         fwrite(&bancos[i].code, sizeof(bancos[i].code), 1, fp);
         len = strlen(bancos[i].name);
         fputc(len, fp);
         fwrite(&bancos[i].name, len, 1, fp);
         len = 0;
         ff = bancos[i].ff;
         do {
            len++;
         } while (ff++->id != FF_END);
         fputc(len, fp);
         fwrite(bancos[i].ff, sizeof(FREEFIELD), len, fp);
         printf("\n");
      }
  }
  return 0;
}
