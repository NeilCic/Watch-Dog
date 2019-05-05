#define _POSIX_SOURCE
#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#include "scheduler.h"
#include "task.h"
#include "wd_utils.h"

#define INVALID_PID -2
#define EXEC_FAIL -1
#define SET_FAIL -1
#define SUCCESS 0
#define OFF 0
#define FAIL 1
#define ON 1
#define YES 1
#define SETENV_FAIL 2
#define WATCHDOG_NAME "proc_wd.out"
#define PROG_NAME "./a.out"

#define IS_CHILD_PROCESS(pid) (0 == (pid))

volatile sig_atomic_t scheduler_flag = ON;
pthread_t thread = 0;
pid_t other_proc;
size_t missed_signals = 0;
char buffer[100];
char user_prog[100] = PROG_NAME;

time_t last_received_sig = 0;
time_t now;

void SignalHandler1(int sig) {
	(void)sig;
	time(&last_received_sig);
}

void SignalHandler2(int sig) {
	(void)sig;
	scheduler_flag = OFF;
}

int DNR(void) {
	scheduler_flag = OFF;
	kill(other_proc, SIGKILL);
	
	return SUCCESS;
}

void WDFork(wd_info_t *wd_info) {
	pid_t pid = 0;

	assert(NULL != wd_info);

	pid = fork();
	if (IS_CHILD_PROCESS(pid)) {
		if (EXEC_FAIL == execv(wd_info->watch_dog, wd_info->argv)) {
			printf("failed opening proc_wd\n");
			perror("execv");
		}
	}
	
	other_proc = pid;
}

int Transmit_Signal(void *wd_info) {
	sleep(((wd_info_t *)wd_info)->time_between_signals);
	kill(other_proc, SIGUSR1);
	printf("transmitting from %d to %d\n", getpid(), other_proc);

	return scheduler_flag;
}

int Check_Received_Signal(void *args) {
	wd_info_t *wd_info = args;
	(void)args;

	assert(NULL != args);

	time(&now);
	if (difftime(now, last_received_sig) > wd_info->time_between_signals) {
		if (missed_signals < wd_info->signal_num) {
			++missed_signals;
		} else {
			missed_signals = 0;

			if (YES == wd_info->is_user_prog) {
				WDFork(wd_info);
			} else {
				scheduler_flag = OFF;
				if (EXEC_FAIL == execv(user_prog, wd_info->argv)) {
					printf("failed opening user prog\n");
					perror("execv");
				}
			}
		}
	}

	return scheduler_flag;
}

static void Setup_SigHandlers(struct sigaction sa1, struct sigaction sa2) {
	sa1.sa_handler = &SignalHandler1;
	sa2.sa_handler = &SignalHandler2;
	sigaction(SIGUSR1, &sa1, NULL);
	sigaction(SIGUSR2, &sa2, NULL);
}

static void Transceiver(wd_info_t *wd_info) {
	sch_t *scheduler = NULL;
	struct timeval timing = {0};
	scheduler = SchCreate();
	SchAdd(scheduler, Transmit_Signal, wd_info, timing);
	SchAdd(scheduler, Check_Received_Signal, wd_info, timing);
	SchRun(scheduler);
}

void *WDRun(void *params) {
	struct sigaction sa1 = {0};
	struct sigaction sa2 = {0};
	wd_info_t *wd_info = params;

	assert(NULL != wd_info);

	Setup_SigHandlers(sa1, sa2);

	if (SUCCESS != kill(other_proc, 0)) {
		WDFork(wd_info);
	}

	Transceiver(wd_info);

	return NULL;
}

static void Setup_WD_Info(wd_info_t *wd_info, size_t time_between_signals,
						size_t sig_num, const int argc, const char *argv[]) {
	wd_info->argc = argc;
	wd_info->argv = (char **)argv;
	wd_info->watch_dog = WATCHDOG_NAME;
	wd_info->is_user_prog = YES;
	wd_info->time_between_signals = time_between_signals;
	wd_info->signal_num = sig_num;
	other_proc = INVALID_PID;
}

int KeepMeAlive(const int argc, const char *argv[], size_t time_between_signals,
													size_t sig_num) {
	pthread_attr_t attr = {0};
	wd_info_t info = {0};

	assert(time_between_signals > 0);
	assert(signal_num > 0);

	sprintf(buffer, "%ld", time_between_signals);
	if (SET_FAIL == setenv("time_between_signals", buffer, 1)) {
		return SETENV_FAIL;
	}

	sprintf(buffer, "%ld", sig_num);
	if (SET_FAIL == setenv("sig_num", buffer, 1)) {
		return SETENV_FAIL;
	}

	Setup_WD_Info(&info, time_between_signals, sig_num, argc, argv);

	pthread_create(&thread, &attr, WDRun, &info);

	return SUCCESS;
}
