#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>

#include <QCoreApplication>
#include <QDebug>

#include "w_process.h"
#include "w.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	bool customConfigName = false;
	if (argc > 1) 
	{
		int signal = getSignalFromCommandName(argv[1]);
		if (signal != 0)
			return sendSignal(getWorkingPid(), signal) == 0 ? 1 : 0;
		else
			customConfigName = true;
	}

	pid_t workingPid = getWorkingPid();
	if (workingPid == -1) 
	{
		printf("Cannot read pid file warden.pid\n");
		return 1;
	}
	else if (workingPid > 0 && isRunningPid(workingPid)) 
	{
		printf("Current working process pid %d\n", workingPid);
		return 0;
	}
	else if (!writePid())
	{
		printf("Cannot write pid file warden.pid\n");
		return 1;
	}
	qDebug() << argv[0] << "build date:" << __DATE__ << __TIME__;
	qDebug() << "Starting new visor process with pid" << getpid();
	Warden *w = Warden::getInstance();
	if (customConfigName)
		w->setConfig(argv[1]);
	else
		w->setConfig("wconfig.conf");
	w->restart();
	//setSignalHandler(WSignals::W_STOP_SIGNAL, &Warden::reciveSignal);
	setSignalHandler(WSignals::W_QUIT_SIGNAL, &Warden::reciveSignal);
	setSignalHandler(Warden::W_RESTART_SIGNAL, &Warden::reciveSignal);
	setSignalHandler(WSignals::W_RELOAD_SIGNAL, &Warden::reciveSignal);
	return a.exec();
}
