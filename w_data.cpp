#include "W_data.h"
#include "W.h"
#include <QDebug>

WProcessData::WProcessData() : prc(0), pid(0)
{
	proxySingalsFlag = false;
	connect(&prc, SIGNAL(error(QProcess::ProcessError)), this, SLOT(proxyPrcError(QProcess::ProcessError)));
	connect(&prc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(proxyPrcFinished(int, QProcess::ExitStatus)));
}
bool WProcessData::operator==(const WProcessData &other)
{
	return (cmd == other.cmd) && (args == other.args);
}
void WProcessData::start()
{
	proxySingalsFlag = true;
    prc.start(cmd + " " + args);
    if (prc.waitForStarted(Warden::getTimeout())) 
    {
    	pid = prc.pid();
		qDebug() << "New process" << pid << cmd << args;
    }
	else
		qWarning() << "New process NOT started " << cmd << args;
}
void WProcessData::stop()
{
	proxySingalsFlag = true;
	qDebug() << "Stopping process" << pid << cmd << args;	
	prc.close();
	//prc.write("quit\n"); - feature only for fcgi tilemaker process
	if (!prc.waitForFinished(Warden::getTimeout()))
		prc.terminate();
}
void WProcessData::proxyPrcError(QProcess::ProcessError e)
{
	if (proxySingalsFlag)
		emit error(e);
}
void WProcessData::proxyPrcFinished(int code, QProcess::ExitStatus status)
{
	if (proxySingalsFlag)
		emit finished(code, status);
}