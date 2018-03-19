#include "W_process.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

pid_t getWorkingPid() 
{
	FILE *pidfile = fopen(W_PID_FILE, "r");
	if (!pidfile)
		return 0;
	char buf[64];
	int size = fread((char *)buf, 1, 64, pidfile);
	fclose(pidfile);
	int pid = atoi(buf);
	return pid;
}
bool isRunningPid(pid_t pid) 
{
	return (kill(pid, 0) == 0) ? true : false;
}
bool writePid() 
{
	FILE *pidfile = fopen(W_PID_FILE, "w");
	if (!pidfile)
		return false;
	if (fprintf(pidfile, "%d", getpid()) <= 0)
		return false;
	fclose(pidfile);
	return true;
}
bool sendSignal(pid_t pid, int signal) 
{
	if (kill(pid, signal) != -1)
		return true;
	else 
	{
		printf("Send signal to %d failed\n", pid);
		return false;
	}	
}
bool setSignalHandler(int signal, signalHandlerFunc handler)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));   
    sa.sa_handler = handler;   
    sigemptyset(&sa.sa_mask);
    if (sigaction(signal, &sa, NULL) == -1)
	{
		printf("Set signal handler failed\n");
		return false;
	}
	return true;
}
int getSignalFromCommandName(char *name)
{
	//if (!strcmp(name, W_STOP))
	//	return WSignals::W_STOP_SIGNAL;
	if (!strcmp(name, W_QUIT))
		return WSignals::W_QUIT_SIGNAL;
	else if (!strcmp(name, W_RESTART))
		return WSignals::W_RESTART_SIGNAL;
	else if (!strcmp(name, W_RELOAD))
		return WSignals::W_RELOAD_SIGNAL;
	else
		return 0;
}