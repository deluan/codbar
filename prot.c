#include <stdio.h>
#include <string.h>

#include ".sum"

/*--------------------------------------------------------------*/
int ProtOk()

{ FILE *f;
  char tmp[128], cmd[128];
  int  sum, s, dummy;

  sprintf(tmp, "/tmp/cbXXXXXX");
  mktemp(tmp);

  sprintf(cmd, "uname -a | sum > %s", tmp);
  if (system(cmd))
     return -1;

  if ((f = fopen(tmp, "rt")) == NULL)
     return -2;

  fscanf(f, "%d %d", &sum, &dummy);
  fclose(f);
  unlink(tmp);

  sscanf(SUM, "%d %d", &s, &dummy);

  return !(sum == s);
}
