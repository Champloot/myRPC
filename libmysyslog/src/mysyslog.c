#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>
#include "mysyslog.h"

void mysyslog(int priority, const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    // Логирование в syslog
    openlog("myRPC", LOG_PID|LOG_CONS, LOG_USER);
    vsyslog(priority, format, args);
    closelog();
    
    // Дублирование в stderr
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    fprintf(stderr, "[%04d-%02d-%02d %02d:%02d:%02d] [%d] ",
            tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec, getpid());
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    
    va_end(args);
}
