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

g++ -g -Wall -I${PWD} -I${PWD}/../../src -DDATADIR="\"${PWD}\"" $(pkg-config --cflags --libs Qt6Core Qt6Widgets) ${PWD}/../../src/ChooserDialog.cpp -fPIC "$0"||exit 1
$VALGRIND ./a.out "$@"
retval=$?
#rm ./a.out
exit $retval

#endif

#include "globals.h"

#define QUITITEM 500
#define OPENITEM 501
#define SAVEITEM 502
#define	FOLDERITEM 503
#define ABOUTITEM 600
#define ABOUTQTITEM 601
#define HELPITEM 602

QPlainTextEdit	*te=NULL;
QMainWindow		*mainwindow=NULL;
QMenu			*fileMenu;
QMenu			*helpMenu;
QVector<QString>	fileTypeFilters;

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

void doOpenFile(void)
{
	chooserDialogClass	chooser(chooserDialogType::openDialog);

	chooser.setMultipleSelect(true);
	chooser.setShowImagesInList(true);

	for(int j=0;j<fileTypeFilters.size();j++)
		chooser.addFileTypes(fileTypeFilters.at(j));

	chooser.dialogWindow.exec();

	if(chooser.valid==false)
		{
			qDebug()<<"Open canceled";
			return;
		}

	if(chooser.multiFileList.count()>0)
		{
			for(int j=0;j<chooser.multiFileList.count();j++)
				{
					qDebug()<<"File"<<j<<chooser.multiFileList.at(j);
				}
		}
	///qDebug()<<"File"<<j<<chooser.multiFileList.at(j);
}

void doSaveFile(void)
{
//	chooserDialogClass	chooser(chooserDialogType::saveDialog,"Untitled");
//	chooser.setMultipleSelect(false);
//	chooser.setShowImagesInList(true);
//
//	for(int j=0;j<fileTypeFilters.size();j++)
//		chooser.addFileTypes(fileTypeFilters.at(j));
//		
//	chooser.dialogWindow.exec();
//	if(chooser.valid==false)
//		{
//			qDebug()<<"Save canceled";
//			return;
//		}
//
//	qDebug()<<"Save File"<<chooser.selectedFilePath;
}

void doSelectFolder(void)
{
	chooserDialogClass	chooser(chooserDialogType::folderDialog);
	chooser.setMultipleSelect(false);
	chooser.setShowImagesInList(true);
		
	chooser.dialogWindow.exec();
	if(chooser.valid==false)
		{
			qDebug()<<"Select canceled";
			return;
		}

	qDebug()<<"Selected Folder"<<chooser.selectedFolder;

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
	act->setData(OPENITEM);

	act=new QAction(QIcon::fromTheme("document-save"),"Save",actions);
	act->setShortcut(QKeySequence::Save);
	act->setData(SAVEITEM);

	act=new QAction(QIcon::fromTheme("document-open"),"Select Folder",actions);
	act->setData(FOLDERITEM);

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
			switch(action->data().toInt())
				{
					case QUITITEM:
						qApp->exit(0);
						break;
					case OPENITEM:
						doOpenFile();
						break;
					case SAVEITEM:
						doSaveFile();
						break;
					case FOLDERITEM:
						doSelectFolder();
						break;
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
	layout->setAlignment(Qt::AlignCenter);

	widg->setLayout(layout);
	mainwindow->setCentralWidget(widg);

	mainwindow->setWindowTitle(PACKAGE_NAME);

	fileMenu=setFileMenu(menuBar);
	helpMenu=setHelpMenu(menuBar);

	mainwindow->setMenuBar(menuBar);

	fileTypeFilters.append("*.cpp;*.c;*.h;*.hpp;*.m;*.mm;*.py;*.go;*.java;*.js;*.rb;*.sh;*.rs;*.tcl;*.pl");
	fileTypeFilters.append("*.html;*.xml;*.css;*.php;*.pro;*.in;*.am;*.m4;*.md;*.ac;*.json;*.class;*.sql");
	fileTypeFilters.append("All Files");

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

