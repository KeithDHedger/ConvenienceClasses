
#include "globals.h"

QT_FindClass::~QT_FindClass()
{
	this->hideDialog();
	delete this->findReplaceDialog;
}

QT_FindClass::QT_FindClass(QMainWindow *window)
{
	this->parent=window;
	this->findList=this->prefs.value("find/findlist").toStringList();
	this->replaceList=this->prefs.value("find/replacelist").toStringList();
	this->insensitiveSearch=this->prefs.value("find/sensitive").toBool();
	this->searchBack=this->prefs.value("find/backwards").toBool();
	this->wrapSearch=this->prefs.value("find/wrap").toBool();
	this->highlightAll=this->prefs.value("find/highlightall").toBool();
	this->findInMultDocument=this->prefs.value("find/multidoc").toBool();
	this->replaceAll=this->prefs.value("find/replaceall").toBool();
	this->useRegex=this->prefs.value("find/regex").toBool();

	this->buildFindDialog();
}

void QT_FindClass::hideDialog(void)
{
	this->prefs.setValue("find/findgeometry",this->findReplaceDialog->saveGeometry());
	this->prefs.setValue("find/findlist",this->findList);
	this->prefs.setValue("find/replacelist",this->replaceList);
	this->prefs.setValue("find/sensitive",this->insensitiveSearch);
	this->prefs.setValue("find/backwards",this->searchBack);
	this->prefs.setValue("find/wrap",this->wrapSearch);
	this->prefs.setValue("find/highlightall",this->highlightAll);
	this->prefs.setValue("find/replaceall",this->replaceAll);
	this->prefs.setValue("find/regex",this->useRegex);

	if(this->showMultiDoc==true)
		this->prefs.setValue("find/multidoc",this->findInMultDocument);

	this->findReplaceDialog->hide();
}

void QT_FindClass::showFindDialog(void)
{
	if(this->prefs.contains("find/findgeometry"))
		this->findReplaceDialog->restoreGeometry(this->prefs.value("find/findgeometry").toByteArray());

	this->multiDocumentCheck->setVisible(this->showMultiDoc);
	if(this->showMultiDoc==false)
		this->findInMultDocument=false;
	this->findReplaceDialog->show();
}

void QT_FindClass::setSearchStrings(void)
{
	this->findList.append(this->findDropBox->currentText());
	this->findList.removeDuplicates();
	this->replaceList.append(this->replaceDropBox->currentText());
	this->replaceList.removeDuplicates();
	this->targetString=this->findDropBox->currentText().replace("\\n","\n");
	this->replacementString=this->replaceDropBox->currentText().replace("\\n","\n");
}

void QT_FindClass::findButtonCB(void)
{
	this->setSearchStrings();
	this->findString(false);	
	if(this->highlightAll==true && this->autoHighlightAll==true)
		this->highLightAllMatches();
}

void QT_FindClass::replaceMatched(QTextCursor tc)
{
	if(tc.hasSelection()==true)
		{
			tc.beginEditBlock();
				if(this->useRegex==true)
					{
						QString data=tc.selectedText();
						data.replace(this->match.regularExpression(),this->replacementString);
						tc.removeSelectedText();
						tc.insertText(data);
					}
				else
					{
						tc.removeSelectedText();
						tc.insertText(this->replacementString);
					}						
			tc.endEditBlock();
		}
	this->te->setTextCursor(tc);
}

void QT_FindClass::replaceAllMatches(void)
{
	this->te->textCursor().setPosition(0);
	while(this->checkForMatch()==true)
		{
			while(this->findString(true)==true)
				this->replaceMatched(this->te->textCursor());
		}
}

void QT_FindClass::replaceButtonCB(void)
{
	QTextCursor cursor;

	this->setSearchStrings();

	if(this->replaceAll==true)
		{
			this->replaceAllMatches();
			return;
		}
	cursor=this->te->textCursor();
	if(cursor==this->holdCursor)
		{
			this->replaceMatched(cursor);
			if(this->findAfterReplace==true)
				this->findString(true);
		}
	else
		this->holdCursor=QTextCursor();
}

