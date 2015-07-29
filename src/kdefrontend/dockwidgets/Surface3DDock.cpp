/***************************************************************************
    File                 : Surface3DDock.cpp
    Project              : LabPlot
    Description          : widget for 3D surfaces properties
    --------------------------------------------------------------------
    Copyright            : (C) 2015 Minh Ngo (minh@fedoraproject.org)

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

#include "Surface3DDock.h"
#include "DockHelpers.h"
#include "backend/core/AbstractAspect.h"
#include "backend/core/AbstractColumn.h"
#include "backend/core/AspectTreeModel.h"
#include "backend/core/Project.h"
#include "backend/matrix/Matrix.h"
#include "backend/worksheet/plots/3d/DataHandlers.h"
#include "kdefrontend/TemplateHandler.h"

#include <QDir>

using namespace DockHelpers;

Surface3DDock::Surface3DDock(QWidget* parent)
	: QWidget(parent)
	, surface(0)
	, aspectTreeModel(0)
	, m_initializing(false) {
	ui.setupUi(this);

	Lock lock(m_initializing);
	this->retranslateUi();

	ui.cbDataSource->insertItem(Surface3D::DataSource_File, i18n("File"));
	ui.cbDataSource->insertItem(Surface3D::DataSource_Spreadsheet, i18n("Spreadsheet"));
	ui.cbDataSource->insertItem(Surface3D::DataSource_Matrix, i18n("Matrix"));
	ui.cbDataSource->insertItem(Surface3D::DataSource_Empty, i18n("Demo"));

	ui.cbType->insertItem(Surface3D::VisualizationType_Triangles, i18n("Triangles"));
	ui.cbType->insertItem(Surface3D::VisualizationType_Wireframe, i18n("Wireframe"));

	QList<const char*>  list;
	list << "Folder" << "Workbook" << "Spreadsheet" << "FileDataSource" << "Column";

	const QVector<TreeViewComboBox*> treeViews(QVector<TreeViewComboBox*>()
			<< ui.cbXCoordinate << ui.cbYCoordinate << ui.cbZCoordinate
			<< ui.cbNode1 << ui.cbNode2 << ui.cbNode3);

	foreach(TreeViewComboBox* view, treeViews) {
		view->setTopLevelClasses(list);
	}

	list.clear();
	list << "Column";

	foreach(TreeViewComboBox* view, treeViews) {
		view->setSelectableClasses(list);
		connect(view, SIGNAL(currentModelIndexChanged(const QModelIndex&)), SLOT(onTreeViewIndexChanged(const QModelIndex&)));
	}

	//Matrix data source
	list.clear();
	list<<"Folder"<<"Workbook"<<"Matrix";
	ui.cbMatrix->setTopLevelClasses(list);

	list.clear();
	list<<"Matrix";
	ui.cbMatrix->setSelectableClasses(list);

	//SIGNALs/SLOTs
	//General
	connect(ui.leName, SIGNAL(returnPressed()), SLOT(nameChanged()));
	connect(ui.leComment, SIGNAL(returnPressed()), SLOT(commentChanged()));
	connect(ui.cbDataSource, SIGNAL(currentIndexChanged(int)), SLOT(onDataSourceChanged(int)));
	connect(ui.cbType, SIGNAL(currentIndexChanged(int)), SLOT(onVisualizationTypeChanged(int)));
	connect(ui.cbFileRequester, SIGNAL(urlSelected(const KUrl&)), SLOT(onFileChanged(const KUrl&)));
	connect(ui.cbMatrix, SIGNAL(currentModelIndexChanged(const QModelIndex&)), SLOT(onTreeViewIndexChanged(const QModelIndex&)));
	connect(ui.chkVisible, SIGNAL(toggled(bool)), SLOT(onVisibilityChanged(bool)));

	//Color filling
	connect(ui.cbColorFillingType, SIGNAL(currentIndexChanged(int)), SLOT(onColorFillingTypeChanged(int)));

	//Mesh

	//Projection


	//template handler
	TemplateHandler* templateHandler = new TemplateHandler(this, TemplateHandler::Surface3D);
	ui.verticalLayout->addWidget(templateHandler);
	templateHandler->show();
	connect(templateHandler, SIGNAL(loadConfigRequested(KConfig&)), this, SLOT(loadConfigFromTemplate(KConfig&)));
	connect(templateHandler, SIGNAL(saveConfigRequested(KConfig&)), this, SLOT(saveConfigAsTemplate(KConfig&)));
	connect(templateHandler, SIGNAL(info(QString)), this, SIGNAL(info(QString)));

	onVisualizationTypeChanged(Surface3D::VisualizationType_Triangles);
	onDataSourceChanged(Surface3D::DataSource_File);
	onColorFillingTypeChanged(Surface3D::ColorFilling_Empty);
}

namespace {
	AbstractColumn* getColumn(const QModelIndex& index) {
		AbstractAspect* aspect = static_cast<AbstractAspect*>(index.internalPointer());
		return aspect ? dynamic_cast<AbstractColumn*>(aspect) : 0;
	}

	Matrix* getMatrix(const QModelIndex& index) {
		AbstractAspect* aspect = static_cast<AbstractAspect*>(index.internalPointer());
		return aspect ? dynamic_cast<Matrix*>(aspect) : 0;
	}
}

void Surface3DDock::setSurface(Surface3D *surface) {
	Lock lock(m_initializing);
	if (this->surface) {
		this->surface->disconnect(this);
		this->surface->fileDataHandler().disconnect(this);
		this->surface->spreadsheetDataHandler().disconnect(this);
		this->surface->matrixDataHandler().disconnect(this);
	}

	this->surface = surface;
	ui.leName->setText(surface->name());
	ui.leComment->setText(surface->comment());
	ui.chkVisible->setChecked(surface->isVisible());
	aspectTreeModel = new AspectTreeModel(surface->project());
	ui.cbXCoordinate->setModel(aspectTreeModel);
	ui.cbYCoordinate->setModel(aspectTreeModel);
	ui.cbZCoordinate->setModel(aspectTreeModel);
	ui.cbNode1->setModel(aspectTreeModel);
	ui.cbNode2->setModel(aspectTreeModel);
	ui.cbNode3->setModel(aspectTreeModel);
	ui.cbMatrix->setModel(aspectTreeModel);
	ui.cbColorFillingMatrix->setModel(aspectTreeModel);

	ui.cbType->setCurrentIndex(surface->visualizationType());

	ui.cbDataSource->setCurrentIndex(surface->dataSource());

	ui.cbColorFillingType->setCurrentIndex(surface->colorFilling());

	connect(surface, SIGNAL(visualizationTypeChanged(Surface3D::VisualizationType)), SLOT(visualizationTypeChanged(Surface3D::VisualizationType)));
	connect(surface, SIGNAL(sourceTypeChanged(Surface3D::DataSource)), SLOT(sourceTypeChanged(Surface3D::DataSource)));
	connect(surface, SIGNAL(visibilityChanged(bool)), ui.chkVisible, SLOT(setChecked(bool)));
	connect(surface, SIGNAL(colorFillingChanged(Surface3D::ColorFilling)), SLOT(colorFillingChanged(Surface3D::ColorFilling)));

	// DataHandlers
	ui.cbFileRequester->setUrl(surface->fileDataHandler().file());
	connect(&surface->fileDataHandler(), SIGNAL(pathChanged(const KUrl&)), SLOT(pathChanged(const KUrl&)));

	const Matrix *matrix = surface->matrixDataHandler().matrix();
	if (matrix)
		ui.cbMatrix->setCurrentModelIndex(aspectTreeModel->modelIndexOfAspect(matrix));
	connect(&surface->matrixDataHandler(), SIGNAL(matrixChanged(const Matrix*)), SLOT(matrixChanged(const Matrix*)));

	SpreadsheetDataHandler *sdh = &surface->spreadsheetDataHandler();
	if (sdh->xColumn())
		ui.cbXCoordinate->setCurrentModelIndex(aspectTreeModel->modelIndexOfAspect(sdh->xColumn()));

	if (sdh->yColumn())
		ui.cbYCoordinate->setCurrentModelIndex(aspectTreeModel->modelIndexOfAspect(sdh->yColumn()));

	if (sdh->zColumn())
		ui.cbZCoordinate->setCurrentModelIndex(aspectTreeModel->modelIndexOfAspect(sdh->zColumn()));

	if (sdh->firstNode())
		ui.cbNode1->setCurrentModelIndex(aspectTreeModel->modelIndexOfAspect(sdh->firstNode()));

	if (sdh->secondNode())
		ui.cbNode2->setCurrentModelIndex(aspectTreeModel->modelIndexOfAspect(sdh->secondNode()));

	if (sdh->thirdNode())
		ui.cbNode3->setCurrentModelIndex(aspectTreeModel->modelIndexOfAspect(sdh->thirdNode()));

	connect(sdh, SIGNAL(xColumnChanged(const AbstractColumn*)), SLOT(xColumnChanged(const AbstractColumn*)));
	connect(sdh, SIGNAL(yColumnChanged(const AbstractColumn*)), SLOT(yColumnChanged(const AbstractColumn*)));
	connect(sdh, SIGNAL(zColumnChanged(const AbstractColumn*)), SLOT(zColumnChanged(const AbstractColumn*)));
	connect(sdh, SIGNAL(firstNodeChanged(const AbstractColumn*)), SLOT(firstNodeChanged(const AbstractColumn*)));
	connect(sdh, SIGNAL(secondNodeChanged(const AbstractColumn*)), SLOT(secondNodeChanged(const AbstractColumn*)));
	connect(sdh, SIGNAL(thirdNodeChanged(const AbstractColumn*)), SLOT(thirdNodeChanged(const AbstractColumn*)));
}

void Surface3DDock::showTriangleInfo(bool pred) {
	showItem(ui.labelX, ui.cbXCoordinate, pred);
	showItem(ui.labelY, ui.cbYCoordinate, pred);
	showItem(ui.labelZ, ui.cbZCoordinate, pred);
	showItem(ui.labelNode1, ui.cbNode1, pred);
	showItem(ui.labelNode2, ui.cbNode2, pred);
	showItem(ui.labelNode3, ui.cbNode3, pred);
	ui.labelNodeHeader->setVisible(pred);
	emit elementVisibilityChanged();
}

//*************************************************************
//****** SLOTs for changes triggered in Surface3DDock *********
//*************************************************************
void Surface3DDock::retranslateUi(){
	//color filling
	ui.cbColorFillingType->insertItem(Surface3D::ColorFilling_Empty, i18n("no filling"));
	ui.cbColorFillingType->insertItem(Surface3D::ColorFilling_SolidColor, i18n("solid color"));
	ui.cbColorFillingType->insertItem(Surface3D::ColorFilling_ColorMap, i18n("color map"));
	ui.cbColorFillingType->insertItem(Surface3D::ColorFilling_ElevationLevel, i18n("elevation level"));
	ui.cbColorFillingType->insertItem(Surface3D::ColorFilling_ColorMapFromMatrix, i18n("color map from matrix"));
}

void Surface3DDock::onTreeViewIndexChanged(const QModelIndex& index) {
	if (m_initializing)
		return;

	AbstractColumn* column = getColumn(index);
	Q_ASSERT(column);

	QObject *senderW = sender();
	if(senderW == ui.cbXCoordinate)
		surface->spreadsheetDataHandler().setXColumn(column);
	else if(senderW  == ui.cbYCoordinate)
		surface->spreadsheetDataHandler().setYColumn(column);
	else if(senderW  == ui.cbZCoordinate)
		surface->spreadsheetDataHandler().setZColumn(column);
	else if(senderW  == ui.cbNode1)
		surface->spreadsheetDataHandler().setFirstNode(column);
	else if(senderW  == ui.cbNode2)
		surface->spreadsheetDataHandler().setSecondNode(column);
	else if(senderW == ui.cbNode3)
		surface->spreadsheetDataHandler().setThirdNode(column);
	else if(senderW == ui.cbMatrix)
		surface->matrixDataHandler().setMatrix(getMatrix(index));
}

void Surface3DDock::onVisibilityChanged(bool visible) {
	if(!m_initializing)
		surface->show(visible);
}

void Surface3DDock::onDataSourceChanged(int index) {
	const Surface3D::DataSource type = static_cast<Surface3D::DataSource>(index);
	showItem(ui.labelFile, ui.cbFileRequester, type == Surface3D::DataSource_File);
	showItem(ui.labelMatrix, ui.cbMatrix, type == Surface3D::DataSource_Matrix);

	if (!m_initializing)
		surface->setDataSource(type);
	emit elementVisibilityChanged();
}

void Surface3DDock::onVisualizationTypeChanged(int index) {
	if(!m_initializing)
		surface->setVisualizationType(static_cast<Surface3D::VisualizationType>(index));

	if(index == Surface3D::VisualizationType_Triangles){
		showItem(ui.labelSource, ui.cbDataSource);
		onDataSourceChanged(ui.cbDataSource->currentIndex());
	}else{
		hideItem(ui.labelSource, ui.cbDataSource);
		hideItem(ui.labelFile, ui.cbFileRequester);
		showTriangleInfo(false);
	}
}

void Surface3DDock::onFileChanged(const KUrl& path) {
	if (m_initializing || !path.isLocalFile())
		return;

	surface->fileDataHandler().setFile(path);
}

void Surface3DDock::visualizationTypeChanged(Surface3D::VisualizationType type) {
	Lock lock(m_initializing);
	ui.cbType->setCurrentIndex(type);
}

void Surface3DDock::sourceTypeChanged(Surface3D::DataSource type) {
	Lock lock(m_initializing);
	ui.cbDataSource->setCurrentIndex(type);
}

void Surface3DDock::pathChanged(const KUrl& url) {
	Lock lock(m_initializing);
	ui.cbFileRequester->setUrl(url);
}

void Surface3DDock::setModelFromAspect(TreeViewComboBox* cb, const AbstractAspect* aspect) {
	Lock lock(m_initializing);
	if (aspect) {
		cb->setCurrentModelIndex(aspectTreeModel->modelIndexOfAspect(aspect));
	} else {
		cb->setCurrentModelIndex(QModelIndex());
	}
}

void Surface3DDock::matrixChanged(const Matrix* matrix) {
	setModelFromAspect(ui.cbMatrix, matrix);
}

void Surface3DDock::xColumnChanged(const AbstractColumn* column) {
	setModelFromAspect(ui.cbXCoordinate, column);
}

void Surface3DDock::yColumnChanged(const AbstractColumn* column) {
	setModelFromAspect(ui.cbYCoordinate, column);
}

void Surface3DDock::zColumnChanged(const AbstractColumn* column) {
	setModelFromAspect(ui.cbZCoordinate, column);
}

void Surface3DDock::firstNodeChanged(const AbstractColumn* column) {
	setModelFromAspect(ui.cbNode1, column);
}

void Surface3DDock::secondNodeChanged(const AbstractColumn* column) {
	setModelFromAspect(ui.cbNode2, column);
}

void Surface3DDock::thirdNodeChanged(const AbstractColumn* column) {
	setModelFromAspect(ui.cbNode3, column);
}

void Surface3DDock::colorFillingChanged(Surface3D::ColorFilling color) {
	if (m_initializing)
		return;

	ui.cbColorFillingType->setCurrentIndex(color);
}

void Surface3DDock::nameChanged() {
	if (m_initializing)
		return;

	surface->setName(ui.leName->text());
}

void Surface3DDock::commentChanged() {
	if (m_initializing)
		return;

	surface->setComment(ui.leComment->text());
}

//Collor filling
void Surface3DDock::onColorFillingTypeChanged(int index) {
	const Surface3D::ColorFilling type = static_cast<Surface3D::ColorFilling>(index);
	if (type == Surface3D::ColorFilling_Empty
			|| type == Surface3D::ColorFilling_ElevationLevel) {
		hideItem(ui.lColorFilling, ui.kcbColorFilling);
		hideItem(ui.lColorFillingMap, ui.cbColorFillingMap);
		hideItem(ui.lColorFillingMatrix, ui.cbColorFillingMatrix);
		hideItem(ui.lColorFillingOpacity, ui.sbColorFillingOpacity);
	} else if (type == Surface3D::ColorFilling_SolidColor) {
		showItem(ui.lColorFilling, ui.kcbColorFilling);
		hideItem(ui.lColorFillingMap, ui.cbColorFillingMap);
		hideItem(ui.lColorFillingMatrix, ui.cbColorFillingMatrix);
		showItem(ui.lColorFillingOpacity, ui.sbColorFillingOpacity);
	} else if (type == Surface3D::ColorFilling_ColorMap) {
		hideItem(ui.lColorFilling, ui.kcbColorFilling);
		showItem(ui.lColorFillingMap, ui.cbColorFillingMap);
		hideItem(ui.lColorFillingMatrix, ui.cbColorFillingMatrix);
		showItem(ui.lColorFillingOpacity, ui.sbColorFillingOpacity);
	} else if (type == Surface3D::ColorFilling_ColorMapFromMatrix) {
		hideItem(ui.lColorFilling, ui.kcbColorFilling);
		hideItem(ui.lColorFillingMap, ui.cbColorFillingMap);
		showItem(ui.lColorFillingMatrix, ui.cbColorFillingMatrix);
		showItem(ui.lColorFillingOpacity, ui.sbColorFillingOpacity);
	}

	if (!m_initializing)
		surface->setColorFilling(type);
	emit elementVisibilityChanged();
}


//*************************************************************
//************************* Settings **************************
//*************************************************************
void Surface3DDock::load() {
	//TODO
}

void Surface3DDock::loadConfigFromTemplate(KConfig& config) {
	//extract the name of the template from the file name
	QString name;
	int index = config.name().lastIndexOf(QDir::separator());
	if (index!=-1)
		name = config.name().right(config.name().size() - index - 1);
	else
		name = config.name();

	//TODO
// 	int size = m_curvesList.size();
// 	if (size>1)
// 		m_curve->beginMacro(i18n("%1 3D-surfaces: template \"%2\" loaded", size, name));
// 	else
// 		m_curve->beginMacro(i18n("%1: template \"%2\" loaded", m_surface->name(), name));

	this->loadConfig(config);

// 	m_surface->endMacro();
}

void Surface3DDock::loadConfig(KConfig& config) {
	KConfigGroup group = config.group("Surface3D");
	//TODO

}

void Surface3DDock::saveConfigAsTemplate(KConfig& config) {
	KConfigGroup group = config.group("Surface3D");
	//TODO

	config.sync();
}