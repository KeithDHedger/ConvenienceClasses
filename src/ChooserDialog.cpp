
#include "ChooserDialog.h"

chooserDialogClass::~chooserDialogClass()
{
}

void chooserDialogClass::setShowImagesInList(bool show)
{
	this->showThumbsInList=show;
	this->setFileList();
}

void chooserDialogClass::setMultipleSelect(bool select)
{
	if(this->saveDialog==true)
		return;

	this->useMulti=select;
	if(select==true)
		this->fileList.setSelectionMode(QAbstractItemView::ExtendedSelection);
	else
		this->fileList.setSelectionMode(QAbstractItemView::SingleSelection);
}

QIcon chooserDialogClass::getFileIcon(QString path)
{
	if(QFileInfo::exists(path)==false)
		return(QIcon::fromTheme("application-octet-stream"));

	QIcon				icon;
	QMimeDatabase		db;
	QString				realpath(QFileInfo(path).canonicalFilePath());
    QMimeType			type=db.mimeTypeForFile(realpath);

	if(type.name().compare("application/x-desktop")==0)
		{
			QIcon::setFallbackSearchPaths(QIcon::fallbackSearchPaths() << "/usr/share/pixmaps");
			QSettings df(realpath,QSettings::IniFormat);
			icon=QIcon::fromTheme(df.value("Desktop Entry/Icon").toString());
		}
	else
		{
			if(type.name().contains("image"))
				{
					if((QFileInfo(path).size()<MAXIMAGESIZETOTHUMB) && (this->showThumbsInList==true))
						icon=QIcon(path);
					else
						icon=QIcon::fromTheme(type.iconName(),QIcon::fromTheme("image"));
				}
			else
				{
					icon=QIcon::fromTheme(type.iconName());
				}
		}
	if(icon.isNull()==true)
		icon=QIcon::fromTheme("application-octet-stream");
	return(icon);
}

void chooserDialogClass::setFileList(void)
{
	QStandardItem	*item=NULL;
	QStringList		namefilters;
	QDir				d=this->rawPath;
	QDir::Filters	dfilts=QDir::System|QDir::Dirs|QDir::NoDot;
	QFileInfoList	fl;

	if(this->rawPath==this->recentFilesPath || this->rawPath==this->recentFoldersPath)
		dfilts|=QDir::NoDotDot;

	if(this->showHidden==true)
		dfilts|=QDir::Hidden;

	if(this->fileTypes.currentText()=="All Files")
		namefilters.clear();
	else
		namefilters=this->fileTypes.currentText().split(';');
		
	fl=d.entryInfoList(QStringList(),dfilts,QDir::Name);

	this->currentFolder=this->rawPath;

	if(this->dialogType!=chooserDialogType::folderDialog)
		{
			dfilts=QDir::Files|QDir::NoDot;
			if(this->showHidden==true)
				dfilts|=QDir::Hidden;

			fl.append(d.entryInfoList(namefilters,dfilts,QDir::Name));
		}

	this->fileListModel->clear();
	for(int j=0;j<fl.size();j++)
		{
			item=NULL;
			
			if(fl[j].isSymLink() && (fl[j].isFile() || fl[j].isDir()))
				{
					item=new QStandardItem(this->getFileIcon(fl[j].canonicalFilePath()),QString("%1->%2").arg(fl[j].fileName()).arg(fl[j].symLinkTarget()));
					item->setFont(QFont(item->font().family(),-1,QFont::Bold));	
				}
			else if(fl[j].isSymLink() && fl[j].exists()==false)
				{
					item=new QStandardItem(this->getFileIcon(fl[j].canonicalFilePath()),QString("%1 - Broken Link").arg(fl[j].fileName()));
					item->setFont(QFont(item->font().family(),-1,QFont::Bold));
				}
			else
				{
					if(fl[j].isDir()==false && this->dialogType==chooserDialogType::folderDialog)
						continue;
					item=new QStandardItem(this->getFileIcon(fl[j].canonicalFilePath()),fl[j].fileName());
				}

			if(item!=NULL)
				{
					if((fl[j].isSymLink() && fl[j].isDir()) || (fl[j].isDir()))
						item->setDragEnabled(true);
					else
						item->setDragEnabled(false);

					item->setData(fl[j].fileName(),Qt::UserRole);
					item->setStatusTip(fl[j].canonicalFilePath());
					this->fileListModel->appendRow(item);
				}
		}

	this->fileList.scrollToTop();
}

