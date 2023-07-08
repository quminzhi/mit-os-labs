#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BUFSIZE 1
#define READ 0
#define WRITE 1

int main() {
  int n, pid;
  int p2c[2], c2p[2];
  char buf[BUFSIZE];

  pipe(p2c);
  pipe(c2p);

  if (fork() == 0) {
    // child process
    close(p2c[WRITE]);
    close(c2p[READ]);

    pid = getpid();
    // receive a byte and send it back
    if ((n = read(p2c[READ], buf, 1)) > 0) {
      printf("%d: received ping\n", pid);
      write(c2p[WRITE], buf, 1);
    }
    close(p2c[READ]);
    close(c2p[WRITE]);
    exit(0);
  }

  // parent process
  close(p2c[READ]);
  close(c2p[WRITE]);

  pid = getpid();
  // fire up
  write(p2c[WRITE], buf, 1);

  // block when no byte to read in the pipe
  if ((n = read(c2p[READ], buf, 1)) > 0) {
    printf("%d: received pong\n", pid);
  }
  close(p2c[WRITE]);
  close(c2p[READ]);

  wait(0);
  exit(0);
}