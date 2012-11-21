#ifndef COMMANDINTERPRETER_H
#define COMMANDINTERPRETER_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QList>
#include <QMetaMethod>

class CommandInterpreter : public QObject
{
  Q_OBJECT
public:
  explicit CommandInterpreter(QObject *parent = 0);

  typedef void (CommandInterpreter::*CommandMethod)(QList<QString>);

  void listCommands();

  friend class Console;

signals:
  void write(QString s);
  void writeAsync(QString s);
  void exit();

public slots:
  void handleString(QString s);

  void cmdExit();
  void cmdHelp(QString cmd);

protected:
  QMap<QString, QMetaMethod> dispatch;
};


#endif // COMMANDINTERPRETER_H
