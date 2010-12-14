/***************************************************************************
    File                 : LineSymbolCurveDock.cpp
    Project              : LabPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2010 Alexander Semke (alexander.semke*web.de)
                           (replace * with @ in the email addresses)
    Description          : widget for LineSymbolCurve properties
                           
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

#include "LineSymbolCurveDock.h"
#include "worksheet/LineSymbolCurve.h"
#include "core/AspectTreeModel.h"
#include "core/AbstractColumn.h"
#include "core/plugin/PluginManager.h"
#include "worksheet/StandardCurveSymbolFactory.h"
#include "widgets/TreeViewComboBox.h"
#include <QTextEdit>
#include <QCheckBox>
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QDebug>

/*!
  \class GuiObserver
  \brief  Provides a widget for editing the properties of the worksheets currently selected in the project explorer.

  \ingroup kdefrontend
*/

LineSymbolCurveDock::LineSymbolCurveDock(QWidget *parent): QWidget(parent){
	ui.setupUi(this);
	
	// Tab "General"
	gridLayout = new QGridLayout(ui.tabGeneral);
	gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
	lName = new QLabel(ui.tabGeneral);
	lName->setObjectName(QString::fromUtf8("lName"));
	QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(lName->sizePolicy().hasHeightForWidth());
	lName->setSizePolicy(sizePolicy);

	gridLayout->addWidget(lName, 0, 0, 1, 1);

	leName = new QLineEdit(ui.tabGeneral);
	leName->setObjectName(QString::fromUtf8("leName"));

	gridLayout->addWidget(leName, 0, 1, 1, 1);

	lComment = new QLabel(ui.tabGeneral);
	lComment->setObjectName(QString::fromUtf8("lComment"));
	sizePolicy.setHeightForWidth(lComment->sizePolicy().hasHeightForWidth());
	lComment->setSizePolicy(sizePolicy);

	gridLayout->addWidget(lComment, 1, 0, 1, 1);

	teComment = new QTextEdit(ui.tabGeneral);
	teComment->setObjectName(QString::fromUtf8("teComment"));
	QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
	sizePolicy1.setHorizontalStretch(0);
	sizePolicy1.setVerticalStretch(0);
	sizePolicy1.setHeightForWidth(teComment->sizePolicy().hasHeightForWidth());
	teComment->setSizePolicy(sizePolicy1);
	teComment->setMaximumSize(QSize(16777215, 50));

	gridLayout->addWidget(teComment, 1, 1, 1, 1);
	
	chkVisible = new QCheckBox(ui.tabGeneral);
	gridLayout->addWidget(chkVisible, 2, 0, 1, 1);
	
	lXColumn= new QLabel(ui.tabGeneral);
	gridLayout->addWidget(lXColumn, 3, 0, 1, 1);
	
	cbXColumn = new TreeViewComboBox(ui.tabGeneral);
	gridLayout->addWidget(cbXColumn, 3, 1, 1, 1);
	
	lYColumn= new QLabel(ui.tabGeneral);
	gridLayout->addWidget(lYColumn, 4, 0, 1, 1);
	
	cbYColumn = new TreeViewComboBox(ui.tabGeneral);
	gridLayout->addWidget(cbYColumn, 4, 1, 1, 1);
	
	verticalSpacer = new QSpacerItem(24, 320, QSizePolicy::Minimum, QSizePolicy::Expanding);
	gridLayout->addItem(verticalSpacer, 5, 0, 1, 1);

	
	//Tab "Values"
	cbValuesColumn = new TreeViewComboBox(ui.tabValues);
	qobject_cast<QGridLayout*>(ui.tabValues->layout())->addWidget(cbValuesColumn, 2, 2, 1, 1);
	
	
	//adjust layouts in the tabs
	QGridLayout* layout;
	for (int i=0; i<ui.tabWidget->count(); ++i){
	  layout=static_cast<QGridLayout*>(ui.tabWidget->widget(i)->layout());
	  if (!layout)
		continue;
	  
	  layout->setContentsMargins(2,2,2,2);
	  layout->setHorizontalSpacing(2);
	  layout->setVerticalSpacing(2);
	}
	
	
	//Slots
	
	//General
	connect( leName, SIGNAL(returnPressed()), this, SLOT(nameChanged()) );
	//TODO signal-slot for teComment
	connect( chkVisible, SIGNAL(stateChanged(int)), this, SLOT(visibilityChanged(int)) );
	connect( cbXColumn, SIGNAL(currentIndexChanged(int)), this, SLOT(xColumnChanged(int)) );
	connect( cbYColumn, SIGNAL(currentIndexChanged(int)), this, SLOT(yColumnChanged(int)) );
	
	//Lines
	connect( ui.cbLineType, SIGNAL(currentIndexChanged(int)), this, SLOT(lineTypeChanged(int)) );
	connect( ui.sbLineInterpolationPointsCount, SIGNAL(valueChanged(int)), this, SLOT(lineInterpolationPointsCountChanged(int)) );
	connect( ui.cbLineStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(lineStyleChanged(int)) );
	connect( ui.kcbLineColor, SIGNAL(changed (const QColor &)), this, SLOT(lineColorChanged(const QColor&)) );
	connect( ui.sbLineWidth, SIGNAL(valueChanged(int)), this, SLOT(lineWidthChanged(int)) );
	connect( ui.sbLineOpacity, SIGNAL(valueChanged(int)), this, SLOT(lineOpacityChanged(int)) );

	connect( ui.cbDropLineType, SIGNAL(currentIndexChanged(int)), this, SLOT(dropLineTypeChanged(int)) );
	connect( ui.cbDropLineStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(dropLineStyleChanged(int)) );
	connect( ui.kcbDropLineColor, SIGNAL(changed (const QColor &)), this, SLOT(dropLineColorChanged(const QColor&)) );
	connect( ui.sbDropLineWidth, SIGNAL(valueChanged(int)), this, SLOT(dropLineWidthChanged(int)) );
	connect( ui.sbDropLineOpacity, SIGNAL(valueChanged(int)), this, SLOT(dropLineOpacityChanged(int)) );
	
	//Symbol
	connect( ui.cbSymbolStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(symbolStyleChanged(int)) );
	connect( ui.sbSymbolSize, SIGNAL(valueChanged(int)), this, SLOT(symbolSizeChanged(int)) );
	connect( ui.sbSymbolRotation, SIGNAL(valueChanged(int)), this, SLOT(symbolRotationChanged(int)) );
	connect( ui.sbSymbolOpacity, SIGNAL(valueChanged(int)), this, SLOT(symbolOpacityChanged(int)) );
	
	connect( ui.cbSymbolFillingStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(symbolFillingStyleChanged(int)) );
	connect( ui.kcbSymbolFillingColor, SIGNAL(changed (const QColor &)), this, SLOT(symbolFillingColorChanged(const QColor)) );

	connect( ui.cbSymbolBorderStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(symbolBorderStyleChanged(int)) );
	connect( ui.kcbSymbolBorderColor, SIGNAL(changed (const QColor &)), this, SLOT(symbolBorderColorChanged(const QColor&)) );
	connect( ui.sbSymbolBorderWidth, SIGNAL(valueChanged(int)), this, SLOT(symbolBorderWidthChanged(int)) );

	//Values
	connect( ui.cbValuesType, SIGNAL(currentIndexChanged(int)), this, SLOT(valuesTypeChanged(int)) );
	connect( ui.cbValuesPosition, SIGNAL(currentIndexChanged(int)), this, SLOT(valuesPositionChanged(int)) );
	connect( ui.sbValuesDistance, SIGNAL(valueChanged(int)), this, SLOT(valuesDistanceChanged(int)) );
	connect( ui.sbValuesRotation, SIGNAL(valueChanged(int)), this, SLOT(valuesRotationChanged(int)) );
	connect( ui.sbValuesOpacity, SIGNAL(valueChanged(int)), this, SLOT(valuesOpacityChanged(int)) );
	
	connect( ui.leValuesPrefix, SIGNAL(returnPressed()), this, SLOT(valuesPrefixChanged()) );
	connect( ui.leValuesSuffix, SIGNAL(returnPressed()), this, SLOT(valuesSuffixChanged()) );
	connect( ui.kfrValuesFont, SIGNAL(fontSelected(const QFont& )), this, SLOT(valuesFontChanged(const QFont&)) );
	connect( ui.kcbValuesFontColor, SIGNAL(changed (const QColor &)), this, SLOT(valuesFontColorChanged(const QColor&)) );
	
	retranslateUi();
	
	QTimer::singleShot(0, this, SLOT(init()));
}

