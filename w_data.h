#pragma once
#include <QList>
#include <QProcess>
#include <QSharedPointer>

class WProcessData : public QObject
{
	Q_OBJECT
	bool proxySingalsFlag;
public:
	WProcessData();
	QString cmd;
	QString args;
	QProcess prc;
	int pid;

	bool operator==(const WProcessData &other);
public slots:
	void start();
	void stop();

	void proxyPrcError(QProcess::ProcessError e);
	void proxyPrcFinished(int code, QProcess::ExitStatus status);
signals:
	void error(QProcess::ProcessError error);
	void finished(int code, QProcess::ExitStatus status);
};

typedef QList< QSharedPointer <WProcessData> > WList;//may be better QSET?