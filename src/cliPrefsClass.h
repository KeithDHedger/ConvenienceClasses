
#ifndef _PREFSCLASS_
#define _PREFSCLASS_

#include <QCoreApplication>
#include <QDir>
#include <getopt.h>

class cliPrefsClass
{
	public:
		cliPrefsClass(QString pname="");
		~cliPrefsClass();

		QHash<int,QStringList>	prefsData;
		QStringList				extraCliArgs;

		bool						doCliArgs(int argc,char **argv,option longoptions[]);
		QStringList				getPrefValue(QString name);

	private:
		void						setPrefValue(QString name,QStringList val);
		void						appendStrPref(QString name,QString str);
};

#endif
