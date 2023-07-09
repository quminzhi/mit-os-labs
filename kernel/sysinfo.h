struct sysinfo {
  uint64 freemem;   // amount of free memory (bytes)
  uint64 nproc;     // number of process
};

// dynamically linked
uint64 snproc();    // proc.c
uint64 sfreemem();  // kalloc.c
