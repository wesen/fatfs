#ifndef HELPERS_H
#define HELPERS_H

#include <QString>

#define countof( array ) ( sizeof( array )/sizeof( array[0] ) )
void qSleep(int ms);

QString qHexdump(QByteArray a);

#endif // HELPERS_H
