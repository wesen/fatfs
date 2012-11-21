#include <stdio.h>

#include "diskio.h"

FILE *drvFile = NULL;

int diskio_init(char *file) {
  if (drvFile != NULL) {
    return 0;
  }

  drvFile = fopen(file, "a");
  if (!drvFile) {
    return 0;
  } else {
    return 1;
  }
}

int diskio_close() {
  if (drvFile) {
    fclose(drvFile);
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
    return RES_ERROR;
  }
  if (fseek(drvFile, sectorNumber * 512, SEEK_SET)) {
    return RES_ERROR;
  }
  if (fread(buffer, 512, sectorCount, drvFile) != sectorCount) {
    return RES_ERROR;
  }
  return RES_OK;
}

DRESULT disk_write(BYTE drv, const BYTE *buffer, DWORD sectorNumber, BYTE sectorCount) {
  if (drv > 0 || drvFile == NULL) {
    return RES_ERROR;
  }
  if (fseek(drvFile, sectorNumber * 512, SEEK_SET)) {
    return RES_ERROR;
  }
  if (fwrite(buffer, 512, sectorCount, drvFile) != sectorCount) {
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
    for (DWORD i = start; i < end; i++) {
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


