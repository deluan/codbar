#include <stdio.h>
#include <string.h>

int main(argc, argv)
int argc;
char **argv;

{ FILE *f;
  int ch;
  int col    = 0;
  int parnum = 0;

  if (argc == 1 || (f = fopen(argv[1], "rb")) == NULL) {
     printf( "typebin arquivo [/d] - mostra em decimal\n" );
     return 1;
  }

  if (!strncmp(argv[2], "/d", 2))
     parnum = 1;

  while (!feof(f)) {
         ch = fgetc(f);

         if (ch < 32)
            printf("%3d", ch);
         else if (parnum)
                 printf("%3d", ch);
              else
                 printf("%3c", ch);
         col += 3;

         if (col > 77) {
            printf("\n");
            col = 0;
         }
  }

  return 0;
}