void LineSymbolCurveDock::init(){
  	//Line
	ui.cbLineType->addItems(LineSymbolCurve::lineTypeStrings());
	QPainter pa;
	QPixmap pm( 20, 20 );
	ui.cbLineType->setIconSize( QSize(20,20) );

	QPen pen(Qt::SolidPattern, 0);
 	pa.setPen( pen );
	
	//no line
	pm.fill(Qt::transparent);
	pa.begin( &pm );
	pa.setRenderHint(QPainter::Antialiasing);
	pa.setBrush(Qt::SolidPattern);
	pa.drawEllipse( 1,1,4,4);
	pa.drawEllipse( 15,15,4,4);
	pa.end();
	ui.cbLineType->setItemIcon(0, pm);
	
	//line
	pm.fill(Qt::transparent);
	pa.begin( &pm );
	pa.setRenderHint(QPainter::Antialiasing);
	pa.setBrush(Qt::SolidPattern);
	pa.drawEllipse( 1,1,4,4);
	pa.drawEllipse( 15,15,4,4);
	pa.drawLine(3,3,17,17);
	pa.end();
	ui.cbLineType->setItemIcon(1, pm);
	
	pm.fill(Qt::transparent);
	pa.begin( &pm );
	pa.setRenderHint(QPainter::Antialiasing);
	pa.setBrush(Qt::SolidPattern);
	pa.drawEllipse( 1,1,4,4);
	pa.drawEllipse( 15,15,4,4);
	pa.drawLine(3,3,17,3);
	pa.drawLine(17,3,17,17);
	pa.end();
	ui.cbLineType->setItemIcon(2, pm);
	
	pm.fill(Qt::transparent);
	pa.begin( &pm );
	pa.setRenderHint(QPainter::Antialiasing);
	pa.setBrush(Qt::SolidPattern);
	pa.drawEllipse( 1,1,4,4);
	pa.drawEllipse( 15,15,4,4);
	pa.drawLine(3,3,3,17);
	pa.drawLine(3,17,17,17);
	pa.end();
	ui.cbLineType->setItemIcon(3, pm);
	
	//horizontal midpoint
	pm.fill(Qt::transparent);
	pa.begin( &pm );
	pa.setRenderHint(QPainter::Antialiasing);
	pa.setBrush(Qt::SolidPattern);
	pa.drawEllipse( 1,1,4,4);
	pa.drawEllipse( 15,15,4,4);
	pa.drawLine(3,3,10,3);
	pa.drawLine(10,3,10,17);
	pa.drawLine(10,17,17,17);
	pa.end();
	ui.cbLineType->setItemIcon(4, pm);
	
	//vertical midpoint
	pm.fill(Qt::transparent);
	pa.begin( &pm );
	pa.setRenderHint(QPainter::Antialiasing);
	pa.setBrush(Qt::SolidPattern);
	pa.drawEllipse( 1,1,4,4);
	pa.drawEllipse( 15,15,4,4);
	pa.drawLine(3,3,3,10);
	pa.drawLine(3,10,17,10);
	pa.drawLine(17,10,17,17);
	pa.end();
	ui.cbLineType->setItemIcon(5, pm);
	
	//2-segments
	pm.fill(Qt::transparent);
	pa.begin( &pm );
	pa.setRenderHint(QPainter::Antialiasing);
	pa.setBrush(Qt::SolidPattern);
	pa.drawEllipse( 1,1,4,4);
	pa.drawEllipse( 8,8,4,4);
	pa.drawEllipse( 15,15,4,4);
	pa.drawLine(3,3,10,10);
	pa.end();
	ui.cbLineType->setItemIcon(6, pm);
	
	//3-segments
	pm.fill(Qt::transparent);
	pa.begin( &pm );
	pa.setRenderHint(QPainter::Antialiasing);
	pa.setBrush(Qt::SolidPattern);
	pa.drawEllipse( 1,1,4,4);
	pa.drawEllipse( 8,8,4,4);
	pa.drawEllipse( 15,15,4,4);
	pa.drawLine(3,3,17,17);
	pa.end();
	ui.cbLineType->setItemIcon(7, pm);
	
	//natural spline
// 	pm.fill(Qt::transparent);
// 	pa.begin( &pm );
// 	pa.setRenderHint(QPainter::Antialiasing);
// 	pa.setBrush(Qt::SolidPattern);
//  	pa.rotate(-45);
// 	pa.drawEllipse( 1,1,4,4);
// 	pa.drawEllipse( 1,15*sqrt(2),4,4);
// // 	pa.drawArc(3,15,14,12, 30*16, 120*16);
// 	pa.end();
// 	ui.cbLineType->setItemIcon(8, pm);
// 	ui.cbLineType->setItemIcon(9, pm);
// 	ui.cbLineType->setItemIcon(10, pm);
// 	ui.cbLineType->setItemIcon(11, pm);
	
	
	this->updatePenStyles(ui.cbLineStyle, Qt::black);
	
	//Drop lines
	ui.cbDropLineType->addItems(LineSymbolCurve::dropLineTypeStrings());
	this->updatePenStyles(ui.cbDropLineStyle, Qt::black);
	
	//Symbols
	this->updatePenStyles(ui.cbSymbolBorderStyle, Qt::black);
	this->fillSymbolStyles();
 	this->updateBrushStyles(ui.cbSymbolFillingStyle, Qt::black);
	
	//Values
	ui.cbValuesType->addItems(LineSymbolCurve::valuesTypeStrings());
	ui.cbValuesPosition->addItems(LineSymbolCurve::valuesPositionStrings());
}

