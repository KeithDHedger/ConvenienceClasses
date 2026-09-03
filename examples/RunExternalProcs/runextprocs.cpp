#if 0

if [[ ! "X$USEVALGRIND" = "X" ]];then
cat>ignorelibleaks<<EOF
{
   ignore_unversioned_libs
   Memcheck:Leak
   ...
   obj:*/lib*/lib*.so
}

{
   ignore_versioned_libs
   Memcheck:Leak
   ...
   obj:*/lib*/libQt.so.*
}

{
   ignorexcbwritev
   Memcheck:Param
   writev(vector[0])
   fun:writev
   obj:/usr/lib/libxcb.so.1.1.0
}

{
   qt6core_waitid_infop
   Memcheck:Param
   waitid(infop)
   fun:syscall
   obj:*/lib*/libQt*
}

EOF

	case $USEVALGRIND in
		1)
			VALGRIND="valgrind"
			;;
		2)
			VALGRIND="valgrind --leak-check=full"
			;;
		3)
			VALGRIND="valgrind --leak-check=full --show-leak-kinds=all"
			;;
		4)
			unset QT_QPA_PLATFORMTHEME
			VALGRIND="valgrind --tool=memcheck --leak-check=yes --leak-check=full  --track-origins=yes --suppressions=./ignorelibleaks -s "
			;;
		5)
			unset QT_QPA_PLATFORMTHEME
			VALGRIND="valgrind --leak-check=full  --show-leak-kinds=all --track-origins=yes --suppressions=./ignorelibleaks -s "
			;;
		6)
			unset QT_QPA_PLATFORMTHEME
			VALGRIND="valgrind --leak-check=full --suppressions=./ignorelibleaks -s "
			;;
	esac
fi

g++ ${PWD}/../../src/QT_RunExternalProc.cpp "$0" -g -Wall -I${PWD} -I${PWD}/../../src -DDATADIR="\"${PWD}\"" $(pkg-config --cflags --libs Qt6Core Qt6Widgets) -fPIC -o ./qt_runexternalproc||exit 1
$VALGRIND ./qt_runexternalproc "$@"
retval=$?
#rm ./qt_runexternalproc
exit $retval
#endif

#include <QtWidgets>

#include "globals.h"

#define QUITITEM 500
#define RUNPROCS 501
#define ABOUTITEM 600
#define ABOUTQTITEM 601
#define HELPITEM 602

QPlainTextEdit	*te=NULL;
QMainWindow		*mainwindow=NULL;
QMenu			*fileMenu;
QMenu			*helpMenu;

QMenu* setHelpMenu(QMenuBar *menubar)
{
	QActionGroup		*actions;
	QAction			*act;
	QMenu			*menu;

	menu=menubar->addMenu("&Help");
	actions=new QActionGroup(menu);
	actions->setExclusive(true);

	act=new QAction(QIcon::fromTheme("help-about"),"About",actions);
	act->setData(ABOUTITEM);

	act=new QAction(QIcon::fromTheme("help-about"),"About QT",actions);
	act->setData(ABOUTQTITEM);

	act=new QAction(QIcon::fromTheme("help-contents"),"Help",actions);
	act->setData(HELPITEM);

	act=new QAction(actions);
	act->setSeparator(true);

	menu->addActions(actions->actions());
	QObject::connect(actions,&QActionGroup::triggered,actions,[&](QAction *action)
		{
			qDebug()<<action->text()<<action->data().toInt();
			switch(action->data().toInt())
				{
					case ABOUTITEM:
						break;
					case ABOUTQTITEM:
						QMessageBox::aboutQt(nullptr);
						break;
					case HELPITEM:
						break;
				}
		});
	return(menu);
}

