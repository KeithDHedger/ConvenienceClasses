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

g++ ${PWD}/../../src/QT_lineEditCompleter.cpp "$0" -g -Wall -I${PWD} -I${PWD}/../../src -DDATADIR="\"${PWD}\"" $(pkg-config --cflags --libs Qt6Core Qt6Widgets) -fPIC -o ./lineeditcompleter||exit 1
$VALGRIND ./lineeditcompleter "$@"
retval=$?
#rm ./lineeditcompleter
exit $retval
#endif

#include <QtWidgets>

#include "globals.h"

#define QUITITEM 500
#define HOMEITEM 501
#define USRITEM 502
#define USERHOMEITEM 503
#define ABOUTITEM 600
#define ABOUTQTITEM 601
#define HELPITEM 602

QPlainTextEdit	*te=NULL;
QMainWindow		*mainwindow=NULL;
QMenu			*fileMenu;
QMenu			*helpMenu;

QT_lineEditCompleterClass	*le=NULL;
QT_lineEditCompleterClass	*lestr=NULL;

void makeCompleterStringList(void)
{
	QStringList	data;
	FILE			*fp=NULL;
	char			line[1024];

	lestr=new QT_lineEditCompleterClass("",mainwindow);
	lestr->setPlaceholderText("Type some function/member strings eg: build)");
	lestr->useInternaleSC=true;

	lestr->setCompleteType(STRINGCOMPLETE);
	QObject::connect(lestr,&QLineEdit::editingFinished,[]()
		{
			QStringList sl=lestr->text().split(' ');
			if(sl.count()>3)
				{
					//QString com=QString("kkeditqt '%1@%2'").arg(sl.at(3)).arg(sl.at(2));
					lestr->setText(sl.at(0));
					lestr->clearFocus();
					//system(qPrintable(com));
				}
		});

	fp=popen("ctags -x *.cpp ../../src/*|awk '{print $1 \" \" $2 \" \" $3 \" \" $4}'","r");
	if(fp!=NULL)
		{
			while(fgets(line,1024,fp))
				{
					if(line[strlen(line)-1]=='\n')
						line[strlen(line)-1]=0;
					data<<line;
				}
			pclose(fp);
		}

	//lestr->strings=data;
	lestr->setUpCompleter(data);
	//lestr->setStrings(data);
}

void makeCompleterFolderList(QString fold)
{
	le=new QT_lineEditCompleterClass("",mainwindow);
	le->setPlaceholderText("Type a path under /home (e.g. user/Do...)");

	le->setCompleteType(FOLDERCOMPLETE);
	le->setRootFolder(fold);
	le->setUpCompleter();
}

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

QMenu* setFileMenu(QMenuBar *menubar)
{
	QActionGroup		*actions;
	QAction			*act;
	QMenu			*menu;

	menu=menubar->addMenu("&File");
	actions=new QActionGroup(menu);
	actions->setExclusive(true);

	act=new QAction(QIcon::fromTheme("folder-open"),"Use /home",actions);
	act->setData(HOMEITEM);

	act=new QAction(QIcon::fromTheme("folder-open"),"Use /usr",actions);
	act->setData(USRITEM);

	act=new QAction(QIcon::fromTheme("user-home"),"Use ~",actions);
	act->setData(USERHOMEITEM);

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
			if(action->data().toInt()==HOMEITEM)
				{
					le->setRootFolder("/home");
				}
			if(action->data().toInt()==USRITEM)
				{
					le->setRootFolder("/usr");
				}
			if(action->data().toInt()==USERHOMEITEM)
				{
					le->setRootFolder(getenv("HOME"));
				}

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

	makeCompleterStringList();
	layout->addWidget(lestr);

	makeCompleterFolderList(argv[1]);
	layout->addWidget(le);

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