void LineSymbolCurveDock::setModel(AspectTreeModel* model){
	QList<const char *>  list;
	list<<"Folder"<<"Spreadsheet"<<"FileDataSource"<<"Column";
	cbXColumn->setTopLevelClasses(list);
	cbYColumn->setTopLevelClasses(list);
	cbValuesColumn->setTopLevelClasses(list);
	
 	list.clear();
	list<<"Column";
	model->setSelectableAspects(list);
	
	m_initializing=true;
  	cbXColumn->setModel(model);
	cbYColumn->setModel(model);
	cbValuesColumn->setModel(model);
	
	m_aspectTreeModel=model;
	m_initializing=false;
}

void LineSymbolCurveDock::setCurves(QList<LineSymbolCurve*> list){
  m_initializing=true;
  
  LineSymbolCurve* curve=list.first();
  
  //if there are more then one curve in the list, disable the tab "general"
  if (list.size()==1){
	lName->setEnabled(true);
	leName->setEnabled(true);
	lComment->setEnabled(true);
	teComment->setEnabled(true);
	lXColumn->setEnabled(true);
	cbXColumn->setEnabled(true);
	lYColumn->setEnabled(true);
	cbYColumn->setEnabled(true);
	ui.lValuesColumn->setEnabled(true);
	
	leName->setText(curve->name());
	teComment->setText(curve->comment());
  }else{
	lName->setEnabled(false);
	leName->setEnabled(false);
	lComment->setEnabled(false);
	teComment->setEnabled(false);
	lXColumn->setEnabled(false);
	cbXColumn->setEnabled(false);
	lYColumn->setEnabled(false);
	cbYColumn->setEnabled(false);	
	ui.lValuesColumn->setEnabled(false);
	
	leName->setText("");
	teComment->setText("");
	cbXColumn->setCurrentIndex(QModelIndex());
	cbYColumn->setCurrentIndex(QModelIndex());
	cbValuesColumn->setCurrentIndex(QModelIndex());
  }
  
  //show the properties of the first curve
  
  //General-tab
  chkVisible->setChecked(curve->isVisible());
  cbXColumn->setCurrentIndex( m_aspectTreeModel->modelIndexOfAspect(curve->xColumn()) );
  cbYColumn->setCurrentIndex( m_aspectTreeModel->modelIndexOfAspect(curve->yColumn()) );
  
  //Line-tab
  ui.cbLineType->setCurrentIndex( curve->lineType() );
  ui.sbLineInterpolationPointsCount->setValue( curve->lineInterpolationPointsCount() );
  ui.cbLineStyle->setCurrentIndex( curve->linePen().style() );
  ui.kcbLineColor->setColor( curve->linePen().color() );
  ui.sbLineWidth->setValue( curve->linePen().width() );
  ui.sbLineOpacity->setValue( curve->lineOpacity()*100 );
  this->updatePenStyles(ui.cbLineStyle, curve->linePen().color() );
  
  ui.cbDropLineType->setCurrentIndex( curve->dropLineType() );
  ui.cbDropLineStyle->setCurrentIndex( curve->dropLinePen().style() );
  ui.kcbDropLineColor->setColor( curve->dropLinePen().color() );
  ui.sbDropLineWidth->setValue( curve->dropLinePen().width() );
  ui.sbDropLineOpacity->setValue( curve->dropLineOpacity()*100 );
  this->updatePenStyles(ui.cbDropLineStyle, curve->dropLinePen().color() );
  
  
  //Symbol-tab
  ui.cbSymbolStyle->setCurrentIndex( ui.cbSymbolStyle->findText(curve->symbolTypeId()) );
  ui.sbSymbolSize->setValue( curve->symbolSize() );
  ui.sbSymbolRotation->setValue( curve->symbolRotationAngle() );
  ui.sbSymbolOpacity->setValue( curve->symbolsOpacity()*100 );
  ui.cbSymbolFillingStyle->setCurrentIndex( curve->symbolsBrush().style() );
  ui.kcbSymbolFillingColor->setColor( curve->symbolsBrush().color() );

  ui.cbSymbolBorderStyle->setCurrentIndex( curve->symbolsPen().style() );
  ui.kcbSymbolBorderColor->setColor( curve->symbolsPen().color() );
  ui.sbSymbolBorderWidth->setValue( curve->symbolsPen().width() );

  this->updatePenStyles(ui.cbSymbolBorderStyle, curve->symbolsPen().color() );
  this->updateBrushStyles(ui.cbSymbolFillingStyle, curve->symbolsBrush().color() );
  
  //Values-tab
  ui.cbValuesPosition->setCurrentIndex( curve->valuesPosition() );
  ui.sbValuesRotation->setValue( curve->valuesRotationAngle() );
  ui.sbValuesDistance->setValue( curve->valuesDistance() );
  ui.sbValuesOpacity->setValue( curve->valuesOpacity()*100 );
  ui.leValuesPrefix->setText( curve->valuesPrefix() );
  ui.leValuesSuffix->setText( curve->valuesSuffix() );
  ui.kfrValuesFont->setFont( curve->valuesFont() );
  ui.kcbValuesFontColor->setColor( curve->valuesPen().color() );

  //TODO
  //Area filling
  //Error bars

  m_curvesList=list;
  m_initializing=false;
}


