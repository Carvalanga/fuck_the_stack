#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "log.h"

static FILE* logfile = NULL;
static int is_log_locked = 0;

LOG_ERROR log_open(const char* file_stream) {

    logfile = fopen(file_stream, "w");

    if (logfile == NULL) {
        fprintf(stderr, "ERROR: CAN NOT OPEN LOGFILE\n");
        return LOG_CANT_OPEN;
    }

    fputs("#####################################\n", logfile);
    fputs("#----------- LOG CREATED -----------#\n\n", logfile);

    return LOG_OK;
}

LOG_ERROR log_close() {

    if (logfile) {
        fputs("\n#----------- LOG CLOSED  -----------#\n", logfile);
        fputs("#####################################\n\n", logfile);

        int result = fclose(logfile);
        if (result == EOF) {
            fprintf(stderr, "ERROR: CAN NOT CLOSE LOGFILE\n");
            return LOG_CANT_CLOSE;
        }

        return LOG_OK;
    }
    else {
        fprintf(stderr, "ERROR: LOG FILE IS NOT OPENED\n");

        return LOG_DOESNT_EXIST;
    }
}

LOG_ERROR log_write(const char* format, ...) {

    if (logfile) {
        if (is_log_locked == 0) {

            va_list args = NULL;
            va_start(args, format);
            vfprintf(logfile, format, args);
            va_end(args);

            fprintf(logfile, "\n");
        }
        else {
            fprintf(stderr, "ERORR: CAN'T WRITE TO LOG, BECAUSE IT IS LOKED\n");
        }

        return LOG_OK;
    }
    else {
        fprintf(stderr, "ERROR: LOG FILE IS NOT OPENED\n");

        return LOG_DOESNT_EXIST;
    }
}

void log_lock() {
    is_log_locked = 1;
}

void log_unlock() {
    is_log_locked = 0;
}
