#include <QtCore/QCoreApplication>
#include <QDebug>
#include <QSocketNotifier>

#include <stdio.h>
#include <signal.h>

#include "console.h"
#include "fatcommandinterpreter.h"
#include "qsignalcatcher.h"

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  int ret = 0;

  signalCatcher.init();

  QObject::connect(&signalCatcher, SIGNAL(sigint()), &a, SLOT(quit()));

  FatCommandInterpreter interpreter;
  Console console("fat", &interpreter);

  QObject::connect(&a, SIGNAL(aboutToQuit()), &console, SLOT(terminate()));

  console.run();

  return ret;
}
