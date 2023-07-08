#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

/**
 * Simplified xargs: simulates xargs -n 1 in Linux, meaning there is at most one
 * command in argument list.
 */

#define STDIN_FILE_NO 0
#define BUFSIZE 512
#define MAXARG 16

// Do not count '\n'
int readline(int fd, char *line) {
  int n;
  char buf[BUFSIZE];

  n = 0;
  memset(buf, 0, sizeof(buf));

  while (read(fd, buf + n, 1) == 1) {
    if (buf[n] == '\n') break;
    n++;  // count character except '\n'
    if (n >= BUFSIZE) {
      fprintf(2, "xargs: readline overflow buf size\n");
      exit(1);
    }
  }
  memcpy(line, buf, n);
  return n;
}

int main(int argc, char *argv[]) {
  int pid, n;
  char line[BUFSIZE];
  char *arglist[MAXARG];

  if (argc < 2) {
    printf("usage: xargs <cmd [arg list]>\n");
    exit(1);
  }

  while (1) {
    memset(line, 0, sizeof(line));
    if ((n = readline(STDIN_FILE_NO, line)) == 0) {
      break;
    }

    memset(arglist, 0, sizeof(char *) * MAXARG);
    for (int i = 0; i < argc; i++) {
      arglist[i] = argv[i];
    }
    arglist[argc] = line;

    if ((pid = fork()) == 0) {
      exec(arglist[1], arglist + 1);
      printf("xargs: exec error\n");
      exit(1);
    }

    wait((int *)0);
  }
  
  exit(0);
}