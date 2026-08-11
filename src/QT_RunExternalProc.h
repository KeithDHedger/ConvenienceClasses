
#ifndef _QT_RUNEXTERNALPROC_
#define _QT_RUNEXTERNALPROC_

#include "globals.h"

enum class stdErrOption{swallow,toFile,output,multiToFile};

class QT_RunExternalProc
{
	public:
		QT_RunExternalProc();
		~QT_RunExternalProc();

		QString						runCommands(void);
		QString						runCommandsInShell(QString commands);
		bool							setCommands(QStringList sl);

		void							setStdErrFileOption(stdErrOption what,QString path="",QIODeviceBase::OpenModeFlag opt=QIODeviceBase::Append);
		void							setStdOutFileOption(QString path,QIODeviceBase::OpenModeFlag opt=QIODeviceBase::Truncate);

	private:
		QVector<QStringList>			commandArgs;
		QVector<QProcess*>			procs;
		QString						stdErrFilePath="";
		QString						stdOutFilePath="";
		QIODeviceBase::OpenModeFlag	stdErrMode=QIODeviceBase::Append;
		QIODeviceBase::OpenModeFlag	stdOutMode=QIODeviceBase::Truncate;
		stdErrOption					stdErrwhat=stdErrOption::swallow;
};

#endif
