#include <stdlib.h>
#include <stdio.h>

#include <readline/readline.h>
#include <readline/history.h>


#include "diskio.h"

#include "ff.h"

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
