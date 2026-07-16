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

g++ -g -Wall -I${PWD} -I${PWD}/../../src -DUSEPLAIN -DDATADIR="\"${PWD}\"" $(pkg-config --cflags --libs Qt6Core Qt6Widgets) ${PWD}/../../src/QT_FindClass.cpp -fPIC "$0"||exit 1
$VALGRIND ./a.out "$@"
retval=$?
#rm ./a.out
exit $retval

#endif

#include <QtWidgets>

#include "globals.h"

#define QUITITEM 500
#define ABOUTITEM 600
#define ABOUTQTITEM 601
#define HELPITEM 602
#define FINDITEM 603
#define SWITCHITEM 604

QPlainTextEdit		*doc=NULL;
QMainWindow			*mainwindow=NULL;
QT_FindClass	*globalfind;
QMenu				*fileMenu;
QMenu				*helpMenu;
QTabWidget			*notebook=NULL;

void justSwitchPage(void)
{
	QPlainTextEdit	*tewidg;
	int				cp;

	globalfind->resetHighLights();

	cp=notebook->currentIndex();

	if(globalfind->searchBack==false)
		{
			cp++;
			if(cp==notebook->count())
				cp=0;
		}
	else
		{
			cp--;
			if(cp<0)
				cp=notebook->count()-1;
		}

	notebook->setCurrentIndex(cp);
	tewidg=(QPlainTextEdit*)notebook->currentWidget();
	globalfind->te=tewidg;
	QTextCursor c=tewidg->textCursor();
	c.clearSelection();
	tewidg->setTextCursor(c);
	tewidg->setFocus();
}

void switchPage(void)
{
	QPlainTextEdit	*tewidg;
	int				cp;

	cp=notebook->currentIndex();

	for(int j=0;j<notebook->count();j++)
		{
			if(globalfind->searchBack==false)
				{
					cp++;
					if(cp==notebook->count())
						cp=0;
				}
			else
				{
					cp--;
					if(cp<0)
						cp=notebook->count()-1;
				}

			notebook->setCurrentIndex(cp);
			tewidg=(QPlainTextEdit*)notebook->currentWidget();
			globalfind->te=tewidg;

			QTextCursor c=tewidg->textCursor();
			if(globalfind->searchBack==false)
				c.setPosition(0);
			else
				c.movePosition(QTextCursor::End);
			tewidg->setTextCursor(c);

			if(globalfind->checkForMatch()==false)
				{
					continue;
				}
			else
				{
					globalfind->resetSearchPositions();
					tewidg->setFocus();
					return;
				}
		}
}

void doFindReplace(void)
{
	globalfind->showFindDialog();
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

	act=new QAction(QIcon::fromTheme("document-open"),"Open",actions);
	act->setShortcut(QKeySequence::Open);
	act->setData(100);

	act=new QAction(QIcon::fromTheme("document-open"),"Switch",actions);
	act->setData(SWITCHITEM);

	act=new QAction(QIcon::fromTheme("edit-find"),"Find",actions);
	act->setData(FINDITEM);

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
			if(action->data().toInt()==FINDITEM)
				doFindReplace();
			if(action->data().toInt()==SWITCHITEM)
				justSwitchPage();
		});
	return(menu);
}

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	QMenuBar		*menuBar;
	QString		realDataDir=QString("%1%2").arg(getenv("APPDIR")).arg(DATADIR);
	QSettings	prefs("KDHedger",PACKAGE_NAME);

	mainwindow=new QMainWindow;
	menuBar=new QMenuBar(mainwindow);

	app.setOrganizationDomain("KDHedger");
	app.setApplicationName(PACKAGE_NAME);

	QIcon::setThemeSearchPaths(QStringList()<<QString("%1/usr/share/icons").arg(getenv("APPDIR"))<<QString("/usr/share/icons")<<QString("%1/.icons").arg(getenv("HOME")) <<QString("%1/icons").arg(realDataDir) );
	QIcon::setFallbackSearchPaths(QStringList()<<QString("%1/usr/share/icons").arg(getenv("APPDIR"))<<QString("/usr/share/icons")<<QString("%1/.icons").arg(getenv("HOME"))  <<QString("%1/icons").arg(realDataDir));

//add some pages
	notebook=new QTabWidget(mainwindow);
	globalfind=new QT_FindClass(mainwindow);
	globalfind->showMultiDoc=true;
	QObject::connect(&globalfind->findWrapped,&QAction::triggered,[&]()
		{
			switchPage();
		});

	doc=new QPlainTextEdit;
	QString data;
	data="abc xx\nxx 123 xx\nxx abc xx\nxx 143 xx\nxx abc xx\nxx 123 xx\nxx1245673xx\n";
	doc->setPlainText(data);
	notebook->addTab(doc,"doc1");
	globalfind->te=doc;

	doc=new QPlainTextEdit;
	data="asd xx\nxx 123 xx\nxx asd xx\nxx 123 xx\nxx12777773xx\n\nxx asd xx\nxx 123 xx\n";
	doc->setPlainText(data);
	notebook->addTab(doc,"doc2");

	doc=new QPlainTextEdit;
	data="123\nasd xx\nxx 123 xx\nxx asd xx\nxx 123 xx\nxx asd xx\nxx 123 xx123\n";
	//data="asd xx\nxx qqq xx\nxx asd xx\nxx zzz xx\nxx asd xx\nxx ttt xx\n";
	doc->setPlainText(data);
	notebook->addTab(doc,"doc3");

	//notebook->tabBar()->hide();
	mainwindow->setCentralWidget(notebook);

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

	delete globalfind;
	delete mainwindow;
	return(0);
}

