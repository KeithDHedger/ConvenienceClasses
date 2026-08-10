
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
		void							setStdErr(stdErrOption opt,QString path="");
		void							setStdErrFileOption(QIODeviceBase::OpenModeFlag opt);

	private:
		QVector<QStringList>			commandArgs;
		QVector<QProcess*>			procs;
		QString						stdErrFile="";
		QIODeviceBase::OpenModeFlag	append=QIODeviceBase::Append;
		stdErrOption					stdErrwhat=stdErrOption::swallow;
};

#endif
