#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "safe_wrappers.h"
#include "stack.h"

int main() {

    const char logfile_location[] = "../_logfile.txt";
    log_open(logfile_location);

    STACK stack = {};

    stack_ctor(&stack, 18 ON_DEBUG(, "stack", __FILE__, __LINE__));

    for (int i = 0; i < 23; i++) {
        stack_push(&stack, i);
    }

    stack_element_t a = 0;

    for (int i = stack.item_number/2; i > 0; i--) {
        stack_pop(&stack, &a);
    }

    stack_dtor(&stack);

    log_close();

    return 0;
}
