#ifndef CONSOLE_H
#define CONSOLE_H

#include <QThread>
#include <QString>
#include <QDebug>

#include "commandinterpreter.h"

class Console : public QThread
{
  Q_OBJECT

public:
  Console(QString name, CommandInterpreter *);
  virtual ~Console();

  static char **ConsoleCompletion(const char *text, int start, int end);
  static char *ConsoleCompleteCommands(const char *text, int state);

  void run();
  void exit(int returnCode);

signals:
  void read(QString);

public slots:
  void write(QString);
  void writeAsync(QString);
  void quit();

protected:
  bool stopping;
  QString name;
  CommandInterpreter *interpreter;
  QString historyFile;
};

#endif // CONSOLE_H
