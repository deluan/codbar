#include <stdio.h>
#include <unistd.h>

main(argc, argv)
int argc;
char **argv;

{ FILE *i;
  int   c, j, size, count = 0, delay;

  delay = atoi(argv[2]);

  i = fopen(argv[1], "rb");
  fseek(i, 0, SEEK_END);
  size = ftell(i);
  fseek(i, 0, SEEK_SET);

  while (!feof(i)) {
        c = fgetc(i);
        if (!(count++ % 1024)) {
           fprintf(stderr, "%s: %7.0f%%\r", argv[1], count/(double)size * 100);
           fflush(stderr);
        }
        for (j = 0; j < delay; j++);
        putchar(c);
  }

  fprintf(stderr, "%s: %7d%%\n", argv[1], 100);
  fclose(i);
}

