#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "safe_wrappers.h"
#include "stack.h"

int main() {

    const char logfile_location[] = "../_logfile.txt";
    log_open(logfile_location);

    log_close();

    return 0;
}
