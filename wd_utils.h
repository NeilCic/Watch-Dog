#include <pthread.h>
#include "scheduler.h"

typedef struct wd_info_t {
	int argc;
	int is_user_prog;
	char **argv;
	char *watch_dog;
	size_t signal_num;
	double time_between_signals;
} wd_info_t;

void *WDRun(void *params);
