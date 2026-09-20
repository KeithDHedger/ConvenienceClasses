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

out=$(basename "$0" .cpp)
g++  "$0" ./CallbackClass.cpp -O0 -g -Wall $(pkg-config --cflags --libs Qt6Core Qt6Widgets) -fPIC -o $out||exit 1
$VALGRIND ./$out "$@"
retval=$?
#rm $out
exit $retval

#endif

#include <stdio.h>
#include <unistd.h>
#include "CallbackClass.h"

int main(int argc, char **argv)
{
	bool					loopflag=true;
	CallbackClassClass	cc;
	int					cnt=0;

	cc.clearCallbacks();

	cc.connectCB([&cc](QString msg)
		{
			qDebug()<<QString("got 1:%1").arg(msg);
		});

	cc.connectCB([&cc,&loopflag](QString msg)
		{
			qDebug()<<QString("got 2:%1").arg(msg);
			loopflag=false;
		});

	while(loopflag==true)
		{
			printf("Hello World!\n");
			cc.triggerCallback(0,"Hello World! from callback");
			cnt++;
			if(cnt==4)
				cc.triggerCallback(1,"I'm done");
			sleep(1);
		}
	cc.triggerCallbacks("One last time");
	cc.triggerCallback(1,"1 >>>>>from cc.ccCB=([&cc](QString msg)");
	cc.triggerCallback(20,"2 ======from cc.ccCB=([&cc](QString msg)");//no call back set

	cc.connectInternalCB([&cc](QString msg)
	//cc.ccCB=([&cc](QString msg)
		{
			qDebug()<<QString("got 3:%1").arg(msg)<<cc.cnt;
			cc.triggerCallback(0,"from cc.ccCB=([&cc](QString msg)");
		});


	cc.testClassCB();

	cc.connectCBByName(QStringLiteral("testing"),[&cc](QString msg)
		{
			qDebug()<<QString("got 4:%1").arg(msg)<<cc.cnt;
		});
	//cc.connectCBByName(QString::fromUtf8("testing2"),[&cc](QString msg)
	cc.connectCBByName(QStringLiteral("testing2"),[&cc](QString msg)
		{
			qDebug()<<QString("got 4:%1").arg(msg)<<cc.cnt;
		});

	cc.triggerCallbackByName(QStringLiteral("testing"),"callback name=testing");
	cc.triggerCallbackByName(QStringLiteral("testing2"),"callback name=testing2");
	cc.triggerCallbackByName(QStringLiteral("testing3"),"callback name=testing3");//no such cb
	return 0;
}