void chooserDialogClass::setSideList(void)
{
	QVariant				fullFilePathData;
	QStorageInfo			storage;
	QStandardItem		*item;
	QList<QStorageInfo>	ml=QStorageInfo::mountedVolumes();
	QString				disktype;
	QSettings			prefs("KDHedger","ChooserDialog");
	QStringList			sl=prefs.value("customfolders").toStringList();

///standard items
	this->sideListModel->clear();
	item=new QStandardItem(QIcon::fromTheme("computer"),"Computer");
	fullFilePathData="/";
	item->setData(fullFilePathData,Qt::UserRole);
	this->sideListModel->appendRow(item);
	fullFilePathData=QDir::homePath();
	item=new QStandardItem(QIcon::fromTheme("user-home"),QFileInfo(QDir().homePath()).baseName());
	item->setData(fullFilePathData,Qt::UserRole);
	this->sideListModel->appendRow(item);
	
//recent folders
	item=new QStandardItem(QIcon::fromTheme("folder-saved-search"),"Recent Folders");
	fullFilePathData=this->recentFoldersPath;
	item->setData(fullFilePathData,Qt::UserRole);
	this->sideListModel->appendRow(item);

//recent files

//recent files
	if(this->dialogType!=chooserDialogType::folderDialog)
		{
			item=new QStandardItem(QIcon::fromTheme("folder-saved-search"),"Recent Files");
			fullFilePathData=this->recentFilesPath;
			item->setData(fullFilePathData,Qt::UserRole);
			this->sideListModel->appendRow(item);
		}
//
//
//	item=new QStandardItem(QIcon::fromTheme("folder-saved-search"),"Recent Files");
//	fullFilePathData=this->recentFilesPath;
//	item->setData(fullFilePathData,Qt::UserRole);
//	this->sideListModel->appendRow(item);

	item=new QStandardItem("");
	item->setEnabled(false);
	this->sideListModel->appendRow(item);

//mounted drives	 
	for(int j=0;j<ml.size();j++)
		{
			storage=ml.at(j);
			if((storage.fileSystemType().compare("tmpfs")!=0) && (storage.rootPath().compare("/")!=0) && (storage.rootPath().compare(QDir().homePath())!=0))
				{
					disktype="drive-harddisk";
					if(storage.fileSystemType().contains("nfs"))
						disktype="folder-remote";
					if(storage.fileSystemType().contains("ssh"))
						disktype="network_local";
					fullFilePathData=storage.rootPath();
					item=new QStandardItem(QIcon::fromTheme(disktype),QFileInfo(storage.rootPath()).baseName());
					item->setData(fullFilePathData,Qt::UserRole);
					this->sideListModel->appendRow(item);
				}
		}
	item=new QStandardItem("");
	item->setEnabled(false);
	this->sideListModel->appendRow(item);

//favs
	for(int j=0;j<sl.size();j++)
		{
			item=new QStandardItem(QIcon::fromTheme("user-bookmarks"),QFileInfo(sl.at(j)).fileName());
			//item->setData(QFileInfo(sl.at(j)).fileName(),Qt::UserRole);
			item->setData(sl.at(j),Qt::UserRole);
			item->setStatusTip(sl.at(j));
			this->sideListModel->appendRow(item);	
		}
}