/*!
	fills the ComboBox for the symbol style with all possible styles in the style factory.
*/
void LineSymbolCurveDock::fillSymbolStyles(){
	QPainter painter;
	int size=20; 	//TODO size of the icon depending on the actuall height of the combobox?
	QPixmap pm( size, size );
 	ui.cbSymbolStyle->setIconSize( QSize(size, size) );
	
  //TODO redesign the PluginManager and adjust this part here (load only symbol plugins and not everything!!!)
	foreach(QObject *plugin, PluginManager::plugins()) {
		CurveSymbolFactory *factory = qobject_cast<CurveSymbolFactory *>(plugin);
		if (factory){
		  symbolFactory=factory;
		  AbstractCurveSymbol* symbol;
		  foreach (const AbstractCurveSymbol* symbolPrototype, factory->prototypes()){
			if (symbolPrototype){
			  symbol= symbolPrototype->clone();
			  symbol->setSize(15);
			  
			  pm.fill(Qt::transparent);
			  painter.begin( &pm );
			  painter.setRenderHint(QPainter::Antialiasing);
			  painter.translate(size/2,size/2);
			  symbol->paint(&painter);
			  painter.end();
			  ui.cbSymbolStyle->addItem(QIcon(pm), symbol->id());
			}
		  }
		}
	}
}


