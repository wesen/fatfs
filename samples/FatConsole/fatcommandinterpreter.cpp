#include <QFile>
#include <QString>
#include <QByteArray>

#include "helpers.h"

#include "fatcommandinterpreter.h"

extern "C" {
int diskio_init(const char *file);
int diskio_close();
}

FatCommandInterpreter::FatCommandInterpreter(QObject *parent) :
    CommandInterpreter(parent),
  init(false),
  mounted(false),
  fileOpened(false),
  showFAT(false)
{
  registerCommands();
}

static const char *getFatFsError(FRESULT ret) {
  switch (ret) {
  case FR_OK:
    return "Success";

  case FR_DISK_ERR:
    return "Unrecoverable lower IO disk error";

  case FR_INT_ERR:
    return "Insanity in the internal process";

  case FR_NOT_READY:
    return "Drive is not ready";

  case FR_NO_FILE:
    return "Could not find file";

  case FR_NO_PATH:
    return "Could not find path";

  case FR_INVALID_NAME:
    return "Invalid path name";

  case FR_DENIED:
    return "Permission denied";

  case FR_EXIST:
    return "Object already exists";

  case FR_INVALID_OBJECT:
    return "Invalid object";

  case FR_WRITE_PROTECTED:
    return "Media is write protected";

  case FR_NOT_ENABLED:
    return "Work area for the drive has not been mounted";

  case FR_NO_FILESYSTEM:
    return "No valid FS on the drive";

  case FR_MKFS_ABORTED:
    return "MKFS has been aborted";

  case FR_TIMEOUT:
    return "Timeout";

  case FR_LOCKED:
    return "File access rejected by file sharing control";

  case FR_NOT_ENOUGH_CORE:
    return "Not enough core memory for the operation";

  case FR_TOO_MANY_OPEN_FILES:
    return "Too many open files";

  case FR_INVALID_PARAMETER:
    return "invalid parameter";

  default:
    return "Unknown error code";
  }
}

void FatCommandInterpreter::cmdExit() {
  cmdIoClose();
  CommandInterpreter::cmdExit();
}

#define CHECK_MOUNTED() \
  if (!mounted) { \
    emit write("Please mount a drive first\n"); \
    return; \
  }

#define CHECK_FILE() \
  if (!fileOpened) { \
    emit write("Please open a file first\n"); \
    return; \
  }

#define ERROR_MSG(s, ret) \
  emit write(QString("%1: %2\n").arg(s).arg(getFatFsError(ret)))

void FatCommandInterpreter::cmdCd(QString dir) {
  CHECK_MOUNTED();
  FRESULT ret = f_chdir(dir.toAscii());
  if (ret != FR_OK) {
    ERROR_MSG("Could not change directory", ret);
  } else {
    emit write("Changed directory\n");
  }
}

void FatCommandInterpreter::cmdLs() {
  CHECK_MOUNTED();
  DIR dir;
  FILINFO fno;
  FRESULT ret = f_opendir(&dir, ".");
  if (ret != FR_OK) {
    ERROR_MSG("Could not open current directory", ret);
    return;
  }
  for (;;) {
    ret = f_readdir(&dir, &fno);
    if (ret != FR_OK || fno.fname[0] == 0) {
      break;
    }
    if (fno.fattrib & AM_DIR) {
      emit write(QString("%1/\n").arg(fno.fname));
    } else {
      emit write(QString("%1\n").arg(fno.fname));
    }
  }
}

void FatCommandInterpreter::cmdCwd() {
  CHECK_MOUNTED();
  TCHAR buf[256];
  FRESULT ret = f_getcwd(buf, 256);
  if (ret != FR_OK) {
    ERROR_MSG("Could not get current directory", ret);
    return;
  }
  emit write(QString("Current directory: %1\n").arg(buf));
}

void FatCommandInterpreter::cmdSync() {
  CHECK_FILE();
  FRESULT ret = f_sync(&file);
  if (ret != FR_OK) {
    ERROR_MSG("Could not sync file", ret);
  } else {
    emit write("Synced data to file\n");
  }
}