void chooserDialogClass::showPreViewData(void)
{
	QIcon			icon;
	QPixmap			pixmap;
	QMimeDatabase	db;
	QString			mod;
	QMimeType		type;;
	int				md;
	struct stat		sb;

	//if(this->selectedFilePath.isEmpty()==true)
	if(this->lastSelectedFilePath.isEmpty()==true)
		return;

	//type=db.mimeTypeForFile(this->selectedFilePath);
	type=db.mimeTypeForFile(this->lastSelectedFilePath);
	this->previewMimeType.setText(type.name());

	if(type.name().contains("image"))
		{
			//pixmap=QIcon(this->selectedFilePath).pixmap(QSize(128,128));
			pixmap=QIcon(this->lastSelectedFilePath).pixmap(QSize(128,128));
			if(pixmap.isNull()==true)
				{
					icon=QIcon::fromTheme(type.iconName(),QIcon::fromTheme("image"));
					pixmap=icon.pixmap(QSize(128,128));
				}
		}
	else
		{
			//icon=this->getFileIcon(this->selectedFilePath);
			icon=this->getFileIcon(this->lastSelectedFilePath);
			pixmap=icon.pixmap(QSize(128,128));
		}

	this->previewIcon.setPixmap(pixmap);
	//this->previewSize.setText(QString("Size: %1").arg(QFileInfo(this->selectedFilePath).size()));
	this->previewSize.setText(QString("Size: %1").arg(QFileInfo(this->lastSelectedFilePath).size()));

	//if(lstat(this->selectedFilePath.toStdString().c_str(),&sb)!=-1)
	if(lstat(this->lastSelectedFilePath.toStdString().c_str(),&sb)!=-1)
		{
			md=sb.st_mode & 07777;
			mod.setNum(md,8);
			this->previewMode.setText(QString("Mode: %1").arg(mod));
		}
}

void chooserDialogClass::selectItem(const QModelIndex &index)
{
	QString t;
	t=this->rawPath+"/"+index.data(Qt::UserRole).toString();
//	if(this->saveDialog==false)
//		{
//			this->filepathEdit.setText(t);
//		}
//	else
//		{
//			if(QFileInfo(t).isDir()==false)
//				this->filepathEdit.setText(index.data(Qt::UserRole).toString());
//		}

		this->selectedFileName=index.data(Qt::UserRole).toString();
	//this->selectedFilePath=t;
	this->lastSelectedFilePath=t;
	//qDebug()<<"9999"<<this->selectedFilePath<<this->lastSelectedFilePath;
	//this->realFilePath=QFileInfo(this->selectedFilePath).canonicalFilePath();
	//this->realFolderPath=QFileInfo(this->realFilePath).canonicalPath();
	//this->realName=QFileInfo(this->realFilePath).fileName();;

//qDebug()<<"---------------------"<<this->currentFolder<<index.data(Qt::UserRole).toString();
this->selectedFolder=this->currentFolder+"/"+index.data(Qt::UserRole).toString();
this->selectedFolder=this->getProperPath(this->selectedFolder);
	this->showPreViewData();
}

void chooserDialogClass::selectSideItem(const QModelIndex &index)
{
	QList<QStorageInfo>		ml=QStorageInfo::mountedVolumes();
	QStorageInfo				storage;
	QString					disktype;
	QPixmap					pixmap;
	QIcon					icon; 
	QLocale					locale;
	QString					type;
	qint64					sze=0;
	qint64					freeb=0;
	const QAbstractItemModel	*model;
	QMap<int,QVariant>		map;
	int						itemoffset=2;

	disktype="drive-harddisk";

	switch(index.row())
		{
			case 0:
				disktype="computer";
				storage=ml.at(index.row());
				sze=storage.bytesTotal();
				type=storage.fileSystemType();
				freeb=storage.bytesFree();
				break;
			case 1:
				disktype="user-home";
				type="";
				sze=0;
				freeb=0;
				break;
			case 2:
				disktype="folder-saved-search";
				type="";
				sze=0;
				freeb=0;
				break;
			case 3:
				disktype="folder-saved-search";
				type="";
				sze=0;
				freeb=0;
				break;
			case 4:
				return;
			default:
				if((index.row()-itemoffset)<ml.size())
					{
						storage=ml.at(index.row()-itemoffset);
						if((storage.rootPath().compare("/")!=0) && (storage.rootPath().compare(QDir().homePath())!=0))
							{
								if(storage.fileSystemType().contains("nfs"))
									disktype="folder-remote";
								if(storage.fileSystemType().contains("ssh"))
									disktype="network_local";	
								sze=storage.bytesTotal();
								type=storage.fileSystemType();
								freeb=storage.bytesFree();
							}
					}
				else
					{
						struct stat	sb;
						int			md;
						QString		mod;
						QString		str;

						model=index.model();
						map=model->itemData(index);
						if(map.find(Qt::StatusTipRole)!=map.end())
							{
								str=map[Qt::StatusTipRole].toString();
								if(str.isEmpty()==true)
									return;
								icon=QIcon::fromTheme("folder");
								pixmap=icon.pixmap(QSize(128,128));
								this->previewIcon.setPixmap(pixmap);
								this->previewSize.setText(QString("Size: %1").arg(QFileInfo(str).size()));
								this->previewMimeType.setText("inode/directory");
								if(lstat(str.toStdString().c_str(),&sb)!=-1)
									{
										md=sb.st_mode & 07777;
										mod.setNum(md,8);
										this->previewMode.setText(QString("Mode: %1").arg(mod));
									}
							}
						return;
					}
		}
	icon=QIcon::fromTheme(disktype);
	pixmap=icon.pixmap(QSize(128,128));
	this->previewIcon.setPixmap(pixmap);
	this->previewSize.setText(QString("Size: %1").arg(locale.formattedDataSize(sze)));
	this->previewMimeType.setText(QString("FS Type: %1").arg(type));
	this->previewMode.setText(QString("Free: %1").arg(locale.formattedDataSize(freeb)));
}

