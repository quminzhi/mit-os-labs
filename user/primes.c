#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

/**
 * pseudocode for sieve of Eratosthenes:
 * 
 * p = get a number from left neighbor
 * print p
 * loop:
 *   n = get a number from left neighbor
 *   if (p does not divide n)
 *       send n to right neighbor
 * 
 * 1. a generating process
 * 2. intermediate sieve processes
 */

#define READ 0
#define WRITE 1
#define MAXN 35

void sieve_process(int rpipe) {
  int p, n, num, pid;
  int np[2];

  // the last sieve: exit if no input from the last sieve
  // read block until two cases:
  //  - data to be written
  //  - all file descriptors referring to the write end to be closed
  if ((n = read(rpipe, &p, sizeof(int))) == 0) {
    close(rpipe);
    exit(0);
  }

  printf("prime %d\n", p);
  // create pipe communication
  pipe(np);
  if ((pid = fork()) == 0) {
    // next sieve process
    close(np[WRITE]);
    sieve_process(np[READ]);
    exit(0);
  }

  // sieve
  close(np[READ]);
  while ((n = read(rpipe, &num, sizeof(int))) > 0) {
    if (num % p != 0) {
      write(np[WRITE], &num, sizeof(int));
    }
  }
  close(np[WRITE]);
  close(rpipe);

  wait((int*)0);
  // exit in fork() when sieve processes are created 
  // printf("process[%d] closed\n", pid);
}

int main() {
  int n;
  int genpipe[2];
  pipe(genpipe);

  if (fork() == 0) {
    // child process
    close(genpipe[WRITE]);
    sieve_process(genpipe[READ]);
    // read end of genpipe is closed in sieve_process
    exit(0);
  }

  close(genpipe[READ]);
  for (int i = 2; i <= MAXN; i++) {
    if ((n = write(genpipe[WRITE], &i, sizeof(int))) != 4) {
      printf("main: failed to write to pipe\n");
      exit(1);
    }
  }
  close(genpipe[WRITE]);
  wait((int *)0);
  
  exit(0);
}