void QT_FindClass::buildFindDialog(void)
{
	QVBoxLayout	*vlayout=new QVBoxLayout;
	QWidget*		hbox;
	QHBoxLayout	*hlayout;
	QLabel		*label;
	QPushButton	*button;
	QIcon		icon;

	this->findReplaceDialog=new QDialog(this->parent);
	this->findReplaceDialog->setWindowTitle("Find/Replace");
	vlayout->setContentsMargins(4,0,4,0);

//find
	label=new QLabel("Find");
	label->setAlignment(Qt::AlignCenter);
	vlayout->addWidget(label);

	this->findDropBox=new QComboBox(this->findReplaceDialog);
	this->findDropBox->setEditable(true);
	this->findDropBox->setCompleter(0);
	this->findDropBox->setInsertPolicy(QComboBox::InsertAtBottom);
	this->findDropBox->addItems(this->findList);
	this->findDropBox->setCurrentIndex(this->findDropBox->count()-1);
	vlayout->addWidget(this->findDropBox);

//replace
	label=new QLabel("Replace With");
	label->setAlignment(Qt::AlignCenter);
	vlayout->addWidget(label);

	this->replaceDropBox=new QComboBox(this->findReplaceDialog);
	this->replaceDropBox->setEditable(true);
	this->replaceDropBox->setCompleter(0);
	this->replaceDropBox->setInsertPolicy(QComboBox::InsertAtBottom);
	this->replaceDropBox->addItems(this->replaceList);
	this->replaceDropBox->setCurrentIndex(this->replaceDropBox->count()-1);
	vlayout->addWidget(replaceDropBox);

//switches 1st row
	hlayout=new QHBoxLayout;
	hlayout->setContentsMargins(0,0,0,0);
	hbox=new QWidget;
	hbox->setLayout(hlayout);

//case
	this->caseCheck=new QCheckBox("Case insensitive",this->findReplaceDialog);
	this->caseCheck->setChecked(this->insensitiveSearch);
	QObject::connect(this->caseCheck,&QCheckBox::checkStateChanged,[this](Qt::CheckState state)
		{
			this->insensitiveSearch=this->caseCheck->isChecked();
		});
	hlayout->addWidget(this->caseCheck);
//use regex
	this->regexCheck=new QCheckBox("Use Regex",this->findReplaceDialog);
	this->regexCheck->setChecked(this->useRegex);
	QObject::connect(this->regexCheck,&QCheckBox::checkStateChanged,[this](Qt::CheckState state)
		{
			this->useRegex=this->regexCheck->isChecked();
		});
	hlayout->addWidget(this->regexCheck);
//find after replace
	this->findAfterCheck=new QCheckBox("Find After Replace",this->findReplaceDialog);
	this->findAfterCheck->setChecked(this->findAfterReplace);
	QObject::connect(this->findAfterCheck,&QCheckBox::checkStateChanged,[this](Qt::CheckState state)
		{
			this->findAfterReplace=this->findAfterCheck->isChecked();
		});
	hlayout->addWidget(this->findAfterCheck);

	vlayout->addWidget(hbox);

//switches 2nd row
	hlayout=new QHBoxLayout;
	hlayout->setContentsMargins(0,0,0,0);
	hbox=new QWidget;
	hbox->setLayout(hlayout);
//wrap
	this->wrapCheck=new QCheckBox("Wrap",this->findReplaceDialog);
	this->wrapCheck->setChecked(this->wrapSearch);
	QObject::connect(this->wrapCheck,&QCheckBox::checkStateChanged,[this](Qt::CheckState state)
		{
			this->wrapSearch=this->wrapCheck->isChecked();
		});
	hlayout->addWidget(this->wrapCheck);

//all files
	this->multiDocumentCheck=new QCheckBox("Multi Files",this->findReplaceDialog);
	this->multiDocumentCheck->setChecked(this->findInMultDocument);
	QObject::connect(this->multiDocumentCheck,&QCheckBox::checkStateChanged,[this](Qt::CheckState state)
		{
			this->findInMultDocument=this->multiDocumentCheck->isChecked();
		});
	hlayout->addWidget(multiDocumentCheck);

//hilite all
	this->highlightAllCheck=new QCheckBox("Highlight All",this->findReplaceDialog);
	this->highlightAllCheck->setChecked(this->highlightAll);
	QObject::connect(this->highlightAllCheck,&QCheckBox::checkStateChanged,[this](Qt::CheckState state)
		{
			this->highlightAll=this->highlightAllCheck->isChecked();
			if(this->highlightAll==false)
				this->resetHighLights();
			else
				this->highLightAllMatches();
		});
	hlayout->addWidget(highlightAllCheck);

//rep all
	this->replaceAllCheck=new QCheckBox("Replace All",this->findReplaceDialog);
	this->replaceAllCheck->setChecked(this->replaceAll);
	QObject::connect(this->replaceAllCheck,&QCheckBox::checkStateChanged,[this](Qt::CheckState state)
		{
			this->replaceAll=this->replaceAllCheck->isChecked();
			if(this->replaceAll==false)
				this->replaceButton->setText("Replace");
			else
				this->replaceButton->setText("Replace All");
		});
	hlayout->addWidget(this->replaceAllCheck);

//search back
	this->backwardsCheck=new QCheckBox("Search Backwards",this->findReplaceDialog);
	this->backwardsCheck->setChecked(this->searchBack);
	QObject::connect(this->backwardsCheck,&QCheckBox::checkStateChanged,[this](Qt::CheckState state)
		{
			this->searchBack=this->backwardsCheck->isChecked();
			this->startPosition=-1;
			this->holdStartPos=-1;
			QTextCursor cursor=this->te->textCursor();

			int pos=cursor.position();
			if(this->searchBack==false)
				cursor.setPosition(pos-1);
			else
				cursor.setPosition(pos+1);
			this->te->setTextCursor(cursor);
		});
	hlayout->addWidget(this->backwardsCheck);

	vlayout->addWidget(hbox);

//switches 3rd row
	hlayout=new QHBoxLayout;
	hlayout->setContentsMargins(0,4,0,4);
	hbox=new QWidget;
	hbox->setLayout(hlayout);

	this->findButton=new QPushButton("Find",this->findReplaceDialog);
	this->findButton->setIcon(QIcon::fromTheme("edit-find"));
	QObject::connect(this->findButton,&QPushButton::clicked,[this]()
		{
			this->findButtonCB();
		});
	hlayout->addWidget(this->findButton);

	if(this->replaceAll==false)
		this->replaceButton=new QPushButton("Replace",this->findReplaceDialog);
	else
		this->replaceButton=new QPushButton("Replace All",this->findReplaceDialog);

	this->replaceButton->setIcon(QIcon::fromTheme("edit-find-replace"));
	QObject::connect(this->replaceButton,&QPushButton::clicked,[this]()
		{
			this->replaceButtonCB();
		});
	hlayout->addWidget(this->replaceButton);

	button=new QPushButton("Close",this->findReplaceDialog);
	button->setIcon(QIcon::fromTheme("window-close"));
	QObject::connect(button,&QPushButton::clicked,[this,button]()
		{
			this->hideDialog();
		});
	hlayout->addWidget(button);

	vlayout->addWidget(hbox);

	this->findReplaceDialog->setLayout(vlayout);

	if(prefs.contains("find/findgeometry"))
		this->findReplaceDialog->restoreGeometry(prefs.value("find/findgeometry").toByteArray());
}

