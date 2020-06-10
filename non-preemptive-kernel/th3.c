#include "common.h"
#include "scheduler.h"
#include "util.h"

static uint64_t start;  // Cycle counters
static uint64_t end;

void thread4(void)
{
    start = get_timer();
    do_yield();

    do_exit();  
}

void thread5(void)
{
    end = get_timer();
    char *message = "Cycles taken for do_yield: ";

    print_str(0, 0, message);
    print_int(0, strlen(message), end - start);

    do_exit();
}
