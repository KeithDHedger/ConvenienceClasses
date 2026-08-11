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

g++ -g -Wall -I${PWD} -I${PWD}/../../src -DDATADIR="\"${PWD}\"" $(pkg-config --cflags --libs Qt6Core Qt6Widgets) ${PWD}/../../src/QT_RunExternalProc.cpp -fPIC "$0" -o ./qt_runexternalproc||exit 1
$VALGRIND ./qt_runexternalproc "$@"
retval=$?
rm ./qt_runexternalproc
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
	QString				retstr;
	QT_RunExternalProc	procs;

procs.setStdErrFileOption(stdErrOption::output);
QString folders="/home/keithhedger/Projects/DevProjects/KKEditQT/KKEditQT/src/*";
QString tags="";
//QStringList tsl=QStringList()<<QString("%1/ctags -x %2 %3").arg("/bin").arg("").arg(folders)<<QString("%1/sort").arg("/bin")<<QString("%1/awk '{print $1 \" \" $2 \" \" $3 \" \" $4}'").arg("/bin");
QStringList tsl=QStringList()<<QString("ctags -x -f - /home/keithhedger/Projects/DevProjects/KKEditQT/KKEditQT/src/*")<<"sort"<<QString("awk '{print $1 \" \" $2 \" \" $3 \" \" $4}'");


qDebug().noquote()<<tsl;


	//if(procs.setCommands(QStringList()<<QString("%1/ctags -x %2 %3").arg(this->realBinDir).arg(this->ctagsExlusions).arg(folders)<<QString("%1/sort").arg(this->realBinDir)<<QString("%1/awk '{print $1 \" \" $2 \" \" $3 \" \" $4}'") .arg(this->realBinDir) )==true)
	if(procs.setCommands(tsl)==true)
			tags=procs.runCommands();//.split('\n',Qt::SkipEmptyParts);

qDebug().noquote()<<tags;

return;
	//procs.setStdErr(stdErrOption::toFile,"/tmp/error.log");
	//procs.setStdErr(stdErrOption::output);
	//procs.setStdErrFileOption(stdErrOption::toFile,"/tmp/error.log");
	//procs.setStdErrFileOption(stdErrOption::multiToFile,"/tmp/error.log");
	if(procs.setCommands(QStringList()<<"touch /zzz"<<"ls /root ~"<<"cat - /etc/fstab"<<"sort -u"<<"tac -")==true)
	//if(procs.setCommands(QStringList()<<"echo -e \"$(stat /tmp)\"")==true)
		{
			retstr=procs.runCommands();
			printf(">>>>>%s<<<<<\n",qPrintable(retstr));
		}
	qDebug()<<"---------------------------";
return;
	procs.setStdErrFileOption(stdErrOption::output);
	if(procs.setCommands(QStringList()<<"ls / /xcxzczxcz")==true)
		{
			retstr=procs.runCommands();
			printf(">>>>>%s<<<<<\n",qPrintable(retstr));
		}

	qDebug()<<"++++++++++++++++++++++++++++++++";
	procs.setStdErrFileOption(stdErrOption::toFile,"/tmp/error1.log",QIODeviceBase::Append);
	procs.setStdOutFileOption("/tmp/log.txt",QIODeviceBase::Append);
	if(procs.setCommands(QStringList()<<"ls ${HOME} $(pwd) / /xcxzczxcz")==true)
		{
			retstr=procs.runCommands();
		}

	qDebug()<<"=============================";

	retstr=procs.runCommandsInShell("ls / /root 2>/tmp/error2.log|tee /tmp/whatis|tac -|tee -a /tmp/what");
	printf("--->>>>>%s<<<<<---\n",qPrintable(retstr));
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

