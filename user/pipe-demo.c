#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NARGS 2

int main() {
  int p[2];
  char *argv[NARGS];

  argv[0] = "wc";
  argv[1] = 0;
  // make a pipe: p[0] for read, p[1] for write
  pipe(p);

  if (fork() == 0) {
    // redirect read and close pipe for child process
    close(0);
    dup(p[0]);
    close(p[0]);
    close(p[1]);

    exec(argv[0], argv);
    exit(-1);
  }

  close(p[0]);
  write(p[1], "hello pipe\n", 11);
  close(p[1]);

  exit(0);
}