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

  void cmdIoInit(QString filename);
  void cmdIoClose();

  void cmdShowFAT();
  void cmdHideFAT();

  void cmdCd(QString dir);
  void cmdLs();
  void cmdCwd();

  void cmdOpen(QString filename);
  void cmdClose();
  void cmdWrite(QString data);
  void cmdGets();
  void cmdSync();
  void cmdRead(unsigned long bytes);
  void cmdTell();
  void cmdSeek(unsigned long bytes);
  void cmdRewind();

  void cmdMv(QString src, QString dest);
  void cmdRm(QString filename);
  void cmdMkdir(QString dirname);

  void cmdGetFree();

  //    void cmdSetDebugLevel(int level);

  void sniffFATOutput(QByteArray);
  void sniffFATInput(QByteArray);

protected:
  FATFS fs;
  FIL file;
  bool init;
  bool mounted;
  bool fileOpened;
  bool showFAT;
  QString initFile;
};

#endif // FATCOMMANDINTERPRETER_H
