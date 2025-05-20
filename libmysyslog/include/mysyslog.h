#ifndef MYSYSLOG_H
#define MYSYSLOG_H

#include <syslog.h>

void mysyslog(int priority, const char *format, ...);

#endif // MYSYSLOG_H
