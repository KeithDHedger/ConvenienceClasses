
#ifndef _QT_LINEEDITCOMPLETER_
#define _QT_LINEEDITCOMPLETER_

#include "globals.h"

enum {STRINGCOMPLETE=0,FOLDERCOMPLETE};

class QT_lineEditCompleterClass: public QLineEdit
{
	public:
		QT_lineEditCompleterClass(const QString &contents,QWidget *parent=nullptr);
		~QT_lineEditCompleterClass();

		bool				useInternaleSC=true;

		void				setCompleteType(int type);
		void				setUpCompleter(QStringList sl=QStringList());
		void				setRootFolder(QString path);
		void				setStrings(QStringList sl);
		void				doActivateKey(void);
		void				doCancelKey(void);

	protected:
		void				focusInEvent(QFocusEvent *e);

	private:
		QCompleter		*completer=NULL;
		QShortcut		*shortcutESC=NULL;
		QShortcut		*shortcutTAB=NULL;
		QStringListModel	*folderModel=NULL;
		QStringListModel	*stringModel=NULL;
		QString			holdFold;
		QString			rootFold="/";
		QString			holdText="";
		QStringList		completeForPrefix(QString typed);
		int				completionType=STRINGCOMPLETE;

};

#endif
