
#ifndef _CHOOSERDIALOG_
#define _CHOOSERDIALOG_

#include "globals.h"

#define MAXIMAGESIZETOTHUMB 2000000

enum class chooserDialogType{saveDialog,openDialog,folderDialog};

class chooserDialogClass
{
	public:
		chooserDialogClass(chooserDialogType type,QString savename="",QString startfolder="");
		~chooserDialogClass();

		QDialog				dialogWindow;
		QVector<QString>		multiFileList;
		QString				selectedFolder;

		bool					fileExists=false;
		bool					useMulti=false;
		bool					valid=false;

		void					setShowImagesInList(bool show=false);
		void					setMultipleSelect(bool select);
		void					addFileTypes(QString types);

	private:
	
		QString				rawPath;
		QString				lastSelectedFilePath;
		QString				selectedFileName;
		QLineEdit			filepathEdit;
		QComboBox			fileTypes;

		QListView			fileList;
		QStandardItemModel	*fileListModel;

		QListView			sideList;
		QStandardItemModel	*sideListModel;

		QLabel				previewIcon;
		QLabel				previewMimeType;
		QLabel				previewSize;
		QLabel				previewMode;
		QLabel				previewFileName;
		QString				saveName;

		bool					showHidden=false;
		bool					showThumbsInList=false;
		bool					saveDialog=false;
		bool					overwriteWarning=true;
		QString				lastSaveFolder;
		QString				lastLoadFolder;
		int					maxRecents=21;
		QString				recentFoldersPath;
		QString				recentFilesPath;
		QString				currentFolder;

		chooserDialogType	dialogType=chooserDialogType::openDialog;

		void					buildMainGui(void);
		void					setSideList(void);
		void					setFileList(void);
		QIcon				getFileIcon(QString path);
		void					selectItem(const QModelIndex &index);
		void					selectSideItem(const QModelIndex &index);
		void					showPreViewData(void);
		void					setFileData(void);
		void					setFavs(void);
		QString				getProperPath(QString str);
		void					setLast(QString str);
		void					setRecents(QString str);
};

#endif
