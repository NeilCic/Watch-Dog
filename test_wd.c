#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "wd_utils.h"
#include "wd_api.h"

#define TIME 10
#define SLEEP 1

int main(int argc, char const *argv[]) {
	int i = 1;

	for (; i < TIME; ++i) {
		printf("%d\n", i);
		sleep(SLEEP);
	}
	/**************************************************************************/

	/*printf("original pid: %d\n", getpid());*/
	KeepMeAlive(argc, argv, 1, 2);

	for (; i < TIME * 3; ++i) {
		printf("%d\n", i);
		sleep(SLEEP);
	}

	DNR();

	/**************************************************************************/
	for (; i <= TIME * 4; ++i) {
		printf("%d\n", i);
		sleep(SLEEP);
	}

	/*
before test:
gc proc_wd.c  wd_utils.c ../../../ds/scheduler.c ../../../ds/task.c ../../../ds/uid.c ../../../ds/priority_queue.c ../../../ds/srt_list.c ../../../ds/dl_list.c -pthread -o proc_wd.out

for test:
gc test_wd.c  wd_utils.c ../../../ds/scheduler.c ../../../ds/task.c ../../../ds/uid.c ../../../ds/priority_queue.c ../../../ds/srt_list.c ../../../ds/dl_list.c -pthread

DE MASTA:
gc proc_wd.c  wd_utils.c ../../../ds/scheduler.c ../../../ds/task.c ../../../ds/uid.c ../../../ds/priority_queue.c ../../../ds/srt_list.c ../../../ds/dl_list.c -pthread -o proc_wd.out | gc test_wd.c  wd_utils.c ../../../ds/scheduler.c ../../../ds/task.c ../../../ds/uid.c ../../../ds/priority_queue.c ../../../ds/srt_list.c ../../../ds/dl_list.c -pthread
*/

	return 0;
}
