#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include "wd_utils.h"

pid_t other_proc;

int main(int argc, char const *argv[])
{
	wd_info_t info = {0};

	info.argc = argc;
	info.argv = (char **)argv;
	info.watch_dog = "proc_wd.out";
	info.is_user_prog = 0;
	info.time_between_signals = atoi(getenv("time_between_signals"));
	info.signal_num = atoi(getenv("sig_num"));
	other_proc = getppid();

	WDRun(&info);

	return 0;
}