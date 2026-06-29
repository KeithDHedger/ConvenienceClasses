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
			VALGRIND="valgrind --leak-check=full --track-origins=yes --suppressions=./ignorelibleaks "
			;;
	esac
fi

g++ -Wall -I${PWD} -I${PWD}/../../src -DDATADIR="\"${PWD}\"" $(pkg-config --cflags --libs Qt6Core Qt6Widgets) -fPIC ${PWD}/../../src/QT_AboutBox.cpp "$0"||exit 1
$VALGRIND ./a.out "$@"
retval=$?
rm ./a.out
exit $retval

#endif

#include "globals.h"

#define PACKAGE_NAME "Qt6 About Example"

#define QUITITEM 500
#define ABOUTITEM 600
#define ABOUTQTITEM 601
#define HELPITEM 602

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
						{
							AboutBoxClass	*about=new AboutBoxClass(qApp->activeWindow(),QString("%1/pixmaps/About.png").arg(getenv("PWD")));
							QFile			file(QString("%1/docs/gpl-3.0.txt").arg(DATADIR));
							if(file.open(QIODevice::ReadOnly | QIODevice::Text))
								{
									QTextStream in(&file);
									about->licence=in.readAll();
									file.close();
								}
							about->credits=credits;
							about->setHomepage("https://keithdhedger.github.io/LFSDesktopProject/","Test LFS Page");
							about->setBodyText("A test about box,");
							about->showAboutQtButton(true);
							about->showLicenceButton(true);
							about->showCreditsButton(true);

							about->runAbout();
						}
						break;
					case ABOUTQTITEM:
						qApp->aboutQt();
						break;
					case HELPITEM:
						{
							AboutBoxClass	*about=new AboutBoxClass(qApp->activeWindow());

							about->showHelp(QString("%1/docs/help/help.html").arg(DATADIR));

						}
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

	act=new QAction(QIcon::fromTheme("document-save"),"Save",actions);
	act->setShortcut(QKeySequence::Save);
	act->setData(101);

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
		});
	return(menu);
}

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	QMainWindow	*mainwindow=new QMainWindow;
	QWidget		*widg=new QWidget(mainwindow);
	QLabel		*label=new QLabel(widg);
	QVBoxLayout	*layout=new QVBoxLayout(widg);
	QMenuBar		*menuBar=new QMenuBar(mainwindow);

	qDebug()<<DATADIR;

	QString realDataDir=QString("%1%2").arg(getenv("APPDIR")).arg(DATADIR);

	QIcon::setThemeSearchPaths(QStringList()<<QString("%1/usr/share/icons").arg(getenv("APPDIR"))<<QString("/usr/share/icons")<<QString("%1/.icons").arg(getenv("HOME")) <<QString("%1/icons").arg(realDataDir) );
	QIcon::setFallbackSearchPaths(QStringList()<<QString("%1/usr/share/icons").arg(getenv("APPDIR"))<<QString("/usr/share/icons")<<QString("%1/.icons").arg(getenv("HOME"))  <<QString("%1/icons").arg(realDataDir));

	app.setApplicationName(PACKAGE_NAME);

	mainwindow->setGeometry(989,230,800,600);
	
	label->setText("Put somthing here ...");

	layout->addWidget(label);
	layout->setAlignment(Qt::AlignCenter);

	widg->setLayout(layout);
	mainwindow->setCentralWidget(widg);

	mainwindow->setWindowTitle(PACKAGE_NAME);

	QMenu		*fileMenu=setFileMenu(menuBar);
	QMenu		*helpMenu=setHelpMenu(menuBar);

	mainwindow->setMenuBar(menuBar);

	mainwindow->show();

	app.exec();
	delete menuBar;
	return(0);
}

