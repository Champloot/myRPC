#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "libmysyslog.h"

int mysyslog(const char* message, int severity, int source, int fmt, const char* log_path) 
{
    FILE* file_handle = fopen(log_path, "a+");
    if (file_handle == NULL) return -1;
}
    time_t current_time;
    time(&current_time);
    char* time_str = ctime(&current_time);
    time_str[strcspn(time_str, "\n")] = 0;

    const char* severity_label;
    switch (severity) {
        case DEBUG:    severity_label = "DEBUG";    break;
        case INFO:     severity_label = "INFO";     break;
        case WARN:     severity_label = "WARN";     break;
        case ERROR:    severity_label = "ERROR";    break;
        case CRITICAL: severity_label = "CRITICAL"; break;
        default:       severity_label = "UNKNOWN";
    }

    if (fmt == 0) {
        fprintf(file_handle, "%s [%s] Source:%d | %s\n", 
                time_str, severity_label, source, message);
    } else {
        fprintf(file_handle, 
                "{\"time\":\"%s\",\"level\":\"%s\",\"source\":%d,\"msg\":\"%s\"}\n",
                time_str, severity_label, source, message);
    }

    fclose(file_handle);
    return 0;
}
