#ifndef LIBMYSYSLOG_H
#define LIBMYSYSLOG_H
#define DEBUG 0
#define INFO 1
#define WARN 2
#define ERROR 3
#define CRITICAL 4
int mysyslog(const char* message, int severity, int source, int fmt, const char* log_path);
#endif // LIBMYSYSLOG_H
