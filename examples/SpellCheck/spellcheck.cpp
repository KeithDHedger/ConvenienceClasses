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

g++ -g -Wall -I${PWD} -I${PWD}/../../src -DDATADIR="\"${PWD}\"" $(pkg-config --cflags --libs Qt6Core Qt6Widgets) -fPIC "$0"||exit 1
$VALGRIND ./a.out "$@"
retval=$?
rm ./a.out
exit $retval

#endif

#include <QtWidgets>

#include "globals.h"

#define PACKAGE_NAME "Spell Check Example"

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

	act=new QAction(QIcon::fromTheme("document-save"),"Save",actions);
	act->setShortcut(QKeySequence::Save);
	act->setData(101);

	act=new QAction(QIcon::fromTheme("preferences-desktop"),"Prefs",actions);
	act->setData(102);

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
	QTextEdit	*te=new QTextEdit(widg);
	QVBoxLayout	*layout=new QVBoxLayout(widg);
	QMenuBar		*menuBar=new QMenuBar(mainwindow);
	QString		realDataDir=QString("%1%2").arg(getenv("APPDIR")).arg(DATADIR);

	QIcon::setThemeSearchPaths(QStringList()<<QString("%1/usr/share/icons").arg(getenv("APPDIR"))<<QString("/usr/share/icons")<<QString("%1/.icons").arg(getenv("HOME")) <<QString("%1/icons").arg(realDataDir) );
	QIcon::setFallbackSearchPaths(QStringList()<<QString("%1/usr/share/icons").arg(getenv("APPDIR"))<<QString("/usr/share/icons")<<QString("%1/.icons").arg(getenv("HOME"))  <<QString("%1/icons").arg(realDataDir));

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

	QMenu		*fileMenu=setFileMenu(menuBar);
	QMenu		*helpMenu=setHelpMenu(menuBar);

	mainwindow->setMenuBar(menuBar);
	mainwindow->setGeometry(1322,331,535,505);

	mainwindow->show();

	app.exec();
	delete mainwindow;
	return(0);
}

