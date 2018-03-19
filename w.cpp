#include "w.h"
#include "w_process.h"
#include <QCoreApplication>
#include <QSettings>
#include <QSharedPointer>
#include <QTimer>
#include <QTime>
#include <QDebug>

char *Warden::log = 0;
int Warden::timeout = 10000;
QByteArray Warden::logBA;
Warden Warden::instance;

Warden::Warden()
{
	qInstallMsgHandler(Warden::logOutput);
	Warden::timeout = 10000;
	currentCommand = 0;
}
Warden::~Warden()
{
	//
}
void Warden::setConfig(QString path)
{
	configPath = path;
}
int Warden::start(WList &list) 
{
	int k = 0;
	for (int i = 0; i < list.size(); i++) 
	{
		connect(list[i].data(), SIGNAL(error(QProcess::ProcessError)),
			SLOT(handlePrcError(QProcess::ProcessError)), Qt::UniqueConnection);
		connect(list[i].data(), SIGNAL(finished(int, QProcess::ExitStatus)),
			SLOT(handlePrcFinished(int, QProcess::ExitStatus)), Qt::UniqueConnection);
		list[i]->start();
		k++;
	}
	return k;
}
int Warden::stop(WList &list) 
{
	for (int i = 0; i < list.size(); i++) 
	{
		list[i]->stop();
	}
	return list.size();
}
void Warden::quit() 
{
	qDebug() << "Quit";	
	stop(processes);
	QMetaObject::invokeMethod(QCoreApplication::instance(),
		"quit",
		Qt::QueuedConnection);
}
void Warden::restart()
{
	qDebug() << "Restart";
	stop(processes);
	processes = readConfig(configPath);
	start(processes);
}
void Warden::reload() 
{
	qDebug() << "Reload";
	WList forStart, forStop, same;
	forStart = readConfig(configPath);
	forStop = processes;
	for (int i = 0; i < forStart.size(); i++)
	{
		for (int j = 0; j < forStop.size(); j++)
		{
			if (forStart[i]->operator==(*(forStop[j].data()))) 
			{
				same.append(forStop[j]);
				forStop.removeAt(j);
				j--;
				forStart.removeAt(i);
				i--;
				break;
			}
		}
	}
	stop(forStop);
	start(forStart);
	processes = same + forStart;
}
void Warden::handlePrcError(QProcess::ProcessError error) 
{
	WProcessData *data = (WProcessData *)sender();
	if (!data)
		return;
	qWarning() << "Process" << data->prc.pid() << "error"  << error;
}
void Warden::handlePrcFinished(int code, QProcess::ExitStatus status)
{
	WProcessData *data = (WProcessData *)sender();
	if (!data)
		return;
	if (status == QProcess::NormalExit) 
	{
		qWarning() << "Process" << data->pid << "finished normaly with code"  << code;
		if (code == 22) 
		{
			//restart after timeout
			QTimer::singleShot(Warden::timeout, data, SLOT(start()));
			qDebug() << "Process" << data->pid << "will restart after timeout" << Warden::timeout;
		}
		return;
	}
	qWarning() << "Process" << data->pid << "crashed with code" << code;
	//else lets restart
	data->start();
}
//STATIC
WList Warden::readConfig(QString path) 
{
	WList list;
	qDebug() << "Loading config" << path;
	if (path.isEmpty()) 
		return list;

	QSettings settings(path, QSettings::IniFormat);
	settings.setIniCodec("UTF-8");
	settings.beginGroup("Processes");
	int size = settings.beginReadArray("prc");
	for (int i = 0; i < size; ++i) {
	    settings.setArrayIndex(i);
	    QSharedPointer<WProcessData> p(new WProcessData);
	    p->cmd = settings.value("cmd").toString();
	    p->args = settings.value("args").toString();
	    list.append(p);
	}
	settings.endArray();
	settings.endGroup();
	settings.beginGroup("Settings");
	
	Warden::timeout = 5000;
	if (settings.contains("Timeout")) 
		Warden::timeout = settings.value("Timeout").toInt();
	qDebug() << "Timeout is" << Warden::timeout;
	
	QString logPath = "w.log";
	if (settings.contains("Log"))
		logPath = settings.value("Log").toString();
	Warden::logBA = logPath.toLocal8Bit();
	qDebug() << "Now logging into" << logBA;
	Warden::log = Warden::logBA.data();

	settings.endGroup();
	return list;
}
Warden *Warden::getInstance()
{
	return &instance;
}
int Warden::getTimeout()
{
	return Warden::timeout;
}
void Warden::reciveSignal(int signal)
{
	if (instance.currentCommand != 0)
		return;
	instance.currentCommand = signal;
	// if (signal == WSignals::W_STOP_SIGNAL)
	// {
	// 	instance.forceStop();
	// 	return;
	// }
	if (signal == WSignals::W_QUIT_SIGNAL)
	{
		instance.quit();
		return;
	}
	else if (signal == WSignals::W_RESTART_SIGNAL)
		instance.restart();
	else if (signal == WSignals::W_RELOAD_SIGNAL)
		instance.reload();
	instance.currentCommand = 0;
}
void Warden::logOutput(QtMsgType type, const char *msg)
{
	std::string time = QTime::currentTime().toString("HH:mm:ss.zzz").toStdString();
	FILE *f = fopen(Warden::log, "a+");
	switch (type) {
		case QtDebugMsg:
			if (f)
				fprintf(f, "[%s] %s\n", time.c_str(), msg);
			else
				printf("[%s] %s\n", time.c_str(), msg);
			break;
		case QtWarningMsg:
			if (f)
				fprintf(f, "[%s][Warning] %s\n", time.c_str(), msg);
			else
				printf("[%s][Warning] %s\n", time.c_str(), msg);
			break;
		case QtCriticalMsg:
			if (f)
				fprintf(f, "[%s][Critical] %s\n", time.c_str(), msg);
			else
				printf("[%s][Critical] %s\n", time.c_str(), msg);
			break;
		case QtFatalMsg:
			if (f) 
			{
				fprintf(f, "[%s][FATAL] %s\n", time.c_str(), msg);
				fclose(f);
			}
			else
				printf("[%s][FATAL] %s\n", time.c_str(), msg);
			break;
			abort();
	}
	if (f)
		fclose(f);
}