void runProcs(void)
{
	QMessageBox msgBox;
	QString				retstr;
	QT_RunExternalProc	procs;
	int					ret;

	msgBox.setText("Choose example");
	msgBox.setInformativeText("runCommandsInShell [sync]\nrunCommandsInShell [async]\nrunCommands [piped]\nrunCommandsInShell [complex]\nrunCommands with errors and redirect stderr to file [errors]\nrunCommands with errors going to stderr [errors 2]\nrunCommands with errors going to mutiple error files [errors 3]");
	msgBox.addButton("sync",QMessageBox::AcceptRole);
	msgBox.addButton("async",QMessageBox::AcceptRole);
	msgBox.addButton("piped",QMessageBox::AcceptRole);
	msgBox.addButton("complex",QMessageBox::AcceptRole);
	msgBox.addButton("errors",QMessageBox::AcceptRole);
	msgBox.addButton("errors 2",QMessageBox::AcceptRole);
	msgBox.addButton("errors 3",QMessageBox::AcceptRole);
	msgBox.addButton(QMessageBox::Close);
	ret=msgBox.exec();

	switch(ret)
		{
			case 2:
				{
					procs.setStdErrFileOption(stdErrOption::output);
					procs.readByLine=true;
					procs.connectCB([&procs](QString msg)
						{
							qDebug()<<QString("got:%1").arg(msg);
						});
					qDebug()<<"Running sync...";
					procs.runCommandsInShell("echo \"starting sync....\";ls;sleep 4;echo done sync");
					qDebug()<<"Finished ...";
				}
				break;
			case 3:
					procs.setStdErrFileOption(stdErrOption::output);
					procs.sync=false;
					procs.connectCB([&procs](QString msg)
						{
							qDebug()<<QString("got:%1").arg(msg);
						});
					qDebug()<<"Running async...";
					procs.runCommandsInShell("echo \"starting async....\";ls;sleep 120;echo done async");
					qDebug()<<"pid"<<procs.lastBGPID;
					qDebug()<<"Finished ...";
				break;
			case 4:
				{
					qDebug()<<"Running piped line by line";
					QString tags="";
					QStringList tsl=QStringList()<<QString("ctags -x ../../src/*")<<"sort"<<QString("awk '{print $1 \" \" $2 \" \" $3 \" \" $4}'");
					procs.readByLine=true;
					procs.connectCB([&procs](QString msg)
						{
							qDebug()<<QString("Read line:%1").arg(msg);
							usleep(12500);
						});

					if(procs.setCommands(tsl)==true)
						tags=procs.runCommands();
				}
				break;
			case 5:
				{
					qDebug()<<"Running complex piped/redirected commands in shell";
					retstr=procs.runCommandsInShell("ls / /root 2>/tmp/error2.log|tee /tmp/whatis|tac -|tee -a /tmp/what;cd /tmp;ls");
					QStringList sl=retstr.split('\n',Qt::SkipEmptyParts);
					for(const QString &str : sl)
						qDebug() << str;
				}
				break;
			case 6:
				{
					qDebug()<<"Run piped commands with errors going to /tmp/error.log";
					procs.clearCallbacks();
					procs.setStdErrFileOption(stdErrOption::toFile,"/tmp/error.log");
					if(procs.setCommands(QStringList()<<"touch /zzz"<<"ls /root ~"<<"cat - /etc/fstab"<<"sort -u"<<"tac -")==true)
						{
							retstr=procs.runCommands();
							printf("%s\n",qPrintable(retstr));
						}
				}
				break;
			case 7:
				{
					qDebug()<<"Run command with errors going to stderr";
					procs.setStdErrFileOption(stdErrOption::output);
					if(procs.setCommands(QStringList()<<"ls / /xcxzczxcz")==true)
						procs.runCommands();
				}
				break;
			case 8:
				{
					qDebug()<<"Run command with errors going to multiple error files\nOp going to file";
					procs.setStdErrFileOption(stdErrOption::multiToFile,"/tmp/error1.log",QIODeviceBase::Append);
					procs.setStdOutFileOption("/tmp/log.txt",QIODeviceBase::Append);
					if(procs.setCommands(QStringList()<<"touch /zzzz"<<"ls ${HOME} $(pwd) / /xcxzczxcz")==true)
						retstr=procs.runCommands();
				}
				break;
		}
}

QMenu* setFileMenu(QMenuBar *menubar)
{
	QActionGroup		*actions;
	QAction			*act;
	QMenu			*menu;

	menu=menubar->addMenu("&File");
	actions=new QActionGroup(menu);
	actions->setExclusive(true);

	act=new QAction(QIcon::fromTheme("system-run"),"Run Procs",actions);
	act->setData(RUNPROCS);

	act=new QAction(actions);
	act->setSeparator(true);

	act=new QAction(QIcon::fromTheme("application-exit"),"Quit",actions);
	act->setShortcut(QKeySequence::Quit);
	act->setData(500);

	menu->addActions(actions->actions());
	QObject::connect(actions,&QActionGroup::triggered,actions,[&](QAction *action)
		{
			qDebug()<<action->text()<<action->data().toInt();
			if(action->data().toInt()==QUITITEM)
				qApp->exit(0);
			if(action->data().toInt()==RUNPROCS)
				runProcs();
		});
	return(menu);
}

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	QWidget		*widg;
	QVBoxLayout	*layout;
	QMenuBar		*menuBar;
	QString		realDataDir=QString("%1%2").arg(getenv("APPDIR")).arg(DATADIR);
	QSettings	prefs("KDHedger",PACKAGE_NAME);

	mainwindow=new QMainWindow;
	widg=new QWidget(mainwindow);
	layout=new QVBoxLayout(widg);
	menuBar=new QMenuBar(mainwindow);

	app.setOrganizationDomain("KDHedger");
	app.setApplicationName(PACKAGE_NAME);

	QIcon::setThemeSearchPaths(QStringList()<<QString("%1/usr/share/icons").arg(getenv("APPDIR"))<<QString("/usr/share/icons")<<QString("%1/.icons").arg(getenv("HOME")) <<QString("%1/icons").arg(realDataDir) );
	QIcon::setFallbackSearchPaths(QStringList()<<QString("%1/usr/share/icons").arg(getenv("APPDIR"))<<QString("/usr/share/icons")<<QString("%1/.icons").arg(getenv("HOME"))  <<QString("%1/icons").arg(realDataDir));

	te=new QPlainTextEdit(widg);
	QFile		file(QString("%1/../../LICENSE").arg(getenv("PWD")));
	if(file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QString data="";
			QTextStream in(&file);
			data=in.readAll();
			file.close();
			te->setPlainText(data);
		}

	layout->addWidget(te);
	layout->setAlignment(Qt::AlignCenter);

	widg->setLayout(layout);
	mainwindow->setCentralWidget(widg);

	mainwindow->setWindowTitle(PACKAGE_NAME);

	fileMenu=setFileMenu(menuBar);
	helpMenu=setHelpMenu(menuBar);

	mainwindow->setMenuBar(menuBar);

	if(prefs.contains("app/geometry"))
		mainwindow->restoreGeometry(prefs.value("app/geometry").toByteArray());
	else
		mainwindow->setGeometry(1322,331,535,505);

	mainwindow->show();

	app.exec();

	prefs.setValue("app/geometry",mainwindow->saveGeometry());
	delete mainwindow;

	return(0);
}

