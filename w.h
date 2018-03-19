#pragma once
#include "w_data.h"

class Warden : public QObject
{
	Q_OBJECT
	Warden();
	~Warden();
	static Warden instance;
	
	//log
	static QByteArray logBA;
	static char *log;
	static int timeout;
	static WList readConfig(QString path);

	int currentCommand;
	QString configPath;
	WList processes; 
	int start(WList &list);
	int stop(WList &list);
public:
	static Warden *getInstance();
	static int getTimeout();
	static void logOutput(QtMsgType type, const char *msg);
	static void reciveSignal(int signal);

	void setConfig(QString path);
	//void forceStop();
	void quit();
	void restart();
	void reload();
public slots:
	void handlePrcError(QProcess::ProcessError error);
	void handlePrcFinished(int code, QProcess::ExitStatus status);
};