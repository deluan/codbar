
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "winprof.h"  /* macro OF() */
#include "codbar.h"

#ifdef CLIENT_DIMARCO
  static char ini_bloq[100] = "\r\r";          /* Dimarco */
#else
  static char ini_bloq[100] = "\x1b\r\x1b";    /* Codbar Delphi */
//  static char ini_bloq[100] = "\x0d\x1b\x30";  /* Carioca */
#endif

int checkini   OF((char));
int printctrl  OF((char *));
int exit       OF((int));

/*-------------------------------------------------------*/
main(argc, argv)
int argc;
char **argv;

{ FILE *inp;
  char c, s[300];
  int num, pos, ini, start;

  fprintf(stderr, "Dump %s - %s, por %s\n", VERSION, REVISION, AUTHOR);
  if (argc == 1) {
     fprintf(stderr, "uso: dump arq_boleta > arq_dump\n\n");
     return 1;
  }

  inp = fopen(argv[1], "rb");
  if (inp == NULL) {
     fprintf(stderr, "dump: arquivo \"%s\" nao encontrado!\n\n", argv[1]);
     return 1;
  }

  fprintf(stderr, "\nini_bloq=\"");
  printctrl(ini_bloq);
  fputc('\"', stderr);
  fputc('\n', stderr);

  puts("");

  num = start = 0;
  pos = 1;

  do {
     c = fgetc(inp);
  } while (!feof(inp) && !checkini(c));

  if (feof(inp)) {
     puts("Inicio de bloqueto nao encontrado!");
     exit(1);
  }
  else {
     printf("\n<BOLETA #%d>\n", ++num);
  }

  while (!feof(inp)) {
        c = fgetc(inp);
        if (checkini(c)) {
	   printf("\n<BOLETA #%d>\n", ++num);
           pos = 1;
	   start = 0;
           continue;
        }
        if (start) {
           if (!isspace(c))
              s[pos-ini] = c;
           else {
              printf("%04d,%04d - %s\n", ini, ini+strlen(s)-1, s);
              start = 0;
           }
        }
        else if (c > 32) {
           start = 1;
           memset(s, 0, sizeof(s));
           ini   = pos;
           s[0]  = c;
        }

        pos++;
  }
  puts("");

  fclose(inp);

  return 0;
}

/*-------------------------------------------------------*/
int checkini(c)
char c;

{ static int  p = 0;

  if (ini_bloq[p] == c) {
     if (++p == strlen(ini_bloq)) {
	p = 0;
	return 1;
     }
  }
  else p = 0;

  return 0;
}

/*-------------------------------------------------------*/
int printctrl(s)
char *s;

{ char *p;

  for (p = s; *p; p++) {
      if (iscntrl(*p)) {
         fputc('^', stderr);
         fputc(*p+64, stderr);
      }
      else
         fputc(*p, stderr);
  }

  return 0;
}
