
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define FONTID_DEFAULT   "16385"
#define WRI_EXT          ".wri"
#define PRN_EXT          ".prn"
#define SFT_EXT          ".sft"

static int GetFontId   (char *);
static int GetTextSize (char *);
static int Prn2Sft     (char *);

/*---------------------------------------------------------*/
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

/*---------------------------------------------------------*/
static int GetTextSize(fname)
char *fname;

{ FILE *f;
  int  ret, c;

  strcat(fname, WRI_EXT);
  if ((f = fopen(fname, "rb")) == NULL) {
     return -1;
  }

  fseek(f, 128, 0);
  ret = 0;
  c = fgetc(f);
  while (c && c != EOF) {
        ret++;
        c = fgetc(f);
  }
  fclose(f);

  return ret;
}

/*---------------------------------------------------------*/
static int Prn2Sft(fname)
char *fname;

{ FILE *inp, *outp;
  char *s, iname[100], oname[100];
  int  trunc, c, fontid;
  long isize, ok2copy;
  char seq1[10], seq2[10];
  int  p1, p2;

  /* Determina o fontid */
  fontid = GetFontId(fname);

  /* Monta sequencias a serem substituidas */
  sprintf(seq1, "\x1b*c%sD", FONTID_DEFAULT); p1 = 0;
  sprintf(seq2, "\x1b(%sX",  FONTID_DEFAULT); p2 = 0;

  /* Stripa a extensao do arquivo */
  if ((s = strrchr(fname, '.')) != NULL)
     *s = 0;

  /* Monta nomes dos arquivos */
  sprintf(iname, "%s%s", fname, PRN_EXT);
  sprintf(oname, "%s%s", fname, SFT_EXT);

  /* Calcula o numero de bytes para "truncar" do final */
  if ((trunc = GetTextSize(fname)) == -1) {
     fprintf(stderr, "prn2sft: Nao pude abrir o arquivo %s!\x07\n", fname);
     return -1;
  }
  trunc += 3; /* FF + ESC E */

  /* Abre os arquivos */
  if ((inp = fopen(iname, "rb")) == NULL)
     return -1;
  if ((outp = fopen(oname, "wb")) == NULL)
     return -1;

  /* Acha o tamanho do arquivo .prn (entrada) */
  fseek(inp, 0, 2);
  isize = ftell(inp);
  fseek(inp, 0, 0);

  ok2copy = 0;
  c = fgetc(inp);
  while (c != EOF && isize > trunc) {
        if (ok2copy)
           fputc(c, outp);

        /* Procura por sequencias */
        if (seq1[p1] == c) {
           if (++p1 == strlen(seq1)) {
              if (ok2copy) {
                 fflush(outp);
                 fseek(outp, ftell(outp) - strlen(seq1), 0);
              }
              fprintf(outp, "\x1b*c%dD", fontid);
              ok2copy = 1;
              p1 = 0;
           }
        }
        else p1 = 0;
        if (seq2[p2] == c) {
           if (++p2 == strlen(seq2)) {
              fflush(outp);
              fseek(outp, ftell(outp) - strlen(seq2), 0);
              fprintf(outp, "\x1b(%dX", fontid);
              p2 = 0;
           }
        }
        else p2 = 0;

        isize--;
        c = fgetc(inp);
  }

  fclose(inp);
  fclose(outp);

  return 0;
}

/*---------------------------------------------------------*/
int main(argc, argv)
int argc;
char **argv;

{
  if (argc == 1) {
     fprintf(stderr, "Prn2Sft 1.00, por General Software Tecnologia Ltda.\n");
     fprintf(stderr, "\nuso: prn2sft <arquivo>[.prn]\n\n");
     exit(1);
  }

  return Prn2Sft(argv[1]);
}



