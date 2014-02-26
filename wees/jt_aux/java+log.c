#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <malloc.h>

void log(const char* msg1, const char* msg2)
{
  time_t t;
  char *ct;
  FILE *log;

  time(&t);
  ct = ctime(&t);
  ct[strlen(ct) - 1] = 0;
  log = fopen("java.log.txt", "a");
  if (log == 0) {
    printf("*** Unable to open java.log.txt ! ***");
    return;
  }
  fprintf(log, "[%s] %s%s\n", ct, msg1, msg2);
  fclose(log);
}

int main(int argc, char** argv)
{
  int   i, len=0, err;
  char *cmd_line, *pp;

  pp = strrchr(argv[0], '\\');
  if (pp != 0)
    pp[1] = 0;
  else
    argv[0][0] = 0;

  for (i = 0; i < argc; i++) {
    len += strlen(argv[i]);
  }
  cmd_line = alloca(len + argc + 100);

  pp = cmd_line;
  pp += sprintf(pp, "%s%s ", argv[0], "java");
  for (i = 1; i < argc; i++) {
    pp += sprintf(pp, "%s ", argv[i]);
  }

  log("executing ", cmd_line);
  if (0 != (err = system(cmd_line))) {
    log("*** error ", cmd_line);
    return err;
  }

  return 0;
}