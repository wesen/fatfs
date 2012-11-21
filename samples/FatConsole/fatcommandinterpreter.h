#ifndef FATCOMMANDINTERPRETER_H
#define FATCOMMANDINTERPRETER_H

#include "commandinterpreter.h"

class FatCommandInterpreter : public CommandInterpreter
{
  Q_OBJECT
public:
  explicit FatCommandInterpreter(QObject *parent = 0);

signals:

public slots:
  //    void cmdMount();
  //    void cmdUnmount();

  //    void cmdShowFAT();
  //    void cmdHideFAT();

  //    void cmdSetDebugLevel(int level);

  void sniffFATOutput(QByteArray);
  void sniffFATInput(QByteArray);

protected:
  bool showFAT;

};

#endif // FATCOMMANDINTERPRETER_H
