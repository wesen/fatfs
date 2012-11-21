#include "helpers.h"

#include "fatcommandinterpreter.h"

FatCommandInterpreter::FatCommandInterpreter(QObject *parent) :
    CommandInterpreter(parent),
  showFAT(false)
{
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