/*!
	fills the ComboBox \c combobox with the six possible Qt::PenStyles, the color \c color is used.
*/
void LineSymbolCurveDock::updatePenStyles(QComboBox* comboBox, const QColor& color){
	int index=comboBox->currentIndex();
	comboBox->clear();

	QPainter pa;
	int offset=2;
	int w=50;
	int h=10;
	QPixmap pm( w, h );
	comboBox->setIconSize( QSize(w,h) );
	
	//loop over six possible Qt-PenStyles, draw on the pixmap and insert it
	QStringList list=QStringList()<<"no line"<<"solid line"<<"dash line"<<"dot line"<<"dash-dot line"<<"dash-dot-dot line";
	for (int i=0;i<6;i++){
		pm.fill(Qt::transparent);
		pa.begin( &pm );
		pa.setPen( QPen( color, 1, (Qt::PenStyle)i ) );
		pa.drawLine( offset, h/2, w-offset, h/2);
		pa.end();
 		comboBox->addItem( QIcon(pm), list.at(i) );
	}
	comboBox->setCurrentIndex(index);
}


/*!
	fills the ComboBox for the symbol filling patterns with the 14 possible Qt::BrushStyles.
*/
void LineSymbolCurveDock::updateBrushStyles(QComboBox* comboBox, const QColor& color){
  	int index=comboBox->currentIndex();
	comboBox->clear();

	QPainter pa;
	int offset=2;
	int w=50;
	int h=20;
	QPixmap pm( w, h );
	comboBox->setIconSize( QSize(w,h) );

	QPen pen(Qt::SolidPattern, 1);
 	pa.setPen( pen );
	
	QStringList list=QStringList()<<"none"<<"uniform"<<"extremely dense"<<"very dense"
														<<"somewhat dense"<<"half dense"<<"somewhat sparce"
														 <<"very sparce"<<"extremely sparce"<<"horiz. lines"
														 <<"vert. lines"<<"crossing lines"<<"backward diag. lines"
														 <<"forward diag. lines"<<"crossing diag. lines";
	for (int i=0;i<15;i++) {
		pm.fill(Qt::transparent);
		pa.begin( &pm );
		pa.setRenderHint(QPainter::Antialiasing);
 		pa.setBrush( QBrush(color, (Qt::BrushStyle)i) );
		pa.drawRect( offset, offset, w - 2*offset, h - 2*offset);
		pa.end();
		comboBox->addItem( QIcon(pm), list.at(i) );
	}

	comboBox->setCurrentIndex(index);
}


//************************************************************
//****************** SLOTS ********************************
//************************************************************
void LineSymbolCurveDock::retranslateUi(){
	lName->setText(i18n("Name:"));
	lComment->setText(i18n("Comment:"));
	chkVisible->setText(i18n("Visible"));
	lXColumn->setText(i18n("x-data:"));
	lYColumn->setText(i18n("y-data:"));
}

// "General"-tab
void LineSymbolCurveDock::nameChanged(){
    m_curvesList.first()->setName(leName->text());
}


void LineSymbolCurveDock::commentChanged(){
  m_curvesList.first()->setComment(teComment->toPlainText());
}

void LineSymbolCurveDock::xColumnChanged(int index){
  if (m_initializing)
	return;
  
  AbstractColumn* column= static_cast<AbstractColumn*>(cbXColumn->currentIndex().internalPointer());
  foreach(LineSymbolCurve* curve, m_curvesList){
	curve->setXColumn(column);
  }
}

void LineSymbolCurveDock::yColumnChanged(int index){
  if (m_initializing)
	return;
  
  AbstractColumn* column= static_cast<AbstractColumn*>(cbXColumn->currentIndex().internalPointer());
  foreach(LineSymbolCurve* curve, m_curvesList){
	curve->setYColumn(column);
  }
}

