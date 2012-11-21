#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#include "diskio.h"

FILE *drvFile = NULL;


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

int diskio_init(const char *file) {
  if (drvFile != NULL) {
    return 0;
  }

  drvFile = fopen(file, "r+");
  if (!drvFile) {
    return 0;
  } else {
    return 1;
  }
}

int diskio_close() {
  if (drvFile) {
    fclose(drvFile);
    drvFile = NULL;
  }
  return 1;
}

DSTATUS disk_initialize(BYTE drv) {
  if (drv > 0) {
    return STA_NODISK;
  }

  return 0;
}

DSTATUS disk_status(BYTE drv) {
  if (drv > 0 || drvFile == NULL) {
    return STA_NODISK;
  }

  return 0;;
}

DRESULT disk_read(BYTE drv, BYTE *buffer, DWORD sectorNumber, BYTE sectorCount) {
  if (drv > 0 || drvFile == NULL) {
    fprintf(stderr, "No opened drive %d\n", drv);
    return RES_ERROR;
  }
  if (fseek(drvFile, sectorNumber * 512, SEEK_SET)) {
    fprintf(stderr, "Could not seek to sector %d\n", sectorNumber);
    return RES_ERROR;
  }
  int ret;
  if ((ret = fread(buffer, 512, sectorCount, drvFile)) != sectorCount) {
    fprintf(stderr, "Could not read %d sectors starting at %d, read %d\n", sectorCount, sectorNumber, ret);
    return RES_ERROR;
  }
  return RES_OK;
}

DRESULT disk_write(BYTE drv, const BYTE *buffer, DWORD sectorNumber, BYTE sectorCount) {
  if (drv > 0 || drvFile == NULL) {
    fprintf(stderr, "No opened drive %d\n", drv);
    return RES_ERROR;
  }
  if (fseek(drvFile, sectorNumber * 512, SEEK_SET)) {
    fprintf(stderr, "Could not seek to sector %d\n", sectorNumber);
    return RES_ERROR;
  }
  if (fwrite(buffer, 512, sectorCount, drvFile) != sectorCount) {
    fprintf(stderr, "Could not write %d sectors\n", sectorCount);
    return RES_ERROR;
  }
  return RES_OK;
}

DRESULT disk_ioctl(BYTE drv, BYTE command, void *buffer) {
  if (drv > 0 || drvFile == NULL) {
    return RES_ERROR;
  }

  switch (command) {
  case CTRL_SYNC:
    fflush(drvFile);
    break;

  case GET_SECTOR_SIZE:
    *((WORD *)buffer) =  512;
    break;

  case GET_SECTOR_COUNT:
    fseek(drvFile, 0L, SEEK_END);
    DWORD sz = ftell(drvFile);
    *((DWORD *)buffer) = sz / 512;
    break;

  case CTRL_ERASE_SECTOR:
  {
    DWORD *ptr = (DWORD *)buffer;
    DWORD start = ptr[0];
    DWORD end = ptr[1];
    BYTE buf[512] = { 0 };
    DWORD i;
    for (i = start; i < end; i++) {
      DRESULT ret = disk_write(drv, buf, i, 1);
      if (ret != RES_OK) {
        return ret;
      }
    }
  }
    break;

  default:
    return RES_PARERR;
  }

  return RES_OK;
}


