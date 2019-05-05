/*
This function makes sure the program gets "revived" incase it fails until the 
DNR function is called (see below).
Arguements: 
	time_between_signals - how much time between a signal is sent to and from
	the watch dog program.
	signal_num - how many missed signals the watch dog can tolerate before
	reviving user program (or the watch dog itself).
Return value - success = 0 or fail != 0.
*/
int KeepMeAlive(const int argc, const char *argv[], size_t time_between_signals, 
													size_t signal_num);

/*
	Once this function is called the watch dog ceases to care for the program,
	if it crashes, the watch dog will not revive it.
*/
int DNR(void);
