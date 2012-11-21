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
  showFAT(false)
{
  registerCommands();
}

void FatCommandInterpreter::cmdExit() {
  cmdClose();
  CommandInterpreter::cmdExit();
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
  int ret;
  if ((ret = f_mount(0, &fs)) != FR_OK) {
    emit write("Could not mount drive 0\n");
  } else {
    emit write("Mounted drive 0\n");
  }
}

void FatCommandInterpreter::cmdUnmount() {

}

void FatCommandInterpreter::cmdFormat() {
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

void FatCommandInterpreter::cmdInit(QString filename) {
  if (!diskio_init(filename.toAscii())) {
    emit write("Could not initialize " + filename + "\n");
  } else {
    emit write("Initialized with file " + filename + "\n");
  }
}

void FatCommandInterpreter::cmdClose() {
  diskio_close();
}

void FatCommandInterpreter::cmdShowFAT() {
  showFAT = true;
}

void FatCommandInterpreter::cmdHideFAT() {
  showFAT = false;
}