void LineSymbolCurveDock::visibilityChanged(int state){
  if (m_initializing)
	return;
  
  bool b;
  if (state==Qt::Checked)
	b=true;
  else
	b=false;

  foreach(LineSymbolCurve* curve, m_curvesList){
	curve->setVisible(b);
  }
}

// "Line"-tab
void LineSymbolCurveDock::lineTypeChanged(int index){
  LineSymbolCurve::LineType lineType = LineSymbolCurve::LineType(index);
  
  if ( lineType == LineSymbolCurve::NoLine){
	ui.cbLineStyle->setEnabled(false);
	ui.kcbLineColor->setEnabled(false);
	ui.sbLineWidth->setEnabled(false);
	ui.sbLineOpacity->setEnabled(false);
	ui.lLineInterpolationPointsCount->hide();
	ui.sbLineInterpolationPointsCount->hide();
  }else{
	ui.cbLineStyle->setEnabled(true);	
	ui.kcbLineColor->setEnabled(true);
	ui.sbLineWidth->setEnabled(true);
	ui.sbLineOpacity->setEnabled(true);
	
	if (lineType==LineSymbolCurve::SplineCubicNatural || lineType==LineSymbolCurve::SplineCubicPeriodic
	  || lineType==LineSymbolCurve::SplineAkimaNatural || lineType==LineSymbolCurve::SplineAkimaPeriodic){
	  ui.lLineInterpolationPointsCount->show();
	  ui.sbLineInterpolationPointsCount->show();
	}else{
	  ui.lLineInterpolationPointsCount->hide();
	  ui.sbLineInterpolationPointsCount->hide();
	 }
  }
  
  if (m_initializing)
	return;
  
  foreach(LineSymbolCurve* curve, m_curvesList){
	curve->setLineType(lineType);
  }
}


void LineSymbolCurveDock::lineInterpolationPointsCountChanged(int count){
   if (m_initializing)
	return;

  foreach(LineSymbolCurve* curve, m_curvesList){
	curve->setLineInterpolationPointsCount(count);
  }
}

void LineSymbolCurveDock::lineStyleChanged(int index){
   if (m_initializing)
	return;

  Qt::PenStyle penStyle=Qt::PenStyle(index);
  QPen pen;
  foreach(LineSymbolCurve* curve, m_curvesList){
	pen=curve->linePen();
	pen.setStyle(penStyle);
	curve->setLinePen(pen);
  }
}

void LineSymbolCurveDock::lineColorChanged(const QColor& color){
  if (m_initializing)
	return;

  QPen pen;
  foreach(LineSymbolCurve* curve, m_curvesList){
	pen=curve->linePen();
	pen.setColor(color);
	curve->setLinePen(pen);
  }  

  this->updatePenStyles(ui.cbLineStyle, color);
}

void LineSymbolCurveDock::lineWidthChanged(int value){
  if (m_initializing)
	return;
  
  QPen pen;
  foreach(LineSymbolCurve* curve, m_curvesList){
	pen=curve->linePen();
	pen.setWidth(value);
	curve->setLinePen(pen);
  }  
}

void LineSymbolCurveDock::lineOpacityChanged(int value){
  if (m_initializing)
	return;
		
  qreal opacity = (float)value/100;
  foreach(LineSymbolCurve* curve, m_curvesList)
	curve->setLineOpacity(opacity);
	
}

void LineSymbolCurveDock::dropLineTypeChanged(int index){
  LineSymbolCurve::DropLineType dropLineType = LineSymbolCurve::DropLineType(index);
  
  if ( dropLineType == LineSymbolCurve::NoDropLine){
	ui.cbDropLineStyle->setEnabled(false);
	ui.kcbDropLineColor->setEnabled(false);
	ui.sbDropLineWidth->setEnabled(false);
	ui.sbDropLineOpacity->setEnabled(false);
  }else{
	ui.cbDropLineStyle->setEnabled(true);
	ui.kcbDropLineColor->setEnabled(true);
	ui.sbDropLineWidth->setEnabled(true);
	ui.sbDropLineOpacity->setEnabled(true);
  }
  
  if (m_initializing)
	return;
  
  foreach(LineSymbolCurve* curve, m_curvesList){
	curve->setDropLineType(dropLineType);
  }
}


void LineSymbolCurveDock::dropLineStyleChanged(int index){
   if (m_initializing)
	return;

  Qt::PenStyle penStyle=Qt::PenStyle(index);
  QPen pen;
  foreach(LineSymbolCurve* curve, m_curvesList){
	pen=curve->dropLinePen();
	pen.setStyle(penStyle);
	curve->setDropLinePen(pen);
  }
}

void LineSymbolCurveDock::dropLineColorChanged(const QColor& color){
  if (m_initializing)
	return;

  QPen pen;
  foreach(LineSymbolCurve* curve, m_curvesList){
	pen=curve->dropLinePen();
	pen.setColor(color);
	curve->setDropLinePen(pen);
  }  

  this->updatePenStyles(ui.cbDropLineStyle, color);
}