void chooserDialogClass::setFavs(void)
{
	QSettings			prefs("KDHedger","ChooserDialog");
	QStringList			sl;
	QItemSelectionModel	*model;
	QModelIndexList		list;
	QString				filepath;

	this->sideList.setSelectionMode(QAbstractItemView::ExtendedSelection);
	this->sideList.selectAll();
	model=this->sideList.selectionModel();
	list=model->selectedIndexes();

	for(int j=0;j<list.count();j++)
		{
			filepath=QFileInfo(list.at(j).data(Qt::StatusTipRole).toString()).absoluteFilePath();
			if(filepath.isEmpty()==false)
				sl<<filepath;
		}
	prefs.setValue("customfolders",sl);
}

void chooserDialogClass::setFileData(void)
{
	QString				fp;
	QItemSelectionModel	*model;
	QModelIndexList		list;
	QString				filepath;
	QSettings			prefs("KDHedger","ChooserDialog");
	QDir					tdir;
	QString				recent;
	
	model=this->fileList.selectionModel();
	list=model->selectedIndexes();
	this->multiFileList.clear();

	if(list.count()>0)
		{
			for(int j=0;j<list.count();j++)
				{
					if(this->dialogType==chooserDialogType::saveDialog)
						filepath=this->currentFolder+"/"+filepathEdit.text();
					else
						filepath=this->currentFolder+"/"+list.at(j).data(Qt::UserRole).toString();
					filepath=this->getProperPath(filepath);
					this->setRecents(filepath);
					if(QFileInfo(filepath).isDir()==false)
						this->multiFileList.push_back(filepath);
				}
		}
	else
		{
			if(this->dialogType==chooserDialogType::folderDialog)
			{
			qDebug()<<this->selectedFolder<<this->currentFolder<<QDir::cleanPath(this->currentFolder);
			filepath=this->currentFolder;
			filepath=this->getProperPath(filepath);
			this->setRecents(filepath);
			this->multiFileList.push_back(filepath);
			this->selectedFolder=filepath;
			qDebug()<<"filepath"<<filepath;
		
}
else
{
			filepath=this->currentFolder+"/"+filepathEdit.text();
			filepath=this->getProperPath(filepath);
			this->setRecents(filepath);
			this->multiFileList.push_back(filepath);
}
		}

	prefs.setValue("choosersize",this->dialogWindow.saveGeometry());

	switch(this->dialogType)
		{
			case chooserDialogType::openDialog:
				this->setLast(this->rawPath);
				prefs.setValue("lastloadfolder",QFileInfo(this->rawPath).path());
				break;				
			case chooserDialogType::saveDialog:
				prefs.setValue("lastsavefolder",this->currentFolder);
				break;
			case chooserDialogType::folderDialog:
				prefs.setValue("lastloadfolder",this->selectedFolder);
				break;
		}

	this->rawPath+="/"+this->selectedFileName;

	this->setFavs();
	this->valid=true;
}

