#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

#define BUFSIZE 512

char *basename(char *path) {
  if (strlen(path) == 0) return 0;

  char *p;
  for (p = path + strlen(path) - 1; p >= path && *p != '/'; p--)
    ;
  p++;

  return p;
}

void find(char *spath, char *pattern) {
  char buf[BUFSIZE], *p, *name;
  int fd, n;
  struct dirent de;
  struct stat st;

  if ((fd = open(spath, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", spath);
    exit(1);
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", spath);
    close(fd);
    return;
  }

  switch (st.type) {
    case T_FILE:
    case T_DEVICE:
      name = basename(spath);
      if (strcmp(name, pattern) == 0) {
        printf("%s\n", spath);
      }
      break;
    case T_DIR:
      // search recursively
      if (strlen(spath) + 1 + DIRSIZ + 1 > sizeof(buf)) {
        printf("ls: path too long\n");
        break;
      }
      memset(buf, 0, sizeof(buf));
      strcpy(buf, spath);
      p = buf + strlen(buf);
      *p++ = '/';
      // read all dir entries
      while ((n = read(fd, &de, sizeof(de))) == sizeof(de)) {
        if (de.inum == 0) continue;  // IMPORTANT: all zero directory entry
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
          continue;
        }
        memmove(p, de.name, strlen(de.name));
        p[strlen(de.name)] = 0;
        find(buf, pattern);
      }
      break;
    default:
      fprintf(2, "find: unknown file type\n");
      break;
  }

  close(fd);
  return;
}

int main(int argc, char *argv[]) {
  char *spath, *pattern;

  if (argc != 3) {
    printf("Usage: find <search_path> <pattern>\n");
    exit(1);
  }

  spath = argv[1];
  pattern = argv[2];

  find(spath, pattern);
  
  exit(0);
}