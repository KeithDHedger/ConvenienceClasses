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

g++ -g -Wall -I${PWD} -I${PWD}/../../src -DDATADIR="\"${PWD}\"" $(pkg-config --cflags --libs Qt6Core Qt6Widgets) -fPIC ${PWD}/../../src/prefsClass.cpp "$0"||exit 1
$VALGRIND ./a.out "$@"
retval=$?
rm ./a.out
exit $retval

#endif

#include "globals.h"

#define PACKAGE_NAME "Qt6-Window.cpp"

#define QUITITEM 500
#define ABOUTITEM 600
#define ABOUTQTITEM 601
#define HELPITEM 602

void doPrefs(void)
{
	prefsClass	newprefs;
	QStringList  prefsdata;;
	QSize		sze;
	QFile		file(QString("%1/exampleprefs.data").arg(getenv("PWD")));

	if(file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QString data="";
			QTextStream in(&file);
			data=in.readAll();
			file.close();
			prefsdata=data.split("\n");
			newprefs.paged=true;
			newprefs.createDialog(PACKAGE_STRING,prefsdata);
			if(newprefs.dialogPrefs.valid==true)
				newprefs.printCurrentPrefs();
		}
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
			else
				{
					switch(action->data().toInt())
						{
							case 100:
								{
									QString filename=QFileDialog::getOpenFileName(nullptr,"Select File","data");
									if(filename.isEmpty()==false)
										{
											qDebug()<<filename;
											//this->dialogPrefs.fileBoxes[filenum]->setText(filename);
											//this->dialogPrefs.fileBoxes[filenum]->setCursorPosition(0);
										}
								}
								break;
							case 101:
								break;
							case 102:
								qDebug()<<"Do prefs ...";
								doPrefs();
								break;
							default:
								break;
						}
				}
				
		});
	return(menu);
}

int main(int argc, char **argv)
{
	QApplication app(argc,argv);

	QSettings	defaults("KDHedger",PACKAGE);
	QMainWindow	*mainwindow=new QMainWindow;
	QWidget		*widg=new QWidget(mainwindow);
	QLabel		*label=new QLabel(widg);
	QVBoxLayout	*layout=new QVBoxLayout(widg);
	QMenuBar		*menuBar=new QMenuBar(mainwindow);

	app.setApplicationName(PACKAGE);

	QString realDataDir=QString("%1%2").arg(getenv("APPDIR")).arg(DATADIR);

	QIcon::setThemeSearchPaths(QStringList()<<QString("%1/usr/share/icons").arg(getenv("APPDIR"))<<QString("/usr/share/icons")<<QString("%1/.icons").arg(getenv("HOME")) <<QString("%1/icons").arg(realDataDir) );
	QIcon::setFallbackSearchPaths(QStringList()<<QString("%1/usr/share/icons").arg(getenv("APPDIR"))<<QString("/usr/share/icons")<<QString("%1/.icons").arg(getenv("HOME"))  <<QString("%1/icons").arg(realDataDir));

	option	long_options[]=
		{
			{"showsyspage",no_argument,NULL,'p'},
			{"opensyspage",required_argument,NULL,'s'},
			{"testoptional",optional_argument,NULL,'t'},
			{0,0,0,0}
		};

//check cli args
{
	prefsClass	newprefs;
	bool parse;

	parse=newprefs.doCliArgs(argc,argv,long_options);
	if(parse==false)
		{
			qDebug()<<"put in some help";
			exit(0);
		}
//newprefs.writeManualPrefs();

	if(newprefs.prefsData.contains(newprefs.hashFromKey("showsyspage")))
		{
			qDebug()<<"showsyspage"<<newprefs.getPrefValue("showsyspage").toBool();
		}
	if(newprefs.prefsData.contains(newprefs.hashFromKey("opensyspage")))
		{
			qDebug()<<"opensyspage"<<newprefs.getPrefValue("opensyspage").toString();
		}
	if(newprefs.prefsData.contains(newprefs.hashFromKey("testoptional")))
		{
			qDebug()<<"testoptional"<<newprefs.getPrefValue("testoptional").toString();
		}
	if(newprefs.extraCliArgs.isEmpty()==false)
		qDebug()<<"xtra args"<<newprefs.extraCliArgs;
}
	if(defaults.contains("geometry"))
		mainwindow->restoreGeometry(defaults.value("geometry").toByteArray());
	else
		mainwindow->setGeometry(100,100,800,600);

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
	defaults.setValue("geometry",mainwindow->saveGeometry());

	delete mainwindow;

	{
		prefsClass	newprefs("AAATESTCODESET");

		QRect r(10, 20, 30, 40);
		newprefs.prefsData.clear();
		newprefs.prefsNames.clear();
		newprefs.addPref("qpx100",QVariant::fromValue(r));
		newprefs.addPref("some data/qpx200",QVariant::fromValue(QString("a string")));
		newprefs.addPref("myfont",QVariant::fromValue(QFont("Sans Serif,9,-1,5,50,0,0,0,0,0")));
		newprefs.addPref("my bool",QVariant::fromValue(true));

		newprefs.writeManualPrefs();

		qDebug()<<newprefs.getPrefValue("some data/qpx200");
		newprefs.setPrefValue("some data/qpx200",QVariant::fromValue(QString("another string")));
		qDebug()<<newprefs.getPrefValue("some data/qpx200");
		qDebug()<<newprefs.getSavedPrefValue("some data/qpx200");
	}

	return(0);
}

