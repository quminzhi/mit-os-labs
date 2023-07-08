#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define NARGS 3

int main() {
  int pid, wstatus;
  char *argv[NARGS];

  argv[0] = "cat";
  argv[1] = "non-existed-file";
  argv[2] = 0;

  if (fork() == 0) {
    // child process:
    //  - redirect stdout and stderr
    //  - call exec
    close(1);                                 // close stdout
    open("redirect.out", O_RDWR | O_CREATE);  // redirect stdout
    close(2);                                 // close stderr
    dup(1);                                   // redirect stderr

    printf("INFO: ready to exec 'exec'\n");  // stdout
    exec("cat", argv);                       // stderr
    printf("exec: failed");
    exit(-1);
  }

  pid = wait(&wstatus);
  // no redirection in parent process
  printf("child process[%d]: exited with code %d\n", pid, wstatus);

  exit(0);
}