bool QT_FindClass::checkForMatch(void)
{
	QRegularExpression	rex;
	QString				docstr=this->te->toPlainText();

	rex=this->getRegex();
	if(docstr.indexOf(rex,0)==-1)
		return(false);

	return(true);
}

void QT_FindClass::highLightAllMatches(void)
{
	QList<QTextEdit::ExtraSelection>	extraSelections;
	QTextCharFormat					format;
	QTextEdit::ExtraSelection		selection;
	QRegularExpression				rex;
	QString							docstr=this->te->toPlainText();
	qsizetype						posstart=-1;

	rex=this->getRegex();
	if(rex.pattern().isEmpty()==true)
		return;

	extraSelections.clear();
	if(this->autoHighlightAll==false)
		extraSelections=this->te->extraSelections();
	format.setBackground(QColor(this->hiliteAllColour));
	selection.format=format;
	selection.cursor=te->textCursor();
	selection.cursor.clearSelection();

	posstart=docstr.indexOf(rex,0,&this->match);
	while(posstart!=-1)
		{
			selection.cursor.setPosition(posstart,QTextCursor::MoveAnchor);
			selection.cursor.movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor,this->match.capturedLength());
			posstart=docstr.indexOf(rex,posstart+this->match.capturedLength(),&this->match);
			extraSelections.append(selection);
		}

	this->te->setExtraSelections(extraSelections);
}