void LineSymbolCurveDock::dropLineWidthChanged(int value){
  if (m_initializing)
	return;
  
  QPen pen;
  foreach(LineSymbolCurve* curve, m_curvesList){
	pen=curve->dropLinePen();
	pen.setWidth(value);
	curve->setDropLinePen(pen);
  }  
}

void LineSymbolCurveDock::dropLineOpacityChanged(int value){
  if (m_initializing)
	return;
		
  qreal opacity = (float)value/100;
  foreach(LineSymbolCurve* curve, m_curvesList)
	curve->setDropLineOpacity(opacity);
	
}

//"Symbol"-tab
void LineSymbolCurveDock::symbolStyleChanged(int index){
  Q_UNUSED(index);
  QString currentSymbolTypeId = ui.cbSymbolStyle->currentText();
  
  if (currentSymbolTypeId=="none"){
	ui.sbSymbolSize->setEnabled(false);
	ui.sbSymbolRotation->setEnabled(false);
	ui.sbSymbolOpacity->setEnabled(false);
	
	ui.kcbSymbolFillingColor->setEnabled(false);
	ui.lSymbolFillingStyle->setEnabled(false);
	ui.cbSymbolFillingStyle->setEnabled(false);
	
	ui.cbSymbolBorderStyle->setEnabled(false);
	ui.kcbSymbolBorderColor->setEnabled(false);
	ui.sbSymbolBorderWidth->setEnabled(false);
  }else{
	ui.sbSymbolSize->setEnabled(true);
	ui.sbSymbolRotation->setEnabled(true);
	ui.sbSymbolOpacity->setEnabled(true);
	
	//enable/disable the symbol filling options in the GUI depending on the currently selected symbol.
	if ( symbolFactory->prototype(currentSymbolTypeId)->fillingEnabled() ){
	  ui.kcbSymbolFillingColor->setEnabled(true);
	  ui.lSymbolFillingStyle->setEnabled(true);
	  ui.cbSymbolFillingStyle->setEnabled(true);
	}else{
	  ui.kcbSymbolFillingColor->setEnabled(false);
	  ui.cbSymbolFillingStyle->setEnabled(false);
	}
	
	ui.cbSymbolBorderStyle->setEnabled(true);
	ui.kcbSymbolBorderColor->setEnabled(true);
	ui.sbSymbolBorderWidth->setEnabled(true);
  }

  if (m_initializing)
	return;

  foreach(LineSymbolCurve* curve, m_curvesList){
	curve->setSymbolTypeId(currentSymbolTypeId);
  }

}


void LineSymbolCurveDock::symbolSizeChanged(int value){
  if (m_initializing)
	return;
	
  foreach(LineSymbolCurve* curve, m_curvesList){
	curve->setSymbolSize(value);
  }
}

void LineSymbolCurveDock::symbolRotationChanged(int value){
  if (m_initializing)
	return;
	
  foreach(LineSymbolCurve* curve, m_curvesList){
	curve->setSymbolRotationAngle(value);
  }
}

void LineSymbolCurveDock::symbolOpacityChanged(int value){
  if (m_initializing)
	return;
		
  qreal opacity = (float)value/100;
  foreach(LineSymbolCurve* curve, m_curvesList){
	curve->setSymbolsOpacity(opacity);
  }
}

void LineSymbolCurveDock::symbolFillingStyleChanged(int index){
  Qt::BrushStyle brushStyle = Qt::BrushStyle(index);
  if (brushStyle == Qt::NoBrush){
	ui.lSymbolFillingColor->setEnabled(false);
	ui.kcbSymbolFillingColor->setEnabled(false);
  }else{
	ui.lSymbolFillingColor->setEnabled(true);
	ui.kcbSymbolFillingColor->setEnabled(true);
  }
  
  if (m_initializing)
	return;

  QBrush brush;
  foreach(LineSymbolCurve* curve, m_curvesList){
	brush=curve->symbolsBrush();
	brush.setStyle(brushStyle);
	curve->setSymbolsBrush(brush);
  }
}

void LineSymbolCurveDock::symbolFillingColorChanged(const QColor& color){
  if (m_initializing)
	return;
	
  QBrush brush;
  foreach(LineSymbolCurve* curve, m_curvesList){
	brush=curve->symbolsBrush();
	brush.setColor(color);
	curve->setSymbolsBrush(brush);
  }

  this->updateBrushStyles(ui.cbSymbolFillingStyle, color );
}

void LineSymbolCurveDock::symbolBorderStyleChanged(int index){
  Qt::PenStyle penStyle=Qt::PenStyle(index);
  
  if ( penStyle == Qt::NoPen ){
	ui.kcbSymbolBorderColor->setEnabled(false);
	ui.sbSymbolBorderWidth->setEnabled(false);
  }else{
	ui.kcbSymbolBorderColor->setEnabled(true);
	ui.sbSymbolBorderWidth->setEnabled(true);
  }

  if (m_initializing)
	return;
  
  QPen pen;
  foreach(LineSymbolCurve* curve, m_curvesList){
	pen=curve->symbolsPen();
	pen.setStyle(penStyle);
	curve->setSymbolsPen(pen);
  }
}

