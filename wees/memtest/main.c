#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/timeb.h>

#include "memfuncs.h"


long  test(int nlaps, char* oper, memfunc_t memfunc, word_t* buf1, word_t* buf2, word_t bufsz)
{
  struct timeb  start, stop;
  int lap = nlaps;
  int millis, nbytes, mb_per_sec;

  memfunc(buf1, buf2, bufsz);
  ftime(&start);
  do {
    memfunc(buf1, buf2, bufsz);
  } while (0 < --lap);
  ftime(&stop);
  millis = (stop.time - start.time) * 1000 + (stop.millitm - start.millitm);

  nbytes = nlaps * bufsz;
  mb_per_sec = (int)(((double)nbytes * 1000) / ((double)millis * 1024 * 1024));
  printf("%s %dM/s\n", oper, mb_per_sec);
  return mb_per_sec;
}

int main(int argc, char** argv)
{
  word_t *buf, *ebuf;
  char* snbytes  = argv[1] ? argv[1] : "0";
  int   km       = toupper(snbytes[strlen(snbytes) - 1]);
  int   nlaps;
  int   numb     = atoi(snbytes);
  int   nbytes   = numb;
  if (km == 'K')
    nbytes *= 1024;
  else if (km == 'M')
    nbytes *= 1024 * 1024;
  else {
    nbytes -= nbytes % sizeof(word_t);
    km = ' ';
  }
  if (nbytes <= 0) {
    char* appname = strrchr(argv[0], '\\');
    if (appname == 0)
      appname = strrchr(argv[0], '/');
    if (appname == 0)
      appname = argv[0];
    else
      ++appname;
    if (strstr(appname, ".exe")) {
      *strstr(appname, ".exe") = 0;
    }
    printf("Usage: %s <block-size>\n", appname);
    printf("Sample invokations:\n");
    printf("\t%s 64   /* test blocks of size 64 bytes */\n", appname);
    printf("\t%s 4k   /* test blocks of size 4K bytes */\n", appname);
    printf("\t%s 8M   /* test blocks of size 8M bytes */\n", appname);
    return -1;
  }

  buf = (word_t*)malloc(nbytes);
  ebuf = buf + (nbytes / sizeof(word_t));

  nlaps = (1024 * 1024 * 1024) / nbytes;
  printf("%d %cBytes, %d laps\n", numb, km, nlaps);
  test(nlaps, "read ", read,  buf, ebuf, nbytes);
  test(nlaps, "write", write, buf, ebuf, nbytes);

  free(buf);

  return 0;
}
