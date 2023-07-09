#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_trace(void) {
  struct proc *p = myproc();
  int no;

  if (argint(0, &no) < 0)
    return -1;
  p->trace = no;

  return 0;
}

uint64
sys_sysinfo(void) {
  uint64 freemem, nproc;
  struct proc *p = myproc();
  struct sysinfo *info;

  if (argaddr(0, (uint64 *)&info) < 0)
    return -1;

  freemem = sfreemem();
  nproc = snproc();

  // copy from kernel space to user space where info pointer locates
  if (copyout(p->pagetable, (uint64)&info->freemem, (char *)&freemem, sizeof(uint64)) < 0) 
    return -1;
  if (copyout(p->pagetable, (uint64)&info->nproc, (char *)&nproc, sizeof(uint64)) < 0)
    return -1;

  return 0;
}