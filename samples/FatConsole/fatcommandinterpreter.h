#ifndef FATCOMMANDINTERPRETER_H
#define FATCOMMANDINTERPRETER_H


#include "ff.h"
#include "diskio.h"

#include "commandinterpreter.h"

class FatCommandInterpreter : public CommandInterpreter
{
  Q_OBJECT
public:
  explicit FatCommandInterpreter(QObject *parent = 0);

signals:

public slots:
  void cmdExit();

  void cmdMount();
  void cmdUnmount();

  void cmdFormat();
  void cmdCreate(QString filename, int sectorCount);

  void cmdInit(QString filename);
  void cmdClose();

  void cmdShowFAT();
  void cmdHideFAT();

  //    void cmdSetDebugLevel(int level);

  void sniffFATOutput(QByteArray);
  void sniffFATInput(QByteArray);

protected:
  FATFS fs;
  bool showFAT;

};

#endif // FATCOMMANDINTERPRETER_H
