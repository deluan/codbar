#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAXMIX  16
#define Random(num)  (int)((rand() / (float) RAND_MAX) * (num))

char mix[MAXMIX][9];

void CalcMix()

{ time_t t;
  int  i, j, p1, p2;
  char idx[] = "01234567";
  char c, aux[9];

  time(&t);
  srand(t);
  strcpy(aux, idx);

  for (i = 0; i < MAXMIX; i++) {
      for (j = 0; j <= Random(30); j++) {
          p1 = Random(8);
          do {
             p2 = Random(8);
          } while (p1 == p2);
          c = aux[p1];
          aux[p1] = aux[p2];
          aux[p2] = c;
      }
      strcpy(mix[i], aux);
  }
}

unsigned long MixUp(mid, num)
int mid;
unsigned long num;

{ char aux[9], out[9], i;
  unsigned long ret;

  sprintf(aux, "%lX", num);
  memset(out, 0, sizeof(out));
  for (i = 0; i < 8; i++)
      out[i] = aux[mix[mid][i]-48];
  sscanf(out, "%lX", &ret);
  return ret;
}


void main()

{ time_t t, old;
  unsigned long l;
  char   aux[16], mid;

  CalcMix();

  do {
     time(&t);
     if (old != t) {
        old = t;
        mid = Random(MAXMIX);
        l = MixUp(mid, t);
        printf("%X) %010ld - %lX - %+11ld - %08lX\n", mid, t, t, l, l);
     }
  } while (1);
}