void LineSymbolCurveDock::symbolBorderColorChanged(const QColor& color){
  if (m_initializing)
	return;
  
  QPen pen;
  foreach(LineSymbolCurve* curve, m_curvesList){
	pen=curve->symbolsPen();
	pen.setColor(color);
	curve->setSymbolsPen(pen);
  }  
  
  this->updatePenStyles(ui.cbSymbolBorderStyle, color);
}

void LineSymbolCurveDock::symbolBorderWidthChanged(int value){
  if (m_initializing)
	return;
  
  QPen pen;
  foreach(LineSymbolCurve* curve, m_curvesList){
	pen=curve->symbolsPen();
	pen.setWidth(value);
	curve->setSymbolsPen(pen);
  }
}

//Values-tab
void LineSymbolCurveDock::valuesTypeChanged(int index){
  if (m_initializing)
	return;

  LineSymbolCurve::ValuesType valuesType = LineSymbolCurve::ValuesType(index);
  
  if (valuesType==LineSymbolCurve::NoValues){
	ui.cbValuesPosition->setEnabled(false);
	ui.lValuesColumn->hide();
	cbValuesColumn->hide();
	ui.sbValuesDistance->setEnabled(false);
	ui.sbValuesRotation->setEnabled(false);	
	ui.sbValuesOpacity->setEnabled(false);
	ui.cbValuesColumnFormat->setEnabled(false);
	ui.cbValuesColumnFormat->setEnabled(false);
	//TODO precision/datetime format
	ui.leValuesPrefix->setEnabled(false);
	ui.leValuesSuffix->setEnabled(false);
	ui.kfrValuesFont->setEnabled(false);
	ui.kcbValuesFontColor->setEnabled(false);
  }else{
	ui.cbValuesPosition->setEnabled(true);
	ui.sbValuesDistance->setEnabled(true);
	ui.sbValuesRotation->setEnabled(true);	
	ui.sbValuesOpacity->setEnabled(true);
	ui.cbValuesColumnFormat->setEnabled(true);
	ui.cbValuesColumnFormat->setEnabled(true);
	//TODO precision/datetime format
	ui.leValuesPrefix->setEnabled(true);
	ui.leValuesSuffix->setEnabled(true);
	ui.kfrValuesFont->setEnabled(true);
	ui.kcbValuesFontColor->setEnabled(true);	
	
	if (valuesType==LineSymbolCurve::ValuesCustomColumn){
	  ui.lValuesColumn->show();
	  cbValuesColumn->show();
	}else{
	  ui.lValuesColumn->hide();
	  cbValuesColumn->hide();
	}
  }
	
  foreach(LineSymbolCurve* curve, m_curvesList){
	curve->setValuesType(valuesType);
  }
}

void LineSymbolCurveDock::valuesPositionChanged(int index){
  if (m_initializing)
	return;
	
  foreach(LineSymbolCurve* curve, m_curvesList){
	curve->setValuesPosition(LineSymbolCurve::ValuesPosition(index));
  }
}

void LineSymbolCurveDock::valuesDistanceChanged(int value){
  if (m_initializing)
	return;
		
  foreach(LineSymbolCurve* curve, m_curvesList){
	curve->setValuesDistance(value);
  }
}

void LineSymbolCurveDock::valuesRotationChanged(int value){
  if (m_initializing)
	return;
		
  foreach(LineSymbolCurve* curve, m_curvesList){
	curve->setValuesRotationAngle(value);
  }
}

void LineSymbolCurveDock::valuesOpacityChanged(int value){
  if (m_initializing)
	return;
		
  qreal opacity = (float)value/100;
  foreach(LineSymbolCurve* curve, m_curvesList){
	curve->setValuesOpacity(opacity);
  }
}

void LineSymbolCurveDock::valuesPrefixChanged(){
  if (m_initializing)
	return;
		
  QString prefix = ui.leValuesPrefix->text();
  foreach(LineSymbolCurve* curve, m_curvesList){
	curve->setValuesPrefix(prefix);
  }
}

void LineSymbolCurveDock::valuesSuffixChanged(){
  if (m_initializing)
	return;
		
  QString suffix = ui.leValuesSuffix->text();
  foreach(LineSymbolCurve* curve, m_curvesList){
	curve->setValuesSuffix(suffix);
  }
}

void LineSymbolCurveDock::valuesFontChanged(const QFont& font){
  if (m_initializing)
	return;
	
  foreach(LineSymbolCurve* curve, m_curvesList){
	curve->setValuesFont(font);
  }
}

void LineSymbolCurveDock::valuesFontColorChanged(const QColor& color){
  if (m_initializing)
	return;
  
  QPen pen;
  foreach(LineSymbolCurve* curve, m_curvesList){
	pen=curve->valuesPen();
	pen.setColor(color);
	curve->setValuesPen(pen);
  }  
}