
#ifndef _QT_FINDCLASS_
#define _QT_FINDCLASS_

class QT_FindClass
{
	public:
		QT_FindClass(QMainWindow *window);
		~QT_FindClass();

#ifdef USEPLAIN
		QPlainTextEdit			*te=NULL;
#else
		QTextEdit				*te=NULL;
#endif

		QAction					findWrapped;
		QString					hiliteAllColour="#40ff0000";
		QDialog					*findReplaceDialog=NULL;
		bool						showMultiDoc=false;
		bool						autoHighlightAll=true;
		bool						searchBack=false;
		bool						highlightAll=false;
		
		void						showFindDialog(void);
		bool						checkForMatch(void);
		bool						findString(bool replace);
		void						highLightAllMatches(void);
		void						resetHighLights(void);
		void						resetSearchPositions(void);

	private:
//guis
		QMainWindow				*parent=NULL;
		QComboBox				*findDropBox=NULL;
		QComboBox				*replaceDropBox=NULL;
		QCheckBox				*caseCheck=NULL;
		QCheckBox				*regexCheck=NULL;
		QCheckBox				*findAfterCheck=NULL;
		QCheckBox				*wrapCheck=NULL;
		QCheckBox				*multiDocumentCheck=NULL;
		QCheckBox				*highlightAllCheck=NULL;
		QCheckBox				*replaceAllCheck=NULL;
		QCheckBox				*backwardsCheck=NULL;
		QPushButton				*replaceButton=NULL;
		QPushButton				*findButton=NULL;
		QStringList				findList;
		QStringList				replaceList;
		QSettings				prefs;

		QString					targetString;
		QString					replacementString;
		QTextCursor				holdCursor;
		QRegularExpressionMatch	match;

		bool						insensitiveSearch=false;
		bool						useRegex=false;
		bool						findAfterReplace=true;
		bool						wrapSearch=false;
		bool						findInMultDocument=false;
		bool						replaceAll=false;
        		
		int						startPosition=-1;
		int						holdStartPos=-1;
        		
		void						buildFindDialog(void);
		void						hideDialog(void);
        		
		void						setSearchStrings(void);
		void						replaceAllMatches(void);
		void						findButtonCB(void);
		void						replaceButtonCB(void);
		void						replaceMatched(QTextCursor tc);
		QRegularExpression		getRegex(void);
};

#endif
