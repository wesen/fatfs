#include <iostream>
#include <string>

#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>

#include <QCoreApplication>
#include <QtGlobal>
#include <QFile>
#include <QDir>

#ifdef WIN32
#include <windows.h>
#endif

#include "console.h"

static int poll() {
  QCoreApplication::processEvents();
#if defined(WIN32)
  Sleep(100);
#else
  //    usleep(10000);
#endif
  return 0;
}

static Console *rl_console = NULL;

Console::Console(QString _name, CommandInterpreter *_interpreter) :
  stopping(false),
  name(_name),
  interpreter(_interpreter)
{
  QObject::connect(this, SIGNAL(read(QString)),
                   interpreter, SLOT(handleString(QString)));
  QObject::connect(interpreter, SIGNAL(write(QString)),
                   this, SLOT(write(QString)));
  QObject::connect(interpreter, SIGNAL(writeAsync(QString)),
                   this, SLOT(writeAsync(QString)));
  QObject::connect(interpreter, SIGNAL(exit()), this, SLOT(quit()));


  /** allow conditional parsing of the inputrc file */
  rl_readline_name = name.toAscii();
  /** tell the completer that we want a crack first. */
  rl_console = this;
  rl_attempted_completion_function = &Console::ConsoleCompletion;
  rl_event_hook = poll;
  rl_catch_signals = 0;
  // rl_set_keyboard_input_timeout(100000);

  using_history();
  historyFile = QDir::homePath() + QString("/.%1_history").arg(name);
  QFile file(historyFile);
  if (!file.exists()) {
    // create file
    file.open(QIODevice::ReadWrite);
    file.close();
  }
  read_history(historyFile.toAscii().constData());
  //    rl_bind_key('\t', readline_completion);
}

Console::~Console()
{
}

char **Console::ConsoleCompletion(const char *text, int start, int) {
  /* if this is the first word, complete with word names */
  if (start == 0) {
    return rl_completion_matches(text, Console::ConsoleCompleteCommands);
  } else {
    return NULL;
  }
}

static char *dupstr(QString &s) {
  QByteArray ascii = s.toAscii();
  char *res = (char *)malloc(ascii.size() + 1);
  strncpy(res, ascii.constData(), ascii.size());
  res[ascii.size()] = '\0';
  return res;
}

char *Console::ConsoleCompleteCommands(const char *text, int state) {
  if (rl_console == NULL) {
    return NULL;
  }

  static QList<QString>::iterator i;
  static QList<QString> keys;
  if (!state) {
    keys = rl_console->interpreter->dispatch.keys();
    i = keys.begin();
  }

  while (i != keys.end()) {
    QString s = *i++;
    if (s.startsWith(text)) {
      return dupstr(s);
    }
  }

  return NULL;
}

void Console::run()
{
  while (!stopping) {
    QCoreApplication::processEvents();
    char *s = readline(QString("%1> ").arg(name).toAscii());
    if (s && *s) {
      add_history(s);
      int ret;
      if ((ret = append_history(1, historyFile.toAscii().constData()))) {
        qDebug() << "Could not append to histoy file " << historyFile.toAscii().constData() << " " << strerror(ret);
      }
    }
    interpreter->handleString(QString(s));
    free(s);
  }

  QCoreApplication::quit();
}

void Console::write(QString string) {
  std::cout << string.toStdString();
}

void Console::writeAsync(QString string)
{
  int need_hack = (rl_readline_state & RL_STATE_READCMD) > 0;

  char *saved_line = NULL;
  int saved_point = 0;
  if (need_hack)
  {
    saved_point = rl_point;
    saved_line = rl_copy_text(0, rl_end);
    rl_save_prompt();
    rl_replace_line("", 0);
    rl_redisplay();
  }

  std::cout << "\r" << string.toStdString();

  if (need_hack)
  {
    rl_restore_prompt();
    rl_replace_line(saved_line, 0);
    rl_point = saved_point;
    rl_redisplay();
    free(saved_line);
  }
}

void Console::exit(int)
{
  stopping = true;
}

void Console::quit() {
  exit(0);
}