void QT_FindClass::resetSearchPositions(void)
{
	QTextCursor	cursor;

	cursor=this->te->textCursor();
	if(this->searchBack==false)
		{
			cursor.movePosition(QTextCursor::Start);
		}
	else
		{
			cursor.movePosition(QTextCursor::End);
		}
	this->te->setTextCursor(cursor);
	this->holdCursor=QTextCursor();
}

bool QT_FindClass::findString(bool replace)
{
	QTextCursor							cursor;
	QRegularExpression					rex;
	QString								docstr=this->te->toPlainText();
	qsizetype							got;

	rex=this->getRegex();
	if(this->checkForMatch()==false && this->findInMultDocument==true)
		{
			this->resetHighLights();
			this->findWrapped.triggered();
			return(false);
		}

	if(this->checkForMatch()==false && this->findInMultDocument==false)
		{
			this->resetHighLights();
			return(false);
		}

	if(this->searchBack==true)
		{
			bool skiptherest=false;
			got=docstr.lastIndexOf(rex,this->te->textCursor().position(),&this->match);
			got=docstr.lastIndexOf(rex,this->te->textCursor().position()-match.capturedLength()-1,&this->match);

			if(this->findInMultDocument==false && this->wrapSearch==true)
				{
					if(got==-1)
						got=docstr.lastIndexOf(rex,docstr.length()-this->match.capturedLength(),&this->match);
					skiptherest=true;
				}

			if(skiptherest==false && this->te->textCursor().position()-this->match.capturedLength()<=0)
				{
					if(this->findInMultDocument==false)
						return(false);
					else
						{
							this->resetHighLights();
							this->findWrapped.triggered();
							return(false);
						}
				}

			if(skiptherest==false && got<0 && this->findInMultDocument==true)
				{
					this->resetHighLights();
					this->findWrapped.triggered();
					return(false);
				}
		}

	if(this->searchBack==false)
		{
			got=docstr.indexOf(rex,this->te->textCursor().position(),&this->match);
			if(got==-1)
				{
					if(this->findInMultDocument==false && this->wrapSearch==true)
						got=docstr.indexOf(rex,0,&this->match);

					if(this->findInMultDocument==true)
						{
							this->resetHighLights();
							this->findWrapped.triggered();
							return(false);							
						}
				}
		}

	if(got!=-1)
		{
			this->holdCursor=this->te->textCursor();
			this->holdCursor.setPosition(got,QTextCursor::MoveAnchor);
			this->holdCursor.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,this->match.capturedLength());
			this->te->setTextCursor(this->holdCursor);
		}

	return(true);
}

void QT_FindClass::resetHighLights(void)
{
	QList<QTextEdit::ExtraSelection>	extraSelections;

	this->te->setExtraSelections(extraSelections);
}

QRegularExpression QT_FindClass::getRegex(void)
{
	QRegularExpression					rex;
	QRegularExpression::PatternOptions	opts=QRegularExpression::InvertedGreedinessOption;

	if(this->insensitiveSearch==true)
		opts|=QRegularExpression::CaseInsensitiveOption;

	rex=QRegularExpression(this->targetString,opts);

	if(this->useRegex==false)
		rex=QRegularExpression(QRegularExpression::escape(this->targetString));

	return(rex);
}
