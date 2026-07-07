
#ifndef _QT_SPELLCHECK_
#define _QT_SPELLCHECK_

#include <aspell.h>

class QT_SpellCheckClass
{
	public:
		QT_SpellCheckClass(QMainWindow *window);
		~QT_SpellCheckClass();

		QPlainTextEdit	*te=NULL;
		QString			badwordHiliteColour="#40000000";

		QStringList		getLanguageCodes(void);
		void				setLanguage(QString lang);
		void				doSpellCheckDoc(void);
		void				doSpellCheckWord(QString word);
		AspellConfig		*getConfig(void);


	private:
		QMainWindow		*mainWindow=NULL;
		QDialog			*spellCheckWord=NULL;
		QLabel			*badWordLabel=NULL;
		QComboBox		*wordListDropbox=NULL;
		QString			badWord;
		QString			goodWord;

		AspellConfig		*aspellConfig=NULL;
		AspellSpeller	*spellChecker=0;

		int				badwordstart=-1;
		int				badwordlen=0;
		bool				blockFlag=false;
		bool				cancelCheck=false;

		bool				checkTheWord(QString word);
		void				buildWordCheckDialog(void);
		void				doChangeWord(void);

};

#endif
