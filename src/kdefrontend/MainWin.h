/***************************************************************************
    File                 : MainWin.h
    Project              : LabPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2011-2015 Alexander Semke (alexander.semke@web.de)
    Copyright            : (C) 2008-2015 by Stefan Gerlach (stefan.gerlach@uni.kn)
    Description          : Main window of the application
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/
#ifndef MAINWIN_H
#define MAINWIN_H
#include <KXmlGuiWindow>
#include <KRecentFilesAction>
#include "commonfrontend/core/PartMdiView.h"
#include <QTimer>

class AbstractAspect;
class AspectTreeModel;
class Folder;
class ProjectExplorer;
class Project;
class Worksheet;
class Workbook;
class Datapicker;
class Image;
class Spreadsheet;
class Matrix;
class GuiObserver;
class AxisDock;
class CartesianPlotDock;
class CartesianPlotLegendDock;
class CustomPointDock;
class ColumnDock;
class MatrixDock;
class ProjectDock;
class SpreadsheetDock;
class XYCurveDock;
class XYEquationCurveDock;
class XYInterpolationCurveDock;
class XYSmoothCurveDock;
class XYFitCurveDock;
class XYFourierFilterCurveDock;
class WorksheetDock;
class LabelWidget;
class ImportFileDialog;
class DatapickerImageWidget;
class DatapickerCurveWidget;

class QDockWidget;
class QStackedWidget;
class QDragEnterEvent;
class QDropEvent;

class MainWin : public KXmlGuiWindow {
	Q_OBJECT

public:
	explicit MainWin(QWidget* parent = 0, const QString& filename=0);
	~MainWin();

	AspectTreeModel* model() const;
	void addAspectToProject(AbstractAspect*);

private:
	QMdiArea* m_mdiArea;
	QMdiSubWindow* m_currentSubWindow;
	Project* m_project;
	AspectTreeModel* m_aspectTreeModel;
	ProjectExplorer* m_projectExplorer;
	QDockWidget* m_projectExplorerDock;
	QDockWidget* m_propertiesDock;
	AbstractAspect* m_currentAspect;
	Folder* m_currentFolder;
	QString m_currentFileName;
	QString m_undoViewEmptyLabel;
	ImportFileDialog* m_importFileDialog;
	bool m_suppressCurrentSubWindowChangedEvent;
	bool m_closing;
	bool m_autoSaveActive;
	QTimer m_autoSaveTimer;
	Qt::WindowStates m_lastWindowState; //< last window state before switching to full screen mode

	KRecentFilesAction* m_recentProjectsAction;
	KAction* m_saveAction;
	KAction* m_saveAsAction;
	KAction* m_printAction;
	KAction* m_printPreviewAction;
	KAction* m_importAction;
	KAction* m_exportAction;
	KAction* m_closeAction;
	KAction* m_newFolderAction;
	KAction* m_newWorkbookAction;
	KAction* m_newSpreadsheetAction;
	KAction* m_newMatrixAction;
	KAction* m_newWorksheetAction;
	KAction* m_newFileDataSourceAction;
	KAction* m_newSqlDataSourceAction;
	KAction* m_newScriptAction;
	KAction* m_newProjectAction;
	KAction* m_historyAction;
	KAction* m_undoAction;
	KAction* m_redoAction;
	KAction* m_tileWindows;
	KAction* m_cascadeWindows;
	KAction* m_newDatapickerAction;


	//toggling doch widgets
	KAction* m_toggleProjectExplorerDockAction;
	KAction* m_togglePropertiesDockAction;

	//worksheet actions
	KAction* worksheetZoomInAction;
	KAction* worksheetZoomOutAction;
	KAction* worksheetZoomOriginAction;
	KAction* worksheetZoomFitPageHeightAction;
	KAction* worksheetZoomFitPageWidthAction;
	KAction* worksheetZoomFitSelectionAction;

	KAction* worksheetNavigationModeAction;
	KAction* worksheetZoomModeAction;
	KAction* worksheetSelectionModeAction;

	KAction* worksheetVerticalLayoutAction;
	KAction* worksheetHorizontalLayoutAction;
	KAction* worksheetGridLayoutAction;
	KAction* worksheetBreakLayoutAction;

	KAction* m_visibilityFolderAction;
	KAction* m_visibilitySubfolderAction;
	KAction* m_visibilityAllAction;

	//Menus
	QMenu* m_visibilityMenu;
	QMenu* m_newMenu;

	//Docks
	QStackedWidget* stackedWidget;
	AxisDock* axisDock;
	CartesianPlotDock* cartesianPlotDock;
	CartesianPlotLegendDock* cartesianPlotLegendDock;
	ColumnDock* columnDock;
	MatrixDock* matrixDock;
	SpreadsheetDock* spreadsheetDock;
	ProjectDock* projectDock;
	XYCurveDock* xyCurveDock;
	XYEquationCurveDock* xyEquationCurveDock;
	XYInterpolationCurveDock* xyInterpolationCurveDock;
	XYSmoothCurveDock* xySmoothCurveDock;
	XYFitCurveDock* xyFitCurveDock;
	XYFourierFilterCurveDock* xyFourierFilterCurveDock;
	WorksheetDock* worksheetDock;
	LabelWidget* textLabelDock;
	CustomPointDock* customPointDock;
	DatapickerImageWidget* datapickerImageDock;
	DatapickerCurveWidget* datapickerCurveDock;

	bool openXML(QIODevice*);

	void initActions();
	void initMenus();
	bool warnModified();
	void activateSubWindowForAspect(const AbstractAspect*) const;
	bool save(const QString&);


	Workbook* activeWorkbook() const;
	Spreadsheet* activeSpreadsheet() const;
	Matrix* activeMatrix() const;
	Worksheet* activeWorksheet() const;
	Datapicker* activeDatapicker() const;

	friend class GuiObserver;
	GuiObserver* m_guiObserver;

protected:
	void closeEvent(QCloseEvent*);
	void dragEnterEvent(QDragEnterEvent*);
	void dropEvent(QDropEvent*);

private slots:
	void initGUI(const QString&);
	void updateGUI();
	void updateGUIOnProjectChanges();
	void undo();
	void redo();

	bool newProject();
	void openProject();
	void openProject(const QString&);
	void openRecentProject(const KUrl&);
	bool closeProject();
	bool saveProject();
	bool saveProjectAs();
	void autoSaveProject();

	void print();
	void printPreview();

	void historyDialog();
	void importFileDialog(const QString& fileName = QString());
	void exportDialog();
	void settingsDialog();
	void projectChanged();

	void newFolder();
	void newWorkbook();
	void newSpreadsheet();
	void newMatrix();
	void newWorksheet();
	void newDatapicker();
	//TODO: void newScript();
	void newFileDataSourceActionTriggered();
	void newSqlDataSourceActionTriggered();

	void createContextMenu(QMenu*) const;
	void createFolderContextMenu(const Folder*, QMenu*) const;

	void handleAspectAdded(const AbstractAspect*);
	void handleAspectAboutToBeRemoved(const AbstractAspect*);
	void handleAspectRemoved(const AbstractAspect*,const AbstractAspect*,const AbstractAspect*);
	void handleCurrentAspectChanged(AbstractAspect* );
	void handleCurrentSubWindowChanged(QMdiSubWindow*);
	void handleShowSubWindowRequested();

	void handleSettingsChanges();

	void setMdiWindowVisibility(QAction*);
	void updateMdiWindowVisibility() const;
	void toggleDockWidget(QAction*) const;
	void toggleFullScreen();
};

#endif
