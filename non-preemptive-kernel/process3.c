#include "common.h"
#include "syslib.h"
#include "util.h"

static uint64_t start;  // Start time
static uint64_t end;  // End time
static _Bool ran_once = 0;  // To tell each execution apart


void _start(void)
{
    char *message = "Cycles taken for yield(): ";

    end = get_timer();

    if (!ran_once) {
        ran_once = 1;
        start = get_timer();
    }

    yield();

    print_str(0, 40, message);
    print_int(0, 40 + strlen(message), end - start);

    exit();
}
