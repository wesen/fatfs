#include <stdlib.h>
#include <stdio.h>

#include <readline/readline.h>
#include <readline/history.h>

#include <sys/time.h>
#include <time.h>
#include "diskio.h"

#include "ff.h"

DWORD get_fattime(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  struct tm *time = localtime(&tv.tv_sec);
  DWORD year = time->tm_year - 80;
  DWORD month = time->tm_mon + 1;
  DWORD day = time->tm_mday;
  DWORD hour = time->tm_hour;
  DWORD minute = time->tm_min;
  DWORD sec = time->tm_sec / 2;

  return ((year & 0x7f) << 25)
      | ((month & 0xF) << 21)
      | ((day & 0x1F) << 16)
      | ((hour & 0x1F) << 11)
      | ((minute & 0x3F) << 5)
      | (sec & 0x1F);
}

int diskio_init(char *file);
int diskio_close();

FATFS fs;

int main(int argc, char *argv[]) {
  int stopping = 0;
  int ret = 0;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s file\n", argv[0]);
    exit(1);
  }

  if (!diskio_init(argv[1])) {
    fprintf(stderr, "Could not open fat file\n");
    exit(1);
  }

  if ((ret = f_mount(0, &fs)) != FR_OK) {
    fprintf(stderr, "Could not mount drive: %d\n", ret);
    exit(1);
  }

  if ((ret = f_mkfs(0, 1, 0)) != FR_OK) {
    fprintf(stderr, "Could not format drive: %d\n", ret);
    exit(1);
  }

  rl_readline_name = "fat";
  using_history();

  while (!stopping) {
    char *s = readline("fat> ");
    if (s && *s) {
      add_history(s);
    }
  }

  diskio_close();

  return 0;
}
