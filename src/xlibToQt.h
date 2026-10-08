
#ifndef _XLIBTOQT_
#define _XLIBTOQT_

#include <QtWidgets>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

class xlibToQtClass
{
	public:
		xlibToQtClass();
		~xlibToQtClass();

		Display	*display=NULL;

		QPixmap	convertXlibPixmapToQPixmap(Pixmap pixmap);
		void		setWindowProps(Window window,const char* grp,const char *type_name,int what);
		Pixmap	getWindowPixmap(Window win);
		void		setCardinalProp(Window w,const char *prop,void *ptr);
};

#endif