void FatCommandInterpreter::cmdOpen(QString filename) {
  const TCHAR *_filename = filename.toAscii();
  if (fileOpened) {
    cmdClose();
  }
  FRESULT ret = f_open(&file, _filename, FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
  if (ret != FR_OK) {
    ERROR_MSG("Could not open file", ret);
  } else {
    emit write("File opened\n");
    fileOpened = true;
  }
}

void FatCommandInterpreter::cmdClose() {
  CHECK_FILE();
  FRESULT ret = f_close(&file);
  if (ret != FR_OK) {
    ERROR_MSG("Could not close file", ret);
  } else {
    emit write("File closed\n");
    fileOpened = false;
  }
}

void FatCommandInterpreter::cmdWrite(QString data) {
  CHECK_FILE();
  const TCHAR *string = data.toAscii();
  int ret = f_puts(string, &file);
  if (ret < 0) {
    emit write("Could not write to file\n");
  } else {
    emit write(QString("Wrote %1 bytes to file\n").arg(ret));
  }
}

void FatCommandInterpreter::cmdGets() {
  CHECK_FILE();
  TCHAR buf[1024];
  TCHAR *ptr = f_gets(buf, 1024, &file);
  if (ptr != NULL) {
    emit write(QString("read: %1\n").arg(buf));
  } else {
    emit write("Could not read from file\n");
  }
}

void FatCommandInterpreter::cmdRead(unsigned long bytes) {
  CHECK_FILE();
  TCHAR buf[bytes];
}

void FatCommandInterpreter::cmdTell() {
  CHECK_FILE();
  DWORD offset = f_tell(&file);
  emit write(QString("Current offset: %1\n").arg(offset));
}

void FatCommandInterpreter::cmdSeek(unsigned long bytes) {
  CHECK_FILE();
  FRESULT ret = f_lseek(&file, bytes);
  if (ret != FR_OK) {
    ERROR_MSG("Could not seek in file", ret);
  } else {
    emit write(QString("Seeked to %1 bytes\n").arg(bytes));
  }
}

void FatCommandInterpreter::cmdRewind() {
  cmdSeek(0);
}


void FatCommandInterpreter::cmdMv(QString src, QString dest) {

}

void FatCommandInterpreter::cmdRm(QString filename) {

}

void FatCommandInterpreter::cmdMkdir(QString dirname) {
  CHECK_MOUNTED();
  FRESULT ret = f_mkdir(dirname.toAscii());
  if (ret != FR_OK) {
    emit write("Could not create directory\n");
  } else {
    emit write(QString("Created directory %1\n").arg(dirname));
  }
}

void FatCommandInterpreter::cmdGetFree() {

}


void FatCommandInterpreter::sniffFATInput(QByteArray a) {
  if (showFAT) {
    emit writeAsync(QString("FAT IN :\n%1\n").arg(qHexdump(a)));
  }
}

void FatCommandInterpreter::sniffFATOutput(QByteArray a) {
  if (showFAT) {
    emit writeAsync(QString("FAT OUT:\n%1\n").arg(qHexdump(a)));
  }
}

void FatCommandInterpreter::cmdMount() {
  if (mounted) {
    cmdUnmount();
  }
  int ret;
  if ((ret = f_mount(0, &fs)) != FR_OK) {
    emit write("Could not mount drive 0\n");
  } else {
    mounted = true;
    emit write("Mounted drive 0\n");
  }
}

void FatCommandInterpreter::cmdUnmount() {
  mounted = false;
}

void FatCommandInterpreter::cmdFormat() {
  if (!mounted) {
    emit write("Please mount a drive first\n");
    return;
  }

  int ret;
  if ((ret = f_mkfs(0, 1, 0)) != FR_OK) {
    emit write("Could not format drive 0\n");
  } else {
    emit write("Formatted drive 0\n");
  }
}

void FatCommandInterpreter::cmdCreate(QString filename, int sectorCount) {
  QFile file(filename);
  if (file.exists()) {
    emit write("File already exists!\n");
    return;
  }
  if (!file.open(QIODevice::ReadWrite)) {
    emit write("Could not create file!\n");
    return;
  }
  QByteArray array(512 ,'\0');
  for (int i = 0; i < sectorCount; i++) {
    file.write(array);
  }
  file.close();
  emit write(QString("Created file %1 with %2 sectors\n").arg(filename).arg(sectorCount));
}

void FatCommandInterpreter::cmdIoInit(QString filename) {
  if (init) {
    cmdIoClose();
  }
  if (!diskio_init(filename.toAscii())) {
    emit write("Could not initialize " + filename + "\n");
  } else {
    emit write("Initialized with file " + filename + "\n");
    init = true;
    initFile = filename;
  }
}

void FatCommandInterpreter::cmdIoClose() {
  if (!init) {
    emit write("Please init a file first\n");
    return;
  }
  diskio_close();
  emit write("File closed\n");
  init = false;
}

void FatCommandInterpreter::cmdShowFAT() {
  showFAT = true;
}

void FatCommandInterpreter::cmdHideFAT() {
  showFAT = false;
}
