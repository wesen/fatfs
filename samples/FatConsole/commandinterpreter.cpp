#include <stdexcept>

#include <QDebug>
#include <QStringList>

#include <QMetaObject>
#include <QMetaMethod>
#include <QFile>
#include <QDir>

#include "helpers.h"

#include "console.h"
#include "commandinterpreter.h"

CommandInterpreter::CommandInterpreter(QObject *parent) :
  QObject(parent)
{
  const QMetaObject *metaObject = this->metaObject();
  for (int i = 0; i < metaObject->methodCount(); i++) {
    QMetaMethod metaMethod = metaObject->method(i);
    //        qDebug() << "Method " << i << " " << metaMethod.signature();
    QString signature(metaMethod.signature());
    if (signature.startsWith("cmd")) {
      QString cmdName = signature.mid(3, signature.indexOf('(') - 3);
      cmdName[0] = cmdName[0].toLower();
      dispatch[cmdName] = metaMethod;
    }
  }

  // setup alias
  dispatch["quit"] = dispatch["exit"];
}

void CommandInterpreter::handleString(QString s) {
  QStringList args = s.split(QRegExp("\\s+"));
  if (!args.isEmpty()) {
    QString cmd = args.takeFirst();
    if (dispatch.contains(cmd)) {
      QMetaMethod method = dispatch[cmd];
      QList<QByteArray> types = method.parameterTypes();
      QList<QByteArray> names = method.parameterNames();

      try {
        if (args.size() < types.size()) {
          if (cmd == "help") {
            listCommands();
            return;
          } else {
            throw std::runtime_error(QString("Not enough arguments for command \"%1\"")
                                     .arg(cmd).toStdString());
          }
        }

        QVariantList converted;

        for (int i = 0; i < types.length(); i++) {
          QVariant::Type parameterType = QVariant::nameToType(types[i]);
          QVariant arg = QVariant(args[i]);
          if (!arg.canConvert(parameterType)) {
            throw std::runtime_error(QString("Cannot convert %1 to %2")
                                     .arg(args[i]).arg(QString(types[i]))
                                     .toStdString());
          }

          if (!arg.convert(parameterType)) {
            throw std::runtime_error(QString("Could not convert %1 to %2")
                                     .arg(args[i]).arg(QString(types[i]))
                                     .toStdString());
          }
          converted << arg;
        }

        QList<QGenericArgument> arguments;

        for (int i = 0; i < converted.size(); i++) {
          QVariant &argument = converted[i];
          QGenericArgument genericArgument(QMetaType::typeName(argument.userType()),
                                           const_cast<void*>(argument.constData()));
          arguments << genericArgument;
          //                    qDebug() << genericArgument.name() << " " << genericArgument.data();
        }


        bool ok = method.invoke(this, Qt::DirectConnection,
                                arguments.value(0),
                                arguments.value(1),
                                arguments.value(2),
                                arguments.value(3),
                                arguments.value(4),
                                arguments.value(5),
                                arguments.value(6),
                                arguments.value(7),
                                arguments.value(8),
                                arguments.value(9)
                                );

        if (!ok) {
          throw std::runtime_error(QString("Could not invoke %1").arg(cmd).toStdString());
        }
      } catch (std::runtime_error &e) {
        emit write(QString("Error: %1\n").arg(e.what()));
        cmdHelp(cmd);
      }
    } else {
      emit write("Unknown command " + cmd + "\n");
      listCommands();
    }
  }
}

void CommandInterpreter::listCommands() {
  QMap<QString, QMetaMethod>::iterator i;
  for (i = dispatch.begin(); i != dispatch.end(); i++) {
    cmdHelp(i.key());
  }
}

void CommandInterpreter::cmdHelp(QString cmd) {
  if (!dispatch.contains(cmd)) {
    emit write("No such command available: " + cmd + "\n");
    return;
  }

  QMetaMethod method = dispatch[cmd];
  QList<QByteArray> types = method.parameterTypes();
  QList<QByteArray> names = method.parameterNames();

  QString msg = QString("\t%1").arg(cmd);
  for (int i = 0; i < types.size(); i++) {
    msg += " " + names[i] + "(" + types[i] + ")";
  }
  emit write(msg + "\n");
}

void CommandInterpreter::cmdExit() {
  emit exit();
}

