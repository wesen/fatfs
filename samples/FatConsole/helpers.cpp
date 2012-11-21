#include <QtGlobal>

#include <QString>
#include <QTextStream>

#include "helpers.h"

#ifdef Q_OS_WIN
#include <windows.h> // for Sleep
#else
#include <time.h>
#endif
void qSleep(int ms) {
#ifdef Q_OS_WIN
  Sleep(uint(ms));
#else
  struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
  nanosleep(&ts, NULL);
#endif
}


static void _hexdump(QTextStream &o, const unsigned char *buffer, unsigned long index) {
  unsigned long width = 16;
  unsigned long i;
  for (i = 0; i < index; i++) {
    hex(o) << qSetFieldWidth(2) << buffer[i] << qSetFieldWidth(0) << " ";
  }
  for (unsigned long spacer = index; spacer < width; spacer++) {
    o << "   ";
  }
  o << ": ";
  for (i = 0; i < index; i++) {
    if (buffer[i] < 32 || buffer[i] >= 127) {
      o << ".";
    } else {
      o << QChar(buffer[i]);
    }
  }
  o << "\n";
}


QString qHexdump(QByteArray a) {
  QString res;
  QTextStream o;
  o.setString(&res);
  o.setPadChar(QChar('0'));

  const unsigned char *ptr = (const unsigned char *)a.constData();

  for (long i = 0; i < a.size(); i += 16) {
    _hexdump(o, ptr + i, qMin((long int)16, a.size() - i));
  }

  o.flush();

  return res;
}