void chooserDialogClass::setRecents(QString str)
{
	QString	tstr=this->getProperPath(str);
	if(QFileInfo(tstr).isDir()==true)
		{
			QFile	recentfolder(tstr);
			QString	recent=QString("%1/%2").arg(this->recentFoldersPath).arg(QFileInfo(tstr).fileName());
			recentfolder.link(recent);
		}
	else
		{
			QFile	recentfile(tstr);
			QFile	recentfolder(QFileInfo(tstr).path());
			QString	recent=QString("%1/%2").arg(this->recentFilesPath).arg(QFileInfo(tstr).fileName());

			recentfile.link(recent);
			recent=QString("%1/%2").arg(this->recentFoldersPath).arg(QFileInfo(tstr).dir().dirName());
			recentfolder.link(recent);
		}
}

void chooserDialogClass::setLast(QString str)
{
	QSettings	prefs("KDHedger","ChooserDialog");
	QString		tstr=this->getProperPath(str);

	if(this->dialogType==chooserDialogType::openDialog)
		prefs.setValue("lastloadfolder",QFileInfo(tstr).path());
}

void chooserDialogClass::addFileTypes(QString types)
{
	this->fileTypes.addItem(types);
}

void chooserDialogClass::buildMainGui(void)
{
	QVBoxLayout	*windowvlayout=new QVBoxLayout;
	QVBoxLayout	*sidevlayout=new QVBoxLayout;
	QVBoxLayout	*infovlayout=new QVBoxLayout;
	QVBoxLayout	*controlsvlayout=new QVBoxLayout;
	QHBoxLayout	*hlayout=new QHBoxLayout;
	QSplitter	*splitter=new QSplitter(Qt::Horizontal,&this->dialogWindow);

	switch(this->dialogType)
		{
			case chooserDialogType::openDialog:
				this->dialogWindow.setWindowTitle("Open File");
				break;
			case chooserDialogType::saveDialog:
				this->dialogWindow.setWindowTitle("Save File");
				break;
			case chooserDialogType::folderDialog:
				this->dialogWindow.setWindowTitle("Select Folder");
				break;
		}

	this->fileListModel=new QStandardItemModel(0,1);
    this->fileList.setModel(this->fileListModel);
	QObject::connect(&this->fileList,&QListView::clicked,[this](const QModelIndex &index)
		{
			if(QFileInfo( this->rawPath+"/"+index.data(Qt::UserRole).toString()).isFile()==true)
				this->filepathEdit.setText(index.data(Qt::UserRole).toString());
			else
				this->filepathEdit.setText("");

			this->selectItem(index);
		});

	//this->fileList.setStyleSheet(QString("QFrame {border-width: 1px;border-color: palette(dark); border-style: solid;}"));
	QObject::connect(&this->fileList,&QListView::doubleClicked,[this](const QModelIndex &index)
		{
				if(index.data(Qt::UserRole).toString().compare("..")==0)
					{
						this->rawPath=QFileInfo(this->rawPath).path();
					}
				else
					{
						this->rawPath+="/"+index.data(Qt::UserRole).toString();
						this->rawPath=this->getProperPath(this->rawPath);

						if(QFileInfo(this->rawPath).isFile()==true)
							{
								if(this->dialogType==chooserDialogType::saveDialog)
									{
										if((QFileInfo(this->rawPath).exists()==true) && (this->overwriteWarning==true) && (this->saveDialog==true))
											{
												QMessageBox::StandardButton reply=QMessageBox::warning(&this->dialogWindow,"File exists",QString("File '%1' exists! Overwrite?").arg(this->rawPath),QMessageBox::Yes|QMessageBox::No);
												if(reply==QMessageBox::No)
													{
														this->rawPath=this->currentFolder;
														this->setFileList();
														return;
													}
											}
									}

								this->setFileData();
								this->dialogWindow.hide();
							}
					}
				
				this->setFileList();
				return;
		});

//sidelist
	QObject::connect(&this->sideList,&QListView::clicked,[this](const QModelIndex &index)
		{
				this->selectSideItem(index);
		});

	QObject::connect(&this->sideList,&QListView::doubleClicked,[this](const QModelIndex &index)
		{
			const QAbstractItemModel	*model;
			model=index.model();
			QMap map(model->itemData(index));
			if(map.find(Qt::StatusTipRole)!=map.end())
				{
					QString str=map[Qt::StatusTipRole].toString();
					this->rawPath=QFileInfo(str).absoluteFilePath();
				}
			else
				this->rawPath=index.data(Qt::UserRole).toString();
			this->rawPath=index.data(Qt::UserRole).toString();
			this->setFileList();
			if(this->saveDialog==false)
				this->filepathEdit.setText("");
		});

	this->sideListModel=new QStandardItemModel(0,1);
    this->sideList.setModel(this->sideListModel);
	this->sideList.setEditTriggers(QAbstractItemView::NoEditTriggers);
	this->fileList.setEditTriggers(QAbstractItemView::NoEditTriggers);

	QObject::connect(splitter,&QSplitter::splitterMoved,[this,splitter](int pos, int index)
		{
			QSettings	prefs("KDHedger","ChooserDialog");
			prefs.setValue("splittersize", splitter->saveState());
		});

	QPushButton *deletefav=new QPushButton("Remove Fav");
	deletefav->setIcon(QIcon::fromTheme("stock_cancel"));
	QObject::connect(deletefav,&QPushButton::clicked,[this]()
		{
			QModelIndex ind=this->sideList.currentIndex();
			if(ind.data(Qt::StatusTipRole).toString().isEmpty()==false)
				this->sideListModel->removeRow(this->sideList.currentIndex().row());
		});
	sidevlayout->addWidget(&this->sideList);
	sidevlayout->addWidget(deletefav);

	this->previewIcon.setMaximumWidth(128);
	this->previewIcon.setMinimumWidth(128);
	this->previewIcon.setAlignment(Qt::AlignCenter);
	this->previewMimeType.setWordWrap(true);
	infovlayout->addWidget(&this->previewIcon);
	infovlayout->addWidget(&this->previewMimeType);
	infovlayout->addWidget(&this->previewSize);
	infovlayout->addWidget(&this->previewMode);
	infovlayout->addStretch();

	QWidget *wrapper=new QWidget(&this->dialogWindow);
	sidevlayout->setContentsMargins(0,0,0,0);
	wrapper->setLayout(sidevlayout);
	splitter->addWidget(wrapper);
	splitter->addWidget(&this->fileList);
	
	hlayout->addWidget(splitter);
	hlayout->addLayout(infovlayout);

	windowvlayout->addLayout(hlayout);

	controlsvlayout->addWidget(&this->filepathEdit);
	QObject::connect(&this->filepathEdit,&QLineEdit::textChanged,[this](const QString &text)
		{
			////this->selectedFilePath=text;
		});

	if(this->dialogType!=chooserDialogType::folderDialog)
		{
			controlsvlayout->addWidget(&this->fileTypes);
			QObject::connect(&this->fileTypes,&QComboBox::currentTextChanged,[this](const QString &text)
				{
					this->setFileList();
				});
		}

	hlayout=new QHBoxLayout;
	QPushButton *cancel=new QPushButton("Cancel");
	cancel->setIcon(QIcon::fromTheme("dialog-cancel"));
	QObject::connect(cancel,&QPushButton::clicked,[this]()
		{
			QSettings	prefs("KDHedger","ChooserDialog");

			prefs.setValue("choosersize",this->dialogWindow.saveGeometry());
			this->dialogWindow.hide();
			this->setFavs();
			this->valid=false;
		});

	QPushButton *hidden=new QPushButton("Hidden");
	hidden->setCheckable(true);
	hidden->setIcon(QIcon::fromTheme("dialog-question"));
	QObject::connect(hidden,&QPushButton::clicked,[this]()
		{
			this->showHidden=!this->showHidden;
			this->setFileList();
		});

	QPushButton *newfolder=new QPushButton("New Folder");
	newfolder->setIcon(QIcon::fromTheme("folder-open"));
	QObject::connect(newfolder,&QPushButton::clicked,[this]()
		{
			bool		ok;
			int		cnt=1;
			QString	nfname="New Folder";
			if(QFileInfo::exists(QString("%1/%2").arg(this->currentFolder).arg(nfname)))
				{
					while(QFileInfo::exists(QString("%1/%2-%3").arg(this->currentFolder).arg(nfname).arg(cnt)))
						cnt++;
					nfname=QString("%1-%2").arg(nfname).arg(cnt);
				}
     		QString	text=QInputDialog::getText(&this->dialogWindow,"New Folder","New folder name",QLineEdit::Normal,nfname,&ok);
			if(ok==true)
				{
					QDir dirp(this->currentFolder);
					dirp.mkdir(text);
					this->setFileList();
				}
		});

	QPushButton *refresh=new QPushButton("Refresh");
	refresh->setIcon(QIcon::fromTheme("refresh"));
	QObject::connect(refresh,&QPushButton::clicked,[this]()
		{
			this->setFileList();
		});

	QPushButton *apply;
	//if(this->saveDialog==false)
//		apply=new QPushButton("Open");
//	else
//		apply=new QPushButton("Save");
	switch(this->dialogType)
		{
			case chooserDialogType::openDialog:
				apply=new QPushButton("Open");
				break;				
			case chooserDialogType::saveDialog:
				apply=new QPushButton("Save");
				break;
			case chooserDialogType::folderDialog:
				apply=new QPushButton("Select");
				break;
		}
	apply->setIcon(QIcon::fromTheme("dialog-ok"));
	apply->setDefault(true);

	QObject::connect(apply,&QPushButton::clicked,[this]()
		{
			switch(this->dialogType)
				{
					case chooserDialogType::openDialog:
						this->lastSelectedFilePath=this->getProperPath(this->lastSelectedFilePath);

						if(QFileInfo(this->lastSelectedFilePath).isDir()==true)
							{
								this->currentFolder=this->lastSelectedFilePath;
								this->rawPath=this->lastSelectedFilePath;
								this->setFileList();	
							}
						else
							{
								this->rawPath=this->lastSelectedFilePath;
								this->setFileData();
								this->dialogWindow.hide();
							}
						break;				
					case chooserDialogType::saveDialog:
						{
							this->lastSelectedFilePath=this->getProperPath(this->lastSelectedFilePath);
							if(this->lastSelectedFilePath.isEmpty()==true)
								{
									this->lastSelectedFilePath=this->currentFolder+"/"+this->filepathEdit.text();
									this->rawPath=this->lastSelectedFilePath;
								}
							
							if((QFileInfo(this->lastSelectedFilePath).exists()==true) && (this->overwriteWarning==true) && (this->saveDialog==true))
								{
									QMessageBox::StandardButton reply=QMessageBox::warning(&this->dialogWindow,"File exists",QString("File '%1' exists! Overwrite?").arg(this->lastSelectedFilePath),QMessageBox::Yes|QMessageBox::No);
									if(reply==QMessageBox::No)
										return;
								}
							this->setFileData();
							this->dialogWindow.hide();
						}
						break;

					case chooserDialogType::folderDialog:
						this->setFileData();
						this->setRecents(this->selectedFolder);
						this->dialogWindow.hide();
						break;
				}
		return;
			//if(this->filepathEdit.text().isEmpty()==true)
			//	return;
//
//			if((QFileInfo(this->selectedFilePath).isDir()==true) && (this->dialogType!=chooserDialogType::folderDialog))
//				{
//					this->localWD=selectedFilePath;
//					this->setFileList();	
//				}
//			else
//				{
//					QString tp;
//					if(this->filepathEdit.text().isEmpty()==true)
//						return;
//					if(this->filepathEdit.text().at(0)=='/')
//						{
//							tp=this->filepathEdit.text();
//							//this->localWD=QFileInfo(tp).dir().absoluteFilePath();
//							//QDir
//							//this->localWD=QFileInfo(tp).dir().canonicalPath();
//							this->localWD=QFileInfo(tp).dir().absolutePath();
//							//QDir td=QFileInfo(tp).dir();
//							//qDebug()<<td.canonicalPath()<<td.absolutePath();
//						}
//					else		
//						{
//							tp=QString("%1/%2").arg(this->localWD).arg(this->filepathEdit.text());
//						}
//
//					if((QFileInfo(tp).exists()==true) && (this->overwriteWarning==true) && (this->saveDialog==true))
//						{
//							QMessageBox::StandardButton reply=QMessageBox::question(&this->dialogWindow,"File exists","File exists! Overwrite?",QMessageBox::Yes|QMessageBox::No);
//							if(reply==QMessageBox::No)
//								return;
//						}
//					this->setFileData();
//					qDebug()<<"raw path="<<this->rawPath;
//					this->dialogWindow.hide();
//				}
		});

	hlayout->addWidget(cancel);
	hlayout->addStretch();
	hlayout->addWidget(hidden);
	hlayout->addStretch();
	hlayout->addWidget(newfolder);
	hlayout->addStretch();
	hlayout->addWidget(refresh);
	hlayout->addStretch();
	hlayout->addWidget(apply);
	controlsvlayout->addLayout(hlayout);
	windowvlayout->addLayout(controlsvlayout);

	this->dialogWindow.setLayout(windowvlayout);
	this->setSideList();
	this->setFileList();
	if(this->saveDialog==false)
		this->filepathEdit.setText("");
	else
		{
//			//this->selectedFilePath=this->localWD+"/"+this->saveName;
//			this->selectedFilePath=this->rawPath+"/"+this->saveName;
//			this->selectedFileName=this->saveName;
//			this->filepathEdit.setText(this->saveName);
		}
	this->fileList.setDragEnabled(true);
	this->sideList.setAcceptDrops(true);

	QSettings	prefs("KDHedger","ChooserDialog");
	splitter->restoreState(prefs.value("splittersize").toByteArray());
}

