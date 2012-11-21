#ifndef QSIGNALCATCHER_H
#define QSIGNALCATCHER_H

#include <QObject>

class QSignalCatcher : public QObject
{
  Q_OBJECT
public:
  explicit QSignalCatcher(QObject *parent = 0);

  void init();
  void restore();

  void onSignal(int signal);

signals:
  void sigint();

public slots:

protected:
  void (*prev_sigint)(int);
};

extern QSignalCatcher signalCatcher;

#endif // QSIGNALCATCHER_H
