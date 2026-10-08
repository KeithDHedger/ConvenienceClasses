
#include "xlibToQt.h"

xlibToQtClass::~xlibToQtClass()
{
}

xlibToQtClass::xlibToQtClass()
{
	QNativeInterface::QX11Application	*x11=qApp->nativeInterface<QNativeInterface::QX11Application>();
	this->display=x11->display();
}

void xlibToQtClass::setCardinalProp(Window w,const char* prop,void *ptr)
{
	XChangeProperty(this->display,w,XInternAtom(this->display,prop,False),XA_CARDINAL,32,PropModeReplace,(const unsigned char*)ptr,1);
}

void xlibToQtClass::setWindowProps(Window window,const char* grp,const char *type_name,int what)
{
	Atom window_type = XInternAtom(this->display,grp,False);
	Atom type=XInternAtom(this->display,type_name,False);

	XChangeProperty(this->display,window,window_type,XA_ATOM,32,what,(unsigned char *)&type,1);
    XFlush(this->display);
}

Pixmap xlibToQtClass::getWindowPixmap(Window win)
{
	Pixmap			currentRootPixmap=None;
	Atom				act_type;
	int				act_format;
	unsigned long	nitems;
	unsigned long	bytes_after;
	unsigned char	*data=NULL;
	Atom				_XROOTPMAP_ID;

	_XROOTPMAP_ID=XInternAtom(this->display,"_XROOTPMAP_ID",False);
	if(XGetWindowProperty(this->display,win,_XROOTPMAP_ID,0,1,False,XA_PIXMAP,&act_type,&act_format,&nitems,&bytes_after,&data)==Success)
		{
			if(data)
				{
					currentRootPixmap=*((Pixmap *)data);
					XFree(data);
				}
		}

	return(currentRootPixmap);
}

QPixmap xlibToQtClass::convertXlibPixmapToQPixmap(Pixmap pixmap)
{
	QPixmap		result;
	unsigned int	wid, hite,border_width,depth_return;
	int			x,y;
	Window		root_return;
    XImage		*xImage=NULL;

	XGetGeometry(this->display,pixmap,&root_return,&x,&y,&wid,&hite,&border_width,&depth_return);
	xImage=XGetImage(this->display,pixmap,0,0,wid,hite,AllPlanes,ZPixmap);
    if(!xImage)
		{
			qDebug()<<"no image"<<xImage;
			return(QPixmap());
		}

	QImage	qImage(reinterpret_cast<uchar*>(xImage->data),wid,hite,QImage::Format_RGB32);
	result=QPixmap::fromImage(qImage.copy());

	XDestroyImage(xImage);

    return(result);
}
