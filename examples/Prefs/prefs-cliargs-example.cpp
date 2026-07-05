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
#rm ./a.out
exit $retval


#endif

#include "globals.h"

int main(int argc, char **argv)
{
	QCoreApplication myapp(argc,argv);

	option	long_options[]=
		{
			{"showsyspage",no_argument,NULL,'s'},
			{"opensyspage",required_argument,NULL,'o'},
			{"testoptional",optional_argument,NULL,'t'},
			{0,0,0,0}
		};

{
	prefsClass	newprefs;
	bool parse;

	parse=newprefs.doCliArgs(argc,argv,long_options);
	if(parse==false)
		{
			qDebug()<<"put in some help";
			exit(0);
		}

	if(newprefs.prefsData.contains(newprefs.hashFromKey("showsyspage")))
		{
			qDebug()<<"showsyspage"<<newprefs.getPrefValue("showsyspage").toStringList();
			for(int j=0;j<newprefs.getPrefValue("showsyspage").toStringList().size();j++)
				{
					qDebug()<<j<<newprefs.getPrefValue("showsyspage").toStringList().at(j);
				}
		}
	if(newprefs.prefsData.contains(newprefs.hashFromKey("opensyspage")))
		{
			qDebug()<<"opensyspage"<<newprefs.getPrefValue("opensyspage").toStringList();
			for(int j=0;j<newprefs.getPrefValue("opensyspage").toStringList().size();j++)
				{
					qDebug()<<j<<newprefs.getPrefValue("opensyspage").toStringList().at(j);
				}
		}
	if(newprefs.prefsData.contains(newprefs.hashFromKey("testoptional")))
		{
			qDebug()<<"testoptional"<<newprefs.getPrefValue("testoptional").toStringList();
			for(int j=0;j<newprefs.getPrefValue("testoptional").toStringList().size();j++)
				{
					qDebug()<<j<<newprefs.getPrefValue("testoptional").toStringList().at(j);
				}


		}
	if(newprefs.extraCliArgs.isEmpty()==false)
		qDebug()<<"xtra args"<<newprefs.extraCliArgs;

}
	return 0;
}