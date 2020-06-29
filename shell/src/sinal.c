#include <signal.h>

#include <common.h>


void INThandler(int sig)
{
	signal(SIGTSTP, SIG_DFL);
}