chooserDialogClass::chooserDialogClass(chooserDialogType type,QString name,QString startfolder)
{
	QSettings	prefs("KDHedger","ChooserDialog");
	QDir			folders("/");
	QString		command;

	this->recentFoldersPath=QString("%1/.config/KDHedger/recentfolders").arg(QDir::homePath());
	this->recentFilesPath=QString("%1/.config/KDHedger/recentfiles").arg(QDir::homePath());
	folders.mkpath(this->recentFoldersPath);
	folders.mkpath(this->recentFilesPath);

	if(type==chooserDialogType::saveDialog)
		{
			this->saveDialog=true;
			
			if(name.isEmpty()==false)
				this->saveName=name;
			else
				this->saveName="Untitled";
			if(startfolder.isEmpty()==true)
				//this->localWD=prefs.value("lastsavefolder").toString();
				this->rawPath=prefs.value("lastsavefolder").toString();
			else
				//this->localWD=startfolder;
				this->rawPath=startfolder;
		}

	if(type==chooserDialogType::openDialog)
		{
			if(name.isEmpty()==true)
				this->rawPath=prefs.value("lastloadfolder").toString();
			else
				this->rawPath=name;

			this->saveDialog=false;
			this->saveName="";
		}

	if(type==chooserDialogType::folderDialog)
		{
			if(name.isEmpty()==true)
				this->rawPath=prefs.value("lastloadfolder").toString();
			else
				this->rawPath=name;

			this->saveDialog=false;
			this->saveName="";
		}

	this->dialogType=type;

	if((this->rawPath.isEmpty()==true) || (QFileInfo(this->rawPath).exists()==false))
		this->rawPath="/";

	this->buildMainGui();

	this->dialogWindow.restoreGeometry(prefs.value("choosersize").toByteArray());

	command=QString("cd %1/ >/dev/null;ls -t1|tail -n +%2| xargs -I {} rm '{}'").arg(this->recentFilesPath).arg(this->maxRecents);
	system(command.toStdString().c_str());
	command=QString("cd %1 >/dev/null;ls -t1|tail -n +%2| xargs -I {} rm '{}'").arg(this->recentFoldersPath).arg(this->maxRecents);
	system(command.toStdString().c_str());
}

QString chooserDialogClass::getProperPath(QString str)
{
	if(QFileInfo(str).isSymLink())
		{
			if(str.startsWith(this->recentFilesPath) || str.startsWith(this->recentFoldersPath))
				return(QFileInfo(str).symLinkTarget());
		}

	return(str);
}
