#ifndef LOG_H
#define LOG_H

#define CALL_FROM __FILE__, __LINE__
#define $HERE printf("HERE - %s:%d\n", CALL_FROM)

enum LOG_ERROR {
    LOG_OK = 0,
    LOG_CANT_OPEN,
    LOG_CANT_CLOSE,
    LOG_DOESNT_EXIST
};

LOG_ERROR log_open(const char* file_stream);

LOG_ERROR log_close();

LOG_ERROR log_write(const char* format, ...) /*__attribute__ ((format (printf, 1, 2)))*/;

void log_lock();

void log_unlock();

#endif
