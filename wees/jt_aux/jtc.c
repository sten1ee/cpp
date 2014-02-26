#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#define STR_JTPATH    "-JTPATH"
#define DEBUGIN(STM)  //STM

char* dupstr(char* str)
{
  if (strchr(str, ' ')) {
    int len = strlen(str) + 3;
    char *res = (char*)malloc(len);
    strcpy(res + 1, str);
    res[0] = res[len - 2] = '"';
    res[len - 1] = 0;
    return res;
  }
  return strcpy((char*)malloc(strlen(str) + 1),  str);
}

char* get_jt_path(int argc, char** argv)
{
  int i;
  char *ps;
  // initialize `jtpath' - the path where jt classes reside
  for (i = 1; i < argc; i++) {
    if (0 == strcmp(argv[i], STR_JTPATH)) {
      return dupstr(argv[i + 1]);
    }
  }
  ps = getenv(STR_JTPATH + 1);
  return ps ? dupstr(ps) : 0;
}

char* get_java_bin_path(void)
{
  char* ps = getenv("JAVA_HOME");
  if (ps) {
    char* res = (char*)malloc(strlen(ps) + 6);
    sprintf(res, (strchr(ps, '/') ? "%s/bin/" : "%s\\bin\\"), ps);
    return res;
  }
  return 0;
}

int main(int argc, char** argv)
{
  int   i, len=0, err;
  char *cmd_line, *pp, *pa, *jt_path, *java_bin_path;

  jt_path = get_jt_path(argc, argv);
  java_bin_path = get_java_bin_path();

  for (i = 1; i < argc; i++) {
    len += strlen(argv[i]);
  }
  len += argc;
  if (jt_path) {
    len += strlen(jt_path);
  }
  if (java_bin_path) {
    len += strlen(java_bin_path);
  }
  len += 30;

  cmd_line = (char*)alloca(len);
  if (java_bin_path) {
    pp = cmd_line + sprintf(cmd_line, "%s", java_bin_path);
    free(java_bin_path);
  }
  else {
    pp = cmd_line;
  }

  if (jt_path) {
    pa = pp + sprintf(pp, "java -cp %s jt.Proc", jt_path);
    free(jt_path);
  }
  else {
    pa = pp + sprintf(pp, "java jt.Proc");
  }

  for (i = 1; i < argc; i++) {
    char* arg = argv[i];
    if (0 == strcmp(arg, STR_JTPATH))
      i++;
    else
      pa += sprintf(pa, (strchr(arg, ' ') ? " \"%s\"" : " %s"), arg);
  }

  DEBUGIN(printf("Executing:\n%s\n", cmd_line));
  if (0 != (err = system(cmd_line))) {
    DEBUGIN(perror(""));
    return err;
  }

  pa = pp + sprintf(pp, "%s ", "javac");
  for (i = 1; i < argc; i++) {
    char* arg = argv[i];
    if (0 == strcmp(arg, STR_JTPATH)) {
      i++;
    }
  	else if (arg == strstr(arg, "-jt.")) {
      if (arg == strstr(arg, "-jt.include")
       || arg == strstr(arg, "-jt.sourcepath"))
        i++; // skip next argument too
    }
    else {
    	pa += sprintf(pa, "%s ", arg);
    }
  }

  DEBUGIN(printf("Executing:\n%s\n", cmd_line));
  if (0 != (err = system(cmd_line))) {
    DEBUGIN(perror(""));
    return err;
  }

  return 0;
}
